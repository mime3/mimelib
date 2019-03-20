#pragma once

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