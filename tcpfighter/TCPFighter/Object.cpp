#include "stdafx.h"
#include "Object.h"

extern ScreenDIB *screen;
extern SpriteDIB *sprite;
extern CList<Object *>	g_list;
extern Player* g_player;
extern TileMap * g_tileMap;

//////////////////////////////////////////////////////////////
// Object
//////////////////////////////////////////////////////////////

/*----------------------------------------------------------*/
// Object::Object (public)
// 설명 : 최상위 오브젝트 객체 생성자
// 인자 : (Pos) 위치 구조체
// 리턴 :
/*----------------*////////////////////////*----------------*/
Object::Object(Pos pos)
{
	_isDead = false;
	_pos = pos;
	_oldPos = pos;
	_frameDelay = 0;
	_delayCount = 0;
	_endFrame = false;
	_renderColor = 0;
	g_list.push_front(this);
}

/*----------------------------------------------------------*/
// Object::~Object() (virtual) (public)
// 설명 : 최상위 오브젝트 객체 소멸자 
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
Object::~Object()
{
}

/*----------------------------------------------------------*/
// Object::NextFrame() (public)
// 설명 : 오브젝트의 스프라이트변경을 위한 프레임진행을 처리한다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Object::NextFrame()
{
	++_delayCount;
	if (_frameDelay < _delayCount)
	{
		_delayCount = 0;
		++_curSprite;

		if (_endSprite <= _curSprite)
		{
			_curSprite = _startSprite;
			_endFrame = true;
		}
	}
}

//////////////////////////////////////////////////////////////
// Player : public Object
//////////////////////////////////////////////////////////////

