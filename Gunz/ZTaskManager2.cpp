#include "stdafx.h"
#include "ZTaskManager2.h"
#include "ZTask2.h"
#include "ZTask_MoveToPos2.h"
#include "ZTask_RotateToDir2.h"
#include "ZTask_AttackMelee2.h"
#include "ZTask_AttackRange2.h"
#include "ZTask_Skill2.h"

#ifdef _DEBUG
static g_bTaskDebug = true;
#endif

ZTaskManager2::ZTaskManager2() : m_pCurrTask(NULL), m_pOnFinishedFunc(NULL)
{

}

ZTaskManager2::~ZTaskManager2()
{
	if (m_pCurrTask)
	{
		delete m_pCurrTask;
		m_pCurrTask = NULL;
	}

	for (list<ZTask2*>::iterator itor = m_Tasks.begin(); itor != m_Tasks.end(); ++itor)
	{
		delete (*itor);
	}
	m_Tasks.clear();
}


void ZTaskManager2::Clear()
{
#ifdef _DEBUG
	if (g_bTaskDebug)
	{
		char szCurrTask[256] = "";
		if (m_pCurrTask) strcpy(szCurrTask, m_pCurrTask->GetTaskName());
		// 로그가 너무 많이 남아서 다른 로그를 볼수가 없어서 주석 처리함. -- by SungE 2007-04-18
		// mlog("TASK: Clear(queue=%d, currtask=%s)\n", (int)m_Tasks.size(), szCurrTask);
	}
#endif

	CancelCurrTask();

	for (list<ZTask2*>::iterator itor = m_Tasks.begin(); itor != m_Tasks.end(); ++itor)
	{
		delete (*itor);
	}
	m_Tasks.clear();
}


void ZTaskManager2::Push(ZTask2* pTask)
{
#ifdef _DEBUG
	if (g_bTaskDebug)
	{
		char szTask[256] = "";
		// 로그가 너무 많이 남아서 다른 로그를 볼수가 없어서 주석 처리함. -- by SungE 2007-04-18
		// mlog("TASK: Push(queue=%d, task=%s)\n", (int)m_Tasks.size(), pTask->GetTaskName());
	}
#endif

	m_Tasks.push_back(pTask);
}

void ZTaskManager2::PushFront(ZTask2* pTask)
{
#ifdef _DEBUG
	if (g_bTaskDebug)
	{
		char szTask[256] = "";
		// 로그가 너무 많이 남아서 다른 로그를 볼수가 없어서 주석 처리함. -- by SungE 2007-04-18
		// mlog("TASK: PushFront(task=%s)\n", pTask->GetTaskName());
	}
#endif

	if (m_pCurrTask)
	{
		m_pCurrTask->Cancel();
		m_Tasks.push_front(m_pCurrTask);
	}

	m_pCurrTask = pTask;
	m_pCurrTask->Start();
}

void ZTaskManager2::Run(float fDelta)
{
	if (m_pCurrTask)
	{
		// 현재 태스크가 있으면 실행
		ZTaskResult ret = m_pCurrTask->Run(fDelta);

		switch (ret)
		{
		case ZTR_RUNNING:
			{

			}
			break;
		case ZTR_COMPLETED:
			{
				CompleteCurrTask();
			}
			break;
		case ZTR_CANCELED:
			{
				CancelCurrTask();
			}
			break;
		}
	}
	else
	{
		if (PopTask())
		{
			// 현재 태스크가 없으면 새로운 태스크를 꺼내서 시작한다.
			m_pCurrTask->Start();
		}
	}
}

bool ZTaskManager2::PopTask()
{
	if (m_Tasks.empty()) return false;

	list<ZTask2*>::iterator itor = m_Tasks.begin();
	m_pCurrTask = (*itor);
	m_Tasks.erase(itor);

	return true;
}


bool ZTaskManager2::CancelCurrTask()
{
	if (!m_pCurrTask) return true;

#ifdef _DEBUG
	if (g_bTaskDebug)
	{
		char szTask[256] = "";
		// 로그가 너무 많이 남아서 다른 로그를 볼수가 없어서 주석 처리함. -- by SungE 2007-04-18
		// mlog("TASK: Cancel(task=%s)\n", m_pCurrTask->GetTaskName());
	}
#endif

	if (m_pCurrTask->Cancel())
	{
		delete m_pCurrTask;
		m_pCurrTask = NULL;
		return true;
	}

	return false;
}

void ZTaskManager2::CompleteCurrTask()
{
	if (!m_pCurrTask) return;

#ifdef _DEBUG
	if (g_bTaskDebug)
	{
		char szTask[256] = "None";
		// 로그가 너무 많이 남아서 다른 로그를 볼수가 없어서 주석 처리함. -- by SungE 2007-04-18
		// mlog("TASK: Complete(task=%s)\n", m_pCurrTask->GetTaskName());
	}
#endif

	m_pCurrTask->Complete();

	if (m_pOnFinishedFunc)
	{
		m_pOnFinishedFunc(m_pCurrTask->GetParent(), m_pCurrTask->GetID());
	}

	delete m_pCurrTask;
	m_pCurrTask = NULL;
}

bool ZTaskManager2::Cancel()
{
	return CancelCurrTask();
}

ZTASK_ID ZTaskManager2::GetCurrTaskID()
{
	if (m_pCurrTask)
	{
		return m_pCurrTask->GetID();
	}
	return ZTID_NONE;
}

//////////////////////////////////////////////////////////////////
ZTask2* ZTaskManager2::CreateMoveToPos(ZActor2* pParent, rvector& vTarPos, bool bChained)
{
	ZTask2* pNew = new ZTask_MoveToPos2(pParent, vTarPos, bChained);
	return pNew;
}

ZTask2* ZTaskManager2::CreateRotateToDir(ZActor2* pParent, rvector& vTarDir)
{
	ZTask2* pNew = new ZTask_RotateToDir2(pParent, vTarDir);
	return pNew;

}

ZTask2* ZTaskManager2::CreateAttackMelee(ZActor2* pParent)
{
	ZTask2* pNew = new ZTask_AttackMelee2(pParent);
	return pNew;
}

ZTask2* ZTaskManager2::CreateAttackRange(ZActor2* pParent, rvector& dir)
{
	ZTask2* pNew = new ZTask_AttackRange2(pParent, dir);
	return pNew;
}

ZTask2* ZTaskManager2::CreateSkill(ZActor2* pParent,int nSkill,MUID& uidTarget,rvector& targetPosition)
{
	ZTask2* pNew = new ZTask_Skill2(pParent,nSkill,uidTarget,targetPosition);
	return pNew;
}

