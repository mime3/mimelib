#pragma once
#define BLOCKSIZE 20
#define MAP_X_SIZE 60
#define MAP_Y_SIZE 30
#define JUMP_MAX 20
#define POINT_MAX 100
// 과정표시용
#define JPS_DISPLAY

using namespace MinLib;

class JPS
{
public:
	enum Direction
	{
		UU,
		RU,
		RR,
		RD,
		DD,
		LD,
		LL,
		LU
	};
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
		Direction direction;
	};
	struct FOUND_NODE
	{
		int x;
		int y;
		COLORREF color;
	};

private:
	char * _mapPointer;
	int _startX;
	int _startY;
	int _endX;
	int _endY;
	int _mapWidth;
	int _mapDepth;
	int _maxJump;
	DoubleBuffer * _doubleBuffer;
	std::list<MAP_NODE *> _openList;
	std::list<MAP_NODE *> _closeList;
	std::list<FOUND_NODE *> _foundList; // 과정 표시용
	POINT _resultPath[POINT_MAX];
	BresenhamLine _bresenLine;
	POINT _optimizePath[BRESENHAM_MAX];

protected:
	// to render
	void DrawLine();
	void DrawBlock();
	void DrawList();

	bool FindTile(MAP_STATUS status, int * findX, int * findY);
	bool FindTile(MAP_STATUS * status, int findX, int findY);
	bool CreateNode(int x, int y, MAP_NODE * parent, int plus, int endX, int endY);
	bool CreateNode(int x, int y, MAP_NODE * parent, Direction direction);
	bool Jump(int x, int y, Direction direction, int * newX, int * newY);
	bool Jump(int x, int y, Direction direction, int * newX, int * newY, COLORREF color); // 과정 표시용
	inline bool CheckWalkable(int x, int y);
	bool CheckPoint(MAP_NODE * parent, int x, int y, Direction direction);
	bool CheckPoint(MAP_NODE * parent, int x, int y, Direction direction, COLORREF color); // 과정 표시용
	void NeighborsNode(MAP_NODE * node);
	void NeighborsNode(MAP_NODE * node, COLORREF color); // 과정표시용
	void SaveResult(MAP_NODE *endNode, int * num);
	void DisplayPath(MAP_NODE *endNode);
	void OptimizePath(MAP_NODE *endNode);

public:
	JPS(HWND hWnd);
	~JPS();
	void ClearNode();
	void SetMap(char * mapPointer, int width, int depth);
	bool FindPath(MAP_NODE ** endNode, POINT ** resultArray, int * num);
	void Render(int num = 0);
	void Render(MAP_NODE *endNode);

	DoubleBuffer * GetDoubleBuffer();

	void SetBresenhamStart(int x, int y);
	void SetBresenhamEnd(int x, int y);
	bool BresenhamLine(POINT **result, int * num);
};

inline bool JPS::CheckWalkable(int x, int y)
{
	MAP_STATUS status;
	if (!FindTile(&status, x, y))
		return false;

	if (status == WALL)
		return false;
	else
		return true;
}

struct Less
{
	bool operator () (JPS::MAP_NODE * one, JPS::MAP_NODE * two) const
	{
		return one->f < two->f;
	}
};