/*----------------------------------------------------------*/
// Player::Move (protected)
// 설명 : 플레이어를 dircetion 방향으로 정해진 이동거리만큼 이동
// 인자 : (int) 방향
// 리턴 : (bool) 이동성공 여부(미완성)
/*----------------*////////////////////////*----------------*/
bool Player::Move(int direction)
{
	_endFrame = false;
	_frameDelay = dfDELAY_MOVE;
	switch (direction)
	{
	case ACTION_MOVE_LL:
	{
		_direction = ACTION_MOVE_LL;
		if (_startSprite != PLAYER_MOVE_L01)
		{
			_startSprite = PLAYER_MOVE_L01;
			_curSprite = PLAYER_MOVE_L01;
			_endSprite = PLAYER_MOVE_L_MAX;
		}
		_pos.x -= 3;
		if (_pos.x <= dfRANGE_MOVE_LEFT)
			_pos.x = _oldPos.x;
		else
			_oldPos.x = _pos.x;
		break;
	}
	case ACTION_MOVE_LU:
	{
		_direction = ACTION_MOVE_LL;
		if (_startSprite != PLAYER_MOVE_L01)
		{
			_startSprite = PLAYER_MOVE_L01;
			_curSprite = PLAYER_MOVE_L01;
			_endSprite = PLAYER_MOVE_L_MAX;
		}
		_pos.x -= 3;
		_pos.y -= 2;
		if (_pos.x <= dfRANGE_MOVE_LEFT || _pos.y <= dfRANGE_MOVE_TOP)
			_pos = _oldPos;
		else
			_oldPos = _pos;
		break;
	}
	case ACTION_MOVE_UU:
	{
		switch (_direction)
		{
		case ACTION_MOVE_LL:
			if (_startSprite != PLAYER_MOVE_L01)
			{
				_startSprite = PLAYER_MOVE_L01;
				_curSprite = PLAYER_MOVE_L01;
				_endSprite = PLAYER_MOVE_L_MAX;
			}
			break;
		case ACTION_MOVE_RR:
			if (_startSprite != PLAYER_MOVE_R01)
			{
				_startSprite = PLAYER_MOVE_R01;
				_curSprite = PLAYER_MOVE_R01;
				_endSprite = PLAYER_MOVE_R_MAX;
			}
			break;
		}
		_pos.y -= 2;
		if (_pos.y <= dfRANGE_MOVE_TOP)
			_pos.y = _oldPos.y;
		else
			_oldPos.y = _pos.y;
		break;
	}
	case ACTION_MOVE_RU:
	{
		_direction = ACTION_MOVE_RR;
		if (_startSprite != PLAYER_MOVE_R01)
		{
			_startSprite = PLAYER_MOVE_R01;
			_curSprite = PLAYER_MOVE_R01;
			_endSprite = PLAYER_MOVE_R_MAX;
		}
		_pos.x += 3;
		_pos.y -= 2;
		if (dfRANGE_MOVE_RIGHT <= _pos.x || _pos.y <= dfRANGE_MOVE_TOP)
			_pos = _oldPos;
		else
			_oldPos = _pos;
		break;
	}
	case ACTION_MOVE_RR:
	{
		_direction = ACTION_MOVE_RR;
		if (_startSprite != PLAYER_MOVE_R01)
		{
			_startSprite = PLAYER_MOVE_R01;
			_curSprite = PLAYER_MOVE_R01;
			_endSprite = PLAYER_MOVE_R_MAX;
		}
		_pos.x += 3;
		if (dfRANGE_MOVE_RIGHT <= _pos.x)
			_pos = _oldPos;
		else
			_oldPos.x = _pos.x;

		break;
	}
	case ACTION_MOVE_RD:
	{
		_direction = ACTION_MOVE_RR;
		if (_startSprite != PLAYER_MOVE_R01)
		{
			_startSprite = PLAYER_MOVE_R01;
			_curSprite = PLAYER_MOVE_R01;
			_endSprite = PLAYER_MOVE_R_MAX;
		}
		_pos.x += 3;
		_pos.y += 2;
		if (dfRANGE_MOVE_RIGHT <= _pos.x || dfRANGE_MOVE_BOTTOM <= _pos.y)
			_pos = _oldPos;
		else
			_oldPos = _pos;
		break;
	}
	case ACTION_MOVE_DD:
	{		
		switch (_direction)
		{
		case ACTION_MOVE_LL:
			if (_startSprite != PLAYER_MOVE_L01)
			{
				_startSprite = PLAYER_MOVE_L01;
				_curSprite = PLAYER_MOVE_L01;
				_endSprite = PLAYER_MOVE_L_MAX;
			}
			break;
		case ACTION_MOVE_RR:
			if (_startSprite != PLAYER_MOVE_R01)
			{
				_startSprite = PLAYER_MOVE_R01;
				_curSprite = PLAYER_MOVE_R01;
				_endSprite = PLAYER_MOVE_R_MAX;
			}
			break;
		}
		_pos.y += 2;
		if (dfRANGE_MOVE_BOTTOM <= _pos.y)
			_pos.y = _oldPos.y;
		else
			_oldPos.y = _pos.y;
		break;
	}
	case ACTION_MOVE_LD:
	{
		_direction = ACTION_MOVE_LL;
		if (_startSprite != PLAYER_MOVE_L01)
		{
			_startSprite = PLAYER_MOVE_L01;
			_curSprite = PLAYER_MOVE_L01;
			_endSprite = PLAYER_MOVE_L_MAX;
		}
		_pos.x -= 3;
		_pos.y += 2;
		if (_pos.x <= dfRANGE_MOVE_LEFT || dfRANGE_MOVE_BOTTOM <= _pos.y)
			_pos = _oldPos;
		else
			_oldPos = _pos;
		break;
	}
	}
	_shadow->SetPos(_pos);
	Pos hppos = _pos;
	hppos.x += (-35);
	hppos.y += 10;
	_HPGuage->SetPos(hppos);
	return true;
}

