//#include "stdafx.h"
//#include "pch.h"
#include "BMPLoader.h"
namespace MinLib
{
	inline COLORREF BMPLoader::GetColor(const int x, const int y) const
	{
		int pos = ((_width * (_infoHeader.biBitCount / 8) + 3) & ~3) * (_height - y - 1) + x * (_infoHeader.biBitCount / 8);
		COLORREF color;
		memcpy_s(&color, sizeof(color), _bitmapPointer + pos, _infoHeader.biBitCount / 8);
		return color;
	}

	inline void BMPLoader::SetColor(const int x, const int y, const COLORREF color) const
	{
		int pos = ((_width * (_infoHeader.biBitCount / 8) + 3) & ~3) * (_height - y - 1) + x * (_infoHeader.biBitCount / 8);
		BYTE* posPointer = _bitmapPointer + pos;
		switch (_infoHeader.biBitCount)
		{
		case 24:
			*posPointer = GetBValue(color);
			*(posPointer + 1) = GetGValue(color);
			*(posPointer + 2) = GetRValue(color);
			break;
		case 32:
			*(COLORREF*)posPointer = color;
			break;
		default:
			break;
		}
	}

	BMPLoader::BMPLoader() : _bitmapPointer(nullptr), _width(0), _height(0), _size(0)
	{
	}

	BMPLoader::BMPLoader(const BMPLoader& bmploader)
	{
		_infoHeader = bmploader._infoHeader;
		_width = bmploader._width;
		_height = bmploader._height;
		_size = bmploader._size;
		_bitmapPointer = (BYTE*)malloc(_size);
		//_bitmapPointer = new BYTE[_size];
		memcpy_s(_bitmapPointer, _size, bmploader._bitmapPointer, bmploader._size);
	}

	BMPLoader::~BMPLoader()
	{
		if (_bitmapPointer != nullptr)
		{
			//free(_bitmapPointer);
			//delete[] _bitmapPointer;
			_bitmapPointer = nullptr;
		}
	}

	bool BMPLoader::LoadBMP(const CHAR* BMPFileName)
	{
		if (_bitmapPointer != nullptr)
		{
			free(_bitmapPointer);
			_bitmapPointer = nullptr;
		}

		FILE* file;

		INT errNo = fopen_s(&file, BMPFileName, "rb");
		if (errNo != 0)
			return false;

		BITMAPFILEHEADER fileHeader;
		ZeroMemory(&fileHeader, sizeof(fileHeader));
		ZeroMemory(&_infoHeader, sizeof(_infoHeader));

		fread_s(&fileHeader, sizeof(fileHeader), sizeof(fileHeader), 1, file);
		if (fileHeader.bfType != TYPE_BITMAP)
		{
			fclose(file);
			return false;
		}

		fread_s(&_infoHeader, sizeof(_infoHeader), sizeof(_infoHeader), 1, file);
		_width = _infoHeader.biWidth;
		_height = _infoHeader.biHeight;

		// 4바이트로 정렬된 상태를 고려한 size계산
		_size = ((_infoHeader.biWidth * (_infoHeader.biBitCount / 8) + 3) & ~3) * _infoHeader.biHeight;
		_bitmapPointer = (BYTE*)malloc(_size);
		//_bitmapPointer = new BYTE[_size];
		fread_s(_bitmapPointer, _size, _size, 1, file);
		fclose(file);
		return true;
	}

	BMPLoader BMPLoader::operator+(const BMPLoader& bmpLoader)
	{
		//if (_size != bmpLoader._size || _width != bmpLoader._width || _height != bmpLoader._height)
		//	return *this;

		BMPLoader* copyBMPLoader = new BMPLoader(*this);
		COLORREF color1, color2, resultColor;
		int r, g, b;
		for (int i = 0; i < _height; ++i)
		{
			if (bmpLoader._infoHeader.biHeight - 1 < i)
				continue;

			for (int j = 0; j < _width; ++j)
			{
				if (bmpLoader._infoHeader.biWidth - 1 < j)
					break;

				color1 = GetColor(j, i);
				color2 = bmpLoader.GetColor(j, i);

				r = min(255, (GetRValue(color1) + GetRValue(color2)) / 2);
				g = min(255, (GetGValue(color1) + GetGValue(color2)) / 2);
				b = min(255, (GetBValue(color1) + GetBValue(color2)) / 2);

				resultColor = RGB(r, g, b);
				copyBMPLoader->SetColor(j, i, resultColor);
			}
		}
		return *copyBMPLoader;
	}
	void BMPLoader::DrawBMPtoHDC(HDC hdc, RECT rect)
	{
		SetStretchBltMode(hdc, COLORONCOLOR);
		StretchDIBits(hdc, 0, 0, rect.right, rect.bottom, 0, 0, _infoHeader.biWidth, _infoHeader.biHeight, _bitmapPointer, (BITMAPINFO*)&_infoHeader, DIB_RGB_COLORS, SRCCOPY);
	}
}