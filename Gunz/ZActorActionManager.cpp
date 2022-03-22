#include "stdafx.h"
#include "ZActorActionManager.h"
#include "ZActorAction.h"


ZActorActionManager::~ZActorActionManager()
{
	Clear();
}

bool ZActorActionManager::ReadXml(MZFileSystem* pFileSystem, const char* szFileName)
{
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

bool ZActorActionManager::ParseXmlFromStr(const char* szXmlContent)
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

	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlDocument.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	bool bResult = true;

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, "ACTION"))
		{
			ZActorAction* pAction = new ZActorAction;

			if (!ParseXml_Action(pAction, chrElement))
			{
				bResult = false;
				delete pAction;
				break;
			}

			if (!AddAction(pAction))
			{
				mlog("ERROR: xml action node name duplicated.\n");
				delete pAction;
				bResult = false;
				break;
			}
		}
	}

	xmlDocument.Destroy();

	// 중간에 파싱이 실패하면 읽었던 내용을 다 버린다
	if (!bResult)
		Clear();

	return bResult;
}

bool ZActorActionManager::ParseXml_Action(ZActorAction* pOutAction, MXmlElement& elem)
{
	char szTagName[256];
	char szTemp[256];

	elem.GetAttribute(szTemp, "name");
	if (0 == strcmp(szTemp, ""))
	{
		mlog("ERROR: xml action node has no name attr.\n");
		return false;
	}
	pOutAction->SetName(szTemp);

	elem.GetAttribute(szTemp, "animation");
	pOutAction->SetAnimationName(szTemp);

	elem.GetAttribute(szTemp, "movinganimation");
	if (0==stricmp("true", szTemp))
		pOutAction->SetMovingAnimation(true);

	MXmlElement chrElement;
	int iCount = elem.GetChildNodeCount();
	for (int i = 0; i < iCount; i++)
	{
		chrElement = elem.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, "MELEESHOT"))
		{
			ZActorActionMeleeShot* pMeleeShot = new ZActorActionMeleeShot;
			if (!ParseXml_ActionMeleeShot(pMeleeShot, chrElement)) {
				delete pMeleeShot;
				return false;
			}
			pOutAction->SetMeleeShot(pMeleeShot);
		}
		else if (!stricmp(szTagName, "RANGESHOT"))
		{
			ZActorActionRangeShot* pRangeShot = new ZActorActionRangeShot;
			if (!ParseXml_ActionRangeShot(pRangeShot, chrElement)) {
				delete pRangeShot;
				return false;
			}
			pOutAction->AddRangeShot(pRangeShot);
		}
	}
	return true;
}

bool ZActorActionManager::ParseXml_ActionShotCommonAttributes(ZActorActionShotCommon* pOutShot, MXmlElement& elem)
{
	float fDamage;
	int nDelay, nPierce, nKnockback;
	string strSound;

	elem.GetAttribute(&nDelay, "delay", 0);

	if (!elem.GetAttribute(&fDamage, "damage"))
	{
		mlog("ERROR: xml action shot node has no damage attr.\n");
		return false;
	}
	elem.GetAttribute(&nPierce, "pierce", 50);
	if (!(0 <= nPierce && nPierce <= 100))
	{
		mlog("ERROR: xml action shot node has invalid pierce attr.\n");
		return false;
	}
	elem.GetAttribute(&nKnockback, "knockback", 0);
	if (nKnockback < 0)
	{
		mlog("ERROR: xml action shot node has invalid knockback attr.\n");
		return false;
	}

	elem.GetAttribute(&strSound, "sound", "");

	pOutShot->SetDelay(nDelay * 0.001f);
	pOutShot->SetDamage(fDamage);
	pOutShot->SetPierce(nPierce * 0.01f);
	pOutShot->SetKnockbackForce(nKnockback);

	pOutShot->SetSound(strSound.c_str());

	return true;
}

