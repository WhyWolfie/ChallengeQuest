#pragma once


class ZActorActionMeleeShot;
class ZActorActionRangeShot;


class IActorAction
{
public:
	virtual ~IActorAction() {}

	virtual const char* GetName() = 0;
	virtual const char* GetAnimationName() = 0;
	virtual bool IsMovingAnimation() = 0;
	virtual const ZActorActionMeleeShot* GetMeleeShot() = 0;
	virtual const ZActorActionRangeShot* GetRangeShot(int idx) = 0;
	virtual int GetNumRangeShot() = 0;
};

// 액션은 기존에 npc의 특수행동을 정의하기 위해 사용했던 ZSkill과 비슷하다.
// 그러나 액션은 꼭 공격스킬일 필요는 없으며, 어떤 애니메이션을 단순히 재생하는 용도로 사용될 수도 있다.
// 공격스킬일 때도 ZSkill과는 다르게 공격판정을 액션 내에 직접 갖고있지 않으며 공격판정을 가진 객체를 
// 월드에 생성하기 위한 정보만을 갖게 된다.


class ZActorAction : public IActorAction
{
	string m_strName;
	string m_strAnimation;
	bool m_bMovingAnimation;

	ZActorActionMeleeShot* m_pMeleeShot;

	typedef vector<ZActorActionRangeShot*>	VecRangeShot;
	typedef VecRangeShot::iterator			ItorRangeShot;
	VecRangeShot m_vecRangeShot;

public:
	ZActorAction() 
		: m_pMeleeShot(0)
		, m_bMovingAnimation(false) {}
	virtual ~ZActorAction();

	void SetName(const char* sz) { m_strName=sz; }
	virtual const char* GetName() { return m_strName.c_str(); }

	void SetAnimationName(const char* sz) { m_strAnimation=sz; }
	virtual const char* GetAnimationName() { return m_strAnimation.c_str(); }

	void SetMovingAnimation(bool b) { m_bMovingAnimation = b; }
	virtual bool IsMovingAnimation() { return m_bMovingAnimation; }

	void SetMeleeShot(ZActorActionMeleeShot* p) { m_pMeleeShot = p; }
	virtual const ZActorActionMeleeShot* GetMeleeShot() { return m_pMeleeShot; }

	void AddRangeShot(ZActorActionRangeShot* p) { m_vecRangeShot.push_back(p); }
	virtual const ZActorActionRangeShot* GetRangeShot(int idx) { 
		if (idx < 0 || GetNumRangeShot() <= idx) { _ASSERT(0); return NULL; }
		return m_vecRangeShot[idx];
	}

	virtual int GetNumRangeShot() { return (int)m_vecRangeShot.size(); }
};

// 밀리샷과 레인지샷의 공통 속성 
class ZActorActionShotCommon
{
protected:
	float m_fDelay;		// 액션이 시작한 뒤 이 딜레이만큼 기다렸다가 샷이 발생
	float m_fDamage;
	float m_fPierce;	// 관통률
	string m_strSound;	// 히트 사운드(지정되지 않은 경우 기본 사운드 발생)
	float m_fKnockbackForce;	// 히트시 넉백 힘

public:
	ZActorActionShotCommon()
		: m_fDelay(0)
		, m_fDamage(0)
		, m_fPierce(0.5f)
		, m_fKnockbackForce(0) {}
	virtual ~ZActorActionShotCommon() {}

	float GetDelay() const { return m_fDelay; }
	float GetDamage() const { return m_fDamage; }
	float GetPierce() const { return m_fPierce; }
	const char* GetSound() const { return m_strSound.c_str(); }
	float GetKnockbackForce() const { return m_fKnockbackForce; }

	void SetDelay(float f) { m_fDelay = f; }
	void SetDamage(float f) { m_fDamage = f; }
	void SetPierce(float f) { m_fPierce= f; }
	void SetSound(const char* sz) { m_strSound = sz; }
	void SetKnockbackForce(float f) { m_fKnockbackForce = f; }
};

class ZActorActionMeleeShot : public ZActorActionShotCommon
{
	float m_fRange;		// 판정 최대 거리
	float m_fAngle;		// 판정 반경 radian

public:
	ZActorActionMeleeShot()
		: m_fRange(0)
		, m_fAngle(0) {}
	
	float GetRange() const { return m_fRange; }
	float GetAngle() const { return m_fAngle; }
	
	void SetRange(float f) { m_fRange = f; }
	void SetAngle(float f) { m_fAngle = f; }

	void ProcessShot(IGame* pGame, const MUID& uidOwner, float fShotTime) const;
};

class ZActorActionRangeShot : public ZActorActionShotCommon
{
	string m_strMeshName;
	_RMeshPartsPosInfoType m_posPartsType;	// 탄환의 발사위치
	float m_fSpeed;
	float m_fCollRadius;	//todok 이것은 폭발형(스플래쉬) 일때만 사용한다. 나중에 정리 필요
	rvector m_vDirectionMod;	// 자신이 보는 방향 기준으로 발사 각도 보정량

public:
	ZActorActionRangeShot()
		: m_fSpeed(100.f)
		, m_fCollRadius(100.f)
		, m_vDirectionMod(0,0,0)
		{}

	const char* GetMeshName() const  { return m_strMeshName.c_str(); }
	_RMeshPartsPosInfoType GetPosPartsType() const { return m_posPartsType; }
	float GetSpeed() const { return m_fSpeed; }
	float GetCollRadius() const  { return m_fCollRadius; }
	const rvector& GetDirectionMod() const { return m_vDirectionMod; }

	void SetMeshName(const char* sz) { m_strMeshName = sz; }
	void SetPosPartsType(_RMeshPartsPosInfoType t) { m_posPartsType = t; }
	void SetSpeed(float f) { m_fSpeed = f; }
	void SetCollRadius(float f) { m_fCollRadius = f; }
	void SetDirectionMod(const rvector& v) { m_vDirectionMod = v; }

	void ProcessShot(const rvector& pos, const rvector& dir, ZObject* pOwner) const;
};