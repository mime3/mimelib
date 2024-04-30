#pragma once
class TileMap
{
private:
	Pos _cameraPos;			// 카메라 월드좌표
	int _spriteNo;			// 타일 스프라이트 번호
	BYTE * _map;	// 월드맵 [x][y]
protected:
	// 카메라 월드좌표 계산
	void SetCameraPos(Pos pos);
public:
	TileMap(int x, int y);
	// 타일 그리기 (전역의 ScreenDIB, SpriteDIB에 종속)
	void DrawTile();
	// 카메라 월드좌표 리턴
	Pos GetCameraPos();
};