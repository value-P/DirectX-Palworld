// default
#include "pch.h"
#include "TaskBoarAttackLoop.h"

// manager
#include "EngineModule.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "GameManager.h"

// client
#include "ClientLoop.h"

// component
#include "MeshComponent.h"

// gameobject
#include "Creature.h"
#include "Monster.h"
#include "Player.h"
#include "EffectGroup.h"

// engine
#include "Model.h"

std::shared_ptr<CTaskBoarAttackLoop> Client::CTaskBoarAttackLoop::Create(const std::shared_ptr<FTaskBoarAttackLoopDesc>& _spTaskBoarAttackLoopDesc)
{
	const shared_ptr<CTaskBoarAttackLoop> spTaskNode = make_shared<CTaskBoarAttackLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskBoarAttackLoopDesc), L"Failed to initialize boar attack loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskBoarAttackLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	// attack loop task desc
	shared_ptr<FTaskBoarAttackLoopDesc> spTaskAttackLoopDesc = static_pointer_cast<FTaskBoarAttackLoopDesc>(_spNodeDesc);
	m_iAnimationID = spTaskAttackLoopDesc->m_iAnimationID;
	m_wpTarget = spTaskAttackLoopDesc->m_spTarget;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskBoarAttackLoop::OnNodeInit(float _fDeltaSecond)
{
	// ���� ���带 ����մϴ�.
	Vector3 vPosition = m_spMonster->GetPosition().value();
	m_eChannelType = m_wpSoundManager.lock()->PlaySoundEx("Rush_01_Start_426502798", vPosition, false);
	m_spMonster->ModifyChannelValue(m_iAnimationID, { m_eChannelType });

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

void Client::CTaskBoarAttackLoop::OnNodeEnter(float _fDeltaSecond)
{
	// ǥ���� �����մϴ�.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
	m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, true);

	// ���� ����Ʈ�� ����մϴ�.
	m_wpRushEffect = m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_BoarRush", m_spMonster, Vector3(0.f));
	if (!m_wpRushEffect.expired())
	{
		Vector3 vLook = m_wpTarget.lock()->GetPosition().value() - m_spMonster->GetPosition().value();
		m_wpRushEffect.lock()->LookAt(m_wpRushEffect.lock()->GetPosition().value() + vLook);
	}
}

EBTNodeResult Client::CTaskBoarAttackLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	// ������ ����� ���ų� ���� ���
	if (m_wpTarget.expired() || m_wpTarget.lock()->IsDead())
	{
		m_vTargetDirection = std::nullopt;
		m_fSumAttackDeltaSeconds = 0.0f;

		OffRushEffect();

		// ������ ���带 ���ϴ�.
		{
			m_wpSoundManager.lock()->StopChannelEx(m_eChannelType);
			vector<EChannelType> vecChannelTypes{ EChannelType::ENUM_END };
			m_spMonster->ModifyChannelValue(m_iAnimationID, vecChannelTypes);
		}

		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
		return EBTNodeResult::FAILED;
	}

	// ��� ���� ������ �������� �ʴ� ���
	ECreatureType eCreatureType = m_wpTarget.lock()->GetCreatureType();
	if (ECreatureType::PAL == eCreatureType)
	{
		if (!m_wpTarget.lock()->IsParticipateInCombat())
		{
			m_vTargetDirection = std::nullopt;
			m_fSumAttackDeltaSeconds = 0.0f;

			OffRushEffect();

			// ������ ���带 ���ϴ�.
			{
				m_wpSoundManager.lock()->StopChannelEx(m_eChannelType);
				vector<EChannelType> vecChannelTypes{ EChannelType::ENUM_END };
				m_spMonster->ModifyChannelValue(m_iAnimationID, vecChannelTypes);
			}

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

	// ���� ����� ������ ���
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET) == true)
	{
		m_vTargetDirection = std::nullopt;
		m_fSumAttackDeltaSeconds = 0.0f;
		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, false);

		OffRushEffect();
		m_wpEffectManager.lock()->ActivateEffect(L"Group_Hit2", m_wpTarget.lock()->GetPosition().value() + Vector3(0.f, 1.f, 0.f));
		m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_Dororong_Stun", m_wpOwnerGameObject.lock(), Vector3(0.f, 1.f, 0.f));

		return EBTNodeResult::SUCCEEDED;
	}

	// �ִ� ���� �ð��� ����� ���
	m_fSumAttackDeltaSeconds += _fDeltaSecond;
	if (m_fMaxAttackDeltaSeconds <= m_fSumAttackDeltaSeconds)
	{
		m_vTargetDirection = std::nullopt;
		m_fSumAttackDeltaSeconds = 0.0f;

		OffRushEffect();

		return EBTNodeResult::SUCCEEDED;
	}

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	m_spMonster->LookAt(vMonsterPosition + m_vTargetDirection.value());
	m_spMonster->MoveForward(CClientLoop::s_spClientDataSheet->k_mapPalDefaultRunSpeeds[EMonsterType::BOAR]);

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskBoarAttackLoop::Release()
{
	m_spMonster = nullptr;
	m_wpTarget.reset();
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}

void Client::CTaskBoarAttackLoop::OffRushEffect()
{
	if (m_wpRushEffect.expired()) { return; }

	m_wpRushEffect.lock()->EndLifeTime();
}
