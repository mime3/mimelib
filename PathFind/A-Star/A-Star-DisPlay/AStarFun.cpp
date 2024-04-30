#include "stdafx.h"

extern DoubleBuffer * db;
extern char map[MAP_Y_SIZE][MAP_X_SIZE];
extern char *start;
extern char *end;
extern bool click;
extern MAP_STATUS select;
extern std::list<MAP_NODE *> openList;
extern std::list<MAP_NODE *> closeList;

char * GetBlock(LPARAM lParam)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	x /= BLOCKSIZE;
	y /= BLOCKSIZE;
	return &map[y][x];
}

void ChangeBlock(char * block)
{
	switch (select)
	{
	case NORMAL:
		if (*block == NORMAL)
			*block = WALL;
		break;
	case START:
		if (*block == NORMAL)
			SetStart(block);
		break;
	case END:
		if (*block == NORMAL)
			SetEnd(block);
		break;
	case WALL:
		if (*block == WALL)
			*block = NORMAL;
		break;
	}
}

void DrawLine(HDC hdc)
{
	SelectObject(hdc, PEN(2));
	for (int i = 0; i < MAP_X_SIZE + 1; ++i)
	{
		MoveToEx(hdc, i * BLOCKSIZE, 0, NULL);
		LineTo(hdc, i * BLOCKSIZE, BLOCKSIZE * MAP_Y_SIZE);
	}

	for (int i = 0; i < MAP_Y_SIZE + 1; ++i)
	{
		MoveToEx(hdc, 0, i * BLOCKSIZE, NULL);
		LineTo(hdc, MAP_X_SIZE * BLOCKSIZE, i * BLOCKSIZE);
	}
}

void DrawBlock(HDC hdc)
{
	// need GDICOLOR 
	SelectObject(hdc, GDICOLOR()->_nullPen);
	for (int y = 0; y < MAP_Y_SIZE; ++y)
	{
		for (int x = 0; x < MAP_X_SIZE; ++x)
		{
			switch (map[y][x])
			{
			case NORMAL:
				break;
			case START:
				SelectObject(hdc, BRUSH(8));
				Rectangle(hdc, x * BLOCKSIZE + 1, y * BLOCKSIZE + 1, (x + 1) * BLOCKSIZE, (y + 1) * BLOCKSIZE);
				break;
			case END:
				SelectObject(hdc, BRUSH(4));
				Rectangle(hdc, x * BLOCKSIZE + 1, y * BLOCKSIZE + 1, (x + 1) * BLOCKSIZE, (y + 1) * BLOCKSIZE);
				break;
			case WALL:
				SelectObject(hdc, BRUSH(1));
				Rectangle(hdc, x * BLOCKSIZE + 1, y * BLOCKSIZE + 1, (x + 1) * BLOCKSIZE, (y + 1) * BLOCKSIZE);
				break;
			default:
				break;
			}
		}
	}
}

void SetStart(char * block)
{
	if (start != nullptr)
		*start = NORMAL;
	*block = START;
	start = block;
}

void SetEnd(char * block)
{
	if (end != nullptr)
		*end = NORMAL;
	*block = END;
	end = block;
}


bool FindAndDisplayPath(MAP_NODE ** endNode, HDC hdc) 
{
	if (start == nullptr || end == nullptr)
		return false;

	int endX;
	int endY;
	if (!FindTile(END, &endX, &endY))
		return false;

	int startX;
	int startY;
	if (!FindTile(START, &startX, &startY))
		return false;

	// 이전에사용한 open close 리스트 비워야함
	ClearNode();
	CreateNode(startX, startY, nullptr, NULL, endX, endY);
	while (true)
	{
		// F 키로 정렬
		openList.sort(Less());
		// 하나 뽑기 -> 뽑을거 없으면 길이 없다
		std::list<MAP_NODE *>::iterator iter = openList.begin();
		if (iter == openList.end())
			return false;
		MAP_NODE * output = (*iter);
		openList.erase(iter);
		// 목적지인지 체크 // 리턴을 이걸로 하자
		if (output->h == 0)
		{
			*endNode = output;
			return true;
		}
		// close에 넣기
		closeList.push_back(output);
		// 주변 노드 만들기
		CreateNode(output->x - 1, output->y, output, 10, endX, endY);
		CreateNode(output->x + 1, output->y, output, 10, endX, endY);
		CreateNode(output->x, output->y - 1, output, 10, endX, endY);
		CreateNode(output->x, output->y + 1, output, 10, endX, endY);
		CreateNode(output->x - 1, output->y + 1, output, 14, endX, endY);
		CreateNode(output->x - 1, output->y - 1, output, 14, endX, endY);
		CreateNode(output->x + 1, output->y + 1, output, 14, endX, endY);
		CreateNode(output->x + 1, output->y - 1, output, 14, endX, endY);

		// 두 리스트 순회하면서 블럭 출력
		DisplayNode(hdc, startX, startY, endX, endY);
		db->Flipping();
	}
}

