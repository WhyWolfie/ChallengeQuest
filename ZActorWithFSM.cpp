#include "stdafx.h"
#include "ZActorWithFSM.h"
#include "MActorDef.h"
#include "ZFSMManager.h"
#include "ZFSM.h"
#include "ZFSMState.h"
#include "ZActorActionManager.h"
#include "ZActorAction.h"
#include "ZNavigationMesh.h"
#include "ZScreenEffectManager.h"
#include "ZModule_HPAP.h"
#include "ZModule_Resistance.h"
#include "ZModule_FireDamage.h"
#include "ZModule_ColdDamage.h"
#include "ZModule_LightningDamage.h"
#include "ZModule_PoisonDamage.h"

MImplementRTTI(ZActorWithFSM, ZActorBase);


ZActorWithFSM::ZActorWithFSM(IGame* pGame, ZActorActionManager* pActionMgr)
: ZActorBase(pGame)
, m_pActionMgr(pActionMgr)
, m_pActorDef(NULL)
, m_pFsm(NULL)
, m_pCurrState(NULL)
, m_pCurrAction(NULL)
, m_pCurrActionStartTime(0)
, m_uidTarget(0,0)
, m_fGroggy(0)
, m_bPostedActionExecute(false)
, m_bPostedRangeShotInAction(false)
, m_bMoving(false)
, m_bSuffering(false)
, m_RealPositionBefore(0,0,0)
, m_bMyControl(false)
, m_nLastTimePostBasicInfo(0)
{
	m_bIsNPC = true;
	SetVisible(true);
	m_bInitialized = true;

	m_pModule_HPAP				= new ZModule_HPAP;
	m_pModule_Resistance		= new ZModule_Resistance;
	m_pModule_FireDamage		= new ZModule_FireDamage;
	m_pModule_ColdDamage		= new ZModule_ColdDamage;
	m_pModule_PoisonDamage		= new ZModule_PoisonDamage;
	m_pModule_LightningDamage	= new ZModule_LightningDamage;

	AddModule(m_pModule_HPAP);
	AddModule(m_pModule_Resistance);
	AddModule(m_pModule_FireDamage);
	AddModule(m_pModule_ColdDamage);
	AddModule(m_pModule_PoisonDamage);
	AddModule(m_pModule_LightningDamage);

	// 이동 모듈은 직접 업데이트 할 것이므로 비활성 상태로 만들어야 ZModuleContainer::UpdateModules()에서 중복 update되지 않는다
	m_pModule_Movable->Active(false);

}

ZActorWithFSM::~ZActorWithFSM()
{
	EmptyHistory();
	DestroyShadow();

	RemoveModule(m_pModule_HPAP);
	RemoveModule(m_pModule_Resistance);
	RemoveModule(m_pModule_FireDamage);
	RemoveModule(m_pModule_ColdDamage);
	RemoveModule(m_pModule_PoisonDamage);
	RemoveModule(m_pModule_LightningDamage);

	delete m_pModule_HPAP;
	delete m_pModule_Resistance;
	delete m_pModule_FireDamage;
	delete m_pModule_ColdDamage;
	delete m_pModule_PoisonDamage;
	delete m_pModule_LightningDamage;
}

void ZActorWithFSM::InitMesh(char* szMeshName)
{
	_ASSERT(m_pVMesh == NULL);

	AllocObjectVMesh();
	m_pVMesh->InitNpcMesh(this, szMeshName);
}

void ZActorWithFSM::OnDraw()
{
	if (m_pVMesh == NULL) return;

	Draw_SetLight(GetPosition() );

	//todok 죽었을 때 사라지는 표현 필요
//	if( IsDieAnimationDone() )
//	{
//#define TRAN_AFTER		0.5f	// 이 시간 이후부터
//#define VANISH_TIME		1.f		// 이 시간동안 투명해진다
//
//		if(m_timeDieAnimationDone==-1) m_timeDieAnimationDone = m_pGame->GetTime();
//
//		//float fOpacity = max(0.f,min(1.0f,(	VANISH_TIME-(m_pGame->GetTime()-GetDeadTime() - TRAN_AFTER))/VANISH_TIME));
//		float fOpacity = max(0.f,min(1.0f,(	VANISH_TIME-(m_pGame->GetTime()-m_timeDieAnimationDone - TRAN_AFTER))/VANISH_TIME));
//
//		m_pVMesh->SetVisibility(fOpacity);
//	}
//	else {
//		if(!m_bHero) m_pVMesh->SetVisibility(1.f);
//		m_timeDieAnimationDone = -1;
//	}

	m_pVMesh->Render();

	//Cond_Pick() 테스트용 : 해당 방향으로 라인을 그린다
	/*D3DXMATRIX world;
	MakeWorldMatrix(&world, GetPosition(), GetDirection(), rvector(0,0,1));
	RGetDevice()->SetTransform(D3DTS_WORLD, &world);
	RGetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	if (!Cond_Pick(90, 200))
		RDrawLine(rvector(-200, 5, 0), rvector(0, 5, 0), 0xffffff);
	if (!Cond_Pick(-90, 200))
		RDrawLine(rvector(200, 5, 0), rvector(0, 5, 0), 0xffffff);
	if (!Cond_Pick(0, 200))
		RDrawLine(rvector(0, 5, -200), rvector(0, 5, 0), 0xffffff);
	if (!Cond_Pick(180, 200))
		RDrawLine(rvector(0, 5, 200), rvector(0, 5, 0), 0xffffff);
		*/
}

