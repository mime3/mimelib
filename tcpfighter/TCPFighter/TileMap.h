#pragma once
class TileMap
{
private:
	Pos _cameraPos;			// ī�޶� ������ǥ
	int _spriteNo;			// Ÿ�� ��������Ʈ ��ȣ
	BYTE * _map;	// ����� [x][y]
protected:
	// ī�޶� ������ǥ ���
	void SetCameraPos(Pos pos);
public:
	TileMap(int x, int y);
	// Ÿ�� �׸��� (������ ScreenDIB, SpriteDIB�� ����)
	void DrawTile();
	// ī�޶� ������ǥ ����
	Pos GetCameraPos();
};