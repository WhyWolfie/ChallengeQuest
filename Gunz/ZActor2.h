#ifndef _ZACTOR2_H
#define _ZACTOR2_H

#include "MRTTI.h"
#include "ZCharacterObject.h"
#include "ZCharacter.h"
#include "MUID.h"
#include "RVisualMeshMgr.h"
#include "MObjectTypes.h"
#include "ZStateMachine.h"
#include "ZAI_Base.h"
#include "MBaseQuest.h"
#include "ZBrain2.h"
#include "ZActorAnimation2.h"
#include "ZTask2.h"
#include "ZTaskManager2.h"
#include "ZModule_HPAP.h"
#include "MMatchTransDataType.h"
#include "ZNPCInfoFromServer.h"

#include <list>
#include <string>
using namespace std;

_USING_NAMESPACE_REALSPACE2

class ZActor2Network;

//todok �ϴ� �ּ������� �ƹ����� ���� ����ؾ߰���
//enum ZACTOR2_FLAG
//{
//	AF_NONE				= 0,
//	AF_LAND				= 0x1,				// ���� ���� ��� �ִ��� ����
//	AF_BLAST			= 0x2,				// ����� ���� �������� ����
//	AF_MOVING			= 0x4,				// �̵�����
//	AF_DEAD				= 0x8,				// �׾���
//	AF_REQUESTED_DEAD	= 0x10,				// ������ ���� ��û�� �ߴ��� ����
//	AF_BLAST_DAGGER		= 0x20,				// ����� ���� �������� ����	
//
//	AF_MY_CONTROL		= 0x100,			// ���� ��Ʈ���ϴ��� ����
//
//
//	AF_SOUND_WOUNDED	= 0x1000,			// �ѹ� �¾����� ������� ����
//};

// ���� �ð��������� NPC���� ��� �ְ�޴� ������
struct ZACTOR2_BASICINFO {
	float			fTime;
	MUID			uidNPC;
	short			posx,posy,posz;
	short			velx,vely,velz;
	short			dirx,diry,dirz;
	BYTE			anistate;
};

struct MQuestNPCInfo;

/// 
class ZActor2 : public ZCharacterObject
{
	MDeclareRTTI;

	friend ZBrain2;
	friend ZActorAnimation2;
private:
	unsigned long int		m_nFlags;
	void UpdateHeight(float fDelta);
	void UpdatePosition(float fDelta);
protected:
	MMatchItemDesc			m_ItemDesc;			// npc�� ����(Ȥ�� ������ ��ũ����)
	MQuestNPCInfo*			m_pNPCInfo;
	ZActorAnimation2		m_Animation;		// Animation�κ��� ����
	ZBrain2*				m_pBrain;			// AI �κ��� ����
	ZTaskManager2			m_TaskManager;
	float					m_timeDieAnimationDone;
	float					m_fSpeed;			// �ӵ��� �ν��Ͻ� �������� �� �ٸ���.
	int						m_nDamageCount;
	bool					m_bReserveStandUp;	// �������� ���� �Ͼ�µ� ���� �ð��� ����
	DWORD					m_dwStandUp;		// �������� ���� �Ͼ�µ����� �ɸ��� �ð�
private:
	void InitFromNPCType(MQUEST_NPC nNPCType);
	void InitMesh(char* szMeshName, MQUEST_NPC nNPCType);
	void OnTaskFinished(ZTASK_ID nLastID);
	static void OnTaskFinishedCallback(ZActor2* pActor, ZTASK_ID nLastID);
protected:

	ZModule_HPAP			*m_pModule_HPAP;
	ZModule_Resistance		*m_pModule_Resistance;
	ZModule_FireDamage		*m_pModule_FireDamage;
	ZModule_ColdDamage		*m_pModule_ColdDamage;
	ZModule_PoisonDamage	*m_pModule_PoisonDamage;
	ZModule_LightningDamage	*m_pModule_LightningDamage;
	ZModule_Skills			*m_pModule_Skills;

	ZActor2Network*		m_pNetworkAgent;

	rvector				m_vAddBlastVel;
	float				m_fAddBlastVelTime;
	rvector				m_TargetDir;
	rvector				m_Accel;
	float				m_fDelayTime;

protected:
	bool				m_bTestControl;
	void TestControl(float fDelta);

	virtual void InitProperty();
	virtual void InitStatus();

	virtual void OnDraw();
	virtual void OnUpdate(float fDelta);
	virtual bool ProcessMotion(float fDelta);

	// task
	virtual void ProcessAI(float fDelta);
	void ProcessMovement(float fDelta);
	void CheckDead(float fDelta);

public:
	ZActor2();
	virtual ~ZActor2();
	static ZActor2* CreateActor(MQUEST_NPC nNPCType, bool bForceCollRadius35=false);
	void InputBasicInfo(ZBasicInfo* pni, BYTE anistate);
	void InputBossHpAp(float fHp, float fAp);
	void Input(AI_INPUT_SET nInput);
	void DebugTest();
	void SetMyControl(bool bMyControl);

