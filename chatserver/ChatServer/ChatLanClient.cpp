#include "pch.h"
#include "ChatLanClient.h"

void ChatLanClient::REQ_New_Client_Login(__int64 clientID, StreamBuffer * packet)
{
	INT64 accountNo;
	SESSION_KEY sessionKey;
	INT64 parameter;

	*packet >> accountNo;
	packet->Out(sessionKey.key, 64);
	*packet >> parameter;

	// accountNo, KEY pair를 저장
	AcquireSRWLockExclusive(_keyMapLock);
	if (!_keyMap->insert(pair<INT64, SESSION_KEY>(accountNo, sessionKey)).second)
		(*_keyMap)[accountNo] = sessionKey;
	ReleaseSRWLockExclusive(_keyMapLock);

	StreamBuffer * lanPacket = PacketAlloc(LanClient);
	MakePacket_RES_New_Client_Login(lanPacket, accountNo, parameter);

	SendPacket(_serverID, lanPacket);
	PacketFree(lanPacket);
}

void ChatLanClient::PacketParse(__int64 clientID, StreamBuffer * packet)
{
	WORD type;
	*packet >> type;
	switch (type)
	{
	case en_PACKET_SS_REQ_NEW_CLIENT_LOGIN:
		REQ_New_Client_Login(clientID, packet);
		break;
	default:
		OnError(0, L"type error");
		break;
	}
}

void ChatLanClient::MakePacket_Loginserver_Login(StreamBuffer * packet)
{
	*packet << (WORD)en_PACKET_SS_LOGINSERVER_LOGIN << (BYTE)dfSERVER_TYPE_CHAT;
	WCHAR serverName[32];
	swprintf_s(serverName, 32, L"채팅서버임");
	packet->In(serverName, 32);
}

void ChatLanClient::MakePacket_RES_New_Client_Login(StreamBuffer * packet, INT64 accountNo, INT64 parameter)
{
	*packet << (WORD)en_PACKET_SS_RES_NEW_CLIENT_LOGIN << accountNo << parameter;
}

void ChatLanClient::OnClientJoin(__int64 ClientID, Session * session)
{
}

void ChatLanClient::OnClientLeave(__int64 ClientID)
{
	// 스레드키고 재연결
	if (_serverID == ClientID)
	{
		_serverID = 0;
		Reconnect();
	}
}

bool ChatLanClient::OnConnectionRequest(char * ClientIP, int Port)
{
	return false;
}

void ChatLanClient::OnRecv(__int64 clientID, StreamBuffer * packet)
{
	PacketParse(clientID, packet);
}

void ChatLanClient::OnSend(__int64 clientID, int sendSize)
{
}

void ChatLanClient::OnError(int errorCode, const WCHAR * errorMessage)
{
	LOG(L"Chat LAN", LOG_ERROR, L"code %d , %s", errorMessage);
}

void ChatLanClient::OnConnect(__int64 ClientID)
{
	// 저장
	_serverID = ClientID;

	StreamBuffer * loginPacket = PacketAlloc(LanClient);
	MakePacket_Loginserver_Login(loginPacket);
	SendPacket(ClientID, loginPacket);
	PacketFree(loginPacket);
}

ChatLanClient::ChatLanClient(const char * config) : LanClient(config)
{
}
