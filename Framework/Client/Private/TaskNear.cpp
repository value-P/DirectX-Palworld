#include "pch.h"
#include "TaskNear.h"

// manager
#include "EngineModule.h"
#include "EffectManager.h"
#include "SoundManager.h"

// component
#include "BehaviorTreeComponent.h"
#include "MeshComponent.h"

// gameobject
#include "Monster.h"
#include "Player.h"

// engine
#include "Model.h"

// utility
#include "MathUtility.h"

std::shared_ptr<CTaskNear> Client::CTaskNear::Create(const std::shared_ptr<FTaskNearDesc>& _spTaskNearDesc)
{
	const shared_ptr<CTaskNear> spTaskNode = make_shared<CTaskNear>();
	ENSUREF(spTaskNode->Initialize(_spTaskNearDesc), L"Failed to initialize near task node.");
	return spTaskNode;
}

HRESULT Client::CTaskNear::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// near task desc
	shared_ptr<FTaskNearDesc> spTaskNearDesc = static_pointer_cast<FTaskNearDesc>(_spNodeDesc);
	m_iAnimationID		= spTaskNearDesc->m_iAnimationID;
	m_spPlayer			= spTaskNearDesc->m_spPlayer;
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	
	// etc
	m_wpEffectManager	= CEffectManager::GetInstance();
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskNear::OnNodeInit(float _fDeltaSecond)
{
	m_vPrePosition = nullopt;
	m_vTargetDirection = std::nullopt;
	m_fSumLerpDeltaSeconds = 0.0f;
}

void Client::CTaskNear::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_CAPTURED)) { return; }

	shared_ptr<CSoundManager> spSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	EMonsterType eMonsterType = m_spMonster->GetMonsterType();
	switch (eMonsterType)
	{
	case Client::EMonsterType::BERRY_GOAT:		spSoundManager->PlaySoundEx("VO_BerryGoat_01_Normal_3_59043235"		, 0.6f);	break;
	case Client::EMonsterType::BOAR:			spSoundManager->PlaySoundEx("VO_Boar_01_Normal_164867547"			, 0.6f);	break;
	case Client::EMonsterType::DREAM_DEMON:		spSoundManager->PlaySoundEx("VO_DreamDemon_01_Normal_2_559150555"	, 0.6f);	break;
	case Client::EMonsterType::GRASS_MAMMOTH:	spSoundManager->PlaySoundEx("VO_GrassMammoth_01_Normal_2_616021679"	, 0.6f);	break;
	case Client::EMonsterType::HEDGE_HOG:		spSoundManager->PlaySoundEx("VO_Hedgehog_01_Normal_2_246580790"		, 0.6f);	break;
	case Client::EMonsterType::HEDGE_HOG_ICE:	spSoundManager->PlaySoundEx("VO_Hedgehog_01_Normal_2_246580790"		, 0.6f);	break;
	case Client::EMonsterType::KITSUNEBI:		spSoundManager->PlaySoundEx("VO_Kitsunebi_01_Normal_541210977"		, 0.6f);	break;
	case Client::EMonsterType::PENGUIN:			spSoundManager->PlaySoundEx("VO_Penguin_01_Normal_291409068"		, 0.6f);	break;
	case Client::EMonsterType::PINK_CAT:		spSoundManager->PlaySoundEx("VO_PinkCat_01_Normal_02_717847191"		, 0.6f);	break;
	case Client::EMonsterType::SHEEP_BALL:		spSoundManager->PlaySoundEx("VO_SheepBall_01_Normal_PT1_2_744521170", 0.6f);	break;
	case Client::EMonsterType::WOOL_FOX:		spSoundManager->PlaySoundEx("VO_WoolFox_01_Normal_427540710"		, 0.6f);	break;
	default: break;
	}


	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
	m_wpBehaviorTreeComponent.lock()->ResetBehindBTNodeResult();

	Vector3 vScale = m_spMonster->GetScale().value();

	switch (m_spMonster->GetMonsterSizeType())
	{
	case EMonsterSizeType::LARGE:
		vScale.y *= 3.0f;
		break;

	case EMonsterSizeType::MIDUM:
		vScale.y *= 2.5f;
		break;

	case EMonsterSizeType::SMALL:
		m_spMonster->GetMonsterType() == EMonsterType::DREAM_DEMON ?  vScale.y *= 2.0f : vScale.y *= 1.2f;
		m_spMonster->GetMonsterType() == EMonsterType::BOAR ? vScale.y *= 1.8f : vScale.y *= 1.2f;
		break;

	default:
		CHECKF(false, L"Monster personality type is not designated.");
		break;
	}

	switch (m_spMonster->GetMonsterPersonalityType())
	{
	case EMonsterPersonalityType::PEACEFUL:
		// 놀라는 표시는 팰 우측 상단에 위치시키기 어려워 느낌표로 대체했습니다.
		m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_Notice00", m_spMonster, Vector3(0.0f, vScale.y, 0.0f));
		break;

	case EMonsterPersonalityType::AGGRESSIVE:
		m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_Notice01", m_spMonster, Vector3(0.0f, vScale.y, 0.0f));
		break;

	case EMonsterPersonalityType::STUPID:
		m_wpEffectManager.lock()->ActivateEffectCombined(L"Group_Notice00", m_spMonster, Vector3(0.0f, vScale.y, 0.0f));
		break;

	default:
		CHECKF(false, L"Monster personality type is not designated.");
		break;
	}
}