void ZActorWithFSM::OnUpdate( float fDelta )
{
	//todok 죽으면 실제로 제거해야한다. 지금 죽어서 안보이는 놈도 업데이트가 계속 돌아가고 있음

	if(m_pVMesh && !IsDie()) {
		m_pVMesh->SetVisibility(1.f);
	}

	if (IsMyControl())
	{
		UpdateFsm(fDelta);

		PostBasicInfo();
		/*m_TaskManager.Run(fDelta);

		ProcessNetwork(fDelta);

		// for test - bird
		if (m_bTestControl)
		{
			TestControl(fDelta);
		}
		else
		{
			__BP(60,"ZActor::OnUpdate::ProcessAI");
			if(isThinkAble())
				ProcessAI(fDelta);
			__EP(60);
		}*/

		UpdateVelocity(fDelta);
		UpdatePosition(fDelta);
		UpdateHeight(fDelta);

		UpdateGroggy(fDelta);
	}
	else
	{
		// 내가 조종하는 npc가 아닌 경우는 단순하게 현재 velocity 기반으로 위치 업데이트
		m_pModule_Movable->Update(fDelta);
		UpdateHeight(fDelta);
	}

	ProcessMotion(fDelta);

	ProcessShotInAction(fDelta);
}

void ZActorWithFSM::SetMyControl(bool bMyControl)
{
	m_bMyControl = bMyControl;
}

bool ZActorWithFSM::IsMyControl()
{
	return m_bMyControl;
}

bool ZActorWithFSM::IsMovingAnimation()
{
	/*if (!m_pCurrState) return false;
	IActorAction* pAction = m_pCurrState->GetAction();
	if (!pAction) return false;
	return pAction->IsMovingAnimation();*/

	if (!m_pCurrAction) return false;
	return m_pCurrAction->IsMovingAnimation();
}

bool ZActorWithFSM::IsCollideable()
{
	if (!m_Collision.IsCollideable())
		return false;

	return !IsDie();
}

bool ZActorWithFSM::IsDie() 
{ 
	if (!m_pFsm) { _ASSERT(0); return true; }

	if (m_pFsm->GetState(FSM_BISTATE_DIE) == m_pCurrState)
		return true;

	if (m_pFsm->GetState(FSM_BISTATE_WAIT_DELETE) == m_pCurrState)
		return true;

	return false;
}

bool ZActorWithFSM::InitWithActorDef( MActorDef* pActorDef, ZFSMManager* pFsmMgr )
{
	m_pActorDef = pActorDef;

	// init mesh
	_ASSERT(m_pVMesh == NULL);
	AllocObjectVMesh();
	m_pVMesh->InitNpcMesh(this, m_pActorDef->GetModelName());
	CreateShadow();

	m_Collision.SetHeight(m_pActorDef->GetCollisionHeight());
	m_Collision.SetRadius(m_pActorDef->GetCollisionRadius());

	// init hp ap
	m_pModule_HPAP->SetMaxHP(pActorDef->GetMaxHp());
	m_pModule_HPAP->SetHP(pActorDef->GetMaxHp());

	m_pModule_HPAP->SetMaxAP(pActorDef->GetMaxAp());
	m_pModule_HPAP->SetAP(pActorDef->GetMaxAp());
	
	m_pModule_HPAP->SetRealDamage(true);

	// init fsm
	m_pFsm = pFsmMgr->GetFsm( pActorDef->GetFsmName());
	if (!m_pFsm)
	{
		_ASSERT(0);
		return false;
	}

	m_pCurrState = m_pFsm->GetState( m_pFsm->GetEntryStateName());
	if (!m_pCurrState)
	{
		_ASSERT(0);
		return false;
	}
	OnEnterState();

	return true;
}

bool ZActorWithFSM::ProcessMotion(float fDelta)
{
	// 메쉬 애니메이션 업데이트를 담당하는 함수!

	if (!m_pVMesh) return false;
	m_pVMesh->Frame();

	rvector vProxyPosition = GetPosition();
	rvector vProxyDirection = GetDirection();

	D3DXMATRIX world;

	rvector MeshPosition;	// 이동량만큼을 뺀 메쉬 렌더링 위치
	rvector vAnimationPositionDiff(0,0,0);

	// 움직임이 있는 애니메이션은 위치조절을 해줘야 한다.
	// 이전 프레임과 현재 프레임에서의 애니메이션 발 위치 변화량을 구해서 실제 액터의 위치를 추가 이동시켜주고
	// 메쉬는 그만큼 오프셋을 반대로 걸어서 렌더링해야 한다.
	if(IsMovingAnimation())
	{
		MakeWorldMatrix(&world,rvector(0,0,0),vProxyDirection,rvector(0,0,1));

		// 발의 위치를 로컬 좌표계로 얻어낸다
		rvector footPosition = m_pVMesh->GetFootPosition();
		rvector realPosition = footPosition * world;

		MeshPosition = vProxyPosition - realPosition ;

		vAnimationPositionDiff = footPosition - m_RealPositionBefore;	// 이전 프레임과 현재의 발 위치 변화량

		// 애니메이션의 움직임을 월드의 움직임으로 변환한다
		vAnimationPositionDiff = vAnimationPositionDiff * world;

		m_RealPositionBefore = footPosition;
	}
	else
		MeshPosition = vProxyPosition;

	MakeWorldMatrix(&world, MeshPosition, vProxyDirection, rvector(0,0,1));
	m_pVMesh->SetWorldMatrix(world);

	if(IsMyControl() && IsMovingAnimation())
	{
		// 애니메이션에 포함된 이동량만큼 액터 위치를 이동
		rvector origdiff=fDelta*GetVelocity();

		rvector diff = vAnimationPositionDiff;
		diff.z+=origdiff.z;

		if(GetDistToFloor()<0 && diff.z<0) diff.z=-GetDistToFloor();

		Move(diff);
	}

/*	//todok 라이트닝 피격처리
	if(IsActiveModule(ZMID_LIGHTNINGDAMAGE)==false) {// 라이트닝 에니메이션 루프..

		if (m_pVMesh->isOncePlayDone())
		{
			m_Animation.Input(ZA_ANIM_DONE);
		}
	}
*/
	return true;
}

