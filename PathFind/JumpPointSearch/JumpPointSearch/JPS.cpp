#include "stdafx.h"
#include "JPS.h"

using namespace MinLib;

void JPS::DrawLine()
{
	SelectObject(_doubleBuffer->GetMemDC(), PEN(2));
	for (int i = 0; i < _mapWidth + 1; ++i)
	{
		MoveToEx(_doubleBuffer->GetMemDC(), i * BLOCKSIZE, 0, NULL);
		LineTo(_doubleBuffer->GetMemDC(), i * BLOCKSIZE, BLOCKSIZE * _mapDepth);
	}

	for (int i = 0; i < _mapDepth + 1; ++i)
	{
		MoveToEx(_doubleBuffer->GetMemDC(), 0, i * BLOCKSIZE, NULL);
		LineTo(_doubleBuffer->GetMemDC(), _mapWidth * BLOCKSIZE, i * BLOCKSIZE);
	}
}

void JPS::DrawBlock()
{	
	for (int y = 0; y < _mapDepth; ++y)
	{
		for (int x = 0; x < _mapWidth; ++x)
		{
			MAP_STATUS status;
			FindTile(&status, x, y);
			HDC memDC = _doubleBuffer->GetMemDC();
			switch (status)
			{
			case NORMAL:
				break;
			case START:
				SelectObject(memDC, BRUSH(8));
				SelectObject(memDC, PEN(0));
				Rectangle(memDC, x * BLOCKSIZE + 1, y * BLOCKSIZE + 1, (x + 1) * BLOCKSIZE, (y + 1) * BLOCKSIZE);
				break;
			case END:
				SelectObject(memDC, BRUSH(4));
				SelectObject(memDC, PEN(0));
				Rectangle(memDC, x * BLOCKSIZE + 1, y * BLOCKSIZE + 1, (x + 1) * BLOCKSIZE, (y + 1) * BLOCKSIZE);
				break;
			case WALL:
				SelectObject(memDC, BRUSH(1));
				SelectObject(memDC, GDICOLOR()->_nullPen);
				Rectangle(memDC, x * BLOCKSIZE + 1, y * BLOCKSIZE + 1, (x + 1) * BLOCKSIZE, (y + 1) * BLOCKSIZE);
				break;
			default:
				break;
			}
		}
	}
}

void JPS::DrawList()
{ 
	HDC memDC = _doubleBuffer->GetMemDC();
	std::list<MAP_NODE *>::iterator iter = _openList.begin();
	SelectObject(memDC, PEN(0));
	SelectObject(memDC, BRUSH(7));
	for (; iter != _openList.end(); ++iter)
	{
		Rectangle(memDC, (*iter)->x * BLOCKSIZE + 1, (*iter)->y * BLOCKSIZE + 1, ((*iter)->x + 1) * BLOCKSIZE, ((*iter)->y + 1) * BLOCKSIZE);
	}
	SelectObject(memDC, BRUSH(9));
	for (iter = _closeList.begin(); iter != _closeList.end(); ++iter)
	{
		Rectangle(memDC, (*iter)->x * BLOCKSIZE + 1, (*iter)->y * BLOCKSIZE + 1, ((*iter)->x + 1) * BLOCKSIZE, ((*iter)->y + 1) * BLOCKSIZE);
	}
	std::list<FOUND_NODE *>::iterator iter2 = _foundList.begin();
	SelectObject(memDC, GDICOLOR()->_nullPen);
	for (; iter2 != _foundList.end(); ++iter2)
	{
		HBRUSH brush = CreateSolidBrush((*iter2)->color);
		SelectObject(memDC, brush);
		Rectangle(memDC, (*iter2)->x * BLOCKSIZE + 1, (*iter2)->y * BLOCKSIZE + 1, ((*iter2)->x + 1) * BLOCKSIZE, ((*iter2)->y + 1) * BLOCKSIZE);
		SelectObject(memDC, GDICOLOR()->_nullBrush);
		DeleteObject(brush);
	}
}

