#include "pch.h"
#include "FSMNode_WindDeerIdle.h"

#include "FSMComponent.h"
#include "ColliderComponent.h"

std::shared_ptr<CFSMNode_WindDeerIdle> Client::CFSMNode_WindDeerIdle::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	shared_ptr<CFSMNode_WindDeerIdle> spInstance = make_shared<CFSMNode_WindDeerIdle>();
	CHECKF(spInstance, L"CFSMNode_WindDeerIdle::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spRidingObject);

	return spInstance;
}

void Client::CFSMNode_WindDeerIdle::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	CFSMNode_Riding::Initialize(_spMeshComponent, _spRidingObject);	 
}

void Client::CFSMNode_WindDeerIdle::OnEnterState()
{
	CFSMNode_Riding::OnEnterState();

	m_wpRidingObject.lock()->GetBodyColliderComponent()->SetLinearVelocity(PxVec3(0.f, 0.f, 0.f));
	m_wpRidingObject.lock()->GetBodyColliderComponent()->SetAngularVelocity(PxVec3(0.f, 0.f, 0.f));

	m_wpRidingObject.lock()->GetBodyColliderComponent()->SetUseGravity(false);
}

void Client::CFSMNode_WindDeerIdle::OnState(float _fDeltaSeconds)
{
	CFSMNode_Riding::OnState(_fDeltaSeconds);

	if (IsMoving())
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)ERidingObjectState::MOVE);
		return;
	}

	if (!m_tOwnerStateData.m_bOnGround)
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)ERidingObjectState::JUMP);
		return;
	}
}

void Client::CFSMNode_WindDeerIdle::OnExitState()
{
	m_wpRidingObject.lock()->GetBodyColliderComponent()->SetUseGravity(true);
}

void Client::CFSMNode_WindDeerIdle::KeyInput()
{
}

void Client::CFSMNode_WindDeerIdle::ExpectNextAnim(FAnimData& _tAnimData)
{
	_tAnimData.m_iAnimID = (int32)EFengyunDeeper::AS_FengyunDeeper_Idle;
}
