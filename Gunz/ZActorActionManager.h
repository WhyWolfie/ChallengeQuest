#pragma once

class IActorAction;
class ZActorAction;
class ZActorActionShotCommon;
class ZActorActionMeleeShot;
class ZActorActionRangeShot;


class ZActorActionManager
{
	typedef map<string, ZActorAction*>	MapAction;
	typedef MapAction::iterator			ItorAction;
	MapAction m_mapAction;

public:
	virtual ~ZActorActionManager();

	bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName);
	bool ParseXmlFromStr(const char* szXmlContent);
	bool ParseXml_Action(ZActorAction* pOutAction, MXmlElement& elem);
	bool ParseXml_ActionShotCommonAttributes(ZActorActionShotCommon* pOutShot, MXmlElement& elem);
	bool ParseXml_ActionMeleeShot(ZActorActionMeleeShot* pOutMeleeShot, MXmlElement& elem);
	bool ParseXml_ActionRangeShot(ZActorActionRangeShot* pOutRangeShot, MXmlElement& elem);

	bool AddAction(ZActorAction* pAction);
	int GetCount() { return (int)m_mapAction.size(); }
	void Clear();

	virtual ZActorAction* GetAction(const char* szName);

	int GetIndexOfAction(IActorAction* pAction);		// �Ŵ��� ������ �׼��� ��ȣ�� ��´�
	ZActorAction* GetActionByIndex(int idx);			// �׼� ��ȣ�� �׼��� ã�´�
};