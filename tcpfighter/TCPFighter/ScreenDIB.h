#pragma once

class ScreenDIB
{
private:
protected:
	BITMAPINFO	_DIBInfo;		// 비트맵처럼 활용하기 위한 비트맵헤더
	BYTE		*_byteBuffer;	// 백버퍼 주소 포인터
	int			_width;			// 백버퍼의 넓이
	int			_height;		// 백버퍼의 높이
	int			_pitch;			// 백버퍼의 피치
	int			_colorBit;		// 백버퍼의 컬러 비트수
	int			_bufferSize;	// 백퍼어의 크기

	// DIBInfo세팅하고 백버퍼생성
	void CreateDIBBuffer(int width, int height, int colorBit);	
	// 백버퍼 해제
	void ReleaseBuffer();										

public:
	ScreenDIB(int width, int height , int colorBit);
	virtual ~ScreenDIB();

	// 백버퍼를 화면에 그린다.
	void	DrawBuffer(HWND hWnd, int x = 0, int y = 0);	
	// 백버퍼 0으로 초기화
	void	Clear();										
	// 백버퍼 넓이리턴
	int		GetWidth();										
	// 백버퍼 높이 리턴
	int		GetHeight();									
	// 백버퍼 피치 리턴
	int		GetPitch();										
	// 백버퍼 주소 포인터 리턴
	BYTE *	GetByteBuffer();								
};
