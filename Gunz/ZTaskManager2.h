#ifndef _ZTASKMANAGER2_H
#define _ZTASKMANAGER2_H

class ZActor2;
class ZTask2;

typedef void (*ZTM_ONFINISHED2)(ZActor2* pActor, ZTASK_ID nLastID);

/// 태스크 관리자
class ZTaskManager2
{
protected:
	list<ZTask2*>			m_Tasks;
	ZTask2*					m_pCurrTask;
	ZTM_ONFINISHED2			m_pOnFinishedFunc;

	bool PopTask();
	bool CancelCurrTask();
	void CompleteCurrTask();
public:
	ZTaskManager2();
	~ZTaskManager2();
	void Clear();
	void Push(ZTask2* pTask);
	void PushFront(ZTask2* pTask);
	bool Cancel();						///< 현재 태스크를 취소한다.
	void Run(float fDelta);

	// interface functions
	bool IsEmpty() { return m_Tasks.empty(); }
	int GetCount() { return (int)m_Tasks.size(); }
	ZTask2* GetCurrTask()	{ return m_pCurrTask; }
	ZTASK_ID GetCurrTaskID();
public:
	static ZTask2* CreateMoveToPos(ZActor2* pParent, rvector& vTarPos, bool bChained=false);
	static ZTask2* CreateRotateToDir(ZActor2* pParent, rvector& vTarDir);
	static ZTask2* CreateAttackMelee(ZActor2* pParent);
	static ZTask2* CreateAttackRange(ZActor2* pParent, rvector& dir);
	static ZTask2* CreateSkill(ZActor2* pParent,int nSkill,MUID& uidTarget,rvector& targetPosition);
	void SetOnFinishedCallback(ZTM_ONFINISHED2 pCallback) { m_pOnFinishedFunc = pCallback; }
};





#endif