#pragma once
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include "Parser.h"
#include "struct.h"
#include "LockFreeStack.h"
#include "LockFreeQueue.h"
#include "StreamBuffer.h"
#include "Clock.h"
#include "Logger.h"


#define MAX_THREADCOUNT 16
#define LAN_MAX_SESSION	200


//////////////////////////////////////////////////////////////
// LanServer :
// 설명 : LAN 서버 클래스
//////////////////////////////////////////////////////////////
class LanServer
{
public:
#pragma pack(push, 1)   
	struct PACKET_HEADER
	{
		WORD len;
	};
#pragma pack(pop)   
	enum SERVER_STATUS
	{
		SERVER_OFF,
		SERVER_ON
	};
private:
	Parser			_configParser;						// 컨피그 파일 파서
	Session			_sessionArray[LAN_MAX_SESSION];			// 세션을 담을 배열
	LF_Stack<int>	_indexStack;						// 현재 사용중이지 않은 배열 인덱스를 담은 스택
	HANDLE			_iocp;								// iocp 핸들
	HANDLE			_acceptThread;						// accept스레드 핸들
	HANDLE			_workerThread[MAX_THREADCOUNT];		// worker스레드 핸들 배열
	SOCKET			_listenSocket;						// listen 소켓
	SERVER_STATUS	_status;							// 서버 on off 상태 멤버
	int				_clientMAX;
	int				_monitorNo;
	int				_workerThreadCount;						// 실행된 worker스레드 수
	INT64			_clientSeed;						// 접속하는 client에게 줄 seed 값
	char			_bindIP[20];
	int				_serverPort;							// 서버 포트번호
	BYTE			_packetCode;
	char			_XORKey1;
	char			_XORKey2;

	

	
	void RecvProc(Session * session);			// recv 프로시저
	void RecvPost(Session * session);			// recv 등록
	bool SendPost(Session * session);			// send 등록

	void TryDisConnect(Session * session);		// 내부의 session 종료시도
	bool GetBlankIndex(int * index);			// 스택에서 인덱스 가져옴
	int FindIndex(INT64 clientID);			// clientID를 인덱스로 변환
	void PutHeader(StreamBuffer * packet);		// 헤더 붙이기
	//BYTE GetCheckSum(char * buffer, int size);
	//void XOR(char * buffer, int size, char key);// XOR
	//void Encode(StreamBuffer * packet);			// 암호화
	//bool Decode(PACKET_HEADER * header, StreamBuffer * packet);			// 복호화

	static unsigned int WINAPI WorkerThreadMain(LPVOID lpParam); // worker스레드 메인
	static unsigned int WINAPI AcceptThreadMain(LPVOID lpParam); // connect스레드 메인
	
protected:
public:
	//디버그 전용
	INT64			_acceptCount;
	int				_acceptTPS;
	ULONGLONG		_acceptTPSTime;

	LanServer();
	virtual ~LanServer();

	bool LoadConfig();
	bool Start(bool nagle = true); //오픈 IP / 포트 / 워커스레드 수 / 나글옵션 / 최대접속자 수
	void Stop();											// 서버 종료
	void GetIP(INT64 clientID ,WCHAR * ipBuffer, int size = 16);// IP가져오기
	int GetClientCount();									// client 수 리턴

	bool Disconnect(INT64 clientID); // SESSION_ID		// 외부에서 DisConnect요청
	bool SendPacket(INT64 clientID, StreamBuffer * packet); // SESSION_ID

	virtual void OnClientJoin(INT64 ClientID, Session * session) = 0;  // < Accept 후 접속처리 완료 후 호출.
	virtual void OnClientLeave(INT64 ClientID) = 0;   	           // < Disconnect 후 호출
	virtual bool OnConnectionRequest(WCHAR * ClientIP, int Port) = 0;        //< accept 직후

	virtual void OnRecv(INT64 clientID, StreamBuffer * packet) = 0;             // < 패킷 수신 완료 후
	virtual void OnSend(INT64 clientID, int sendSize) = 0;          // < 패킷 송신 완료 후

	//virtual void OnWorkerThreadBegin() = 0;                   // < 워커스레드 GQCS 바로 하단에서 호출
	//virtual void OnWorkerThreadEnd() = 0;                     // < 워커스레드 1루프 종료 후

	virtual void OnError(int errorCode, const WCHAR * errorMessage) = 0;
	
};

