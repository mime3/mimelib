#include "stdafx.h"
#include "SectorManager.h"

extern multimap<TAG_ACTION, SECTOR_POS> g_moveInsertTable;
extern multimap<TAG_ACTION, SECTOR_POS> g_moveDeleteTable;

SECTOR_POS SectorManager::CalculateSector(Player * player)
{
	return SECTOR_POS{ player->_pos.x / SECTOR_WIDTH ,player->_pos.y / SECTOR_DEPTH};
}

SectorManager::SectorManager()
{
}

SectorManager::~SectorManager()
{
}

bool SectorManager::InsertPlayer(Player * player)
{
	if (player == nullptr)
	{
		LOG(Logger::LOG_LEVEL_ERROR, L"InsertPlayer argment is nullptr");
		return false;
	}

	_sector[player->_newSector.x][player->_newSector.y].push_back(player);
	return true;
}

bool SectorManager::DeletePlayer(Player * player)
{
	if (player == nullptr)
	{
		LOG(Logger::LOG_LEVEL_ERROR, L"DeletePlayer argment is nullptr");
		return false;
	}

	list<Player *>::iterator iter = _sector[player->_oldSector.x][player->_oldSector.y].begin();
	list<Player *>::iterator iterEnd = _sector[player->_oldSector.x][player->_oldSector.y].end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == player)
		{
			_sector[player->_oldSector.x][player->_oldSector.y].erase(iter);
			return true;
		}
	}
	LOG(Logger::LOG_LEVEL_ERROR, L"DeletePlayer can't find from sector");
	return false;
}

void SectorManager::NotifyAroundSector(Player * player)
{
	SECTOR_POS pos = CalculateSector(player);
	AddSectorUserPacket(player, SECTOR_POS{pos.x, pos.y});
	AddSectorUserPacket(player, SECTOR_POS{pos.x - 1, pos.y - 1});
	AddSectorUserPacket(player, SECTOR_POS{pos.x - 1, pos.y});
	AddSectorUserPacket(player, SECTOR_POS{pos.x - 1, pos.y + 1});
	AddSectorUserPacket(player, SECTOR_POS{pos.x, pos.y - 1});
	AddSectorUserPacket(player, SECTOR_POS{pos.x, pos.y + 1});
	AddSectorUserPacket(player, SECTOR_POS{pos.x + 1, pos.y - 1});
	AddSectorUserPacket(player, SECTOR_POS{pos.x + 1, pos.y});
	AddSectorUserPacket(player, SECTOR_POS{pos.x + 1, pos.y + 1});
	AddUserPacket_ForSector(player, SECTOR_POS{ pos.x, pos.y});
	AddUserPacket_ForSector(player, SECTOR_POS{ pos.x - 1, pos.y - 1});
	AddUserPacket_ForSector(player, SECTOR_POS{ pos.x - 1, pos.y });
	AddUserPacket_ForSector(player, SECTOR_POS{ pos.x - 1, pos.y + 1});
	AddUserPacket_ForSector(player, SECTOR_POS{ pos.x, pos.y - 1});
	AddUserPacket_ForSector(player, SECTOR_POS{ pos.x, pos.y + 1});
	AddUserPacket_ForSector(player, SECTOR_POS{ pos.x + 1, pos.y - 1});
	AddUserPacket_ForSector(player, SECTOR_POS{ pos.x + 1, pos.y });
	AddUserPacket_ForSector(player, SECTOR_POS{ pos.x + 1, pos.y + 1});
}

bool SectorManager::UpdateSector(Player * player)
{
	player->_newSector = CalculateSector(player);
	if (player->_oldSector.x != player->_newSector.x || player->_oldSector.y != player->_newSector.y)
		return true;
	return false;
}

