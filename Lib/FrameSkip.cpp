#pragma once
//#include "stdafx.h"
//#include "pch.h"
#include "FrameSkip.h"

//////////////////////////////////////////////////////////////
// class : public class
//////////////////////////////////////////////////////////////

/*----------------------------------------------------------*/
// FrameSkip::FrameSkip (public)
// 설명 : 프레임스킵 생성자
//  ㄴ  : timeBeginPeriod(1)을 호출한다.
// 인자 : (int) 초당 프레임 수
// 리턴 :
/*----------------*////////////////////////*----------------*/
FrameSkip::FrameSkip(int FPS)
{
	timeBeginPeriod(1);
	_systemTick = timeGetTime();
	_maxFrame = FPS;
	_oneFrameTick = 1000 / FPS;
	_tick = 0;
}

/*----------------------------------------------------------*/
// FrameSkip::~FrameSkip (public)
// 설명 : 프레임스킵 소멸자
//  ㄴ  : timeEndPeriod(1)을 호출한다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
FrameSkip::~FrameSkip()
{
	timeEndPeriod(1);
}

/*----------------------------------------------------------*/
// FrameSkip::Skip (public)
// 설명 : 프레임 스킵여부 리턴
// 인자 : 
// 리턴 : (bool) 프레임 스킵여부
/*----------------*////////////////////////*----------------*/
bool FrameSkip::Skip()
{
	DWORD curTime = timeGetTime();
	_tick += (curTime - _systemTick) - _oneFrameTick;
	_systemTick = curTime;
	if (_oneFrameTick <= _tick)
	{
		_tick -= _oneFrameTick;
		return true;
	}

	else if (_tick < 0)
	{
		Sleep((-1) * _tick);
		_tick = 0;
		_systemTick = timeGetTime();
	}
	return false;
}

FrameSkipServer::FrameSkipServer(int FPS) : FrameSkip(FPS)
{
}

FrameSkipServer::~FrameSkipServer()
{
}

bool FrameSkipServer::Skip()
{
	DWORD curTime = timeGetTime();
	_tick += (curTime - _systemTick) - _oneFrameTick;
	_systemTick = curTime;
	// 늦은 시간 따라가기
	if (_oneFrameTick <= _tick)
	{
		_tick -= _oneFrameTick;
		return true;
	}
	// 너무 빨리 호출됬을땐 false반환
	return false;
}

int FrameSkipServer::GetTick()
{
	return _tick;
}

void FrameSkipServer::ChangeTick(int tick)
{
	_tick = tick;
}
