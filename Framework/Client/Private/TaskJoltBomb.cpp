#include "pch.h"
#include "TaskJoltBomb.h"

#include "MeshComponent.h"
#include "ColliderComponent.h"

#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskJoltBomb> Client::CTaskJoltBomb::Create(const std::shared_ptr<FTaskJoltBombDesc>& _spTaskJoltBombDesc)
{
	shared_ptr<CTaskJoltBomb> spTaskNode = make_shared<CTaskJoltBomb>();
	ENSUREF(spTaskNode->Initialize(_spTaskJoltBombDesc), L"Failed to initialize jolt bomb task.");
	return spTaskNode;
}

HRESULT Client::CTaskJoltBomb::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// jolt bomb task desc
	shared_ptr<FTaskJoltBombDesc> spTaskDesc = static_pointer_cast<FTaskJoltBombDesc>(_spNodeDesc);
	m_iAnimationID = spTaskDesc->m_iAnimationID;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskJoltBomb::OnNodeInit(float _fDeltaSecond)
{
	// 시뮬레이션에서 제외합니다.
	m_spMonster->GetBodyColliderComponent()->ApplySimulationDirectly(false);
}

void Client::CTaskJoltBomb::OnNodeEnter(float _fDeltaSecond)
{
	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskJoltBomb::ExecuteTaskNode(float _fDeltaSecond)
{
	m_fSumDurationDeltaSeconds += _fDeltaSecond;
	if (m_fMaxDurationDeltaSeconds <= m_fSumDurationDeltaSeconds)
	{
		m_spMonster->GetBodyColliderComponent()->ApplySimulationDirectly(true);
		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_PARTNER_SKILL, false);
		m_fSumDurationDeltaSeconds = 0.0f;
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskJoltBomb::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CTaskNode::Release();
}
