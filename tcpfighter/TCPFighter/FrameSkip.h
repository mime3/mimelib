#pragma once
class FrameSkip
{
	DWORD _systemTick;		// 마지막 프레임 시간
	int _maxFrame;			// 목표 프레임 수
	int _oneFrameTick;		// 1프레임당 시간
	int _tick;				// 시간 초과치 저장 변수

public:
	FrameSkip(int FPS);
	~FrameSkip();
	// 프레임 스킵여부
	bool Skip();
};