/*----------------------------------------------------------*/
// Player::Attack (protected)
// 설명 : 플레이어를 type 타입으로 공격상태 전환한다.
// 인자 : (int) 공격타입
// 리턴 : (bool) 공격성공 여부(미완성)
/*----------------*////////////////////////*----------------*/
bool Player::Attack(int type)
{
	_endFrame = false;
	_canAction = false;
	_delayCount = 0;

	Pos effectPos = _pos;
	switch(type)
	{
	case ATTACK_TYPE_1:
		_frameDelay = dfDELAY_ATTACK1;
		switch (_direction)
		{
		case ACTION_MOVE_LL:
			_startSprite = PLAYER_ATTACK1_L01;
			_curSprite = PLAYER_ATTACK1_L01;
			_endSprite = PLAYER_ATTACK1_L_MAX;
			break;
		case ACTION_MOVE_RR:
			_startSprite = PLAYER_ATTACK1_R01;
			_curSprite = PLAYER_ATTACK1_R01;
			_endSprite = PLAYER_ATTACK1_R_MAX;
			break;
		}
		break;
	case ATTACK_TYPE_2:
		_frameDelay = dfDELAY_ATTACK2;
		switch (_direction)
		{
		case ACTION_MOVE_LL:
			_startSprite = PLAYER_ATTACK2_L01;
			_curSprite = PLAYER_ATTACK2_L01;
			_endSprite = PLAYER_ATTACK2_L_MAX;
			break;
		case ACTION_MOVE_RR:
			_startSprite = PLAYER_ATTACK2_R01;
			_curSprite = PLAYER_ATTACK2_R01;
			_endSprite = PLAYER_ATTACK2_R_MAX;
			break;
		}
		break;
	case ATTACK_TYPE_3:
		_frameDelay = dfDELAY_ATTACK3;
		switch (_direction)
		{
		case ACTION_MOVE_LL:
			_startSprite = PLAYER_ATTACK3_L01;
			_curSprite = PLAYER_ATTACK3_L01;
			_endSprite = PLAYER_ATTACK3_L_MAX;
			break;
		case ACTION_MOVE_RR:
			_startSprite = PLAYER_ATTACK3_R01;
			_curSprite = PLAYER_ATTACK3_R01;
			_endSprite = PLAYER_ATTACK3_R_MAX;
			break;
		}
		break;
	}
	//Effect * spark = new Effect(effectPos, EFFECT_SPARK_01, effectDelay);

	return true;
}

/*----------------------------------------------------------*/
// Player::Stand (protected)
// 설명 : 플레이어를 방향이 있는 Stand상태로 전환한다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Player::Stand()
{
	_frameDelay = dfDELAY_STAND;
	switch (_direction)
	{
	case ACTION_MOVE_LL:
		if (_startSprite == PLAYER_STAND_L01)
			break;
		_startSprite = PLAYER_STAND_L01;
		_curSprite = PLAYER_STAND_L01;
		_endSprite = PLAYER_STAND_L_MAX;
		break;
	case ACTION_MOVE_RR:
		if (_startSprite == PLAYER_STAND_R01)
			break;
		_startSprite = PLAYER_STAND_R01;
		_curSprite = PLAYER_STAND_R01;
		_endSprite = PLAYER_STAND_R_MAX;
		break;
	}
}

/*----------------------------------------------------------*/
// Player::Player (public)
// 설명 : 플레이어의 생성자
//	ㄴ  : 그림자와 HP바도 생성한다
// 인자 : (Pos) 생성위치 구조체
// 리턴 :
/*----------------*////////////////////////*----------------*/
Player::Player(Pos pos) : Object(pos)
{
	_id = 0;
	_type = PLAYER;
	_owner = false;
	_action = STAND;
	_healthPoint = 100;
	_direction = ACTION_MOVE_LL;
	_canAction = true;

	_startSprite = PLAYER_STAND_L01;
	_curSprite = PLAYER_STAND_L01;
	_endSprite = PLAYER_STAND_L_MAX;

	Pos hppos = pos;
	hppos.x -= 35;
	hppos.y += 10;
	_shadow = new Effect(pos, SHADOW);
	_HPGuage = new Effect(hppos, GUAGE_HP);
	
	printf_s("Player Created\n");
}

