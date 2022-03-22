#include "stdafx.h"
#include "ZBehavior2.h"
#include "ZBrain2.h"
#include "ZBehavior_Idle2.h"
#include "ZBehavior_Attack2.h"
#include "ZActor2.h"

ZBehavior2::ZBehavior2() : m_pCurrState(NULL), m_pBrain(NULL)
{
}

ZBehavior2::~ZBehavior2()
{
}

void ZBehavior2::Init(ZBrain2* pBrain)
{
	if ( !pBrain)
		return;

	m_pBrain = pBrain;

	ZState* pState = new ZBehavior_Idle2(pBrain);
	pState->AddTransition(ZBEHAVIOR_INPUT_ATTACKED,		ZBEHAVIOR_STATE_ATTACK);
	m_FSM.AddState(pState);

	pState = new ZBehavior_Attack2(pBrain);
	m_FSM.AddState(pState);

	
	m_nOffecseType	= (ZOFFENSETYPE)pBrain->GetBody()->GetNPCInfo()->nOffenseType;
	m_bFriendly		= pBrain->GetBody()->GetNPCInfo()->bFriendly;
}

void ZBehavior2::Run(float fDelta)
{
	if ( !m_pCurrState)
		return;

    m_pCurrState->Run( fDelta);
}

bool ZBehavior2::Input(ZBEHAVIOR_INPUT nInput)
{
	int nextState = m_FSM.StateTransition(nInput);
	if (nextState == ZStateMachine::INVALID_STATE) return false;

	ZBEHAVIOR_STATE nNextBehaviorState = ZBEHAVIOR_STATE(nextState);
	ChangeBehavior(nNextBehaviorState);

	return true;
}

void ZBehavior2::ForceState(ZBEHAVIOR_STATE nState)
{
	ChangeBehavior(nState);
}


void ZBehavior2::ChangeBehavior(ZBEHAVIOR_STATE nState)
{
	if ( m_pCurrState)
	{
		if ( m_FSM.GetCurrStateID() == nState)
			return;

		((ZBehaviorState2*)m_pCurrState)->Exit();
	}

	m_FSM.SetState(nState);

	m_pCurrState = (ZBehaviorState2*)m_FSM.GetCurrState();
	m_pCurrState->Enter();
}