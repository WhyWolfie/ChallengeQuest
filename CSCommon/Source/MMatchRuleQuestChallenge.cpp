#include "stdafx.h"
#include "MMatchRuleQuestChallenge.h"
#include "MNewQuestNpcManager.h"
#include "MNewQuestPlayerManager.h"
#include "MSharedCommandTable.h"
#include "MMatchWorldItemDesc.h"

#define FILENAME_NPC2_XML		"npc2.xml"
#define FILENAME_SCENARIO2_XML	"scenario2.xml"
MActorDefManager MMatchRuleQuestChallenge::ms_actorDefMgr;
MNewQuestScenarioManager MMatchRuleQuestChallenge::ms_scenarioMgr;



////////////////////////////
// todok del
// 임시로 어떤 npc를 몇마리나 스폰할 것인지 지정한 파일을 로딩한다
static int gs_MaxSpawn = 1;
static char gs_szNpcName[1024] = "";
static std::vector<std::string> gs_vecNpcName;
void LoadTempINI()
{
	gs_MaxSpawn = GetPrivateProfileInt("NEWQUEST", "MAXSPAWN", 1, "./newquest.ini");
	GetPrivateProfileString("NEWQUEST", "NPCS", "knifeman", gs_szNpcName, 1024, "./newquest.ini");
	gs_vecNpcName.clear();

	char* sz = strtok(gs_szNpcName, "/");
	if (sz) 
		gs_vecNpcName.push_back( sz );

	while (sz != NULL)
	{
		sz = strtok(NULL, "/");
		if (sz) 
			gs_vecNpcName.push_back( sz );
	}
}

////////////////////////////


void MMatchRuleQuestChallenge::InitActorDefMgr()
{
	if (!ms_actorDefMgr.ReadXml(NULL, FILENAME_NPC2_XML))
	{
		mlog("Read new quest file %s Failed\n", FILENAME_NPC2_XML);
		_ASSERT(0); return;
	}
}

void MMatchRuleQuestChallenge::InitScenarioMgr()
{
	if (!ms_scenarioMgr.ReadXml(NULL, FILENAME_SCENARIO2_XML, MMatchServer::GetInstance()->GetQuest()->GetDropTable()))
	{
		mlog("Read new quest file %s Failed\n", FILENAME_SCENARIO2_XML);
		_ASSERT(0); return;
	}
}

MNewQuestScenarioManager* MMatchRuleQuestChallenge::GetScenarioMgr()
{
	return &ms_scenarioMgr;
}


MMatchRuleQuestChallenge::MMatchRuleQuestChallenge(MMatchStage* pStage)
: MMatchRule(pStage)
, m_pPlayerMgr(NULL)
, m_pNpcMgr(NULL)
, m_pScenario(NULL)
, m_nCurrSector(0)
{
	LoadTempINI();
}

MMatchRuleQuestChallenge::~MMatchRuleQuestChallenge()
{
}

// 모든 라운드가 끝나면 false 반환
bool MMatchRuleQuestChallenge::RoundCount()
{
	if (!m_pScenario)
	{
		_ASSERT(0);
		return false;
	}

	if (m_nCurrSector < m_pScenario->GetNumSector() - 1)
	{
		++m_nCurrSector;
		return true;
	}

	return false;
}

void MMatchRuleQuestChallenge::OnBegin()
{
	m_pPlayerMgr = new MNewQuestPlayerManager;
	m_pNpcMgr = new MNewQuestNpcManager;
	m_nCurrSector = 0;

	//todok 플레이어 접속 종료 및 방 나갈때 제거해줘야 한다

	// 시나리오 세팅
	m_pScenario = ms_scenarioMgr.GetScenario( m_pStage->GetMapName());
	if (!m_pScenario)
		m_pScenario = ms_scenarioMgr.GetScenario( ms_scenarioMgr.GetDefaultScenarioName());

	// 게임에 존재하는 플레이어 추가
	for (MUIDRefCache::iterator it=m_pStage->GetObjBegin(); it!=m_pStage->GetObjEnd(); ++it)
	{
		MUID uidChar = it->first;

		MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(uidChar);
		if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;

		m_pPlayerMgr->AddPlayer(pObj);
	}
}

