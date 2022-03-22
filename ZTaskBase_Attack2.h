#ifndef _ZTASK_ATTACK2_H
#define _ZTASK_ATTACK2_H


#include "ZTask2.h"

class ZTaskBase_Attack2 : public ZTask2
{
private:
protected:
	virtual bool OnCancel()
	{
		return false;
	}
public:
	ZTaskBase_Attack2(ZActor2* pParent) : ZTask2(pParent) {}
	virtual ~ZTaskBase_Attack2() {}
};


#endif