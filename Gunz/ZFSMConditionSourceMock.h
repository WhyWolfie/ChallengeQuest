#pragma once

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZFSMCondition.h"

class ZFSMState;

class ZFSMConditionSourceMock : public IFSMConditionSource
{
public:
	rvector m_vMyPos;
	rvector m_vMyDir;

	float m_fHp;
	float m_fGroggy;
	rvector m_vVelocity;

	bool m_bEndAction;
	bool m_bLanding;

	bool m_bHasTarget;
	rvector m_vTargetPos;

	int m_nDice;

	bool m_bActiveLightningDamage;

	virtual float Cond_GetHp() { return m_fHp; }
	virtual float Cond_GetGroggy() { return m_fGroggy; }
	virtual const rvector& Cond_GetVelocity() { return m_vVelocity; }
	virtual bool Cond_IsEndAction() { return m_bEndAction; }
	virtual bool Cond_IsLanding() { return m_bLanding; }

	virtual bool Cond_HasTarget() { return m_bHasTarget; }

	virtual void Cond_GetPos(rvector& out) { out=m_vMyPos; }
	virtual void Cond_GetTargetPos(rvector& out) { out=m_vTargetPos; }

	virtual void Cond_GetDirection(rvector& out) { out=m_vMyDir; }

	virtual int Cond_GetDice() { return m_nDice; }

	virtual bool Cond_CheckStateCooltime(ZFSMState* pState, DWORD dwCurrTime) { return true; }
	virtual DWORD Cond_GetStateEnteredTime() { return false; }

	virtual bool IsActiveLightningDamageModule() { return m_bActiveLightningDamage; }

	virtual bool Cond_Pick(float fAngleDegree, float fDist) { return true; }
};


#endif