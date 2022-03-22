#ifndef _ZBEHAVIOR2_H
#define _ZBEHAVIOR2_H


#include "ZStateMachine.h"
/*todok 따로가야한다
/// AI가 하는 일
enum ZBEHAVIOR_STATE
{
	ZBEHAVIOR_STATE_IDLE			=0,				///< 가만히 있는다.
	ZBEHAVIOR_STATE_PATROL,							///< 순찰
	ZBEHAVIOR_STATE_ATTACK,							///< 공격
	ZBEHAVIOR_STATE_RETREAT,						///< 도망
	ZBEHAVIOR_STATE_SCRIPT,							///< 스크립트
	ZBEHAVIOR_STATE_END
};

enum ZBEHAVIOR_INPUT 
{
	ZBEHAVIOR_INPUT_NONE = 0,
	ZBEHAVIOR_INPUT_ATTACKED,						///< 공격당함

	ZBEHAVIOR_INPUT_END
};

// 공격 특성
enum ZOFFENSETYPE
{
	ZOFFENSETYPE_MELEE		= 1,					// 근거리 공격형
	ZOFFENSETYPE_RANGE		= 2,					// 원거리 공격형
};
*/

class ZBrain2;

/// Behavior State 추상 클래스
class ZBehaviorState2 : public ZState
{
protected:
	ZBrain2*		m_pBrain;

	virtual void OnEnter() {}
	virtual void OnExit() {}
	virtual void OnRun(float fDelta) {}
public:
	ZBehaviorState2(ZBrain2* pBrain, int nStateID) : ZState(nStateID), m_pBrain(pBrain) { }
	virtual ~ZBehaviorState2() { }
	void Run(float fDelta)	{ OnRun(fDelta); }
	void Enter()			{ OnEnter(); }
	void Exit()				{ OnExit(); }
};



class ZBehavior2
{
private:
	ZStateMachine		m_FSM;
	ZBehaviorState2*		m_pCurrState;
	ZBrain2*				m_pBrain;
	ZOFFENSETYPE		m_nOffecseType;
	bool				m_bFriendly;
	void ChangeBehavior(ZBEHAVIOR_STATE nState);

public:
	ZBehavior2();
	virtual ~ZBehavior2();
	void Init(ZBrain2* pBrain);
	void Run(float fDelta);
	bool Input(ZBEHAVIOR_INPUT nInput);
	void ForceState(ZBEHAVIOR_STATE nState);
	ZOFFENSETYPE GetOffenseType()						{ return m_nOffecseType;	}
	void SetOffenseType( ZOFFENSETYPE nType)			{ m_nOffecseType = nType;	}
	bool IsFriendly()									{ return m_bFriendly;		}
	void SetFriendly( bool bFriendly)					{ m_bFriendly = bFriendly;	}
};




#endif