#include "pch.h"
#include "TaskSleepEnd.h"

#include "MeshComponent.h"
#include "BehaviorTreeComponent.h"

#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskSleepEnd> Client::CTaskSleepEnd::Create(const std::shared_ptr<FTaskSleepEndDesc>& _spTaskSleepEndDesc)
{
	shared_ptr<CTaskSleepEnd> spTaskNode = make_shared<CTaskSleepEnd>();
	ENSUREF(spTaskNode->Initialize(_spTaskSleepEndDesc), L"Failed to initialize sleep end task node.");
	return spTaskNode;
}

HRESULT Client::CTaskSleepEnd::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task node desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }
	
	// sleep end task desc
	shared_ptr<FTaskSleepEndDesc> spTaskSleepEndDesc = static_pointer_cast<FTaskSleepEndDesc>(_spNodeDesc);
	m_iAnimationID = spTaskSleepEndDesc->m_iAnimationID;

	// etc
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskSleepEnd::OnNodeInit(float _fDeltaSecond)
{
	m_vPrePosition = nullopt;
}

void Client::CTaskSleepEnd::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::TIRED);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_wpBehaviorTreeComponent.lock()->ResetBehindBTNodeResult();
	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskSleepEnd::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}
	m_spMonster->SetPosition(m_vPrePosition.value());

	const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
	if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAnimationID)
	{
		m_vPrePosition = nullopt;

		// 침대를 비웁니다.
		m_spMonster->VacantPalBed();

		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_SLEEP, false);
		m_spMonster->SetMonsterToDoState(EMonsterToDoState::NO_WORK);
		m_spMonster->RequestToDoCommand();
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskSleepEnd::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
