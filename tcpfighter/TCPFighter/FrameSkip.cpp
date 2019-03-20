#include "stdafx.h"
#pragma comment(lib, "Winmm.lib")

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
	// 현재시간
	DWORD curTime = timeGetTime();
	// 1프레임당 걸려야 할 시간과의 차이
	_tick += (curTime - _systemTick) - _oneFrameTick;
	_systemTick = timeGetTime();
	// 저장된 시간 갱신
	if (_oneFrameTick <= _tick)
	{
		_tick -= _oneFrameTick;
		return true;
	}
	else if (_tick < 0)
	{
		_systemTick -= _tick;
		Sleep((-1) * _tick);
		_tick = 0;
		return false;
	}
	else
		return false;
	
}
