#include "stdafx.h"
#include "ZBehavior_Idle2.h"

ZBehavior_Idle2::ZBehavior_Idle2( ZBrain2* pBrain) : ZBehaviorState2( pBrain, ZBEHAVIOR_STATE_IDLE)
{
	m_pBrain = pBrain;
}

ZBehavior_Idle2::~ZBehavior_Idle2()
{
}


void ZBehavior_Idle2::OnEnter()
{
}

void ZBehavior_Idle2::OnExit()
{
}

void ZBehavior_Idle2::OnRun(float fDelta)
{
	if ( !m_pBrain)
		return;
}
