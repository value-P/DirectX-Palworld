#include "pch.h"
#include "TaskAttackStart.h"

#include "Monster.h"
#include "MeshComponent.h"
#include "Model.h"

std::shared_ptr<CTaskAttackStart> Client::CTaskAttackStart::Create(const std::shared_ptr<FTaskAttackStartDesc>& _spTaskAttackStartDesc)
{
	shared_ptr<CTaskAttackStart> spTaskNode = make_shared<CTaskAttackStart>();
	ENSUREF(spTaskNode->Initialize(_spTaskAttackStartDesc), L"Failed to initialize attack start task node.");
	return spTaskNode;
}

HRESULT Client::CTaskAttackStart::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// attack start desc
	shared_ptr<FTaskAttackStartDesc> spTaskAttackStartDesc = static_pointer_cast<FTaskAttackStartDesc>(_spNodeDesc);
	m_iAnimationID		= spTaskAttackStartDesc->m_iAnimationID;
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskAttackStart::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskAttackStart::ExecuteTaskNode(float _fDeltaSecond)
{
	const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
	if (tAnimationData.m_tBaseAnimData.m_bLoopEnd
		&& tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAnimationID)
	{
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskAttackStart::OnNodeInit(float _fDeltaSecond)
{
}

void Client::CTaskAttackStart::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