bool JPS::FindTile(MAP_STATUS status, int * findX, int * findY)
{
	char * target = _mapPointer;
	for (int i = 0; i < _mapWidth * _mapDepth; ++i)
	{
		if (*target == status)
		{
			*findX = i % _mapWidth;
			*findY = i / _mapWidth;
			return true;
		}
		target++;
	}
	return false;
}

bool JPS::FindTile(MAP_STATUS * status, int findX, int findY)
{
	if (findX < 0 || _mapWidth - 1 < findX)
		return false;
	if (findY < 0 || _mapDepth - 1 < findY)
		return false;

	*status = (MAP_STATUS)*(_mapPointer + findY * _mapWidth + findX);
	return true;
}

void JPS::ClearNode()
{
	std::list<MAP_NODE *>::iterator iter = _openList.begin();
	for (; iter != _openList.end();)
	{
		delete (*iter);
		iter = _openList.erase(iter);
	}
	iter = _closeList.begin();
	for (; iter != _closeList.end();)
	{
		delete (*iter);
		iter = _closeList.erase(iter);
	}

	std::list<FOUND_NODE *>::iterator iter2 = _foundList.begin();
	for (; iter2 != _foundList.end();)
	{
		delete (*iter2);
		iter2 = _foundList.erase(iter2);
	}
}

bool JPS::CreateNode(int x, int y, MAP_NODE * parent, int plus, int endX, int endY)
{
	// 아웃좌표 검사
	if (x < 0 || _mapWidth - 1 < x)
		return false;
	if (y < 0 || _mapDepth - 1 < y)
		return false;
	//	벽 검사
	MAP_STATUS status;
	FindTile(&status, x, y);
	if (status == WALL)
		return false;
	//	노드 중복검사open
	std::list<MAP_NODE *>::iterator iter = _openList.begin();
	for (; iter != _openList.end(); ++iter)
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
	iter = _closeList.begin();
	for (; iter != _closeList.end(); ++iter)
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
	_openList.push_back(newNode);
	return true;
}

bool JPS::CreateNode(int x, int y, MAP_NODE * parent, Direction direction)
{
	// 아웃좌표 검사
	if (x < 0 || _mapWidth - 1 < x)
		return false;
	if (y < 0 || _mapDepth - 1 < y)
		return false;
	//	벽 검사
	MAP_STATUS status;
	FindTile(&status, x, y);
	if (status == WALL)
		return false;

	//int nextG = (max(abs(_startX - x), abs(_startY - y)) * 10)
	//	- (min(abs(_startX - x), abs(_startY - y)) * 10)
	//	+ (min(abs(_startX - x), abs(_startY - y)) * 14);
	int px = parent->x;
	int py = parent->y;

	int nextG = (max(abs(px - x), abs(py - y)) * 10)
		- (min(abs(px - x), abs(py - y)) * 10)
		+ (min(abs(px - x), abs(py - y)) * 14);
	nextG += parent->g;

	//	노드 중복검사open
	std::list<MAP_NODE *>::iterator iter = _openList.begin();
	for (; iter != _openList.end(); ++iter)
	{
		if ((*iter)->x == x && (*iter)->y == y)
		{
			//	g값이 내가 만들려는것보다 크면 빙 돌아온거니까 g,f와 부모를 바꿔줘야한다.
			if (nextG < (*iter)->g)
			{
				(*iter)->g = nextG;
				(*iter)->f = nextG + (*iter)->h;
				(*iter)->parent = parent;
			}
			return false;
		}
	}
	//	노드 중복검사close
	iter = _closeList.begin();
	for (; iter != _closeList.end(); ++iter)
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
		newNode->g = nextG;
	newNode->h = (max(abs(_endX - x), abs(_endY - y)) * 10)
		- (min(abs(_endX - x), abs(_endY - y)) * 10)
		+ (min(abs(_endX - x), abs(_endY - y)) * 14);
	newNode->f = newNode->g + newNode->h;
	newNode->parent = parent;
	newNode->direction = direction;
	_openList.push_back(newNode);
	return true;
}

