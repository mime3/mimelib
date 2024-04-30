#pragma once
#include <map>
#include <unordered_map>
#include "Player.h"
#include "Sector.h"
#include "process.h"
//#include "CommonProtocol.h"
#include "ProFiler.h"
#include "SessionKey.h"
#include "DBConnector.h"

#define TIMER_INIT \
    LARGE_INTEGER frequency; \
    LARGE_INTEGER t1,t2; \
    double elapsedTime; \
    QueryPerformanceFrequency(&frequency);


/** Use to start the performance timer */
#define TIMER_START QueryPerformanceCounter(&t1);

/** Use to stop the performance timer and output the result to the standard stream. Less verbose than \c TIMER_STOP_VERBOSE */
#define TIMER_STOP \
    QueryPerformanceCounter(&t2); \
    elapsedTime=(float)(t2.QuadPart-t1.QuadPart)/frequency.QuadPart; \

using namespace std;

// 메시지 구조체
typedef struct MESSAGE
{
	// 타입
	enum MESSAGE_TYPE
	{
		JOIN,
		LEAVE,
		PACKET
	};
	MESSAGE_TYPE type;		// 타입
	__int64 sessionID;		// 세션 ID
	StreamBuffer * data;	// 패킷 포인터
}Message;

constexpr WORD SECTOR_X = 100;
constexpr WORD SECTOR_Y = 100;

class ChatNetServer : public NetServer
{
	SRWLOCK *							_keyMapLock;
	unordered_map<INT64, SESSION_KEY> * _keyMap;

public:
	MemoryPoolTLS<PLAYER>			_memoryPoolPlayer;		// 플레이어 메모리풀
	MemoryPoolTLS<MESSAGE>			_memoryPoolMessage;		// 메시지 메모리풀
	unordered_map<__int64, PLAYER *>_playerMap;				// 플레이어들을 저장할 MAP
	LF_Queue<MESSAGE *>				_messageQueue;			// 메시지를 담을 락프리 큐

	DBConnector						_dbConnector;			// chat저장용 DB커넥터

	int _LOGIN_PLAYER = 0;
	int _NOT_EXIST_SESSIONKEY = 0;
	int _WRONG_SESSIONKEY = 0;
	int _PACKET_TYPE_ERROR = 0;								// 디버그전용
	int _NOT_EXIST_PLAYER = 0;								// 디버그전용
	int _WRONG_MOVE = 0;									// 디버그전용
	int _WRONG_CHATLEN = 0;									// 디버그전용
	int _UPDATE_TPS = 0;									// 디버그전용
	ULONGLONG _UPDATE_TPS_TIME = GetTickCount64();									// 디버그전용

private:
	HANDLE					_wakeUpEvent;					// 스레드를 깨울 이벤트객체
	HANDLE					_updateThread;					// 컨텐츠 처리 스레드
	SECTOR					_sector[SECTOR_Y][SECTOR_X];	// 섹터관리 2중배열
	bool					_exitFlag;						// 종료 플래그 (미구현)
	
	static unsigned int WINAPI UpdateThreadMain(LPVOID lpParam);	// 업데이트 스레드 메인
	void PutMessageData(Message * message);							// 큐에 메시지 넣고 스레드 깨움
	bool GetMessageData(Message ** message);						// 큐에서 메시지 꺼냄

	void NewPlayer(MESSAGE * message);						// 새 플레이어 생성
	void DeletePlayer(__int64 sessionID);					// 플레이어 삭제
	void PacketProc(MESSAGE * message);						// 패킷 파싱함수

	bool Login(__int64 sessionID, StreamBuffer * data);		// 로그인 처리함수
	bool MoveSector(__int64 sessionID, StreamBuffer * data);// 섹터이동 처리함수
	bool Chat(__int64 sessionID, StreamBuffer * data);		// 채팅 처리함수

	void MakePacket_Res_Login(StreamBuffer * packet, bool login, __int64 accountNo);
	void MakePacket_Res_MoveSector(StreamBuffer * packet, __int64 accountNo, WORD sectorX, WORD sectorY);
	void MakePacket_Res_Chat(StreamBuffer * packet, __int64 accountNo, WCHAR * ID, WCHAR * nickName, WORD chatLen, WCHAR * chat);

	void BroadCastNeighborSector(StreamBuffer * packet, PLAYER * player); // 플레이어 주변 9섹터에 브로드캐스트
	void BroadCastSector(StreamBuffer * packet, WORD sectorX, WORD sectorY); // 1개 섹터 내부의 모든플레이어에게 브로드캐스트
public:
	// NetServer을(를) 통해 상속됨
	ChatNetServer();
	~ChatNetServer();

	void Stop();

	virtual void OnClientJoin(__int64 ClientID, Session * session) override;
	virtual void OnClientLeave(__int64 ClientID) override;
	virtual bool OnConnectionRequest(WCHAR * ClientIP, int Port) override;
	virtual void OnRecv(__int64 clientID, StreamBuffer * packet) override;
	virtual void OnSend(__int64 clientID, int sendSize) override;
	virtual void OnError(int errorCode, const WCHAR * errorMessage) override;

	friend class ChatServer;
};

enum ERROR_CODE
{
	PACKET_TYPE_ERROR,
	NOT_EXIST_PLAYER,
	NOT_LOGIN,
	WRONG_ACCOUNT_NO,
	WRONG_MOVE_SECTOR_X,
	WRONG_MOVE_SECTOR_Y,
	WRONG_CHATLEN,
	NOT_EXIST_SESSIONKEY,
	WRONG_SESSIONKEY
};