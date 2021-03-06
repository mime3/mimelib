// main.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
/*
초당 루프수
초당 패킷전송 수
초당 수신수
초당 전송수

select 1회 시간
전체 전송시간
전체 수신시간
로직 시간
RTT
*/
#include "stdafx.h"
Moniter moniter;
char g_ip[32];
WCHAR g_string[1000];

ULONGLONG printTime = GetTickCount64();

void Init();
void PrintState();
void Working();


Pos DeadReckoning(Session * session);
int main()
{
	_wsetlocale(LC_ALL, L"Korean");
	wstring * fileName = new wstring(L"DummyLog.txt");
	Logger::GetInstance().SetFileName(fileName);
	Init();
	while (1)
	{
		NetworkingEx();
		moniter.loopPerSec++;
		PacketRecv();
		Working();
		ULONGLONG cutTime = GetTickCount64();
		if (cutTime - printTime > 1000)
		{
			printTime = cutTime;
			PrintState();
			moniter.recvCount = 0;
			moniter.loopPerSec = 0;
		}
	}
    return 0;
}

void PrintState()
{
	LOG(Logger::LOG_LEVEL_ERROR, L"세션수 : %d , 접속중 : %d, 접속실패 %d", moniter.sessionFullCount, moniter.sessionLiveCount, moniter.sessionDeadCount);
	LOG(Logger::LOG_LEVEL_ERROR, L"누적Sync수 : %d , 초당 수신수 %d", moniter.syncCount, moniter.recvCount);
	LOG(Logger::LOG_LEVEL_ERROR, L"endCode에러 %d", moniter.endCodeError);
	LOG(Logger::LOG_LEVEL_ERROR, L"초당 loop 수 %d", moniter.loopPerSec);
	LOG(Logger::LOG_LEVEL_ERROR, L"");
}

void Init()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	printf_s("IP :");
	scanf_s("%s", g_ip, 32);
	// 소켓생성 후 map에 담기
	int sessionCount;
	printf_s("세션수 :");
	scanf_s("%d", &sessionCount);
	moniter.sessionFullCount = sessionCount;
	for (int i = 0; i < sessionCount; ++i)
	{
		SOCKET sock = socket(AF_INET, SOCK_STREAM, NULL);
		SOCKADDR_IN addr;
		ZeroMemory(&addr, sizeof(addr));
		inet_pton(AF_INET, g_ip, &addr.sin_addr.s_addr);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(NETWORK_PORT);
		int retval = connect(sock, (SOCKADDR*)&addr, sizeof(addr));
		if (retval != 0)
		{
			printf_s("connect error %d\n", i + 1);
			moniter.sessionDeadCount++;
			continue;
		}
		Session * newSession = new Session(sock, addr);
		moniter.sessionLiveCount++;
		printf_s("%d Connected\n", i + 1);
	}
}

void Working()
{
	map<SOCKET, Session *>::iterator iter = moniter.user_socket.begin();
	map<SOCKET, Session *>::iterator iterEnd = moniter.user_socket.end();
	for (; iter != iterEnd; ++iter)
	{
		if (iter->second->character == nullptr)
			continue;
		ULONGLONG curTime = GetTickCount64();
		if (curTime - iter->second->character->_lastActionTime < iter->second->character->_delay)
			continue;
	
		iter->second->character->_delay = rand() % 4000 + 1000;
		Pos pos = DeadReckoning(iter->second);
		iter->second->character->_pos = pos;
		int action = rand() % (8 + 3 + 1); // 0 ~ 11
		switch (action)
		{
		case 0:
		case 1:
		case 2:
		case 3:	
		case 4:
		case 5:		
		case 6:			
		case 7:
		{
			StreamBuffer * move;
			MakePacket_Move_Start(&move, (BYTE)action, iter->second->character->_pos);
			SendPacket(move, iter->second);
			iter->second->character->_action = (TAG_ACTION)action;
			break;
		}
		case 8:
		{
			StreamBuffer * attack1;
			MakePacket_Attack1(&attack1, dfPACKET_MOVE_DIR_LL, iter->second->character->_pos);
			SendPacket(attack1, iter->second);
			iter->second->character->_action = ATTACK_TYPE_1;
			break;
		}
		case 9:
		{
			StreamBuffer * attack2;
			MakePacket_Attack2(&attack2, dfPACKET_MOVE_DIR_LL, iter->second->character->_pos);
			SendPacket(attack2, iter->second);
			iter->second->character->_action = ATTACK_TYPE_2;
			break;
		}
		case 10:
		{
			StreamBuffer * attack3;
			MakePacket_Attack3(&attack3, dfPACKET_MOVE_DIR_LL, iter->second->character->_pos);
			SendPacket(attack3, iter->second);
			iter->second->character->_action = ATTACK_TYPE_3;
			break;
		}
		case 11:
		{
			StreamBuffer * stop;
			MakePacket_Move_Stop(&stop, dfPACKET_MOVE_DIR_LL, iter->second->character->_pos);
			SendPacket(stop, iter->second);
			iter->second->character->_action = STAND;
			break;
		}
		default:
			break;
		}
		iter->second->character->_lastActionTime = curTime;
	}
}

Pos DeadReckoning(Session * session)
{
	ULONGLONG time = GetTickCount64() - session->character->_lastActionTime;
	time /= 20;
	short x = session->character->_pos.x;
	short y = session->character->_pos.y;
	Pos oldPos = { x, y };
	for (int i = 0; i < time; ++i)
	{
		switch (session->character->_action)
		{
		case ACTION_MOVE_LL:
			x -= MOVE_TICK_X;
			break;
		case ACTION_MOVE_LU:
			x -= MOVE_TICK_X;
			y -= MOVE_TICK_Y;
			break;
		case ACTION_MOVE_UU:
			y -= MOVE_TICK_Y;
			break;
		case ACTION_MOVE_RU:
			x += MOVE_TICK_X;
			y -= MOVE_TICK_Y;
			break;
		case ACTION_MOVE_RR:
			x += MOVE_TICK_X;
			break;
		case ACTION_MOVE_RD:
			x += MOVE_TICK_X;
			y += MOVE_TICK_Y;
			break;
		case ACTION_MOVE_DD:
			y += MOVE_TICK_Y;
			break;
		case ACTION_MOVE_LD:
			x -= MOVE_TICK_X;
			y += MOVE_TICK_Y;
			break;
		default:
			return Pos{ x, y };
		}
		if (x < 0 || MAP_X_SIZE < x)
			return Pos{ oldPos.x, oldPos.y };
		if (y < 0 || MAP_Y_SIZE < y)
			return Pos{ oldPos.x, oldPos.y };

		oldPos.x = x;
		oldPos.y = y;
	}
	return Pos{ x, y };
}