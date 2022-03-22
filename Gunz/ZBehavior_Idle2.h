#ifndef _ZBEHAVIOR_IDLE2_H
#define _ZBEHAVIOR_IDLE2_H

#include "ZBehavior2.h"

class ZBehavior_Idle2 : public ZBehaviorState2
{
protected:
	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnRun(float fDelta);
public:
	ZBehavior_Idle2(ZBrain2* pBrain);
	virtual ~ZBehavior_Idle2();
};


#endif