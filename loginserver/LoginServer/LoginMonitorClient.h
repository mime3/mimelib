#pragma once
#include "MonitorLanClient.h"
#include "CommonProtocol.h"

class LoginMonitorClient : public MonitorLanClient
{
	LoginNetServer * _netPointer;
	LoginLanServer * _lanPointer;

	// MonitorLanClient을(를) 통해 상속됨
	virtual void OnUpdate() override;

public:
	LoginMonitorClient(const char * configFile, WCHAR * processName, int ID);
	friend class LoginServer;
};