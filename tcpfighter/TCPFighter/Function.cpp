#include "stdafx.h"
#include "Function.h"

extern ScreenDIB *screen;
extern SpriteDIB *sprite;
extern CList<Object *> g_list;
extern FrameSkip *frameSkip;
extern Player * g_player;
extern TileMap * g_tileMap;
extern bool active;

extern DWORD time;
// frame계산
DWORD timecheck = timeGetTime();
int frame = 0;
int printframe = 0;
int logic = 0;
int printlogic = 0;
// frame계산

// 주요함수

/*----------------------------------------------------------*/
// ::Update
// 설명 : 게임 메인 루프
// 인자 : (HWND) 윈도우 핸들
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Update(HWND hWnd)
{
	KeyProcess();
	Logic(); 
	logic++;
	if (!frameSkip->Skip())
	{
		Render(hWnd);
		frame++;
	}

	// frame계산
	DWORD curTime = timeGetTime();
	if (curTime - timecheck >= 1000)
	{
		timecheck = curTime;
		printframe = frame;
		frame = 0;
		printlogic = logic;
		logic = 0;
	}
	
	HDC hdc = GetDC(hWnd);
	WCHAR buffer[8];
	wsprintf(buffer, L"%d", printframe);
	TextOut(hdc, 0, 0, buffer, lstrlen(buffer));
	wsprintf(buffer, L"%d", printlogic);
	TextOut(hdc, 20, 0, buffer, lstrlen(buffer));
	ReleaseDC(hWnd, hdc);
	// frame계산
}
/*----------------------------------------------------------*/
// ::KeyProcess 
// 설명 : 키 입력을 확인하여 플레이어를 컨트롤한다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void KeyProcess()
{
	// 현재 윈도우 비활성화상태
	if (!active)
		return;
	// 플레이어가 아직 없음
	if (g_player == nullptr)
		return;

	// 0x8000: 현재 눌려져 있는지
	// 0x0001: 저번에 눌렸던게 있는지
	// 체크 순서가 위에있을수록 우선명령
	if (GetAsyncKeyState(0x5A) & 0x8000) // z
		g_player->Command(ATTACK_TYPE_1);
	else if (GetAsyncKeyState(0x58) & 0x8000)  // x
		g_player->Command(ATTACK_TYPE_2);
	else if (GetAsyncKeyState(0x43) & 0x8000)  // c
		g_player->Command(ATTACK_TYPE_3);
	else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) 
		&& (GetAsyncKeyState(VK_LEFT) & 0x8000))
		g_player->Command(ACTION_MOVE_LD);
	else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) 
		&& (GetAsyncKeyState(VK_RIGHT) & 0x8000))
		g_player->Command(ACTION_MOVE_RD);
	else if ((GetAsyncKeyState(VK_UP) & 0x8000) 
		&& (GetAsyncKeyState(VK_LEFT) & 0x8000))
		g_player->Command(ACTION_MOVE_LU);
	else if ((GetAsyncKeyState(VK_UP) & 0x8000) 
		&& (GetAsyncKeyState(VK_RIGHT) & 0x8000))
		g_player->Command(ACTION_MOVE_RU);
	else if (GetAsyncKeyState(VK_UP) & 0x8000)
		g_player->Command(ACTION_MOVE_UU);
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		g_player->Command(ACTION_MOVE_LL);
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		g_player->Command(ACTION_MOVE_RR);
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		g_player->Command(ACTION_MOVE_DD);
	else
		g_player->Command(STAND);
}

/*----------------------------------------------------------*/
// ::Logic 
// 설명 : 게임 로직
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Logic()
{
	CList<Object *>::iterator iter = g_list.begin();
	for (; iter != g_list.end();)
	{
		if (!(*iter)->Action())
		{
			delete (*iter);
			iter = g_list.erase(iter);
		}
		else
			++iter;
	}
}

/*----------------------------------------------------------*/
// ::Render 
// 설명 : 타일과 오브젝트들을 화면에 렌더링한다.
// 인자 : (HWND) 윈도우 핸들
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Render(HWND hWnd)
{
	// 흐얽ㅜㅠ하드코딩
	// bubble sort 
	CList<Object *>::iterator iter, iter2;
	for (int i = 0; i < g_list.size(); i++)
	{
		iter = g_list.begin();
		for (int j = 0; j < g_list.size() - i; j++)
		{
			iter2 = iter._node->_next;
			if (iter2 == g_list.end())
				break;

			int compare = GetPriority(*iter) - GetPriority(*iter2);
			if (compare == 0)
			{
				Pos pos1, pos2;
				pos1 = (*iter)->GetPos();
				pos2 = (*iter2)->GetPos();
				if (pos2.y < pos1.y)
				{
					Object * temp = *iter2;
					*iter2 = *iter;
					*iter = temp;
				}
			}
			else if (0 < compare)
			{
				Object * temp = *iter2;
				*iter2 = *iter;
				*iter = temp;
			}
			iter++;
		}
	}

	// Tiledraw
	g_tileMap->DrawTile();
	// Render
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
		(*iter)->Draw();
	// Flip
	screen->DrawBuffer(hWnd);
}

// 작은함수

/*----------------------------------------------------------*/
// ::GetPriority
// 설명 : 오브젝트들의 렌더링 우선도를 리턴한다.
// 인자 : (Object *) 오브젝트 포인터
// 리턴 : (int) 렌더링 우선도
/*----------------*////////////////////////*----------------*/
int GetPriority(Object * object)
{
	Object::TYPE type = object->GetType();
	switch (type)
	{
	case Object::MAP:
		return 0;
		break;
	case Object::PLAYER:
		return 2;
		break;
	case Object::EFFECT:
		if (((Effect *)object)->GetInfinite() == true)
			return 1;
		else
			return 3;
		break;
	}
	return -1;
}

/*----------------------------------------------------------*/
// ::FindUser
// 설명 : 리스트에서 유저를 찾아서 그 포인터를 리턴한다.
// 인자 : (int) 유저 식별번호
// 리턴 : (Player *) 플레이어의 포인터
/*----------------*////////////////////////*----------------*/
Player * FindUser(int ID)
{
	CList<Object *>::iterator iter = g_list.begin();
	for (; iter != g_list.end();)
	{
		if (((Player *)(*iter))->GetID() == ID)
			return ((Player *)(*iter));
		++iter;
	}
	return nullptr;
}
