#pragma once
class FrameSkip
{
	DWORD _systemTick;		// ������ ������ �ð�
	int _maxFrame;			// ��ǥ ������ ��
	int _oneFrameTick;		// 1�����Ӵ� �ð�
	int _tick;				// �ð� �ʰ�ġ ���� ����

public:
	FrameSkip(int FPS);
	~FrameSkip();
	// ������ ��ŵ����
	bool Skip();
};