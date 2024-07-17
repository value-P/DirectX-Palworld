#include "pch.h"
#include "TaskSleepLoop.h"

#include "Monster.h"
#include "MeshComponent.h"
#include "Model.h"

std::shared_ptr<CTaskSleepLoop> Client::CTaskSleepLoop::Create(const std::shared_ptr<FTaskSleepLoopDesc>& _spTaskSleepLoopDesc)
{
	shared_ptr<CTaskSleepLoop> spTaskNode = make_shared<CTaskSleepLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskSleepLoopDesc), L"Failed to initialize sleep loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskSleepLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task node desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// sleep loop task desc
	shared_ptr<FTaskSleepLoopDesc> spTaskSleepDesc = static_pointer_cast<FTaskSleepLoopDesc>(_spNodeDesc);
	m_iAnimationID = spTaskSleepDesc->m_iAnimationID;
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskSleepLoop::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::CLOSED);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskSleepLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}
	m_spMonster->SetPosition(m_vPrePosition.value());

	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_DAY) == true)
	{
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskSleepLoop::OnNodeInit(float _fDeltaSecond)
{
	m_vPrePosition = nullopt;
}

void Client::CTaskSleepLoop::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