bool ZActorActionManager::ParseXml_ActionMeleeShot(ZActorActionMeleeShot* pOutMeleeShot, MXmlElement& elem)
{
	if (!ParseXml_ActionShotCommonAttributes(pOutMeleeShot, elem))
		return false;

	float fAngle, fRange;
	
	if (!elem.GetAttribute(&fAngle, "angle"))
	{
		mlog("ERROR: xml action melee shot node has no angle attr.\n");
		return false;
	}
	if (!elem.GetAttribute(&fRange, "range"))
	{
		mlog("ERROR: xml action melee shot node has no range attr.\n");
		return false;
	}
	
	pOutMeleeShot->SetRange(fRange);
	pOutMeleeShot->SetAngle(fAngle);
	
	return true;
}

bool ZActorActionManager::ParseXml_ActionRangeShot(ZActorActionRangeShot* pOutRangeShot, MXmlElement& elem)
{
	if (!ParseXml_ActionShotCommonAttributes(pOutRangeShot, elem))
		return false;

	string strMeshName, strDirMod, strPosParts;
	float fSpeed, fCollRadius;
	rvector vDirMod;
	_RMeshPartsPosInfoType posPartsType = eq_parts_pos_info_end;

	elem.GetAttribute(&strMeshName, "mesh", "");

	if (!elem.GetAttribute(&fSpeed, "speed"))
	{
		mlog("ERROR: xml action range shot node has no speed attr.\n");
		return false;
	}
	
	elem.GetAttribute(&fCollRadius, "collradius");
	/*todok del collradius 이제는 선택적으로 할 수 있다
	if (!elem.GetAttribute(&fCollRadius, "collradius"))
	{
		mlog("ERROR: xml action range shot node has no collradius attr.\n");
		return false;
	}*/

	elem.GetAttribute(&strDirMod, "dirmod", "0 0 0");
	if (3 != sscanf(strDirMod.c_str(), "%f %f %f", &vDirMod.x, &vDirMod.y, &vDirMod.z))
	{
		mlog("ERROR: xml action range shot node has invalid dirmod attr.\n");
		return false;
	}

	if (elem.GetAttribute(&strPosParts, "posparts"))
	{
		if (0==stricmp(strPosParts.c_str(), "head"))			posPartsType = eq_parts_pos_info_Head;
		else if (0==stricmp(strPosParts.c_str(), "lhand"))		posPartsType = eq_parts_pos_info_LHand;
		else if (0==stricmp(strPosParts.c_str(), "rhand"))		posPartsType = eq_parts_pos_info_RHand;
		else 
		{
			mlog("ERROR: xml action range shot node has invalid posparts attr.\n");
			return false;
		}
	}

	pOutRangeShot->SetMeshName(strMeshName.c_str());
	pOutRangeShot->SetSpeed(fSpeed);
	pOutRangeShot->SetCollRadius(fCollRadius);
	pOutRangeShot->SetDirectionMod(vDirMod);
	pOutRangeShot->SetPosPartsType(posPartsType);

	return true;
}

bool ZActorActionManager::AddAction(ZActorAction* pAction)
{
	// 중복 이름이면 실패
	ItorAction it = m_mapAction.find(pAction->GetName());
	if (it != m_mapAction.end())
		return false;

	m_mapAction[pAction->GetName()] = pAction;
	return true;
}

void ZActorActionManager::Clear()
{
	for (ItorAction it=m_mapAction.begin(); it!=m_mapAction.end(); ++it)
		delete it->second;
	m_mapAction.clear();
}

ZActorAction* ZActorActionManager::GetAction( const char* szName )
{
	ItorAction it = m_mapAction.find(szName);
	if (it != m_mapAction.end())
		return it->second;
	
	return NULL;
}

int ZActorActionManager::GetIndexOfAction(IActorAction* pAction)
{
	int idx=0;
	for (ItorAction it=m_mapAction.begin(); it!=m_mapAction.end(); ++it)
	{
		if (it->second == pAction)
			return idx;
		++idx;
	}
	return -1;
}

ZActorAction* ZActorActionManager::GetActionByIndex(int idx)
{
	int numAction = (int)m_mapAction.size();
	if (idx < 0 || numAction <= idx)
		return NULL;

	ItorAction it = m_mapAction.begin();
	for (int i=0; i<idx; ++i)
		++it;

	return it->second;
}