/*----------------------------------------------------------*/
// Player::Player (public)
// 설명 : 플레이어의 생성자
//  ㄴ  : 서버로부터 인자를 패킷으로 받아 생성한다.
// 인자 : (int) 식별번호, (int) Stand방향, (Pos) 생성위치 구조체, (int)체력
// 리턴 :
/*----------------*////////////////////////*----------------*/
Player::Player(int id, int direction, Pos pos, int healthPoint) : Object(pos)
{
	_id = id;
	_type = PLAYER;
	_owner = false;
	_command = STAND;
	_action = STAND;
	_healthPoint = healthPoint;
	if(direction == dfPACKET_MOVE_DIR_LL)
		_direction = ACTION_MOVE_LL;
	else if(direction == dfPACKET_MOVE_DIR_RR)
		_direction = ACTION_MOVE_RR;
	else 
		_direction = ACTION_MOVE_LL;
	_canAction = true;

	_startSprite = PLAYER_STAND_L01;
	_curSprite = PLAYER_STAND_L01;
	_endSprite = PLAYER_STAND_L_MAX;

	Pos hppos = pos;
	hppos.x -= 35;
	hppos.y += 10;
	_shadow = new Effect(pos, SHADOW, true);
	_HPGuage = new Effect(hppos, GUAGE_HP);

	printf_s("Player Created\n");
}

/*----------------------------------------------------------*/
// Player::~Player (virtual) (public)
// 설명 : 플레이어 소멸자
//  ㄴ  : 오브젝트 리스트를 전체순회하면서 자신의 HP와 그림자도 삭제한다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
Player::~Player()
{
	// 현재 패킷처리부에서 캐스팅하여 호출중
	CList<Object *>::iterator iter;
	for (iter = g_list.begin(); iter != g_list.end();)
	{
		if ((*iter) == _shadow || (*iter) == _HPGuage)
		{
			delete (*iter);
			iter = g_list.erase(iter);
		}		
		else
			++iter;
	}
	printf_s("Player Deleted\n");
}

/*----------------------------------------------------------*/
// Player::Action (virtual) (public)
// 설명 : 플레이어의 행동을 정의한다.
//  ㄴ  : _command에 저장된 행동을 수행가능하면 수행한다.
//  ㄴ  : 상태가 변화하면 서버로 패킷을 전송한다.
// 인자 : 
// 리턴 : (bool) 오브젝트 삭제플래그
/*----------------*////////////////////////*----------------*/
bool Player::Action()
{
	if (_isDead)
		return false;
	if (_endFrame)
		_canAction = true;

	// (내)상태 변화시 패킷전송
	if (_action != _command && _owner)
	{
		// 이동중에 공격명령시 STAND전송
		if (_action <= ACTION_MOVE_LD && (ATTACK_TYPE_1 <= _command && _command <= ATTACK_TYPE_3))
			MakeSendPacket(STAND);
		// 공격종료 후 STAND로 상태 변화시에는 전송하지 않는다
		if ((ATTACK_TYPE_1 <= _action && _action <= ATTACK_TYPE_3) && _command == STAND)
			;
		else
			MakeSendPacket(_command);
	}

	// command는 지금당장 해야할일이라고 가정
	switch (_command)
	{
	case ACTION_MOVE_LL:
	case ACTION_MOVE_LU:
	case ACTION_MOVE_UU:
	case ACTION_MOVE_RU:
	case ACTION_MOVE_RR:
	case ACTION_MOVE_RD:
	case ACTION_MOVE_DD:
	case ACTION_MOVE_LD:
		Move(_command);
		_action = _command;
		break;
	case ATTACK_TYPE_1:
	case ATTACK_TYPE_2:
	case ATTACK_TYPE_3:
		Attack(_command);
		_action = _command;
		_command = NONE;
		break;
	case STAND:
		Stand();
		_action = _command;
		_command = NONE;
		break;
	case NONE:
		if (_endFrame)
		{
			Stand();
			_action = STAND;
			_command = STAND;
		}
		break;
	default:
		break;
	}
	_HPGuage->ChangeRate(_healthPoint);
	
	return true;
}