void ZActorWithFSM::UpdateVelocity(float fDelta)
{
	// todok del 원래 ZActor에 있던 검사 (플레이어 캐릭터와 기준이 다르다)
	//bool bOnFloor = !(GetDistToFloor() > 10.f || GetVelocity().z > 0.1f);

	/*// 어느정도 공중에 떠있거나 위로 날아가는 중이면 land 플래그를 끈다
	if (GetDistToFloor() > 10.f || GetVelocity().z > 0.1f)
	{
		SetFlag(AF_LAND, false);
	}
	else 
	{
		if(!CheckFlag(AF_LAND))
		{
			SetFlag(AF_LAND, true);
			m_Animation.Input(ZA_EVENT_REACH_GROUND);
		}
	}*/

	//if(!bOnFloor)
	// 무빙애니메이션인 경우 애니메이션을 온전히 출력하기 위해 중력을 적용하지 않는다
	if (IsAir() && !IsMovingAnimation())
		m_pModule_Movable->UpdateGravity(fDelta);

	//if ( CheckFlag(AF_MOVING) && CheckFlag(AF_LAND) &&
	//	((GetCurrAni() == ZA_ANIM_WALK) || (GetCurrAni() == ZA_ANIM_RUN)))

	// npc의 의지대로 감속 가속할 수 있는 상황인가
	if (!IsSuffering() && !IsAir())
	{
		// 달리고 싶어하는 중인가
		if (IsMoving())
		{
			// 가속처리
			const float fAccel = 10000.f;
			AddVelocity(m_Direction * (fAccel * fDelta));

			// 최고속력은 넘지 않도록 한다
			float fSpeed = m_pActorDef->GetSpeed();

			rvector vel = GetVelocity();
			if (Magnitude(vel) > fSpeed)
			{
				Normalize(vel);
				vel *= fSpeed;
				SetVelocity(vel);
			}
		}
		else
		{
			// 감속처리
			rvector vVelocity = GetVelocity();
			vVelocity.x = 0;
			vVelocity.y = 0;
			SetVelocity(vVelocity);
		}
	}

	/*todok 특수피격(단검 넉백의 속도 조정)
	if(CheckFlag(AF_BLAST_DAGGER)) {
		float fSpeed = m_fSpeed;
		//		rvector vel = GetVelocity() * fDelta * 100;

#define BLAST_DAGGER_MAX_TIME 0.8f

		float fTime = max((1.f - (m_pGame->GetTime() - m_fAddBlastVelTime) / BLAST_DAGGER_MAX_TIME),0.0f);

		if( fTime < 0.4f )
			fTime = 0.f;

		float fPower = 400.f * fTime * fTime * fDelta * 80.f;

		//		m_vAddBlastVel.z -= fDelta;

		if(fPower==0.f)
			SetFlag(AF_BLAST_DAGGER,false);//힘이 다된것

		rvector vel = m_vAddBlastVel * fPower;

		SetVelocity( vel );

		return;
	}*/

//todok 달리는 상황이 아닌 경우의 감속 루틴이지만 임시로 vel = 0처리함
/*
	if (ZActorAnimation::IsAttackAnimation(GetCurrAni()))
	{
		SetVelocity(rvector(0,0,0));
	}
	else
	{
		// 브레이크를 걸자
		rvector dir=rvector(GetVelocity().x,GetVelocity().y,0);
		float fSpeed=Magnitude(dir);
		Normalize(dir);

		float fRatio = m_pModule_Movable->GetMoveSpeedRatio();

		float max_speed = 600.f	* fRatio;	// TODO : max는 module_movable 을 참조하도록 한다

		// 최대속도 이상이면 최대속도에 맞춘다..
		if(fSpeed>max_speed)
			fSpeed=max_speed;

#define NPC_STOP_SPEED			2000.f			// 아무키도 안눌렀을때 감속도..

		fSpeed = max(fSpeed-NPC_STOP_SPEED*fDelta,0);
		SetVelocity(dir.x*fSpeed, dir.y*fSpeed, GetVelocity().z);
	}
*/
}

void ZActorWithFSM::UpdatePosition(float fDelta)
{
	//todok 특수피격
/*	if( CheckFlag(AF_MY_CONTROL) ) 
	{
		if ((CheckFlag(AF_BLAST) == true) && (GetCurrAni() == ZA_ANIM_BLAST) && (GetVelocity().z < 0.0f))
		{
			m_Animation.Input(ZA_EVENT_REACH_PEAK);
		}

		if ((CheckFlag(AF_BLAST_DAGGER) == true) && (GetCurrAni() == ZA_ANIM_BLAST_DAGGER) )
		{
			if( Magnitude(GetVelocity()) < 20.f )
			{
				m_Animation.Input(ZA_EVENT_REACH_GROUND_DAGGER);
				SetFlag(AF_BLAST_DAGGER,false);
			}
		}
	}
*/

	//todok del
	if (IsSuffering())
	{
		mlog("suffering vel:%f %f %f\n", GetVelocity().x, GetVelocity().y, GetVelocity().z);
	}

	//todok 굉장히 민감한 부분이다.  이것을 직접 update한다는 것은 UpdateModule에서 중복실행되지 않도록
	// m_pModule_Movable->Active(false)를 해둔다는 뜻이다. 또한 피어가 컨트롤하는 npc의 경우엔 어떻게 할 것인지 생각해야 한다.
	m_pModule_Movable->Update(fDelta);

	//todok del
	if (IsSuffering())
	{
		mlog("suffering vel:%f %f %f\n", GetVelocity().x, GetVelocity().y, GetVelocity().z);
	}

	//todok del
	/*if (m_pModule_Movable->isLanding()) {
		mlog("actor isLanding = true, position.z = %f, distToFloor = %f (%f)\n", GetPosition().z, GetDistToFloor(), fDelta);
	}*/

	// 착지 순간의 처리
	if(m_pModule_Movable->isLanding())
	{
		//SetFlag(AF_LAND, true);
		//m_Animation.Input(ZA_EVENT_REACH_GROUND);
		SetVelocity(0,0,0);

		if(GetPosition().z + 100.f < m_pModule_Movable->GetFallHeight())
		{
			float fSpeed=fabs(GetVelocity().z);

			// 점프 착지시 먼지..
			rvector vPos = GetPosition();
			rvector vDir = rvector(0.f,0.f,-1.f);
			vPos.z += 50.f;

			RBSPPICKINFO pInfo;
			if (m_pGame->PickWorld(vPos, vDir, &pInfo))
			{
				vPos = pInfo.PickPos;

				vDir.x = pInfo.pInfo->plane.a;
				vDir.y = pInfo.pInfo->plane.b;
				vDir.z = pInfo.pInfo->plane.c;

				AddLandingEffect(vPos, vDir);
			}
		}
	}
}

