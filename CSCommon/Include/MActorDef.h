#pragma once

// �ű� ����Ʈ�� FSM�� ����� npc�� ���� ����
class MActorDef
{
	string m_strName;
	string m_strFsm;
	string m_strModel;

	int m_nMaxHp;
	int m_nMaxAp;

	float m_fCollRadius;
	float m_fCollHeight;

	float m_fSpeed;		// �ʴ� �̵��Ÿ�
	float m_fRotSpeed;	// �ʴ� ȸ���ӵ�(radian)
	float m_fGroggyRecoverySpeed;	// �ʴ� �׷α� ȸ����

	bool m_bNeverBlasted;		// ���� �鿪

public:
	void SetName(const char* sz) { m_strName = sz; }
	void SetFsmName(const char* sz) { m_strFsm = sz; }
	void SetModelName(const char* sz) { m_strModel = sz; }
	const char* GetName() { return m_strName.c_str(); }
	const char* GetFsmName() { return m_strFsm.c_str();}
	const char* GetModelName() { return m_strModel.c_str(); }

	void SetMaxHp(int n) { m_nMaxHp = n; }
	void SetMaxAp(int n) { m_nMaxAp = n; }
	int GetMaxHp() { return m_nMaxHp; }
	int GetMaxAp() { return m_nMaxAp; }

	void SetCollisionRadius(float f) { m_fCollRadius = f; }
	void SetCollisionHeight(float f) { m_fCollHeight = f; }
	float GetCollisionRadius() { return m_fCollRadius; }
	float GetCollisionHeight() { return m_fCollHeight; }

	void SetSpeed(float f) { m_fSpeed = f; }
	void SetRotSpeed(float f) { m_fRotSpeed = f; }
	float GetSpeed() { return m_fSpeed; }
	float GetRotSpeed() { return m_fRotSpeed; }

	void SetGroggyRecoverySpeed(float f) { m_fGroggyRecoverySpeed = f; }
	float GetGroggyRecoverySpeed() { return m_fGroggyRecoverySpeed; }

	void SetNeverBlasted(bool b) { m_bNeverBlasted=b; }
	bool IsNeverBlasted() { return m_bNeverBlasted; }
};


class MActorDefManager
{
	typedef map<string, MActorDef*>		MapActorDef;
	typedef MapActorDef::iterator		ItorActorDef;
	MapActorDef m_mapActorDef;

public:
	~MActorDefManager();

	bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName);
	bool ParseXmlFromStr(const char* szXmlContent);
	int GetCount() { return (int)m_mapActorDef.size(); }
	void Clear();
	bool AddActorDef(MActorDef* pDef);

	MActorDef* GetDef(const char* szActorName);
};