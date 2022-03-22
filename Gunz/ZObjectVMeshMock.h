#pragma once

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다

#include "ZObjectVMesh.h"


class ZObjectVMeshMock : public ZObjectVMesh
{
	rmatrix m_matIdentity;
	rvector m_vecZero;

public:
	ZObjectVMeshMock()
	{
		D3DXMatrixIdentity(&m_matIdentity);
		m_vecZero = rvector(0,0,0);
	}

	virtual void InitOwnVMesh() {}
	virtual void InitNpcMesh(ZActorBase* pActor, const char* szMeshName) {}
	virtual int InitCharacterMesh(ZCharacter* pCharacter, char* szMeshName) { return 0; }


	virtual bool Create(RMesh* pMesh) { return true; }
	virtual void Destroy() {}

	virtual void SetMesh(RMesh* pMesh) {}
	virtual RMesh* GetMesh() { return NULL; }
	virtual void SetVisualMeshToMesh() {}

	virtual rmatrix GetCurrentWeaponPositionMatrix( bool right=false ) { return m_matIdentity; }
	virtual rvector GetCurrentWeaponPosition( bool right=false ) { return m_vecZero; }
	virtual rvector GetBipTypePosition(RMeshPartsPosInfoType type) { return m_vecZero; }

	virtual rvector GetFootPosition() { return m_vecZero; }

	virtual rvector	GetHeadPosition() { return m_vecZero; }
	virtual rvector	GetRFootPosition() { return m_vecZero; }
	virtual rvector	GetLFootPosition() { return m_vecZero; }
	virtual rvector	GetRootPosition() { return m_vecZero; }

	virtual const D3DXMATRIX& GetWeaponDummyMatrix(WeaponDummyType type) { return m_matIdentity; }
	virtual bool GetWeaponDummyMatrix(WeaponDummyType type,rmatrix* mat,bool bLeft) { *mat = m_matIdentity; return true; }
	virtual bool GetWeaponDummyPos(WeaponDummyType type,rvector* pos,bool bLeft) { *pos = m_vecZero; return true; }

	virtual void SetEnChantType(REnchantType EnchantType) {}
	virtual void GetWeaponPos(rvector* p,bool bLeft=false) { *p=m_vecZero; }
	virtual float GetWeaponSize() { return 10.f; }
	virtual bool IsDoubleWeapon() { return false; }

	virtual int GetLastWeaponTrackPos(rvector* pOutVec) { *pOutVec=m_vecZero; return 0; }
	virtual void SetDrawTracksMotion(int i, bool b) {}
	virtual void SetDrawTracks(bool s) {}


	virtual void AddWeapon(RWeaponMotionType type,RMesh* pVMesh,RAnimation* pAni=NULL) {}
	virtual void RemoveWeapon(RWeaponMotionType type) {}
	virtual void RemoveAllWeapon() {}

	virtual int				GetSetectedWeaponMotionID() { return 0; }
	virtual void			SelectWeaponMotion(RWeaponMotionType type) {}
	virtual RVisualMesh*	GetSelectWeaponVMesh() { return NULL; }
	virtual RWeaponMotionType GetSelectWeaponMotionType() { return eq_weapon_etc; }


	virtual void SetLight(int index,D3DLIGHT9* light,bool ShaderOnly) {}


	virtual AniFrameInfo* GetFrameInfo(RAniMode mode) { return NULL; }

	virtual bool SetAnimation(RAnimation* pAniSet,bool b=false) { return true; }
	virtual bool SetAnimation(char* ani_name,bool b=false)  { return true; }
	virtual bool SetAnimation(RAniMode animode,const char* ani_name,bool b=false) { return true; }
	virtual bool SetAnimation(RAniMode animode,RAnimation* pAniSet,bool b=false) { return true; }

	virtual bool SetReserveAnimation(RAnimation* pAniSet,int tick) { return true; }
	virtual bool SetReserveAnimation(char* ani_name,int tick) { return true; }
	virtual bool SetReserveAnimation(RAniMode animode,RAnimation* pAniSet,int tick) { return true; }
	virtual bool SetReserveAnimation(RAniMode animode,char* ani_name,int tick) { return true; }

	virtual void Play(RAniMode amode = ani_mode_lower) {}
	virtual void Stop(RAniMode amode = ani_mode_lower) {}
	virtual void Pause(RAniMode amode = ani_mode_lower) {}
	virtual bool isOncePlayDone(RAniMode amode = ani_mode_lower) { return false; }
	virtual void SetSpeed(float s,float s_up=4.8f) {}


	virtual const rvector& GetRotXYZ()		{ return m_vecZero; }
	virtual void SetRotXYZ(const rvector& v){}
	virtual void SetRotX(float x)			{}
	virtual void SetRotY(float y)			{}
	virtual void SetRotZ(float z)			{}


	virtual void SetVisibility(float vis) {}
	virtual float GetVisibility() { return 1.f; }


	virtual void OnRestore() {}
	virtual void OnInvalidate() {}


	virtual void Frame() {}
	virtual void Render(bool low = false ,bool render_buffer=false) {}

	virtual RFrameTime* GetFrameTime() { return NULL; }
	virtual int GetMaxFrame(RAniMode amode) { return 0; }

	virtual bool IsRender() { return false;}
	virtual bool IsRenderWeapon() { return true; }


	virtual void SetWorldMatrix(rmatrix& mat) {}
	virtual void SetScale(rvector& v) {}
	virtual const rvector& GetScale() { return m_vecZero; }

	virtual bool Pick(int x,int y,RPickInfo* pInfo) { return false; }
	virtual bool Pick(rvector* vInVec,RPickInfo* pInfo) { return false; }
	virtual bool Pick( const rvector& pos, const rvector& dir, RPickInfo* pInfo ) { return false; }
	virtual bool Pick(rvector& pos,rvector& dir,rvector* v,float* t) { return false; }


	virtual void SetSpRenderMode(ALPHAPASS ePass) {}
	virtual bool UpdateSpWeaponFire() { return true; }


	virtual bool IsSelectWeaponGrenade() { return true; }
	virtual bool IsAddGrenade() { return true; }

	virtual void SetAddGrenade(bool b) {}
	virtual void SetGrenadeFire(bool b) {}
	virtual void SetGrenadeFireTime(DWORD time)	{}


	virtual bool CreateCloth(RMeshNode* pMeshNode,float fAccel,int Numiter ) { return true; }
	virtual void DestroyCloth() {}

	virtual bool ChangeChestCloth(float fAccel,int Numiter ) { return true; }
	virtual bool IsClothModel() { return false; }

	virtual void SetClothValue(bool bGame,float fDist) {}
	virtual bool isChestClothMesh()	{ return true; }
	virtual void UpdateForce(D3DXVECTOR3& force) {}
	virtual void SetClothState(int state) {}
	virtual void SetClothForce(D3DXVECTOR3& f) {}
	virtual void UpdateCloth() {}
	virtual void RenderCloth() {}

	virtual void SetNPCBlendColor(D3DCOLORVALUE color) {}

	virtual void SetCheckViewFrustum(bool b) {}


	virtual void SetParts(RMeshPartsType parts,RMeshNode* pMN) {}
	virtual void SetParts(RMeshPartsType parts,char* name) {}

	virtual void ClearParts() {}

	virtual void SetBaseParts(RMeshPartsType parts) {}

	virtual void SetLowPolyModel(RMesh* pMesh) {}

	virtual void OutputDebugString_CharacterState() {}

	virtual void ShiftFugitiveValues() {}
};

#endif