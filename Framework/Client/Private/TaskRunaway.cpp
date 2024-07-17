// model
#include "pch.h"
#include "TaskRunaway.h"

// component
#include "MeshComponent.h"

// gameobject
#include "Creature.h"
#include "Monster.h"
#include "Player.h"

// engine
#include "Model.h"

// utility
#include "MathUtility.h"

std::shared_ptr<CTaskRunaway> Client::CTaskRunaway::Create(const std::shared_ptr<FTaskRunawayDesc>& _spTaskRunawayLoopDesc)
{
	shared_ptr<CTaskRunaway> spTaskNode = make_shared<CTaskRunaway>();
	ENSUREF(spTaskNode->Initialize(_spTaskRunawayLoopDesc), L"Failed to initialize runaway task node.");
	return spTaskNode;
}

HRESULT Client::CTaskRunaway::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// attack end task desc
	shared_ptr<FTaskRunawayDesc> spTaskRunawayLoopDesc = static_pointer_cast<FTaskRunawayDesc>(_spNodeDesc);
	m_iAnimationID				= spTaskRunawayLoopDesc->m_iAnimationID;
	m_wpTarget					= spTaskRunawayLoopDesc->m_spTarget;
	m_fMaxRunawayDeltaSeconds	= spTaskRunawayLoopDesc->m_fMaxRunawayDeltaSeconds;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskRunaway::OnNodeInit(float _fDeltaSecond)
{
	if (m_wpTarget.lock()->GetCreatureType() == ECreatureType::PLAYER)
	{
		m_wpPlayer = static_pointer_cast<CPlayer>(m_wpTarget.lock());
	}
}

void Client::CTaskRunaway::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DAMAGED);
	m_spMonster->ChangeMouth(EPalMouthType::DAMAGED);

	if (!m_vTargetDirection.has_value())
	{
		Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
		Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();

		m_vTargetDirection = vMonsterPosition - vTargetPosition;
		m_vTargetDirection.value().Normalize();
	}

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskRunaway::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_RUNAWAY))
	{
		m_vTargetDirection = std::nullopt;
		m_fSumLerpDeltaSeconds = 0.0f;
		m_fSumRunawayDeltaSeconds = 0.0f;
		return EBTNodeResult::FAILED;
	}

	// 도망칠 대상이 없거나 죽은 경우
	if (m_wpTarget.expired() || m_wpTarget.lock()->IsDead())
	{
		m_vTargetDirection = std::nullopt;
		m_fSumLerpDeltaSeconds = 0.0f;
		m_fSumRunawayDeltaSeconds = 0.0f;

		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_RUNAWAY, false);
		return EBTNodeResult::SUCCEEDED;
	}

	// 최대 도망 시간이 만료된 경우
	m_fSumRunawayDeltaSeconds += _fDeltaSecond;
	if (m_fMaxRunawayDeltaSeconds <= m_fSumRunawayDeltaSeconds)
	{
		m_vTargetDirection = std::nullopt;
		m_fSumLerpDeltaSeconds = 0.0f;
		m_fSumRunawayDeltaSeconds = 0.0f;

		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_RUNAWAY, false);
		return EBTNodeResult::SUCCEEDED;
	}

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	Vector3 vForward = m_spMonster->GetForwardVector().value();

	if (m_vTargetDirection.has_value())
	{
		m_fSumLerpDeltaSeconds += _fDeltaSecond;
		if (m_fSumLerpDeltaSeconds <= m_fMaxLerpDeltaSeconds)
		{
			float fRatio = m_fSumLerpDeltaSeconds / m_fMaxLerpDeltaSeconds;
			Vector3 vLerpDirection = CMathUtility::EaseOutCirc(vForward, m_vTargetDirection.value(), fRatio);
			m_spMonster->LookAt(vMonsterPosition + vLerpDirection);
		}
		else
		{
			m_spMonster->LookAt(vMonsterPosition + m_vTargetDirection.value());
		}
		m_spMonster->MoveForward(m_spMonster->GetMoveSpeed());
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskRunaway::Release()
{
	m_spMonster = nullptr;
	m_wpTarget.reset();
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
