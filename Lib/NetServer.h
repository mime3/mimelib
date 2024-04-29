﻿#ifndef __MINLIB_NET_SERVER__
#define	__MINLIB_NET_SERVER__
#pragma once
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include "Parser.h"
#include "Session.h"
#include "LockFreeStack.h"
#include "LockFreeQueue.h"
#include "StreamBuffer.h"
#include "Clock.h"
#include "Logger.h"


#define MAXTHREADCOUNT 16
#define MAXSESSION	20000


//////////////////////////////////////////////////////////////
// NetServer :
// 설명 : 서버 클래스
//////////////////////////////////////////////////////////////
namespace MinLib
{
	class NetServer
	{
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
		enum SERVER_STATUS
		{
			SERVER_OFF,
			SERVER_ON
		};
	private:
		Parser			_configParser;						// 컨피그 파일 파서
		Session			_sessionArray[MAXSESSION];			// 세션을 담을 배열
		LF_Stack<int>	_indexStack;						// 현재 사용중이지 않은 배열 인덱스를 담은 스택
		HANDLE			_iocp;								// iocp 핸들
		HANDLE			_acceptThread;						// accept스레드 핸들
		HANDLE			_workerThread[MAXTHREADCOUNT];		// worker스레드 핸들 배열
		SOCKET			_listenSocket;						// listen 소켓
		SERVER_STATUS	_status;							// 서버 on off 상태 멤버
		int				_clientMAX;
		int				_monitorNo;
		int				_workerThreadCount;					// 실행된 worker스레드 수
		int				_activeWorkerThreadCount;			// 동시에 실행될 iocp스레드 수
		int64_t			_clientSeed;						// 접속하는 client에게 줄 seed 값
		char			_bindIP[20];
		int				_serverPort;						// 서버 포트번호
		BYTE			_packetCode;
		char			_XORKey1;
		char			_XORKey2;




		void RecvProc(Session* session);			// recv 프로시저
		void RecvPost(Session* session);			// recv 등록
		bool SendPost(Session* session);			// send 등록

		void TryDisConnect(Session* session);		// 내부의 session 종료시도
		bool GetBlankIndex(int* index);			// 스택에서 인덱스 가져옴
		int FindIndex(int64_t clientID);				// clientID를 인덱스로 변환
		void PutHeader(StreamBuffer* packet);		// 헤더 붙이기
		BYTE GetCheckSum(char* buffer, int size);
		void XOR(char* buffer, int size, char key);// XOR
		void Encode(StreamBuffer* packet);			// 암호화
		bool Decode(PACKET_HEADER* header, StreamBuffer* packet);			// 복호화

		static unsigned int WINAPI WorkerThreadMain(LPVOID lpParam); // worker스레드 메인
		static unsigned int WINAPI AcceptThreadMain(LPVOID lpParam); // accept스레드 메인

	protected:
	public:
		//디버그 전용
		int64_t			_acceptCount;
		int				_acceptTPS;
		ULONGLONG		_acceptTPSTime;

		NetServer();
		virtual ~NetServer();

		bool LoadConfig();
		bool Start(bool nagle = true); //오픈 IP / 포트 / 워커스레드 수 / 나글옵션 / 최대접속자 수
		void Stop();											// 서버 종료
		void GetIP(int64_t clientID, WCHAR* ipBuffer, int size = 16); // IP가져오기
		int GetClientCount();									// client 수 리턴

		bool Disconnect(int64_t clientID); // SESSION_ID		// 외부에서 DisConnect요청
		bool SendPacket(int64_t clientID, StreamBuffer* packet, bool isWorker = false); // SESSION_ID
		bool SendDisConnect(int64_t clientID, StreamBuffer* packet);

		virtual void OnClientJoin(int64_t ClientID, Session* session) = 0;  // < Accept 후 접속처리 완료 후 호출.
		virtual void OnClientLeave(int64_t ClientID) = 0;   	           // < Disconnect 후 호출
		virtual bool OnConnectionRequest(WCHAR* ClientIP, int Port) = 0;        //< accept 직후

		virtual void OnRecv(int64_t clientID, StreamBuffer* packet) = 0;             // < 패킷 수신 완료 후
		virtual void OnSend(int64_t clientID, int sendSize) = 0;          // < 패킷 송신 완료 후

		//virtual void OnWorkerThreadBegin() = 0;                   // < 워커스레드 GQCS 바로 하단에서 호출
		//virtual void OnWorkerThreadEnd() = 0;                     // < 워커스레드 1루프 종료 후

		virtual void OnError(int errorCode, const WCHAR* errorMessage) = 0;


	};
}
#endif // !__MINLIB_NET_SERVER__