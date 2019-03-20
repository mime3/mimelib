#pragma once
#include "MonitorLanClient.h"
#include "ChatNetServer.h"
#include "CommonProtocol.h"

class ChatMonitorClient : public MonitorLanClient
{
	ChatNetServer * _netPointer;

public:
	ChatMonitorClient(const char * configFile, WCHAR * processName, int ID);
	// MonitorLanClient을(를) 통해 상속됨
	virtual void OnUpdate() override;

	friend class ChatServer;
};

