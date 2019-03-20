#include "pch.h"
#include "MMOSession.h"

inline BYTE MMOSession::GetCheckSum(char * buffer, int size)
{
	BYTE checkSum = 0;
	while (size--)
	{
		checkSum += *(buffer++);
	}
	return checkSum;
}

inline void MMOSession::XOR(char * buffer, int size, char key)
{
	while (size--)
	{
		*(buffer++) ^= key;
	}
}

inline void MMOSession::Encode(StreamBuffer * packet)
{
	PACKET_HEADER * bufferPointer = (PACKET_HEADER *)packet->GetBuffer();

	//XOR((char *)&bufferPointer->checkSum, bufferPointer->len + 1, bufferPointer->randXORKey);
	XOR((char *)&bufferPointer->checkSum, bufferPointer->len + 1, bufferPointer->randXORKey ^ _XORKey1 ^ _XORKey2);
	//XOR((char *)&bufferPointer->randXORKey, bufferPointer->len + 2, _XORKey1);
	//XOR((char *)&bufferPointer->randXORKey, bufferPointer->len + 2, _XORKey2);
	XOR((char *)&bufferPointer->randXORKey, 2, _XORKey1 ^ _XORKey2);
}

inline void MMOSession::PutHeader(StreamBuffer * packet)
{
	//PACKET_HEADER header;
	//int headerSize = sizeof(PACKET_HEADER);

	//header.code = _packetCode;
	//header.len = packet->GetUseSize() - headerSize;
	//header.randXORKey = rand() % 256;
	//header.checkSum = GetCheckSum(packet->GetBuffer() + headerSize, header.len);

	//packet->FillHeader((char *)&header, headerSize);
	//Encode(packet);

	PACKET_HEADER * header = (PACKET_HEADER *)packet->GetBuffer();
	header->code = _packetCode;
	header->len = packet->GetUseSize() - sizeof(PACKET_HEADER);
	header->randXORKey = rand() % 256;
	header->checkSum = GetCheckSum(packet->GetBuffer() + sizeof(PACKET_HEADER), header->len);
	
	packet->_headerFillFlag = true;

	Encode(packet);
}

MMOSession::MMOSession()
{
}


MMOSession::~MMOSession()
{
}

void MMOSession::Init(ACCEPT_INFO * acceptInfo, INT64 sessionID)
{
	_socket = acceptInfo->socket;
	wcscpy_s(_IP, acceptInfo->IP);
	_port = acceptInfo->port;
	_sessionID = sessionID;

	_logOutFlag = false;
	_authToContentsFlag = false;

	_sendFlag = FALSE;

	InterlockedIncrement((LONG*)&_ioCount);
	_mode = AUTH;
}

void MMOSession::Clean()
{
	//ZeroMemory(this, sizeof(MMOSession));
	// 보내기전 큐에 남은거 정리
	int count = (int)_sendQueue.GetUseCount();
	while (count--)
	{
		StreamBuffer * packet = nullptr;
		_sendQueue.DeQueue(&packet);
		PacketFree(packet);
	}

	// 보내기후 큐에 남은거 다 정리
	for (int i = 0; i < _sendCount; i++)
	{
		PacketFree(_sendArray[i]);
	}

	// 수신된 완성패킷 큐 정리
	//count = (int)_completeRecvQueue.size();
	//while (count--)
	//{
	//	StreamBuffer * packet = nullptr;
	//	packet = _completeRecvQueue.front();
	//	_completeRecvQueue.pop();
	//	PacketFree(packet);
	//}
	count = (int)_completeRecvQueue.GetUseCount();
	while (count--)
	{
		StreamBuffer * packet = nullptr;
		_completeRecvQueue.DeQueue(&packet);
		PacketFree(packet);
	}

	_recvQueue.Clear();

	_sendCount = 0;
	_sendFlag = FALSE;
	_socket = INVALID_SOCKET;
	_mode = NONE;
}

bool MMOSession::SendPacket(StreamBuffer * packet)
{
	if (_mode != AUTH && _mode != CONTENTS)
		return false;

	packet->AddRef();
	if (!packet->_headerFillFlag)
		PutHeader(packet);
	_sendQueue.EnQueue_UnSafe(packet);
	return true;
}

BYTE MMOSession::_packetCode = 0;
BYTE MMOSession::_XORKey1 = 0;
BYTE MMOSession::_XORKey2 = 0;