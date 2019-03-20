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
// ���� : �ֻ��� ������Ʈ ��ü ������
// ���� : (Pos) ��ġ ����ü
// ���� :
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
// ���� : �ֻ��� ������Ʈ ��ü �Ҹ��� 
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
Object::~Object()
{
}

/*----------------------------------------------------------*/
// Object::NextFrame() (public)
// ���� : ������Ʈ�� ��������Ʈ������ ���� ������������ ó���Ѵ�.
// ���� : 
// ���� :
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
// ���� : �÷��̾ dircetion �������� ������ �̵��Ÿ���ŭ �̵�
// ���� : (int) ����
// ���� : (bool) �̵����� ����(�̿ϼ�)
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
// ���� : �÷��̾ type Ÿ������ ���ݻ��� ��ȯ�Ѵ�.
// ���� : (int) ����Ÿ��
// ���� : (bool) ���ݼ��� ����(�̿ϼ�)
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
// ���� : �÷��̾ ������ �ִ� Stand���·� ��ȯ�Ѵ�.
// ���� : 
// ���� :
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
// ���� : �÷��̾��� ������
//	��  : �׸��ڿ� HP�ٵ� �����Ѵ�
// ���� : (Pos) ������ġ ����ü
// ���� :
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
// ���� : �÷��̾��� ������
//  ��  : �����κ��� ���ڸ� ��Ŷ���� �޾� �����Ѵ�.
// ���� : (int) �ĺ���ȣ, (int) Stand����, (Pos) ������ġ ����ü, (int)ü��
// ���� :
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
// ���� : �÷��̾� �Ҹ���
//  ��  : ������Ʈ ����Ʈ�� ��ü��ȸ�ϸ鼭 �ڽ��� HP�� �׸��ڵ� �����Ѵ�.
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
Player::~Player()
{
	// ���� ��Ŷó���ο��� ĳ�����Ͽ� ȣ����
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
// ���� : �÷��̾��� �ൿ�� �����Ѵ�.
//  ��  : _command�� ����� �ൿ�� ���డ���ϸ� �����Ѵ�.
//  ��  : ���°� ��ȭ�ϸ� ������ ��Ŷ�� �����Ѵ�.
// ���� : 
// ���� : (bool) ������Ʈ �����÷���
/*----------------*////////////////////////*----------------*/
bool Player::Action()
{
	if (_isDead)
		return false;
	if (_endFrame)
		_canAction = true;

	// (��)���� ��ȭ�� ��Ŷ����
	if (_action != _command && _owner)
	{
		// �̵��߿� ���ݸ�ɽ� STAND����
		if (_action <= ACTION_MOVE_LD && (ATTACK_TYPE_1 <= _command && _command <= ATTACK_TYPE_3))
			MakeSendPacket(STAND);
		// �������� �� STAND�� ���� ��ȭ�ÿ��� �������� �ʴ´�
		if ((ATTACK_TYPE_1 <= _action && _action <= ATTACK_TYPE_3) && _command == STAND)
			;
		else
			MakeSendPacket(_command);
	}

	// command�� ���ݴ��� �ؾ������̶�� ����
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
// ���� : �÷��̾��� ������
//  ��  : ������ǥ -> ȭ����ǥ ��ȯ�Ͽ� �������Ѵ�.
// ���� : 
// ���� :
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
// ���� : �÷��̾�� �׸���, HP ��ǥ�̵�
//  ��  : �����κ��� ��ũ ��Ŷ�� ������ ȣ��
// ���� : (Pos) ��ǥ ����ü
// ���� :
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
// ���� : ����Ʈ�� ������
// ���� : (Pos) ������ġ ����ü, (SPRITE) ����Ʈ�� ��������Ʈ, 
//  ��  : (bool) ������ ����, (int) ó�� ������ ��������Ӽ�
// ���� :
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
// ���� : ����Ʈ�� �ൿ�� �����Ѵ�.
// ���� : 
// ���� :(bool) ������Ʈ �����÷���
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
// ���� : ����Ʈ�� ������
//  ��  : ����������� ���������� ����Ѵ�.
//  ��  : ������ǥ -> ȭ����ǥ�� ��ȯ�Ͽ� �������Ѵ�.
// ���� : 
// ���� :
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
// ���� : ����Ʈ�� �̵��� ó���Ѵ�.
//  ��  : �ڡ١ڡٹ̱����١ڡ١�
// ���� : (int) ����
// ���� :
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
// ���� : ����Ʈ�� ������ ������ �����Ѵ�.
// ���� : (int) �����
// ���� :
/*----------------*////////////////////////*----------------*/
void Effect::ChangeRate(int rate)
{
	_drawRate = rate;
}

/*----------------------------------------------------------*/
// Effect::ChangePosAndDelay
// ���� : ����Ʈ�� ��ġ�� ����������� �����Ѵ�.
//  ��  : ������ ��Ŷ���� ������ ����Ʈ�� ����� ��������� ������ ���� ���
// ���� : (int) ������ �ĺ���ȣ, (int) �ǰ��� �ĺ���ȣ
// ���� :
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

	// �ǰ��ڰ� ����
	if (iterDamage != g_list.end())
	{
		_pos = (*iterDamage)->GetPos();
		// �����ڰ� ����
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
	// ������ �� ����
	else
		delete this;
	
}

//////////////////////////////////////////////////////////////
// Map : public Object
//////////////////////////////////////////////////////////////

/*----------------------------------------------------------*/
// Map::Map
// ���� : �� ������Ʈ ������
// ���� : (Pos) ������ġ ����ü
// ���� :
/*----------------*////////////////////////*----------------*/
Map::Map(Pos pos) : Object(pos)
{
	pos = _pos;
	_type = TYPE::MAP;
	_curSprite = SPRITE::MAP;
}

/*----------------------------------------------------------*/
// Map::Action (virtual)
// ���� : ���� �ൿ�� �����Ѵ�.
// ���� : 
// ���� : (bool) ������Ʈ �����÷���
/*----------------*////////////////////////*----------------*/
bool Map::Action()
{
	return true;
}

/*----------------------------------------------------------*/
// Map::Draw (virtual)
// ���� : ���� �������Ѵ�.
//  ��  : ���� ������Ʈ ����Ʈ���� �Ǿտ� �־����
// ���� : 
// ���� :
/*----------------*////////////////////////*----------------*/
void Map::Draw()
{
	sprite->DrawImage(_curSprite, _pos.x, _pos.y, screen->GetByteBuffer(), SCREEN_X, SCREEN_Y, SCREEN_X * 4);
}
