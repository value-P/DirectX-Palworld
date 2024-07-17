// default
#include "pch.h"
#include "TaskElecPandaAttackLoop.h"

// manager
#include "SoundManager.h"
#include "EffectManager.h"
#include "PhysXManager.h"

// level
#include "World.h"
#include "Level.h"

// module
#include "EngineModule.h"

// component
#include "MeshComponent.h"
#include "ColliderComponent.h"

// gameobject
#include "Creature.h"
#include "ElecPanda.h"
#include "Player.h"
#include "RedRazor.h"

// engine
#include "Model.h"
#include "Panel.h"

// utility
#include "MathUtility.h"

// client
#include "ClientLoop.h"
#include "UIManager.h"

std::shared_ptr<CTaskElecPandaAttackLoop> Client::CTaskElecPandaAttackLoop::Create(const std::shared_ptr<FTaskElecPandaAttackLoopDesc>& _spTaskElecPandaAttackLoopDesc)
{
	const shared_ptr<CTaskElecPandaAttackLoop> spTaskNode = make_shared<CTaskElecPandaAttackLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskElecPandaAttackLoopDesc), L"Failed to initialize grass mammoth attack loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskElecPandaAttackLoop::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();
	m_wpPhysXManager = CEngineModule::GetInstance()->GetPhysXManager();
	m_wpUIManager = CUIManager::GetInstance();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	// attack loop task desc
	shared_ptr<FTaskElecPandaAttackLoopDesc> spTaskAttackLoopDesc = static_pointer_cast<FTaskElecPandaAttackLoopDesc>(_spNodeDesc);
	
	m_wpPlayer						= spTaskAttackLoopDesc->m_spPlayer;
	m_iStartAnimation				= spTaskAttackLoopDesc->m_iStartAnimation;
	m_iStartLoopAnimation			= spTaskAttackLoopDesc->m_iStartLoopAnimation;
	m_fMaxStartLoopDeltaSeconds		= spTaskAttackLoopDesc->m_fMaxStartLoopDeltaSeconds;
	m_iAction00AnimationID			= spTaskAttackLoopDesc->m_iAction00AnimationID;
	m_iAction01AnimationID			= spTaskAttackLoopDesc->m_iAction01AnimationID;
	m_iAction02AnimationID			= spTaskAttackLoopDesc->m_iAction02AnimationID;
	m_iActionLoop00AnimationID		= spTaskAttackLoopDesc->m_iActionLoop00AnimationID;
	m_iActionLoop01AnimationID		= spTaskAttackLoopDesc->m_iActionLoop01AnimationID;
	m_iActionLoop02AnimationID		= spTaskAttackLoopDesc->m_iActionLoop02AnimationID;
	m_fMaxActionLoop00DeltaSeconds	= spTaskAttackLoopDesc->m_fMaxActionLoop00DeltaSeconds;
	m_fMaxActionLoop01DeltaSeconds	= spTaskAttackLoopDesc->m_fMaxActionLoop01DeltaSeconds;
	m_fMaxActionLoop02DeltaSeconds	= spTaskAttackLoopDesc->m_fMaxActionLoop02DeltaSeconds;
	m_iEndAnimationID				= spTaskAttackLoopDesc->m_iEndAnimationID;
	m_eCurElecPandaSkillType		= spTaskAttackLoopDesc->m_eCurElecPandaSkillType;
	
	// etc
	m_spMonster						= dynamic_pointer_cast<CElecPanda>(m_wpOwnerGameObject.lock());
	m_spMeshComponent				= m_spMonster->GetMeshComponent();
	m_spAttackColliderComponent		= m_spMonster->GetAttackColliderComponent();

	InitializeRazorEffect();
	return S_OK;
}

void Client::CTaskElecPandaAttackLoop::OnNodeInit(float _fDeltaSecond)
{
	switch (m_eCurElecPandaSkillType)
	{
	case Client::EElecPandaSkillType::SNIPE:			// ����
	case Client::EElecPandaSkillType::DISCHARGE:		// ����
	case Client::EElecPandaSkillType::MELEE:			// ���� ���ӱ�
	case Client::EElecPandaSkillType::ELECTRIC_ARROW:	// ���� ���� ȭ��
	case Client::EElecPandaSkillType::ELECTRIC_BALL:	// ���� ���� ����
	case Client::EElecPandaSkillType::LIGHTNING:		// ����� ����
		m_spMeshComponent->ChangeAnimation(m_iStartAnimation, false);
		break;

	default:
		CHECKF(false, L"ElecPanda skill type is not designated.");
		break;
	}

	m_spAttackColliderComponent->ApplySimulationDirectly(false);

	m_bStartCompleted = false;
	m_bStartLoopCompleted = false;
	m_bActionCompleted = false;
	m_bActionLoopCompleted = false;
	m_bEndCompleted = false;

	m_fSumLoopDeltaSeconds = 0.0f;
	m_vPrePosition = nullopt;
}

