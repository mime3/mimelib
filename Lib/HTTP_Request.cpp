#include "pch.h"
#include "HTTP_Request.h"

BOOL HTTP_Request::DomainToIP(const WCHAR *domain, IN_ADDR *addr)
{
	ADDRINFOW *addrInfo;
	SOCKADDR_IN *sockAddr;

	if (GetAddrInfo(domain, L"0", NULL, &addrInfo) != 0)
		return FALSE;

	char stringBuf[16];

	//while (1)
	//{
	//	sockAddr = (SOCKADDR_IN*)addrInfo->ai_addr;
	//	*addr = sockAddr->sin_addr;
	//	inet_ntop(AF_INET, addr, stringBuf, sizeof(stringBuf));
	//	//printf_s("%s\n", stringBuf, sizeof(stringBuf));

	//	if ((SOCKADDR_IN*)addrInfo->ai_next == nullptr)
	//		break;

	//	addrInfo = addrInfo->ai_next;
	//}
	sockAddr = (SOCKADDR_IN*)addrInfo->ai_addr;
	*addr = sockAddr->sin_addr;
	inet_ntop(AF_INET, addr, stringBuf, sizeof(stringBuf));
	
	_domain.append(stringBuf);

	FreeAddrInfo(addrInfo);
	return TRUE;
}

void HTTP_Request::DeleteChunk(string * origin, string * result)
{
	// 그냥 부스트 라이브러리 쓰자
	string::size_type idx, idx2;
	char * end;
	while (1)
	{
		idx = origin->find("\r\n");
		if (idx == string::npos)
			break;
		long len = strtol(origin->substr(0, idx).c_str(), &end, 16);
		if (len == 0)
			break;
		*result += origin->substr(idx + 2, len);
		*origin = origin->substr((idx + 2) + len + 2);
	}
}

HTTP_Request::HTTP_Request(Method method)
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2,2), &wsadata);
	_method = method;
}

HTTP_Request::HTTP_Request(const WCHAR * url, int port, Method method)
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	_method = method;
	SetURL(url, port);
}

HTTP_Request::~HTTP_Request()
{
	WSACleanup();
}

bool HTTP_Request::SetURL(const WCHAR * url, int port)
{
	_domain.clear();
	IN_ADDR addr;
	BOOL error = DomainToIP(url, &addr);
	if(error == FALSE)
		return false;

	ZeroMemory(&_addr, sizeof(_addr));
	//_addr.sin_addr = _domain.c_str();
	inet_pton(AF_INET, _domain.c_str(), &_addr.sin_addr.s_addr);
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	return true;
}

bool HTTP_Request::SendData(const CHAR sendPage[], string * sendData, int * resultCode, string * recvData)
{
	// non block
	_socket = socket(AF_INET, SOCK_STREAM, NULL);

	linger ling;
	ling.l_onoff = 1;
	ling.l_linger = 0;
	setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));

	//u_long nonBlock = 1;
	//ioctlsocket(_socket, FIONBIO, &nonBlock);

	int retval = connect(_socket, (SOCKADDR*)&_addr, sizeof(_addr));

	if (retval == SOCKET_ERROR)
	{
		DWORD error = GetLastError();
		if (error != WSAEWOULDBLOCK)
			return false;
	}

	FD_SET fwrite;
	FD_ZERO(&fwrite);
	FD_SET(_socket, &fwrite);
	timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	select(NULL, NULL, &fwrite, NULL, &tv);
	if (!FD_ISSET(_socket, &fwrite))
		return false;

	// block socket
	//nonBlock = 0;
	//ioctlsocket(_socket, FIONBIO, &nonBlock);

	int len = strlen(sendData->c_str());
	char buffer[512];
	sprintf_s(buffer, 
		"%s http://%s%s HTTP/1.1\r\nUser-Agent: C Client\r\nHost: %s\r\nConnection: Close\r\nContent-Length: %d\r\n\r\n", 
		(_method == GET)?"GET":"POST", _domain.c_str(), sendPage, _domain.c_str(), len);

	int sended = send(_socket, buffer, strlen(buffer), 0);
	sended += send(_socket, sendData->c_str(), len, 0);

	// 헤더 먼저 다받음
	string recvString;
	char recvBuffer[1024];
	while (1)
	{
		ZeroMemory(recvBuffer, 1024);
		int res = recv(_socket, recvBuffer, 1024, 0);

		if (res == SOCKET_ERROR)
		{
			int error = GetLastError();
			return false;
		}
		recvString += recvBuffer;

		if (recvString.find("\r\n\r\n") != string::npos)
			break;
	}

	// 헤더 파싱
	string::size_type idx, idx2, idx3;
	idx = recvString.find(0x20);
	idx2 = recvString.find(0x20, idx + 1);
	string codePart = recvString.substr(idx + 1, idx2 - idx);
	*resultCode = atoi(codePart.c_str());

	// 인코딩 방식에 따라서 나뉜다
	idx = recvString.find("Content-Length: ");
	if (idx == string::npos)
	{
		idx = recvString.find("Transfer-Encoding: chunked");
		if (idx == string::npos)
			return false;

		// 바디 시작부분 찾기
		idx = recvString.find("\r\n\r\n") + 4;
		while (1)
		{
			// 바디 끝 찾기
			idx2 = recvString.find("0\r\n\r\n");
			if (idx2 != string::npos)
				break;
			char recvBuffer[1024] = { 0, };
			recv(_socket, recvBuffer, 1024, 0);
			recvString += recvBuffer;
		}
		// 헤더 제거
		string temp = recvString.substr(idx, idx2 - idx + 5);
		DeleteChunk(&temp, recvData);
		//*recvData = recvString.substr(idx, idx2 - idx + 5);

		// chunk 데이터 없애야함
	}
	else
	{
		idx2 = recvString.find(/*0x0d*/" ", idx);
		idx3 = recvString.find("\r\n", idx2) - 1;
		string lenPart = recvString.substr(idx2 + 1, idx3 - idx2);
		int bodyLen = atoi(lenPart.c_str());
	
		// 바디 리시브 확인
		idx = recvString.find("\r\n\r\n") + 4;
		while (recvString.size() < bodyLen + idx)
		{
			char recvBuffer[1024];
			recv(_socket, recvBuffer, 1024, 0);
			recvString += recvBuffer;
		}

		*recvData += recvString.substr(idx);
	}

	closesocket(_socket);
	return true;
}

bool HTTP_Request::SendData(wstring * sendData, int resultCode, wstring * recvData)
{
	return false;
}
