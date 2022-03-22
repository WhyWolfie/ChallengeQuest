#include "stdafx.h"
#include "ZRuleQuestChallenge.h"
#include "ZMatch.h"
#include "MActorDef.h"
#include "MNewQuestScenario.h"
#include "ZFSMManager.h"
#include "ZActorActionManager.h"
#include "ZActorAction.h"
#include "ZActorWithFSM.h"
#include "ZApplication.h"
#include "ZFSMManager.h"
#include "ZFSMParser.h"
#include "ZFSM.h"

ZRuleQuestChallenge::ZRuleQuestChallenge(ZMatch* pMatch) 
 : ZRule(pMatch)
 , m_pFsmManager(new ZFSMManager)
 , m_pActorDefMgr(new MActorDefManager)
 , m_pActorActionMgr(new ZActorActionManager)
 , m_pScenario(NULL)
 , m_nCurrSector(0)
{
}

ZRuleQuestChallenge::~ZRuleQuestChallenge()
{
	delete m_pFsmManager;
	delete m_pActorDefMgr;
	delete m_pActorActionMgr;
}

bool ZRuleQuestChallenge::LoadScenarioMap(const char* szScenarioName)
{
	MNewQuestScenarioManager* pScenarioMgr = ZApplication::GetStageInterface()->GetChallengeQuestScenario();
	if (!pScenarioMgr) { _ASSERT(0); return false; }
	MNewQuestScenario* pScenario = pScenarioMgr->GetScenario(szScenarioName);
	if (!pScenario) { _ASSERT(0); return false; }

	int numSector = pScenario->GetNumSector();
	for (int i=0; i<numSector; ++i)
	{
		MNewQuestSector* pSector = pScenario->GetSector(i);
		if (!pSector)  { _ASSERT(0); return false; }

		const char* szMapName = pSector->GetMapName();
		_ASSERT(0 != strlen(szMapName));
		ZGetWorldManager()->AddWorld(szMapName);
	}

	return true;
}

int ZRuleQuestChallenge::GetRoundMax()
{
	if (!m_pScenario) {
		_ASSERT(0);
		return 1;
	}

	return m_pScenario->GetNumSector();
}


bool ZRuleQuestChallenge::Init()
{
	//todok quest 게임을 할때마다 새로 로딩하는 것을 개선하자.

	// npc action 읽기
	string strActionFile("system/zactoraction.xml");
#ifndef _DEBUG
	strActionFile += "";
#endif
	if (!m_pActorActionMgr->ReadXml(ZApplication::GetFileSystem(), strActionFile.c_str()))
	{
		mlog("Error while Load %s\n", strActionFile.c_str());
		return false;
	}

	// npc fsm 기반 AI 읽기
	string strAIFSMFile("system/aifsm.xml");
#ifndef _DEBUG
	strAIFSMFile += "";
#endif
	vector<ZFSM*> vecFSM;
	ZFSMParser fsmParser;
	if (!fsmParser.ReadXml(ZApplication::GetFileSystem(), strAIFSMFile.c_str(), vecFSM, m_pActorActionMgr))
	{
		mlog("Error while Load %s\n", strAIFSMFile.c_str());
		return false;
	}
	m_pFsmManager->AddFsm(&vecFSM[0], (unsigned int)vecFSM.size());


	// npc 모델 읽기
	string strNpcMeshListFile("model/npc2.xml");
#ifndef _DEBUG
	strNpcMeshListFile += "";
#endif	
	if (ZGetNpcMeshMgr()->LoadXmlList(strNpcMeshListFile.c_str()) == -1)
	{
		mlog("Error while Load %s\n", strNpcMeshListFile.c_str());
		return false;
	}

	// npc 정의 읽기
	string strNpcDefFile("system/npc2.xml");
#ifndef _DEBUG
	strNpcDefFile += "";
#endif
	if (!m_pActorDefMgr->ReadXml(ZApplication::GetFileSystem(), strNpcDefFile.c_str()))
	{
		mlog("Error while Read %s\n", strNpcDefFile.c_str());
		return false;
	}


	// 이 퀘스트의 시나리오 객체 알아두기
	const char* szScenarioName = ZGetGameClient()->GetMatchStageSetting()->GetMapName();
	
	MNewQuestScenarioManager* pScenarioMgr = ZApplication::GetStageInterface()->GetChallengeQuestScenario();
	if (!pScenarioMgr) { _ASSERT(0); return false; }
	MNewQuestScenario* pScenario = pScenarioMgr->GetScenario(szScenarioName);
	if (!pScenario) { _ASSERT(0); return false; }

	m_pScenario = pScenario;

	m_nCurrSector = 0;

	return true;
}

