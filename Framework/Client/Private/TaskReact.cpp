#include "pch.h"
#include "TaskReact.h"

#include "MeshComponent.h"
#include "BehaviorTreeComponent.h"

#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskReact> Client::CTaskReact::Create(const std::shared_ptr<FTaskReactDesc>& _spTaskReactDesc)
{
	shared_ptr<CTaskReact> spTaskNode = make_shared<CTaskReact>();
	ENSUREF(spTaskNode->Initialize(_spTaskReactDesc), L"Failed to initialize react task node.");
	return spTaskNode;
}

HRESULT Client::CTaskReact::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// react task desc
	shared_ptr<FTaskReactDesc> spTaskReactDesc = static_pointer_cast<FTaskReactDesc>(_spNodeDesc);
	m_iEatAnimationID = spTaskReactDesc->m_iEatAnimationID;
	m_iPetAnimationID = spTaskReactDesc->m_iPetAnimationID;
	m_iSlaughterAnimationID = spTaskReactDesc->m_iSlaughterAnimationID;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();

	// 작업을 일시 정지합니다.
	m_spMonster->PauseToDoCommand();
	return S_OK;
}

void Client::CTaskReact::OnNodeInit(float _fDeltaSecond)
{
	m_eMonsterInteractType = static_cast<EMonsterInteractType>(m_spMonster->GetIntValue(EBlackboardIntValue::INT_INTERACT_TYPE));
	switch (m_eMonsterInteractType)
	{
	case EMonsterInteractType::EAT:

		// 표정을 변경합니다.
		m_spMonster->ChangeEye(EPalEyeType::SMIZED);
		m_spMonster->ChangeMouth(EPalMouthType::SMILE);

		m_spMeshComponent->ChangeAnimation(m_iEatAnimationID, false);
		break;

	case EMonsterInteractType::PET:

		// 표정을 변경합니다.
		m_spMonster->ChangeEye(EPalEyeType::SMIZED);
		m_spMonster->ChangeMouth(EPalMouthType::SMILE);

		m_spMeshComponent->ChangeAnimation(m_iPetAnimationID, false);
		break;

	case EMonsterInteractType::SLAUGHTER:

		// 표정을 변경합니다.
		m_spMonster->ChangeEye(EPalEyeType::FAINT);
		m_spMonster->ChangeMouth(EPalMouthType::DAMAGED);

		m_spMeshComponent->ChangeAnimation(m_iSlaughterAnimationID, false);
		break;

	default:
		CHECKF(false, L"Monster react type is not desginated.");
		break;
	}

	m_vPrePosition = nullopt;
	m_fSumReactDeltaSeconds = 0.0f;
}

void Client::CTaskReact::OnNodeEnter(float _fDeltaSecond)
{
}

EBTNodeResult Client::CTaskReact::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}
	m_spMonster->SetPosition(m_vPrePosition.value());

	switch (m_eMonsterInteractType)
	{
	case EMonsterInteractType::EAT:
	{
		// 허기 게이지를 증가시킵니다.
		m_spMonster->IncreaseHungerGauge(_fDeltaSecond);

		m_fSumReactDeltaSeconds += _fDeltaSecond;
		if (m_fMaxReactDeltaSeconds <= m_fSumReactDeltaSeconds)
		{
			m_fSumReactDeltaSeconds = 0.0f;
			m_eMonsterInteractType = EMonsterInteractType::ENUM_END;
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_HUNGER, false);
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_FEEDED, true);
			m_spMonster->ModifyIntValue(EBlackboardIntValue::INT_INTERACT_TYPE, (int32)EMonsterInteractType::ENUM_END);

			m_vPrePosition = nullopt;
			return EBTNodeResult::SUCCEEDED;
		}
	}
	break;

	case EMonsterInteractType::PET:
	{
		m_fSumReactDeltaSeconds += _fDeltaSecond;
		if (m_fMaxReactDeltaSeconds <= m_fSumReactDeltaSeconds)
		{
			m_fSumReactDeltaSeconds = 0.0f;
			m_eMonsterInteractType = EMonsterInteractType::ENUM_END;
			m_spMonster->ModifyIntValue(EBlackboardIntValue::INT_INTERACT_TYPE, (int32)EMonsterInteractType::ENUM_END);

			m_vPrePosition = nullopt;
			return EBTNodeResult::SUCCEEDED;
		}
	}
	break;

	case EMonsterInteractType::SLAUGHTER:
	{
		m_fSumReactDeltaSeconds += _fDeltaSecond;
		if (m_fMaxReactDeltaSeconds <= m_fSumReactDeltaSeconds)
		{
			m_fSumReactDeltaSeconds = 0.0f;
			m_eMonsterInteractType = EMonsterInteractType::ENUM_END;
			m_spMonster->ModifyIntValue(EBlackboardIntValue::INT_INTERACT_TYPE, (int32)EMonsterInteractType::ENUM_END);
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_DEAD, true);

			m_vPrePosition = nullopt;
			m_spMonster->GetBehaviorTreeComponent()->ResetBehindBTNodeResult();


			return EBTNodeResult::SUCCEEDED;
		}
	}
	break;

	default:
		CHECKF(false, L"Monster react type is not desginated.");
		break;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskReact::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CBTNode::Release();
}
