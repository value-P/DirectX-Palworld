// default
#include "pch.h"
#include "TaskHedgeHogAttackLoop.h"

// manager
#include "EffectManager.h"

// component
#include "MeshComponent.h"
#include "ColliderComponent.h"

// gameobject
#include "Monster.h"

#include "Model.h"

std::shared_ptr<CTaskHedgeHogAttackLoop> Client::CTaskHedgeHogAttackLoop::Create(const std::shared_ptr<FTaskHedgeHogAttackLoopDesc>& _spTaskAttackLoopDesc)
{
	shared_ptr<CTaskHedgeHogAttackLoop> spTaskNode = make_shared<CTaskHedgeHogAttackLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskAttackLoopDesc), L"Failed to initialize dreamdemon attack loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskHedgeHogAttackLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	m_wpEffectManager = CEffectManager::GetInstance();

	// attack loop task desc
	shared_ptr<FTaskHedgeHogAttackLoopDesc> spTaskAttackLoopDesc = static_pointer_cast<FTaskHedgeHogAttackLoopDesc>(_spNodeDesc);
	m_iActionAnimationID = spTaskAttackLoopDesc->m_iActionAnimationID;
	m_iActionLoopAnimationID = spTaskAttackLoopDesc->m_iActionLoopAnimationID;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	m_wpColliderComponent = m_spMonster->GetAttackColliderComponent();
	return S_OK;
}

void Client::CTaskHedgeHogAttackLoop::OnNodeInit(float _fDeltaSecond)
{
	m_bActionCompleted = false;
	m_fSumAttackDeltaSeconds = 0.0f;
}

void Client::CTaskHedgeHogAttackLoop::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iActionAnimationID, false);
	m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, true);
	m_wpColliderComponent.lock()->ApplySimulationDirectly(true);

	m_wpEffectManager.lock()->ActivateEffect(L"Group_Dochi_ElectricBomb_Explosion", m_spMonster->GetPosition().value());
}

EBTNodeResult Client::CTaskHedgeHogAttackLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	if (m_bActionCompleted)
	{
		m_fSumAttackDeltaSeconds += _fDeltaSecond;
		if (m_fMaxAttackDeltaSeconds <= m_fSumAttackDeltaSeconds)
		{
			m_fSumAttackDeltaSeconds = 0.0f;
			m_bActionCompleted = false;

			m_wpColliderComponent.lock()->ApplySimulationDirectly(false);
			return EBTNodeResult::SUCCEEDED;
		}
	}
	else
	{
		const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
		if (tAnimationData.m_tBaseAnimData.m_bLoopEnd
			&& tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iActionAnimationID)
		{
			m_spMeshComponent->ChangeAnimation(m_iActionLoopAnimationID, false);
			m_bActionCompleted = true;
		}
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskHedgeHogAttackLoop::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}