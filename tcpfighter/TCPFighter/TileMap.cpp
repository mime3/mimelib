#include "stdafx.h"

extern ScreenDIB * screen;
extern SpriteDIB * sprite;
extern Player * g_player;

//////////////////////////////////////////////////////////////
// TileMap
//////////////////////////////////////////////////////////////

/*----------------------------------------------------------*/
// TileMap::TileMap (public)
// 설명 : 타일맵 생성자
// 인자 : 
/*----------------*////////////////////////*----------------*/
TileMap::TileMap(int x, int y)
{
	_cameraPos = { 0,0 };
	_spriteNo = TILE;
	_map = new BYTE[x * y];
	ZeroMemory(_map, x * y * sizeof(BYTE));
}

/*----------------------------------------------------------*/
// TileMap::DrawTile (public)
// 설명 : 타일을 그린다.
// 인자 : 
/*----------------*////////////////////////*----------------*/
void TileMap::DrawTile()
{
	if (g_player == nullptr)
		return;
	SetCameraPos(g_player->GetPos());
	Pos screenCoordinate = { _cameraPos.x % TILE_X_SIZE, _cameraPos.y % TILE_Y_SIZE };

	for (int i = 0; i < SCREEN_X / TILE_X_SIZE + 1; ++i)
	{
		for (int j = 0; j < SCREEN_Y / TILE_Y_SIZE + 2; ++j)
		{
			sprite->DrawImage(*(_map + (i * (SCREEN_X / TILE_X_SIZE)) + j ) + _spriteNo,
				TILE_X_SIZE * i - screenCoordinate.x,
				TILE_Y_SIZE * j - screenCoordinate.y,
				screen->GetByteBuffer(), 
				SCREEN_X,
				SCREEN_Y,
				SCREEN_X * 4);
		}
	}
}

/*----------------------------------------------------------*/
// TileMap::SetCameraPos (public)
// 설명 : 카메라 월드좌표를 설정한다.
// 인자 : (Pos) 화면좌표에서 중심이 되어야 할 좌표 구조체
/*----------------*////////////////////////*----------------*/
void TileMap::SetCameraPos(Pos pos)
{
	_cameraPos.x = pos.x - SCREEN_X / 2;
	_cameraPos.y = pos.y - SCREEN_Y / 2;

	if (_cameraPos.x < 0)
		_cameraPos.x = 0;
	if(MAP_X_SIZE - SCREEN_X < _cameraPos.x)
		_cameraPos.x = MAP_X_SIZE - SCREEN_X;
	if (_cameraPos.y < 0)
		_cameraPos.y = 0;
	if (MAP_Y_SIZE - SCREEN_Y < _cameraPos.y)
		_cameraPos.y = MAP_Y_SIZE - SCREEN_Y;
}

/*----------------------------------------------------------*/
// TileMap::GetCameraPos (public)
// 설명 : 현재 카메라 월드좌표 구조체를 리턴한다.
// 인자 : 
/*----------------*////////////////////////*----------------*/
Pos TileMap::GetCameraPos()
{
	return _cameraPos;
}