void JPS::NeighborsNode(MAP_NODE * node)
{
	int x = node->x;
	int y = node->y;
	if (node->parent == nullptr)
	{
		CheckPoint(node, x - 1, y - 1, LU);
		CheckPoint(node, x, y - 1, UU);
		CheckPoint(node, x + 1, y - 1, RU);

		CheckPoint(node, x - 1, y, LL);
		CheckPoint(node, x + 1, y, RR);
		
		CheckPoint(node, x - 1, y + 1, LD);
		CheckPoint(node, x, y + 1, DD);
		CheckPoint(node, x + 1, y + 1, RD);
	}
	else
	{
		switch (node->direction)
		{
		case UU:
			CheckPoint(node, x, y - 1, UU);
			if (!CheckWalkable(x - 1, y))
				CheckPoint(node, x - 1, y - 1, LU);
			if (!CheckWalkable(x + 1, y))
				CheckPoint(node, x + 1, y - 1, RU);
			break;
		case LL:
			CheckPoint(node, x - 1, y, LL);
			if (!CheckWalkable(x, y - 1))
				CheckPoint(node, x - 1, y - 1, LU);
			if (!CheckWalkable(x, y + 1))
				CheckPoint(node, x - 1, y + 1, LD);
			break;
		case RR:
			CheckPoint(node, x + 1, y, RR);
			if (!CheckWalkable(x, y - 1))
				CheckPoint(node, x + 1, y - 1, RU);
			if (!CheckWalkable(x, y + 1))
				CheckPoint(node, x + 1, y + 1, RD);
			break;
		case DD:
			CheckPoint(node, x, y + 1, DD);
			if (!CheckWalkable(x - 1, y))
				CheckPoint(node, x - 1, y + 1, LD);
			if (!CheckWalkable(x + 1, y))
				CheckPoint(node, x + 1, y + 1, RD);
			break;
		case LU:
			CheckPoint(node, x - 1, y, LL);
			CheckPoint(node, x - 1, y - 1, LU);
			CheckPoint(node, x, y - 1, UU);
			if (!CheckWalkable(x, y + 1))
				CheckPoint(node, x - 1, y + 1, LD);			
			if (!CheckWalkable(x + 1, y))
				CheckPoint(node, x + 1, y - 1, RU);
			break;
		case RU:
			CheckPoint(node, x + 1, y, RR);
			CheckPoint(node, x + 1, y - 1, RU);
			CheckPoint(node, x, y - 1, UU);
			if (!CheckWalkable(x - 1, y))
				CheckPoint(node, x - 1, y - 1, LU);
			if (!CheckWalkable(x, y + 1))
				CheckPoint(node, x + 1, y + 1, RD);
			break;
		case LD:
			CheckPoint(node, x - 1, y, LL);
			CheckPoint(node, x - 1, y + 1, LD);
			CheckPoint(node, x, y + 1, DD);
			if (!CheckWalkable(x, y - 1))
				CheckPoint(node, x - 1, y - 1, LU);
			if (!CheckWalkable(x + 1, y))
				CheckPoint(node, x + 1, y + 1, RD);
			break;
		case RD:
			CheckPoint(node, x + 1, y, RR);
			CheckPoint(node, x + 1, y + 1, RD);
			CheckPoint(node, x, y + 1, DD);
			if (!CheckWalkable(x - 1, y))
				CheckPoint(node, x - 1, y + 1, LD);
			if (!CheckWalkable(x, y - 1))
				CheckPoint(node, x + 1, y - 1, RU);
			break;
		}
	}
}

