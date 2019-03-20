#include "stdafx.h"

SpriteDIB::SpriteDIB(int maxSprite, DWORD colorKey)
{
	_maxSprite = maxSprite;
	_colorKey = colorKey;

	_spritePointer = new SPRITE[_maxSprite];
}

SpriteDIB::~SpriteDIB()
{
	for (int i = 0; i < _maxSprite; ++i)
	{
		ReleaseDIB(i);
	}
	delete[] _spritePointer;
}

bool SpriteDIB::LoadDIBSprite(int spriteIndex, const char * fileName, int centerPointX, int centerPointY)
{
	if (_maxSprite < spriteIndex)
		return false;
		
	FILE *file;
	int errNo = fopen_s(&file, fileName, "rb");
	if (errNo != 0)
		return false;

	BITMAPFILEHEADER bitMapFileHeader;
	BITMAPINFOHEADER bitMapInfo;

	fread_s(&bitMapFileHeader, sizeof(bitMapFileHeader), sizeof(bitMapFileHeader), 1, file);
	if (bitMapFileHeader.bfType != TYPE_BITMAP)
	{
		fclose(file);
		return false;
	}
	fread_s(&bitMapInfo, sizeof(bitMapInfo), sizeof(bitMapInfo), 1, file);
	_spritePointer[spriteIndex].centerPointX = centerPointX;
	_spritePointer[spriteIndex].centerPointY = centerPointY;
	_spritePointer[spriteIndex].width = bitMapInfo.biWidth;
	_spritePointer[spriteIndex].height = bitMapInfo.biHeight;
	_spritePointer[spriteIndex].pitch = (bitMapInfo.biWidth * (bitMapInfo.biBitCount / 8) + 3) & ~3;
	int size = _spritePointer[spriteIndex].pitch * _spritePointer[spriteIndex].height;
	_spritePointer[spriteIndex].byteImage = new BYTE[size];

	// Use more Buffer Ver
	BYTE * buffer = new BYTE[size];
	fread_s(buffer, size, size, 1, file);
	for (int y = 0; y < _spritePointer[spriteIndex].height; ++y)
	{
		memcpy_s(_spritePointer[spriteIndex].byteImage + (_spritePointer[spriteIndex].pitch * y), 
			_spritePointer[spriteIndex].pitch, 
			buffer + (_spritePointer[spriteIndex].pitch * (_spritePointer[spriteIndex].height - y - 1)), 
			_spritePointer[spriteIndex].pitch);
	}
	delete[] buffer;

	//// Use fseek Ver;	// 끝에 이상한 값이 더해져있는경우가 있어서 위험
	//fseek(file, 0, SEEK_END);
	//for (int y = 0; y < _spritePointer[spriteIndex].height; ++y)
	//{
	//	fseek(file, (-1) * _spritePointer[spriteIndex].pitch, SEEK_CUR);
	//	fread_s(_spritePointer[spriteIndex].byteImage + (y * _spritePointer[spriteIndex].pitch), 
	//			_spritePointer[spriteIndex].pitch, _spritePointer[spriteIndex].pitch, 1, file);
	//	fseek(file, (-1) * _spritePointer[spriteIndex].pitch, SEEK_CUR);
	//}

	fclose(file);
	return true;
}

void SpriteDIB::ReleaseDIB(int spriteIndex)
{
	delete[] _spritePointer[spriteIndex].byteImage;
}

