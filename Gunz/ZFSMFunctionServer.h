#pragma once
#include "ZFSMGlobal.h"

// FSM���� ���Ϳ��� ��û�� �� �ִ� ����� ����� ������ �ְ� �� ����� ������ ó�����ش�
// FSM�� ����ϴ� ���ʹ� �� �������̽��� �����ؾ� �Ѵ�
class IFSMFunctionServer
{
public:
	virtual bool ExecuteFunction(float fFrameDelta, int id, int nArg1=0, int nArg2=0, int nArg3=0, int nArg4=0) = 0;
};


// �ű�����Ʈ���� ���Ͱ� FSM�� ���� ó������ �� �ִ� ��ǵ��� ����ü
class ZFSMFunctionServer : public IFSMFunctionServer
{
public:
	enum {
		FSMFUNC_INVALID = -1,
		FSMFUNC_DONOTHING,
		FSMFUNC_DICE,

		FSMFUNC_FIND_TARGET,
		FSMFUNC_SET_TARGET_LAST_ATTACKER,	// ���������� �ڽ��� ������ ��븦 Ÿ������ ����

		FSMFUNC_ROTATE_TO_TARGET,			// ���� ���� ������ �ӵ��� ȸ��
		FSMFUNC_FACE_TO_TARGET,				// ���� ���� ��� ȸ��

		FSMFUNC_REDUCE_GROGGY,

		FSMFUNC_BUILD_WAYPOINTS_TO_TARGET,
		FSMFUNC_RUN_WAYPOINTS,

		// built-in state������ ����ϴ� �͵� (���� �ļ��� ����)
		FSMFUNC_DISAPPEAR,					// �ð��� ���� �޽� ��������(�׾����� ���)
		FSMFUNC_POST_DEAD,					// ������ ���� �˸�
		FSMFUNC_ON_ENTER_BLASTED_SOAR,		// ��� ���� �ǰ� ����
		FSMFUNC_ON_ENTER_BLASTED_THRUST,	// �ܰ� ��ġ�� �ǰ� ����
		FSMFUNC_ON_UPDATE_BLASTED_THRUST,	// �ܰ� ��ġ�� �ǰ� ������Ʈ
		FSMFUNC_EXIT_SUFFERING,				// ���ۺҴ� �ǰ� �÷��� ����
		FSMFUNC_ON_ENTER_DIE,
	};
public:
	ZFSMFunctionServer();

	virtual bool ExecuteFunction(float fFrameDelta, int funcId, int nArg1=0, int nArg2=0, int nArg3=0, int nArg4=0);

	int GetDice() { return m_nDice; }

protected:
	virtual void Func_Dice(int nMin, int nMax);
	virtual void Func_FindTarget() { _ASSERT(0); }
	virtual void Func_SetTargetLastAttacker() { _ASSERT(0); }

	virtual void Func_RotateToTarget(float fFrameDelta) { _ASSERT(0); }
	virtual void Func_FaceToTarget() { _ASSERT(0); }

	virtual void Func_ReduceGroggy(float f) { _ASSERT(0); }

	virtual void Func_BuildWaypointsToTarget() { _ASSERT(0); }
	virtual void Func_RunWaypoints(float fFrameDelta) { _ASSERT(0); }
	
	virtual void Func_Disappear(float fFrameDelta, DWORD nDelay, float fRatio) { _ASSERT(0); }
	virtual void Func_PostDead() { _ASSERT(0); }
	virtual void Func_OnEnterBlastedSoar() { _ASSERT(0); }
	virtual void Func_OnEnterBlastedThrust() { _ASSERT(0); }
	virtual void Func_OnUpdateBlastedThrust(float fFrameDelta) { _ASSERT(0); }
	virtual void Func_ExitSuffering() { _ASSERT(0); }
	virtual void Func_OnEnterDie() { _ASSERT(0); }


private:
	int m_nDice;
};


// ��� ȣ�� ���尴ü
struct ZFSMFunctionCall
{
	int id;
	int nArg[MAX_FSMMETHOD_ARG];

	void Clear() {
		id = ZFSMFunctionServer::FSMFUNC_INVALID;
		for (int i=0; i<MAX_FSMMETHOD_ARG; ++i)
			nArg[i] = FSMMETHOD_ARG_NONE;
	}
};