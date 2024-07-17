// default
#include "pch.h"
#include "TaskBerryGoatAttackLoop.h"

// manager
#include "EffectManager.h"
#include "GameManager.h"

// level
#include "World.h"
#include "Level.h"

// component
#include "MeshComponent.h"

// gameobject
#include "Creature.h"
#include "Monster.h"
#include "Player.h"
#include "EffectGroup.h"
#include "ProjectileEffectGroup.h"

// engine
#include "Model.h"

std::shared_ptr<CTaskBerryGoatAttackLoop> Client::CTaskBerryGoatAttackLoop::Create(const std::shared_ptr<FTaskBerryGoatAttackLoopDesc>& _spTaskBerryGoatAttackLoopDesc)
{
	shared_ptr<CTaskBerryGoatAttackLoop> spTaskNode = make_shared<CTaskBerryGoatAttackLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskBerryGoatAttackLoopDesc), L"Failed to initialize berrygoat attack loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskBerryGoatAttackLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// attack loop task desc
	shared_ptr<FTaskBerryGoatAttackLoopDesc> spTaskAttackLoopDesc = static_pointer_cast<FTaskBerryGoatAttackLoopDesc>(_spNodeDesc);
	m_iAnimationID = spTaskAttackLoopDesc->m_iAnimationID;
	m_wpTarget = spTaskAttackLoopDesc->m_spTarget;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();

	InitializeProjectileEffectGroups(spTaskAttackLoopDesc->m_wstrProjectileEffectName, spTaskAttackLoopDesc->m_eProjectileCollisionLayer, spTaskAttackLoopDesc->m_eProjectileCollsionFlag);

	return S_OK;
}

void Client::CTaskBerryGoatAttackLoop::OnNodeInit(float _fDeltaSecond)
{
	if (shared_ptr<CGameObject> spAttackTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
	}
}

void Client::CTaskBerryGoatAttackLoop::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
	m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, true);

	ActivateProjectileEffectGroup(0, m_wpTarget.lock(), m_spMonster->GetPosition().value() + Vector3(0.f, 1.f, 0.f));
}

EBTNodeResult Client::CTaskBerryGoatAttackLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	// 공격할 대상이 없거나 죽은 경우
	if (m_wpTarget.expired() || m_wpTarget.lock()->IsDead())
	{
		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
		return EBTNodeResult::FAILED;
	}

	// 상대 팰이 전투에 참여하지 않는 경우
	ECreatureType eCreatureType = m_wpTarget.lock()->GetCreatureType();
	if (ECreatureType::PAL == eCreatureType)
	{
		if (!m_wpTarget.lock()->IsParticipateInCombat())
		{
			if (m_spMonster->GetMonsterAIType() == EMonsterAIType::WILD_PAL)
			{
				shared_ptr<CPlayer> spPlayer = CGameManager::GetInstance()->GetPlayer();
				m_spMonster->ModifyGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET, spPlayer);
			}
			else if (m_spMonster->GetMonsterAIType() == EMonsterAIType::PARTNER_PAL)
			{
				m_spMonster->RequestCombatCommand();
			}

			return EBTNodeResult::FAILED;
		}
	}

	const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
	if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAnimationID)
	{
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskBerryGoatAttackLoop::Release()
{
	m_spMonster = nullptr;
	m_wpTarget.reset();
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}

void Client::CTaskBerryGoatAttackLoop::InitializeProjectileEffectGroups(const std::wstring& _wstrProjectileEffectGroupName, ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag)
{
	FProjectileEffectGroupDesc tProjectileEffectGroupDesc;

	tProjectileEffectGroupDesc.m_bActive = false;
	tProjectileEffectGroupDesc.m_wstrEffectGroupName = _wstrProjectileEffectGroupName;

	tProjectileEffectGroupDesc.m_spOwner = m_spMonster;
	tProjectileEffectGroupDesc.m_spTarget = m_wpTarget.lock();
	tProjectileEffectGroupDesc.m_eOwnerMonsterType = EMonsterType::BERRY_GOAT;
	tProjectileEffectGroupDesc.m_iProjectileSlot = 0;

	// 정해진 개수만큼 투사체를 생성하여 컨테이너에 보관합니다.
	vector<shared_ptr<CProjectileEffectGroup>> vecProjectileEffects;
	vecProjectileEffects.reserve(5);
	vecProjectileEffects.resize(5);
	m_umapProjectileEffectGroups.emplace(tProjectileEffectGroupDesc.m_iProjectileSlot, vecProjectileEffects);
	for (uint32 iInnerIndex = 0; iInnerIndex < 5; ++iInnerIndex)
	{
		shared_ptr<CProjectileEffectGroup> spProjectileEffectGroup = CProjectileEffectGroup::CreateFromJson(make_shared<FProjectileEffectGroupDesc>(tProjectileEffectGroupDesc), _wstrProjectileEffectGroupName);
		CHECKF(spProjectileEffectGroup, L"Failed to Create CProjectileEffect : CTaskDreamDemonAttackLoop");

		spProjectileEffectGroup->SetCollisionOption(_eCollisionLayer, _eCollisionFlag);
		spProjectileEffectGroup->SetPosition(Vector3(0.f, -1000.f, 0.f));

		shared_ptr<CLevel> spLevel = CWorld::GetInstance()->GetPostFrontLevel();
		spLevel->AddGameObject(spProjectileEffectGroup);

		m_umapProjectileEffectGroups[tProjectileEffectGroupDesc.m_iProjectileSlot][iInnerIndex] = spProjectileEffectGroup;
	}
}

HRESULT Client::CTaskBerryGoatAttackLoop::ActivateProjectileEffectGroup(int32 _iProjectileSlot, std::shared_ptr<CCreature> _spTarget, const Vector3 _vActivatedPosition)
{
	if (!m_umapProjectileEffectGroups.contains(_iProjectileSlot)) { return E_FAIL; }

	for (auto& projectile : m_umapProjectileEffectGroups[_iProjectileSlot])
	{
		if (!projectile->IsActive())
		{
			projectile->SetPosition(_vActivatedPosition);
			projectile->SetTarget(_spTarget);
			projectile->Reset();
			projectile->LookAt(_spTarget->GetPosition().value() + Vector3(0.f, 1.2f, 0.f));
			return S_OK;
		}
	}

	return E_FAIL;
}
