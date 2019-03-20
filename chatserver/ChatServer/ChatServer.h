#pragma once
#include <unordered_map>
#include "ChatLanClient.h"
#include "ChatNetServer.h"
#include "ChatMonitorClient.h"
#include "SessionKey.h"

class ChatServer
{
	SRWLOCK								_keyMapLock;
	WCHAR								_processName[128];
public:
	ChatServer(WCHAR * processName);
	unordered_map<INT64, SESSION_KEY>	_keyMap;

	ChatLanClient * _lanClient;
	ChatNetServer * _netServer;
	ChatMonitorClient * _monitorClient;
	void Start();
	void Stop();
};