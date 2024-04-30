#pragma once
#include "pch.h"
#include "LoginNetServer.h"

extern CrashDump cd;

void LoginNetServer::Login(INT64 clientID, StreamBuffer * packet)
{
	INT64 accountNo;
	char sessionKey[64];

	*packet >> accountNo;
	packet->Out(sessionKey, 64);

	// accountNo 가 실제로 존재하는지 DB확인
	_DBConnectorTLS->Query(L"SELECT `accountNo` FROM `account` WHERE accountno = %I64d", accountNo);
	if (_DBConnectorTLS->FetchRow() == NULL)
	{
		// 바로 실패응답 보낸다
		StreamBuffer * resPacket = PacketAlloc(NetServer);
		MakePacket_New_Client_Login(resPacket, accountNo, dfLOGIN_STATUS_ACCOUNT_MISS);
		SendDisConnect(clientID, resPacket);
		PacketFree(resPacket);
		return;
	}
	_DBConnectorTLS->FreeResult();

	_DBConnectorTLS->Query(L"SELECT `accountNo` FROM `sessionkey` WHERE accountno = %I64d", accountNo);
	if (_DBConnectorTLS->FetchRow() == NULL)
	{
		// 바로 실패응답 보낸다
		StreamBuffer * resPacket = PacketAlloc(NetServer);
		MakePacket_New_Client_Login(resPacket, accountNo, dfLOGIN_STATUS_SESSION_MISS);
		SendDisConnect(clientID, resPacket);
		PacketFree(resPacket);
		return;
	}
	_DBConnectorTLS->FreeResult();

	_DBConnectorTLS->Query(L"SELECT `status` FROM `status` WHERE accountno = %I64d", accountNo);
	MYSQL_ROW statusRow = _DBConnectorTLS->FetchRow();
	if (statusRow == NULL)
	{
		// 바로 실패응답 보낸다
		StreamBuffer * resPacket = PacketAlloc(NetServer);
		MakePacket_New_Client_Login(resPacket, accountNo, dfLOGIN_STATUS_STATUS_MISS);
		SendDisConnect(clientID, resPacket);
		PacketFree(resPacket);
		return;
	}
	// 이미 접속중인 경우
	if (strcmp(statusRow[0], "0") != 0)
	{
		// 바로 실패응답 보낸다
		StreamBuffer * resPacket = PacketAlloc(NetServer);
		MakePacket_New_Client_Login(resPacket, accountNo, dfLOGIN_STATUS_GAME);
		SendDisConnect(clientID, resPacket);
		PacketFree(resPacket);
		return;
	}
	_DBConnectorTLS->FreeResult();

	// 테이블 확인 완료, map에 accountNo와 parameter저장
	LOGIN_DATA * loginData = _memoryPoolLogin->Alloc();
	loginData->accountNo = accountNo;
	loginData->status = 0;

	AcquireSRWLockExclusive(_mapLock);
	if (!_loginData->insert(pair<INT64, LOGIN_DATA *>(clientID, loginData)).second)
	{
		_memoryPoolLogin->Free((*_loginData)[clientID]);
		(*_loginData)[accountNo] = loginData;
	}
	ReleaseSRWLockExclusive(_mapLock);

	// LAN서버를 통해서 보내기
	StreamBuffer * lanPacket = PacketAlloc(LanServer);
	MakePacket_SS_REQ_New_Client_Login(lanPacket, accountNo, sessionKey, clientID);
	if (!_lanServer->BroadCastServer(lanPacket))
	{
		// 연결된 서버가 없어서 못보냈다
		// 실패 패킷을 보낸다
		StreamBuffer * resPacket = PacketAlloc(NetServer);
		MakePacket_New_Client_Login(resPacket, accountNo, dfLOGIN_STATUS_NOSERVER);
		SendDisConnect(clientID, resPacket);
		PacketFree(resPacket);

		//// 맵에 있는것도 빼버린다, 근데 안전한가?
		//if ((*_loginData)[clientID]->accountNo == accountNo)
		//{
		//	AcquireSRWLockExclusive(_mapLock);
		//	_loginData->erase(accountNo);
		//	ReleaseSRWLockExclusive(_mapLock);
		//}
	}
	PacketFree(lanPacket);
	InterlockedIncrement64((LONG64 *)&_loginTPS);
}

