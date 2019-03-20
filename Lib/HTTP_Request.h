#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;
class HTTP_Request
{
public:
	enum Method
	{
		GET,
		POST
	};
private:
	string _domain;
	Method _method;
	SOCKET _socket;
	SOCKADDR_IN _addr;
protected:
	BOOL DomainToIP(const WCHAR *domain, IN_ADDR *addr);
	void DeleteChunk(string * recvData, string * result);
public:
	HTTP_Request(Method method = POST);
	HTTP_Request(const WCHAR * url, int port ,Method method = POST);
	~HTTP_Request();

	bool SetURL(const WCHAR * url, int port);
	bool SendData(const CHAR sendPage[], string * sendData, int * resultCode, string * recvData);
	bool SendData(wstring * sendData, int resultCode, wstring * recvData);
};