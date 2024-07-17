// default
#include "pch.h"
#include "TaskJumpEnd.h"

// component
#include "MeshComponent.h"

// game object
#include "Monster.h"
#include "Model.h"

std::shared_ptr<CTaskJumpEnd> Client::CTaskJumpEnd::Create(const std::shared_ptr<FTaskJumpEndDesc>& _spTaskJumpEndDesc)
{
	shared_ptr<CTaskJumpEnd> spTaskNode = make_shared<CTaskJumpEnd>();
	ENSUREF(spTaskNode->Initialize(_spTaskJumpEndDesc), L"Failed to initialize jump end task node.");
	return spTaskNode;
}

HRESULT Client::CTaskJumpEnd::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// jump task desc
	shared_ptr<FTaskJumpEndDesc> spTaskJumpEndDesc = static_pointer_cast<FTaskJumpEndDesc>(_spNodeDesc);
	m_iAnimationID = spTaskJumpEndDesc->m_iAnimationID;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskJumpEnd::OnNodeInit(float _fDeltaSecond)
{
	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

void Client::CTaskJumpEnd::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);
}

EBTNodeResult Client::CTaskJumpEnd::ExecuteTaskNode(float _fDeltaSecond)
{
	FModelAnimData tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
	if (tAnimationData.m_tBaseAnimData.m_bLoopEnd
		&& tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAnimationID)
	{
		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_JUMP, false);
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskJumpEnd::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
