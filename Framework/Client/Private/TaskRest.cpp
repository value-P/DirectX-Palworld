#include "pch.h"
#include "TaskRest.h"

#include "Monster.h"
#include "MeshComponent.h"
#include "Model.h"

std::shared_ptr<CTaskRest> Client::CTaskRest::Create(const std::shared_ptr<FTaskRestDesc>& _spTaskRestDesc)
{
	shared_ptr<CTaskRest> spTaskNode = make_shared<CTaskRest>();
	ENSUREF(spTaskNode->Initialize(_spTaskRestDesc), L"Failed to initialize rest task node.");
	return spTaskNode;
}

HRESULT Client::CTaskRest::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	shared_ptr<FTaskRestDesc> spTaskRestDesc = static_pointer_cast<FTaskRestDesc>(_spNodeDesc);
	m_iAnimationID = spTaskRestDesc->m_iAnimationID;
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskRest::OnNodeInit(float _fDeltaSecond)
{
	m_fSumRestDeltaSeconds = 0.0f;
}

void Client::CTaskRest::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskRest::ExecuteTaskNode(float _fDeltaSecond)
{
	m_fSumRestDeltaSeconds += _fDeltaSecond;
	if (m_fMaxRestDeltaSeconds <= m_fSumRestDeltaSeconds)
	{
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskRest::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
