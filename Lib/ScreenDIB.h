#ifndef __MINLIB_SCREEN_DIB__
#define __MINLIB_SCREEN_DIB__
#pragma once
#include <windows.h>
//#include <wingdi.h>
namespace MinLib
{
	class ScreenDIB
	{
	private:
	protected:
		BITMAPINFO	_DIBInfo;
		BYTE* _byteBuffer;

		int _width;
		int _height;
		int _pitch;
		int _colorBit;
		int _bufferSize;

		void CreateDIBBuffer(int width, int height, int colorBit);
		void ReleaseBuffer();

	public:
		ScreenDIB(int width, int height, int colorBit);
		virtual ~ScreenDIB();

		void DrawBuffer(HWND hWnd, int x = 0, int y = 0);
		void Clear();
		int GetWidth();
		int GetHeight();
		int GetPitch();
		BYTE* GetByteBuffer();
	};
}
#endif // !__MINLIB_SCREEN_DIB__