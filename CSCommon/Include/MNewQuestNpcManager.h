#pragma once

class MNewQuestNpcObject
{
private:
	MUID m_uid;
	//MQUEST_NPC m_nType;
	MUID m_uidController;
	int m_nDropItemId;

public:
	MNewQuestNpcObject(MUID& uid) : m_uid(uid), m_uidController(0,0), m_nDropItemId(0) {}
	~MNewQuestNpcObject() {}

	MUID GetUID() { return m_uid; }

	// �� npc�� ������ �÷��̾�
	void AssignControl(MUID& uidPlayer) { m_uidController=uidPlayer; }
	void ReleaseControl() { m_uidController.SetInvalid(); }
	MUID& GetController() { return m_uidController; }

	int GetDropItemId()			{ return m_nDropItemId; }
	void SetDropItemId(int id)	{ m_nDropItemId = id; }
	bool HasDropItem()			{ return m_nDropItemId != 0; }
};

class MNewQuestNpcManager
{
	typedef map<MUID, MNewQuestNpcObject*>	MapNpc;
	typedef MapNpc::iterator				ItorNpc;

	MapNpc m_mapNpc;

public:
	void AddNpcObject(MUID uidNpc, MUID uidController, int nDropItemId);
	void DeleteNpcObject(MUID uidNpc);

	int GetNumNpc() { return (int)m_mapNpc.size(); }
	MNewQuestNpcObject* GetNpc(MUID uid);
};