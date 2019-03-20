#pragma once
//#include "MonitorLanClient.h"
//#include "CommonProtocol.h"
#include "LoginMonitorClient.h"

struct LOGIN_DATA
{
	INT64 accountNo;
	int status;
};

class LoginServer
{
	LoginNetServer			_LoginNetServer;
	LoginLanServer			_LoginLanServer;
	LoginMonitorClient *	_MonitorClient;

	WCHAR					_processName[128];

	MemoryPoolTLS<LOGIN_DATA>			_memoryPoolLogin;
	SRWLOCK								_mapLock;
	unordered_map<INT64, LOGIN_DATA *>	_loginData;
public:
	LoginServer(WCHAR * processName);
	void Start();
	void Stop();
	void PrintInfo();
};