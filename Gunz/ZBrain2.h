#ifndef _ZBRAIN2_H
#define _ZBRAIN2_H

class ZActor2;

#include "ZActorAnimation2.h"
#include "ZTask2.h"
#include "ZTaskManager2.h"
#include "ZBehavior2.h"
#include "ZTimer.h"

#define		DIST_FORCEDIN		300000.0f
#define		DIST_IN				1600000.0f
#define		DIST_OUT			5000000.0f
#define		DIST_HEIGHT			320.0f


///< Actor�� AI��ü
class ZBrain2
{
	friend ZActor2;
	friend ZActorAnimation2;

private:
	// Timer
	ZUpdateTimer		m_PathFindingTimer;
	ZUpdateTimer		m_AttackTimer;
	ZUpdateTimer		m_DefaultAttackTimer;
	DWORD				m_dwNoSkillTimer;
	DWORD				m_dwNeglectTimer;

	float				m_fDistForcedIn;
	float				m_fDistIn;
	float				m_fDistOut;

protected:
	ZActor2*			m_pBody;			///< ��ü
	ZBehavior2			m_Behavior;
	MUID				m_uidTarget;		///< Ÿ�����ϰ� �ִ� �� UID
	
	rvector				m_exPosition;		///< ��ü�� ���� ��ġ
	DWORD				m_dwExPositionTime;	///< ���� ��ġ ����� �ð�
	rvector				m_exPositionForWarp;		///< ��ü�� ���� ��ġ
	DWORD				m_dwExPositionTimeForWarp;	///< ���� ��ġ ����� �ð�
	

	// ��ã�� ����
	list<rvector>		m_WayPointList;

	// Update tiemr
	float MakePathFindingUpdateTime( char nIntelligence);
	float MakeAttackUpdateTime( char nAgility);
	float MakeDefaultAttackCoolTime();
	static float MakeSpeed( float fSpeed);
	void MakeNeglectUpdateTime();
	void AdjustWayPointWithBound(list<rvector>& wayPointList, RNavigationMesh* pNavMesh);
	bool EscapeFromStuckIn(list<rvector>& wayPointList);
	void ResetStuckInState();
	void ResetStuckInStateForWarp();
	void PushWayPointsToTask();


	// Think
	bool FindTarget();
	void ProcessAttack( float fDelta);
	bool GetUseableSkill( int *pnSkill, MUID *pTarget, rvector *pTargetPosition);
	void ProcessBuildPath( float fDelta);


	// ��ü������ �̺�Ʈ �߻� �������̽�
	virtual void OnBody_AnimEnter( ZA_ANIM_STATE nAnimState);
	virtual void OnBody_AnimExit( ZA_ANIM_STATE nAnimState);
	virtual void OnBody_CollisionWall();
	virtual void OnBody_OnTaskFinished( ZTASK_ID nLastID);


public:
	ZBrain2();
	virtual ~ZBrain2();

	static ZBrain2* CreateBrain( MQUEST_NPC nNPCType);

	void Init( ZActor2* pBody);

	void Think( float fDelta);

	ZActor2* GetBody()					{ return m_pBody; }
	ZObject* GetTarget();

	void OnDamaged();
};


#endif