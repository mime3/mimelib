#ifndef __MINLIB_MMO_SESSION__
#define __MINLIB_MMO_SESSION__
#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <queue>

#include "StreamBuffer.h"
#include "StreamQueue.h"
#include "LockFreeQueue.h"

namespace MinLib
{
	constexpr int mmoSendArraySize = 200;
	enum class MODE
	{
		NONE,
		AUTH,
		AUTH_TO_CONTENTS,
		CONTENTS,
		LOGOUT_IN_AUTH,
		LOGOUT_IN_CONTENTS,
		WAIT_LOGOUT
	};

	struct ACCEPT_INFO
	{
		SOCKET socket;
		WCHAR IP[16];
		int port;
	};

	class MMOSession
	{
	friend class MMOServer;
	public:
#pragma pack(push, 1)   
		struct PACKET_HEADER
		{
			BYTE code;
			WORD len;
			BYTE randXORKey;
			BYTE checkSum;
		};
#pragma pack(pop)
		static BYTE	packetCode_;
		static BYTE	XORKey1_;
		static BYTE	XORKey2_;

	private:
		SOCKET	socket_			= {};
		WCHAR	IP_[16]			= { 0, };
		int		port_			= { 0 };
		INT64	sessionID_		= { 0 };
		int		ioCount_		= { 0 };
		MODE	mode_			= { MODE::NONE };

		OVERLAPPED				sendOverLapped_ = {};			// sendoverlapped
		OVERLAPPED				recvOverLapped_ = {};			// recvoverlapped
		BOOL					sendFlag_ = { FALSE };			// 전송중 여부 플래그
		int						sendCount_ = { 0 };				// 전송한 패킷 수
		LF_Queue<StreamBuffer*> sendQueue_;
		StreamQueue				recvQueue_;
		StreamBuffer*			sendArray_[mmoSendArraySize];	// 전송한 패킷 저장배열
		//std::queue<StreamBuffer *> _completeRecvQueue;
		LF_Queue<StreamBuffer*> completeRecvQueue_;

		virtual void OnAuth_Packet(StreamBuffer* packet) = 0;
		virtual void OnAuth_ClientLeave(bool isExit) = 0;
		virtual void OnContents_Packet(StreamBuffer* packet) = 0;
		virtual void OnContents_ClientJoin() = 0;
		virtual void OnContents_ClientLeave() = 0;
		virtual void OnContents_ClientRelease() = 0;

	protected:
		bool logOutFlag_			= { false };
		bool authToContentsFlag_	= { false };

		inline BYTE GetCheckSum(char* buffer, int size);
		inline void XOR(char* buffer, int size, char key);
		inline void Encode(StreamBuffer* packet);
		inline void PutHeader(StreamBuffer* packet);

	public:
		MMOSession();
		~MMOSession();
		void Init(ACCEPT_INFO* acceptInfo, INT64 sessionID);
		void Clean();
		bool SendPacket(StreamBuffer* packet);
	};
}
#endif // !__MINLIB_MMO_SESSION__