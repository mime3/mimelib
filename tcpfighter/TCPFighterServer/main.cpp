// TCPFighterServer.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

multimap<TAG_ACTION, SECTOR_POS> g_moveInsertTable;
multimap<TAG_ACTION, SECTOR_POS> g_moveDeleteTable;
map<SOCKET, Session *> g_user_socket;
map<SOCKET, Session *> g_deleteUser_socket;
list<Player *> g_playerList;
//FrameSkipServer g_frameSkipServer(50);
int g_ID = 1;

// frame계산
DWORD g_systemTime = timeGetTime();
DWORD g_delayTime = 0;
DWORD printTimes = timeGetTime();
DWORD printTime = 0;
DWORD intervalTime = g_systemTime;
DWORD maxInterval = 0;

// 키 입력
int g_keyState = 'l';
void KeyProcess();

int main()
{
	timeBeginPeriod(1);
	_wsetlocale(LC_ALL, L"Korean");
	SOCKET listenSocket;
	wstring * fileName = new wstring(L"ServerLog.txt");
	Logger::GetInstance().SetFileName(fileName);
	if (Init(&listenSocket))
	{
		int frame = 0;
		while (true)
		{
			KeyProcess();
			NetworkingEx(listenSocket);
			PacketRecv();
			DeleteSession();
			// logic는 1초에 50회 실행되도록
			DWORD curTime = timeGetTime();
			g_delayTime += curTime - g_systemTime;
			g_systemTime = curTime;
			while (g_delayTime >= 20)
			{
				if (maxInterval < curTime - intervalTime)
					maxInterval = curTime - intervalTime;
				//if (curTime - intervalTime > 30)
					//LOG(Logger::LOG_LEVEL_WARNING, L"frames interval %d ms", curTime - intervalTime);
				//if (curTime - intervalTime < 10)
					//LOG(Logger::LOG_LEVEL_DEBUG, L"frames interval %d ms", curTime - intervalTime);
				intervalTime = curTime;
				Logic();
				g_delayTime -= 20;
				frame++;
			}
			
			printTime = timeGetTime();
			if (printTime - printTimes >= 1000)
			{
				if(frame != 50)
					LOG(Logger::LOG_LEVEL_ERROR, L"%d frame", frame);
				if(maxInterval > 30)
					LOG(Logger::LOG_LEVEL_ERROR, L"max IntervalTime:%d", maxInterval);
				printTimes = printTime;
				frame = 0;
				maxInterval = 0;
			}
		}
	}
	timeEndPeriod(1);
	WSACleanup();
    return 0;
}

void KeyProcess()
{
	if (_kbhit())
	{
		int key = _getch();
		key = tolower(key);
		switch (key)
		{
		case 'u':
			if (g_keyState == 'l')
			{
				g_keyState = key;
				LOG(Logger::LOG_LEVEL_ERROR, L"Unlock");
			}
			break;
		case 'l':
			if (g_keyState == 'u')
			{
				g_keyState = key;
				LOG(Logger::LOG_LEVEL_ERROR, L"lock");
			}
			break;
		case '1':
			if (g_keyState == 'u')
			{
				Logger::GetInstance().SetLogLevel(Logger::LOG_LEVEL_DEBUG);
				g_keyState = 'l';
				LOG(Logger::LOG_LEVEL_ERROR, L"Set LOG_LEVEL_DEBUG");
			}
			break;
		case '2':
			if (g_keyState == 'u')
			{
				Logger::GetInstance().SetLogLevel(Logger::LOG_LEVEL_WARNING);
				g_keyState = 'l';
				LOG(Logger::LOG_LEVEL_ERROR, L"Set LOG_LEVEL_WARNING");
			}
			break;
		case '3':
			if (g_keyState == 'u')
			{
				Logger::GetInstance().SetLogLevel(Logger::LOG_LEVEL_ERROR);
				g_keyState = 'l';
				LOG(Logger::LOG_LEVEL_ERROR, L"Set LOG_LEVEL_ERROR");
			}
			break;
		case 's':
			if (g_keyState == 'u')
			{
				Logger::GetInstance().OnOff(true);
				g_keyState = 'l';
				LOG(Logger::LOG_LEVEL_ERROR, L"Save On");
			}
			break;
		case 'd':
			if (g_keyState == 'u')
			{
				Logger::GetInstance().OnOff(false);
				g_keyState = 'l';
				LOG(Logger::LOG_LEVEL_ERROR, L"Save Off");
			}
			break;
		}
	}
}