#include "stdafx.h"
extern list<Player *> g_playerList;

/*----------------------------------------------------------*/
// ::Function
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void RecvPacket_CS_Move_Start(Session * session, StreamBuffer * payload)
{
	BYTE direction;
	Pos pos;
	*payload >> direction >> pos.x >> pos.y;

	// 위치 보정
	if (CheckDeadReckoning(session->character, pos))
	{
		session->character->_pos = DeadReckoning(session);
		LOG(Logger::LOG_LEVEL_WARNING, L"Sync Move_Start [%d, %d] -> [%d, %d]", pos.x, pos.y, session->character->_pos.x, session->character->_pos.y);
		StreamBuffer * syncPacket;
		MakePacket_Sync(&syncPacket, session->character->_ID, session->character->_pos);
		UniCast(syncPacket, session);
		LOG(Logger::LOG_LEVEL_WARNING, L"UniCast Sync CS_Move_Start %d", session->character->_ID);
	}

	session->character->_action = (TAG_ACTION)direction;
	session->character->_moveStartPos = session->character->_pos;
	session->character->_lastActionTime = GetTickCount64();

	StreamBuffer * packet;
	MakePacket_Move_Start(&packet, session->character->_ID, direction, session->character->_pos);
	BroadCast_Sector(packet, session);
	LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Move Start %d to %d - [%d, %d] -> [%d, %d]", session->character->_ID, direction, pos.x, pos.y, session->character->_pos.x, session->character->_pos.y);
}

/*----------------------------------------------------------*/
// ::Function
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void RecvPacket_CS_Move_Stop(Session * session, StreamBuffer * payload)
{
	BYTE direction;
	Pos pos;
	*payload >> direction >> pos.x >> pos.y;

	// 위치 보정
	if (CheckDeadReckoning(session->character, pos))
	{
		session->character->_pos = DeadReckoning(session);
		LOG(Logger::LOG_LEVEL_WARNING, L"Sync Move_Stop [%d, %d] -> [%d, %d]", pos.x, pos.y, session->character->_pos.x, session->character->_pos.y);
		StreamBuffer * syncPacket;
		MakePacket_Sync(&syncPacket, session->character->_ID, session->character->_pos);
		UniCast(syncPacket, session);
		LOG(Logger::LOG_LEVEL_WARNING, L"UniCast Sync CS_Move_Stop %d", session->character->_ID);
	}

	session->character->_action = STAND;
	session->character->_direction = (TAG_ACTION)direction;
	session->character->_moveStartPos = session->character->_pos;
	session->character->_lastActionTime = GetTickCount64();
	
	StreamBuffer * packet;
	MakePacket_Move_Stop(&packet, session->character->_ID, direction, session->character->_pos);
	BroadCast_Sector(packet, session, false);
	LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Move Stop %d - [%d, %d] -> [%d, %d]", session->character->_ID, pos.x, pos.y, session->character->_pos.x, session->character->_pos.y);
}

