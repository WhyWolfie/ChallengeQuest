#include "stdafx.h"
#include "MNewQuestScenario.h"

bool MNewQuestSector::AddSpawn( MNewQuestSpawn* pSpawn )
{
	if (!pSpawn) { _ASSERT(0); return false; }

	// �̹� �����ϴ� �̸��̸� ����
	ItorSpawn it = m_mapSpawn.find(pSpawn->GetPosTag());
	if (it != m_mapSpawn.end())
		return false;

	m_mapSpawn[pSpawn->GetPosTag()] = pSpawn;
	return true;
}

MNewQuestSpawn* MNewQuestSector::GetSpawnByIndex(int index)
{
	if (index < 0 || (int)m_mapSpawn.size() <= index) { _ASSERT(0); return NULL; }

	ItorSpawn it = m_mapSpawn.begin();
	for (int i=0; i<index; ++i)
		++it;

	return it->second;
}


MNewQuestScenarioManager::MNewQuestScenarioManager()
: m_pDropTable(0)
{

}

MNewQuestScenarioManager::~MNewQuestScenarioManager()
{
	Clear();
}

void MNewQuestScenarioManager::Clear()
{
	for (ItorScenario it=m_mapScenario.begin(); it!=m_mapScenario.end(); ++it)
		delete it->second;
	m_mapScenario.clear();
}

bool MNewQuestScenarioManager::AddScenario(MNewQuestScenario* pScenario)
{
	if (!pScenario) { _ASSERT(0); return false; }

	// �̹� �����ϴ� �̸��̸� ����
	if (GetScenario(pScenario->GetName()))
		return false;

	m_mapScenario[pScenario->GetName()] = pScenario;
	return true;
}

MNewQuestScenario* MNewQuestScenarioManager::GetScenario(const char* szName)
{
	ItorScenario it = m_mapScenario.find(szName);
	if (it != m_mapScenario.end())
		return it->second;
	return NULL;
}

void MNewQuestScenarioManager::GetScenarioNameList( vector<string>& out )
{
	out.clear();
	for (ItorScenario it=m_mapScenario.begin(); it!=m_mapScenario.end(); ++it)
		out.push_back(it->second->GetName());
}

const char* MNewQuestScenarioManager::GetDefaultScenarioName()
{
	if (m_mapScenario.empty())
	{
		_ASSERT(0);
		return "";
	}

	//todok ���� ���� ������ �ó������� ��ȯ�ϵ��� �����ؾ� �Ѵ�
	return m_mapScenario.begin()->second->GetName();
}

bool MNewQuestScenarioManager::ReadXml(MZFileSystem* pFileSystem, const char* szFileName, MQuestDropTable* pDropTable)
{
	m_pDropTable = pDropTable;

	MXmlDocument xmlIniData;
	xmlIniData.Create();

	char *buffer;
	MZFile mzf;

	if(pFileSystem) {
		if(!mzf.Open(szFileName,pFileSystem))  {
			if(!mzf.Open(szFileName))  {
				xmlIniData.Destroy();
				return false;
			}
		}
	} 
	else  {

		if(!mzf.Open(szFileName)) {

			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;

	mzf.Read(buffer,mzf.GetLength());

	bool bResult = ParseXmlFromStr(buffer);

	delete[] buffer;
	mzf.Close();

	return bResult;
}

bool MNewQuestScenarioManager::ParseXmlFromStr(const char* szXmlContent)
{
	Clear();

	if (!szXmlContent) return NULL;

	MXmlDocument xmlDocument;
	xmlDocument.Create();

	if (!xmlDocument.LoadFromMemory(szXmlContent))
	{
		xmlDocument.Destroy();
		return NULL;
	}

	MXmlElement rootElement, chrElement;
	char szTagName[256];

	rootElement = xmlDocument.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	bool bResult = true;

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, "SCENARIO"))
		{
			MNewQuestScenario* pScenario = ParseScenario(chrElement);
			if (!pScenario)
			{
				bResult = false;
				break;
			}

			if (!AddScenario(pScenario))
			{
				mlog("ERROR: xml scenario node name duplicated.");
				delete pScenario;
				bResult = false;
				break;
			}
		}
	}

	xmlDocument.Destroy();

	// �߰��� �Ľ��� �����ϸ� �о��� ������ �� ������
	if (!bResult)
		Clear();

	return bResult;
}

