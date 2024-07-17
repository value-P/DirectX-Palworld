// default
#include "pch.h"
#include "TaskLookAt.h"

// client
#include "ClientLoop.h"

// component
#include "MeshComponent.h"

// gameobject
#include "Monster.h"
#include "Player.h"
#include "Model.h"

// utility
#include "MathUtility.h"

std::shared_ptr<CTaskLookAt> Client::CTaskLookAt::Create(const std::shared_ptr<FTaskLookAtDesc>& _spTaskLookAtDesc)
{
	shared_ptr<CTaskLookAt> spTaskNode = make_shared<CTaskLookAt>();
	ENSUREF(spTaskNode->Initialize(_spTaskLookAtDesc), L"Failed to initialize look at task node.");
	return spTaskNode;
}

HRESULT Client::CTaskLookAt::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// look at task desc
	shared_ptr<FTaskLookAtDesc> spTaskLookAtDesc = static_pointer_cast<FTaskLookAtDesc>(_spNodeDesc);
	m_iAnimationID			= spTaskLookAtDesc->m_iAnimationID;
	m_wpTarget				= spTaskLookAtDesc->m_spTarget;
	m_fMaxLerpDeltaSeconds	= spTaskLookAtDesc->m_fMaxLerpDeltaSeconds;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskLookAt::OnNodeInit(float _fDeltaSecond)
{
	if (!m_vForward.has_value())
	{
		m_vForward = m_spMonster->GetForwardVector().value();
		m_vForward.value().Normalize();
	}

	if (shared_ptr<CGameObject> spAttackTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
	}

	m_fSumLerpDeltaSeconds = 0.0f;
}

void Client::CTaskLookAt::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskLookAt::ExecuteTaskNode(float _fDeltaSecond)
{
	if (m_wpTarget.expired() || m_wpTarget.lock()->IsDead() || !m_wpTarget.lock()->IsParticipateInCombat())
	{
		m_spMonster->RequestCombatCommand();
		m_wpTarget.reset();
		m_vForward = nullopt;
		return EBTNodeResult::FAILED;
	}

	
	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();

	Vector3 vDirection = vTargetPosition - vMonsterPosition;
	vDirection.Normalize();

	m_fSumLerpDeltaSeconds += _fDeltaSecond;
	if (m_fMaxLerpDeltaSeconds <= m_fSumLerpDeltaSeconds)
	{
		m_vForward = nullopt;
		m_fSumLerpDeltaSeconds = 0.0f;
		return EBTNodeResult::SUCCEEDED;
	}

	float fRatio = m_fSumLerpDeltaSeconds / m_fMaxLerpDeltaSeconds;
	Vector3 vLerpDirection = Vector3::Lerp(m_vForward.value(), vDirection, fRatio);
	m_spMonster->LookAt(vMonsterPosition + vLerpDirection);

	EMonsterType eMonsterType = m_spMonster->GetMonsterType();
	m_spMonster->MoveForward(CClientLoop::s_spClientDataSheet->k_mapPalDefaultCreepSpeeds[eMonsterType]);
	
	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskLookAt::Release()
{
	m_spMeshComponent = nullptr;
	m_wpTarget.reset();
	m_spMonster = nullptr;

	CBTNode::Release();
}