void ZRuleQuestChallenge::OnUpdate( float fDelta )
{
	if (ZGetGame() == NULL) return;

	//MUID uidChar(ZGetMyUID());
	//static int tempUid = 111;
	//MUID uidNPC(1,tempUid);
	////unsigned char nNPCType, nPositionIndex;

	////pCommand->GetParameter(&uidChar,			0, MPT_UID);
	////pCommand->GetParameter(&uidNPC,				1, MPT_UID);
	////pCommand->GetParameter(&nNPCType,			2, MPT_UCHAR);
	////pCommand->GetParameter(&nPositionIndex,		3, MPT_UCHAR);

	//// 일단 죽은 놈은 제거한다
	//ZNPCObjectMap* pNpcMap = ZGetObjectManager()->GetNPCObjectMap();
	//for (ZNPCObjectMap::iterator it=pNpcMap->begin(); it!=pNpcMap->end(); )
	//{
	//	if (it->second->IsDie())
	//	{
	//		delete it->second;
	//		it = pNpcMap->erase(it);
	//	}
	//	else
	//		++it;
	//}

	//// 동시 스폰 수를 제한한다
	//if (5 <= pNpcMap->size())
	//	return;

	//tempUid++;
	//uidNPC = MUID(1, tempUid);

	///*
	//MQUEST_NPC NPCType = NPC_GOBLIN_GUNNER;

	////ZMapSpawnType nSpawnType = ZMST_NPC_MELEE;

	//ZMapSpawnManager* pMSM = ZGetGame()->GetMapDesc()->GetSpawnManager();

	//MQuestNPCInfo* pNPCInfo = ZGetQuest()->GetNPCCatalogue()->GetInfo(NPCType);
	////MQuestNPCInfo* pNPCInfo = GetNPCInfo(NPCType);
	//if (pNPCInfo == NULL) return;

	//ZMapSpawnData* pSpawnData = pMSM->GetSpawnData(ZMST_NPC_MELEE, 0);

	//rvector NPCPos = rvector(0,0,0);
	//rvector NPCDir = rvector(1,0,0);

	//if (pSpawnData)
	//{
	//	NPCPos = pSpawnData->m_Pos;
	//	NPCDir = pSpawnData->m_Dir;
	//}*/

	//bool bMyControl = (uidChar == ZGetMyUID());
	//SpawnActor("knifeman", uidNPC, bMyControl);


	//todokkkkk 죽은 엔피씨 서버에 알림
}

void ZRuleQuestChallenge::OnSetRoundState( MMATCH_ROUNDSTATE roundState )
{
	switch(roundState) 
	{
	case MMATCH_ROUNDSTATE_PREPARE: 
		{
		}
		break;
	case MMATCH_ROUNDSTATE_PRE_COUNTDOWN:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_COUNTDOWN : 
		{
		}
		break;
	case MMATCH_ROUNDSTATE_PLAY:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_FINISH:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_EXIT:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_FREE:
		{
		}
		break;
	};
}

float ZRuleQuestChallenge::FilterDelayedCommand( MCommand* pCommand )
{
	switch (pCommand->GetID())
	{
	case MC_NEWQUEST_PEER_NPC_ATTACK_MELEE:	//todok 이제 사용하지 않음
		{
			char szActionName[64];	//todokkkk 액션 이름을 보내지 말고 정수같은 효율적인 값으로 구별할수 있게 하자
			pCommand->GetParameter(szActionName,	1, MPT_STR, sizeof(szActionName));

			IActorAction* pAction = m_pActorActionMgr->GetAction(szActionName);
			if (!pAction) { _ASSERT(0); break; }

			const ZActorActionMeleeShot* pMeleeShot = pAction->GetMeleeShot();
			return pMeleeShot->GetDelay();
		}
	}

	return 0;
}