void MMatchRuleQuestChallenge::OnEnd()
{
	delete m_pNpcMgr;
	delete m_pPlayerMgr;
}

void MMatchRuleQuestChallenge::OnRoundBegin()
{
	// PLAY 상태로 넘어갈때 전부 스폰 시킨다
	ProcessNpcSpawning();

	MMatchRule::OnRoundBegin();
}

void MMatchRuleQuestChallenge::OnRoundEnd()
{
	// 마지막 섹터가 아니라면 다음 섹터로 이동하게 한다
	if (m_nCurrSector < m_pScenario->GetNumSector() - 1)
		RouteMoveToNextSector();

	MMatchRule::OnRoundEnd();
}

bool MMatchRuleQuestChallenge::OnRun()
{
	//todok 각 클라의 핑을 체크해서 핑이 높은 클라의 npc 제어를 빠른 쪽으로 옮기는 구현 필요
	if (GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		//SendClientLatencyPing();
		//ReAssignNPC();
	}

	return MMatchRule::OnRun();
}

bool MMatchRuleQuestChallenge::OnCheckRoundFinish()
{
	// 전부 소탕하면 라운드를 끝낸다
	if (m_pNpcMgr->GetNumNpc() == 0)
		return true;

	return false;
}


void MMatchRuleQuestChallenge::ProcessNpcSpawning()
{
	//todok 임시 코드로 대충 몬스터 스폰하게 만들어 뒀다
	// todok del
	// 요건 INI의 설정에 따라서 맵 중앙에 스폰시키는 것
	//if (m_pNpcMgr->GetNumNpc() < gs_MaxSpawn)
	//{
	//	// 플레이어들에게 npc 스폰 커맨드를 보낸다
	//	//todok 일단 랜덤하게 대충 골라서 스폰한다.
	//	const char* szActorDef = NULL;
	//	int r = rand() % (int)gs_vecNpcName.size();
	//	szActorDef = gs_vecNpcName[r].c_str();

	//	SpawnNpc(szActorDef, 0, 0);
	//}

	// todok 요건 모든 스폰포인트에서 액터를 스폰하는 것 // 기획 나오고 구현을 픽스할 때는 코드 정리 좀 하자 if문 대박이네
	MNewQuestSector* pCurrSector = NULL;
	if (m_pScenario)
	{
		MNewQuestSector* pCurrSector = m_pScenario->GetSector(m_nCurrSector);
		if (pCurrSector)
		{
			int numSpawnType = pCurrSector->GetNumSpawnType();
			for (int i=0; i<numSpawnType; ++i)
			{
				MNewQuestSpawn* pSpawn = pCurrSector->GetSpawnByIndex(i);
				if (pSpawn)
				{
					for (int k=0; k<pSpawn->GetNum(); ++k)
					{
						// drop item 결정
						MQuestDropItem item;
						MMatchServer::GetInstance()->GetQuest()->GetDropTable()->Roll(item, pSpawn->GetDropTableId(), 0);

						SpawnNpc(pSpawn->GetActor(), i, k, item.nID);
					}
				}
			}
		}
	}
}


