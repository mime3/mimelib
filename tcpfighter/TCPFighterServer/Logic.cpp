#pragma once
#include "stdafx.h"

extern list<Player *> g_playerList;
extern FrameSkipServer g_frameSkipServer;
extern DWORD g_systemTime;
extern DWORD g_delayTime;

void Logic()
{
	list<Player *>::iterator iter = g_playerList.begin();
	list<Player *>::iterator iterEnd = g_playerList.end();
	for (; iter != iterEnd;)
	{
		if (!(*iter)->Action())
		{
			delete (*iter);
			iter = g_playerList.erase(iter);
		}
		else
			++iter;
	}
}