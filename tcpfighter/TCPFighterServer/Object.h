#pragma once
struct SECTOR_POS;
struct Session;
struct Player;

// 위치 구조체
struct Pos
{
	short x;
	short y;
};

// 세션 구조체
struct Session
{
	SOCKET socket;				// 소켓
	SOCKADDR_IN addr;			// 소켓 주소 구조체
	StreamQueue sendStream;		// send링버퍼
	StreamQueue recvStream;		// recv링버퍼
	bool saveHeaderFlag;		// 헤더 저장 플래그
	PACKET_HEADER saveHeader;	// 저장된 헤더
	Player * character;			// 캐릭터 포인터
	ULONGLONG lastRecvTime;		// 패킷 마지막으로 받은 시간
	// 생성자
	Session(SOCKET socket, SOCKADDR_IN addr);
};

// 캐릭터 구조체
struct Player
{
	Session * _session;			// 세션 포인터	
	int _ID;					// 고유번호
	Pos _pos;					// 위치
	Pos _moveStartPos;			// 이동이 시작된 위치
	BYTE _direction;			// 캐릭터 방향
	BYTE _healthPoint;			// 체력
	TAG_ACTION _action;			// 현재 행동
	SECTOR_POS _oldSector;		// 이전 섹터위치
	SECTOR_POS _newSector;		// 새 섹터위치
	ULONGLONG _lastActionTime;	// 마지막 행동 시간

	// 생성자
	Player(Session * session, int ID);
	// 소멸자
	~Player();
	// 행동
	bool Action();
	// 이동
	void Move(short x, short y);
};

