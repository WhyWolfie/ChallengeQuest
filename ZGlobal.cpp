#include "stdafx.h"
#include "ZGlobal.h"
#include "ZApplication.h"
#include "ZQuest.h"
//#include "SVNRevision/SVNRevision.cpp"
#include "GlobalSubtitutes.h"

bool ZIsLaunchDevelop(void) { 
	return ZApplication::GetInstance()->IsLaunchDevelop(); 
}
bool ZIsLaunchTest(void) { 
	return ZApplication::GetInstance()->IsLaunchTest(); 
}

RMeshMgr* ZGetNpcMeshMgr(void) { 
#ifdef _TDD
	RMeshMgr* p = (RMeshMgr*)ZTddGetGlobalSub(GI_NpcMeshMgr);
	if (p) return p;
#endif
	return ZApplication::GetNpcMeshMgr(); 
}

RMeshMgr* ZGetMeshMgr(void) { 
	return ZApplication::GetMeshMgr(); 
}

RMeshMgr* ZGetWeaponMeshMgr(void) { 
	return ZApplication::GetWeaponMeshMgr(); 
}

RAniEventMgr* ZGetAniEventMgr(void)
{
	return ZApplication::GetAniEventMgr();
}

ZSoundEngine* ZGetSoundEngine(void) { 
	return ZApplication::GetSoundEngine(); 
}

ZEffectManager*	ZGetEffectManager(void) { 
#ifdef _TDD
	ZEffectManager* p = (ZEffectManager*)ZTddGetGlobalSub(GI_EffectManager);
	if (p) return p;
#endif
	return ZGetGameInterface()->GetEffectManager(); 
}

ZScreenEffectManager* ZGetScreenEffectManager(void) { 
	return ZGetGameInterface()->GetScreenEffectManager(); 
}

int ZGetSVNRevision(void)
{
	return 1;
}

#ifdef _TDD
	DWORD g_nPseudoTimeForTDD = 0;
	bool g_bUsePseudoTimeForTDD = false;

	DWORD ZTDDTimeGetTime()
	{
		if (g_bUsePseudoTimeForTDD)
			return g_nPseudoTimeForTDD;
		else
			return timeGetTime();
	}
#endif