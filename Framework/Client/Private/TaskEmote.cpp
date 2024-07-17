#include "pch.h"
#include "TaskEmote.h"

#include "BehaviorTreeComponent.h"
#include "MeshComponent.h"
#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskEmote> Client::CTaskEmote::Create(const std::shared_ptr<FTaskEmoteDesc>& _spTaskEmoteDesc)
{
	shared_ptr<CTaskEmote> spTaskNode = make_shared<CTaskEmote>();
	ENSUREF(spTaskNode->Initialize(_spTaskEmoteDesc), L"Failed to initialize emote task node.");
	return spTaskNode;
}

HRESULT Client::CTaskEmote::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// react task desc
	shared_ptr<FTaskEmoteDesc> spTaskEmoteDesc = static_pointer_cast<FTaskEmoteDesc>(_spNodeDesc);
	m_iAnimationID = spTaskEmoteDesc->m_iAnimationID;
	m_fMaxEmoteDeltaSeconds = spTaskEmoteDesc->m_fMaxEmoteDeltaSeconds;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskEmote::OnNodeInit(float _fDeltaSecond)
{
	m_vPrePosition = nullopt;
	m_fSumEmoteDeltaSeconds = 0.0f;

	// 작업을 완료한 경우
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_TODO_FINISH) == true)
	{
		// work ui를 꺼줍니다.
		m_spMonster->SetActivePalWorkPanel(false);
	}
}

void Client::CTaskEmote::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::SMIZED);
	m_spMonster->ChangeMouth(EPalMouthType::SMILE);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskEmote::ExecuteTaskNode(float _fDeltaSecond)
{
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_DEAD))
	{
		m_vPrePosition = nullopt;
		m_fSumEmoteDeltaSeconds = 0.0f;
		return EBTNodeResult::FAILED;
	}

	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}
	m_spMonster->SetPosition(m_vPrePosition.value());

	m_fSumEmoteDeltaSeconds += _fDeltaSecond;
	if (m_fMaxEmoteDeltaSeconds <= m_fSumEmoteDeltaSeconds)
	{
		m_fSumEmoteDeltaSeconds = 0.0f;

		// 상호작용을 완료한 경우
		if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_INTERACTED) == true)
		{
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_INTERACTED, false);
			m_spMonster->SetCanInteract(false);

			// 작업을 재요청합니다.
			m_spMonster->RequestToDoCommand();
		}

		// 밥을 다 먹은 경우
		if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_FEEDED) == true)
		{
			// 작업을 재요청합니다.
			m_spMonster->RequestToDoCommand();
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_FEEDED, false);
		}

		// 작업을 완료한 경우
		if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_TODO_FINISH) == true)
		{
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_START, false);
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_FINISH, false);
			m_spMonster->FinishToDoCommand();
		}

		m_wpBehaviorTreeComponent.lock()->ResetBehindBTNodeResult();
		m_vPrePosition = nullopt;
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskEmote::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CBTNode::Release();
}