void ZActorWithFSM::UpdateHeight(float fDelta)
{
	// 발이 뭍혔거나 내려가야 하는데 못간경우
	rvector diff=fDelta*GetVelocity();

	bool bUp = (diff.z>0.01f);
	bool bDownward= (diff.z<0.01f);

	if(GetDistToFloor()<0 || (bDownward && m_pModule_Movable->GetLastMove().z>=0))
	{
		if(GetVelocity().z<1.f && GetDistToFloor()<1.f)
		{
			if(GetVelocity().z<0)
				SetVelocity(GetVelocity().x,GetVelocity().y,0);
		}
	}

	// 계단 같은곳에서 스르륵 올라오도록..
	if(GetDistToFloor()<0 && !IsDie())
	{
		float fAdjust=400.f*fDelta;	// 초당 이만큼
		rvector diff=rvector(0,0,min(-GetDistToFloor(),fAdjust));
		Move(diff);
	}
}

void ZActorWithFSM::UpdateGroggy(float fDelta)
{
	// 그로기의 자연 회복을 적용
	if (!m_pActorDef) { _ASSERT(0); return; }
	float fRecoverSpeed = m_pActorDef->GetGroggyRecoverySpeed();
	m_fGroggy -= fRecoverSpeed * fDelta;
	if (m_fGroggy < 0)
		m_fGroggy = 0;

	//todok del
	//mlog("ㅂACTOR_GROGGY : %f\n", m_fGroggy);
}

ZFSMState* ZActorWithFSM::CheckGlobalTransition()
{
	/* todok 죽음 전이는 fsm 정의 xml에서 수동으로 하도록 한다.
	// todok 따라서 서버에 죽음 통보하는 것은 func으로 구현해야 한다.
	// 우선 죽음 검사부터.. todok 여기에 두지 말고 함수로 빼서 옮기자
	ZFSMState* pDieState = m_pFsm->GetState(FSM_BISTATE_DIE);
	if (m_pCurrState != pDieState)
	{
		if (Cond_GetHp() <= 0)
		{
			//todok 서버에 보내야 한다. 죽음 상태 진입시 바로 보내자 근데 이걸 enter function으로 구현해야 하나?
			//ZPostQuestRequestNPCDead(uidAttacker, GetUID(), GetPosition());
			return pDieState;
		}
	}*/
	return NULL;
}

void ZActorWithFSM::SetAnimationInAction(IActorAction* pAction)
{
	if (!pAction) return;

	const char* szAnimName = pAction->GetAnimationName();
	if (!m_pVMesh->SetAnimation(ani_mode_lower, szAnimName, false))
		_ASSERT(0);
}

void ZActorWithFSM::UpdateFsm(float fDelta)
{
	if (!IsMyControl()) return;
	if (!m_pFsm) { _ASSERT(0); return; }
	if (!m_pCurrState) { _ASSERT(0); return; }

	ValidateTarget();

	// 전이 검사
	ZFSMState* pNextState = NULL;

	pNextState = CheckGlobalTransition();
	if (!pNextState)
		pNextState = m_pCurrState->CheckTransition(this);

	if (pNextState)
	{
		OnExitState();
		m_pCurrState = pNextState;
		OnEnterState();
	}

	// 액션 실행
	IActorAction* pAction = m_pCurrState->GetAction();
	if (pAction)
	{
		SetAnimationInAction(pAction);

		//todok del
/*		const ZActorActionMeleeShot* pMeleeShot = pAction->GetMeleeShot();
		const ZActorActionRangeShot* pRangeShot = pAction->GetRangeShot();

		// 공격 커맨드는 state내에서 1번만 보낸다
		if (pMeleeShot && !m_bPostedActionExecute)
		{
			ZPostNewQuestPeerNpcAttackMelee(m_UID, pAction->GetName());
			m_bPostedActionExecute = true;
		}
		if (pRangeShot && !m_bPostedRangeShotInAction)
		{
			ZPostNewQuestPeerNpcAttackRange(m_UID, pAction->GetName());
			m_bPostedRangeShotInAction = true;
		}*/
	}

	// function 실행
	int numFunc = m_pCurrState->GetFunctionCallCount();
	for (int i=0; i<numFunc; ++i)
	{
		const ZFSMFunctionCall* pFunc = m_pCurrState->GetFunctionCall(i);
		ExecuteFunction(fDelta, pFunc->id, pFunc->nArg[0], pFunc->nArg[1], pFunc->nArg[2], pFunc->nArg[3]);
	}
}

