#pragma once
#include "pch.h"
#include "LoginLanServer.h"

// 다른 서버의 로그인서버 접속 처리
void LoginLanServer::LoginServer_Login(INT64 clientID, StreamBuffer * packet)
{
	SERVER * newServer = new SERVER;
	*packet >> newServer->ServerType;
	packet->Out((char *)newServer->ServerName, 64);

	AcquireSRWLockExclusive(&_serverListLock);
	_serverList.insert(pair<INT64, SERVER*>(clientID, newServer));
	ReleaseSRWLockExclusive(&_serverListLock);

	printf("server login\n");
}

void LoginLanServer::New_Client_Login(StreamBuffer * packet)
{
	// 유저에게 접속성공 알리기
	INT64 accountNo;
	INT64 parameter;
	*packet >> accountNo >> parameter;
	
	BYTE status = 0;

	// map에서 체크
	AcquireSRWLockExclusive(_mapLock);
	auto iter = _loginData->find(parameter);
	if(iter != _loginData->end())
	{
		if (iter->second->accountNo == accountNo)
		{
			status = dfLOGIN_STATUS_OK;
			iter->second->status = status;
			//_loginData->erase(iter); // 나가면 지운다
		}
	}
	ReleaseSRWLockExclusive(_mapLock);

	StreamBuffer * resPacket = PacketAlloc(NetServer);
	_netServer->MakePacket_New_Client_Login(resPacket, accountNo, status);
	_netServer->SendDisConnect(parameter, resPacket);
	PacketFree(resPacket);
}

bool LoginLanServer::BroadCastServer(StreamBuffer * packet)
{
	auto iter = _serverList.begin();
	auto iterEnd = _serverList.end();

	if (iter == iterEnd)
		return false;

	for (; iter != iterEnd; ++iter)
	{
		SendPacket(iter->first, packet);
	}
	return true;
}

// 패킷 파싱
void LoginLanServer::PacketParse(INT64 clientID, StreamBuffer * packet)
{
	WORD type;
	*packet >> type;
	switch (type)
	{
	case en_PACKET_SS_LOGINSERVER_LOGIN:
		LoginServer_Login(clientID, packet);
		break;
	case en_PACKET_SS_RES_NEW_CLIENT_LOGIN:
		New_Client_Login(packet);
		break;
	default:
		break;
	}
}

LoginLanServer::LoginLanServer()
{
	InitializeSRWLock(&_serverListLock);
}

void LoginLanServer::OnClientJoin(INT64 ClientID, Session * session)
{
}

void LoginLanServer::OnClientLeave(INT64 ClientID)
{
	AcquireSRWLockExclusive(&_serverListLock);
	_serverList.erase(ClientID);
	ReleaseSRWLockExclusive(&_serverListLock);
	printf("server down\n");
}

bool LoginLanServer::OnConnectionRequest(WCHAR * ClientIP, int Port)
{
	return true;
}

void LoginLanServer::OnRecv(INT64 clientID, StreamBuffer * packet)
{
	PacketParse(clientID, packet);
}

void LoginLanServer::OnSend(INT64 clientID, int sendSize)
{
}

void LoginLanServer::OnError(int errorCode, const WCHAR * errorMessage)
{
}
