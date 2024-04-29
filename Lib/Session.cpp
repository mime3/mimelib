#include "Session.h"

#include <strsafe.h>

namespace MinLib
{
	Session::Session(SOCKET socket, int64_t clientID)
	{
		Init(socket, clientID);
	}

	void Session::Init(SOCKET socket, int64_t clientID, WCHAR IP[16])
	{
		Clear();
		if (IP != nullptr)
		{
			StringCchCopy(this->IP_, 16, IP);
		}
		clientID_ = clientID;
		socket_ = socket;
		setOption();
		sendFlag_ = FALSE;

		InterlockedIncrement((LONG*)&ioCountUseFlag_.ioCount); // 멀티스레드 환경에서 생성되자마자 오인되서 종료될수 있음
		ioCountUseFlag_.useFlag = TRUE; // TRUE로 바꾸기전에 IOCount를 1로 올려야한다
	}

	void Session::Clear()
	{
		ZeroMemory(&recvOverLapped_, sizeof(OVERLAPPED));
		ZeroMemory(&sendOverLapped_, sizeof(OVERLAPPED));
		for (int i = 0; i < sendCount_; i++)
		{
			PacketFree(sendArray_[i]);
		}
		sendCount_ = 0;
		recvQueue_.Clear();
		disConnectFlag_ = false;
	}

	void Session::setOption()
	{
		linger ling;
		ling.l_onoff = 1;
		ling.l_linger = 0;
		setsockopt(socket_, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));

		// SendBuffer Size 0 설정
		//char opt = 0;
		//setsockopt(this->socket, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt));

		// Socket에 NoDelay 설정
		//char opt_val = true;
		//setsockopt(this->socket, IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(opt_val));

		// KeepAlive 설정
		//DWORD opt = 0;
		//setsockopt(this->socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&opt, sizeof(opt));

		// KeepAlive 상세 설정
		tcp_keepalive tcpkl;
		tcpkl.onoff = 1;
		tcpkl.keepalivetime = 30000;
		tcpkl.keepaliveinterval = 2000;

		DWORD dwReturnByte;
		WSAIoctl(socket_, SIO_KEEPALIVE_VALS, &tcpkl, sizeof(tcp_keepalive),
			0, 0, &dwReturnByte, NULL, NULL);
	}
}