void LoginNetServer::PacketParse(INT64 clientID, StreamBuffer * packet)
{
	WORD type;
	*packet >> type;
	switch (type)
	{
	case en_PACKET_CS_LOGIN_REQ_LOGIN:
		Login(clientID, packet);
		break;
	default:
		OnError(1, L"LoginNetServer Packet Type Wrong");
		break;
	}
}

void LoginNetServer::MakePacket_New_Client_Login(StreamBuffer * packet, INT64 accountNo, BYTE status)
{
	*packet << (WORD)en_PACKET_CS_LOGIN_RES_LOGIN << accountNo << status;
	WCHAR ID[20] = { 0, };
	WCHAR Nickname[20] = { 0, };
	WCHAR GameServerIP[16] = { 0, };
	USHORT GameServerPort = 0;
	WCHAR	ChatServerIP[16] = { 0, };
	USHORT	ChatServerPort = 0;

	if (status == dfLOGIN_STATUS_OK)
	{
		// Lan서버의 접속 서버리스트 순회
		auto iter = _lanServer->_serverList.begin();
		auto iterEnd = _lanServer->_serverList.end();
		for (; iter != iterEnd; ++iter)
		{
			if (iter->second->ServerType == dfSERVER_TYPE_CHAT)
			{
				_lanServer->GetIP(iter->first, ChatServerIP);
				break;
			}
		}
		//swprintf_s(ChatServerIP, 16, L"127.0.0.1");
		ChatServerPort = 12001;
	}

	// 채우자
	packet->In(ID, 20);
	packet->In(Nickname, 20);
	packet->In(GameServerIP, 16);
	*packet << GameServerPort;
	packet->In(ChatServerIP, 16);
	*packet << ChatServerPort;
	
}

void LoginNetServer::MakePacket_New_Client_Login(StreamBuffer * packet, INT64 accountNo, BYTE status, WCHAR ID[20], WCHAR Nickname[20], WCHAR GameServerIP[16], USHORT GameServerPort, USHORT ChatServerIP[16], USHORT ChatServerPort)
{
}

void LoginNetServer::MakePacket_SS_REQ_New_Client_Login(StreamBuffer * packet, INT64 accountNo, char sessionKey[64], INT64 parameter)
{
	*packet << (WORD)en_PACKET_SS_REQ_NEW_CLIENT_LOGIN << accountNo;
	packet->In(sessionKey, 64);
	*packet << parameter;
}

LoginNetServer::LoginNetServer()
{
	_DBConnectorTLS = new DBConnectorTLS("DBConfig.cnf");
}


LoginNetServer::~LoginNetServer()
{
}

void LoginNetServer::OnClientJoin(INT64 ClientID, Session * session)
{
}

void LoginNetServer::OnClientLeave(INT64 ClientID)
{
	AcquireSRWLockExclusive(_mapLock);
	auto iter = _loginData->find(ClientID);
	if (iter != _loginData->end())
	{
		_memoryPoolLogin->Free(iter->second);
		_loginData->erase(ClientID);
	}
	ReleaseSRWLockExclusive(_mapLock);
}

bool LoginNetServer::OnConnectionRequest(WCHAR * ClientIP, int Port)
{
	return true;
}

void LoginNetServer::OnRecv(INT64 clientID, StreamBuffer * packet)
{
	PacketParse(clientID, packet);
}

void LoginNetServer::OnSend(INT64 clientID, int sendSize)
{
	//Disconnect(clientID);
	AcquireSRWLockExclusive(_mapLock);
	auto iter = _loginData->find(clientID);
	if (iter != _loginData->end())
	{
		//_DBConnectorTLS->Query_Save(L"UPDATE `status` SET `status` = %d WHERE `accountNo` = %I64d"
		//	,/*iter->second->status*/0, iter->second->accountNo);
	}
	ReleaseSRWLockExclusive(_mapLock);
}

void LoginNetServer::OnError(int errorCode, const WCHAR * errorMessage)
{
}

