#include "stdafx.h"
#include "ZTask_AttackMelee2.h"
#include "ZActor2.h"

ZTask_AttackMelee2::ZTask_AttackMelee2(ZActor2* pParent) : ZTaskBase_Attack2(pParent)
{

}

ZTask_AttackMelee2::~ZTask_AttackMelee2()
{

}


void ZTask_AttackMelee2::OnStart()
{
	m_pParent->Attack_Melee();
}

ZTaskResult ZTask_AttackMelee2::OnRun(float fDelta)
{
	if (m_pParent->GetCurrAni() == ZA_ANIM_ATTACK_MELEE) return ZTR_RUNNING;
	return ZTR_COMPLETED;
}

void ZTask_AttackMelee2::OnComplete()
{

}

