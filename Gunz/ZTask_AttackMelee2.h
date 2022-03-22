#ifndef _ZTASK_ATTACK_MELEE2_H
#define _ZTASK_ATTACK_MELEE2_H

#include "ZTask2.h"
#include "ZTaskBase_Attack2.h"

class ZTask_AttackMelee2 : public ZTaskBase_Attack2
{
private:
protected:
	virtual void OnStart();
	virtual ZTaskResult OnRun(float fDelta);
	virtual void OnComplete();
public:
	DECLARE_TASK_ID(ZTID_ATTACK_MELEE);

	ZTask_AttackMelee2(ZActor2* pParent);
	virtual ~ZTask_AttackMelee2();
	virtual const char* GetTaskName() { return "AttackMelee"; }
};




#endif