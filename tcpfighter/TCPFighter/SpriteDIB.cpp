#include "stdafx.h"
//////////////////////////////////////////////////////////////
// SpriteDIB
//////////////////////////////////////////////////////////////

/*----------------------------------------------------------*/
// SpriteDIB::SpriteDIB (public)
// 설명 : SpriteDIB 생성자
// 인자 : (int)저장할 최대 스프라이트 수, (DWORD) 컬러키
// 리턴 :
/*----------------*////////////////////////*----------------*/
SpriteDIB::SpriteDIB(int maxSprite, DWORD colorKey)
{
	_maxSprite = maxSprite;
	_colorKey = colorKey;
	// 스프라이트 정보를 저장할 동적배열생성
	_spritePointer = new SPRITE[_maxSprite];
}

/*----------------------------------------------------------*/
// SpriteDIB::~SpriteDIB (public)
// 설명 : SpriteDIB 소멸자
//  ㄴ  : 저장된 모든 스프라이트에 대해 ReleaseDIB 호출
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
SpriteDIB::~SpriteDIB()
{
	for (int i = 0; i < _maxSprite; ++i)
	{
		ReleaseDIB(i);
	}
	delete[] _spritePointer;
}

/*----------------------------------------------------------*/
// SpriteDIB::LoadDIBSprite (public)
// 설명 : 스프라이트 저장
// 인자 : (int) 스프라이트 번호, (const char *) 파일경로, 
//  ㄴ  : (int) 이미지 중심보정 x값, (int) 이미지 중심보정 y값
// 리턴 : (bool) 저장 성공여부
/*----------------*////////////////////////*----------------*/
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

/*----------------------------------------------------------*/
// SpriteDIB::ReleaseDIB (virtual) (private) (protected) (public)
// 설명 : 지정된 스프라이트 해제
// 인자 : (int) 스프라이트 번호
// 리턴 :
/*----------------*////////////////////////*----------------*/
void SpriteDIB::ReleaseDIB(int spriteIndex)
{
	delete[] _spritePointer[spriteIndex].byteImage;
}

/*----------------------------------------------------------*/
// SpriteDIB::DrawSprite (virtual) (private) (protected) (public)
// 설명 : 이미지를 버퍼에 컬러키를 걸러내고 1픽셀씩 그린다.
// 인자 : (int) 스프라이트번호, (int) 그릴 버퍼의x좌표, (int)그릴 버퍼의 y좌표
//  ㄴ  : (BYTE *)그릴 버퍼의 포인터, (int)그릴 버퍼의 넓이, (int)그릴 버퍼의 높이
//  ㄴ  : (int) 그릴 버퍼의 피치, (int) 이미지의 그릴 백분율
// 리턴 :
/*----------------*////////////////////////*----------------*/
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

/*----------------------------------------------------------*/
// SpriteDIB::DrawSpriteRed (virtual) (private) (protected) (public)
// 설명 : 이미지를 버퍼에 컬러키를 걸러내면서 RED비율을 높게 1픽셀씩 그린다.
// 인자 : (int) 스프라이트번호, (int) 그릴 버퍼의x좌표, (int)그릴 버퍼의 y좌표
//  ㄴ  : (BYTE *)그릴 버퍼의 포인터, (int)그릴 버퍼의 넓이, (int)그릴 버퍼의 높이
//  ㄴ  : (int) 그릴 버퍼의 피치, (int) 이미지의 그릴 백분율
// 리턴 :
/*----------------*////////////////////////*----------------*/
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

/*----------------------------------------------------------*/
// SpriteDIB::DrawSpriteHalfAlpha (virtual) (private) (protected) (public)
// 설명 : 이미지를 버퍼에 컬러키를 걸러내고 색값을 1/2하여 반투명하게 1픽셀씩 그린다.
// 인자 : (int) 스프라이트번호, (int) 그릴 버퍼의x좌표, (int)그릴 버퍼의 y좌표
//  ㄴ  : (BYTE *)그릴 버퍼의 포인터, (int)그릴 버퍼의 넓이, (int)그릴 버퍼의 높이
//  ㄴ  : (int) 그릴 버퍼의 피치, (int) 이미지의 그릴 백분율
// 리턴 :
/*----------------*////////////////////////*----------------*/
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

/*----------------------------------------------------------*/
// SpriteDIB::DrawImage (virtual) (private) (protected) (public)
// 설명 : 이미지를 그대로 memcpy한다.
// 인자 : (int) 스프라이트번호, (int) 그릴 버퍼의x좌표, (int)그릴 버퍼의 y좌표
//  ㄴ  : (BYTE *)그릴 버퍼의 포인터, (int)그릴 버퍼의 넓이, (int)그릴 버퍼의 높이
//  ㄴ  : (int) 그릴 버퍼의 피치, (int) 이미지의 그릴 백분율
// 리턴 :
/*----------------*////////////////////////*----------------*/
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
		memcpy_s(dest, destPitch, imagePointer, readWidth * 4);
		dest += destPitch;
		pitchPointer += pitch;
		imagePointer = pitchPointer;
	}
}
