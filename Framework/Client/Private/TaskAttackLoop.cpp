// default
#include "pch.h"
#include "TaskAttackLoop.h"

// client
#include "ClientLoop.h"
#include "EngineModule.h"
#include "SoundManager.h"
#include "GameManager.h"

// component
#include "MeshComponent.h"

// gameobject
#include "Creature.h"
#include "Monster.h"
#include "Player.h"

// engine
#include "Model.h"

std::shared_ptr<CTaskAttackLoop> Client::CTaskAttackLoop::Create(const std::shared_ptr<FTaskAttackLoopDesc>& _spTaskAttackLoopDesc)
{
	shared_ptr<CTaskAttackLoop> spTaskNode = make_shared<CTaskAttackLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskAttackLoopDesc), L"Failed to initialize attack loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskAttackLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	// attack loop task desc
	shared_ptr<FTaskAttackLoopDesc> spTaskAttackLoopDesc = static_pointer_cast<FTaskAttackLoopDesc>(_spNodeDesc);
	m_iAnimationID = spTaskAttackLoopDesc->m_iAnimationID;
	m_wpTarget = spTaskAttackLoopDesc->m_spTarget;
	
	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskAttackLoop::OnNodeInit(float _fDeltaSecond)
{
	if (shared_ptr<CGameObject> spAttackTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
	}

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();

	m_vTargetDirection = vTargetPosition - vMonsterPosition;
	m_vTargetDirection.value().Normalize();
	m_fSumAttackDeltaSeconds = 0.0f;
}

void Client::CTaskAttackLoop::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
	m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, true);
}

EBTNodeResult Client::CTaskAttackLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	// 공격할 대상이 없거나 죽은 경우
	if (m_wpTarget.expired() || m_wpTarget.lock()->IsDead())
	{
		m_vTargetDirection = std::nullopt;
		m_fSumAttackDeltaSeconds = 0.0f;

		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
		return EBTNodeResult::FAILED;
	}

	// 상대 팰이 전투에 참여하지 않는 경우
	ECreatureType eCreatureType = m_wpTarget.lock()->GetCreatureType();
	if (ECreatureType::PAL == eCreatureType)
	{
		if (!m_wpTarget.lock()->IsParticipateInCombat())
		{
			m_vTargetDirection = std::nullopt;
			m_fSumAttackDeltaSeconds = 0.0f;


			if (m_spMonster->GetMonsterAIType() == EMonsterAIType::WILD_PAL)
			{
				shared_ptr<CPlayer> spPlayer = CGameManager::GetInstance()->GetPlayer();
				m_spMonster->ModifyGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET, spPlayer);
			}
			else if (m_spMonster->GetMonsterAIType() == EMonsterAIType::PARTNER_PAL)
			{
				m_spMonster->RequestCombatCommand();
			}

			return EBTNodeResult::FAILED;
		}
	}

	// 최대 공격 시간이 만료된 경우
	m_fSumAttackDeltaSeconds += _fDeltaSecond;
	if (m_fMaxAttackDeltaSeconds <= m_fSumAttackDeltaSeconds)
	{
		m_vTargetDirection = std::nullopt;
		m_fSumAttackDeltaSeconds = 0.0f;
		return EBTNodeResult::SUCCEEDED;
	}

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	m_spMonster->LookAt(vMonsterPosition + m_vTargetDirection.value());
	m_spMonster->MoveForward(CClientLoop::s_spClientDataSheet->k_mapPalDefaultMoveSpeeds[m_spMonster->GetMonsterType()]);

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskAttackLoop::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;
	m_wpTarget.reset();

	CTaskNode::Release();
}
