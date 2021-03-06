// ChatServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include "CrashDump.h"
#include "ProFiler.h"
#include "ChatServer.h"

#pragma comment(lib, "winmm.lib")
bool Check_1Sec(ULONGLONG *time);

CrashDump cd;

bool Check_1Sec(ULONGLONG *time)
{
	ULONGLONG curTime = GetTickCount64();
	if (curTime - *time >= 1000)
	{
		*time = curTime;
		return true;
	}
	return false;
}

int wmain(int argc, WCHAR * argv[])
{
	timeBeginPeriod(1);

	ChatServer * chatServer = new ChatServer(argv[0]);
	chatServer->Start();
	ChatNetServer * chatNetServer = chatServer->_netServer;
	while (1)
	{

		printf_s("[CHAT SERVER] LOGIN SERVER CONNECTION <%s>\n", chatServer->_lanClient->_serverID == 0 ? "NO":"OK");
		printf_s("----------------------------------------------------\n");
		printf_s("Session Count = %d\n", chatNetServer->GetClientCount());
		printf_s("Packet Alloc Count = %d\n", StreamBuffer::_allocCount);

		printf_s("UpdateThread MSG Alloc Count = %I64d\n", chatNetServer->_memoryPoolMessage._allocCount);
		printf_s("UpdateThread Queue Count = %I64d\n", chatNetServer->_messageQueue.GetUseCount());
		
		printf_s("Player Alloc Count = %I64d\n", chatNetServer->_memoryPoolPlayer._allocCount);
		printf_s("Player MapSize = %I64d\n", chatNetServer->_playerMap.size());

		printf_s("Accept Count = %I64d\n", chatNetServer->_acceptCount);
		printf_s("Accept TPS = %d\n", chatNetServer->_acceptTPS);
		printf_s("Update TPS = %d\n", chatNetServer->_UPDATE_TPS);

		printf_s("key map size = %d\n", chatServer->_keyMap.size());
		printf_s("NOT EXIST SESSION KEY = %d\n", chatNetServer->_NOT_EXIST_SESSIONKEY);
		printf_s("WRONG SESSION KEY = %d\n", chatNetServer->_WRONG_SESSIONKEY);

		printf_s("----------------------------------------------------\n");

		InterlockedAdd((LONG*)&chatNetServer->_acceptTPS, -chatNetServer->_acceptTPS);
		InterlockedAdd((LONG*)&chatNetServer->_UPDATE_TPS, -chatNetServer->_UPDATE_TPS);

		Sleep(1000);
	}
	chatServer->Stop();

	timeEndPeriod(1);
	return 0;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
