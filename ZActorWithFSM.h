#pragma once
#include "ZActorBase.h"
#include "ZFSMCondition.h"
#include "ZFSMStateEnteredTimeStamper.h"
#include "ZFSMFunctionServer.h"

class ZActorActionManager;
class MActorDef;
class ZFSMManager;
class ZFSM;
class ZFSMState;
class IActorAction;
class ZActorActionShotCommon;

// ���� �ð��������� NPC���� ��� �ְ�޴� ������
struct ZACTOR_WITHFSM_BASICINFO {
	//float			fTime;	//todok del
	MUID			uidNPC;
	short			posx,posy,posz;
	short			velx,vely,velz;
	short			dirx,diry,dirz;
	int				nActionIndex;
};

//todok �Ʒ� �����Լ��� �� �����̳� �ӽð� ��ȯ�ϴ� �͵� ó���ؾ� �Ѵ�

class ZActorWithFSM : public ZActorBase, public ZFSMFunctionServer, public IFSMConditionSource
{
	MDeclareRTTI;

private:
	ZActorActionManager* m_pActionMgr;

	MActorDef* m_pActorDef;
	ZFSM* m_pFsm;
	ZFSMState* m_pCurrState;
	ZFSMStateEnteredTimeStamper m_stateEnteredTimeStamper;

	bool m_bPostedActionExecute;	// �׼��� ���۵Ǿ����� �˷ȴ°�//todok del
	bool m_bPostedRangeShotInAction;	// ���� state�� �׼��� range shot�� ���� �ִٸ� �̰��� peer post�ߴ°�	//todok del

	IActorAction* m_pCurrAction;		// ���� ���� ���� �׼�
	float m_pCurrActionStartTime;		// ���� ���� ���� �׼� ���� �ð�
	
	set<const ZActorActionShotCommon*> m_setProcessedShotInAction;	// ���� ���� ���� �׼� ������ ó���� �� ���
	bool IsProcessedShot(const ZActorActionShotCommon* p) { return m_setProcessedShotInAction.end() != m_setProcessedShotInAction.find(p); }
	void SetProcessedShot(const ZActorActionShotCommon* p) { m_setProcessedShotInAction.insert(p); }

	float m_fGroggy;
	MUID m_uidTarget;

	list<rvector> m_listWaypoint;

	rvector m_blastedDir;
	rvector m_daggerBlastedDir;
	rvector m_daggerBlastedPos;

	rvector m_RealPositionBefore;	// �̵��� ���Ե� �ִϸ��̼� ó����


	ZModule_HPAP			*m_pModule_HPAP;
	ZModule_Resistance		*m_pModule_Resistance;
	ZModule_FireDamage		*m_pModule_FireDamage;
	ZModule_ColdDamage		*m_pModule_ColdDamage;
	ZModule_PoisonDamage	*m_pModule_PoisonDamage;
	ZModule_LightningDamage	*m_pModule_LightningDamage;

	bool m_bMyControl;
	unsigned long int m_nLastTimePostBasicInfo;

public:
	ZActorWithFSM(IGame* pGame, ZActorActionManager* pActionMgr);
	~ZActorWithFSM();

	virtual MMatchTeam GetTeamID() { return MMT_BLUE; }			// NPC�� ������ BLUE��

	virtual void OnDraw();
	virtual void OnUpdate(float fDelta);

	virtual void SetMyControl(bool bMyControl);
	virtual bool IsMyControl();

	bool IsMovingAnimation();
	virtual bool IsCollideable();
	virtual bool IsDie();

	virtual bool IsNeverPushed() { return false; }
	virtual float GetCollideRadius() { return 35.f; }
	virtual float ModifyAttackDamage(float fDamage) { return fDamage; }

	virtual int GetActualHP() { return m_pModule_HPAP->GetHP(); }
	virtual int GetActualAP() { return m_pModule_HPAP->GetAP(); }
	virtual int GetActualMaxHP() { return m_pModule_HPAP->GetMaxHP(); }
	virtual int GetActualMaxAP() { return m_pModule_HPAP->GetMaxAP(); }

