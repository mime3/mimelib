#pragma once
class SpriteDIB
{
public:
public:
	enum
	{
		TYPE_BITMAP = 0x4d42 // 일반적인 비트맵 식별코드
	};
	struct SPRITE
	{
		BYTE *byteImage;	// 스프라이트 이미지 주소 포인터
		int	width;			// 스프라이트 이미지 넓이
		int height;			// 스프라이트 이미지 높이
		int pitch;			// 스프라이트 이미지 피치
	
		int centerPointX;	// 스프라이트 이미지 중심보정 x값
		int centerPointY;	// 스프라이트 이미지 중심보정 y값
	};
private:
protected:
	SPRITE *_spritePointer; // 스프라이트 저장할 배열을 가리킬 포인터
	int		_maxSprite;		// 스프라이트 최대 수 (배열크기)
	DWORD	_colorKey;		// 컬러키
public:
	SpriteDIB(int maxSprite, DWORD colorKey);
	virtual ~SpriteDIB();
	// 이미지를 스프라이트로 저장
	bool LoadDIBSprite(int spriteIndex, const char *fileName, int centerPointX, int centerPointY);
	// 스프라이트 해제
	void ReleaseDIB(int spriteIndex);
	// 컬러키 처리
	void DrawSprite(int spriteIndex, int drawX, int drawY, BYTE *dest, int destWidth, int destHeight, int destPitch, int drawRate = 100);
	void DrawSpriteRed(int spriteIndex, int drawX, int drawY, BYTE *dest, int destWidth, int destHeight, int destPitch, int drawRate = 100);
	void DrawSpriteHalfAlpha(int spriteIndex, int drawX, int drawY, BYTE *dest, int destWidth, int destHeight, int destPitch, int drawRate = 100);
	// 컬러키 미처리
	void DrawImage(int spriteIndex, int drawX, int drawY, BYTE *dest, int destWidth, int destHeight, int destPitch, int drawRate = 100);
};