bool ZRuleQuestChallenge::OnCommand( MCommand* pCommand )
{
	switch (pCommand->GetID())
	{
	case MC_NEWQUEST_PEER_NPC_ATTACK_MELEE:	//todok 이제 사용하지 않음
		{
			MUID uidNpc;
			char szActionName[64];	//todokkkk 액션 이름을 보내지 말고 정수같은 효율적인 값으로 구별할수 있게 하자
			pCommand->GetParameter(&uidNpc, 0, MPT_UID);
			pCommand->GetParameter(szActionName,	1, MPT_STR, sizeof(szActionName));

			ZActorBase* pActor = ZGetObjectManager()->GetNPCObject(uidNpc);
			ZActorWithFSM* pActorWithFSM = MDynamicCast(ZActorWithFSM, pActor);
			if (pActorWithFSM)
			{
				pActorWithFSM->OnPeerMeleeShot(szActionName);
			}

			return true;
		}
	case MC_NEWQUEST_NPC_SPAWN:
		{
			MUID uidController;
			MUID uidNpc;
			char szActorDefName[128];
			unsigned char nCustomSpawnTypeIndex, nSpawnIndex;
			pCommand->GetParameter(&uidController,	0, MPT_UID);
			pCommand->GetParameter(&uidNpc,			1, MPT_UID);
			pCommand->GetParameter(szActorDefName,	2, MPT_STR, sizeof(szActorDefName));
			pCommand->GetParameter(&nCustomSpawnTypeIndex,	3, MPT_UCHAR);
			pCommand->GetParameter(&nSpawnIndex,			4, MPT_UCHAR);

			SpawnActor(szActorDefName, uidNpc, nCustomSpawnTypeIndex, nSpawnIndex, ZGetMyUID()==uidController);
			return true;
		}

	case MC_NEWQUEST_NPC_DEAD:
		{
			MUID uidKiller, uidNPC;

			pCommand->GetParameter(&uidKiller,	0, MPT_UID);
			pCommand->GetParameter(&uidNPC,		1, MPT_UID);

			ZActorWithFSM* pActor = (ZActorWithFSM*)ZGetObjectManager()->GetNPCObject(uidNPC);
			if (pActor)
			{
				ZGetObjectManager()->Delete(pActor);

				//todok 아래 주석 구현해줘야 함
				/*m_GameInfo.IncreaseNPCKilled();

				ZCharacter* pCharacter = (ZCharacter*) ZGetCharacterManager()->Find(uidKiller);
				if (pCharacter)
				{
					ZModule_QuestStatus* pMod = (ZModule_QuestStatus*)pCharacter->GetModule(ZMID_QUESTSTATUS);
					if (pMod)
					{
						pMod->AddKills();
					}
				}*/
			}
			return true;
		}

	case MC_NEWQUEST_MOVE_TO_NEXT_SECTOR:
		{
			MoveToNextSector();
			return true;
		}

	case MC_NEWQUEST_PEER_NPC_BASICINFO:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if(pParam->GetType()!=MPT_BLOB)
			{
				_ASSERT(0); break;
			}

			ZACTOR_WITHFSM_BASICINFO* pbi= (ZACTOR_WITHFSM_BASICINFO*)pParam->GetPointer();
			ZActorWithFSM* pActor = (ZActorWithFSM*)ZGetObjectManager()->GetNPCObject(pbi->uidNPC);
			if (pActor)
			{
				pActor->OnBasicInfo(pbi);
			}

			return true;
		}
	case MC_NEWQUEST_PEER_NPC_ACTION_EXECUTE:
		{
			MUID uidNpc;
			int nActionIndex;
			pCommand->GetParameter(&uidNpc, 0, MPT_UID);
			pCommand->GetParameter(&nActionIndex, 1, MPT_INT, sizeof(nActionIndex));

			ZActorBase* pActor = ZGetObjectManager()->GetNPCObject(uidNpc);
			ZActorWithFSM* pActorWithFSM = MDynamicCast(ZActorWithFSM, pActor);
			if (pActorWithFSM)
			{
				pActorWithFSM->OnPeerActionExecute(nActionIndex);
			}

			return true;
		}
	}

	return false;
}

