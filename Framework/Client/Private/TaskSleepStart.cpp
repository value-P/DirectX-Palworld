#include "pch.h"
#include "TaskSleepStart.h"

#include "ClientModule.h"
#include "EffectManager.h"

#include "Monster.h"
#include "MeshComponent.h"
#include "Model.h"


std::shared_ptr<CTaskSleepStart> Client::CTaskSleepStart::Create(const std::shared_ptr<FTaskSleepStartDesc>& _spTaskSleepStartDesc)
{
	shared_ptr<CTaskSleepStart> spTaskNode = make_shared<CTaskSleepStart>();
	ENSUREF(spTaskNode->Initialize(_spTaskSleepStartDesc), L"Failed to initialize sleep start task node.");
	return spTaskNode;
}

HRESULT Client::CTaskSleepStart::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// sleep start task desc
	shared_ptr<FTaskSleepStartDesc> spTaskSleepDesc = static_pointer_cast<FTaskSleepStartDesc>(_spNodeDesc);
	m_iAnimationID		= spTaskSleepDesc->m_iAnimationID;
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskSleepStart::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::TIRED);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskSleepStart::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}
	m_spMonster->SetPosition(m_vPrePosition.value());

	const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
	if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAnimationID)
	{
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskSleepStart::OnNodeInit(float _fDeltaSecond)
{
	m_vPrePosition = nullopt;
}

void Client::CTaskSleepStart::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
