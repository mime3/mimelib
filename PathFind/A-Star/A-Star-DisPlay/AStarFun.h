#pragma once
#define UM_BUTTON (WM_USER + 1)

char * GetBlock(LPARAM lParam);
void ChangeBlock(char * blcok);
void DrawLine(HDC hdc);
void DrawBlock(HDC hdc);
void SetStart(char * block);
void SetEnd(char * block);

bool FindAndDisplayPath(MAP_NODE ** endNode, HDC hdc);
bool FindPath(MAP_NODE ** endNode);
bool FindTile(MAP_STATUS status, int * findX, int * findY);
void ClearWall();
void ClearNode();
void DisplayNode(HDC hdc, int startX, int startY, int endX, int endY);
bool CreateNode(int x, int y, MAP_NODE * parent, int plus, int endX, int endY);