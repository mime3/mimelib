#pragma once
#include "pch.h"
#include "LoginServer.h"

LoginServer::LoginServer(WCHAR * processName)
{
	StringCchCopy(_processName, 128, processName);
}

void LoginServer::Start()
{
	_MonitorClient = new LoginMonitorClient("MonitorLanClient.cnf", _processName, 1);
	InitializeSRWLock(&_mapLock);

	_LoginNetServer._lanServer = &_LoginLanServer;
	_LoginLanServer._netServer = &_LoginNetServer;

	_LoginNetServer._mapLock = &_mapLock;
	_LoginNetServer._loginData = &_loginData;
	_LoginNetServer._memoryPoolLogin = &_memoryPoolLogin;
	_LoginLanServer._mapLock = &_mapLock;
	_LoginLanServer._loginData = &_loginData;
	_LoginLanServer._memoryPoolLogin = &_memoryPoolLogin;

	_LoginLanServer.Start();
	_LoginNetServer.Start();	

	_MonitorClient->_lanPointer = &_LoginLanServer;
	_MonitorClient->_netPointer = &_LoginNetServer;
	_MonitorClient->Start();
}

void LoginServer::Stop()
{
	_LoginLanServer.Stop();
	_LoginNetServer.Stop();
	//_MonitorLanClient->Stop();
	//delete _MonitorLanClient;
}

void LoginServer::PrintInfo()
{
	printf_s("[LOGIN SERVER]\n");
	printf_s("----------------------------------------------------\n");

	printf_s("[NET]Session Count = %d\n", _LoginNetServer.GetClientCount());
	printf_s("[LOGIN]Packet Alloc Count = %d\n", StreamBuffer::_allocCount);

	printf_s("[LAN]Server List Count = %I64d\n", _LoginLanServer._serverList.size());

	printf_s("[LOGIN]Login ING Count = %I64d\n", _loginData.size());
	printf_s("[LOGIN]Login Struct Alloc Count = %I64d\n", _memoryPoolLogin._allocCount);
	printf_s("[NET]Login TPS = %d\n", _LoginNetServer._loginTPS);

	printf_s("[NET]Accept Count = %I64d\n", _LoginNetServer._acceptCount);
	printf_s("[NET]Accept TPS = %d\n", _LoginNetServer._acceptTPS);
			  
	printf_s("----------------------------------------------------\n");

	InterlockedAdd64((LONG64 *)&_LoginNetServer._loginTPS, -_LoginNetServer._loginTPS);
	InterlockedAdd((LONG*)&_LoginNetServer._acceptTPS, -_LoginNetServer._acceptTPS);

}