void MMatchRuleQuestChallenge::SpawnNpc(const char* szActorDef, int nCustomSpawnTypeIndex, int nSpawnIndex, int nDropItemId)
{
	if (!m_pNpcMgr || !m_pPlayerMgr)
		return;

	// 적당한 컨트롤러를 찾는다. 컨트롤러할 사람이 없으면 스폰을 취소
	MUID uidController = m_pPlayerMgr->FindSuitableController();
	if (uidController.IsInvalid())
		return;

	// npc 매니저에 실제 npc 생성 요청. 아까 찾은 컨트롤러 uid를 넘겨준다
	MUID uidNpc = MMatchServer::GetInstance()->UseUID();
	m_pNpcMgr->AddNpcObject(uidNpc, uidController, nDropItemId);
	m_pPlayerMgr->IncreaseNpcControl(uidController, uidNpc);

	//todok szActorDef를 이름이 아닌 인덱스로 보내도록 수정하자
	RouteSpawnNpc(uidNpc, uidController, szActorDef, nCustomSpawnTypeIndex, nSpawnIndex);
}

void MMatchRuleQuestChallenge::RouteSpawnNpc(MUID uidNPC, MUID uidController, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex)
{
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_NPC_SPAWN, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUID(uidController));
	pCmd->AddParameter(new MCmdParamUID(uidNPC));
	pCmd->AddParameter(new MCmdParamStr(szNpcDefName));
	pCmd->AddParameter(new MCmdParamUChar(nCustomSpawnTypeIndex));
	pCmd->AddParameter(new MCmdParamUChar(nSpawnIndex));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmd);
}

void MMatchRuleQuestChallenge::RouteNpcDead(MUID uidNPC, MUID uidKiller)
{
	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_NPC_DEAD, MUID(0,0));
	pNew->AddParameter(new MCmdParamUID(uidKiller));
	pNew->AddParameter(new MCmdParamUID(uidNPC));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}

void MMatchRuleQuestChallenge::RouteMoveToNextSector()
{
	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_MOVE_TO_NEXT_SECTOR, MUID(0,0));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}

void MMatchRuleQuestChallenge::OnCommand(MCommand* pCommand)
{
	switch (pCommand->GetID())
	{
	case MC_NEWQUEST_REQUEST_NPC_DEAD:
		{
			MUID uidSender = pCommand->GetSenderUID();
			MUID uidKiller, uidNPC;
			MShortVector s_pos;
			pCommand->GetParameter(&uidKiller,	0, MPT_UID);
			pCommand->GetParameter(&uidNPC,		1, MPT_UID);
			pCommand->GetParameter(&s_pos,		2, MPT_SVECTOR);
			MVector pos = MVector((float)s_pos.x, (float)s_pos.y, (float)s_pos.z);

			MNewQuestNpcObject* pNpc = m_pNpcMgr->GetNpc(uidNPC);
			if (!pNpc)
			{
				_ASSERT(0); break;
			}

			// 커맨드 보낸이는 반드시 해당 npc의 컨트롤러여야 한다. 아니라면 해킹이나 버그 상황
			if (pNpc->GetController() != uidSender)
			{
				_ASSERT(0); break;
			}

			// 드롭 아이템 생성
			DropItemByNpcDead(uidKiller, pNpc->GetDropItemId(), pos);

			m_pNpcMgr->DeleteNpcObject(uidNPC);
			m_pPlayerMgr->DecreaseNpcControl(uidSender);

			RouteNpcDead(uidNPC, uidKiller);
		}
		break;
	}
}

void MMatchRuleQuestChallenge::DropItemByNpcDead(const MUID& uidKiller, int nWorldItemId, const MVector& pos)
{
	// 월드아이템인지 검사하자
	if (!MGetMatchWorldItemDescMgr()->GetItemDesc(nWorldItemId))
		return;

	MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject(uidKiller);
	if (!pPlayer) return;

	int nWorldItemExtraValues[WORLDITEM_EXTRAVALUE_NUM];
	for (int i = 0; i < WORLDITEM_EXTRAVALUE_NUM; i++) nWorldItemExtraValues[i] = 0;

	m_pStage->SpawnServerSideWorldItem(pPlayer, nWorldItemId, pos.x, pos.y, pos.z,
		QUEST_DYNAMIC_WORLDITEM_LIFETIME, nWorldItemExtraValues );
}