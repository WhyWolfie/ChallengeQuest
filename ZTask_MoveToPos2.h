#ifndef _ZTASK_MOVE_TO_POS2_H
#define _ZTASK_MOVE_TO_POS2_H

#include "ZTask2.h"

class ZTask_MoveToPos2 : public ZTask2
{
private:
	// data
	rvector		m_TargetPos;		// ������
	bool		m_bRotated;
	bool				m_bChained;			///< ���� Task�� ����Ǿ� �ִ��� ����.
protected:
	virtual void OnStart();
	virtual ZTaskResult OnRun(float fDelta);
	virtual void OnComplete();
	virtual bool OnCancel();
public:
	DECLARE_TASK_ID(ZTID_MOVE_TO_POS);

	ZTask_MoveToPos2(ZActor2* pParent, rvector& vTarPos, bool bChained=false);
	virtual ~ZTask_MoveToPos2();
	virtual const char* GetTaskName() { return "MoveToPos"; }
};







#endif