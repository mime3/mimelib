#pragma once

class ScreenDIB
{
private:
protected:
	BITMAPINFO	_DIBInfo;		// ��Ʈ��ó�� Ȱ���ϱ� ���� ��Ʈ�����
	BYTE		*_byteBuffer;	// ����� �ּ� ������
	int			_width;			// ������� ����
	int			_height;		// ������� ����
	int			_pitch;			// ������� ��ġ
	int			_colorBit;		// ������� �÷� ��Ʈ��
	int			_bufferSize;	// ���۾��� ũ��

	// DIBInfo�����ϰ� ����ۻ���
	void CreateDIBBuffer(int width, int height, int colorBit);	
	// ����� ����
	void ReleaseBuffer();										

public:
	ScreenDIB(int width, int height , int colorBit);
	virtual ~ScreenDIB();

	// ����۸� ȭ�鿡 �׸���.
	void	DrawBuffer(HWND hWnd, int x = 0, int y = 0);	
	// ����� 0���� �ʱ�ȭ
	void	Clear();										
	// ����� ���̸���
	int		GetWidth();										
	// ����� ���� ����
	int		GetHeight();									
	// ����� ��ġ ����
	int		GetPitch();										
	// ����� �ּ� ������ ����
	BYTE *	GetByteBuffer();								
};
