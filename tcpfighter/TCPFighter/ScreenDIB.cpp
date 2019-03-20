#include "stdafx.h"
//////////////////////////////////////////////////////////////
// ScreenDIB
//////////////////////////////////////////////////////////////

/*----------------------------------------------------------*/
// ScreenDIB::CreateDIBBuffer (protected) 
// 설명 : DIB헤더를 세팅하고 백버퍼 동적할당
// 인자 : (int) 넓이, (int) 높이, (int) 컬러비트수
// 리턴 :
/*----------------*////////////////////////*----------------*/
void ScreenDIB::CreateDIBBuffer(int width, int height, int colorBit)
{
	_DIBInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	_DIBInfo.bmiHeader.biWidth = width;
	_DIBInfo.bmiHeader.biHeight = -height;
	_DIBInfo.bmiHeader.biPlanes = 1;
	_DIBInfo.bmiHeader.biBitCount = colorBit;
	_DIBInfo.bmiHeader.biCompression = BI_RGB;
	_DIBInfo.bmiHeader.biSizeImage = 0;
	_DIBInfo.bmiHeader.biXPelsPerMeter = 0;
	_DIBInfo.bmiHeader.biYPelsPerMeter = 0;
	_DIBInfo.bmiHeader.biClrUsed = 0;
	_DIBInfo.bmiHeader.biClrImportant = 0;

	_bufferSize = _width * _pitch;
	_byteBuffer = new BYTE[_bufferSize];
}

/*----------------------------------------------------------*/
// ScreenDIB::ReleaseBuffer (protected) 
// 설명 : 백버퍼 할당해제
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void ScreenDIB::ReleaseBuffer()
{
	delete[] _byteBuffer;
}

/*----------------------------------------------------------*/
// ScreenDIB::ScreenDIB (public) 
// 설명 : 스크린DIB 생성자
//  ㄴ  : CreateDIBBuffer 호출
// 인자 : (int) 넓이, (int) 높이, (int) 컬러비트수
// 리턴 :
/*----------------*////////////////////////*----------------*/
ScreenDIB::ScreenDIB(int width, int height, int colorBit)
{
	_width = width;
	_height = height;
	_colorBit = colorBit;
	_pitch = (_width * _colorBit + 3) & ~3;
	CreateDIBBuffer(_width, _height, _colorBit);
}

/*----------------------------------------------------------*/
// ScreenDIB::~ScreenDIB (public) 
// 설명 : 스크린DIB 소멸자
//  ㄴ  : ReleaseBuffer 호출
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
ScreenDIB::~ScreenDIB()
{
	ReleaseBuffer();
}

/*----------------------------------------------------------*/
// ScreenDIB::CreateDIBBuffer (public) 
// 설명 : 백버퍼를 실제 화면에 그리기
// 인자 : (HWND) 윈도우핸들, (int) 윈도우의 x좌표, (int)윈도우의 y좌표
// 리턴 :
/*----------------*////////////////////////*----------------*/
void ScreenDIB::DrawBuffer(HWND hWnd, int x, int y)
{
	HDC hdc = GetDC(hWnd);
	RECT rect;
	GetClientRect(hWnd, &rect);
	SetStretchBltMode(hdc, COLORONCOLOR);
	StretchDIBits(hdc, x, y, rect.right, rect.bottom, 0, 0, _width, _height, _byteBuffer, &_DIBInfo, DIB_RGB_COLORS, SRCCOPY);
	InvalidateRect(hWnd, &rect, false);
	ReleaseDC(hWnd, hdc);
}

/*----------------------------------------------------------*/
// ScreenDIB::Clear (public) 
// 설명 : 백버퍼 0으로 초기화
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void ScreenDIB::Clear()
{
	ZeroMemory(_byteBuffer, _bufferSize);
}

/*----------------------------------------------------------*/
// ScreenDIB::GetWidth (public) 
// 설명 : 백버퍼의 넓이를 리턴한다.
// 인자 : 
// 리턴 : (int) 백버퍼의 넓이
/*----------------*////////////////////////*----------------*/
int ScreenDIB::GetWidth()
{
	return _width;
}

/*----------------------------------------------------------*/
// ScreenDIB::GetHeight (public) 
// 설명 : 백버퍼의 높이를 리턴한다.
// 인자 : 
// 리턴 : (int) 백버퍼의 높이
/*----------------*////////////////////////*----------------*/
int ScreenDIB::GetHeight()
{
	return _height;
}

/*----------------------------------------------------------*/
// ScreenDIB::GetPitch (public) 
// 설명 : 백버퍼의 피치를 리턴한다.
// 인자 : 
// 리턴 : (int) 백버퍼의 피치
/*----------------*////////////////////////*----------------*/
int ScreenDIB::GetPitch()
{
	return _pitch;
}

/*----------------------------------------------------------*/
// ScreenDIB::GetByteBuffer (public) 
// 설명 : 백버퍼의 주소포인터를 리턴한다.
// 인자 : 
// 리턴 : (BYTE *) 백버퍼의 주소 포인터
/*----------------*////////////////////////*----------------*/
BYTE * ScreenDIB::GetByteBuffer()
{
	return _byteBuffer;
}
