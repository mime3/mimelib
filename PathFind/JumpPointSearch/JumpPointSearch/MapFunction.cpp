#include "stdafx.h"
extern char g_map[MAP_Y_SIZE][MAP_X_SIZE];
extern JPS::MAP_STATUS select;
extern bool click;
extern char * g_start;
extern char * g_end;

char * GetBlock(LPARAM lParam)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	x /= BLOCKSIZE;
	y /= BLOCKSIZE;
	return &g_map[y][x];
}

void SetStart(char * block)
{
	if (g_start != nullptr)
		*g_start = JPS::MAP_STATUS::NORMAL;
	*block = JPS::MAP_STATUS::START;
	g_start = block;
}

void SetEnd(char * block)
{
	if (g_end != nullptr)
		*g_end = JPS::MAP_STATUS::NORMAL;
	*block = JPS::MAP_STATUS::END;
	g_end = block;
}

void ChangeBlock(char * block)
{
	switch (select)
	{
	case JPS::MAP_STATUS::NORMAL:
		if (*block == JPS::MAP_STATUS::NORMAL)
			*block = JPS::MAP_STATUS::WALL;
		break;
	case JPS::MAP_STATUS::START:
		if (*block == JPS::MAP_STATUS::NORMAL)
			SetStart(block);
		break;
	case JPS::MAP_STATUS::END:
		if (*block == JPS::MAP_STATUS::NORMAL)
			SetEnd(block);
		break;
	case JPS::MAP_STATUS::WALL:
		if (*block == JPS::MAP_STATUS::WALL)
			*block = JPS::MAP_STATUS::NORMAL;
		break;
	}
}

void DeleteWall()
{
	for (int y = 0; y < MAP_Y_SIZE; ++y)
	{
		for (int x = 0; x < MAP_X_SIZE; ++x)
		{
			if (g_map[y][x] == JPS::MAP_STATUS::WALL)
				g_map[y][x] = JPS::MAP_STATUS::NORMAL;
		}
	}
}
