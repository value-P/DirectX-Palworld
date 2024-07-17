#include "pch.h"
#include "TaskHuggyFire.h"

#include "MeshComponent.h"
#include "ColliderComponent.h"

#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskHuggyFire> Client::CTaskHuggyFire::Create(const std::shared_ptr<FTaskHuggyFireDesc>& _spTaskHuggyFireDesc)
{
	shared_ptr<CTaskHuggyFire> spTaskNode = make_shared<CTaskHuggyFire>();
	ENSUREF(spTaskNode->Initialize(_spTaskHuggyFireDesc), L"Failed to initialize huggy fire task.");
	return spTaskNode;
}

HRESULT Client::CTaskHuggyFire::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// huggy fire task desc
	shared_ptr<FTaskHuggyFireDesc> spTaskDesc = static_pointer_cast<FTaskHuggyFireDesc>(_spNodeDesc);
	m_iAnimationID = spTaskDesc->m_iAnimationID;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskHuggyFire::OnNodeInit(float _fDeltaSecond)
{
	// 시뮬레이션에서 제외합니다.
	m_spMonster->GetBodyColliderComponent()->ApplySimulationDirectly(false);
}

void Client::CTaskHuggyFire::OnNodeEnter(float _fDeltaSecond)
{
	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskHuggyFire::ExecuteTaskNode(float _fDeltaSecond)
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

void Client::CTaskHuggyFire::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CTaskNode::Release();
}
