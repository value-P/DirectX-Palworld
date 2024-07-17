// default
#include "pch.h"
#include "TaskMove.h"

// component
#include "MeshComponent.h"

// engine
#include "Model.h"
#include "Panel.h"

// utility
#include "MathUtility.h"

// gameobject
#include "Architecture.h"
#include "Monster.h"
#include "PalBed.h"

#include "ClientLoop.h"

#include "ToDoSystem.h"

std::shared_ptr<CTaskMove> Client::CTaskMove::Create(const std::shared_ptr<FTaskMoveDesc>& _spTaskMoveDesc)
{
	shared_ptr<CTaskMove> spTaskNode = make_shared<CTaskMove>();
	ENSUREF(spTaskNode->Initialize(_spTaskMoveDesc), L"Failed to initialize move task node.");
	return spTaskNode;
}

HRESULT Client::CTaskMove::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// move task desc
	shared_ptr<FTaskMoveDesc> spTaskMoveDesc = static_pointer_cast<FTaskMoveDesc>(_spNodeDesc);
	m_iAnimationID		= spTaskMoveDesc->m_iAnimationID;
	m_vSpawnPosition	= spTaskMoveDesc->m_vSpawnPosition;

	// etc
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_eMonsterType		= m_spMonster->GetMonsterType();
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskMove::OnNodeInit(float _fDeltaSecond)
{
	m_bCreep = false;

	// 자러 이동합니다.
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_SLEEP) && m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_INTERACTED))
	{
		// 플레이어가 이동하면 안 됩니다.
		m_wpTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::PLAYER_TARGET);
		m_fAcceptableRadius = m_spMonster->GetFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS);
	}
	
	else if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_SLEEP))
	{
		m_bCreep = true;

		// 표정을 변경합니다.
		m_spMonster->ChangeEye(EPalEyeType::TIRED);
		m_spMonster->ChangeMouth(EPalMouthType::DAMAGED);
		m_spMonster->GrabTool(EItemType::CONSTRUCT_HAMMER, false);

		m_wpTarget = m_spMonster->FindArchitecture(EArchitectureType::PAL_BED);
		m_spMonster->OccupyPalBed(dynamic_pointer_cast<CPalBed>(m_wpTarget.lock()));
		m_fAcceptableRadius = 0.5f;
	}

	// 작업을 하러 이동합니다.
	else if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_TODO_START))
	{
		m_wpTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET);

		m_vDstPosition = m_wpTarget.lock()->GetPosition().value();
		m_fAcceptableRadius = m_spMonster->GetFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS);

		if (EToDoType::ENUM_END != m_spMonster->GetToDoCommand()->m_eToDoType)
		{
			// 작업 UI를 켭니다.
			m_spMonster->SetActivePalWorkPanel(true);
			m_spMonster->SetPalWorkPanelImage(m_spMonster->GetToDoCommand()->m_eToDoType);
			m_spMonster->GetPalWorkPanel().first->SetPivotWorldPoint(m_spMonster->GetPosition().value() + m_spMonster->GetUISize());
		}
	}

	// 플레이어와 상호작용하러 이동합니다.
	else if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_INTERACTED))
	{
		// 플레이어가 이동하면 안 됩니다.
		m_wpTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::PLAYER_TARGET);
		m_fAcceptableRadius = m_spMonster->GetFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS);
	}

	// 파트너 스킬을 사용하려 이동합니다.
	else if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_PARTNER_SKILL))
	{
		m_wpTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::PLAYER_TARGET);
		m_fAcceptableRadius = m_spMonster->GetFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS);
	}

	// 공격 후 무작위로 이동합니다.
	else if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_AGGRO))
	{
		float fX = 0.0f;
		float fZ = 0.0f;

		if (CMathUtility::GetRandomBool(0.5f)) { fX = CMathUtility::GetRandomFloat(-6.0f, -4.0f); }
		else { fX = CMathUtility::GetRandomFloat(4.0f, 6.0f); }

		if (CMathUtility::GetRandomBool(0.5f)) { fZ = CMathUtility::GetRandomFloat(-6.0f, -4.0f); }
		else { fZ = CMathUtility::GetRandomFloat(4.0f, 6.0f); }

		Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
		vMonsterPosition.x += fX;
		vMonsterPosition.z += fZ;

		m_vDstPosition = vMonsterPosition;
		m_fAcceptableRadius = 1.0f;
	}

	// 밥을 먹으러 이동합니다.
	else if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_HUNGER))
	{
		m_wpTarget = m_spMonster->FindArchitecture(EArchitectureType::FEEDING_BOX);
		m_fAcceptableRadius = 1.0f;
	}

	// 무작위로 이동합니다.
	else
	{
		m_bCreep = true;

		float fX = 0.0f;
		float fZ = 0.0f;

		float fMinX = 0.0f, fMaxX = 0.0f;
		float fMinZ = 0.0f, fMaxZ = 0.0f;
		switch (m_spMonster->GetMonsterSizeType())
		{
		case EMonsterSizeType::SMALL:
			fMinX = 4.0f, fMaxX = 6.0f;
			fMinZ = 4.0f, fMaxZ = 6.0f;
			break;

		case EMonsterSizeType::MIDUM:
			fMinX = 8.0f, fMaxX = 12.0f;
			fMinZ = 8.0f, fMaxZ = 12.0f;
			break;

		case EMonsterSizeType::LARGE:
			fMinX = 12.0f, fMaxX = 18.0f;
			fMinZ = 12.0f, fMaxZ = 18.0f;
			break;
		}

		if (CMathUtility::GetRandomBool(0.5f)) { fX = CMathUtility::GetRandomFloat(-fMaxX, -fMinX); }
		else { fX = CMathUtility::GetRandomFloat(fMinX, fMaxX); }

		if (CMathUtility::GetRandomBool(0.5f)) { fZ = CMathUtility::GetRandomFloat(-fMaxZ, -fMinZ); }
		else { fZ = CMathUtility::GetRandomFloat(fMinZ, fMaxZ); }

		Vector3 vSpawnPosition = m_vSpawnPosition;
		vSpawnPosition.x += fX;
		vSpawnPosition.z += fZ;

		m_vDstPosition = vSpawnPosition;
		m_fAcceptableRadius = 2.0f;
	}

	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_fSumLerpDeltaSeconds = 0.0f;
}

