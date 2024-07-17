#include "pch.h"
#include "TaskEncount.h"

#include "MeshComponent.h"
#include "Monster.h"
#include "Model.h"

#include "GameManager.h"
#include "Player.h"

std::shared_ptr<CTaskEncount> Client::CTaskEncount::Create(const std::shared_ptr<FTaskEncountDesc>& _spTaskEncountDesc)
{
	shared_ptr<CTaskEncount> spTaskNode = make_shared<CTaskEncount>();
	ENSUREF(spTaskNode->Initialize(_spTaskEncountDesc), L"Failed to initialize encount task node.");
	return spTaskNode;
}

HRESULT Client::CTaskEncount::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// encount task desc
	shared_ptr<FTaskEncountDesc> spTaskEncountDesc = static_pointer_cast<FTaskEncountDesc>(_spNodeDesc);
	m_iAnimationID		= spTaskEncountDesc->m_iAnimationID;
	m_wpTarget			= spTaskEncountDesc->m_spTarget;

	// etc
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();
	return S_OK;
}

void Client::CTaskEncount::OnNodeInit(float _fDeltaSecond)
{
	if (shared_ptr<CGameObject> spAttackTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
	}
}

void Client::CTaskEncount::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskEncount::ExecuteTaskNode(float _fDeltaSecond)
{
	const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
	if (tAnimationData.m_tBaseAnimData.m_bLoopEnd
		&& tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAnimationID)
	{
		return EBTNodeResult::SUCCEEDED;
	}

	if (m_wpTarget.expired() || m_wpTarget.lock()->IsDead())
	{
		m_wpTarget.reset();
		m_spMonster->RequestCombatCommand();
		return EBTNodeResult::SUCCEEDED;
	}

	Vector3 vPosition = m_wpTarget.lock()->GetPosition().value();
	m_spMonster->LookAt(vPosition);

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskEncount::Release()
{
	m_spMeshComponent = nullptr;
	m_spMonster = nullptr;
	m_wpTarget.reset();

	CTaskNode::Release();
}
