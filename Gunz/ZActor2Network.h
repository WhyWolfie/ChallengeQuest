#ifndef _ZACTOR2NETWORK_H
#define _ZACTOR2NETWORK_H

// ZActor2 에서 네트워크 통신 부분을 분리해낸 모듈

class ZActor2;


class ZActor2Network
{
	enum ZACTOR2_LASTTIME
	{
		ACTOR_LASTTIME_HPINFO		= 0,
		ACTOR_LASTTIME_BASICINFO,
		ACTOR_LASTTIME_MAX
	};
	unsigned long int	m_nLastTime[ACTOR_LASTTIME_MAX];

protected:
	virtual void PostBasicInfo(ZActor2* pActor);
	virtual void PostBossHpAp(ZActor2* pActor);

public:
	ZActor2Network();
	virtual ~ZActor2Network();

	virtual void Process(ZActor2* pActor);
};



#endif