/*----------------------------------------------------------*/
// ::Function
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void RecvPacket_CS_Attack1(Session * session, StreamBuffer * payload)
{
	BYTE direction;
	Pos pos;
	*payload >> direction >> pos.x >> pos.y;

	session->character->_action = STAND;

	// 위치 보정
	if (CheckDeadReckoning(session->character, pos))
	{
		session->character->_pos = DeadReckoning(session);
		LOG(Logger::LOG_LEVEL_WARNING, L"Sync Attack1 [%d, %d] -> [%d, %d]", pos.x, pos.y, session->character->_pos.x, session->character->_pos.y);
		StreamBuffer * syncPacket;
		MakePacket_Sync(&syncPacket, session->character->_ID, session->character->_pos);
		UniCast(syncPacket, session);
		LOG(Logger::LOG_LEVEL_WARNING, L"UniCast Sync Attack1 %d", session->character->_ID);
	}

	// 피격자 탐색
	int attackID = session->character->_ID;
	list<Player *>::iterator iter = g_playerList.begin();
	list<Player *>::iterator iterEnd = g_playerList.end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->_ID == attackID)
			continue;
		Pos damagePos = (*iter)->_pos;
		if ((direction == ACTION_MOVE_LL) && 
			(session->character->_pos.x - damagePos.x) < dfATTACK1_RANGE_X &&
			0 < (session->character->_pos.x - damagePos.x) &&
			(abs(session->character->_pos.y - damagePos.y) < dfATTACK1_RANGE_Y))
		{
			(*iter)->_healthPoint = max((*iter)->_healthPoint - ATTACK1_DAMAGE, 0);
			StreamBuffer * damagePacket;
			MakePacket_Damage(&damagePacket, attackID, (*iter)->_ID, (*iter)->_healthPoint);
			BroadCast_Sector(damagePacket, session, false);
			LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Damage %d -> %d", session->character->_ID, (*iter)->_ID);
		}
		else if (direction == ACTION_MOVE_RR &&
			(damagePos.x - session->character->_pos.x) < dfATTACK1_RANGE_X &&
			0 < (damagePos.x - session->character->_pos.x) &&
			(abs(session->character->_pos.y - damagePos.y) < dfATTACK1_RANGE_Y))
		{
			(*iter)->_healthPoint = max((*iter)->_healthPoint - ATTACK1_DAMAGE, 0);
			StreamBuffer * damagePacket;
			MakePacket_Damage(&damagePacket, attackID, (*iter)->_ID, (*iter)->_healthPoint);
			BroadCast_Sector(damagePacket, session, false);
			LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Damage %d -> %d", session->character->_ID, (*iter)->_ID);
		}
	}
	session->character->_moveStartPos = session->character->_pos;
	session->character->_lastActionTime = GetTickCount64();

	StreamBuffer * packet;
	MakePacket_Attack1(&packet, session->character->_ID, direction, session->character->_pos);
	BroadCast_Sector(packet, session, false);
	LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Attack1 %d - [%d, %d] -> [%d, %d]", session->character->_ID, pos.x, pos.y, session->character->_pos.x, session->character->_pos.y);
}

/*----------------------------------------------------------*/
// ::Function
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void RecvPacket_CS_Attack2(Session * session, StreamBuffer * payload)
{
	BYTE direction;
	Pos pos;
	*payload >> direction >> pos.x >> pos.y;

	session->character->_action = STAND;

	// 위치 보정
	if (CheckDeadReckoning(session->character, pos))
	{
		session->character->_pos = DeadReckoning(session);
		LOG(Logger::LOG_LEVEL_WARNING, L"Sync Attack2 [%d, %d] -> [%d, %d]", pos.x, pos.y, session->character->_pos.x, session->character->_pos.y);
		StreamBuffer * syncPacket;
		MakePacket_Sync(&syncPacket, session->character->_ID, session->character->_pos);
		UniCast(syncPacket, session);
		LOG(Logger::LOG_LEVEL_WARNING, L"UniCast Sync Attack2 %d", session->character->_ID);
	}

	// 피격자 탐색
	int attackID = session->character->_ID;
	list<Player *>::iterator iter = g_playerList.begin();
	list<Player *>::iterator iterEnd = g_playerList.end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->_ID == attackID)
			continue;
		Pos damagePos = (*iter)->_pos;
		if ((direction == ACTION_MOVE_LL) &&
			(session->character->_pos.x - damagePos.x) < dfATTACK2_RANGE_X &&
			0 < (session->character->_pos.x - damagePos.x) &&
			(abs(session->character->_pos.y - damagePos.y) < dfATTACK2_RANGE_Y))
		{
			(*iter)->_healthPoint = max((*iter)->_healthPoint - ATTACK2_DAMAGE, 0);
			StreamBuffer * damagePacket;
			MakePacket_Damage(&damagePacket, attackID, (*iter)->_ID, (*iter)->_healthPoint);
			BroadCast_Sector(damagePacket, session, false);
			LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Damage %d -> %d", session->character->_ID, (*iter)->_ID);
		}
		else if (direction == ACTION_MOVE_RR &&
			(damagePos.x - session->character->_pos.x) < dfATTACK2_RANGE_X &&
			0 < (damagePos.x - session->character->_pos.x) &&
			(abs(session->character->_pos.y - damagePos.y) < dfATTACK2_RANGE_Y))
		{
			(*iter)->_healthPoint = max((*iter)->_healthPoint - ATTACK2_DAMAGE, 0);
			StreamBuffer * damagePacket;
			MakePacket_Damage(&damagePacket, attackID, (*iter)->_ID, (*iter)->_healthPoint);
			BroadCast_Sector(damagePacket, session, false);
			LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Damage %d -> %d", session->character->_ID, (*iter)->_ID);
		}
	}
	session->character->_moveStartPos = session->character->_pos;
	session->character->_lastActionTime = GetTickCount64();

	StreamBuffer * packet;
	MakePacket_Attack2(&packet, session->character->_ID, direction, session->character->_pos);
	BroadCast_Sector(packet, session, false);
	LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Attack2 %d - [%d, %d] -> [%d, %d]", session->character->_ID, pos.x, pos.y, session->character->_pos.x, session->character->_pos.y);
}

