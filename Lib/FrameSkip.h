#pragma once
class FrameSkip
{
protected:
	DWORD _systemTick;		// ������ ������ �ð�
	int _maxFrame;			// ��ǥ ������ ��
	int _oneFrameTick;		// 1�����Ӵ� �ð�
	int _tick;				// �ð� �ʰ�ġ ���� ����
public:
	FrameSkip(int FPS);
	virtual ~FrameSkip();
	// ������ ��ŵ����
	virtual bool Skip();
};

class FrameSkipServer : public FrameSkip
{
public:
	FrameSkipServer(int FPS);
	~FrameSkipServer();
	bool Skip();
	int GetTick();
	void ChangeTick(int tick);
};