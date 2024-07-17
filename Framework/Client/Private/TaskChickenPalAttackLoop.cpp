// default
#include "pch.h"
#include "TaskChickenPalAttackLoop.h"

// component
#include "MeshComponent.h"

// gameobject
#include "Creature.h"
#include "Monster.h"

// engine
#include "Model.h"

std::shared_ptr<CTaskChickenPalAttackLoop> Client::CTaskChickenPalAttackLoop::Create(const std::shared_ptr<FTaskChickenPalAttackLoopDesc>& _spTaskChickenPalAttackLoopDesc)
{
	shared_ptr<CTaskChickenPalAttackLoop> spTaskNode = make_shared<CTaskChickenPalAttackLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskChickenPalAttackLoopDesc), L"Failed to initialize chickenpal attack loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskChickenPalAttackLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// attack loop task desc
	shared_ptr<FTaskChickenPalAttackLoopDesc> spTaskAttackLoopDesc = static_pointer_cast<FTaskChickenPalAttackLoopDesc>(_spNodeDesc);
	m_iAnimationID = spTaskAttackLoopDesc->m_iAnimationID;
	m_wpTarget = spTaskAttackLoopDesc->m_spTarget;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskChickenPalAttackLoop::OnNodeInit(float _fDeltaSecond)
{
	if (shared_ptr<CGameObject> spAttackTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
	}

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	Vector3 vTargetPosition = m_wpTarget.lock()->GetPosition().value();

	m_vTargetDirection = vTargetPosition - vMonsterPosition;
	m_vTargetDirection.value().Normalize();
}

void Client::CTaskChickenPalAttackLoop::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
	m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, true);
}

EBTNodeResult Client::CTaskChickenPalAttackLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	// 공격할 대상이 없거나 죽은 경우
	if (m_wpTarget.expired() || m_wpTarget.lock()->IsDead())
	{
		m_vTargetDirection = std::nullopt;
		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
		return EBTNodeResult::SUCCEEDED;
	}

	const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
	if (tAnimationData.m_tBaseAnimData.m_bLoopEnd
		&& tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAnimationID)
	{
		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
		return EBTNodeResult::SUCCEEDED;
	}

	Vector3 vMonsterPosition = m_spMonster->GetPosition().value();
	m_spMonster->LookAt(vMonsterPosition + m_vTargetDirection.value());

	Vector3 vRawRootVelocity = tAnimationData.m_vRootmotionVelocity;
	vRawRootVelocity *= 0.01f;

	Vector3 vDXRootVelocity(0.0f);
	vDXRootVelocity.x = -vRawRootVelocity.x;
	vDXRootVelocity.y = vRawRootVelocity.z;
	vDXRootVelocity.z = -vRawRootVelocity.y;

	// 블랜더 이동량만큼 월드 위치를 이동해야 한다.

	Vector3 vForward = m_spMonster->GetForwardVector().value() * vDXRootVelocity.z;
	Vector3 vRight = m_spMonster->GetRightVector().value() * vDXRootVelocity.x;
	Vector3 vUp= m_spMonster->GetForwardVector().value() * vDXRootVelocity.y;

	Vector3 vPos = m_spMonster->GetPosition().value();
	Vector3 vMoveVelocity = vForward + vRight + vUp;

	m_spMonster->SetPosition(vPos + vMoveVelocity);

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskChickenPalAttackLoop::Release()
{
	m_spMonster = nullptr;
	m_wpTarget.reset();
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