bool FindPath(MAP_NODE ** endNode)
{
	if (start == nullptr || end == nullptr)
		return false;
	
	int endX;
	int endY;
	if (!FindTile(END, &endX, &endY))
		return false;

	int startX;
	int startY;
	if (!FindTile(START, &startX, &startY))
		return false;

	ClearNode();
	CreateNode(startX, startY, nullptr, NULL, endX, endY);
	while (true)
	{
		// F 키로 정렬
		openList.sort(Less());
		// 하나 뽑기 -> 뽑을거 없으면 길이 없다
		std::list<MAP_NODE *>::iterator iter = openList.begin();
		if ((*iter) == nullptr)
			return false;
		MAP_NODE * output = (*iter);
		openList.erase(iter);
		// 목적지인지 체크 // 리턴을 이걸로 하자
		if (output->h == 0)
		{
			*endNode = output;
			return true;
		}
		// close에 넣기
		closeList.push_back(output);
		// 주변 노드 만들기
		CreateNode(output->x - 1, output->y, output, 10, endX, endY);
		CreateNode(output->x + 1, output->y, output, 10, endX, endY);
		CreateNode(output->x, output->y - 1, output, 10, endX, endY);
		CreateNode(output->x, output->y + 1, output, 10, endX, endY);
		CreateNode(output->x - 1, output->y + 1, output, 14, endX, endY);
		CreateNode(output->x - 1, output->y - 1, output, 14, endX, endY);
		CreateNode(output->x + 1, output->y + 1 , output, 14, endX, endY);
		CreateNode(output->x + 1, output->y - 1, output, 14, endX, endY);
	}

}

bool FindTile(MAP_STATUS status, int * findX, int * findY)
{
	for (int y = 0; y < MAP_Y_SIZE; ++y)
	{
		for (int x = 0; x < MAP_X_SIZE; ++x)
		{
			if (map[y][x] == status)
			{
				*findX = x;
				*findY = y;
				return true;
			}
		}
	}
	return false;
}

void ClearWall()
{
	for (int y = 0; y < MAP_Y_SIZE; ++y)
	{
		for (int x = 0; x < MAP_X_SIZE; ++x)
		{
			if (map[y][x] == WALL)
				map[y][x] = NORMAL;
		}
	}
}

void ClearNode()
{
	std::list<MAP_NODE *>::iterator iter = openList.begin();
	for (;iter != openList.end();)
	{
		delete (*iter);
		iter = openList.erase(iter);
	}	
	iter = closeList.begin();
	for (; iter != closeList.end();)
	{
		delete (*iter);
		iter = closeList.erase(iter);
	}
}

void DisplayNode(HDC hdc, int startX, int startY, int endX, int endY)
{
	std::list<MAP_NODE *>::iterator iter;
	SelectObject(hdc, BRUSH(12));
	for (iter = openList.begin(); iter != openList.end(); ++iter)
	{
		if ((*iter)->x != endX || (*iter)->y != endY)
			Rectangle(hdc, (*iter)->x * BLOCKSIZE, (*iter)->y * BLOCKSIZE, ((*iter)->x + 1) * BLOCKSIZE, ((*iter)->y + 1) * BLOCKSIZE);
	}
	SelectObject(hdc, BRUSH(7));
	for (iter = closeList.begin(); iter != closeList.end(); ++iter)
	{
		if ((*iter)->x != startX || (*iter)->y != startY)
			Rectangle(hdc, (*iter)->x * BLOCKSIZE, (*iter)->y * BLOCKSIZE, ((*iter)->x + 1) * BLOCKSIZE, ((*iter)->y + 1) * BLOCKSIZE);
	}
}

bool CreateNode(int x, int y, MAP_NODE * parent, int plus, int endX, int endY)
{
	// 아웃좌표 검사
	if (x < 0 || MAP_X_SIZE - 1 < x)
		return false;
	if (y < 0 || MAP_Y_SIZE - 1 < y)
		return false;
	//	벽 검사
	if (map[y][x] == WALL)
		return false;
	//	노드 중복검사open
	std::list<MAP_NODE *>::iterator iter = openList.begin();
	for (; iter != openList.end(); ++iter)
	{
		if ((*iter)->x == x && (*iter)->y == y)
		{
			//	g값이 내가 만들려는것보다 크면 빙 돌아온거니까 g,f와 부모를 바꿔줘야한다.
			if (parent->g + plus < (*iter)->g)
			{
				(*iter)->g = parent->g + plus;
				(*iter)->f = parent->g + (*iter)->h;
				(*iter)->parent = parent;
			}
			return false;
		}
	}
	//	노드 중복검사close
	iter = closeList.begin();
	for (; iter != closeList.end(); ++iter)
	{
		if ((*iter)->x == x && (*iter)->y == y)
			return false;
	}

	MAP_NODE * newNode = new MAP_NODE;
	newNode->x = x;
	newNode->y = y;
	if (parent == nullptr)
		newNode->g = 0;
	else
		newNode->g = parent->g + plus;
	newNode->h = (max(abs(endX - x), abs(endY - y)) * 10)
		- (min(abs(endX - x), abs(endY - y)) * 10)
		+ (min(abs(endX - x), abs(endY - y)) * 14);
	newNode->f = newNode->g + newNode->h;
	newNode->parent = parent;
	openList.push_back(newNode);
	return true;
}
