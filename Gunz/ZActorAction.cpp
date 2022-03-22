#include "stdafx.h"
#include "ZActorAction.h"

ZActorAction::~ZActorAction()
{
	delete m_pMeleeShot;
	
	int num = GetNumRangeShot();
	for (int i=0; i<num; ++i)
		delete m_vecRangeShot[i];
}

void ZActorActionMeleeShot::ProcessShot( IGame* pGame, const MUID& uidOwner, float fShotTime ) const
{
	// ����� �������� �� ��� ������ ��� �����߽��ϴ� �ʿ��ϸ� �߰�

	ZObjectManager* pObjectManager = ZGetObjectManager();
	if (!pObjectManager) return;

	// ������ ������ ����
	ZObject *pAttacker = pObjectManager->GetObject(uidOwner);
	if (!pAttacker) return;

	if (m_fRange == 0) return;

	float fRadius = cosf( ToRadian( m_fAngle * 0.5f));

	// �������� ��ġ�� ���⿡ ���� ����ȭ
	rvector AttackerPos		= pAttacker->GetPosition();
	rvector AttackerNorPos	= AttackerPos;
	AttackerNorPos.z		= 0;

	rvector AttackerDir		= pAttacker->m_Direction;
	rvector AttackerNorDir	= AttackerDir;
	AttackerNorDir.z		= 0; 
	Normalize( AttackerNorDir);

	// ���� ���
	bool bPlayer = false;
	rvector Pos = pAttacker->GetPosition();
	ZApplication::GetSoundEngine()->PlaySoundElseDefault("blade_swing","blade_swing",rvector(Pos.x,Pos.y,Pos.z),bPlayer);

	// ������ Ÿ�ٿ� ��Ʈ �ߴ��� üũ�Ѵ�.
	bool bHit = false;

	// Ÿ�Ͽ� ���� �˻縦 �����Ѵ�.
	for ( ZObjectManager::iterator itor = pObjectManager->begin(); itor != pObjectManager->end(); ++itor)
	{
		// Ÿ�ٿ� ���� ������ ��´�.
		ZObject* pVictim = (*itor).second;

		// Ÿ���� ���� ���¸� ���� Ÿ������ �Ѿ��.
		ZModule_HPAP* pModule = (ZModule_HPAP*)pVictim->GetModule(ZMID_HPAP);
		if ( pVictim->IsDie())
			continue;

		// Ÿ���� ������ �ڽ��̸� ���� Ÿ������ �Ѿ��.
		if ( pAttacker == pVictim)
			continue;

		// ���� ������ ��밡 �ƴϸ� ���� Ÿ������ �Ѿ��.
		//if (!CanAttack( pAttacker, pVictim))

		if (pAttacker->GetTeamID() == pVictim->GetTeamID())
			continue;

		// Ÿ���� ������ ���Ѵ�.
		rvector VictimPos, VictimDir;
		if (!pVictim->GetHistory( &VictimPos, &VictimDir, fShotTime))
			continue;

		// �����ڿ� ���� Ÿ���� ��ġ�� ���� ����ȭ
		rvector VictimNorPos	= VictimPos;
		VictimNorPos.z			= 0;

		rvector VictimNorDir	= VictimPos - ( AttackerPos - ( AttackerNorDir * 50.f));
		VictimNorDir.z			= 0;
		Normalize( VictimNorDir);


		// �����ڿ� Ÿ�ٰ��� x,y ��� ���� �Ÿ��� ���� ����, �Ÿ��� ���� ���� �Ÿ����� �ָ� ���� Ÿ������ �Ѿ��.
		float fDist = Magnitude( AttackerNorPos - VictimNorPos);
		if ( fDist > m_fRange)
			continue;

		// Ÿ���� ���� ������ ���� ���̸� ���� Ÿ������ �Ѿ��.
		float fDot = D3DXVec3Dot( &AttackerNorDir, &VictimNorDir);
		if ( fDot < fRadius)
			continue;

		// Ÿ���� �ش� ���� ���� �ȿ��� ���� ���� ���̿� �ִ��� Ȯ���Ѵ�.
		if (!pGame->InRanged(pAttacker, pVictim))
			continue;

		// �����ڿ� Ÿ�� ���̿� ���� ���� ������ ���� Ÿ������ �Ѿ��.
		int nDebugRegister = 0;
		if (pGame->IsWallBlocked( pAttacker, pVictim, nDebugRegister, true))
			continue;

		// ���� �����̸鼭 ���� ������ ������ ���� ����
		if ( pVictim->IsGuard() && ( DotProduct( pVictim->m_Direction, AttackerNorDir) < 0))
		{
			rvector pos = pVictim->GetPosition();
			pos.z += 120.f;

			// ���� ����Ʈ ǥ��
			ZGetEffectManager()->AddSwordDefenceEffect( pos + ( pVictim->m_Direction * 50.f), pVictim->m_Direction);
			pVictim->OnMeleeGuardSuccess();
			return;
		}

		// �밡�� ���¿��� ������ ������... �´� ����Ʈ
		rvector pos = pVictim->GetPosition();
		pos.z += 130.f;
		pos   -= AttackerDir * 50.f;
		// ����Ʈ�� ����Ѵ�.
		ZGetEffectManager()->AddBloodEffect( pos, -VictimNorDir);
		ZGetEffectManager()->AddSlashEffect( pos, -VictimNorDir, 0);


		// ������ ���� ���Ѵ�.
		float fActualDamage = m_fDamage; //CalcActualDamage( pAttacker, pVictim, (float)pItemDesc->m_nDamage.Ref());
		float fPierceRatio = m_fPierce; //pItem->GetPiercingRatio( pItemDesc->m_nWeaponType.Ref() , eq_parts_chest );
		pVictim->OnDamaged( pAttacker, pAttacker->GetPosition(), ZD_MELEE, MWT_DAGGER, fActualDamage, fPierceRatio, 0);

		//todok NPC�� ���ݿ� �˹�Ӽ��� �ִ� ��츦 ó������� �Ѵ�
		/*
		// ���� �޾Ƽ� �ڷ� �и�
		bool bPushSkip = false;

		float fKnockbackForce = pItem->GetKnockbackForce();
		if ( bPushSkip)
		{
			ZGetSoundEngine()->PlaySound( "fx_bullethit_mt_met");
			fKnockbackForce = 1.0f;
		}

		pVictim->OnKnockback( pAttacker->m_Direction, fKnockbackForce);
		*/

		// ����Ʈ ���� ���
		if (m_strSound.empty())
			ZGetSoundEngine()->PlaySoundBladeDamage(NULL, pos);	// �⺻ ��Ҹ�
		else
			ZGetSoundEngine()->PlaySound((char*)m_strSound.c_str(), pos);


		// ������ �¾Ҵ�.
		bHit = true;
	}

	//todok ���� ����ũ Ƣ�� ó�� ����� �Ѵ�
/*
	// ĳ���͸� ������ ���ߴٸ�...
	// test �������� ���� �浹�Ұ�� ����ũ..
	if ( !bHit)
	{

		rvector vPos = pAttacker->GetPosition();
		rvector vDir = AttackerNorDir;

		vPos.z += 130.f;

		RBSPPICKINFO bpi;

		if(pGame->GetWorld()->GetBsp()->Pick(vPos, vDir, &bpi)) {

			float fDist = Magnitude(vPos - bpi.PickPos);

			if (fDist < m_fRange) {

				rplane r = bpi.pInfo->plane;
				rvector vWallDir = rvector( r.a, r.b, r.c );
				Normalize(vWallDir);

				ZGetEffectManager()->AddSlashEffectWall( bpi.PickPos - (vDir*5.f) , vWallDir ,cm);

				rvector pos = bpi.PickPos;

				ZGetSoundEngine()->PlaySoundBladeConcrete(pItemDesc, pos);
			}
		}
	}
*/

	return;
}

void ZActorActionRangeShot::ProcessShot(const rvector& pos, const rvector& dir, ZObject* pOwner) const
{
	//rvector vMissilePos, vMissileDir;
	//GetMissilePosDir( vMissileDir, vMissilePos, targetPos);
	//ZGetGame()->m_WeaponManager.AddNewQuestProjectile(this, pos, dir, pOwner);

	ZGetGame()->m_WeaponManager.AddNewQuestProjectile(this, pos, dir, pOwner);
}
