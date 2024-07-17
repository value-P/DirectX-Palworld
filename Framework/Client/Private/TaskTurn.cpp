#include "pch.h"
#include "TaskTurn.h"

// client
#include "ClientLoop.h"

// component
#include "MeshComponent.h"

// gameobject
#include "Monster.h"
#include "Player.h"
#include "Model.h"

// utility
#include "MathUtility.h"

std::shared_ptr<CTaskTurn> Client::CTaskTurn::Create(const std::shared_ptr<FTaskTurnDesc>& _spTaskTurnDesc)
{
	shared_ptr<CTaskTurn> spTaskNode = make_shared<CTaskTurn>();
	ENSUREF(spTaskNode->Initialize(_spTaskTurnDesc), L"Failed to initialize turn task node.");
	return spTaskNode;
}

HRESULT Client::CTaskTurn::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// look at task desc
	shared_ptr<FTaskTurnDesc> spTaskDesc = static_pointer_cast<FTaskTurnDesc>(_spNodeDesc);
	m_iAnimationID = spTaskDesc->m_iAnimationID;
	m_wpTarget = spTaskDesc->m_spTarget;
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());

	// etc
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskTurn::OnNodeInit(float _fDeltaSecond)
{
	m_vForward = m_spMonster->GetForwardVector().value();
	m_vForward.value().Normalize();
	
	m_vPrePosition = nullopt;
	m_fSumTurnDeltaSeconds = 0.0f;
}

void Client::CTaskTurn::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskTurn::ExecuteTaskNode(float _fDeltaSecond)
{
	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();

	Vector3 vDirection = vTargetPosition - vMonsterPosition;
	vDirection.Normalize();

	m_fSumTurnDeltaSeconds += _fDeltaSecond;
	if (m_fMaxTurnDeltaSeconds <= m_fSumTurnDeltaSeconds)
	{
		m_vForward = nullopt;
		m_vPrePosition = nullopt;
		m_fSumTurnDeltaSeconds = 0.0f;
		return EBTNodeResult::SUCCEEDED;
	}

	m_spMonster->SetLinearVelocity({ 0.0f, 0.0f, 0.0f });
	m_spMonster->SetAngularVelocity({ 0.0f, 0.0f, 0.0f });

	float fRatio = m_fSumTurnDeltaSeconds / m_fMaxTurnDeltaSeconds;
	Vector3 vLerpDirection = Vector3::Lerp(m_vForward.value(), vDirection, fRatio);
	m_spMonster->LookAt(vMonsterPosition + vLerpDirection);

	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}

	m_spMonster->SetPosition(m_vPrePosition.value());

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskTurn::Release()
{
	m_spMeshComponent = nullptr;
	m_wpTarget.reset();
	m_spMonster = nullptr;

	CBTNode::Release();
}
