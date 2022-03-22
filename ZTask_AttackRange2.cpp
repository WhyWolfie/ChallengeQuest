#include "stdafx.h"
#include "ZTask_AttackRange2.h"
#include "ZActor2.h"

ZTask_AttackRange2::ZTask_AttackRange2(ZActor2* pParent, rvector& dir) : ZTaskBase_Attack2(pParent), m_vDirection(dir)
{

}

ZTask_AttackRange2::~ZTask_AttackRange2()
{

}


void ZTask_AttackRange2::OnStart()
{
	m_pParent->Attack_Range(m_vDirection);
}

ZTaskResult ZTask_AttackRange2::OnRun(float fDelta)
{
	if (m_pParent->GetCurrAni() == ZA_ANIM_ATTACK_RANGE)
		return ZTR_RUNNING;
	return ZTR_COMPLETED;
}

void ZTask_AttackRange2::OnComplete()
{

}

