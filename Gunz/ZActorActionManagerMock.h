#pragma once

#ifdef _TDD		// 모든 단위 테스트 관련 코드는 _TDD로 감싸도록 합니다
#include <gtest/gtest.h>


class ZActorActionManagerMock : public ZActorActionManager
{
public:
	// 테스트용으로 재정의함. 요청받은 액션이 없으면 가짜 액션을 만들어서 되돌려준다
	virtual ZActorAction* GetAction(const char* szName)
	{
		// 빈 이름은 허용하지 않는다
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