#include "pch.h"
#include "TaskPengulletCannon.h"

#include "MeshComponent.h"
#include "ColliderComponent.h"

#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskPengulletCannon> Client::CTaskPengulletCannon::Create(const std::shared_ptr<FTaskPengulletCannonDesc>& _spTaskPengulletCannonDesc)
{
	shared_ptr<CTaskPengulletCannon> spTaskNode = make_shared<CTaskPengulletCannon>();
	ENSUREF(spTaskNode->Initialize(_spTaskPengulletCannonDesc), L"Failed to initialize task.");
	return spTaskNode;
}

HRESULT Client::CTaskPengulletCannon::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// pengullet cannon task desc
	shared_ptr<FTaskPengulletCannonDesc> spTaskDesc = static_pointer_cast<FTaskPengulletCannonDesc>(_spNodeDesc);
	m_iAnimationID = spTaskDesc->m_iAnimationID;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskPengulletCannon::OnNodeInit(float _fDeltaSecond)
{
	// 시뮬레이션에서 제외합니다.
	m_spMonster->GetBodyColliderComponent()->ApplySimulationDirectly(false);
}

void Client::CTaskPengulletCannon::OnNodeEnter(float _fDeltaSecond)
{
	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskPengulletCannon::ExecuteTaskNode(float _fDeltaSecond)
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

void Client::CTaskPengulletCannon::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CTaskNode::Release();
}
