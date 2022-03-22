#ifndef _MMATCHRULEQUESTCHALLENGE_H
#define _MMATCHRULEQUESTCHALLENGE_H

#include "MMatchRule.h"
#include "MActorDef.h"
#include "MNewQuestScenario.h"

class MNewQuestPlayerManager;
class MNewQuestNpcManager;

class MMatchRuleQuestChallenge : public MMatchRule
{
	static MActorDefManager ms_actorDefMgr;
	static MNewQuestScenarioManager ms_scenarioMgr;
public:
	static void InitActorDefMgr();
	static void InitScenarioMgr();
	static MNewQuestScenarioManager* GetScenarioMgr();

private:

	MNewQuestPlayerManager* m_pPlayerMgr;
	MNewQuestNpcManager* m_pNpcMgr;

	MNewQuestScenario* m_pScenario;

	int m_nCurrSector;		// ���� �������� ����

protected:
	virtual bool RoundCount();										///< ���� ī��Ʈ. ��� ���尡 ������ false�� ��ȯ�Ѵ�.
	virtual void OnBegin();											///< ��ü ���� ���۽� ȣ��
	virtual void OnEnd();											///< ��ü ���� ����� ȣ��
	virtual void OnRoundBegin();									///< ���� ������ �� ȣ��
	virtual void OnRoundEnd();										///< ���� ���� �� ȣ��
	virtual bool OnRun();											///< ����ƽ�� ȣ��

	//virtual void ProcessNPCSpawn() {}						///< NPC �����۾�
	//virtual bool CheckNPCSpawnEnable() { return true; }					///< NPC�� ���� �������� ����
	//virtual void RouteGameInfo() {}							///< Ŭ���̾�Ʈ�� ���� ���� �����ش�.
	//virtual void RouteStageGameInfo() {}					///< ����� ������������ �ٲ� ���� ������ �����ش�.

	////virtual void OnCompleted();					///< ����Ʈ ������ ȣ��ȴ�.
	////virtual void OnFailed();					///< ����Ʈ ���н� ȣ��ȴ�.
	//virtual void RouteCompleted() {}			///< ����Ʈ ���� �޽����� ������. - ��������� �Բ� ������.
	//virtual void RouteFailed() {}				///< ����Ʈ ���� �޽��� ������.
	//virtual void DistributeReward() {}			///< ����Ʈ ������ ������ ���

	//virtual void OnRequestMovetoPortal(const MUID& uidPlayer) {}	/// �÷��̾ ��Ż�� �̵����� ��� ȣ��ȴ�.
	//virtual void OnReadyToNewSector(const MUID& uidPlayer) {}		/// ��Ż�� �̵��ϰ� ���� �̵��� �Ϸ�Ǿ��� ��� ȣ��ȴ�.

	//virtual void OnRequestTestSectorClear() {}
	//virtual void OnRequestTestFinish() {}

protected:
	virtual bool OnCheckRoundFinish();							///< ���尡 �������� üũ

public:
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_QUEST_CHALLENGE; }

public:
	MMatchRuleQuestChallenge(MMatchStage* pStage);
	virtual ~MMatchRuleQuestChallenge();

	virtual void OnCommand(MCommand* pCommand);

private:
	void RouteSpawnNpc(MUID uidNPC, MUID uidController, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex);
	void RouteNpcDead(MUID uidNPC, MUID uidKiller);
	void RouteMoveToNextSector();

	void ProcessNpcSpawning();
	void SpawnNpc(const char* szActorDef, int nCustomSpawnTypeIndex, int nSpawnIndex, int nDropItemId);
	void DropItemByNpcDead(const MUID& uidKiller, int nWorldItemId, const MVector& pos);
};

#endif