void ZActorWithFSM::ProcessShotInAction(float fDelta)
{
	if (!m_pCurrAction) return;

	float fCurrTime = ZGetGame()->GetTime();

	const ZActorActionMeleeShot* pMeleeShot = m_pCurrAction->GetMeleeShot();
	if (pMeleeShot)
	{
		if (fCurrTime >= m_pCurrActionStartTime + pMeleeShot->GetDelay() &&
			!IsProcessedShot(pMeleeShot))
		{
			pMeleeShot->ProcessShot(m_pGame, m_UID, fCurrTime);
			SetProcessedShot(pMeleeShot);
		}
	}

	int numRangeShot = m_pCurrAction->GetNumRangeShot();
	for (int i=0; i<numRangeShot; ++i)
	{
		const ZActorActionRangeShot* pRangeShot = m_pCurrAction->GetRangeShot(i);
		if (pRangeShot)
		{
			if (fCurrTime >= m_pCurrActionStartTime + pRangeShot->GetDelay() &&
				!IsProcessedShot(pRangeShot))
			{
				rvector vDir = GetDirection() + pRangeShot->GetDirectionMod();
				Normalize(vDir);

				rvector vPos;
				_RMeshPartsPosInfoType posPartsType = pRangeShot->GetPosPartsType();
				if (posPartsType == eq_parts_pos_info_end)
					vPos = GetPosition();
				else
					vPos = m_pVMesh->GetBipTypePosition(posPartsType);

				pRangeShot->ProcessShot(vPos, vDir, this);
				SetProcessedShot(pRangeShot);
			}
		}
	}
}

void ZActorWithFSM::ValidateTarget()
{
	// 없어지거나 죽은 경우 타겟을 해제한다
	ZCharacter* pChar = (ZCharacter*) m_pGame->GetCharacterMgr()->Find(m_uidTarget);
	if (!pChar || pChar->IsDie())
	{
		m_uidTarget.SetInvalid();
		return;
	}
}

void ZActorWithFSM::PostBasicInfo()
{
	DWORD nNowTime = timeGetTime();
	if (GetInitialized() == false) return;

	// 죽고나서 5초가 지나면 basicinfo를 보내지 않는다.
	if(IsDie() && m_pGame->GetTime() - GetDeadTime()>5.f) return;
	int nMoveTick = m_pGame->GetCharacterBasicInfoTick();
	nMoveTick = 1; //todok del

	if ((int)(nNowTime - m_nLastTimePostBasicInfo) >= nMoveTick)
	{
		m_nLastTimePostBasicInfo = nNowTime;

		ZACTOR_WITHFSM_BASICINFO pbi;
		//pbi.fTime = m_pGame->GetTime();//todok del
		pbi.uidNPC = GetUID();

		pbi.posx = GetPosition().x;
		pbi.posy = GetPosition().y;
		pbi.posz = GetPosition().z;
		/*pbi.posx = (int(GetPosition().x * 0.1f)) * 10.f;
		pbi.posy = (int(GetPosition().y * 0.1f)) * 10.f;
		pbi.posz = (int(GetPosition().z * 0.1f)) * 10.f;*/	// 오차로 인한 버그재현을 쉽게 하기 위해 1의 자리까지 절사한 버전

		pbi.velx = GetVelocity().x;
		pbi.vely = GetVelocity().y;
		pbi.velz = GetVelocity().z;

		pbi.dirx = GetDirection().x*32000.0f;
		pbi.diry = GetDirection().y*32000.0f;
		pbi.dirz = GetDirection().z*32000.0f;

		pbi.nActionIndex = -1;
		if (m_pCurrAction)
			pbi.nActionIndex = m_pActionMgr->GetIndexOfAction(m_pCurrAction);


		ZPOSTCMD1(MC_NEWQUEST_PEER_NPC_BASICINFO, MCommandParameterBlob(&pbi,sizeof(ZACTOR_WITHFSM_BASICINFO)));


		// history 에 보관
#define ACTOR_HISTROY_COUNT 100
		ZBasicInfoItem *pitem=new ZBasicInfoItem;
		pitem->info.position = GetPosition();
		pitem->info.direction = GetDirection();
		pitem->info.velocity = GetVelocity();

		pitem->fSendTime = pitem->fReceivedTime = m_pGame->GetTime();
		m_BasicHistory.push_back(pitem);

		while(m_BasicHistory.size()>ACTOR_HISTROY_COUNT)
		{
			delete *m_BasicHistory.begin();
			m_BasicHistory.pop_front();
		}
	}
}

void ZActorWithFSM::OnBasicInfo(ZACTOR_WITHFSM_BASICINFO* pbi)
{
	if (!IsMyControl())
	{
		SetPosition(rvector(pbi->posx,pbi->posy,pbi->posz));
		SetVelocity(rvector(pbi->velx,pbi->vely,pbi->velz));
		SetDirection(1.f/32000.f * rvector(pbi->dirx,pbi->diry,pbi->dirz));

		IActorAction* pAction = m_pActionMgr->GetActionByIndex(pbi->nActionIndex);
		m_pCurrAction = pAction;
		SetAnimationInAction(pAction);
	}
}

void ZActorWithFSM::OnExitState()
{
	// 탈출 func 실행
	int numExitFunc = m_pCurrState->GetExitFunctionCallCount();
	for (int i=0; i<numExitFunc; ++i)
	{
		const ZFSMFunctionCall* pFunc = m_pCurrState->GetExitFunctionCall(i);
		ExecuteFunction(0, pFunc->id, pFunc->nArg[0], pFunc->nArg[1], pFunc->nArg[2], pFunc->nArg[3]);
	}

	// 이번 상태에서 켜둔 이동중 플래그가 있을 수 있으니 끈다
	SetMoving(false);
}

void ZActorWithFSM::OnEnterState()
{
	m_stateEnteredTimeStamper.SetEnteredTime(m_pCurrState, ZTimeGetTime());

	// 진입 func 실행
	int numEnterFunc = m_pCurrState->GetEnterFunctionCallCount();
	for (int i=0; i<numEnterFunc; ++i)
	{
		const ZFSMFunctionCall* pFunc = m_pCurrState->GetEnterFunctionCall(i);
		ExecuteFunction(0, pFunc->id, pFunc->nArg[0], pFunc->nArg[1], pFunc->nArg[2], pFunc->nArg[3]);
	}

	m_bPostedActionExecute = false;
	m_bPostedRangeShotInAction = false;
	m_RealPositionBefore = rvector(0,0,0);

	// 액션 시작을 알려서 각 peer가 이 액션에 포함된 공격을 처리하도록 한다
	int nActionIndex = -1;
	IActorAction* pAction = m_pCurrState->GetAction();
	if (pAction)
		nActionIndex = m_pActionMgr->GetIndexOfAction(pAction);
	ZPostNewQuestPeerNpcActionExecute(m_UID, nActionIndex);
}

