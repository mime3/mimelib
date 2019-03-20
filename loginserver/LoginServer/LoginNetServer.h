#pragma once
#include <unordered_map>
#include "NetServer.h"
#include "DBConnector.h"

struct LOGIN_DATA;
class LoginLanServer;
class LoginNetServer : public NetServer
{
	LoginLanServer *						_lanServer;

	MemoryPoolTLS<LOGIN_DATA> *				_memoryPoolLogin;
	SRWLOCK	*								_mapLock;
	unordered_map<INT64, LOGIN_DATA *> *	_loginData;
	DBConnectorTLS *						_DBConnectorTLS;

	INT64									_loginTPS = 0;

	void PacketParse(INT64 clientID, StreamBuffer * packet);
	void Login(INT64 clientID, StreamBuffer * packet);

	void MakePacket_New_Client_Login(StreamBuffer * packet, INT64 accountNo, BYTE status);
	void MakePacket_New_Client_Login(StreamBuffer * packet, INT64 accountNo, BYTE status, WCHAR ID[20], WCHAR	Nickname[20]
		, WCHAR GameServerIP[16], USHORT GameServerPort, USHORT ChatServerIP[16], USHORT ChatServerPort);
	void MakePacket_SS_REQ_New_Client_Login(StreamBuffer * packet, INT64 accountNo, char sessionKey[64], INT64 parameter);

public:
	LoginNetServer();
	~LoginNetServer();

	// NetServer을(를) 통해 상속됨
	virtual void OnClientJoin(INT64 ClientID, Session * session) override;
	virtual void OnClientLeave(INT64 ClientID) override;
	virtual bool OnConnectionRequest(WCHAR * ClientIP, int Port) override;
	virtual void OnRecv(INT64 clientID, StreamBuffer * packet) override;
	virtual void OnSend(INT64 clientID, int sendSize) override;
	virtual void OnError(int errorCode, const WCHAR * errorMessage) override;
	friend class LoginServer;
	friend class LoginLanServer;
};

