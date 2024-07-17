#include "pch.h"
#include "FSMNode_JetDragonIdle.h"

#include "FSMComponent.h"
#include "ColliderComponent.h"

std::shared_ptr<CFSMNode_JetDragonIdle> Client::CFSMNode_JetDragonIdle::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	shared_ptr<CFSMNode_JetDragonIdle> spInstance = make_shared<CFSMNode_JetDragonIdle>();
	CHECKF(spInstance, L"CFSMNode_JetDragonIdle::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spRidingObject);

	return spInstance;
}

void Client::CFSMNode_JetDragonIdle::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	CFSMNode_Riding::Initialize(_spMeshComponent, _spRidingObject);
}

void Client::CFSMNode_JetDragonIdle::OnEnterState()
{
	CFSMNode_Riding::OnEnterState();

	m_wpRidingObject.lock()->GetBodyColliderComponent()->SetLinearVelocity(PxVec3(0.f, 0.f, 0.f));
	m_wpRidingObject.lock()->GetBodyColliderComponent()->SetAngularVelocity(PxVec3(0.f, 0.f, 0.f));

	m_wpRidingObject.lock()->GetBodyColliderComponent()->SetUseGravity(false);
}

void Client::CFSMNode_JetDragonIdle::OnState(float _fDeltaSeconds)
{
	CFSMNode_Riding::OnState(_fDeltaSeconds);

	if (IsMoving())
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)ERidingObjectState::MOVE);
		return;
	}
}

void Client::CFSMNode_JetDragonIdle::OnExitState()
{
	m_wpRidingObject.lock()->GetBodyColliderComponent()->SetUseGravity(true);
}

void Client::CFSMNode_JetDragonIdle::KeyInput()
{
}

void Client::CFSMNode_JetDragonIdle::ExpectNextAnim(FAnimData& _tAnimData)
{
	_tAnimData.m_iAnimID = (int32)EJetDragon::AS_JetDragon_Idle;
}