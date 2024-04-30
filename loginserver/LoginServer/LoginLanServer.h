#pragma once
#include <unordered_map>
#include "LanServer.h"

struct LOGIN_DATA;

struct SERVER
{
	BYTE	ServerType;		// dfSERVER_TYPE_GAME / dfSERVER_TYPE_CHAT
	WCHAR	ServerName[32];		// 해당 서버의 이름.  
};

class LoginLanServer : public LanServer
{
	LoginNetServer *						_netServer;

	MemoryPoolTLS<LOGIN_DATA> *				_memoryPoolLogin;
	SRWLOCK	*								_mapLock;
	unordered_map<INT64, LOGIN_DATA *> *	_loginData;

	SRWLOCK									_serverListLock;
	unordered_map<INT64, SERVER *>			_serverList;

	void PacketParse(INT64 clientID, StreamBuffer * packet);
	void LoginServer_Login(INT64 clientID, StreamBuffer * packet);
	void New_Client_Login(StreamBuffer * packet);

	bool BroadCastServer(StreamBuffer * packet);
	

public:
	LoginLanServer();
	// LanClient을(를) 통해 상속됨
	virtual void OnClientJoin(INT64 ClientID, Session * session) override;
	virtual void OnClientLeave(INT64 ClientID) override;
	virtual bool OnConnectionRequest(WCHAR * ClientIP, int Port) override;
	virtual void OnRecv(INT64 clientID, StreamBuffer * packet) override;
	virtual void OnSend(INT64 clientID, int sendSize) override;
	virtual void OnError(int errorCode, const WCHAR * errorMessage) override;
	friend class LoginServer;
	friend class LoginNetServer;
};