#pragma once
#include <unordered_map>
#include "LanClient.h"
#include "SessionKey.h"

class ChatLanClient : public LanClient
{
public:
	__int64 _serverID;
private:
	SRWLOCK *							_keyMapLock;
	unordered_map<INT64, SESSION_KEY> * _keyMap;

	void REQ_New_Client_Login(__int64 clientID, StreamBuffer * packet);
	void PacketParse(__int64 clientID, StreamBuffer * packet);

	void MakePacket_Loginserver_Login(StreamBuffer * packet);
	void MakePacket_RES_New_Client_Login(StreamBuffer * packet, INT64 accountNo, INT64 parameter);

	// LanClient을(를) 통해 상속됨
	virtual void OnClientJoin(__int64 ClientID, Session * session) override;
	virtual void OnClientLeave(__int64 ClientID) override;
	virtual bool OnConnectionRequest(char * ClientIP, int Port) override;
	virtual void OnRecv(__int64 clientID, StreamBuffer * packet) override;
	virtual void OnSend(__int64 clientID, int sendSize) override;
	virtual void OnError(int errorCode, const WCHAR * errorMessage) override;
	virtual void OnConnect(__int64 ClientID) override;

public:
	ChatLanClient(const char * config);

	friend class ChatServer;
};