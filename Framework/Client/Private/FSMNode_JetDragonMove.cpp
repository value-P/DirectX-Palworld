#include "pch.h"
#include "FSMNode_JetDragonMove.h"

#include "SoundManager.h"
#include "InputManager.h"
#include "FSMComponent.h"

std::shared_ptr<CFSMNode_JetDragonMove> Client::CFSMNode_JetDragonMove::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	shared_ptr<CFSMNode_JetDragonMove> spInstance = make_shared<CFSMNode_JetDragonMove>();
	CHECKF(spInstance, L"CFSMNode_JetDragonMove::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spRidingObject);

	return spInstance;
}

void Client::CFSMNode_JetDragonMove::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	CFSMNode_Riding::Initialize(_spMeshComponent, _spRidingObject);
}

void Client::CFSMNode_JetDragonMove::OnEnterState()
{
	m_eChannel = m_wpSoundManager.lock()->PlaySoundEx("Rush_02_Loop_270537059", m_wpRidingObject.lock()->GetPosition().value(), 1.f, true);

	CFSMNode_Riding::OnEnterState();
	m_wpRidingObject.lock()->OnRidingEffect();
}

void Client::CFSMNode_JetDragonMove::OnState(float _fDeltaSeconds)
{
	CFSMNode_Riding::OnState(_fDeltaSeconds);

	if (!IsMoving())
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)ERidingObjectState::IDLE);
		return;
	}
}

void Client::CFSMNode_JetDragonMove::OnExitState()
{
	m_bSprint = false;
	m_wpRidingObject.lock()->SetSpeed(10.f);
	m_wpRidingObject.lock()->SetRiderCamera(false);
	m_wpRidingObject.lock()->OffRidingEffect();

	m_wpSoundManager.lock()->StopChannelEx(m_eChannel);
}

void Client::CFSMNode_JetDragonMove::KeyInput()
{
	if (m_wpInputManager.lock()->IsKeyDown(DIK_LSHIFT))
	{
		m_bSprint = true;

		m_wpRidingObject.lock()->SetSpeed(20.f);

		m_wpRidingObject.lock()->SetRiderCamera(true);

		NodePlaySound("Rush_01_Start_426502798");
	}
	else if (m_wpInputManager.lock()->IsKeyUp(DIK_LSHIFT))
	{
		m_bSprint = false;
		m_wpRidingObject.lock()->SetSpeed(10.f);

		m_wpRidingObject.lock()->SetRiderCamera(false);
	}
}

void Client::CFSMNode_JetDragonMove::ExpectNextAnim(FAnimData& _tAnimData)
{
	if (m_bSprint)
	{
		_tAnimData.m_iAnimID = (int32)EJetDragon::AS_JetDragon_Sprint;
	}
	else
	{
		_tAnimData.m_iAnimID = (int32)EJetDragon::AS_JetDragon_Walk_Riding;
		//_tAnimData.m_iAnimID = (int32)EJetDragon::AS_JetDragon_Run_Riding;
	}
}