void JPS::NeighborsNode(MAP_NODE * node, COLORREF color)
{
	int x = node->x;
	int y = node->y;
	if (node->parent == nullptr)
	{
		CheckPoint(node, x - 1, y - 1, LU, color);
		CheckPoint(node, x, y - 1, UU, color);
		CheckPoint(node, x + 1, y - 1, RU, color);

		CheckPoint(node, x - 1, y, LL, color);
		CheckPoint(node, x + 1, y, RR, color);

		CheckPoint(node, x - 1, y + 1, LD, color);
		CheckPoint(node, x, y + 1, DD, color);
		CheckPoint(node, x + 1, y + 1, RD, color);
	}
	else
	{
		switch (node->direction)
		{
		case UU:
			CheckPoint(node, x, y - 1, UU, color);
			if (!CheckWalkable(x - 1, y))
				CheckPoint(node, x - 1, y - 1, LU, color);
			if (!CheckWalkable(x + 1, y))
				CheckPoint(node, x + 1, y - 1, RU, color);
			break;
		case LL:
			CheckPoint(node, x - 1, y, LL, color);
			if (!CheckWalkable(x, y - 1))
				CheckPoint(node, x - 1, y - 1, LU, color);
			if (!CheckWalkable(x, y + 1))
				CheckPoint(node, x - 1, y + 1, LD, color);
			break;
		case RR:
			CheckPoint(node, x + 1, y, RR, color);
			if (!CheckWalkable(x, y - 1))
				CheckPoint(node, x + 1, y - 1, RU, color);
			if (!CheckWalkable(x, y + 1))
				CheckPoint(node, x + 1, y + 1, RD, color);
			break;
		case DD:
			CheckPoint(node, x, y + 1, DD, color);
			if (!CheckWalkable(x - 1, y))
				CheckPoint(node, x - 1, y + 1, LD, color);
			if (!CheckWalkable(x + 1, y))
				CheckPoint(node, x + 1, y + 1, RD, color);
			break;
		case LU:
			CheckPoint(node, x - 1, y, LL, color);
			CheckPoint(node, x - 1, y - 1, LU, color);
			CheckPoint(node, x, y - 1, UU, color);
			if (!CheckWalkable(x, y + 1))
				CheckPoint(node, x - 1, y + 1, LD, color);
			if (!CheckWalkable(x + 1, y))
				CheckPoint(node, x + 1, y - 1, RU, color);
			break;
		case RU:
			CheckPoint(node, x + 1, y, RR, color);
			CheckPoint(node, x + 1, y - 1, RU, color);
			CheckPoint(node, x, y - 1, UU, color);
			if (!CheckWalkable(x - 1, y))
				CheckPoint(node, x - 1, y - 1, LU, color);
			if (!CheckWalkable(x, y + 1))
				CheckPoint(node, x + 1, y + 1, RD, color);
			break;
		case LD:
			CheckPoint(node, x - 1, y, LL, color);
			CheckPoint(node, x - 1, y + 1, LD, color);
			CheckPoint(node, x, y + 1, DD, color);
			if (!CheckWalkable(x, y - 1))
				CheckPoint(node, x - 1, y - 1, LU, color);
			if (!CheckWalkable(x + 1, y))
				CheckPoint(node, x + 1, y + 1, RD, color);
			break;
		case RD:
			CheckPoint(node, x + 1, y, RR, color);
			CheckPoint(node, x + 1, y + 1, RD, color);
			CheckPoint(node, x, y + 1, DD, color);
			if (!CheckWalkable(x - 1, y))
				CheckPoint(node, x - 1, y + 1, LD, color);
			if (!CheckWalkable(x, y - 1))
				CheckPoint(node, x + 1, y - 1, RU, color);
			break;
		}
	}
}

void JPS::SaveResult(MAP_NODE * endNode, int * num)
{
	MAP_NODE * current = endNode;
	int resultNum = 0;
	while (current != nullptr)
	{
		_resultPath[resultNum++] = { current->x, current->y };
		current = current->parent;
	}
	*num = resultNum;
}

bool JPS::CheckPoint(MAP_NODE * parent, int x, int y, Direction direction)
{
	if (!CheckWalkable(x, y))
		return false;

	int newX, newY;

	if (!Jump(x, y, direction, &newX, &newY))
		return false;

	if (CreateNode(newX, newY, parent, direction))
		return true;

	return false;
}

bool JPS::CheckPoint(MAP_NODE * parent, int x, int y, Direction direction, COLORREF color)
{
	if (!CheckWalkable(x, y))
		return false;

	int newX, newY;
	if (!Jump(x, y, direction, &newX, &newY, color))
		return false;

	Render();
	Sleep(100);

	if (CreateNode(newX, newY, parent, direction))
		return true;
	return false;
}

