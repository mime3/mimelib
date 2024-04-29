#ifndef __MINLIB_SESSION__
#define __MINLIB_SESSION__
#pragma once
//#include "boost/lockfree/queue.hpp"
#include <WinSock2.h>
#include <Mstcpip.h>

// winsock2.h를 include 한 이후 windows.h를 include해야함
#include "StreamBuffer.h"
#include "StreamQueue.h"
#include "LockFreeQueue.h"

#pragma comment(lib,"ws2_32.lib")
namespace MinLib
{
	//#define BOOST
	constexpr int sendArraySize = 200;

	struct UseFlag_IOCount
	{
		BOOL useFlag;
		int ioCount;
	};

	class Session
	{
	public:
		Session() = default;
		Session(SOCKET socket, int64_t clientID);
		~Session() = default;

		void Init(SOCKET socket, int64_t clientID, WCHAR IP[16] = nullptr);

		void Clear();
		void setOption();

	public:
		WCHAR IP_[16];
		SOCKET socket_;								// 소켓
		int64_t clientID_;							// 클라이언트 ID
#ifdef BOOST
		boost::lockfree::queue<StreamBuffer*> sendQueue;
#else
		LF_Queue<StreamBuffer*> sendQueue_;			// send할 패킷을 담을 큐
#endif
		StreamQueue recvQueue_;						// recv 링버퍼
		OVERLAPPED sendOverLapped_;					// sendoverlapped
		OVERLAPPED recvOverLapped_;					// recvoverlapped
		BOOL sendFlag_;								// 전송중 여부 플래그
		int sendCount_;								// 전송한 패킷 수
		LF_Queue<StreamBuffer*> sendQ_;				// send한 패킷을 잠시 담을 큐
		StreamBuffer* sendArray_[sendArraySize];
		UseFlag_IOCount ioCountUseFlag_;			// ioCount와 사용중 플래그
		bool disConnectFlag_;
	};
}
#endif // !__MINLIB_SESSION__