MNewQuestScenario* MNewQuestScenarioManager::ParseScenario(MXmlElement& elem)
{
	MXmlElement chrElement;
	char szTagName[256];
	char szName[256];
	int rewardItem;

	elem.GetAttribute(szName, "name");
	elem.GetAttribute(&rewardItem, "reward_item", 0);

	// ���� üũ
	if (0 == strcmp(szName, "")) { 
		mlog("ERROR: xml scenario node has no name attr.\n"); _ASSERT(0); return NULL;
	}

	MNewQuestScenario* pScenario = new MNewQuestScenario;
	pScenario->SetName(szName);
	pScenario->SetRewardItem(rewardItem);

	int iCount = elem.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		chrElement = elem.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, "SECTOR"))
		{
			MNewQuestSector* pSector = ParseSector(chrElement);

			if (!pSector)
			{
				delete pScenario;
				return NULL;
			}

			pScenario->AddSector(pSector);
		}
	}

	return pScenario;
}

MNewQuestSector* MNewQuestScenarioManager::ParseSector(MXmlElement& elem)
{
	MXmlElement chrElement;
	char szTagName[256];
	char szMap[256];
	int rewardXp;
	int rewardBp;

	elem.GetAttribute(szMap, "map");
	elem.GetAttribute(&rewardXp, "xp", 0);
	elem.GetAttribute(&rewardBp, "bp", 0);

	// ���� üũ
	if (0 == strcmp(szMap, "")) { 
		mlog("ERROR: xml scenario sector node has no map attr.\n"); _ASSERT(0); return NULL;
	}

	MNewQuestSector* pSector = new MNewQuestSector;
	pSector->SetMap(szMap);
	pSector->SetRewardXp(rewardXp);
	pSector->SetRewardBp(rewardBp);

	int iCount = elem.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		chrElement = elem.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, "SPAWN"))
		{
			MNewQuestSpawn* pSpawn = ParseSpawn(chrElement);

			if (!pSpawn)
			{
				delete pSector;
				return NULL;
			}

			pSector->AddSpawn(pSpawn);
		}
	}

	return pSector;
}

MNewQuestSpawn* MNewQuestScenarioManager::ParseSpawn(MXmlElement& elem)
{
	MXmlElement chrElement;
	char szPosTag[256];
	int num;
	char szActor[256];
	char szDropTable[256];

	elem.GetAttribute(szPosTag, "postag");
	elem.GetAttribute(&num, "num", 0);
	elem.GetAttribute(szActor, "actor");
	elem.GetAttribute(szDropTable, "drop");

	// ���� üũ
	if (0 == strcmp(szPosTag, "")) { 
		mlog("ERROR: xml scenario sector spawn node has no postag attr.\n"); _ASSERT(0); return NULL;
	}
	if (0 >= num) {
		mlog("ERROR: xml scenario sector spawn node has invalid num attr.\n"); _ASSERT(0); return NULL;
	}
	if (0 == strcmp(szActor, "")) { 
		mlog("ERROR: xml scenario sector spawn node has no actor attr.\n"); _ASSERT(0); return NULL;
	}

	MNewQuestSpawn* pSpawn = new MNewQuestSpawn;
	pSpawn->SetPosTag(szPosTag);
	pSpawn->SetNum(num);
	pSpawn->SetActor(szActor);
	int dropTableId = -1;
	if (m_pDropTable)
		dropTableId = m_pDropTable->GetIdByName(szDropTable);
	pSpawn->SetDropTableId(dropTableId);

	return pSpawn;
}