// default
#include "pch.h"
#include "TaskChase.h"

// manager
#include "GameManager.h"

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

// client
#include "ClientLoop.h"

std::shared_ptr<CTaskChase> Client::CTaskChase::Create(const std::shared_ptr<FTaskChaseDesc>& _spTaskChaseDesc)
{
	shared_ptr<CTaskChase> spTaskNode = make_shared<CTaskChase>();
	ENSUREF(spTaskNode->Initialize(_spTaskChaseDesc), L"Failed to initialize chase task node.");
	return spTaskNode;
}

HRESULT Client::CTaskChase::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// chase task desc
	shared_ptr<FTaskChaseDesc> spTaskChaseDesc = static_pointer_cast<FTaskChaseDesc>(_spNodeDesc);
	m_iAnimationID			= spTaskChaseDesc->m_iAnimationID;
	m_wpTarget				= spTaskChaseDesc->m_spTarget;

	if (spTaskChaseDesc->m_fMaxChaseDeltaSeconds != 0.0f)
	{
		m_fMaxChaseDeltaSeconds = spTaskChaseDesc->m_fMaxChaseDeltaSeconds;
	}
	
	m_fAcceptableRadius		= spTaskChaseDesc->m_fAcceptableRadius;

	// etc
	m_spMonster				= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent		= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskChase::OnNodeInit(float _fDeltaSecond)
{
	m_fSumChaseDeltaSeconds = 0.0f;

	// 임시
	if (shared_ptr<CGameObject> spAttackTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		shared_ptr<CPlayer> spPlayer = CGameManager::GetInstance()->GetPlayer();
		if (m_spMonster == spPlayer->GetPartnerPal() && spAttackTarget == spPlayer)
		{
			m_wpTarget.reset();
		}
		else
		{
			m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
		}
	}
}

void Client::CTaskChase::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskChase::ExecuteTaskNode(float _fDeltaSecond)
{
	if (m_wpTarget.expired() || m_wpTarget.lock()->IsDead() || !m_wpTarget.lock()->IsParticipateInCombat())
	{
		m_spMonster->RequestCombatCommand();
		m_wpTarget.reset();
		return EBTNodeResult::FAILED;
	}

	Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();
	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();

	float fDistance = Vector3::Distance(vTargetPosition, vMonsterPosition);
	if (fDistance < m_fAcceptableRadius)
	{
		return EBTNodeResult::SUCCEEDED;
	}

	// 추격할 수 있는 최대 시간이 있는 경우
	if (m_fMaxChaseDeltaSeconds.has_value())
	{
		m_fSumChaseDeltaSeconds += _fDeltaSecond;
		if (m_fMaxChaseDeltaSeconds <= m_fSumChaseDeltaSeconds)
		{
			return EBTNodeResult::FAILED;
		}
	}

	m_spMonster->LookAt(vTargetPosition);

	float fSpeed = CClientLoop::s_spClientDataSheet->k_mapPalDefaultRunSpeeds[m_spMonster->GetMonsterType()];
	m_spMonster->MoveForward(fSpeed);
	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskChase::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CTaskNode::Release();
}
