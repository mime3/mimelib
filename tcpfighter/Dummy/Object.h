#pragma once

//struct SECTOR_POS;
struct Session;
struct Player;

// ��ġ ����ü
struct Pos
{
	short x;
	short y;
};

// ���� ����ü
struct Session
{
	SOCKET socket;				// ����
	SOCKADDR_IN addr;			// ���� �ּ� ����ü
	StreamQueue sendStream;		// send������
	StreamQueue recvStream;		// recv������
	bool saveHeaderFlag;		// ��� ���� �÷���
	PACKET_HEADER saveHeader;	// ����� ���
	Player * character;			// ĳ���� ������
	ULONGLONG lastRecvTime;		// ��Ŷ ���������� ���� �ð�
	// ������
	Session(SOCKET socket, SOCKADDR_IN addr);
	~Session();
};

// ĳ���� ����ü
struct Player
{
	Session * _session;			// ���� ������	
	int _ID;					// ������ȣ
	Pos _pos;					// ��ġ
	Pos _moveStartPos;			// �̵��� ���۵� ��ġ
	BYTE _direction;			// ĳ���� ����
	BYTE _healthPoint;			// ü��
	TAG_ACTION _action;			// ���� �ൿ
	ULONGLONG _lastActionTime;		// ������ �ൿ�ð�
	ULONGLONG _delay;

	// ������
	Player(Session * session, int ID);
	// �Ҹ���
	~Player();
	// �ൿ
	bool Action();
	// �̵�
	/*void Move(short x, short y);*/
};

struct Moniter
{
	map<SOCKET, Session *> user_socket;
	list<Player *> playerList;

	int sessionFullCount;
	int sessionLiveCount;
	int sessionDeadCount;
	int syncCount;
	int recvCount;
	int endCodeError;
	int loopPerSec;
};