#ifndef __MINLIB_DOUBLE_BUFFER__
#define __MINLIB_DOUBLE_BUFFER__
#pragma once
#include <Windows.h>
namespace MinLib
{
	class DoubleBuffer
	{
	private:
		HWND	_hWnd;
		HDC		_memDC;
		HBITMAP	_bitMap;

	protected:
	public:
		RECT	_rect;

		DoubleBuffer(const HWND hWnd);
		virtual ~DoubleBuffer();
		inline HDC GetMemDC();
		void Clear();
		void ReSizeBitMap();
		void Flipping();
	};

	class AlertDoubleBuffer : public DoubleBuffer
	{
	public:
		enum ALERT
		{
			ALERT_ON,
			ALERT_OFF
		};
		enum BACK_STATE
		{
			COLOR_NORMAL,
			COLOR_ALERT
		};

	private:
		HBRUSH	_oldBrsuh;
		HBRUSH	_alertBrush;
		ALERT _alertOnOff;
		BACK_STATE _state;

	protected:
	public:
		AlertDoubleBuffer(const HWND hWnd);
		~AlertDoubleBuffer();

		ALERT GetAlertOnOff();
		void AlertOnOff();
		void Alert();
		BACK_STATE GetState();
		void StateChange();
	};
}
#endif // !__MINLIB_DOUBLE_BUFFER__
