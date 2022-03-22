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

//todok 일단 주석이지만 아무래도 따로 운용해야겠지
//enum ZACTOR2_FLAG
//{
//	AF_NONE				= 0,
//	AF_LAND				= 0x1,				// 발이 땅에 닿아 있는지 여부
//	AF_BLAST			= 0x2,				// 띄움을 당한 상태인지 여부
//	AF_MOVING			= 0x4,				// 이동여부
//	AF_DEAD				= 0x8,				// 죽었다
//	AF_REQUESTED_DEAD	= 0x10,				// 서버에 죽음 요청을 했는지 여부
//	AF_BLAST_DAGGER		= 0x20,				// 띄움을 당한 상태인지 여부	
//
//	AF_MY_CONTROL		= 0x100,			// 내가 컨트롤하는지 여부
//
//
//	AF_SOUND_WOUNDED	= 0x1000,			// 한번 맞았을때 울었는지 여부
//};

// 일정 시간간격으로 NPC정보 계속 주고받는 데이터
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
	MMatchItemDesc			m_ItemDesc;			// npc의 무기(혹은 아이템 디스크립션)
	MQuestNPCInfo*			m_pNPCInfo;
	ZActorAnimation2		m_Animation;		// Animation부분을 관장
	ZBrain2*				m_pBrain;			// AI 부분을 관장
	ZTaskManager2			m_TaskManager;
	float					m_timeDieAnimationDone;
	float					m_fSpeed;			// 속도는 인스턴스 개개마다 다 다르다.
	int						m_nDamageCount;
	bool					m_bReserveStandUp;	// 쓰러지고 나서 일어나는데 지연 시간을 가짐
	DWORD					m_dwStandUp;		// 쓰러지고 나서 일어나는데까지 걸리는 시간
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
	void Stop(bool bWithAniStop=true);		// bWithAniStop이 false이면 애니메이션은 여전히 달리는 동작이다.
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
	

	// 특정시점의 hit test 를 리턴해줘야 한다, pOutPos 가 있으면 hit된 위치를 리턴해줘야 한다
	virtual ZOBJECTHITTEST HitTest( const rvector& origin, const rvector& to, float fTime, rvector *pOutPos=NULL );

	// 동작/이벤트 상속
	virtual bool IsDie();

	virtual MMatchTeam GetTeamID() { return MMT_BLUE; }			// NPC는 무조건 BLUE팀

	virtual void OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio=1.f, int nMeleeType=-1);

	// knockback을 적용받아야한다
	virtual void OnKnockback(rvector& dir, float fForce);
	virtual void OnDie();
	virtual void OnPeerDie(MUID& uidKiller);

	bool IsDieAnimationDone();
	bool CanSee(ZObject* pTarget);				///< 시야에 보이는지 여부
	bool CanAttackRange(ZObject* pTarget);		///< 원거리 공격 가능한지 여부
	bool CanAttackMelee(ZObject* pTarget, ZSkillDesc *pSkillDesc=NULL);	///< 근거리 공격 가능한지 여부, 만약 pSkillDesc=NULL이면 기본 근접공격

	MUID GetLastAttacker() { return m_pModule_HPAP->GetLastAttacker(); }

	void SaveHistory();

	bool IsBossType() { return (m_pNPCInfo->nGrade == NPC_GRADE_BOSS) || (m_pNPCInfo->nGrade == NPC_GRADE_LEGENDARY); }

	// 디버그용
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
// ○ NPC 정책
//
// * 모든 NPC는 같은 구조의 애니메이션 세트를 가진다. -> ZActorAnimation
// * 모든 NPC는 같은 구조의 Brain을 가지고, 단지 수치로서 NPC의 아이덴티티를 확립한다. 수치는 MBaseQuest에 기술되어 있음.
// * NPC구조는 크게 AI를 관장하는 ZBrain과 몸체인 ZActor로 구분한다.
// * ZTask란? : NPC의 일련의 행동 구분은 작게 Task로 구분된다. ZTask는 Command 패턴으로 되어있다. Task는 몸체에 달려 있으며, 
//    ZBrain이 Task를 입력해줌으로써 ZActor를 행동하게 한다.
// * ZBehabior란? : NPC 객체의 가장 큰 행동 분류이다. State 패턴으로 되어있다.
//
//
//
//


// sendDelayedMsgToMe



#endif