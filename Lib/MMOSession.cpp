//#include "pch.h"
#include "MMOSession.h"

namespace MinLib
{
	inline BYTE MMOSession::GetCheckSum(char* buffer, int size)
	{
		BYTE checkSum = 0;
		while (size--)
		{
			checkSum += *(buffer++);
		}
		return checkSum;
	}

	inline void MMOSession::XOR(char* buffer, int size, char key)
	{
		while (size--)
		{
			*(buffer++) ^= key;
		}
	}

	inline void MMOSession::Encode(StreamBuffer* packet)
	{
		PACKET_HEADER* bufferPointer = (PACKET_HEADER*)packet->GetBuffer();

		//XOR((char *)&bufferPointer->checkSum, bufferPointer->len + 1, bufferPointer->randXORKey);
		XOR((char*)&bufferPointer->checkSum, bufferPointer->len + 1, bufferPointer->randXORKey ^ XORKey1_ ^ XORKey2_);
		//XOR((char *)&bufferPointer->randXORKey, bufferPointer->len + 2, _XORKey1);
		//XOR((char *)&bufferPointer->randXORKey, bufferPointer->len + 2, _XORKey2);
		XOR((char*)&bufferPointer->randXORKey, 2, XORKey1_ ^ XORKey2_);
	}

	inline void MMOSession::PutHeader(StreamBuffer* packet)
	{
		//PACKET_HEADER header;
		//int headerSize = sizeof(PACKET_HEADER);

		//header.code = _packetCode;
		//header.len = packet->GetUseSize() - headerSize;
		//header.randXORKey = rand() % 256;
		//header.checkSum = GetCheckSum(packet->GetBuffer() + headerSize, header.len);

		//packet->FillHeader((char *)&header, headerSize);
		//Encode(packet);

		PACKET_HEADER* header = (PACKET_HEADER*)packet->GetBuffer();
		header->code = packetCode_;
		header->len = (WORD)(packet->GetUseSize() - sizeof(PACKET_HEADER));
		header->randXORKey = rand() % 256;
		header->checkSum = GetCheckSum(packet->GetBuffer() + sizeof(PACKET_HEADER), header->len);

		packet->headerFillFlag_ = true;

		Encode(packet);
	}

	MMOSession::MMOSession()
	{
	}


	MMOSession::~MMOSession()
	{
	}

	void MMOSession::Init(ACCEPT_INFO* acceptInfo, INT64 sessionID)
	{
		socket_ = acceptInfo->socket;
		wcscpy_s(IP_, acceptInfo->IP);
		port_ = acceptInfo->port;
		sessionID_ = sessionID;

		logOutFlag_ = false;
		authToContentsFlag_ = false;

		sendFlag_ = FALSE;

		InterlockedIncrement((LONG*)&ioCount_);
		mode_ = MODE::AUTH;
	}

	void MMOSession::Clean()
	{
		//ZeroMemory(this, sizeof(MMOSession));
		// 보내기전 큐에 남은거 정리
		int count = (int)sendQueue_.GetUseCount();
		while (count--)
		{
			StreamBuffer* packet = nullptr;
			sendQueue_.DeQueue(&packet);
			PacketFree(packet);
		}

		// 보내기후 큐에 남은거 다 정리
		for (int i = 0; i < sendCount_; i++)
		{
			PacketFree(sendArray_[i]);
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
		count = (int)completeRecvQueue_.GetUseCount();
		while (count--)
		{
			StreamBuffer* packet = nullptr;
			completeRecvQueue_.DeQueue(&packet);
			PacketFree(packet);
		}

		recvQueue_.Clear();

		sendCount_ = 0;
		sendFlag_ = FALSE;
		socket_ = INVALID_SOCKET;
		mode_ = MODE::NONE;
	}

	bool MMOSession::SendPacket(StreamBuffer* packet)
	{
		if (mode_ != MODE::AUTH && mode_ != MODE::CONTENTS)
			return false;

		packet->AddRef();
		if (!packet->headerFillFlag_)
			PutHeader(packet);
		sendQueue_.EnQueue_UnSafe(packet);
		return true;
	}

	BYTE MMOSession::packetCode_ = 0;
	BYTE MMOSession::XORKey1_ = 0;
	BYTE MMOSession::XORKey2_ = 0;
}