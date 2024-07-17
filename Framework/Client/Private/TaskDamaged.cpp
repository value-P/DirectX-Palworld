#include "pch.h"
#include "TaskDamaged.h"

#include "MeshComponent.h"
#include "BehaviorTreeComponent.h"

#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskDamaged> Client::CTaskDamaged::Create(const std::shared_ptr<FTaskDamageDesc>& _spTaskDamageDesc)
{
	shared_ptr<CTaskDamaged> spTaskNode = make_shared<CTaskDamaged>();
	ENSUREF(spTaskNode->Initialize(_spTaskDamageDesc), L"Failed to initialize task damage node.");
	return spTaskNode;
}

HRESULT Client::CTaskDamaged::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// damage task desc
	shared_ptr<FTaskDamageDesc> spTaskDamageDesc = static_pointer_cast<FTaskDamageDesc>(_spNodeDesc);
	m_iAnimationID = spTaskDamageDesc->m_iAnimationID;
	m_wpTarget = spTaskDamageDesc->m_spTarget;

	// etc
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskDamaged::OnNodeInit(float _fDeltaSecond)
{
	if (shared_ptr<CGameObject> spAttackTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
	}
}

void Client::CTaskDamaged::OnNodeEnter(float _fDeltaSecond)
{
	m_wpBehaviorTreeComponent.lock()->ResetBehindBTNodeResult();
	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);

	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DAMAGED);
	m_spMonster->ChangeMouth(EPalMouthType::DAMAGED);

	if (!m_wpTarget.expired())
	{
		Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();
		m_spMonster->LookAt(vTargetPosition);
	}
}

EBTNodeResult Client::CTaskDamaged::ExecuteTaskNode(float _fDeltaSecond)
{
	// 공격받지 않았거나 목표를 공격하는 중인 경우
	if (!m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_DAMAGED) ||
		m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_ATTACK))
	{
		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_DAMAGED, false);
		return EBTNodeResult::FAILED;
	}

	const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
	if (tAnimationData.m_tBaseAnimData.m_bLoopEnd
		&& tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAnimationID)
	{
		// 팰이 공격당한 후 반격합니다.
		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_DAMAGED, false);

		switch (m_spMonster->GetMonsterType())
		{
		case EMonsterType::WOOL_FOX:
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_RUNAWAY, true);
			break;

		default:
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_RUNAWAY, false);
			break;
		}
		
		return EBTNodeResult::SUCCEEDED;
	}

    return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskDamaged::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;
	m_wpTarget.reset();

	CTaskNode::Release();
}