bool JPS::Jump(int x, int y, Direction direction, int * newX, int * newY)
{
	int jumpCount = 0;
	switch (direction)
	{
	case UU:
	{
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x - 1, y) && CheckWalkable(x - 1, y - 1)) ||
				!CheckWalkable(x + 1, y) && CheckWalkable(x + 1, y - 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			--y;
			++jumpCount;
		}
		break;
	}
	case RR:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x, y - 1) && CheckWalkable(x + 1, y - 1)) ||
				!CheckWalkable(x, y + 1) && CheckWalkable(x + 1, y + 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			++x;
			++jumpCount;
		}
		break;
	case DD:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x - 1, y) && CheckWalkable(x - 1, y + 1)) ||
				!CheckWalkable(x + 1, y) && CheckWalkable(x + 1, y + 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			++y;
			++jumpCount;
		}
		break;
	case LL:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x, y - 1) && CheckWalkable(x - 1, y - 1)) ||
				!CheckWalkable(x, y + 1) && CheckWalkable(x - 1, y + 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			--x;
			++jumpCount;
		}
		break;
	case RU:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x - 1, y) && CheckWalkable(x - 1, y - 1)) ||
				!CheckWalkable(x, y + 1) && CheckWalkable(x + 1, y + 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			if (Jump(x + 1, y, RR, newX, newY) || Jump(x, y - 1, UU, newX, newY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			++x;
			--y;
			++jumpCount;
		}
		break;
	case RD:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x - 1, y) && CheckWalkable(x - 1, y + 1)) ||
				!CheckWalkable(x, y - 1) && CheckWalkable(x + 1, y - 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			if (Jump(x + 1, y, RR, newX, newY) || Jump(x, y + 1, DD, newX, newY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			++x;
			++y;
			++jumpCount;
		}
		break;
	case LD:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x + 1, y) && CheckWalkable(x + 1, y + 1)) ||
				!CheckWalkable(x, y - 1) && CheckWalkable(x - 1, y - 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			if (Jump(x - 1, y, LL, newX, newY) || Jump(x, y + 1, DD, newX, newY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			--x;
			++y;
			++jumpCount;
		}
		break;
	case LU:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x + 1, y) && CheckWalkable(x + 1, y - 1)) ||
				!CheckWalkable(x, y + 1) && CheckWalkable(x - 1, y + 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			if (Jump(x - 1, y, LL, newX, newY) || Jump(x, y - 1, UU, newX, newY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			--x;
			--y;
			++jumpCount;
		}
		break;
	}
	return false;
}

