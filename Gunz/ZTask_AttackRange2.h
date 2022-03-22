#ifndef _ZTASK_ATTACK_RANGE2_H
#define _ZTASK_ATTACK_RANGE2_H

#include "ZTask2.h"
#include "ZTaskBase_Attack2.h"

class ZTask_AttackRange2 : public ZTaskBase_Attack2
{
private:
	rvector			m_vDirection;
protected:
	virtual void OnStart();
	virtual ZTaskResult OnRun(float fDelta);
	virtual void OnComplete();
public:
	DECLARE_TASK_ID(ZTID_ATTACK_RANGE);

	ZTask_AttackRange2(ZActor2* pParent, rvector& dir);
	virtual ~ZTask_AttackRange2();
	virtual const char* GetTaskName() { return "AttackRange"; }
};











#endif