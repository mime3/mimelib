#pragma once
#include <map>
#include <unordered_map>
#include "Player.h"
#include "Sector.h"
#include "process.h"
#include "CommonProtocol.h"
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

// �޽��� ����ü
typedef struct MESSAGE
{
	// Ÿ��
	enum MESSAGE_TYPE
	{
		JOIN,
		LEAVE,
		PACKET
	};
	MESSAGE_TYPE type;		// Ÿ��
	__int64 sessionID;		// ���� ID
	StreamBuffer * data;	// ��Ŷ ������
}Message;

constexpr WORD SECTOR_X = 100;
constexpr WORD SECTOR_Y = 100;

class ChatNetServer : public NetServer
{
	SRWLOCK *							_keyMapLock;
	unordered_map<INT64, SESSION_KEY> * _keyMap;

public:
	MemoryPoolTLS<PLAYER>			_memoryPoolPlayer;		// �÷��̾� �޸�Ǯ
	MemoryPoolTLS<MESSAGE>			_memoryPoolMessage;		// �޽��� �޸�Ǯ
	unordered_map<__int64, PLAYER *>_playerMap;				// �÷��̾���� ������ MAP
	LF_Queue<MESSAGE *>				_messageQueue;			// �޽����� ���� ������ ť

	DBConnector						_dbConnector;			// chat����� DBĿ����

	int _LOGIN_PLAYER = 0;
	int _NOT_EXIST_SESSIONKEY = 0;
	int _WRONG_SESSIONKEY = 0;
	int _PACKET_TYPE_ERROR = 0;								// ���������
	int _NOT_EXIST_PLAYER = 0;								// ���������
	int _WRONG_MOVE = 0;									// ���������
	int _WRONG_CHATLEN = 0;									// ���������
	int _UPDATE_TPS = 0;									// ���������
	ULONGLONG _UPDATE_TPS_TIME = GetTickCount64();									// ���������

private:
	HANDLE					_wakeUpEvent;					// �����带 ���� �̺�Ʈ��ü
	HANDLE					_updateThread;					// ������ ó�� ������
	SECTOR					_sector[SECTOR_Y][SECTOR_X];	// ���Ͱ��� 2�߹迭
	bool					_exitFlag;						// ���� �÷��� (�̱���)
	
	static unsigned int WINAPI UpdateThreadMain(LPVOID lpParam);	// ������Ʈ ������ ����
	void PutMessageData(Message * message);							// ť�� �޽��� �ְ� ������ ����
	bool GetMessageData(Message ** message);						// ť���� �޽��� ����

	void NewPlayer(MESSAGE * message);						// �� �÷��̾� ����
	void DeletePlayer(__int64 sessionID);					// �÷��̾� ����
	void PacketProc(MESSAGE * message);						// ��Ŷ �Ľ��Լ�

	bool Login(__int64 sessionID, StreamBuffer * data);		// �α��� ó���Լ�
	bool MoveSector(__int64 sessionID, StreamBuffer * data);// �����̵� ó���Լ�
	bool Chat(__int64 sessionID, StreamBuffer * data);		// ä�� ó���Լ�

	void MakePacket_Res_Login(StreamBuffer * packet, bool login, __int64 accountNo);
	void MakePacket_Res_MoveSector(StreamBuffer * packet, __int64 accountNo, WORD sectorX, WORD sectorY);
	void MakePacket_Res_Chat(StreamBuffer * packet, __int64 accountNo, WCHAR * ID, WCHAR * nickName, WORD chatLen, WCHAR * chat);

	void BroadCastNeighborSector(StreamBuffer * packet, PLAYER * player); // �÷��̾� �ֺ� 9���Ϳ� ��ε�ĳ��Ʈ
	void BroadCastSector(StreamBuffer * packet, WORD sectorX, WORD sectorY); // 1�� ���� ������ ����÷��̾�� ��ε�ĳ��Ʈ
public:
	// NetServer��(��) ���� ��ӵ�
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