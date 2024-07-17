// default
#include "pch.h"
#include "TaskJumpLoop.h"

// component
#include "MeshComponent.h"

// game object
#include "Monster.h"

std::shared_ptr<CTaskJumpLoop> Client::CTaskJumpLoop::Create(const std::shared_ptr<FTaskJumpLoopDesc>& _spTaskAttackLoopDesc)
{
	shared_ptr<CTaskJumpLoop> spTaskNode = make_shared<CTaskJumpLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskAttackLoopDesc), L"Failed to initialize jump loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskJumpLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// jump task desc
	shared_ptr<FTaskJumpLoopDesc> spTaskJumpLoopDesc = static_pointer_cast<FTaskJumpLoopDesc>(_spNodeDesc);
	m_iAnimationID = spTaskJumpLoopDesc->m_iAnimationID;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskJumpLoop::OnNodeInit(float _fDeltaSecond)
{
	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

void Client::CTaskJumpLoop::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DAMAGED);
	m_spMonster->ChangeMouth(EPalMouthType::SMILE);
}

EBTNodeResult Client::CTaskJumpLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	if (m_spMonster->IsJump() == false)
	{
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskJumpLoop::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
