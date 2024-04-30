#pragma once
#define BLOCKSIZE 20
#define MAP_X_SIZE 60
#define MAP_Y_SIZE 30

enum MAP_STATUS
{
	NORMAL,
	START,
	END,
	WALL
};

struct MAP_NODE
{
	int x;
	int y;
	int g; // 출발점과의 거리
	int h; // 목적지까지의 거리
	int f; // 총합
	MAP_NODE * parent;
};

struct Less
{
	bool operator () (MAP_NODE * one, MAP_NODE * two) const
	{
		return one->f < two->f;
	}
};

//char map[MAP_Y_SIZE][MAP_X_SIZE];
//char *start;
//char *end;
//bool click;
//MAP_STATUS select;
//std::list<MAP_NODE*> openList;
//std::list<MAP_NODE*> closeList;

