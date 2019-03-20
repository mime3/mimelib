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
	bool	_isDead;				// True시 다음 Action()에서 False반환
	Pos		_pos;					// 현재 위치
	Pos		_oldPos;				// 이동 불가지역 이동시 되돌리기용
	TYPE	_type;					// 타입 저장용
	int		_startSprite;			// 반복을 위한 첫 스프라이트저장
	int		_curSprite;				// 현재 스프라이트
	int		_endSprite;				// 끝 스프라이트
	int		_frameDelay;			// 현재스프라이트의 고정 딜레이
	int		_delayCount;			// 딜레이카운트용
	bool	_endFrame;				// 끝 스트라이프에 최소 한번 도달했는지 여부
	char	_renderColor;			// 렌더링 컬러 플래그

public:
	Object(Pos pos);
	virtual ~Object();
	virtual bool Action() = 0;
	virtual void Draw() = 0;
	// 위치 리턴
	Pos GetPos();			
	// 타입 리턴
	TYPE GetType();					
	// 스프라이트 리턴
	int GetSprite();				
	// 프레임 진행
	void NextFrame();				
	// 위치 강제변경
	void SetPos(Pos pos);			
	// 렌더링 컬러 플래그 변경
	void ChangeColor(char color);	
	// dead플래그 변경
	void Die();						
};

//////////////////////////////////////////////////////////////
// Effect : publie Object
//////////////////////////////////////////////////////////////

class Effect : public Object
{
	bool _infinite;					// 무한지속여부
	bool _isHalfAlpha;				// 반투명여부
	int  _drawRate;					// 스프라이트 출력비율
	int	 _waitFrame;				// 첫 스프라이트 시작 지연시간
public:
	Effect(Pos pos, SPRITE sprite, bool isHalfAlpha = false, int waitFrame = 0);
	virtual bool Action() override;
	virtual void Draw() override;
	// _infinite 리턴
	bool GetInfinite();
	// 이펙트 이동
	void Move(int direction);
	// 이펙트 출력비율 변경
	void ChangeRate(int rate);
	// 이펙트 위치와 대기프레임 변경
	void ChangePosAndDelay(int AttackID, int DamageID);
};

//////////////////////////////////////////////////////////////
// Player : publie Object
//////////////////////////////////////////////////////////////

class Player : public Object
{
private:
	int			_id;				// 식별번호
	bool		_owner;				// 플레이어 체크용
	int			_healthPoint;		// HP
	int			_direction;			// Stand시 쳐다보는 방향
	bool		_canAction;			// action 수행가능 여부
	TAG_ACTION  _command;			// 들어온 명령
	TAG_ACTION	_action;			// 다음 명령 -> 현재 수행중인 명령
	Effect		*_shadow;			// 자기 그림자 포인터
	Effect		*_HPGuage;			// 자기 hp바 포인터
protected:
	bool Move(int direction);		// action에 의해 호출
	bool Attack(int type);			// action에 의해 호출
	void Stand();
public:
	Player(Pos pos);
	Player(int id, int direction, Pos pos, int healthPoint);
	~Player() ;
	// 세팅된 _action 으로 행동
	virtual bool Action() override;			
	// 렌더링
	virtual void Draw() override;			
	// 다음 action, _command 세팅
	inline void Command(TAG_ACTION tag);
	// ID가져오기
	inline int GetID();
	// HP가져오기
	inline int GetHealthPoint();
	// 방향 char로 리턴
	inline char GetDirection();
	// 현재액션 리턴
	inline TAG_ACTION GetAction();
	// 위치 강제 변경
	void ForceSetPos(Pos pos);
	// 자신의 플레이어 플래그 활성화
	inline void SetOwner();
	// Stand방향 전환
	inline void ChangeDirection(TAG_ACTION tag);
	// HP변경
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
