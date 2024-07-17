#include "pch.h"
#include "TaskFollow.h"

#include "BehaviorTreeComponent.h"
#include "MeshComponent.h"
#include "CameraComponent.h"

#include "Monster.h"
#include "Player.h"
#include "Model.h"

#include "MathUtility.h"

std::shared_ptr<CTaskFollow> Client::CTaskFollow::Create(const std::shared_ptr<FTaskFollowDesc>& _spTaskFollowDesc)
{
	shared_ptr<CTaskFollow> spTaskNode = make_shared<CTaskFollow>();
	ENSUREF(spTaskNode->Initialize(_spTaskFollowDesc), L"Failed to initialize follow task node.");
	return spTaskNode;
}

HRESULT Client::CTaskFollow::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// follow task desc
	shared_ptr<FTaskFollowDesc> spTaskFollowDesc = static_pointer_cast<FTaskFollowDesc>(_spNodeDesc);
	m_wpPlayer			= spTaskFollowDesc->m_spPlayer;
	m_iWalkAnimationID	= spTaskFollowDesc->m_iWalkAnimationID;
	m_iRunAnimationID	= spTaskFollowDesc->m_iRunAnimationID;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	m_spCameraComponent = m_wpPlayer.lock()->GetCameraComponent();
	return S_OK;
}

void Client::CTaskFollow::OnNodeInit(float _fDeltaSecond)
{
	m_wpBehaviorTreeComponent.lock()->ResetBehindBTNodeResult();
}

void Client::CTaskFollow::OnNodeEnter(float _fDeltaSecond)
{
	// ǥ���� �����մϴ�.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);

	m_spMeshComponent->ChangeAnimation(m_iRunAnimationID, false);
	m_fSumLerpDeltaSeconds = 0.0f;
}

EBTNodeResult Client::CTaskFollow::ExecuteTaskNode(float _fDeltaSecond)
{
	// �÷��̾� ��ġ�� ���մϴ�.
	Vector3 vPlayerPosition = m_wpPlayer.lock()->GetPosition().value();

	// ī�޶� ���� ���͸� ���մϴ�.
	Vector3 vCameraForward = m_spCameraComponent->GetSphericalCameraForwardVector();
	vCameraForward.y = 0.0f;
	vCameraForward.Normalize();

	Vector3 vCameraRight = m_spCameraComponent->GetSphericalCameraRightVector();
	vCameraRight.y = 0.0f;
	vCameraRight.Normalize();

	Vector3 vDirection = vCameraForward + vCameraRight;
	vDirection.Normalize();

	// ���� �־�� �� ���� ��ġ�� ���մϴ�.
	Vector3 vTargetPosition = vPlayerPosition + m_fDistanceRatio * vDirection;
	Vector3 vYKilledTargetPosition = { vTargetPosition.x, 0.0f, vTargetPosition.z };

	// ���� ��ġ�� ���մϴ�.
	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	Vector3 vYKilledMonsterPosition = { vMonsterPosition.x, 0.0f, vMonsterPosition.z };

	float fDistance = Vector3::Distance(vYKilledTargetPosition, vYKilledMonsterPosition);
	if (m_fFarDistance < fDistance)
	{
		m_fMoveSpeedRatio = 1.8f;
	}
	else if (m_fNearDistance <= fDistance && fDistance <= m_fFarDistance)
	{
		m_fMoveSpeedRatio = 1.4f;
	}
	else
	{
		m_fMoveSpeedRatio = 1.0f;
	}
	
	if (fDistance < 1.0f)
	{
		m_fSumLerpDeltaSeconds = 0.0f;
		return EBTNodeResult::SUCCEEDED;
	}

	// ���� �־�� �� ���� ��ġ�� �ٶ󺾴ϴ�.
	m_fSumLerpDeltaSeconds += _fDeltaSecond;
	if (m_fSumLerpDeltaSeconds < m_fMaxLerpDeltaSeconds)
	{
		Vector3 vDirection = vTargetPosition - vMonsterPosition;
		vDirection.Normalize();

		Vector3 vForward = m_spMonster->GetForwardVector().value();
		vForward.Normalize();

		float fRatio = m_fSumLerpDeltaSeconds / m_fMaxLerpDeltaSeconds;
		Vector3 vLerpDirection = CMathUtility::EaseOutCirc(vForward, vDirection, fRatio);

		m_spMonster->LookAt(vMonsterPosition + vLerpDirection);
	}
	else
	{
		m_spMonster->LookAt(vTargetPosition);
	}

	// ���� �־�� �� ���� ��ġ�� �̵��մϴ�.
	m_spMonster->MoveForward(m_fMoveSpeedRatio * m_spMonster->GetMoveSpeed());
	
	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskFollow::Release()
{
	m_spMeshComponent = nullptr;
	m_spCameraComponent = nullptr;
	m_spMonster = nullptr;
	m_wpPlayer.reset();

	CBTNode::Release();
}

void Client::CTaskFollow::UpdatePalPosition()
{
	
}
