#pragma once
typedef struct PLAYER
{
	__int64 sessionID;
	__int64 accountNo;
	bool loginFlag;

	WORD sectorX;
	WORD sectorY;

	WCHAR ID[20];
	WCHAR nickName[20];

	char sessionKey[64];
	PLAYER()
	{
		accountNo = 0;
		loginFlag = false;
		sectorX = 0;
		sectorY = 0;
	}
}Player;

