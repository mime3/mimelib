#include "stdafx.h"
#pragma comment(lib, "Winmm.lib")

//////////////////////////////////////////////////////////////
// class : public class
//////////////////////////////////////////////////////////////

/*----------------------------------------------------------*/
// FrameSkip::FrameSkip (public)
// ���� : �����ӽ�ŵ ������
//  ��  : timeBeginPeriod(1)�� ȣ���Ѵ�.
// ���� : (int) �ʴ� ������ ��
// ���� :
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
// ���� : �����ӽ�ŵ �Ҹ���
//  ��  : timeEndPeriod(1)�� ȣ���Ѵ�.
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
FrameSkip::~FrameSkip()
{
	timeEndPeriod(1);
}

/*----------------------------------------------------------*/
// FrameSkip::Skip (public)
// ���� : ������ ��ŵ���� ����
// ���� : 
// ���� : (bool) ������ ��ŵ����
/*----------------*////////////////////////*----------------*/
bool FrameSkip::Skip()
{
	// ����ð�
	DWORD curTime = timeGetTime();
	// 1�����Ӵ� �ɷ��� �� �ð����� ����
	_tick += (curTime - _systemTick) - _oneFrameTick;
	_systemTick = timeGetTime();
	// ����� �ð� ����
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
