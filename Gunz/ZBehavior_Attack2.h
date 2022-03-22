#ifndef _ZBEHAVIOR_ATTACK2_H
#define _ZBEHAVIOR_ATTACK2_H

#include "ZBehavior2.h"

class ZBehavior_Attack2 : public ZBehaviorState2
{
protected:
	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnRun(float fDelta);
public:
	ZBehavior_Attack2(ZBrain2* pBrain);
	virtual ~ZBehavior_Attack2();
};








#endif