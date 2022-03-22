#pragma once

#ifdef _TDD		// ��� ���� �׽�Ʈ ���� �ڵ�� _TDD�� ���ε��� �մϴ�
#include <gtest/gtest.h>


class ZActorActionManagerMock : public ZActorActionManager
{
public:
	// �׽�Ʈ������ ��������. ��û���� �׼��� ������ ��¥ �׼��� ���� �ǵ����ش�
	virtual ZActorAction* GetAction(const char* szName)
	{
		// �� �̸��� ������� �ʴ´�
		if (0 == strlen(szName))
			return NULL;

		if (NULL == ZActorActionManager::GetAction(szName))
		{
			ZActorAction* pAction = new ZActorAction;
			pAction->SetName(szName);
			AddAction(pAction);
		}

		return ZActorActionManager::GetAction(szName);
	}
};


#endif