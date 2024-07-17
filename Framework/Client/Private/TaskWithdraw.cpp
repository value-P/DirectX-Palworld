#include "pch.h"
#include "TaskWithdraw.h"

#include "Monster.h"
#include "MeshComponent.h"
#include "ColliderComponent.h"

#include "MathUtility.h"

std::shared_ptr<CTaskWithdraw> Client::CTaskWithdraw::Create(const std::shared_ptr<FTaskWithdrawDesc>& _spTaskWithdrawDesc)
{
	const std::shared_ptr<CTaskWithdraw> spTask = make_shared<CTaskWithdraw>();
	ENSUREF(spTask->Initialize(_spTaskWithdrawDesc), L"Failed to initialize withdraw task.");
	return spTask;
}

HRESULT Client::CTaskWithdraw::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// withdraw task desc
	shared_ptr<FTaskWithdrawDesc> spTaskWithdrawDesc = static_pointer_cast<FTaskWithdrawDesc>(_spNodeDesc);
	m_iAnimationID = spTaskWithdrawDesc->m_iAnimationID;
	m_fMinDistance = spTaskWithdrawDesc->m_fMinDistance;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	m_spColliderComponent = m_spMonster->GetBodyColliderComponent();
	return S_OK;
}

void Client::CTaskWithdraw::OnNodeInit(float _fDeltaSecond)
{
	if (shared_ptr<CGameObject> spTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = spTarget;
	}

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();

	// 거리가 충분히 먼 경우
	if (m_fMinDistance < Vector3::Distance(vMonsterPosition, vTargetPosition))
	{
		m_bWithDrawRequired = false;
	}

	m_fSumWithdrawDeltaSeconds = 0.0f;
}

void Client::CTaskWithdraw::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	// 거리가 충분히 먼 경우
	if (!m_bWithDrawRequired)
	{
		m_bWithDrawRequired = true;
		return;
	}

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();
	
	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);

	Vector3 vBackward = vMonsterPosition - vTargetPosition;
	vBackward.Normalize();

	Vector3 vUp = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 vCross = vUp.Vector3::Cross(vBackward);

	float fOffset = CMathUtility::GetRandomFloat(-1.0f, 1.0f);

	Vector3 vDirection = vUp + vBackward + fOffset * vCross;
	vDirection.Normalize();

	m_spMonster->LookAt(vTargetPosition);
	m_spColliderComponent->AddForce(vDirection, m_fPower);
}

EBTNodeResult Client::CTaskWithdraw::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_bWithDrawRequired)
	{
		return EBTNodeResult::SUCCEEDED;
	}

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();

	m_fSumWithdrawDeltaSeconds += _fDeltaSecond;
	if (m_fMaxWithdrawDeltaSeconds <= m_fSumWithdrawDeltaSeconds)
	{
		m_fSumWithdrawDeltaSeconds = 0.0f;
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskWithdraw::Release()
{
	m_spColliderComponent = nullptr;
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;
	m_wpTarget.reset();

	CBTNode::Release();
}