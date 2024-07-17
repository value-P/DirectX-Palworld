#include "pch.h"
#include "TaskFluffyShield.h"

#include "MeshComponent.h"
#include "ColliderComponent.h"
#include "BehaviorTreeComponent.h"

#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskFluffyShield> Client::CTaskFluffyShield::Create(const std::shared_ptr<FTaskFluffyShieldDesc>& _spTaskFluffyShieldDesc)
{
	shared_ptr<CTaskFluffyShield> spTaskNode = make_shared<CTaskFluffyShield>();
	ENSUREF(spTaskNode->Initialize(_spTaskFluffyShieldDesc), L"Failed to initialize task.");
	return spTaskNode;
}

HRESULT Client::CTaskFluffyShield::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// fluffy shield task desc
	shared_ptr<FTaskFluffyShieldDesc> spTaskDesc = static_pointer_cast<FTaskFluffyShieldDesc>(_spNodeDesc);
	m_iAnimationID = spTaskDesc->m_iAnimationID;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskFluffyShield::OnNodeInit(float _fDeltaSecond)
{
	m_fSumDurationDeltaSeconds = 0.0f;
	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

void Client::CTaskFluffyShield::OnNodeEnter(float _fDeltaSecond)
{
}

EBTNodeResult Client::CTaskFluffyShield::ExecuteTaskNode(float _fDeltaSecond)
{
	m_fSumDurationDeltaSeconds += _fDeltaSecond;
	if (m_fMaxDurationDeltaSeconds <= m_fSumDurationDeltaSeconds)
	{
		m_spMeshComponent->ChangeAnimation((int32)ESheepBall::AS_SheepBall_Idle, false);
		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_PARTNER_SKILL, false);
		m_fSumDurationDeltaSeconds = 0.0f;

		m_wpBehaviorTreeComponent.lock()->ResetBehindBTNodeResult();
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskFluffyShield::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CTaskNode::Release();
}