/*----------------------------------------------------------*/
// Player::Draw (virtual) (public)
// 설명 : 플레이어의 렌더링
//  ㄴ  : 월드좌표 -> 화면좌표 변환하여 렌더링한다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Player::Draw()
{
	Pos cameraCoordinate = g_tileMap->GetCameraPos();
	cameraCoordinate.x = _pos.x - cameraCoordinate.x;
	cameraCoordinate.y = _pos.y - cameraCoordinate.y;

	if(_owner)
		sprite->DrawSpriteRed(_curSprite, cameraCoordinate.x, cameraCoordinate.y, screen->GetByteBuffer(), SCREEN_X, SCREEN_Y, SCREEN_X * 4);
	else
	{
		switch (_renderColor)
		{
		case NORMAL:
			sprite->DrawSprite(_curSprite, cameraCoordinate.x, cameraCoordinate.y, screen->GetByteBuffer(), SCREEN_X, SCREEN_Y, SCREEN_X * 4);
			break;
		case RED:
			sprite->DrawSpriteRed(_curSprite, cameraCoordinate.x, cameraCoordinate.y, screen->GetByteBuffer(), SCREEN_X, SCREEN_Y, SCREEN_X * 4);
			break;
		}
	}
	NextFrame();
}

/*----------------------------------------------------------*/
// Player::ForceSetPos (public)
// 설명 : 플레이어와 그림자, HP 좌표이동
//  ㄴ  : 서버로부터 싱크 패킷을 받으면 호출
// 인자 : (Pos) 좌표 구조체
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Player::ForceSetPos(Pos pos)
{
	SetPos(pos);
	_shadow->SetPos(pos);
	_HPGuage->SetPos(Pos{ pos.x - 35, pos.y + 10 });
}

//////////////////////////////////////////////////////////////
// Effect : public Object
//////////////////////////////////////////////////////////////

/*----------------------------------------------------------*/
// Effect::Effect (public)
// 설명 : 이펙트의 생성자
// 인자 : (Pos) 생성위치 구조체, (SPRITE) 이펙트의 스프라이트, 
//  ㄴ  : (bool) 반투명 여부, (int) 처음 렌더링 대기프레임수
// 리턴 :
/*----------------*////////////////////////*----------------*/
Effect::Effect(Pos pos, SPRITE sprite, bool isHalfAlpha, int waitFrame) : Object(pos)
{
	_pos = pos;
	_frameDelay = dfDELAY_EFFECT;
	_type = EFFECT;
	_drawRate = 100;
	_waitFrame = waitFrame;
	_startSprite = sprite;
	_curSprite = sprite;
	_isHalfAlpha = isHalfAlpha;
	switch (sprite)
	{
	case EFFECT_SPARK_01:
		_infinite = false;
		_endSprite = EFFECT_SPARK_MAX;
		break;
	case GUAGE_HP:
	case SHADOW:
		_infinite = true;
		_endSprite = sprite;
		break;
	}
}

/*----------------------------------------------------------*/
// Effect::Action (virtual) (public)
// 설명 : 이펙트의 행동을 정의한다.
// 인자 : 
// 리턴 :(bool) 오브젝트 삭제플래그
/*----------------*////////////////////////*----------------*/
bool Effect::Action()
{
	if (_isDead)
		return false;
	if (!_infinite && _endFrame)
		return false;
	if (0 < _waitFrame)
		--_waitFrame;
	return true;
}

/*----------------------------------------------------------*/
// Effect::Draw (virtual) (public)
// 설명 : 이펙트의 렌더링
//  ㄴ  : 대기프레임이 남아있으면 대기한다.
//  ㄴ  : 월드좌표 -> 화면좌표로 변환하여 렌더링한다.
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Effect::Draw()
{
	Pos cameraCoordinate = g_tileMap->GetCameraPos();
	cameraCoordinate.x = _pos.x - cameraCoordinate.x;
	cameraCoordinate.y = _pos.y - cameraCoordinate.y;

	if (_waitFrame == 0)
	{
		switch (_renderColor)
		{
		case NORMAL:
			if (_isHalfAlpha)
				sprite->DrawSpriteHalfAlpha(_curSprite, cameraCoordinate.x, cameraCoordinate.y, screen->GetByteBuffer(), SCREEN_X, SCREEN_Y, SCREEN_X * 4, _drawRate);
			else
				sprite->DrawSprite(_curSprite, cameraCoordinate.x, cameraCoordinate.y, screen->GetByteBuffer(), SCREEN_X, SCREEN_Y, SCREEN_X * 4, _drawRate);
			break;
		case RED:
			sprite->DrawSpriteRed(_curSprite, cameraCoordinate.x, cameraCoordinate.y, screen->GetByteBuffer(), SCREEN_X, SCREEN_Y, SCREEN_X * 4, _drawRate);
			break;
		}
		NextFrame();
	}
}