bool JPS::Jump(int x, int y, Direction direction, int * newX, int * newY, COLORREF color)
{
	int jumpCount = 0;
	switch (direction)
	{
	case UU:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x - 1, y) && CheckWalkable(x - 1, y - 1)) ||
				!CheckWalkable(x + 1, y) && CheckWalkable(x + 1, y - 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			FOUND_NODE * foundNode = new FOUND_NODE;
			foundNode->x = x;
			foundNode->y = y;
			foundNode->color = color;
			_foundList.push_back(foundNode);
			--y;
			++jumpCount;
		}
		break;
	case RR:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x, y - 1) && CheckWalkable(x + 1, y - 1)) ||
				!CheckWalkable(x, y + 1) && CheckWalkable(x + 1, y + 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			FOUND_NODE * foundNode = new FOUND_NODE;
			foundNode->x = x;
			foundNode->y = y;
			foundNode->color = color;
			_foundList.push_back(foundNode);
			++x;
			++jumpCount;
		}
		break;
	case DD:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x - 1, y) && CheckWalkable(x - 1, y + 1)) ||
				!CheckWalkable(x + 1, y) && CheckWalkable(x + 1, y + 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			FOUND_NODE * foundNode = new FOUND_NODE;
			foundNode->x = x;
			foundNode->y = y;
			foundNode->color = color;
			_foundList.push_back(foundNode);
			++y;
			++jumpCount;
		}
		break;
	case LL:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x, y - 1) && CheckWalkable(x - 1, y - 1)) ||
				!CheckWalkable(x, y + 1) && CheckWalkable(x - 1, y + 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			FOUND_NODE * foundNode = new FOUND_NODE;
			foundNode->x = x;
			foundNode->y = y;
			foundNode->color = color;
			_foundList.push_back(foundNode);
			--x;
			++jumpCount;
		}
		break;
	case RU:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x - 1, y) && CheckWalkable(x - 1, y - 1)) ||
				!CheckWalkable(x, y + 1) && CheckWalkable(x + 1, y + 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			if (Jump(x + 1, y, RR, newX, newY, color) || Jump(x, y - 1, UU, newX, newY, color))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			FOUND_NODE * foundNode = new FOUND_NODE;
			foundNode->x = x;
			foundNode->y = y;
			foundNode->color = color;
			_foundList.push_back(foundNode);
			++x;
			--y;
			++jumpCount;
		}
		break;
	case RD:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x - 1, y) && CheckWalkable(x - 1, y + 1)) ||
				!CheckWalkable(x, y - 1) && CheckWalkable(x + 1, y - 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			if (Jump(x + 1, y, RR, newX, newY, color) || Jump(x, y + 1, DD, newX, newY, color))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			FOUND_NODE * foundNode = new FOUND_NODE;
			foundNode->x = x;
			foundNode->y = y;
			foundNode->color = color;
			_foundList.push_back(foundNode);
			++x;
			++y;
			++jumpCount;
		}
		break;
	case LD:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x + 1, y) && CheckWalkable(x + 1, y + 1)) ||
				!CheckWalkable(x, y - 1) && CheckWalkable(x - 1, y - 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			if (Jump(x - 1, y, LL, newX, newY, color) || Jump(x, y + 1, DD, newX, newY, color))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			FOUND_NODE * foundNode = new FOUND_NODE;
			foundNode->x = x;
			foundNode->y = y;
			foundNode->color = color;
			_foundList.push_back(foundNode);
			--x;
			++y;
			++jumpCount;
		}
		break;
	case LU:
		while (CheckWalkable(x, y))
		{
			if (_maxJump < jumpCount)
				break;
			if ((!CheckWalkable(x + 1, y) && CheckWalkable(x + 1, y - 1)) ||
				!CheckWalkable(x, y + 1) && CheckWalkable(x - 1, y + 1) ||
				(x == _endX && y == _endY))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			if (Jump(x - 1, y, LL, newX, newY, color) || Jump(x, y - 1, UU, newX, newY, color))
			{
				*newX = x;
				*newY = y;
				return true;
			}
			FOUND_NODE * foundNode = new FOUND_NODE;
			foundNode->x = x;
			foundNode->y = y;
			foundNode->color = color;
			_foundList.push_back(foundNode);
			--x;
			--y;
			++jumpCount;
		}
		break;
	}
	return false;
}

void JPS::DisplayPath(MAP_NODE *endNode)
{
	MAP_NODE * parent;
	HDC memDC = _doubleBuffer->GetMemDC();
	SelectObject(memDC, PEN(10));
	while (endNode->parent != nullptr)
	{
		parent = endNode->parent;
		MoveToEx(memDC,
			(endNode->x * BLOCKSIZE) + (BLOCKSIZE / 2),
			(endNode->y * BLOCKSIZE) + (BLOCKSIZE / 2),
			NULL);
		LineTo(memDC,
			(parent->x * BLOCKSIZE) + (BLOCKSIZE / 2),
			(parent->y * BLOCKSIZE) + (BLOCKSIZE / 2));
		endNode = endNode->parent;
	}
}

