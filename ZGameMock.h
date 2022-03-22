#pragma once

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>
#include "ZNavigationMesh.h"



class ZCharacterManagerMock : public ZCharacterManagerBase
{
public:

};

class MMatchItemDescMgrMock : public MMatchItemDescMgrBase
{
	MMatchItemDesc m_itemDesc;

public:
	virtual bool ReadXml(const char* szFileName) { return true; }
	virtual bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName) { return true; }
	virtual void Clear() { clear(); }
	virtual MMatchItemDesc* GetItemDesc(unsigned long int nID)
	{
		return &m_itemDesc;
	}
	virtual void ShiftMemoryGradually() {}
};

struct ZGameMock : public IGame
{
	ZCharacterManagerMock m_characterManager;

	virtual float GetTime() { return 0; }
	virtual ZMyCharacter* GetMyCharacter() { return NULL;}
	virtual MUID GetMyUid() { return MUID(0,0); }

	virtual ZCharacterManagerBase* GetCharacterMgr() { return &m_characterManager; }

	virtual bool CanAttack(ZObject *pAttacker, ZObject *pTarget) { return false; }
	virtual bool CanAttack_DebugRegister(ZObject *pAttacker, ZObject *pTarget) { return false; }

	virtual bool Pick(ZObject *pOwnerObject,rvector &origin,rvector &dir,ZPICKINFO *pickinfo,DWORD dwPassFlag=RM_FLAG_ADDITIVE | RM_FLAG_HIDE,bool bMyChar=false) { return false; }

	virtual bool PickWorld( const rvector &pos, const rvector &dir,RBSPPICKINFO *pOut,DWORD dwPassFlag=RM_FLAG_ADDITIVE | RM_FLAG_USEOPACITY | RM_FLAG_HIDE) { return false; }
	virtual bool CheckWall(rvector &origin, rvector &targetpos, float fRadius, float fHeight=0.f, RCOLLISIONMETHOD method=RCW_CYLINDER, int nDepth=0, rplane *pimpactplane=NULL) { return false; }
	virtual rvector GetFloor(rvector pos, rplane *pimpactplane=NULL, MUID uID=MUID(0,0) ) { return rvector(0,0,0); }
	virtual void AdjustMoveDiff(ZObject* pObject, rvector& diff) {}

	virtual ZNavigationMesh GetNavigationMesh() { return ZNavigationMesh(NULL); }

	virtual bool InRanged( ZObject* pAttacker, ZObject* pVictim) { return false; }
	virtual bool InRanged( ZObject* pAttacker, ZObject* pVictim, int &nDebugRegister/*디버그 레지스터 해킹 방지를 위한 변수*/) { return false;}

	virtual bool IsWallBlocked(ZObject* pObj1,ZObject* pObj2, bool bCoherentToPeer=false) { return false; }
	virtual bool IsWallBlocked(ZObject* pObj1,ZObject* pObj2, int & nDebugRegister/*단순 비교용*/, bool bCoherentToPeer=false) { return false; }

	virtual bool IsExceptedFromNpcTargetting(const ZCharacter* pChar) { return false; }
};

#endif