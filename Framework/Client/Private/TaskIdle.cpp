#include "pch.h"
#include "TaskIdle.h"

#include "MeshComponent.h"
#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskIdle> Client::CTaskIdle::Create(const std::shared_ptr<FTaskIdleDesc>& _spTaskIdleDesc)
{
	shared_ptr<CTaskIdle> spTaskNode = make_shared<CTaskIdle>();
	ENSUREF(spTaskNode->Initialize(_spTaskIdleDesc), L"Failed to initialize idle task node.");
	return spTaskNode;
}

HRESULT Client::CTaskIdle::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// idle task desc
	shared_ptr<FTaskIdleDesc> spTaskIdleDesc = static_pointer_cast<FTaskIdleDesc>(_spNodeDesc);
	m_iAnimationID	= spTaskIdleDesc->m_iAnimationID;
	m_fMaxIdleDeltaSeconds = spTaskIdleDesc->m_fMaxIdleDeltaSeconds;

	// etc
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskIdle::OnNodeInit(float _fDeltaSecond)
{
	m_vPrePosition = nullopt;
	m_fSumIdleDeltaSeconds = 0.0f;
}

void Client::CTaskIdle::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskIdle::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}
	m_spMonster->SetPosition(m_vPrePosition.value());

	m_fSumIdleDeltaSeconds += _fDeltaSecond;
	if (m_fMaxIdleDeltaSeconds <= m_fSumIdleDeltaSeconds)
	{
		m_vPrePosition = nullopt;
		m_fSumIdleDeltaSeconds = 0.0f;
		return EBTNodeResult::SUCCEEDED;
	}
	
	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskIdle::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CBTNode::Release();
}
