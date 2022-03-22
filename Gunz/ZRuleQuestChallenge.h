#ifndef _ZRULE_QUESTCHALLENGE_H
#define _ZRULE_QUESTCHALLENGE_H

#include "ZRule.h"

class ZFSMManager;
class MActorDefManager;
class ZActorActionManager;
class MNewQuestScenario;


class ZRuleQuestChallenge : public ZRule
{
	ZFSMManager* m_pFsmManager;
	MActorDefManager* m_pActorDefMgr;
	ZActorActionManager* m_pActorActionMgr;


	MNewQuestScenario* m_pScenario;	// �� ����Ʈ�� �ó�����
	int m_nCurrSector;				// ���� �������� ���� ��ȣ

public:
	ZRuleQuestChallenge(ZMatch* pMatch);
	virtual ~ZRuleQuestChallenge();

public:

	ZActorActionManager* GetActorActionMgr() { return m_pActorActionMgr; }

	static bool LoadScenarioMap(const char* szScenarioName);

	virtual bool Init();
	virtual void OnUpdate(float fDelta);
	virtual void OnSetRoundState(MMATCH_ROUNDSTATE roundState);

	virtual float FilterDelayedCommand(MCommand* pCommand);
	virtual bool OnCommand(MCommand* pCommand);

	virtual int GetRoundMax();

	void SpawnActor(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl);
	void MoveToNextSector();
};



#endif