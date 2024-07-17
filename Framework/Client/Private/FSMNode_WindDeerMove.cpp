#include "pch.h"
#include "FSMNode_WindDeerMove.h"

#include "InputManager.h"
#include "FSMComponent.h"

std::shared_ptr<CFSMNode_WindDeerMove> Client::CFSMNode_WindDeerMove::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	shared_ptr<CFSMNode_WindDeerMove> spInstance = make_shared<CFSMNode_WindDeerMove>();
	CHECKF(spInstance, L"CFSMNode_WindDeerMove::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spRidingObject);

	return spInstance;
}

void Client::CFSMNode_WindDeerMove::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	CFSMNode_Riding::Initialize(_spMeshComponent, _spRidingObject);
}

void Client::CFSMNode_WindDeerMove::OnEnterState()
{
	CFSMNode_Riding::OnEnterState();
	m_wpRidingObject.lock()->OnRidingEffect();
}

void Client::CFSMNode_WindDeerMove::OnState(float _fDeltaSeconds)
{
	CFSMNode_Riding::OnState(_fDeltaSeconds);

	ComputeFootstep(_fDeltaSeconds);

	if (!IsMoving())
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)ERidingObjectState::IDLE);
		return;
	}

	if (!m_tOwnerStateData.m_bOnGround)
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)ERidingObjectState::JUMP);
		return;
	}
}

void Client::CFSMNode_WindDeerMove::OnExitState()
{
	m_bSprint = false;
	m_wpRidingObject.lock()->SetSpeed(8.f);
	m_wpRidingObject.lock()->SetRiderCamera(false);
	m_wpRidingObject.lock()->OffRidingEffect();
}

void Client::CFSMNode_WindDeerMove::KeyInput()
{
	if (m_wpInputManager.lock()->IsKeyDown(DIK_LSHIFT))
	{
		m_bSprint = true;

		m_wpRidingObject.lock()->SetSpeed(13.f);

		m_wpRidingObject.lock()->SetRiderCamera(true);

		NodePlaySound("Rush_01_Start_426502798");
	}
	else if(m_wpInputManager.lock()->IsKeyUp(DIK_LSHIFT))
	{
		m_bSprint = false;
		m_wpRidingObject.lock()->SetSpeed(8.f);

		m_wpRidingObject.lock()->SetRiderCamera(false);
	}
}

void Client::CFSMNode_WindDeerMove::ExpectNextAnim(FAnimData& _tAnimData)
{
	_tAnimData.m_iAnimID = (int32)EFengyunDeeper::AS_FengyunDeeper_Run;
}

void Client::CFSMNode_WindDeerMove::ComputeFootstep(float _fDeltaSeconds)
{
	m_fFootstepTime += _fDeltaSeconds;

	if (m_fFootstepTime > 0.8f)
	{
		m_fFootstepTime = 0.f;
		NodePlaySound("Rush_01_Set (1)_322105374");
	}
}
