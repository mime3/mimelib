//#include "stdafx.h"
//#include "pch.h"
#include "DoubleBuffer.h"

DoubleBuffer::DoubleBuffer(HWND hWnd)
{
	_hWnd = hWnd;
	HDC hdc = GetDC(hWnd);
	_memDC = CreateCompatibleDC(hdc);
	ReleaseDC(hWnd, hdc);
	GetClientRect(_hWnd, &_rect);
	_bitMap = CreateCompatibleBitmap(GetDC(hWnd), _rect.right, _rect.bottom);
	SelectObject(_memDC, _bitMap);
}

DoubleBuffer::~DoubleBuffer()
{
	DeleteObject(_bitMap);
	ReleaseDC(_hWnd, _memDC);
}

inline HDC DoubleBuffer::GetMemDC()
{
	return _memDC;
}

void DoubleBuffer::Clear()
{
	PatBlt(_memDC, 0, 0, _rect.right, _rect.bottom, WHITENESS);
}

void DoubleBuffer::Flipping()
{
	HDC hdc = GetDC(_hWnd);
	BitBlt(hdc, 0, 0, _rect.right, _rect.bottom, _memDC, 0, 0, SRCCOPY);
	ReleaseDC(_hWnd, hdc);
}

void DoubleBuffer::ReSizeBitMap()
{
	// 삭제하기전에 DC에 연결된 비트맵을 연결 해제해야 할수도 있음
	DeleteObject(_bitMap);
	GetClientRect(_hWnd, &_rect);
	_bitMap = CreateCompatibleBitmap(GetDC(_hWnd), _rect.right, _rect.bottom);
	SelectObject(_memDC, _bitMap);
}

AlertDoubleBuffer::AlertDoubleBuffer(const HWND hWnd) : DoubleBuffer(hWnd)
{
	_oldBrsuh = (HBRUSH)GetStockObject(WHITE_BRUSH);
	_alertBrush = CreateSolidBrush(RGB(255, 0, 0));
	SelectObject(GetMemDC(), _alertBrush);

	_alertOnOff = ALERT::ALERT_OFF;
	_state = COLOR_NORMAL;
}

AlertDoubleBuffer::~AlertDoubleBuffer()
{
	DeleteObject(_alertBrush);
}

AlertDoubleBuffer::ALERT AlertDoubleBuffer::GetAlertOnOff()
{
	return _alertOnOff;
}

void AlertDoubleBuffer::AlertOnOff()
{
	switch (_alertOnOff)
	{
	case ALERT::ALERT_ON:
		_alertOnOff = ALERT::ALERT_OFF;
		break;
	case ALERT::ALERT_OFF:
		_alertOnOff = ALERT::ALERT_ON;
		break;
	}
}

void AlertDoubleBuffer::Alert()
{
	Rectangle(GetMemDC(), _rect.left, _rect.top, _rect.right, _rect.bottom);
}

AlertDoubleBuffer::BACK_STATE AlertDoubleBuffer::GetState()
{
	return _state;
}

void AlertDoubleBuffer::StateChange()
{
	switch (_state)
	{
	case COLOR_NORMAL:
		_state = COLOR_ALERT;
		break;
	case COLOR_ALERT:
		_state = COLOR_NORMAL;
		break;
	}
}
