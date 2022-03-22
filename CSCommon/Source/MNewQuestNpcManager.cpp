#include "stdafx.h"
#include "MNewQuestNpcManager.h"


void MNewQuestNpcManager::AddNpcObject(MUID uidNpc, MUID uidController, int nDropItemId)
{
	if (uidNpc.IsInvalid() || uidController.IsInvalid()) { _ASSERT(0); return; }

	MNewQuestNpcObject* pNewNPC = new MNewQuestNpcObject(uidNpc);
	pNewNPC->AssignControl(uidController);
	pNewNPC->SetDropItemId(nDropItemId);
	m_mapNpc[uidNpc] = pNewNPC;
}

void MNewQuestNpcManager::DeleteNpcObject(MUID uidNpc)
{
	if (uidNpc.IsInvalid()) { _ASSERT(0); return; }

	ItorNpc it = m_mapNpc.find(uidNpc);
	if (it == m_mapNpc.end())
		return;

	delete it->second;
	m_mapNpc.erase(it);
}

MNewQuestNpcObject* MNewQuestNpcManager::GetNpc( MUID uid )
{
	if (uid.IsInvalid()) { _ASSERT(0); return NULL; }

	ItorNpc it = m_mapNpc.find(uid);
	if (it == m_mapNpc.end())
		return NULL;

	return it->second;
}
