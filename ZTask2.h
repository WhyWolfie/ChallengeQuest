#ifndef _ZTASK2_H
#define _ZTASK2_H

#include <list>
#include <queue>
using namespace std;

#include "ZTaskID.h"

/// task 결과 todok 따로 가야한다
/*enum ZTaskResult
{
	ZTR_RUNNING,
	ZTR_COMPLETED,
	ZTR_CANCELED
};*/

class ZActor2;
class ZBrain2;

/// Task의 추상클래스 - Task는 command 패턴을 사용한다.
class ZTask2
{
protected:
	ZActor2*				m_pParent;
	queue<int>			m_Params;
	bool				m_bCancelEnable;


	virtual void OnStart() = 0;				///< 태스크 시작시
	virtual ZTaskResult OnRun(float fDelta) = 0;		///< 태스크 실행
	virtual void OnComplete() = 0;			///< 테스크 완료시
	virtual bool OnCancel() = 0;			///< 태스크 취소시
public:
	ZTask2(ZActor2* pParent) : m_pParent(pParent), m_bCancelEnable(true) {}
	virtual ~ZTask2() {}

	void ClearParams();
	void PushParam(int nValue);
	void PushParam(float fValue);
	float PopParamFloat();
	int PopParamInt();

	void Start();
	ZTaskResult Run(float fDelta);
	void Complete();
	bool Cancel();
	ZActor2* GetParent() { return m_pParent; }
	void SetCancelEnable(bool bEnable) { m_bCancelEnable = bEnable; }

	virtual ZTASK_ID GetID() = 0;
	virtual const char* GetTaskName() = 0;
};


#define DECLARE_TASK_ID(_ID)	ZTASK_ID GetID() { return _ID; }





#endif