#ifndef _ZTASK_SKILL2_H
#define _ZTASK_SKILL2_H

#include "ZTask2.h"
#include "ZTaskBase_Attack2.h"

class ZTask_Skill2 : public ZTaskBase_Attack2
{
	int		m_nSkill;
	float	m_fStartTime;
	bool	m_bExecuted;
	ZSkillDesc *m_pSkillDesc;
	MUID	m_uidTarget;
	rvector m_TargetPosition;
protected:
	virtual void OnStart();
	virtual ZTaskResult OnRun(float fDelta);
	virtual void OnComplete();
	virtual bool OnCancel();
public:
	DECLARE_TASK_ID(ZTID_SKILL);

	ZTask_Skill2(ZActor2* pParent,int nSkill,MUID& uidTarget,rvector& targetPosition);
	virtual ~ZTask_Skill2();
	virtual const char* GetTaskName() { return "Skill"; }
};

#endif