void ZActorWithFSM::RotateToDirection(rvector vTargetDir, float fFrameDelta)
{
	float fRotSpeed = 1.0f;
	if (m_pActorDef)
		fRotSpeed = m_pActorDef->GetRotSpeed();

	rmatrix mat;
	rvector vMyDir = GetDirection();
	float fAngle=GetAngleOfVectors(vTargetDir, vMyDir);
	float fRotAngle = fFrameDelta * fRotSpeed;

	if (fAngle > 0.0f) fRotAngle = -fRotAngle;
	if (fabs(fRotAngle) > fabs(fAngle)) 
	{
		fRotAngle = -fAngle;
	}
	D3DXMatrixRotationZ(&mat, fRotAngle);

	SetDirection(vMyDir * mat);
}



void ZActorWithFSM::AddLandingEffect( rvector vPos, rvector vDir )
{
	ZGetEffectManager()->AddLandingEffect(vPos,vDir);
}

ZOBJECTHITTEST ZActorWithFSM::HitTest( const rvector& origin, const rvector& to, float fTime, rvector *pOutPos )
{
	//todok 보스의 경우 메쉬피킹하도록 해야 한다. def xml 에 메쉬피킹 여부를 지정할 수 있도록 하기
	return HitTest_Actor(false, origin, to, fTime, pOutPos);
}

void ZActorWithFSM::OnBlast(rvector &dir)
{
	if (!IsMyControl()) return;
	if (!m_pActorDef) { _ASSERT(0); return; }
	if (m_pActorDef->IsNeverBlasted()) return;
	
	if (!m_pFsm) return;
	ZFSMState* pStateBlastedSoar = m_pFsm->GetState(FSM_BISTATE_BLASTEDSOAR);
	if (!pStateBlastedSoar) { _ASSERT(0); return; }

	m_blastedDir = dir;

	OnExitState();
	m_pCurrState = pStateBlastedSoar;
	OnEnterState();
}

void ZActorWithFSM::OnBlastDagger(rvector &dir,rvector& pos)
{
	if (!IsMyControl()) return;
	if (!m_pActorDef) { _ASSERT(0); return; }
	if (m_pActorDef->IsNeverBlasted()) return;

	if (!m_pFsm) return;
	ZFSMState* pStateBlastedThrust = m_pFsm->GetState(FSM_BISTATE_BLASTEDTHRUST);
	if (!pStateBlastedThrust) { _ASSERT(0); return; }

	m_daggerBlastedDir = dir;
	m_daggerBlastedPos = pos;

	OnExitState();
	m_pCurrState = pStateBlastedThrust;
	OnEnterState();
}

void ZActorWithFSM::OnPeerMeleeShot(const char* szActionName)
{
	//todok del
	if (!m_pActionMgr) { _ASSERT(0); return; }
	
	IActorAction* pAction = m_pActionMgr->GetAction(szActionName);
	if (!pAction) { _ASSERT(0); return; }

	const ZActorActionMeleeShot* pMeleeShot = pAction->GetMeleeShot();
	if (!pMeleeShot) { _ASSERT(0); return; }

	pMeleeShot->ProcessShot(m_pGame, m_UID, ZGetGame()->GetTime());
}

void ZActorWithFSM::OnPeerActionExecute(int nActionIndex)
{
	m_pCurrAction = NULL;

	if (!m_pActionMgr) { _ASSERT(0); return; }

	IActorAction* pAction = m_pActionMgr->GetActionByIndex(nActionIndex);
	if (!pAction) { return; }

	m_pCurrAction = pAction;
	m_pCurrActionStartTime = ZGetGame()->GetTime();
	m_setProcessedShotInAction.clear();
}

void ZActorWithFSM::OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType)
{
	if (IsMyControl())
	{
		// 라이트닝 강베기인 경우
		if (damageType==ZD_KATANA_SPLASH)
		{
			ZCharacterObject* pCObj = MDynamicCast(ZCharacterObject, pAttacker);
			if (pCObj)
			{
				ZC_ENCHANT etype = pCObj->GetEnchantType();
				if( etype == ZC_ENCHANT_LIGHTNING )
				{
					OnDamaged_LightningSplash();
				}
			}
		}

		// 그로기미터 증가
		m_fGroggy += fDamage;
		//todok del
		//mlog("ㅈACTOR_GROGGY : %f\n", m_fGroggy);
	}

	ZObject::OnDamaged(pAttacker,srcPos,damageType,weaponType,fDamage,fPiercingRatio,nMeleeType);
}

void ZActorWithFSM::OnDamaged_LightningSplash()
{
	if (!IsMyControl()) return;
	if (!m_pFsm) { _ASSERT(0); return; }
	
	ZFSMState* pStatetLightningStun = m_pFsm->GetState(FSM_BISTATE_LIGHTNINGSTUN);
	if (!pStatetLightningStun) { _ASSERT(0); return; }

	OnExitState();
	m_pCurrState = pStatetLightningStun;
	OnEnterState();

	SetVelocity(0,0,0);
};

//////////////////////////////////////////////////////////////////////////
//		IFSMConditionSource 구현
//////////////////////////////////////////////////////////////////////////

bool ZActorWithFSM::Cond_IsEndAction()
{
	if (m_pVMesh)
	{
		if (m_pVMesh->isOncePlayDone())
			return true;
	}
	return false;
}

bool ZActorWithFSM::Cond_IsLanding()
{
	return m_pModule_Movable->isLanding();
}


bool ZActorWithFSM::Cond_HasTarget()
{
	if (m_uidTarget.IsValid())
		return true;

	return false;
}

