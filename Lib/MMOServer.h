#pragma once
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#include <ws2tcpip.h>
#include "MMOSession.h"
#include "Logger.h"
#include "LockFreeStack.h"
#include "LockFreeQueue.h"
#include "MemoryPool.h"
#include "Parser.h"
#include "CrashDump.h"
#include "ProFiler.h"

//constexpr auto ACCEPT_SLEEP = 10;
//constexpr auto SEND_SLEEP = 10;
//constexpr auto AUTH_SLEEP = 50;
//constexpr auto CONTENTS_SLEEP = 10;
//constexpr auto RELEASE_SLEEP = 50;
//
//constexpr auto MAX_AUTH_COUNT = 50;
//constexpr auto MAX_AUTH_TO_CONTENTS = 10;
constexpr auto MAX_SESSION = 20000;
constexpr auto MAX_THREAD = 16;
//constexpr auto AUTH_PACKET_PROCESS = 1;
//constexpr auto CONTENTS_PACKET_PROCESS = 50;

class MMOServer
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
private:
	Parser _configParser;
	SOCKET _listenSocket;
	LF_Stack<int> _indexStack;
	LF_Queue<ACCEPT_INFO *> _acceptQueue;
	MemoryPoolTLS<ACCEPT_INFO> _acceptInfoPool;
	HANDLE _iocp;
	HANDLE _acceptThread;
	HANDLE _workerThread[MAX_THREAD];
	HANDLE _authThread;
	HANDLE _sendThread[MAX_THREAD];
	HANDLE _contentsThread;
	HANDLE _releaseThread;
	MMOSession * _sessionArray[MAX_SESSION];

	BYTE	_packetCode;
	BYTE	_XORKey1;
	BYTE	_XORKey2;
	char	_bindIP[20];
	int		_serverPort;
	int		_workerThreadCount;
	int		_activeWorkerThreadCount;
	int		_sendThreadCount;
	int		_maxClient;
	int		_monitorNo;
	INT64	_clientSeed;

	// seed
	int		_sendThreadSeed = 0;

	// configºÎºÐ
	int ACCEPT_SLEEP = 10;
	int SEND_SLEEP = 10;
	int AUTH_SLEEP = 50;
	int CONTENTS_SLEEP = 10;
	int RELEASE_SLEEP = 50;
	
	int MAX_AUTH_COUNT = 50;
	int MAX_AUTH_TO_CONTENTS = 10;
	int AUTH_PACKET_PROCESS = 1;
	int CONTENTS_PACKET_PROCESS = 50;


	bool LoadConfig(const char * fileName);
	void InitListenSocket();
	void RecvProc(MMOSession * session);
	bool RecvPost(MMOSession * session);

	inline BYTE GetCheckSum(char * buffer, int size);
	inline void XOR(char * buffer, int size, char key);
	inline void Encode(StreamBuffer * packet);
	inline bool Decode(PACKET_HEADER * header, StreamBuffer * packet);

	static unsigned int WINAPI AcceptThreadMain(LPVOID param);
	static unsigned int WINAPI WorkerThreadMain(LPVOID param);
	static unsigned int WINAPI AuthThreadMain(LPVOID param);
	static unsigned int WINAPI SendThreadMain(LPVOID param);
	static unsigned int WINAPI ContentsThreadMain(LPVOID param);
	static unsigned int WINAPI ReleaseThreadMain(LPVOID param);

	virtual void OnAuth_Update() = 0;
	virtual void OnContents_Update() = 0;

protected:
	int GetServerPort() { return _serverPort; }

	void SetSessionArray(void * pointer, int index);

public:
	// debug Info
	int _acceptCount = 0;
	LONG _authFPS = 0;
	LONG _contentsFPS = 0;	
	LONG _authFPS_Display = 0;
	LONG _contentsFPS_Display = 0;
	LONG _sendTPS = 0;
	LONG _recvTPS = 0;
	LONG _acceptTPS = 0;
	LONG _noIndex = 0;
	LONG _TotalSession = 0;
	LONG _AuthSession = 0;
	LONG _ContentsSession = 0;


public:
	MMOServer();
	~MMOServer();

	void Start(const char * fileName);
	void Stop();
	void BroadCast(StreamBuffer * packet);
	void DisConnect(MMOSession * session);
	inline int GetMonitorNo() { return _monitorNo; }
	virtual void PrintInfo();
};

