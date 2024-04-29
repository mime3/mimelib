#pragma once
#ifndef __MINLIB_BMP_LOADER__
#define __MINLIB_BMP_LOADER__
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

namespace MinLib
{
	class BMPLoader
	{
	public:
		enum
		{
			TYPE_BITMAP = 0x4d42
		};
	
	private:
		BITMAPINFOHEADER	_infoHeader;
		BYTE				*_bitmapPointer;
		INT					_width;
		INT					_height;
		INT					_size;
	
	protected:
		COLORREF GetColor(const int x, const int y) const;
		void SetColor(const int x, const int y, const COLORREF color) const;
	
	public:
		BMPLoader();
		BMPLoader(const BMPLoader & bmploader);	
		~BMPLoader();
		bool LoadBMP(const CHAR *BMPFileName);
		BMPLoader operator + (const BMPLoader & bmpLoader);
		
		void DrawBMPtoHDC(HDC hdc, RECT rect);
	};
}
#endif