/*----------------------------------------------------------*/
// ::Function
// 설명 : 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void RecvPacket_CS_Attack3(Session * session, StreamBuffer * payload)
{
	BYTE direction;
	Pos pos;
	*payload >> direction >> pos.x >> pos.y;

	session->character->_action = STAND;

	// 위치 보정
	if (CheckDeadReckoning(session->character, pos))
	{
		session->character->_pos = DeadReckoning(session);
		LOG(Logger::LOG_LEVEL_WARNING, L"Sync Attack3 [%d, %d] -> [%d, %d]", pos.x, pos.y, session->character->_pos.x, session->character->_pos.y);
		StreamBuffer * syncPacket;
		MakePacket_Sync(&syncPacket, session->character->_ID, session->character->_pos);
		UniCast(syncPacket, session);
		LOG(Logger::LOG_LEVEL_WARNING, L"UniCast Sync Attack3 %d", session->character->_ID);
	}

	// 피격자 탐색
	int attackID = session->character->_ID;
	list<Player *>::iterator iter = g_playerList.begin();
	list<Player *>::iterator iterEnd = g_playerList.end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->_ID == attackID)
			continue;
		Pos damagePos = (*iter)->_pos;
		if ((direction == ACTION_MOVE_LL) &&
			(session->character->_pos.x - damagePos.x) < dfATTACK3_RANGE_X &&
			0 < (session->character->_pos.x - damagePos.x) &&
			(abs(session->character->_pos.y - damagePos.y) < dfATTACK3_RANGE_Y))
		{
			(*iter)->_healthPoint = max((*iter)->_healthPoint - ATTACK3_DAMAGE, 0);
			StreamBuffer * damagePacket;
			MakePacket_Damage(&damagePacket, attackID, (*iter)->_ID, (*iter)->_healthPoint);
			BroadCast_Sector(damagePacket, session, false);
			LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Damage %d -> %d", session->character->_ID, (*iter)->_ID);
		}
		else if (direction == ACTION_MOVE_RR &&
			(damagePos.x - session->character->_pos.x) < dfATTACK3_RANGE_X &&
			0 < (damagePos.x - session->character->_pos.x) &&
			(abs(session->character->_pos.y - damagePos.y) < dfATTACK3_RANGE_Y))
		{
			(*iter)->_healthPoint = max((*iter)->_healthPoint - ATTACK3_DAMAGE, 0);
			StreamBuffer * damagePacket;
			MakePacket_Damage(&damagePacket, attackID, (*iter)->_ID, (*iter)->_healthPoint);
			BroadCast_Sector(damagePacket, session, false);
			LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Damage %d -> %d", session->character->_ID, (*iter)->_ID);
		}
	}
	session->character->_moveStartPos = session->character->_pos;
	session->character->_lastActionTime = GetTickCount64();

	StreamBuffer * packet;
	MakePacket_Attack3(&packet, session->character->_ID, direction, session->character->_pos);
	BroadCast_Sector(packet, session, false);
	LOG(Logger::LOG_LEVEL_DEBUG, L"BroadCast Attack3 %d - [%d, %d] -> [%d, %d]", session->character->_ID, pos.x, pos.y, session->character->_pos.x, session->character->_pos.y);
}

void RecvPacket_CS_Echo(Session * session, StreamBuffer * payload)
{
	DWORD time;
	*payload >> time;

	StreamBuffer * echoPacket;
	MakePacket_Echo(&echoPacket, time);
	UniCast(echoPacket, session);
	LOG(Logger::LOG_LEVEL_DEBUG, L"Echo %d", session->character->_ID);
}