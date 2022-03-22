#include "stdafx.h"
#include "ZTask_RotateToDir2.h"
#include "ZActor2.h"

ZTask_RotateToDir2::ZTask_RotateToDir2(ZActor2* pParent, rvector& vDir) 
					: ZTask2(pParent), m_TargetDir(vDir), m_bRotated(false)
{

}

ZTask_RotateToDir2::~ZTask_RotateToDir2()
{

}

void ZTask_RotateToDir2::OnStart()
{

}

ZTaskResult ZTask_RotateToDir2::OnRun(float fDelta)
{
	rvector dir = m_TargetDir;

	if (!m_bRotated)
	{
		// 방향이 맞지 않으면 제자리에서 회전한다.
		rmatrix mat;
		rvector vBodyDir = m_pParent->GetDirection();
		float fAngle=GetAngleOfVectors(dir, vBodyDir);
		float fRotAngle = m_pParent->GetNPCInfo()->fRotateSpeed * (fDelta / 1.0f);

		if (fAngle > 0.0f) fRotAngle = -fRotAngle;
		if (fabs(fRotAngle) > fabs(fAngle)) 
		{
			fRotAngle = -fAngle;
			m_bRotated = true;
			return ZTR_COMPLETED;
		}
		D3DXMatrixRotationZ(&mat, fRotAngle);

		m_pParent->RotateTo(vBodyDir * mat);
	}
	else
	{
		return ZTR_COMPLETED;
	}

	

	return ZTR_RUNNING;
}

void ZTask_RotateToDir2::OnComplete()
{
	
}

bool ZTask_RotateToDir2::OnCancel()
{
	return true;
}