void SpriteDIB::DrawSprite(int spriteIndex, int drawX, int drawY, BYTE * dest, int destWidth, int destHeight, int destPitch, int drawRate)
{
	if (_maxSprite < spriteIndex)
		return;

	drawX -= _spritePointer[spriteIndex].centerPointX;
	drawY -= _spritePointer[spriteIndex].centerPointY;

	int readWidth = _spritePointer[spriteIndex].width;
	int readHeight = _spritePointer[spriteIndex].height;
	int pitch = _spritePointer[spriteIndex].pitch;
	BYTE * imagePointer = _spritePointer[spriteIndex].byteImage;
	BYTE * pitchPointer = imagePointer;

	readWidth = readWidth * drawRate / 100;
	//readHeight = readHeight * drawRate / 100;

	if (drawX < 0)
	{
		drawX *= -1;
		readWidth -= drawX;
		imagePointer += drawX * sizeof(DWORD);
		pitchPointer = imagePointer;
		drawX = 0;
	}
	if (destWidth < drawX + _spritePointer[spriteIndex].width)
	{
		readWidth -= drawX + _spritePointer[spriteIndex].width - destWidth;
	}
	if (drawY < 0)
	{
		drawY *= -1;
		readHeight -= drawY;
		imagePointer += drawY * pitch;
		pitchPointer = imagePointer;
		drawY = 0;
	}
	if (destHeight < drawY + _spritePointer[spriteIndex].height)
	{
		readHeight -= drawY + _spritePointer[spriteIndex].height - destHeight;
	}

	dest += drawX * sizeof(DWORD);
	dest += drawY * destPitch;

	for (int y = 0; y < readHeight; ++y)
	{
		for (int x = 0; x < readWidth; ++x)
		{
			*(DWORD*)imagePointer |= 0xff000000;
			if (*(DWORD*)imagePointer != _colorKey)
				*(DWORD*)dest = *(DWORD*)imagePointer;
			dest += sizeof(DWORD);
			imagePointer += sizeof(DWORD);
		}
		dest += destPitch - readWidth * 4;
		pitchPointer += pitch; 
		imagePointer = pitchPointer;
	}
}

void SpriteDIB::DrawSpriteRed(int spriteIndex, int drawX, int drawY, BYTE * dest, int destWidth, int destHeight, int destPitch, int drawRate)
{
	if (_maxSprite < spriteIndex)
		return;

	drawX -= _spritePointer[spriteIndex].centerPointX;
	drawY -= _spritePointer[spriteIndex].centerPointY;

	int readWidth = _spritePointer[spriteIndex].width;
	int readHeight = _spritePointer[spriteIndex].height;
	int pitch = _spritePointer[spriteIndex].pitch;
	BYTE * imagePointer = _spritePointer[spriteIndex].byteImage;
	BYTE * pitchPointer = imagePointer;

	readWidth = readWidth * drawRate / 100;
	//readHeight = readHeight * drawRate / 100;

	if (drawX < 0)
	{
		drawX *= -1;
		readWidth -= drawX;
		imagePointer += drawX * sizeof(DWORD);
		pitchPointer = imagePointer;
		drawX = 0;
	}
	if (destWidth < drawX + _spritePointer[spriteIndex].width)
	{
		readWidth -= drawX + _spritePointer[spriteIndex].width - destWidth;
	}
	if (drawY < 0)
	{
		drawY *= -1;
		readHeight -= drawY;
		imagePointer += drawY * pitch;
		pitchPointer = imagePointer;
		drawY = 0;
	}
	if (destHeight < drawY + _spritePointer[spriteIndex].height)
	{
		readHeight -= drawY + _spritePointer[spriteIndex].height - destHeight;
	}

	dest += drawX * sizeof(DWORD);
	dest += drawY * destPitch;

	for (int y = 0; y < readHeight; ++y)
	{
		for (int x = 0; x < readWidth; ++x)
		{
			*(DWORD*)imagePointer |= 0xff000000; //ABGR
			if (*(DWORD*)imagePointer != _colorKey)
			{
				*dest = *imagePointer / 2;				//B
				*(dest + 1) = *(imagePointer + 1) / 2;	//G
				*(dest + 2) = *(imagePointer + 2);		//R
				*(dest + 3) = *(imagePointer + 3);		//A
			}
			dest += sizeof(DWORD);
			imagePointer += sizeof(DWORD);
		}
		dest += destPitch - readWidth * 4;
		pitchPointer += pitch;
		imagePointer = pitchPointer;
	}
}

