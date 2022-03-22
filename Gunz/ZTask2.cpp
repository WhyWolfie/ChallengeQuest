#include "stdafx.h"
#include "ZTask2.h"
#include "MDebug.h"


void ZTask2::ClearParams()
{

}

void ZTask2::PushParam(int nValue)
{
	m_Params.push(nValue);
}

void ZTask2::PushParam(float fValue)
{
	m_Params.push(*((int*)&fValue));
}

float ZTask2::PopParamFloat()
{
	if (m_Params.empty()) 
	{
		_ASSERT(0);
		return 0.0f;
	}

	int ret = m_Params.front();
	m_Params.pop();
	return *((float*)&ret);
}

int ZTask2::PopParamInt()
{
	if (m_Params.empty()) 
	{
		_ASSERT(0);
		return 0;
	}

	int ret = m_Params.front();
	m_Params.pop();
	return ret;

}



void ZTask2::Start()
{
	OnStart();
}

ZTaskResult ZTask2::Run(float fDelta)
{
	return OnRun(fDelta);
}

void ZTask2::Complete()
{
	OnComplete();
}

bool ZTask2::Cancel()
{
	if (!m_bCancelEnable) return false;

	return OnCancel();
}



