#include "pch.h"
#include "TaskAttackEnd.h"

#include "BehaviorTreeComponent.h"
#include "MeshComponent.h"
#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskAttackEnd> Client::CTaskAttackEnd::Create(const std::shared_ptr<FTaskAttackEndDesc>& _spTaskAttackEndDesc)
{
	shared_ptr<CTaskAttackEnd> spTaskNode = make_shared<CTaskAttackEnd>();
	ENSUREF(spTaskNode->Initialize(_spTaskAttackEndDesc), L"Failed to initialize attack end task node.");
	return spTaskNode;
}

HRESULT Client::CTaskAttackEnd::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// attack end task desc
	shared_ptr<FTaskAttackEndDesc> spTaskAttackEndDesc = static_pointer_cast<FTaskAttackEndDesc>(_spNodeDesc);
	m_iAttackEndAnimationID = spTaskAttackEndDesc->m_iAttackEndAnimationID;
	m_iGetUpAnimationID		= spTaskAttackEndDesc->m_iGetUpAnimationID;

	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskAttackEnd::OnNodeEnter(float _fDeltaSecond)
{
	/*
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET) == true
		&& m_iGetUpAnimationID != -1)
	{
		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, false);
		m_spMeshComponent->ChangeAnimation(m_iGetUpAnimationID, false);
	}
	else
	*/

	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iAttackEndAnimationID, false);
	m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
}

EBTNodeResult Client::CTaskAttackEnd::ExecuteTaskNode(float _fDeltaSecond)
{
	const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
	if (tAnimationData.m_tBaseAnimData.m_bLoopEnd &&
		(tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAttackEndAnimationID) || (tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iGetUpAnimationID))
	{
		EMonsterAIType eMonsterAIType = m_spMonster->GetMonsterAIType();

		switch (eMonsterAIType)
		{
		case Client::EMonsterAIType::WILD_PAL:
			break;

		case Client::EMonsterAIType::PARTNER_PAL:
		{
			// 목표가 죽은 경우
			shared_ptr<CGameObject> spGameObject = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET);
			shared_ptr<CCreature> spCreature = static_pointer_cast<CCreature>(spGameObject);

			if (spCreature->IsDead())
			{
				m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, false);
				m_spMonster->RequestCombatCommand();
			}
		}
		break;

		default:
			CHECKF(false, L"Monster AI type is not designated.");
			break;
		}

		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_RUNAWAY, false);
		m_wpBehaviorTreeComponent.lock()->ResetBehindBTNodeResult();
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskAttackEnd::OnNodeInit(float _fDeltaSecond)
{
}

void Client::CTaskAttackEnd::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
