#include "pch.h"
#include "TaskHoldPosition.h"

// client
#include "ClientLoop.h"

// component
#include "MeshComponent.h"
#include "ColliderComponent.h"

// gameobject
#include "Monster.h"
#include "Player.h"
#include "Model.h"

// utility
#include "MathUtility.h"

std::shared_ptr<CTaskHoldPosition> Client::CTaskHoldPosition::Create(const std::shared_ptr<FTaskHoldPositionDesc>& _spTaskHoldPositionDesc)
{
	shared_ptr<CTaskHoldPosition> spTaskNode = make_shared<CTaskHoldPosition>();
	ENSUREF(spTaskNode->Initialize(_spTaskHoldPositionDesc), L"Failed to initialize hold position task node.");
	return spTaskNode;
}

HRESULT Client::CTaskHoldPosition::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// look at task desc
	shared_ptr<FTaskHoldPositionDesc> spTaskDesc = static_pointer_cast<FTaskHoldPositionDesc>(_spNodeDesc);
	m_iAnimationID = spTaskDesc->m_iAnimationID;
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());

	// etc
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskHoldPosition::OnNodeInit(float _fDeltaSecond)
{
}

void Client::CTaskHoldPosition::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskHoldPosition::ExecuteTaskNode(float _fDeltaSecond)
{
	m_spMonster->SetLinearVelocity({ 0.0f, 0.0f, 0.0f });
	m_spMonster->SetAngularVelocity({ 0.0f, 0.0f, 0.0f });

	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}

	m_spMonster->SetPosition(m_vPrePosition.value());
	
	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskHoldPosition::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;

	CBTNode::Release();
}
