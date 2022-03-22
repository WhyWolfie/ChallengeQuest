#ifndef _ZTASK_ROTATE_TO_DIR2_H
#define _ZTASK_ROTATE_TO_DIR2_H

#include "ZTask2.h"

class ZTask_RotateToDir2 : public ZTask2
{
private:
	// data
	rvector		m_TargetDir;		// ¸ñÀûÁö
	bool		m_bRotated;
protected:
	virtual void OnStart();
	virtual ZTaskResult OnRun(float fDelta);
	virtual void OnComplete();
	virtual bool OnCancel();
public:
	DECLARE_TASK_ID(ZTID_ROTATE_TO_DIR);

	ZTask_RotateToDir2(ZActor2* pParent, rvector& vDir);
	virtual ~ZTask_RotateToDir2();
	virtual const char* GetTaskName() { return "RotateToDir"; }
};







#endif