void ZActorWithFSM::Cond_GetTargetPos( rvector& out )
{
	out = rvector(0,0,0);

	if (!m_pGame) { _ASSERT(0); return; }
	if (m_uidTarget.IsInvalid()) return;

	ZCharacterObject* pChar = m_pGame->GetCharacterMgr()->Find(m_uidTarget);
	if (!pChar) return;

	out = pChar->GetPosition();
}

bool ZActorWithFSM::Cond_CheckStateCooltime( ZFSMState* pState, DWORD dwCooltime )
{
	if (!m_pFsm) { _ASSERT(0); return false; }

	return m_stateEnteredTimeStamper.CheckCooltime(pState, dwCooltime);
}

DWORD ZActorWithFSM::Cond_GetStateEnteredTime()
{
	if (!m_pFsm) { _ASSERT(0); return false; }
	if (!m_pCurrState) { _ASSERT(0); return false; }

	return m_stateEnteredTimeStamper.GetEnteredTime(m_pCurrState);
}

bool ZActorWithFSM::IsActiveLightningDamageModule()
{
	return IsActiveModule(ZMID_LIGHTNINGDAMAGE);
}

bool ZActorWithFSM::Cond_Pick(float fAngleDegree, float fDist)
{
	rvector pos = GetPosition() + rvector(0,0,100);
	rvector dir = GetDirection();
	dir.z = 0;
	D3DXMATRIX mat;
	D3DXMatrixRotationZ(&mat, ToRadian(fAngleDegree));
	dir = dir*mat;
	Normalize(dir);

	ZPICKINFO pickinfo;
	memset(&pickinfo,0,sizeof(ZPICKINFO));

	const DWORD dwPickPassFlag=RM_FLAG_ADDITIVE | RM_FLAG_HIDE;

	if (m_pGame->Pick(this,pos, dir, &pickinfo, dwPickPassFlag)) 
	{
		rvector pickpos;

		if (pickinfo.bBspPicked)
		{
			pickpos = pickinfo.bpi.PickPos;
		}
		else if (pickinfo.pObject)
		{
			pickpos = pickinfo.pObject->GetPosition();
			pickpos.z += 100;
		}

		float fDistSq = fDist * fDist;
		if (MagnitudeSq(pos - pickpos) < fDistSq)
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//		ZFSMFunctionServer 구현
//////////////////////////////////////////////////////////////////////////

void ZActorWithFSM::Func_FindTarget()
{
	if (!m_pGame) { _ASSERT(0); return; }

	MUID uidTarget	= MUID(0,0);
	float fDist		= FLT_MAX;

	ZCharacterManagerBase* pCharMgr = m_pGame->GetCharacterMgr();
	ZCharacterManagerBase::iterator it;
	ZCharacter* pChar;
	for (it=pCharMgr->begin(); it!=pCharMgr->end(); ++it)
	{
		pChar = (ZCharacter*) it->second;
		if ( pChar->IsDie())
			continue;

		// 거리를 구한다.
		float dist = MagnitudeSq( pChar->GetPosition() - this->GetPosition());

		// 더 가까운 놈이면 이놈을 타겟으로 정한다.
		if ( dist < fDist)
		{
			fDist = dist;
			uidTarget = pChar->GetUID();
		}
	}

	m_uidTarget = uidTarget;
}

void ZActorWithFSM::Func_SetTargetLastAttacker()
{
	m_uidTarget = m_pModule_HPAP->GetLastAttacker();
}

void ZActorWithFSM::Func_RotateToTarget(float fFrameDelta)
{
	if (!m_pGame) { _ASSERT(0); return; }

	if (m_uidTarget.IsInvalid()) return;

	ZCharacterObject* pChar = m_pGame->GetCharacterMgr()->Find(m_uidTarget);
	if (!pChar) return;

	// 이번 프레임에 회전할 만큼만 타겟을 향해 회전
	rvector targetDir(pChar->GetPosition() - GetPosition());
	Normalize(targetDir);

	RotateToDirection(targetDir, fFrameDelta);
}

void ZActorWithFSM::Func_FaceToTarget()
{
	if (!m_pGame) { _ASSERT(0); return; }

	if (m_uidTarget.IsInvalid()) return;

	ZCharacterObject* pChar = m_pGame->GetCharacterMgr()->Find(m_uidTarget);
	if (!pChar) return;

	// 이번 프레임에 회전할 만큼만 타겟을 향해 회전
	rvector targetDir(pChar->GetPosition() - GetPosition());
	targetDir.z = 0;
	Normalize(targetDir);

	SetDirection(targetDir);
}

void ZActorWithFSM::Func_ReduceGroggy(float f)
{
	m_fGroggy -= f;

	//todok del
	//mlog("ㄷACTOR_GROGGY : %f\n", m_fGroggy);
}

void ZActorWithFSM::Func_BuildWaypointsToTarget()
{
	if (!m_pGame) { _ASSERT(0); return; }

	if (m_uidTarget.IsInvalid()) return;

	ZCharacterObject* pTarget = m_pGame->GetCharacterMgr()->Find(m_uidTarget);
	if (!pTarget) return;

	// Make path
	ZNavigationMesh navMesh = m_pGame->GetNavigationMesh();
	if ( navMesh.IsNull())
		return;

	// Make navigation path
	rvector tarpos = pTarget->GetPosition();
	if ( !navMesh.BuildNavigationPath( GetPosition(), tarpos))
		return;

	m_listWaypoint.clear();
	for (list<rvector>::iterator it=navMesh.GetWaypointList().begin(); it!=navMesh.GetWaypointList().end(); ++it)
		m_listWaypoint.push_back(*it);

	//AdjustWayPointWithBound(m_WayPointList, navMesh);
}

void ZActorWithFSM::Func_RunWaypoints(float fFrameDelta)
{
	if (m_listWaypoint.empty()) return;
	if (!m_pActorDef) return;

	rvector nextWaypoint = *m_listWaypoint.begin();
	rvector myPos = GetPosition();

	nextWaypoint.z = 0;
	myPos.z = 0;

	float diffSq = MagnitudeSq(nextWaypoint - myPos);
	if (diffSq <= 20.f*20.f)
	{
		m_listWaypoint.pop_front();
		this->Func_RunWaypoints(fFrameDelta);	// 웨이포인트에 도달했으면 재귀호출로 다음 웨이포인트에 대한 처리를 하게 한다
		return;
	}

	//rvector dir = nextWaypoint - myPos;
	//Normalize(dir);
	//SetDirection(dir);

	rvector vWaypointDir = nextWaypoint - myPos;
	Normalize(vWaypointDir);

	// 다음 웨이포인트를 향해 방향 전환
	RotateToDirection(vWaypointDir, fFrameDelta);

	// 웨이포인트 향해 이동
	//float fSpeed = m_pActorDef->GetSpeed();
	//rvector newPos = vWaypointDir * (fSpeed * fFrameDelta) + myPos;
	//SetPosition(newPos);
	SetMoving(true);
}

void ZActorWithFSM::Func_Disappear(float fFrameDelta, DWORD nDelay, float fRatio)
{
	if (!m_pVMesh) { _ASSERT(0); return; }
	if (!m_pCurrState) { _ASSERT(0); return; }

	DWORD elapsed = ZTimeGetTime() - Cond_GetStateEnteredTime();

	// 딜레이만큼 기다렸다가 투명화 조작
	if (elapsed >= nDelay)
	{
		elapsed -= nDelay;

		float fOpacity = 1.f - fRatio * (elapsed*0.001f);

		if (fOpacity > 1.f) fOpacity = 1.f;
		if (fOpacity < 0) fOpacity = 0;

		m_pVMesh->SetVisibility(fOpacity);
	}
}

void ZActorWithFSM::Func_PostDead()
{
	if (IsMyControl())
		ZPostNewQuestRequestNPCDead(m_pModule_HPAP->GetLastAttacker(), GetUID(), GetPosition());
	else
		_ASSERT(0);
}

void ZActorWithFSM::Func_OnEnterBlastedSoar()
{
	rvector dir = GetDirection();
	dir.x = -m_blastedDir.x;
	dir.y = -m_blastedDir.y;
	SetDirection(dir);

	SetVelocity(m_blastedDir * 300.f + rvector(0,0, (float)RandomNumber( 1500, 1900)));

	//m_bBlasting = true;
	//SetFlag(AF_LAND, false); //todok land 플래그 만들어지면 처리하자
}

void ZActorWithFSM::Func_OnEnterBlastedThrust()
{
	rvector dir = GetDirection();
	dir.x = -m_daggerBlastedDir.x;
	dir.y = -m_daggerBlastedDir.y;
	SetDirection(dir);

	Normalize(m_daggerBlastedDir);
	AddVelocity(m_daggerBlastedDir * (RUN_SPEED * 4));

	m_bSuffering = true;

	/*rvector dir = GetDirection();
	dir.x = -m_daggerBlastedDir.x;
	dir.y = -m_daggerBlastedDir.y;
	SetDirection(dir);

	SetVelocity(m_daggerBlastedDir * 300.f + rvector(0,0,100.f));

	m_vAddBlastVel = GetPosition() - m_daggerBlastedPos;
	m_vAddBlastVel.z = 0.f;

	Normalize(m_vAddBlastVel);

	m_fAddBlastVelTime = m_pGame->GetTime();

	m_bSuffering = true;
	//SetFlag(AF_LAND, false); //todok land 플래그 만들어지면 처리하자*/
}

float accum_fBlastedThrustVelSubtract = 0;

void ZActorWithFSM::Func_OnUpdateBlastedThrust(float fFrameDelta)
{
	_ASSERT(IsSuffering());

	// 시간의 흐름에 따라 속도에서 x, y 성분을 감속시킨다
	rvector dir=rvector(GetVelocity().x,GetVelocity().y,0);
	float fSpeed = Magnitude(dir);
	Normalize(dir);

	const float fBlastThrustBreak = 4000.f;

	fSpeed -= fBlastThrustBreak * fFrameDelta;
	if (fSpeed < 0)
		fSpeed = 0;

	SetVelocity(dir.x*fSpeed, dir.y*fSpeed, GetVelocity().z);

	//// 시간의 흐름에 따라 속도에서 x, y 성분을 감속시킨다
	//rvector vel = GetVelocity();
	//vel.x *= 0.9;
	//vel.y *= 0.9;
	//SetVelocity(vel);

	/*float subtractRatio = 0.9f * fFrameDelta;
	accum_fBlastedThrustVelSubtract += subtractRatio;
	if (accum_fBlastedThrustVelSubtract > 0.1f)
	{
		vel.x -= vel.x * subtractRatio;
		vel.y -= vel.y * subtractRatio;

		SetVelocity(vel);

		accum_fBlastedThrustVelSubtract = 0;
	}*/
	
	//vel.x -= 500.f * fFrameDelta;
	//vel.y -= 500.f * fFrameDelta;

	//todok del
	if (IsSuffering())
	{
		mlog("* suffering vel setted :%f %f %f\n", GetVelocity().x, GetVelocity().y, GetVelocity().z);
	}

/*
#define BLAST_DAGGER_MAX_TIME 0.8f

	float fTime = max((1.f - (m_pGame->GetTime() - m_fAddBlastVelTime) / BLAST_DAGGER_MAX_TIME),0.0f);

	if( fTime < 0.4f )
		fTime = 0.f;

	float fPower = 400.f * fTime * fTime * fFrameDelta * 80.f;

	//todok 아래 주석이 필요할런지? 확인이 필요
//	if(fPower==0.f)
//		SetFlag(AF_BLAST_DAGGER,false);//힘이 다된것

	rvector vel = m_vAddBlastVel * fPower;

	SetVelocity(vel);*/
}

void ZActorWithFSM::Func_ExitSuffering()
{
	m_bSuffering = false;
}

void ZActorWithFSM::Func_OnEnterDie()
{
	SetDeadTime(m_pGame->GetTime());
}