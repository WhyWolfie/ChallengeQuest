#include "stdafx.h"
#include "ZActor2Network.h"
#include "ZActor2.h"

ZActor2Network::ZActor2Network()
{
	memset(m_nLastTime, 0, sizeof(m_nLastTime));
}

ZActor2Network::~ZActor2Network()
{
}

void ZActor2Network::Process(ZActor2* pActor)
{
	PostBasicInfo(pActor);
	PostBossHpAp(pActor);
}

void ZActor2Network::PostBasicInfo(ZActor2* pActor)
{
	DWORD nNowTime = timeGetTime();
	if (pActor->GetInitialized() == false) return;

	// 죽고나서 5초가 지나면 basicinfo를 보내지 않는다.
	if(pActor->IsDie() && ZGetGame()->GetTime() - pActor->GetDeadTime()>5.f) return;
	int nMoveTick = (ZGetGameClient()->GetAllowTunneling() == false) ? PEERMOVE_TICK : PEERMOVE_AGENT_TICK;

	if ((int)(nNowTime - m_nLastTime[ACTOR_LASTTIME_BASICINFO]) >= nMoveTick)
	{
		m_nLastTime[ACTOR_LASTTIME_BASICINFO] = nNowTime;

		ZACTOR2_BASICINFO pbi;
		pbi.fTime = ZGetGame()->GetTime();
		pbi.uidNPC = pActor->GetUID();

		const rvector& pos = pActor->GetPosition();
		pbi.posx = pos.x;
		pbi.posy = pos.y;
		pbi.posz = pos.z;
		/*pbi.posx = (int(GetPosition().x * 0.1f)) * 10.f;
		pbi.posy = (int(GetPosition().y * 0.1f)) * 10.f;
		pbi.posz = (int(GetPosition().z * 0.1f)) * 10.f;*/	// 오차로 인한 버그재현을 쉽게 하기 위해 1의 자리까지 절사한 버전

		const rvector& vel = pActor->GetVelocity();
		pbi.velx = vel.x;
		pbi.vely = vel.y;
		pbi.velz = vel.z;

		const rvector& dir = pActor->GetDirection();
		pbi.dirx = dir.x*32000.0f;
		pbi.diry = dir.y*32000.0f;
		pbi.dirz = dir.z*32000.0f;

		pbi.anistate = pActor->GetCurrAni();

		ZPOSTCMD1(MC_QUEST_PEER_NPC_BASICINFO, MCommandParameterBlob(&pbi,sizeof(ZACTOR2_BASICINFO)));

		// history 에 보관
		pActor->SaveHistory();
	}
}

//todok 보스인 경우는 다른 네트워크 모듈을 줄 수 있도록 클래스를 파생시킨다
void ZActor2Network::PostBossHpAp(ZActor2* pActor)
{
	// 만약 보스일 경우 HP AP를 전송해준다.
	if (pActor->IsBossType())
	{
		if (ZGetQuest()->GetGameInfo()->GetBoss() != pActor->GetUID()) return;
		
		DWORD nNowTime = timeGetTime();
		if (pActor->GetInitialized() == false) return;
		
		// 죽고나서 5초가 지나면 basicinfo를 보내지 않는다.
		if(pActor->IsDie() && ZGetGame()->GetTime() - pActor->GetDeadTime()>5.f) return;
		int nMoveTick = (ZGetGameClient()->GetAllowTunneling() == false) ? PEERMOVE_TICK : PEERMOVE_AGENT_TICK;

		if ((int)(nNowTime - m_nLastTime[ACTOR_LASTTIME_HPINFO]) >= nMoveTick)
		{
			m_nLastTime[ACTOR_LASTTIME_HPINFO] = nNowTime;
			
			ZPOSTCMD3(MC_QUEST_PEER_NPC_BOSS_HPAP, MCmdParamUID(pActor->GetUID()), 
				MCommandParameterFloat(pActor->GetHP()), MCommandParameterFloat(pActor->GetAP()));
		}
	}
}
