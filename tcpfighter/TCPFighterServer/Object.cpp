#pragma once
#include "stdafx.h"
extern map<SOCKET, Session *> g_user_socket;
extern list<Player *> g_playerList;

/*----------------------------------------------------------*/
// Session::Session (public)
// 설명 : 세션 구조체 생성자
// 인자 : (SOCKET) 소켓, (SOCKADDR_IN) 소켓정보
// 리턴 :
/*----------------*////////////////////////*----------------*/
Session::Session(SOCKET socket, SOCKADDR_IN addr)
{
	this->socket = socket;
	this->addr = addr;
	character = nullptr;
	saveHeaderFlag = false;
	lastRecvTime = 0;
	g_user_socket.insert(pair<SOCKET, Session*>(socket, this));
}

/*----------------------------------------------------------*/
// Player::Move (protected)
// 설명 : 플레이어의 이동함수
//  ㄴ  : 현재위치에서 x,y를 더한다.
// 인자 : (short) x값, (short) y값
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Player::Move(short x, short y)
{
	Pos oldPos = _pos;
	_pos.x += x;
	_pos.y += y;
	if (_pos.x < 0 || MAP_X_SIZE < _pos.x)
		_pos = oldPos;
	if (_pos.y < 0 || MAP_Y_SIZE < _pos.y)
		_pos = oldPos;

	_newSector = SECTOR_MANAGER.CalculateSector(this);
	if (SECTOR_MANAGER.UpdateSector(this))
	{
		SECTOR_MANAGER.ChangeSector(this);
		_oldSector = _newSector;
	}
}

/*----------------------------------------------------------*/
// Player::Player (public)
// 설명 : 플레이어 구조체 생성자
//  ㄴ  : 세션에 자신을 등록하고 플레이어 전역 리스트에 넣는다.
// 인자 : (Session *) 세션 포인터, (int) 고유번호
// 리턴 :
/*----------------*////////////////////////*----------------*/
Player::Player(Session * session, int ID)
{
	_session = session;
	_ID = ID;
	_pos = Pos{ 250, 150 };
	_pos = Pos{ rand() % 6400, rand() % 6400 };
	_moveStartPos = _pos;
	_direction = ACTION_MOVE_LL;
	_healthPoint = MAX_HP;
	_action = STAND;
	_lastActionTime = GetTickCount64();
	session->character = this;
	g_playerList.push_back(this);
	_oldSector = SECTOR_MANAGER.CalculateSector(this);
	_newSector = _oldSector;
}

/*----------------------------------------------------------*/
// Player::~Player (virtual) (public)
// 설명 : 플레이어 구조체 소멸자
//  ㄴ  : 플레이어 전역 리스트에서 삭제한다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
Player::~Player()
{
	list<Player *>::iterator iter = g_playerList.begin();
	list<Player *>::iterator iterEnd = g_playerList.end();
	for (; iter != iterEnd;)
	{
		if ((*iter) == this)
		{
			g_playerList.erase(iter);
			break;
		}
		++iter;
	}
}

/*----------------------------------------------------------*/
// Player::Action (public)
// 설명 : 플레이어의 행동함수
//  ㄴ  : 서버의 매 프레임마다 할 행동을 정의한다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
bool Player::Action()
{
	switch (_action)
	{
	case ACTION_MOVE_LL:
		Move(-MOVE_TICK_X, 0);
		break;
	case ACTION_MOVE_LU:
		Move(-MOVE_TICK_X, -MOVE_TICK_Y);
		break;
	case ACTION_MOVE_UU:
		Move(0, -MOVE_TICK_Y);
		break;
	case ACTION_MOVE_RU:
		Move(MOVE_TICK_X, -MOVE_TICK_Y);
		break;
	case ACTION_MOVE_RR:
		Move(MOVE_TICK_X, 0);
		break;
	case ACTION_MOVE_RD:
		Move(MOVE_TICK_X, MOVE_TICK_Y);
		break;
	case ACTION_MOVE_DD:
		Move(0, MOVE_TICK_Y);
		break;
	case ACTION_MOVE_LD:
		Move(-MOVE_TICK_X, MOVE_TICK_Y);
		break;
	case STAND:
		break;
	default:
		break;
	}
	return true;
}