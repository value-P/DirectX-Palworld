#include "pch.h"
#include "TaskSheepballAttackLoop.h"

// manager
#include "EngineModule.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "GameManager.h"

// component
#include "MeshComponent.h"

// gameobject
#include "Creature.h"
#include "Monster.h"
#include "Player.h"
#include "EffectGroup.h"

// engine
#include "Model.h"

// utility
#include "MathUtility.h"

std::shared_ptr<CTaskSheepballAttackLoop> Client::CTaskSheepballAttackLoop::Create(const std::shared_ptr<FTaskSheepBallAttackLoopDesc>& _spTaskSheepBallAttackLoopDesc)
{
	shared_ptr<CTaskSheepballAttackLoop> spTaskNode = make_shared<CTaskSheepballAttackLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskSheepBallAttackLoopDesc), L"Failed to initialize sheep attack task node.");
	return spTaskNode;
}

HRESULT Client::CTaskSheepballAttackLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	// attack end task desc
	shared_ptr<FTaskSheepBallAttackLoopDesc> spTaskAttackDesc = static_pointer_cast<FTaskSheepBallAttackLoopDesc>(_spNodeDesc);
	m_iAnimationID = spTaskAttackDesc->m_iAnimationID;
	m_wpTarget = spTaskAttackDesc->m_spTarget;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskSheepballAttackLoop::OnNodeInit(float _fDeltaSecond)
{
	// ������ ���带 ����մϴ�.
	Vector3 vPosition = m_spMonster->GetPosition().value();
	m_eChannelType = m_wpSoundManager.lock()->PlaySoundEx("Roll_01_Rolling_Loop_67125546", vPosition, true);
	m_spMonster->ModifyChannelValue(m_iAnimationID, { m_eChannelType });

	// ������ ����Ʈ�� ����մϴ�.
	m_wpSpinEffect = m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_Dororong_Spin", m_wpOwnerGameObject.lock(), Vector3(0.0f, 0.5f, 0.0f));
	m_spMonster->ModifyGameObjectValue(EBlackboardGameObjectValue::EFFECT_GROUP_00, m_wpSpinEffect.lock());

	if (shared_ptr<CGameObject> spAttackTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
	}
	m_fSumAttackDeltaSeconds = 0.0f;
	m_fSumLerpDeltaSeconds = 0.0f;
}

void Client::CTaskSheepballAttackLoop::OnNodeEnter(float _fDeltaSecond)
{
	// ǥ���� �����մϴ�.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
	m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, true);

	m_wpSpinEffect.lock()->SetPosition(m_spMonster->GetPosition().value() + Vector3(0.0f, 0.5f, 0.0f));
	m_iEffectCount = 0;
}

EBTNodeResult Client::CTaskSheepballAttackLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	// ������ ����� ���ų� ���� ���
	if (m_wpTarget.expired() || m_wpTarget.lock()->IsDead())
	{
		m_fSumLerpDeltaSeconds = 0.0f;
		m_fSumAttackDeltaSeconds = 0.0f;

		// ������ ����Ʈ�� ���ϴ�.
		OffEffects();
		
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
			m_fSumLerpDeltaSeconds = 0.0f;
			m_fSumAttackDeltaSeconds = 0.0f;

			// ������ ����Ʈ�� ���ϴ�.
			OffEffects();

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
		m_fSumLerpDeltaSeconds = 0.0f;
		m_fSumAttackDeltaSeconds = 0.0f;

		// ������ ����Ʈ�� ���ϴ�.
		OffEffects();

		// ������ Ÿ�� ����Ʈ�� ����մϴ�.
		m_wpEffectManager.lock()->ActivateEffect(L"Group_Hit1", m_wpOwnerGameObject.lock()->GetPosition().value());

		// ���� ����Ʈ�� ����մϴ�.
		m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_Dororong_Stun", m_wpOwnerGameObject.lock(), Vector3(0.f, 1.f, 0.f));

		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, false);

		// ������ ���带 ���ϴ�.
		{
			m_wpSoundManager.lock()->StopChannelEx(m_eChannelType);
			vector<EChannelType> vecChannelTypes{ EChannelType::ENUM_END };
			m_spMonster->ModifyChannelValue(m_iAnimationID, vecChannelTypes);
		}

		return EBTNodeResult::SUCCEEDED;
	}

	// �ִ� ���� �ð��� ����� ���
	m_fSumAttackDeltaSeconds += _fDeltaSecond;
	if (m_fMaxAttackDeltaSeconds <= m_fSumAttackDeltaSeconds)
	{
		m_fSumLerpDeltaSeconds = 0.0f;
		m_fSumAttackDeltaSeconds = 0.0f;

		// ������ ����Ʈ�� ���ϴ�.
		OffEffects();

		// ������ ���带 ���ϴ�.
		{
			m_wpSoundManager.lock()->StopChannelEx(m_eChannelType);
			vector<EChannelType> vecChannelTypes{ EChannelType::ENUM_END };
			m_spMonster->ModifyChannelValue(m_iAnimationID, vecChannelTypes);
		}

		return EBTNodeResult::SUCCEEDED;
	}

	Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();
	m_spMonster->LookAt(vTargetPosition);

	// 1�ʿ� �� �� ���� ����Ʈ�� ����մϴ�.
	if ((float)m_iEffectCount < m_fSumAttackDeltaSeconds)
	{
		m_wpEffectManager.lock()->ActivateEffect(L"Group_Dororong_Spin_Smoke", m_spMonster->GetPosition().value() + Vector3(0.f, 0.25f, 0.f));
		++m_iEffectCount;
	}

	// ����Ʈ�� ȸ���� �����մϴ�.
	if (!m_wpSpinEffect.expired())
	{
		m_wpSpinEffect.lock()->LookAt(vTargetPosition);
	}
	
	m_spMonster->MoveForward(m_spMonster->GetMoveSpeed());
	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskSheepballAttackLoop::Release()
{
	m_spMonster = nullptr;
	m_wpTarget.reset();
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}

void Client::CTaskSheepballAttackLoop::OffEffects()
{
	if (!m_wpSpinEffect.expired())
	{
		m_wpSpinEffect.lock()->NoneActivate();
	}

	m_iEffectCount = 0;
}