EBTNodeResult Client::CTaskNear::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}
	m_spMonster->SetPosition(m_vPrePosition.value());

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	Vector3 vPlayerPosition = m_spPlayer->GetPosition().value();

	Vector3 vDirection = vPlayerPosition - vMonsterPosition;
	vDirection.Normalize();

	// 거리가 먼 경우, 실패합니다.
	float fDistance = Vector3::Distance(vPlayerPosition, vMonsterPosition);
	if (m_fAcceptableRadius < fDistance)
	{
		m_vTargetDirection = std::nullopt;
		m_fSumLerpDeltaSeconds = 0.0f;
		return EBTNodeResult::FAILED;
	}

	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_CAPTURED))
	{
		m_vTargetDirection = std::nullopt;
		m_fSumLerpDeltaSeconds = 0.0f;
		return EBTNodeResult::FAILED;
	}

	EMonsterPersonalityType ePersonalityType = m_spMonster->GetMonsterPersonalityType();
	switch (ePersonalityType)
	{
	// 공격적인
	case EMonsterPersonalityType::AGGRESSIVE:
	{
		m_fSumLerpDeltaSeconds += _fDeltaSecond;
		if (m_fSumLerpDeltaSeconds < m_fMaxLerpDeltaSeconds)
		{
			if (!m_vTargetDirection.has_value())
			{
				m_vTargetDirection = m_spMonster->GetForwardVector().value();
				m_vTargetDirection.value().Normalize();
			}

			float fRatio = m_fSumLerpDeltaSeconds / m_fMaxLerpDeltaSeconds;

			Vector3 vLerpDirection = CMathUtility::EaseOutCirc(m_vTargetDirection.value(), vDirection, fRatio);
			m_spMonster->LookAt(vMonsterPosition + vLerpDirection);
		}
		else
		{
			m_vTargetDirection = nullopt;
			m_spMonster->ModifyGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET, m_spPlayer);
			ENSUREF(m_spMonster->PariticipateInCombat(), L"Failed to participate in combat.");
			return EBTNodeResult::SUCCEEDED;
		}
	}
	break;

	// 평화로운(소심한)
	case EMonsterPersonalityType::PEACEFUL:
	{
		m_fSumLerpDeltaSeconds += _fDeltaSecond;
		if (m_fSumLerpDeltaSeconds < m_fMaxLerpDeltaSeconds)
		{
			if (!m_vTargetDirection.has_value())
			{
				m_vTargetDirection = m_spMonster->GetForwardVector().value();
				m_vTargetDirection.value().Normalize();
			}

			float fRatio = m_fSumLerpDeltaSeconds / m_fMaxLerpDeltaSeconds;

			Vector3 vLerpDirection = CMathUtility::EaseOutCirc(m_vTargetDirection.value(), vDirection, fRatio);
			m_spMonster->LookAt(vMonsterPosition + vLerpDirection);
		}
		else
		{
			if (m_spMonster->GetMonsterType() == EMonsterType::HEDGE_HOG_ICE)
			{
				// 플레이어가 횃불을 들고 있지 않은 경우
				if (!m_spPlayer->IsHandTorchHeld())
				{
					m_vTargetDirection = nullopt;
					m_spMonster->LookAt(vMonsterPosition + vDirection);
					return EBTNodeResult::IN_PROGRESS;
				}
			}
			
			m_vTargetDirection = nullopt;
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_RUNAWAY, true);
			return EBTNodeResult::SUCCEEDED;
		}
	}
	break;

	// 멍청한
	case EMonsterPersonalityType::STUPID:
	{
		m_fSumLerpDeltaSeconds += _fDeltaSecond;
		if (m_fSumLerpDeltaSeconds < m_fMaxLerpDeltaSeconds)
		{
			if (!m_vTargetDirection.has_value())
			{
				m_vTargetDirection = m_spMonster->GetForwardVector().value();
				m_vTargetDirection.value().Normalize();
			}

			float fRatio = m_fSumLerpDeltaSeconds / m_fMaxLerpDeltaSeconds;

			Vector3 vLerpDirection = CMathUtility::EaseOutCirc(m_vTargetDirection.value(), vDirection, fRatio);
			m_spMonster->LookAt(vMonsterPosition + vLerpDirection);
		}
		else
		{
			m_vTargetDirection = nullopt;
			m_spMonster->LookAt(vMonsterPosition + vDirection);
		}
	}
	break;

	default:
		CHECKF(false, L"Monster personality is not designated.");
		break;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskNear::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CBTNode::Release();
}