/*----------------------------------------------------------*/
// Effect::Move
// 설명 : 이펙트의 이동을 처리한다.
//  ㄴ  : ★☆★☆미구현☆★☆★
// 인자 : (int) 방향
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Effect::Move(int direction)
{
	switch (direction)
	{
	case ACTION_MOVE_LL:
	case ACTION_MOVE_LU:
	case ACTION_MOVE_LD:
	case ACTION_MOVE_UU:
	case ACTION_MOVE_RR:
	case ACTION_MOVE_RU:
	case ACTION_MOVE_RD:
	case ACTION_MOVE_DD:
		break;
	}
}

/*----------------------------------------------------------*/
// Effect::ChangeRate
// 설명 : 이펙트의 렌더링 비율을 조절한다.
// 인자 : (int) 백분율
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Effect::ChangeRate(int rate)
{
	_drawRate = rate;
}

/*----------------------------------------------------------*/
// Effect::ChangePosAndDelay
// 설명 : 이펙트의 위치와 대기프레임을 변경한다.
//  ㄴ  : 데미지 패킷에서 데미지 이펙트를 만들고 멤버변수값 수정을 위해 사용
// 인자 : (int) 공격자 식별번호, (int) 피격자 식별번호
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Effect::ChangePosAndDelay(int AttackID, int DamageID)
{
	CList<Object *>::iterator iterAttack = g_list.end();
	CList<Object *>::iterator iterDamage = g_list.end();
	CList<Object *>::iterator iter = g_list.begin();
	for (; iter != g_list.end();)
	{
		if (((Player*)(*iter))->GetID() == AttackID)
			iterAttack = iter;
		else if (((Player*)(*iter))->GetID() == DamageID)
			iterDamage = iter;
		++iter;
	}

	// 피격자가 존재
	if (iterDamage != g_list.end())
	{
		_pos = (*iterDamage)->GetPos();
		// 공격자가 존재
		if (iterAttack != g_list.end())
		{
			Pos AttackPos = (*iterAttack)->GetPos();

			int diffX = AttackPos.x - _pos.x;
			int diffY = AttackPos.y - _pos.y;
			_pos.x += diffX / 3;
			_pos.y += diffY;

			switch (((Player*)(*iterAttack))->GetAction())
			{
			case ATTACK_TYPE_1:
				_waitFrame = 4;
				break;
			case ATTACK_TYPE_2:
				_waitFrame = 8;
				break;
			case ATTACK_TYPE_3:
				_waitFrame = 12;
				break;
			default:
				break;
			}
		}
		_pos.y -= 70;
	}
	// 없으면 걍 지움
	else
		delete this;
	
}

//////////////////////////////////////////////////////////////
// Map : public Object
//////////////////////////////////////////////////////////////

/*----------------------------------------------------------*/
// Map::Map
// 설명 : 맵 오브젝트 생성자
// 인자 : (Pos) 생성위치 구조체
// 리턴 :
/*----------------*////////////////////////*----------------*/
Map::Map(Pos pos) : Object(pos)
{
	pos = _pos;
	_type = TYPE::MAP;
	_curSprite = SPRITE::MAP;
}

/*----------------------------------------------------------*/
// Map::Action (virtual)
// 설명 : 맵의 행동을 정의한다.
// 인자 : 
// 리턴 : (bool) 오브젝트 삭제플래그
/*----------------*////////////////////////*----------------*/
bool Map::Action()
{
	return true;
}

/*----------------------------------------------------------*/
// Map::Draw (virtual)
// 설명 : 맵을 렌더링한다.
//  ㄴ  : 맵은 오브젝트 리스트에서 맨앞에 있어야함
// 인자 : 
// 리턴 :
/*----------------*////////////////////////*----------------*/
void Map::Draw()
{
	sprite->DrawImage(_curSprite, _pos.x, _pos.y, screen->GetByteBuffer(), SCREEN_X, SCREEN_Y, SCREEN_X * 4);
}