void Client::CTaskMove::OnNodeEnter(float _fDeltaSecond)
{
	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskMove::ExecuteTaskNode(float _fDeltaSecond)
{
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_DEAD)) { return EBTNodeResult::FAILED; }

	float fSpeed = 0.0f;

	if (m_bCreep)
	{
		fSpeed = CClientLoop::s_spClientDataSheet->k_mapPalDefaultCreepSpeeds[m_eMonsterType];
	}
	else
	{
		fSpeed = CClientLoop::s_spClientDataSheet->k_mapPalDefaultMoveSpeeds[m_eMonsterType];
	}


	// 어그로가 끌리지 않고 자러 이동하는 경우
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_SLEEP) &&
		!m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_AGGRO) &&
		!m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_INTERACTED))
	{
		// 침대가 있는 경우
		if (!m_wpTarget.expired())
		{
			m_vDstPosition = m_wpTarget.lock()->GetPosition().value();
		}
		// 침대가 없는 경우
		else
		{
			m_fSumLerpDeltaSeconds = 0.0f;
			return EBTNodeResult::SUCCEEDED;
		}
	}

	if (
		m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_PARTNER_SKILL) ||	// 파트너 스킬을 사용하거나 
		m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_INTERACTED) ||		// 플레이어와 상호작용하거나
		m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_HUNGER)			// 배가 고픈 경우
		)
	{
		m_vDstPosition = m_wpTarget.lock()->GetPosition().value();
	}

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	vMonsterPosition.y = 0.0f;
	m_vDstPosition.y = 0.0f;

	float fDistance = Vector3::Distance(vMonsterPosition, m_vDstPosition);
	if (fDistance <= m_fAcceptableRadius)
	{
		if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_INTERACTED))
		{
			m_spMonster->SetCanInteract(true);
		}

		m_fSumLerpDeltaSeconds = 0.0f;
		return EBTNodeResult::SUCCEEDED;
	}

	Vector3 vDirection = m_vDstPosition - vMonsterPosition;
	vDirection.Normalize();

	m_fSumLerpDeltaSeconds += _fDeltaSecond;
	if (m_fSumLerpDeltaSeconds < m_fMaxLerpDeltaSeconds)
	{
		Vector3 vForward = m_spMonster->GetForwardVector().value();
		vForward.Normalize();

		float fRatio = m_fSumLerpDeltaSeconds / m_fMaxLerpDeltaSeconds;
		Vector3 vLerpDirection = CMathUtility::EaseOutCirc(vForward, vDirection, fRatio);
		m_spMonster->LookAt(vMonsterPosition + vLerpDirection);
	}
	else
	{
		m_spMonster->LookAt(vMonsterPosition + vDirection);
	}

	m_spMonster->MoveForward(fSpeed);

	// 거점 팰의 작업 UI 위치를 갱신합니다.
	if (m_spMonster->GetMonsterAIType() == EMonsterAIType::SLAVE_PAL)
	{
		if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_TODO_START))
		{
			if (EToDoType::ENUM_END != m_spMonster->GetToDoCommand()->m_eToDoType)
			{
				if (m_spMonster->GetPalWorkPanel().first->GetActive())
				{
					m_spMonster->GetPalWorkPanel().first->SetPivotWorldPoint(m_spMonster->GetPosition().value() + m_spMonster->GetUISize());
				}
			}
		}
	}
	
	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskMove::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;
	m_wpTarget.reset();
	CTaskNode::Release();
}