void SpriteDIB::DrawSpriteHalfAlpha(int spriteIndex, int drawX, int drawY, BYTE * dest, int destWidth, int destHeight, int destPitch, int drawRate)
{
	if (_maxSprite < spriteIndex)
		return;

	drawX -= _spritePointer[spriteIndex].centerPointX;
	drawY -= _spritePointer[spriteIndex].centerPointY;

	int readWidth = _spritePointer[spriteIndex].width;
	int readHeight = _spritePointer[spriteIndex].height;
	int pitch = _spritePointer[spriteIndex].pitch;
	BYTE * imagePointer = _spritePointer[spriteIndex].byteImage;
	BYTE * pitchPointer = imagePointer;

	readWidth = readWidth * drawRate / 100;
	//readHeight = readHeight * drawRate / 100;

	if (drawX < 0)
	{
		drawX *= -1;
		readWidth -= drawX;
		imagePointer += drawX * sizeof(DWORD);
		pitchPointer = imagePointer;
		drawX = 0;
	}
	if (destWidth < drawX + _spritePointer[spriteIndex].width)
	{
		readWidth -= drawX + _spritePointer[spriteIndex].width - destWidth;
	}
	if (drawY < 0)
	{
		drawY *= -1;
		readHeight -= drawY;
		imagePointer += drawY * pitch;
		pitchPointer = imagePointer;
		drawY = 0;
	}
	if (destHeight < drawY + _spritePointer[spriteIndex].height)
	{
		readHeight -= drawY + _spritePointer[spriteIndex].height - destHeight;
	}

	dest += drawX * sizeof(DWORD);
	dest += drawY * destPitch;

	for (int y = 0; y < readHeight; ++y)
	{
		for (int x = 0; x < readWidth; ++x)
		{
			*(DWORD*)imagePointer |= 0xff000000;
			if (*(DWORD*)imagePointer != _colorKey)
			{
				*dest = (*imagePointer + *dest )/ 2;					//B
				*(dest + 1) = *(imagePointer + 1) + *(dest + 1) / 2;	//G
				*(dest + 2) = *(imagePointer + 2) + *(dest + 2) / 2;	//R
				*(dest + 3) = *(imagePointer + 3) + *(dest + 3) / 2;	//A
			}
			dest += sizeof(DWORD);
			imagePointer += sizeof(DWORD);
		}
		dest += destPitch - readWidth * 4;
		pitchPointer += pitch;
		imagePointer = pitchPointer;
	}
}

void SpriteDIB::DrawImage(int spriteIndex, int drawX, int drawY, BYTE * dest, int destWidth, int destHeight, int destPitch, int drawRate)
{
	if (_maxSprite < spriteIndex)
		return;

	drawX -= _spritePointer[spriteIndex].centerPointX;
	drawY -= _spritePointer[spriteIndex].centerPointY;

	int readWidth = _spritePointer[spriteIndex].width;
	int readHeight = _spritePointer[spriteIndex].height;
	int pitch = _spritePointer[spriteIndex].pitch;
	BYTE * imagePointer = _spritePointer[spriteIndex].byteImage;
	BYTE * pitchPointer = imagePointer;

	readWidth = readWidth * drawRate / 100;
	readHeight = readHeight * drawRate / 100;

	if (drawX < 0)
	{
		readWidth = _spritePointer[spriteIndex].width + drawX;
		imagePointer -= drawX;
	}
	if (destWidth < drawX)
	{
		readWidth = drawX - destWidth;
	}
	if (drawY < 0)
	{
		readHeight = _spritePointer[spriteIndex].height + drawY;
		imagePointer -= drawY * readHeight;
	}
	if (destHeight < drawY)
	{
		readHeight = drawY - destHeight;
	}

	dest += drawX * sizeof(DWORD);
	dest += drawY * destPitch;

	for (int y = 0; y < readHeight; ++y)
	{
		memcpy_s(dest, destPitch, imagePointer, readWidth * 4);
		dest += destPitch;
		pitchPointer += pitch;
		imagePointer = pitchPointer;
	}
}
