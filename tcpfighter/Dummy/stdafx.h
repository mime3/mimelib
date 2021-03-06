// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#pragma comment(lib, "ws2_32.lib")

using namespace std;
#include "targetver.h"

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <windowsx.h>
#include <TimeAPI.h>
#include <list>
#include <map>
#include <conio.h>
#include <time.h>
#include <locale.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "StreamQueue.h"
#include "StreamBuffer.h"
#include "Clock.h"
#include "Logger.h"

#include "../TCPFighterServer/Define.h"
#include "../TCPFighterServer/PacketDefine.h"
#include "Object.h"
#include "NetworkFunction.h"
#include "MakePacket.h"