void Client::CTaskElecPandaAttackLoop::OnNodeEnter(float _fDeltaSecond)
{
	// ǥ���� �����մϴ�.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);
}

EBTNodeResult Client::CTaskElecPandaAttackLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}
	m_spMonster->SetPosition(m_vPrePosition.value());

	switch (m_eCurElecPandaSkillType)
	{
	case Client::EElecPandaSkillType::SNIPE:
	{
		// ���� ��� ����
		if (!m_bStartCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iStartAnimation)
			{
				m_wpUIManager.lock()->GetLockOnPanel()->SetActive(true);
				m_spMeshComponent->ChangeAnimation(m_iStartLoopAnimation, false);
				m_bStartCompleted = true;

				// ���� ������ ����Ʈ�� Ȱ��ȭ �մϴ�.
				if (m_spRedRazor)
				{
					m_spRedRazor->Reset();
				}
			}
			m_wpUIManager.lock()->GetLockOnPanel()->SetPivotWorldPoint(Vector3(m_wpPlayer.lock()->GetPosition().value().x, m_wpPlayer.lock()->GetPosition().value().y + 1.2f, m_wpPlayer.lock()->GetPosition().value().z));
			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// ���� ��� ��
		else if (!m_bStartLoopCompleted)
		{
			m_fSumLoopDeltaSeconds += _fDeltaSecond;

			if (m_fMaxStartLoopDeltaSeconds <= m_fSumLoopDeltaSeconds)
			{
				m_wpUIManager.lock()->GetLockOnPanel()->SetActive(false);
				m_spMeshComponent->ChangeAnimation(m_iAction00AnimationID, false);
				m_fSumLoopDeltaSeconds = 0.0f;
				m_bStartLoopCompleted = true;

				m_spMonster->ChangeEye(EPalEyeType::ANGRY);

				// ���� ������ ����Ʈ�� ��Ȱ��ȭ �մϴ�.
				if (m_spRedRazor)
				{
					m_spRedRazor->EndLifeTime();
				}
			}
			m_wpUIManager.lock()->GetLockOnPanel()->SetPivotWorldPoint(Vector3(m_wpPlayer.lock()->GetPosition().value().x, m_wpPlayer.lock()->GetPosition().value().y + 1.2f, m_wpPlayer.lock()->GetPosition().value().z));
			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// �߻� ����
		else if (!m_bActionCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction00AnimationID)
			{
				// ������ ����Ʈ�� �����մϴ�.
				Vector3 vMonsterForward = m_spMonster->GetForwardVector().value();
				Vector3 vEffectPos = m_spMonster->GetPosition().value() + Vector3(0.0f, 2.0f, 0.0f) + vMonsterForward * 2.0f;
				shared_ptr<CEffectGroup> spEffectGroup = m_wpEffectManager.lock()->ActivateEffect(L"Group_ElecPanda_Razor_Charge", vEffectPos);
				spEffectGroup->LookAt(m_wpPlayer.lock()->GetPosition().value() + Vector3(0.f, 0.8f, 0.f));
				
				// �Ϸ� �Ǵ� �Ӹ� ������ �÷��̾� �������� ������ �߻��մϴ�.
				PxRaycastHit tRaycastHit;
				int32 iCollisionFlag = (int32)ECollisionLayer::BUILDING | (int32)ECollisionLayer::PLAYER;

				Vector3 vHeadPosition = m_spMonster->GetPosition().value();
				Vector3 vPlayerPosition = m_wpPlayer.lock()->GetPosition().value() + Vector3(0.f, 0.8f, 0.f);
				Vector3 vDirection = vPlayerPosition - vHeadPosition;

				if (m_wpPhysXManager.lock()->SingleRayCast(vHeadPosition, vDirection, 200.f, iCollisionFlag, &tRaycastHit))
				{
					PxVec3 vHitPosition = tRaycastHit.position;
					Vector3 vHitPositionV3 = Vector3(vHitPosition.x, vHitPosition.y, vHitPosition.z);

					CColliderComponent* pColliderComponent = static_cast<CColliderComponent*>(tRaycastHit.actor->userData);

					if (pColliderComponent->GetCollisionLayer() == ECollisionLayer::PLAYER)
					{
						m_wpPlayer.lock()->TakeDamage(m_spMonster, 100.0f);
						// m_wpEffectManager.lock()->ActivateEffect(L"Group_ThunderHit01", vHitPositionV3);
					}
				}

				m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
				m_spMeshComponent->ChangeAnimation(m_iEndAnimationID, false);
				m_bActionCompleted = true;
			}
		}

		// ���� ��
		else
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iEndAnimationID)
			{
				m_bStartCompleted		= false;
				m_bStartLoopCompleted	= false;
				m_bActionCompleted		= false;
				m_bActionLoopCompleted	= false;
				m_bEndCompleted			= false;

				m_spMonster->ModifyIntValue(EBlackboardIntValue::INT_SKILL_TYPE, CMathUtility::GetRandomInt((int32)EElecPandaSkillType::DISCHARGE, (int32)EElecPandaSkillType::DISCHARGE));
				return EBTNodeResult::SUCCEEDED;
			}
		}
	}
	break;

	case Client::EElecPandaSkillType::DISCHARGE:
	{
		// ���� ��� ����
		if (!m_bStartCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iStartAnimation)
			{
				m_spMeshComponent->ChangeAnimation(m_iStartLoopAnimation, false);
				m_bStartCompleted = true;
				m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
			}

			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// ���� ��� ��
		else if (!m_bStartLoopCompleted)
		{
			m_fSumLoopDeltaSeconds += _fDeltaSecond;
			if (m_fMaxStartLoopDeltaSeconds <= m_fSumLoopDeltaSeconds)
			{
				m_spMeshComponent->ChangeAnimation(m_iAction00AnimationID, false);
				m_fSumLoopDeltaSeconds = 0.0f;
				m_bStartLoopCompleted = true;
				m_spMonster->ChangeEye(EPalEyeType::ANGRY);
			}

			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// ����
		else if (!m_bActionCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction00AnimationID)
			{
				m_spMeshComponent->ChangeAnimation(m_iActionLoop00AnimationID, false);

				// ���� �ݶ��̴��� �մϴ�.
				m_spAttackColliderComponent->SetActive(true);
				m_bActionCompleted = true;

				// ���� ����Ʈ�� Ȱ��ȭ�մϴ�.
				Vector3 vEffectPos = m_spMonster->GetPosition().value() + Vector3(0.0f, 0.5f, 0.0f);
				m_wpDischargeThunder = m_wpEffectManager.lock()->ActivateEffect(L"Group_ElecPanda_ThunderCharge", vEffectPos);
			}
		}

		// ���� ����
		else if (!m_bActionLoopCompleted)
		{
			m_fSumDischargeDeltaSeconds += _fDeltaSecond;
			if (m_fMaxDischargeDeltaSeconds <= m_fSumDischargeDeltaSeconds)
			{
				// TODO; ���⼭ ���� ����Ʈ�� ����ϸ� ��. 0.2�� �������� �� IF�� ������ ����.
				Vector3 vEffectPos = m_spMonster->GetPosition().value() + Vector3(0.0f, 0.5f, 0.0f);
				m_wpEffectManager.lock()->ActivateEffect(L"Group_ElecPanda_Discharge_ThunderRing", vEffectPos);

				m_fSumDischargeDeltaSeconds = 0.0f;
			}

			m_fSumLoopDeltaSeconds += _fDeltaSecond;
			if (m_fMaxActionLoop00DeltaSeconds <= m_fSumLoopDeltaSeconds)
			{
				m_spMeshComponent->ChangeAnimation(m_iEndAnimationID, false);

				// ���� �ݶ��̴��� ���ϴ�.
				m_spAttackColliderComponent->SetActive(false);
				m_fSumDischargeDeltaSeconds = 0.0f;
				m_fSumLoopDeltaSeconds = 0.0f;
				m_bActionLoopCompleted = true;
			}
		}

		// ���� ��
		else
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iEndAnimationID)
			{
				m_bStartCompleted		= false;
				m_bStartLoopCompleted	= false;
				m_bActionCompleted		= false;
				m_bActionLoopCompleted	= false;
				m_bEndCompleted			= false;

				m_spAttackColliderComponent->SetActive(false);
				if (!m_wpDischargeThunder.expired())
				{
					m_wpDischargeThunder.lock()->EndLifeTime();
				}

				m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
				m_spMonster->ModifyIntValue(EBlackboardIntValue::INT_SKILL_TYPE, CMathUtility::GetRandomInt((int32)EElecPandaSkillType::MELEE, (int32)EElecPandaSkillType::MELEE));
				return EBTNodeResult::SUCCEEDED;
			}
		}
	}
	break;

	case Client::EElecPandaSkillType::MELEE:
	{
		// ���� ��� ����
		if (!m_bStartCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iStartAnimation)
			{
				m_spMonster->ChangeEye(EPalEyeType::ANGRY);
				m_spAttackColliderComponent->SetActive(true);

				// Ʈ������ �մϴ�.
				m_wpSoundManager.lock()->PlaySoundEx("Thuderbolt_01_Charge_348705042", 1.f, false);
				m_spMeshComponent->ChangeAnimation(m_iAction00AnimationID, false);
				// m_spMonster->OffAllNailTrails();
				// m_spMonster->OnRNailTrails(2.0f);
				m_bStartCompleted = true;
			}

			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// ���� ��
		else if (!m_bActionCompleted)
		{
			// �Ϸ� �Ǵ� �ִϸ��̼��� ����մϴ�.
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();

			// �Ϸ� �Ǵٸ� �̵��մϴ�.
			Vector3 vPlayerPosition = m_wpPlayer.lock()->GetPosition().value();
			Vector3 vBossPosition = m_spMonster->GetPosition().value();

			float fDistance = Vector3::Distance(vPlayerPosition, vBossPosition);
			if (10.0f <= fDistance)
			{
				m_spMonster->MoveForward(-tAnimationData.m_vRootmotionVelocity.y * 0.5f);
			}

			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction00AnimationID)
			{
				// Ʈ������ �մϴ�.
				m_spMeshComponent->ChangeAnimation(m_iAction01AnimationID, false);
				// m_spMonster->OffAllNailTrails();
				// m_spMonster->OnLNailTrails(1.2f);
			}
			else if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction01AnimationID)
			{
				// Ʈ������ �մϴ�.
				m_spMeshComponent->ChangeAnimation(m_iAction02AnimationID, false);
				// m_spMonster->OffAllNailTrails();
				// m_spMonster->OnRNailTrails(1.2f);
			}
			else if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction02AnimationID)
			{
				m_spAttackColliderComponent->SetActive(false);

				m_spMeshComponent->ChangeAnimation(m_iEndAnimationID, false);
				m_bActionCompleted = true;
			}

			// �Ϸ� �Ǵٸ� ȸ���մϴ�.
			if (tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction00AnimationID)
			{
				if (9.0f <= tAnimationData.m_tBaseAnimData.m_fTrackPosition && tAnimationData.m_tBaseAnimData.m_fTrackPosition <= 10.0f)
				{
					m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
				}
			}
			else if (tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction01AnimationID)
			{
				if (9.0f <= tAnimationData.m_tBaseAnimData.m_fTrackPosition && tAnimationData.m_tBaseAnimData.m_fTrackPosition <= 11.0f)
				{
					m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
				}
			}
			else if (tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction02AnimationID)
			{
				if (13.0f <= tAnimationData.m_tBaseAnimData.m_fTrackPosition && tAnimationData.m_tBaseAnimData.m_fTrackPosition <= 15.0f)
				{
					m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
				}
			}
		}

		// ���� ��
		else
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iEndAnimationID)
			{
				m_spAttackColliderComponent->SetActive(false);
				m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
				m_bStartCompleted		= false;
				m_bStartLoopCompleted	= false;
				m_bActionCompleted		= false;
				m_bActionLoopCompleted	= false;
				m_bEndCompleted			= false;

				m_spMonster->ModifyIntValue(EBlackboardIntValue::INT_SKILL_TYPE, CMathUtility::GetRandomInt((int32)EElecPandaSkillType::ELECTRIC_ARROW, (int32)EElecPandaSkillType::ELECTRIC_ARROW));
				return EBTNodeResult::SUCCEEDED;
			}
		}
	}
	break;

	case Client::EElecPandaSkillType::ELECTRIC_ARROW:
	{
		// ���� ��� ����
		if (!m_bStartCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iStartAnimation)
			{
				m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
				m_spMeshComponent->ChangeAnimation(m_iAction00AnimationID, false);
				m_bStartCompleted = true;
			}
			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// ����
		else if (!m_bActionCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction00AnimationID)
			{
				m_spMonster->ChangeEye(EPalEyeType::ANGRY);
				m_spMeshComponent->ChangeAnimation(m_iActionLoop00AnimationID, false);

				m_bActionCompleted = true;
			}
		}

		// ���� ��
		else if (!m_bActionLoopCompleted)
		{
			// �Ϸ� �Ǵٰ� �÷��̾� ������ �ٶ󺾴ϴ�.
			Vector3 vPlayerPosition = m_wpPlayer.lock()->GetPosition().value();
			m_spMonster->LookAt(vPlayerPosition);

			if (m_dqPlayerPositions.empty())
			{
				m_dqPlayerPositions.push_back(vPlayerPosition);
				m_dqPlayerPositions.push_back(vPlayerPosition);
			}

			// �÷��̾� ���� ��ġ�� �����մϴ�.
			m_fSumArrowDeltaSeconds += _fDeltaSecond;
			if (m_fMaxArrowDeltaSeconds <= m_fSumArrowDeltaSeconds)
			{
				Vector3 vPrePosition = m_dqPlayerPositions.front();
				m_dqPlayerPositions.pop_front();
				m_dqPlayerPositions.push_back(vPlayerPosition);

				// �̰����� ���� ��ġ���� �÷��̾� ���� ��ġ�� ����ü�� �߻��մϴ�.
				// vPrePosition; <- �÷��̾� ���� ��ġ�� ����ü ������ ��. ���� ��ġ�� 0.6�ʸ��� ���ŵ�.
				Vector3 vMonsterRight = m_spMonster->GetRightVector().value();
				vMonsterRight.Normalize();

				Vector3 vLeftBallPos = m_spMonster->GetPosition().value() + Vector3(0.f, 2.5f, 0.f) - vMonsterRight;
				Vector3 vRightBallPos = m_spMonster->GetPosition().value() + Vector3(0.f, 2.5f, 0.f) + vMonsterRight;

				m_spMonster->ActiveEffectObject(EElecPandaEffectSlots::ELECTRIC_ARROW, vLeftBallPos, m_wpPlayer.lock()->GetPosition().value() + Vector3(0.f, 1.f, 0.f), m_wpPlayer.lock());
				m_spMonster->ActiveEffectObject(EElecPandaEffectSlots::ELECTRIC_ARROW, vRightBallPos, m_wpPlayer.lock()->GetPosition().value() + Vector3(0.f, 1.f, 0.f), m_wpPlayer.lock());

				m_fSumArrowDeltaSeconds = 0.0f;
			}

			m_fSumLoopDeltaSeconds += _fDeltaSecond;
			if (m_fMaxActionLoop00DeltaSeconds <= m_fSumLoopDeltaSeconds)
			{
				m_dqPlayerPositions.clear();
				m_spMeshComponent->ChangeAnimation(m_iEndAnimationID, false);
				m_fSumArrowDeltaSeconds = 0.0f;
				m_fSumLoopDeltaSeconds = 0.0f;
				m_bActionLoopCompleted = true;
			}
		}

		// ���� ��
		else
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iEndAnimationID)
			{
				m_bStartCompleted		= false;
				m_bStartLoopCompleted	= false;
				m_bActionCompleted		= false;
				m_bActionLoopCompleted	= false;
				m_bEndCompleted			= false;

				m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
				m_spMonster->ModifyIntValue(EBlackboardIntValue::INT_SKILL_TYPE, CMathUtility::GetRandomInt((int32)EElecPandaSkillType::LIGHTNING, (int32)EElecPandaSkillType::LIGHTNING));
				return EBTNodeResult::SUCCEEDED;
			}
		}
	}
	break;
	
	case Client::EElecPandaSkillType::LIGHTNING:
	{
		// ���� ��� ����
		if (!m_bStartCompleted)
		{
			m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iStartAnimation)
			{
				m_spMeshComponent->ChangeAnimation(m_iStartLoopAnimation, false);
				m_bStartCompleted = true;
			}

			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// ���� ��� ��
		else if (!m_bStartLoopCompleted)
		{
			m_fSumLoopDeltaSeconds += _fDeltaSecond;
			if (m_fMaxStartLoopDeltaSeconds <= m_fSumLoopDeltaSeconds)
			{
				m_spMeshComponent->ChangeAnimation(m_iAction00AnimationID, false);
				m_fSumLoopDeltaSeconds = 0.0f;
				m_bStartLoopCompleted = true;
				m_spMonster->ChangeEye(EPalEyeType::ANGRY);
			}

			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// ���� ����
		else if (!m_bActionCompleted)
		{
			// ������ ����ĥ ��ġ�� ���մϴ�.
			if (m_dqLightningPositions.empty())
			{
				Vector3 vBossPosition = m_spMonster->GetPosition().value();
				Vector3 vPlayerPosition = m_wpPlayer.lock()->GetPosition().value();

				Vector3 vDirection = vPlayerPosition - vBossPosition;
				vDirection.Normalize();

				float fDistanceBtwLightning = 2.0f;

				m_dqLightningPositions.push_back(vBossPosition + 1.0f * fDistanceBtwLightning * vDirection);
				m_dqLightningPositions.push_back(vBossPosition + 2.0f * fDistanceBtwLightning * vDirection);
				m_dqLightningPositions.push_back(vBossPosition + 3.0f * fDistanceBtwLightning * vDirection);
				m_dqLightningPositions.push_back(vBossPosition + 4.0f * fDistanceBtwLightning * vDirection);
				m_dqLightningPositions.push_back(vBossPosition + 5.0f * fDistanceBtwLightning * vDirection);
			}

			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction00AnimationID)
			{
				m_spMeshComponent->ChangeAnimation(m_iActionLoop00AnimationID, false);
				m_bActionCompleted = true;
			}
		}

		// ���� ��
		else if (!m_bActionLoopCompleted)
		{
			m_fSumLightningDeltaSeconds += _fDeltaSecond;
			if (!m_dqLightningPositions.empty() && (m_fMaxLightningDeltaSeconds <= m_fSumLightningDeltaSeconds))// 0.15�ʸ��� ����
			{
				Vector3 vLightningPosition = m_dqLightningPositions.front();
				m_dqLightningPositions.pop_front();

				// �̰����� ���� ��ġ���� �÷��̾� ��ġ�� ������ 5�� ����ü�� �߻��մϴ�.
				// vLightningPosition <- ���� ���� ��ġ
				m_spMonster->ActiveEffectObject(EElecPandaEffectSlots::LIGHTING, vLightningPosition, Vector3(0.f), m_wpPlayer.lock());

				m_fSumLightningDeltaSeconds = 0.0f;
			}

			m_fSumLoopDeltaSeconds += _fDeltaSecond;
			if (m_fMaxActionLoop00DeltaSeconds <= m_fSumLoopDeltaSeconds)// m_fMaxActionLoop00DeltaSeconds�� �ּ� 2�ʴ� �Ǿ�� ��!
			{
				m_dqLightningPositions.clear();

				m_spMeshComponent->ChangeAnimation(m_iEndAnimationID, false);
				m_fSumLoopDeltaSeconds = 0.0f;
				m_bActionLoopCompleted = true;
			}
		}

		// ���� ��
		else
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iEndAnimationID)
			{
				m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
				m_bStartCompleted		= false;
				m_bStartLoopCompleted	= false;
				m_bActionCompleted		= false;
				m_bActionLoopCompleted	= false;
				m_bEndCompleted			= false;

				m_spMonster->ModifyIntValue(EBlackboardIntValue::INT_SKILL_TYPE, CMathUtility::GetRandomInt((int32)EElecPandaSkillType::SNIPE, (int32)EElecPandaSkillType::SNIPE));
				return EBTNodeResult::SUCCEEDED;
			}
		}
	}
	break;

	default:
		break;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskElecPandaAttackLoop::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;
	m_spAttackColliderComponent = nullptr;
	m_wpPlayer.reset();

	CTaskNode::Release();
}

void Client::CTaskElecPandaAttackLoop::InitializeRazorEffect()
{
	FEffectGroupDesc tEffectGroupDesc;

	tEffectGroupDesc.m_bActive = false;
	tEffectGroupDesc.m_wstrEffectGroupName = L"Group_ElecPanda_RedRazor";

	/* JSON���Ϸκ��� ��ü�� �����մϴ�. */
	m_spRedRazor = CRedRazor::CreateFromJson(make_shared<FEffectGroupDesc>(tEffectGroupDesc), tEffectGroupDesc.m_wstrEffectGroupName);

	CHECKF(m_spRedRazor, L"Failed to CreateFromJson : CEffectManager");
	m_spRedRazor->SetPosition(Vector3(0.f, -1000.f, 0.f));
	m_spRedRazor->SetPlayer(m_wpPlayer.lock());
	m_spRedRazor->SetElecPanda(static_pointer_cast<CElecPanda>(m_spMonster));
	shared_ptr<CLevel> spPersistentLevel = CWorld::GetInstance()->GetPersistentLevel();
	spPersistentLevel->AddGameObject(m_spRedRazor);
}
