#include "pch.h"
#include "ChatServer.h"

ChatServer::ChatServer(WCHAR * processName)
{
	StringCchCopy(_processName, 128, processName);
}

void ChatServer::Start()
{
	_lanClient = new ChatLanClient("LanClient.cnf");
	_netServer = new ChatNetServer;
	_monitorClient = new ChatMonitorClient("MonitorLanClient.cnf", _processName, 2);
	
	_monitorClient->_netPointer = _netServer;

	InitializeSRWLock(&_keyMapLock);
	_lanClient->_keyMapLock = &_keyMapLock;
	_lanClient->_keyMap = &_keyMap;
	_netServer->_keyMapLock = &_keyMapLock;
	_netServer->_keyMap = &_keyMap;
			  
	_lanClient->Start();
	_netServer->Start();
	_monitorClient->Start();
	_monitorClient->SetMasterMode();
}

void ChatServer::Stop()
{
	_lanClient->Stop();
	_netServer->Stop();
	
	// 남아있는 key 삭제
	AcquireSRWLockExclusive(&_keyMapLock);
	auto iter = _keyMap.begin();
	auto iterEnd = _keyMap.end();
	for (; iter != iterEnd; ++iter)
	{
		_keyMap.erase(iter);
	}
	ReleaseSRWLockExclusive(&_keyMapLock);
}