void ZRuleQuestChallenge::SpawnActor(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl)
{
	rvector pos(0,0,0);
	rvector dir(1,0,0);

	ZMapSpawnManager* pMSM = ZGetGame()->GetMapDesc()->GetSpawnManager();
	if (pMSM)
	{
		ZMapSpawnData* pSpawnData = pMSM->GetCustomSpawnData(nCustomSpawnTypeIndex, nSpawnIndex);
		if (pSpawnData)
		{
			pos = pSpawnData->m_Pos;
			dir = pSpawnData->m_Dir;
		}
	}

	MActorDef* pActorDef = m_pActorDefMgr->GetDef(szActorDefName);
	if (!pActorDef) 
	{
		mlog("ERROR : cannot found actordef:\'%s\'\n", szActorDefName);
		_ASSERT(0); 
		return; 
	}

	// 만약 리소스 로딩을 안했으면 로드 - 이럴일은 테스트빼곤 없어야한다.
	RMesh* pNPCMesh = ZGetNpcMeshMgr()->Get(pActorDef->GetName());
	if (pNPCMesh)
	{
		if (!pNPCMesh->m_isMeshLoaded)
		{
			ZGetNpcMeshMgr()->Load(pActorDef->GetName());
			ZGetNpcMeshMgr()->ReloadAllAnimation();
		}
	}

	ZActorWithFSM* pNewActor = new ZActorWithFSM(ZGetGame(), m_pActorActionMgr);
	pNewActor->InitWithActorDef(pActorDef, m_pFsmManager);
	pNewActor->SetUID(uid);
	pNewActor->SetPosition(pos);
	pNewActor->SetDirection(dir);
	pNewActor->SetMyControl(bMyControl);

	ZGetObjectManager()->Add(pNewActor);
	ZGetEffectManager()->AddReBirthEffect(pNewActor->GetPosition());
}

void ZRuleQuestChallenge::MoveToNextSector()
{
	ZCharacter *pMyChar = ZGetGame()->m_pMyCharacter;
	pMyChar->InitStatus();

	// 전 화면에서 남아있을 수 있는 탄과 이펙트를 제거
	ZGetEffectManager()->Clear();
	ZGetGame()->m_WeaponManager.Clear();

	// 새로운 월드로 이동!!
	++m_nCurrSector;	//todok 현재 섹터가 마지막 섹터라면? 괜찮은건가..
	ZGetWorldManager()->SetCurrent(m_nCurrSector);
	//todok 라운드 형태로 룰을 구현했기 때문에 이 코드는 이제 필요 없는듯
	//// 이번에 이동할 캐릭터의 위치
	//int nPosIndex = ZGetCharacterManager()->GetCharacterIndex(pMyChar->GetUID(), false);
	//if (nPosIndex < 0) nPosIndex=0;
	//ZMapSpawnData* pSpawnData = ZGetWorld()->GetDesc()->GetSpawnManager()->GetSoloData(nPosIndex);
	//// 새 좌표로 이동
	//if (pSpawnData!=NULL && pMyChar!=NULL)
	//{
	//	pMyChar->SetPosition(pSpawnData->m_Pos);
	//	pMyChar->SetDirection(pSpawnData->m_Dir);
	//	ZGetEffectManager()->AddReBirthEffect(pSpawnData->m_Pos);
	//}

	//todok quest 임시로 주석함 구현해야 한다
	//// 아무도 보여주지 않는다.
	//for(ZCharacterManager::iterator i = ZGetCharacterManager()->begin();i!=ZGetCharacterManager()->end();i++)
	//{
	//	i->second->SetVisible(false);
	//}

	//// ko수 동기화
	//ZModule_QuestStatus* pMod = (ZModule_QuestStatus*)pMyChar->GetModule(ZMID_QUESTSTATUS);
	//if (pMod)
	//{
	//	int nKills = pMod->GetKills();
	//	ZGetScreenEffectManager()->SetKO(nKills);
	//}
}