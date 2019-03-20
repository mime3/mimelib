#pragma once
class SpriteDIB
{
public:
public:
	enum
	{
		TYPE_BITMAP = 0x4d42 // �Ϲ����� ��Ʈ�� �ĺ��ڵ�
	};
	struct SPRITE
	{
		BYTE *byteImage;	// ��������Ʈ �̹��� �ּ� ������
		int	width;			// ��������Ʈ �̹��� ����
		int height;			// ��������Ʈ �̹��� ����
		int pitch;			// ��������Ʈ �̹��� ��ġ
	
		int centerPointX;	// ��������Ʈ �̹��� �߽ɺ��� x��
		int centerPointY;	// ��������Ʈ �̹��� �߽ɺ��� y��
	};
private:
protected:
	SPRITE *_spritePointer; // ��������Ʈ ������ �迭�� ����ų ������
	int		_maxSprite;		// ��������Ʈ �ִ� �� (�迭ũ��)
	DWORD	_colorKey;		// �÷�Ű
public:
	SpriteDIB(int maxSprite, DWORD colorKey);
	virtual ~SpriteDIB();
	// �̹����� ��������Ʈ�� ����
	bool LoadDIBSprite(int spriteIndex, const char *fileName, int centerPointX, int centerPointY);
	// ��������Ʈ ����
	void ReleaseDIB(int spriteIndex);
	// �÷�Ű ó��
	void DrawSprite(int spriteIndex, int drawX, int drawY, BYTE *dest, int destWidth, int destHeight, int destPitch, int drawRate = 100);
	void DrawSpriteRed(int spriteIndex, int drawX, int drawY, BYTE *dest, int destWidth, int destHeight, int destPitch, int drawRate = 100);
	void DrawSpriteHalfAlpha(int spriteIndex, int drawX, int drawY, BYTE *dest, int destWidth, int destHeight, int destPitch, int drawRate = 100);
	// �÷�Ű ��ó��
	void DrawImage(int spriteIndex, int drawX, int drawY, BYTE *dest, int destWidth, int destHeight, int destPitch, int drawRate = 100);
};