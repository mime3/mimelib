#pragma once
#define BRESENHAM_MAX 100

class BresenhamLine
{
public:
	enum DIRECTION
	{
		HORIZONTAL,
		VERTICAL
	};
private:
	int _save;
	POINT _start;
	POINT _end;
	POINT _line[BRESENHAM_MAX];
protected:
	void Check(POINT * start, POINT * end);
	bool Next(int * x, int * y, DIRECTION axis, POINT start, POINT end);
public:
	BresenhamLine();
	BresenhamLine(int sx, int sy, int ex, int ey);
	~BresenhamLine();
	void SetPoint(int sx, int sy, int ex, int ey);
	void SetStartPoint(int sx, int sy);
	void SetEndPoint(int ex, int ey);
	bool GetLine(POINT ** resultLine, int * pointNum);

	void DrawLine(HDC hdc, int num);
};

