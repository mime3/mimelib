#pragma once

struct Pos
{
	int x;
	int y;
};

//////////////////////////////////////////////////////////////
// Object
//////////////////////////////////////////////////////////////

class Object
{
public:
	enum TYPE
	{
		MAP,
		PLAYER,
		EFFECT
	};
	enum RENDER_COLOR
	{
		NORMAL,
		RED
	};
private:
protected:
	bool	_isDead;				// True�� ���� Action()���� False��ȯ
	Pos		_pos;					// ���� ��ġ
	Pos		_oldPos;				// �̵� �Ұ����� �̵��� �ǵ������
	TYPE	_type;					// Ÿ�� �����
	int		_startSprite;			// �ݺ��� ���� ù ��������Ʈ����
	int		_curSprite;				// ���� ��������Ʈ
	int		_endSprite;				// �� ��������Ʈ
	int		_frameDelay;			// ���罺������Ʈ�� ���� ������
	int		_delayCount;			// ������ī��Ʈ��
	bool	_endFrame;				// �� ��Ʈ�������� �ּ� �ѹ� �����ߴ��� ����
	char	_renderColor;			// ������ �÷� �÷���

public:
	Object(Pos pos);
	virtual ~Object();
	virtual bool Action() = 0;
	virtual void Draw() = 0;
	// ��ġ ����
	Pos GetPos();			
	// Ÿ�� ����
	TYPE GetType();					
	// ��������Ʈ ����
	int GetSprite();				
	// ������ ����
	void NextFrame();				
	// ��ġ ��������
	void SetPos(Pos pos);			
	// ������ �÷� �÷��� ����
	void ChangeColor(char color);	
	// dead�÷��� ����
	void Die();						
};

//////////////////////////////////////////////////////////////
// Effect : publie Object
//////////////////////////////////////////////////////////////

class Effect : public Object
{
	bool _infinite;					// �������ӿ���
	bool _isHalfAlpha;				// ��������
	int  _drawRate;					// ��������Ʈ ��º���
	int	 _waitFrame;				// ù ��������Ʈ ���� �����ð�
public:
	Effect(Pos pos, SPRITE sprite, bool isHalfAlpha = false, int waitFrame = 0);
	virtual bool Action() override;
	virtual void Draw() override;
	// _infinite ����
	bool GetInfinite();
	// ����Ʈ �̵�
	void Move(int direction);
	// ����Ʈ ��º��� ����
	void ChangeRate(int rate);
	// ����Ʈ ��ġ�� ��������� ����
	void ChangePosAndDelay(int AttackID, int DamageID);
};

//////////////////////////////////////////////////////////////
// Player : publie Object
//////////////////////////////////////////////////////////////

class Player : public Object
{
private:
	int			_id;				// �ĺ���ȣ
	bool		_owner;				// �÷��̾� üũ��
	int			_healthPoint;		// HP
	int			_direction;			// Stand�� �Ĵٺ��� ����
	bool		_canAction;			// action ���డ�� ����
	TAG_ACTION  _command;			// ���� ���
	TAG_ACTION	_action;			// ���� ��� -> ���� �������� ���
	Effect		*_shadow;			// �ڱ� �׸��� ������
	Effect		*_HPGuage;			// �ڱ� hp�� ������
protected:
	bool Move(int direction);		// action�� ���� ȣ��
	bool Attack(int type);			// action�� ���� ȣ��
	void Stand();
public:
	Player(Pos pos);
	Player(int id, int direction, Pos pos, int healthPoint);
	~Player() ;
	// ���õ� _action ���� �ൿ
	virtual bool Action() override;			
	// ������
	virtual void Draw() override;			
	// ���� action, _command ����
	inline void Command(TAG_ACTION tag);
	// ID��������
	inline int GetID();
	// HP��������
	inline int GetHealthPoint();
	// ���� char�� ����
	inline char GetDirection();
	// ����׼� ����
	inline TAG_ACTION GetAction();
	// ��ġ ���� ����
	void ForceSetPos(Pos pos);
	// �ڽ��� �÷��̾� �÷��� Ȱ��ȭ
	inline void SetOwner();
	// Stand���� ��ȯ
	inline void ChangeDirection(TAG_ACTION tag);
	// HP����
	inline void ChangeHP(int healthPoint);
};

//////////////////////////////////////////////////////////////
// Map : publie Object
//////////////////////////////////////////////////////////////

class Map : public Object
{
public:
	Map(Pos pos);
	virtual bool Action() override;
	virtual void Draw() override;
};

///////////////////////////////////////////////////////////////
// Inline Method
///////////////////////////////////////////////////////////////
inline Pos Object::GetPos()
{
	return _pos;
}

inline Object::TYPE Object::GetType()
{
	return _type;
}

inline int Object::GetSprite()
{
	return _curSprite;
}

inline void Object::SetPos(Pos pos)
{
	_pos = pos;
}

inline void Object::ChangeColor(char color)
{
	_renderColor = color;
}

inline void Object::Die()
{
	_isDead = true;
}

inline bool Effect::GetInfinite()
{
	return _infinite;
}

inline void Player::Command(TAG_ACTION tag)
{
	if (_owner)
	{
		if (_canAction)
			_command = tag;
	}
	else
		_command = tag;
}

inline void Player::SetOwner()
{
	_owner = true;
}

inline void Player::ChangeDirection(TAG_ACTION tag)
{
	switch (tag)
	{
		case ACTION_MOVE_LL	:
		case ACTION_MOVE_LU	:	
			_direction = ACTION_MOVE_LL;
			break;
		case ACTION_MOVE_UU	:	
			break;
		case ACTION_MOVE_RU	:		
		case ACTION_MOVE_RR	:		
		case ACTION_MOVE_RD	:		
			_direction = ACTION_MOVE_RR;
			break;
		case ACTION_MOVE_DD	:		
			break;
		case ACTION_MOVE_LD	:
			_direction = ACTION_MOVE_LL;
			break;
		default:
			break;
	}
}

inline void Player::ChangeHP(int healthPoint)
{
	_healthPoint = healthPoint;
}

inline int Player::GetID()
{
	return _id;
}

inline int Player::GetHealthPoint()
{
	return _healthPoint;
}

inline TAG_ACTION Player::GetAction()
{
	return _action;
}

inline char Player::GetDirection()
{
	return (char)_direction;
}
