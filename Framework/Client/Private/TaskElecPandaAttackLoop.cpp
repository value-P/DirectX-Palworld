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
	case Client::EElecPandaSkillType::SNIPE:			// 저격
	case Client::EElecPandaSkillType::DISCHARGE:		// 방전
	case Client::EElecPandaSkillType::MELEE:			// 근접 연속기
	case Client::EElecPandaSkillType::ELECTRIC_ARROW:	// 직사 전기 화살
	case Client::EElecPandaSkillType::ELECTRIC_BALL:	// 유도 전기 구슬
	case Client::EElecPandaSkillType::LIGHTNING:		// 방사형 낙뢰
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
	// 표정을 변경합니다.
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
		// 공격 대기 시작
		if (!m_bStartCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iStartAnimation)
			{
				m_wpUIManager.lock()->GetLockOnPanel()->SetActive(true);
				m_spMeshComponent->ChangeAnimation(m_iStartLoopAnimation, false);
				m_bStartCompleted = true;

				// 빨간 레이저 이펙트를 활성화 합니다.
				if (m_spRedRazor)
				{
					m_spRedRazor->Reset();
				}
			}
			m_wpUIManager.lock()->GetLockOnPanel()->SetPivotWorldPoint(Vector3(m_wpPlayer.lock()->GetPosition().value().x, m_wpPlayer.lock()->GetPosition().value().y + 1.2f, m_wpPlayer.lock()->GetPosition().value().z));
			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// 공격 대기 중
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

				// 빨간 레이저 이펙트를 비활성화 합니다.
				if (m_spRedRazor)
				{
					m_spRedRazor->EndLifeTime();
				}
			}
			m_wpUIManager.lock()->GetLockOnPanel()->SetPivotWorldPoint(Vector3(m_wpPlayer.lock()->GetPosition().value().x, m_wpPlayer.lock()->GetPosition().value().y + 1.2f, m_wpPlayer.lock()->GetPosition().value().z));
			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// 발사 시작
		else if (!m_bActionCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction00AnimationID)
			{
				// 레이저 이펙트를 생성합니다.
				Vector3 vMonsterForward = m_spMonster->GetForwardVector().value();
				Vector3 vEffectPos = m_spMonster->GetPosition().value() + Vector3(0.0f, 2.0f, 0.0f) + vMonsterForward * 2.0f;
				shared_ptr<CEffectGroup> spEffectGroup = m_wpEffectManager.lock()->ActivateEffect(L"Group_ElecPanda_Razor_Charge", vEffectPos);
				spEffectGroup->LookAt(m_wpPlayer.lock()->GetPosition().value() + Vector3(0.f, 0.8f, 0.f));
				
				// 일렉 판다 머리 위에서 플레이어 방향으로 광선을 발사합니다.
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

		// 공격 끝
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
		// 공격 대기 시작
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

		// 공격 대기 중
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

		// 공격
		else if (!m_bActionCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction00AnimationID)
			{
				m_spMeshComponent->ChangeAnimation(m_iActionLoop00AnimationID, false);

				// 공격 콜라이더를 켭니다.
				m_spAttackColliderComponent->SetActive(true);
				m_bActionCompleted = true;

				// 번개 이펙트를 활성화합니다.
				Vector3 vEffectPos = m_spMonster->GetPosition().value() + Vector3(0.0f, 0.5f, 0.0f);
				m_wpDischargeThunder = m_wpEffectManager.lock()->ActivateEffect(L"Group_ElecPanda_ThunderCharge", vEffectPos);
			}
		}

		// 공격 루프
		else if (!m_bActionLoopCompleted)
		{
			m_fSumDischargeDeltaSeconds += _fDeltaSecond;
			if (m_fMaxDischargeDeltaSeconds <= m_fSumDischargeDeltaSeconds)
			{
				// TODO; 여기서 방출 이펙트를 출력하면 됌. 0.2초 간격으로 이 IF문 안으로 들어옴.
				Vector3 vEffectPos = m_spMonster->GetPosition().value() + Vector3(0.0f, 0.5f, 0.0f);
				m_wpEffectManager.lock()->ActivateEffect(L"Group_ElecPanda_Discharge_ThunderRing", vEffectPos);

				m_fSumDischargeDeltaSeconds = 0.0f;
			}

			m_fSumLoopDeltaSeconds += _fDeltaSecond;
			if (m_fMaxActionLoop00DeltaSeconds <= m_fSumLoopDeltaSeconds)
			{
				m_spMeshComponent->ChangeAnimation(m_iEndAnimationID, false);

				// 공격 콜라이더를 끕니다.
				m_spAttackColliderComponent->SetActive(false);
				m_fSumDischargeDeltaSeconds = 0.0f;
				m_fSumLoopDeltaSeconds = 0.0f;
				m_bActionLoopCompleted = true;
			}
		}

		// 공격 끝
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
		// 공격 대기 시작
		if (!m_bStartCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iStartAnimation)
			{
				m_spMonster->ChangeEye(EPalEyeType::ANGRY);
				m_spAttackColliderComponent->SetActive(true);

				// 트레일을 켭니다.
				m_wpSoundManager.lock()->PlaySoundEx("Thuderbolt_01_Charge_348705042", 1.f, false);
				m_spMeshComponent->ChangeAnimation(m_iAction00AnimationID, false);
				// m_spMonster->OffAllNailTrails();
				// m_spMonster->OnRNailTrails(2.0f);
				m_bStartCompleted = true;
			}

			m_spMonster->LookAt(m_wpPlayer.lock()->GetPosition().value());
		}

		// 공격 중
		else if (!m_bActionCompleted)
		{
			// 일렉 판다 애니메이션을 재생합니다.
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();

			// 일렉 판다를 이동합니다.
			Vector3 vPlayerPosition = m_wpPlayer.lock()->GetPosition().value();
			Vector3 vBossPosition = m_spMonster->GetPosition().value();

			float fDistance = Vector3::Distance(vPlayerPosition, vBossPosition);
			if (10.0f <= fDistance)
			{
				m_spMonster->MoveForward(-tAnimationData.m_vRootmotionVelocity.y * 0.5f);
			}

			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction00AnimationID)
			{
				// 트레일을 켭니다.
				m_spMeshComponent->ChangeAnimation(m_iAction01AnimationID, false);
				// m_spMonster->OffAllNailTrails();
				// m_spMonster->OnLNailTrails(1.2f);
			}
			else if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAction01AnimationID)
			{
				// 트레일을 켭니다.
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

			// 일렉 판다를 회전합니다.
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

		// 공격 끝
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
		// 공격 대기 시작
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

		// 공격
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

		// 공격 중
		else if (!m_bActionLoopCompleted)
		{
			// 일렉 판다가 플레이어 방향을 바라봅니다.
			Vector3 vPlayerPosition = m_wpPlayer.lock()->GetPosition().value();
			m_spMonster->LookAt(vPlayerPosition);

			if (m_dqPlayerPositions.empty())
			{
				m_dqPlayerPositions.push_back(vPlayerPosition);
				m_dqPlayerPositions.push_back(vPlayerPosition);
			}

			// 플레이어 이전 위치를 갱신합니다.
			m_fSumArrowDeltaSeconds += _fDeltaSecond;
			if (m_fMaxArrowDeltaSeconds <= m_fSumArrowDeltaSeconds)
			{
				Vector3 vPrePosition = m_dqPlayerPositions.front();
				m_dqPlayerPositions.pop_front();
				m_dqPlayerPositions.push_back(vPlayerPosition);

				// 이곳에서 보스 위치에서 플레이어 이전 위치로 투사체를 발사합니다.
				// vPrePosition; <- 플레이어 과거 위치로 투사체 날리면 됨. 과거 위치는 0.6초마다 갱신됨.
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

		// 공격 끝
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
		// 공격 대기 시작
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

		// 공격 대기 중
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

		// 공격 시작
		else if (!m_bActionCompleted)
		{
			// 번개가 내리칠 위치를 구합니다.
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

		// 공격 중
		else if (!m_bActionLoopCompleted)
		{
			m_fSumLightningDeltaSeconds += _fDeltaSecond;
			if (!m_dqLightningPositions.empty() && (m_fMaxLightningDeltaSeconds <= m_fSumLightningDeltaSeconds))// 0.15초마다 생성
			{
				Vector3 vLightningPosition = m_dqLightningPositions.front();
				m_dqLightningPositions.pop_front();

				// 이곳에서 보스 위치에서 플레이어 위치로 일직선 5개 투사체를 발사합니다.
				// vLightningPosition <- 번개 생성 위치
				m_spMonster->ActiveEffectObject(EElecPandaEffectSlots::LIGHTING, vLightningPosition, Vector3(0.f), m_wpPlayer.lock());

				m_fSumLightningDeltaSeconds = 0.0f;
			}

			m_fSumLoopDeltaSeconds += _fDeltaSecond;
			if (m_fMaxActionLoop00DeltaSeconds <= m_fSumLoopDeltaSeconds)// m_fMaxActionLoop00DeltaSeconds가 최소 2초는 되어야 함!
			{
				m_dqLightningPositions.clear();

				m_spMeshComponent->ChangeAnimation(m_iEndAnimationID, false);
				m_fSumLoopDeltaSeconds = 0.0f;
				m_bActionLoopCompleted = true;
			}
		}

		// 공격 끝
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

	/* JSON파일로부터 객체를 생성합니다. */
	m_spRedRazor = CRedRazor::CreateFromJson(make_shared<FEffectGroupDesc>(tEffectGroupDesc), tEffectGroupDesc.m_wstrEffectGroupName);

	CHECKF(m_spRedRazor, L"Failed to CreateFromJson : CEffectManager");
	m_spRedRazor->SetPosition(Vector3(0.f, -1000.f, 0.f));
	m_spRedRazor->SetPlayer(m_wpPlayer.lock());
	m_spRedRazor->SetElecPanda(static_pointer_cast<CElecPanda>(m_spMonster));
	shared_ptr<CLevel> spPersistentLevel = CWorld::GetInstance()->GetPersistentLevel();
	spPersistentLevel->AddGameObject(m_spRedRazor);
}
