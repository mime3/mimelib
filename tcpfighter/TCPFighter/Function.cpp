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
// frame���
DWORD timecheck = timeGetTime();
int frame = 0;
int printframe = 0;
int logic = 0;
int printlogic = 0;
// frame���

// �ֿ��Լ�

/*----------------------------------------------------------*/
// ::Update
// ���� : ���� ���� ����
// ���� : (HWND) ������ �ڵ�
// ���� :
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

	// frame���
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
	// frame���
}
/*----------------------------------------------------------*/
// ::KeyProcess 
// ���� : Ű �Է��� Ȯ���Ͽ� �÷��̾ ��Ʈ���Ѵ�.
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void KeyProcess()
{
	// ���� ������ ��Ȱ��ȭ����
	if (!active)
		return;
	// �÷��̾ ���� ����
	if (g_player == nullptr)
		return;

	// 0x8000: ���� ������ �ִ���
	// 0x0001: ������ ���ȴ��� �ִ���
	// üũ ������ ������������ �켱���
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
// ���� : ���� ����
// ���� : 
// ���� :
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
// ���� : Ÿ�ϰ� ������Ʈ���� ȭ�鿡 �������Ѵ�.
// ���� : (HWND) ������ �ڵ�
// ���� :
/*----------------*////////////////////////*----------------*/
void Render(HWND hWnd)
{
	// ����̤��ϵ��ڵ�
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

// �����Լ�

/*----------------------------------------------------------*/
// ::GetPriority
// ���� : ������Ʈ���� ������ �켱���� �����Ѵ�.
// ���� : (Object *) ������Ʈ ������
// ���� : (int) ������ �켱��
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
// ���� : ����Ʈ���� ������ ã�Ƽ� �� �����͸� �����Ѵ�.
// ���� : (int) ���� �ĺ���ȣ
// ���� : (Player *) �÷��̾��� ������
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
