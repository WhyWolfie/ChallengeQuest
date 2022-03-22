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
// �ӽ÷� � npc�� ����� ������ ������ ������ ������ �ε��Ѵ�
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

// ��� ���尡 ������ false ��ȯ
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

	//todok �÷��̾� ���� ���� �� �� ������ ��������� �Ѵ�

	// �ó����� ����
	m_pScenario = ms_scenarioMgr.GetScenario( m_pStage->GetMapName());
	if (!m_pScenario)
		m_pScenario = ms_scenarioMgr.GetScenario( ms_scenarioMgr.GetDefaultScenarioName());

	// ���ӿ� �����ϴ� �÷��̾� �߰�
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
	// PLAY ���·� �Ѿ�� ���� ���� ��Ų��
	ProcessNpcSpawning();

	MMatchRule::OnRoundBegin();
}

void MMatchRuleQuestChallenge::OnRoundEnd()
{
	// ������ ���Ͱ� �ƴ϶�� ���� ���ͷ� �̵��ϰ� �Ѵ�
	if (m_nCurrSector < m_pScenario->GetNumSector() - 1)
		RouteMoveToNextSector();

	MMatchRule::OnRoundEnd();
}

bool MMatchRuleQuestChallenge::OnRun()
{
	//todok �� Ŭ���� ���� üũ�ؼ� ���� ���� Ŭ���� npc ��� ���� ������ �ű�� ���� �ʿ�
	if (GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		//SendClientLatencyPing();
		//ReAssignNPC();
	}

	return MMatchRule::OnRun();
}

bool MMatchRuleQuestChallenge::OnCheckRoundFinish()
{
	// ���� �����ϸ� ���带 ������
	if (m_pNpcMgr->GetNumNpc() == 0)
		return true;

	return false;
}


void MMatchRuleQuestChallenge::ProcessNpcSpawning()
{
	//todok �ӽ� �ڵ�� ���� ���� �����ϰ� ����� �״�
	// todok del
	// ��� INI�� ������ ���� �� �߾ӿ� ������Ű�� ��
	//if (m_pNpcMgr->GetNumNpc() < gs_MaxSpawn)
	//{
	//	// �÷��̾�鿡�� npc ���� Ŀ�ǵ带 ������
	//	//todok �ϴ� �����ϰ� ���� ��� �����Ѵ�.
	//	const char* szActorDef = NULL;
	//	int r = rand() % (int)gs_vecNpcName.size();
	//	szActorDef = gs_vecNpcName[r].c_str();

	//	SpawnNpc(szActorDef, 0, 0);
	//}

	// todok ��� ��� ��������Ʈ���� ���͸� �����ϴ� �� // ��ȹ ������ ������ �Ƚ��� ���� �ڵ� ���� �� ���� if�� ����̳�
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
						// drop item ����
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

	// ������ ��Ʈ�ѷ��� ã�´�. ��Ʈ�ѷ��� ����� ������ ������ ���
	MUID uidController = m_pPlayerMgr->FindSuitableController();
	if (uidController.IsInvalid())
		return;

	// npc �Ŵ����� ���� npc ���� ��û. �Ʊ� ã�� ��Ʈ�ѷ� uid�� �Ѱ��ش�
	MUID uidNpc = MMatchServer::GetInstance()->UseUID();
	m_pNpcMgr->AddNpcObject(uidNpc, uidController, nDropItemId);
	m_pPlayerMgr->IncreaseNpcControl(uidController, uidNpc);

	//todok szActorDef�� �̸��� �ƴ� �ε����� �������� ��������
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

			// Ŀ�ǵ� �����̴� �ݵ�� �ش� npc�� ��Ʈ�ѷ����� �Ѵ�. �ƴ϶�� ��ŷ�̳� ���� ��Ȳ
			if (pNpc->GetController() != uidSender)
			{
				_ASSERT(0); break;
			}

			// ��� ������ ����
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
	// ������������� �˻�����
	if (!MGetMatchWorldItemDescMgr()->GetItemDesc(nWorldItemId))
		return;

	MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject(uidKiller);
	if (!pPlayer) return;

	int nWorldItemExtraValues[WORLDITEM_EXTRAVALUE_NUM];
	for (int i = 0; i < WORLDITEM_EXTRAVALUE_NUM; i++) nWorldItemExtraValues[i] = 0;

	m_pStage->SpawnServerSideWorldItem(pPlayer, nWorldItemId, pos.x, pos.y, pos.z,
		QUEST_DYNAMIC_WORLDITEM_LIFETIME, nWorldItemExtraValues );
}