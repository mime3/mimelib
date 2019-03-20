#include "stdafx.h"

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

void ScreenDIB::ReleaseBuffer()
{
	delete[] _byteBuffer;
}

ScreenDIB::ScreenDIB(int width, int height, int colorBit)
{
	_width = width;
	_height = height;
	_colorBit = colorBit;
	_pitch = (_width * _colorBit + 3) & ~3;
	CreateDIBBuffer(_width, _height, _colorBit);
}

ScreenDIB::~ScreenDIB()
{
	ReleaseBuffer();
}

void ScreenDIB::DrawBuffer(HWND hWnd, int x, int y)
{
	HDC hdc = GetDC(hWnd);
	RECT rect;
	GetClientRect(hWnd, &rect);
	SetStretchBltMode(hdc, COLORONCOLOR);
	StretchDIBits(hdc, 0, 0, rect.right, rect.bottom, 0, 0, _width, _height, _byteBuffer, &_DIBInfo, DIB_RGB_COLORS, SRCCOPY);
	InvalidateRect(hWnd, &rect, false);
	ReleaseDC(hWnd, hdc);
}

void ScreenDIB::Clear()
{
	ZeroMemory(_byteBuffer, _bufferSize);
}

int ScreenDIB::GetWidth()
{
	return _width;
}

int ScreenDIB::GetHeight()
{
	return _height;
}

int ScreenDIB::GetPitch()
{
	return _pitch;
}

BYTE * ScreenDIB::GetByteBuffer()
{
	return _byteBuffer;
}