	virtual void InputBasicInfo(ZBasicInfo* pni, BYTE anistate) {}
	virtual void InputBossHpAp(float fHp, float fAp) {}

	virtual void OnPeerDie(MUID& uidKiller) {}
	virtual void OnDie() {}

	// ����׿�
	virtual void GetDebugInfoString(char* szBuf1, char* szBuf2, int index) {}

public:
	// IFSMConditionSource ����
	virtual float Cond_GetHp() { return GetActualHP(); }
	virtual float Cond_GetGroggy() { return m_fGroggy; }
	virtual const rvector& Cond_GetVelocity() { return GetVelocity(); }
	virtual bool Cond_IsEndAction();
	virtual bool Cond_IsLanding();

	virtual bool Cond_HasTarget();
	virtual void Cond_GetPos(rvector& out) { out = GetPosition(); }
	virtual void Cond_GetTargetPos(rvector& out);

	virtual void Cond_GetDirection(rvector& out) { out = GetDirection(); }

	virtual int Cond_GetDice() { return GetDice(); }

	virtual bool Cond_CheckStateCooltime(ZFSMState* pState, DWORD dwCooltime);
	virtual DWORD Cond_GetStateEnteredTime();

	virtual bool IsActiveLightningDamageModule();

	virtual bool Cond_Pick(float fAngleDegree, float fDist);

public:
	// ZFSMFunctionServer ����
	virtual void Func_FindTarget();
	virtual void Func_SetTargetLastAttacker();
	virtual void Func_RotateToTarget(float fFrameDelta);
	virtual void Func_FaceToTarget();

	virtual void Func_ReduceGroggy(float f);

	virtual void Func_BuildWaypointsToTarget();
	virtual void Func_RunWaypoints(float fFrameDelta);
	
	virtual void Func_Disappear(float fFrameDelta, DWORD nDelay, float fRatio);
	virtual void Func_PostDead();
	virtual void Func_OnEnterBlastedSoar();
	virtual void Func_OnEnterBlastedThrust();
	virtual void Func_OnUpdateBlastedThrust(float fFrameDelta);
	virtual void Func_ExitSuffering();
	virtual void Func_OnEnterDie();

public:
	bool InitWithActorDef(MActorDef* pActorDef, ZFSMManager* pFsmMgr);

	MUID GetTargetUID() { return m_uidTarget; }

private:
	bool m_bSuffering;		// ������ �� ���� �ǰ� ���� ���ΰ�
	bool m_bMoving;
public:
	bool IsMoving() { return m_bMoving; }
	void SetMoving(bool b) { m_bMoving = b; }
	bool IsSuffering() { return m_bSuffering; }
	bool IsAir() 
	{
		// ���߿� ���ְų� �������� ������ üũ
		bool bAir = (GetDistToFloor() > 0.1f || GetVelocity().z > 0.1f);
		return bAir;
	}


public:
	void InitMesh(char* szMeshName);
	bool ProcessMotion(float fDelta);
	void ProcessShotInAction(float fDelta);
	void UpdateVelocity(float fDelta);
	void UpdatePosition(float fDelta);
	void UpdateHeight(float fDelta);
	void UpdateGroggy(float fDelta);
	void SetAnimationInAction(IActorAction* pAction);
	void UpdateFsm(float fDelta);
	void OnExitState();
	void OnEnterState();
	ZFSMState* CheckGlobalTransition();

	void RotateToDirection(rvector vTargetDir, float fFrameDelta);

	virtual void AddLandingEffect(rvector vPos, rvector vDir);

	void ValidateTarget();

	void PostBasicInfo();
	void OnBasicInfo(ZACTOR_WITHFSM_BASICINFO* pbi);

	// ����/�̺�Ʈ ���
	//void OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType);

	virtual void OnBlast(rvector &dir);
	virtual void OnBlastDagger(rvector &dir,rvector& pos);

	virtual ZOBJECTHITTEST HitTest( const rvector& origin, const rvector& to, float fTime, rvector *pOutPos );
	void OnPeerMeleeShot(const char* szActionName);	//todok del
	void OnPeerActionExecute(int nActionIndex);
	void OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType);
	void OnDamaged_LightningSplash();
};