bool SectorManager::ChangeSector(Player * player)
{
	if (!UpdateSector(player))
		return false;
	
	bool retval = true;
	retval &= DeletePlayer(player);
	retval &= InsertPlayer(player);

	SECTOR_POS oldSector = player->_oldSector;
	SECTOR_POS newSector = player->_newSector;

	TAG_ACTION moveSector;
	if (oldSector.x < newSector.x)
	{
		if (oldSector.y < newSector.y)
			moveSector = ACTION_MOVE_RD;
		else if (newSector.y < oldSector.y)
			moveSector = ACTION_MOVE_RU;
		else
			moveSector = ACTION_MOVE_RR;

	}
	else if (newSector.x < oldSector.x)
	{
		if (oldSector.y < newSector.y)
			moveSector = ACTION_MOVE_LD;
		else if (newSector.y < oldSector.y)
			moveSector = ACTION_MOVE_LU;
		else
			moveSector = ACTION_MOVE_LL;
	}
	else
	{
		if (oldSector.y < newSector.y)
			moveSector = ACTION_MOVE_DD;
		else if (newSector.y < oldSector.y)
			moveSector = ACTION_MOVE_UU;
		else
		{
			LOG(Logger::LOG_LEVEL_ERROR, L"try Sector move but don't move");
			return false;
		}
	}

	multimap<TAG_ACTION, SECTOR_POS>::iterator iter = g_moveInsertTable.lower_bound(moveSector);
	multimap<TAG_ACTION, SECTOR_POS>::iterator iterEnd = g_moveInsertTable.upper_bound(moveSector);
	for (; iter != iterEnd; ++iter)
	{
		AddSectorUserPacket(player, SECTOR_POS{ newSector.x + iter->second.x, newSector.y + iter->second.y });
		AddUserPacket_ForSector(player, SECTOR_POS{ newSector.x + iter->second.x, newSector.y + iter->second.y });
	}

	iter = g_moveDeleteTable.lower_bound(moveSector);
	iterEnd = g_moveDeleteTable.upper_bound(moveSector);
	for (; iter != iterEnd; ++iter)
	{
		DeleteSectorUserPacket(player, SECTOR_POS{ oldSector.x + iter->second.x, oldSector.y + iter->second.y });
		DeleteUserPacket_ForSector(player, SECTOR_POS{ oldSector.x + iter->second.x, oldSector.y + iter->second.y });
	}

	return true;
}
// 플레이어에게 주변캐릭정보를 보낸다.
bool SectorManager::AddSectorUserPacket(Player * player, SECTOR_POS pos)
{
	if (pos.x < 0 || pos.y < 0)
		return false;
	list<Player *>::iterator iter = _sector[pos.x][pos.y].begin();
	list<Player *>::iterator iterEnd = _sector[pos.x][pos.y].end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == player)
			continue;
		MinLib::StreamBuffer * makePlayer;
		MakePacket_Create_Other_Character(&makePlayer, (*iter)->_ID, (*iter)->_direction, (*iter)->_pos, (*iter)->_healthPoint);
		UniCast(makePlayer, player->_session);
		// 현재 이동중
		if ((*iter)->_action != STAND)
		{
			MinLib::StreamBuffer * move;
			MakePacket_Move_Start(&move, (*iter)->_ID, (*iter)->_action, (*iter)->_pos);
			UniCast(move, player->_session);
		}
	}
	return true;
}
// 플레이어에게 주변 캐릭삭제를 보낸다.
bool SectorManager::DeleteSectorUserPacket(Player * player, SECTOR_POS pos)
{
	if (pos.x < 0 || pos.y < 0)
		return false;
	list<Player *>::iterator iter = _sector[pos.x][pos.y].begin();
	list<Player *>::iterator iterEnd = _sector[pos.x][pos.y].end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == player)
			continue;
		MinLib::StreamBuffer * makePlayer;
		MakePacket_Delete_Character(&makePlayer, (*iter)->_ID);
		UniCast(makePlayer, player->_session);
	}
	return true;
}
// 섹터의 유저에게 플레이어의 정보를 준다.
bool SectorManager::AddUserPacket_ForSector(Player * player, SECTOR_POS pos)
{
	if (pos.x < 0 || pos.y < 0)
		return false;
	list<Player *>::iterator iter = _sector[pos.x][pos.y].begin();
	list<Player *>::iterator iterEnd = _sector[pos.x][pos.y].end();
	MinLib::StreamBuffer * makePlayer, * move;
	MakePacket_Create_Other_Character(&makePlayer, player->_ID, player->_direction, player->_pos, player->_healthPoint);
	MakePacket_Move_Start(&move, player->_ID, player->_action, player->_pos);
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == player)
			continue;
		UniCast(makePlayer, (*iter)->_session, false);
		UniCast(move, (*iter)->_session, false);
	}
	delete makePlayer;
	delete move;
	return true;
}
// 섹터의 유저에게 플레이어를 삭제하라고 한다.
bool SectorManager::DeleteUserPacket_ForSector(Player * player, SECTOR_POS pos)
{
	if (pos.x < 0 || pos.y < 0)
		return false;
	list<Player *>::iterator iter = _sector[pos.x][pos.y].begin();
	list<Player *>::iterator iterEnd = _sector[pos.x][pos.y].end();
	MinLib::StreamBuffer * makePlayer;
	MakePacket_Delete_Character(&makePlayer, player->_ID);
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == player)
			continue;
		UniCast(makePlayer, (*iter)->_session, false);
	}
	delete makePlayer;
	return true;
}
bool SectorManager::NewPlayer(Player * player)
{
	if (!InsertPlayer(player))
		return false;

	NotifyAroundSector(player);
	return true;
}
bool SectorManager::BroadCast_Sector(MinLib::StreamBuffer * stream, Session * session, bool dontSend, SECTOR_POS pos)
{
	if (pos.x < 0 || pos.y < 0)
		return false;

	list<Player *>::iterator iter = _sector[pos.x][pos.y].begin();
	list<Player *>::iterator iterEnd = _sector[pos.x][pos.y].end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->_session == session && dontSend)
			continue;
		UniCast(stream, (*iter)->_session, false);
	}
	return true;
}
SectorManager * SectorManager::_instance = nullptr;