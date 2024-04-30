#include "stdafx.h"
#include "BresenhamLine.h"


void BresenhamLine::Check(POINT * start, POINT * end)
{
	if (!(start->x <= end->x))
	{
		POINT temp = *end;
		*end = *start;
		*start = temp;
	}
}

bool BresenhamLine::Next(int * x, int * y, DIRECTION axis, POINT start, POINT end)
{
	// 종점을 넣은경우
	if ((*x) == end.x && (*y) == end.y)
		return false;
	switch (axis)
	{
	case HORIZONTAL:
		if ((*x) == end.x)
			return false;

		_save += abs(end.y - start.y);
		if (end.x - start.x <= _save)
		{
			if (start.y < end.y) // 방향이 오른쪽 아래
				++(*y);
			else
				--(*y);
			_save -= end.x - start.x;
		}
		++(*x);
		break;
	case VERTICAL:
		if ((*y) == end.y)
			return false;

		_save += end.x - start.x;
		if (abs(end.y - start.y) <= _save)
		{
			++(*x);
			_save -= abs(end.y - start.y);
		}

		if (start.y < end.y) // 방향이 오른쪽 아래
			++(*y);
		else
			--(*y);
		break;
	}
	return true;
}

BresenhamLine::BresenhamLine()
{
	_save = 0;
}

BresenhamLine::BresenhamLine(int sx, int sy, int ex, int ey)
{
	_save = 0;
	SetPoint(sx, sy, ex, ey);
}

BresenhamLine::~BresenhamLine()
{
}

void BresenhamLine::SetPoint(int sx, int sy, int ex, int ey)
{
	if (sx < ex)
	{
		_start.x = sx;
		_start.y = sy;
		_end.x = ex;
		_end.y = ey;
	}
	else
	{
		_start.x = ex;
		_start.y = ey;
		_end.x = sx;
		_end.y = sy;
	}
}

void BresenhamLine::SetStartPoint(int sx, int sy)
{
	_start.x = sx;
	_start.y = sy;
}

void BresenhamLine::SetEndPoint(int ex, int ey)
{
	_end.x = ex;
	_end.y = ey;
}

bool BresenhamLine::GetLine(POINT ** resultLine, int * pointNum)
{
	POINT start = _start;
	POINT end = _end;

	Check(&start, &end);

	int x = start.x;
	int y = start.y;
	DIRECTION direction;
	
	if (end.x - start.x < abs(end.y - start.y))
	{
		direction = VERTICAL;
		_save = abs(end.y - start.y) / 2;
	}
	else
	{
		direction = HORIZONTAL;
		_save = (end.x - start.x) / 2;
	}

	int index = 0;
	_line[index].x = x;
	_line[index].y = y;
	++index;
	while (Next(&x, &y, direction, start, end))
	{
		if (BRESENHAM_MAX - 1 < index)
			return false;
		_line[index].x = x;
		_line[index].y = y;
		++index;
	}

	*resultLine = _line;
	*pointNum = index;
	return true;
}

void BresenhamLine::DrawLine(HDC hdc, int num)
{
	SelectObject(hdc, BRUSH(0));
	SelectObject(hdc, GDICOLOR()->_nullPen);
	for (int i = 0; i < num; ++i)
	{
		Rectangle(hdc, _line[i].x * BLOCKSIZE + 1, _line[i].y * BLOCKSIZE + 1, (_line[i].x + 1) * BLOCKSIZE, (_line[i].y + 1) * BLOCKSIZE);
	}
}
