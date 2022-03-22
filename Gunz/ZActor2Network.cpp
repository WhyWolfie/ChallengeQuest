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

	// �װ��� 5�ʰ� ������ basicinfo�� ������ �ʴ´�.
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
		pbi.posz = (int(GetPosition().z * 0.1f)) * 10.f;*/	// ������ ���� ���������� ���� �ϱ� ���� 1�� �ڸ����� ������ ����

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

		// history �� ����
		pActor->SaveHistory();
	}
}

//todok ������ ���� �ٸ� ��Ʈ��ũ ����� �� �� �ֵ��� Ŭ������ �Ļ���Ų��
void ZActor2Network::PostBossHpAp(ZActor2* pActor)
{
	// ���� ������ ��� HP AP�� �������ش�.
	if (pActor->IsBossType())
	{
		if (ZGetQuest()->GetGameInfo()->GetBoss() != pActor->GetUID()) return;
		
		DWORD nNowTime = timeGetTime();
		if (pActor->GetInitialized() == false) return;
		
		// �װ��� 5�ʰ� ������ basicinfo�� ������ �ʴ´�.
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