void JPS::OptimizePath(MAP_NODE * endNode)
{
	MAP_NODE * start = endNode;
	MAP_NODE * current = endNode->parent;
	MAP_NODE * parent = current->parent;
	HDC memDC = _doubleBuffer->GetMemDC();
	POINT * point;
	int num;
	int optimizePathNum = 0;
	_optimizePath[optimizePathNum++] = { endNode->x, endNode->y };

	SelectObject(memDC, PEN(14));
	_bresenLine.SetStartPoint(start->x, start->y);
	while (true)
	{
		_bresenLine.SetEndPoint(parent->x, parent->y);
		if (!_bresenLine.GetLine(&point, &num))
			return;

		// 직선안의 벽 검색
		for (int i = 0; i < num; ++i)
		{
			MAP_STATUS status;
			FindTile(&status, point[i].x, point[i].y);
			if (status == WALL)
			{
				// 선을 그린다
				MoveToEx(memDC,
					(start->x * BLOCKSIZE) + (BLOCKSIZE / 2),
					(start->y * BLOCKSIZE) + (BLOCKSIZE / 2),
					NULL);
				LineTo(memDC,
					(current->x * BLOCKSIZE) + (BLOCKSIZE / 2),
					(current->y * BLOCKSIZE) + (BLOCKSIZE / 2));

				_optimizePath[optimizePathNum++] = { current->x, current->y };
				start = current;
				_bresenLine.SetStartPoint(current->x, current->y);
				break;
			}
		}
		if (parent->parent == nullptr)
		{
			// 마지막 선 그리고 루프종료
			MoveToEx(memDC,
				(start->x * BLOCKSIZE) + (BLOCKSIZE / 2),
				(start->y * BLOCKSIZE) + (BLOCKSIZE / 2),
				NULL);
			LineTo(memDC,
				(parent->x * BLOCKSIZE) + (BLOCKSIZE / 2),
				(parent->y * BLOCKSIZE) + (BLOCKSIZE / 2));

			_optimizePath[optimizePathNum] = { parent->x, parent->y };
			break;
		}
		parent = parent->parent;
		current = current->parent;
	}
}

JPS::JPS(HWND hWnd)
{
	_doubleBuffer = new DoubleBuffer(hWnd);
	_maxJump = JUMP_MAX;
}

JPS::~JPS()
{
	delete _doubleBuffer;
}

void JPS::SetMap(char * mapPointer, int width, int depth)
{
	_mapPointer = mapPointer;
	_mapWidth = width;
	_mapDepth = depth;
}

bool JPS::FindPath(MAP_NODE ** endNode, POINT ** resultArray, int * num)
{
	if (!FindTile(END, &_endX, &_endY))
		return false;
	if (!FindTile(START, &_startX, &_startY))
		return false;

	ClearNode();
	CreateNode(_startX, _startY, nullptr, NULL, _endX, _endY);
	while (true)
	{
		// F 키로 정렬
		_openList.sort(Less());
		// 하나 뽑기 -> 뽑을거 없으면 길이 없다
		std::list<MAP_NODE *>::iterator iter = _openList.begin();
		if (iter == _openList.end())
			return false;
		MAP_NODE * output = (*iter);
		_openList.erase(iter);
		// 목적지인지 체크 // 리턴을 이걸로 하자
		if (output->h == 0)
		{
			*endNode = output;
			SaveResult(*endNode, num);
			return true;
		}
		// close에 넣기
		_closeList.push_back(output);
#ifdef JPS_DISPLAY
		COLORREF color = RGB(rand() % 50 + 205, rand() % 50 + 205, rand() % 50 + 205);
		NeighborsNode(output, color);
#else
		NeighborsNode(output);
#endif // JPS_DISPLAY
	}
	return false;
}

void JPS::Render(int num)
{
	_doubleBuffer->Clear();
	DrawLine();
	DrawList();
	DrawBlock();
	if(num != 0)
		_bresenLine.DrawLine(_doubleBuffer->GetMemDC(), num);
	_doubleBuffer->Flipping();
}

void JPS::Render(MAP_NODE * endNode)
{
	_doubleBuffer->Clear();
	DrawLine();
	DrawList();
	DrawBlock();
	DisplayPath(endNode);
	OptimizePath(endNode);
	_doubleBuffer->Flipping();
}

DoubleBuffer * JPS::GetDoubleBuffer()
{
	return _doubleBuffer;
}

void JPS::SetBresenhamStart(int x, int y)
{
	_bresenLine.SetStartPoint(x, y);
}

void JPS::SetBresenhamEnd(int x, int y)
{
	_bresenLine.SetEndPoint(x, y);
}

bool JPS::BresenhamLine(POINT **result, int * num)
{
	if (_bresenLine.GetLine(result, num))
		return true;
	return false;
}
