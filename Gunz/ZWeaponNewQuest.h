#pragma once

class ZActorActionRangeShot;

// 일반적인 탄환 (폭발이나 기타 효과 없음)
class ZWeaponNpcBullet : public ZMovingWeapon
{
	MDeclareRTTI
private:
	float		m_fScale;
	ZObject*	m_pOwner;
	
	const ZActorActionRangeShot* m_pRangeShot;

public:
	ZWeaponNpcBullet() : ZMovingWeapon() { m_fScale = 1.f; }

	void Create(RMesh* pMesh, const ZActorActionRangeShot* pRangeShot, const rvector &pos, const rvector &dir, float fScale, ZObject* pOwner);
	bool Update(float fElapsedTime);
	void Render();

	void Explosion(ZObject* pVictim, const rvector& dir);
	void ExplosionOnMap();

	float	m_fStartTime;
	float	m_fLastAddTime;
};
