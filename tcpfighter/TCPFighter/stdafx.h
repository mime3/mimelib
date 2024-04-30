﻿// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Imm32.lib")
#include "targetver.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <TimeAPI.h>
#include <windowsx.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
//#include "DefineOption.h"

#include "Queue.h"
#include "LinkedList.h"

#include "StreamQueue.h"
#include "StreamBuffer.h"

#include "ScreenDIB.h"
#include "SpriteDIB.h"
#include "FrameSkip.h"

#include "EnumAndDefine.h"
#include "PacketDefine.h"
#include "NetWorkObject.h"

#include "Object.h"
#include "TileMap.h"
#include "Function.h"
#include "NetWorkFunction.h"