	inline ZA_ANIM_STATE	GetCurrAni();
	inline void SetFlag(unsigned int nFlag, bool bValue);
	inline bool CheckFlag(unsigned int nFlag);
	inline void SetFlags(unsigned int nFlags);
	inline unsigned long GetFlags();
	inline bool IsMyControl();
	inline int GetHP();
	inline int GetAP();
	inline float GetHitRate();
public:
	void RunTo(rvector& dir);
	void Stop(bool bWithAniStop=true);		// bWithAniStop�� false�̸� �ִϸ��̼��� ������ �޸��� �����̴�.
	void RotateTo(rvector& dir);
	void OnNeglect( int nNum);

	virtual void OnBlast(rvector &dir);
	virtual void OnBlastDagger(rvector &dir,rvector& pos);
	virtual bool IsCollideable();
	virtual bool IsAttackable();
	virtual void Attack_Melee();
	virtual void Attack_Range(rvector& dir);
	virtual void Skill(int nSkill);

	bool isThinkAble();

	ZBrain2* GetBrain()					{ return m_pBrain; }
	MQuestNPCInfo* GetNPCInfo()			{ return m_pNPCInfo; }
	ZTaskManager2* GetTaskManager()		{ return &m_TaskManager; }
	

	// Ư�������� hit test �� ��������� �Ѵ�, pOutPos �� ������ hit�� ��ġ�� ��������� �Ѵ�
	virtual ZOBJECTHITTEST HitTest( const rvector& origin, const rvector& to, float fTime, rvector *pOutPos=NULL );

	// ����/�̺�Ʈ ���
	virtual bool IsDie();

	virtual MMatchTeam GetTeamID() { return MMT_BLUE; }			// NPC�� ������ BLUE��

	virtual void OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio=1.f, int nMeleeType=-1);

	// knockback�� ����޾ƾ��Ѵ�
	virtual void OnKnockback(rvector& dir, float fForce);
	virtual void OnDie();
	virtual void OnPeerDie(MUID& uidKiller);

	bool IsDieAnimationDone();
	bool CanSee(ZObject* pTarget);				///< �þ߿� ���̴��� ����
	bool CanAttackRange(ZObject* pTarget);		///< ���Ÿ� ���� �������� ����
	bool CanAttackMelee(ZObject* pTarget, ZSkillDesc *pSkillDesc=NULL);	///< �ٰŸ� ���� �������� ����, ���� pSkillDesc=NULL�̸� �⺻ ��������

	MUID GetLastAttacker() { return m_pModule_HPAP->GetLastAttacker(); }

	void SaveHistory();

	bool IsBossType() { return (m_pNPCInfo->nGrade == NPC_GRADE_BOSS) || (m_pNPCInfo->nGrade == NPC_GRADE_LEGENDARY); }

	// ����׿�
	char m_szOwner[64];
	float m_fLastBasicInfo;
	void SetOwner(const char* szOwner) { strcpy(m_szOwner,szOwner); }
};



void SetClientNPCInfoFromServerNPCInfo2( MQuestNPCInfo* pClientNPCInfo, const MTD_NPCINFO* pServerNPCInfo );



// inline /////////////////////////////////////////////////////////////////////////////////////////

inline void ZActor2::SetFlags(unsigned int nFlags)
{
	if (m_nFlags != nFlags)
	{
		m_nFlags = nFlags;
	}
}

inline void ZActor2::SetFlag(unsigned int nFlag, bool bValue)
{
	if (bValue) m_nFlags |= nFlag;
	else m_nFlags &= ~nFlag;
}

inline bool ZActor2::CheckFlag(unsigned int nFlag)
{
	return ((m_nFlags & nFlag) != 0);
}

inline unsigned long ZActor2::GetFlags() 
{ 
	return m_nFlags; 
}

inline ZA_ANIM_STATE ZActor2::GetCurrAni()
{ 
	return m_Animation.GetCurrState();
}

inline int ZActor2::GetHP()	
{ 
	return m_pModule_HPAP->GetHP(); 
}

inline int ZActor2::GetAP()	
{
	return m_pModule_HPAP->GetAP(); 
}

inline float ZActor2::GetHitRate()
{
	return (m_pNPCInfo->fAttackHitRate);
}

inline bool ZActor2::IsMyControl()
{
	return CheckFlag(AF_MY_CONTROL);
}

////////////////////////////////////////////////
// �� NPC ��å
//
// * ��� NPC�� ���� ������ �ִϸ��̼� ��Ʈ�� ������. -> ZActorAnimation
// * ��� NPC�� ���� ������ Brain�� ������, ���� ��ġ�μ� NPC�� ���̵�ƼƼ�� Ȯ���Ѵ�. ��ġ�� MBaseQuest�� ����Ǿ� ����.
// * NPC������ ũ�� AI�� �����ϴ� ZBrain�� ��ü�� ZActor�� �����Ѵ�.
// * ZTask��? : NPC�� �Ϸ��� �ൿ ������ �۰� Task�� ���еȴ�. ZTask�� Command �������� �Ǿ��ִ�. Task�� ��ü�� �޷� ������, 
//    ZBrain�� Task�� �Է��������ν� ZActor�� �ൿ�ϰ� �Ѵ�.
// * ZBehabior��? : NPC ��ü�� ���� ū �ൿ �з��̴�. State �������� �Ǿ��ִ�.
//
//
//
//


// sendDelayedMsgToMe



#endif