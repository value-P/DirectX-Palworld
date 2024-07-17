// default
#include "pch.h"
#include "TaskKitsunebiAttackLoop.h"

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

std::shared_ptr<CTaskKitsunebiAttackLoop> Client::CTaskKitsunebiAttackLoop::Create(const std::shared_ptr<FTaskKitsunebiAttackLoopDesc>& _spTaskAttackLoopDesc)
{
	shared_ptr<CTaskKitsunebiAttackLoop> spTaskNode = make_shared<CTaskKitsunebiAttackLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskAttackLoopDesc), L"Failed to initialize chickenpal attack loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskKitsunebiAttackLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// attack loop task desc
	shared_ptr<FTaskKitsunebiAttackLoopDesc> spTaskAttackLoopDesc = static_pointer_cast<FTaskKitsunebiAttackLoopDesc>(_spNodeDesc);
	m_iAnimationID = spTaskAttackLoopDesc->m_iAnimationID;
	m_wpTarget = spTaskAttackLoopDesc->m_spTarget;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();

	InitializeProjectileEffectGroups(spTaskAttackLoopDesc->m_wstrProjectileEffectName, spTaskAttackLoopDesc->m_eProjectileCollisionLayer, spTaskAttackLoopDesc->m_eProjectileCollsionFlag);

	return S_OK;
}

void Client::CTaskKitsunebiAttackLoop::OnNodeInit(float _fDeltaSecond)
{
	if (shared_ptr<CGameObject> spAttackTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
	}
}

void Client::CTaskKitsunebiAttackLoop::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
	m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, true);

	Vector3 vMonsterForward = m_spMonster->GetForwardVector().value();
	vMonsterForward.Normalize();
	ActivateProjectileEffectGroup(0, m_wpTarget.lock(), m_spMonster->GetPosition().value() + vMonsterForward + Vector3(0.f, 1.5f, 0.f));

	shared_ptr<CEffectGroup> spEffectGroup = m_wpEffectManager.lock()->ActivateEffect(L"Group_Fiho_FireBall_ShockWave", m_spMonster->GetPosition().value() + vMonsterForward);
	if (!spEffectGroup) { return; }

	spEffectGroup->LookAt(spEffectGroup->GetPosition().value() + vMonsterForward);
}

EBTNodeResult Client::CTaskKitsunebiAttackLoop::ExecuteTaskNode(float _fDeltaSecond)
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

void Client::CTaskKitsunebiAttackLoop::Release()
{
	m_spMonster = nullptr;
	m_wpTarget.reset();
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}

void Client::CTaskKitsunebiAttackLoop::InitializeProjectileEffectGroups(const std::wstring& _wstrProjectileEffectGroupName, ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag)
{
	FProjectileEffectGroupDesc tProjectileEffectGroupDesc;

	tProjectileEffectGroupDesc.m_bActive = false;
	tProjectileEffectGroupDesc.m_wstrEffectGroupName = _wstrProjectileEffectGroupName;

	tProjectileEffectGroupDesc.m_spOwner = m_spMonster;
	tProjectileEffectGroupDesc.m_spTarget = m_wpTarget.lock();
	tProjectileEffectGroupDesc.m_eOwnerMonsterType = EMonsterType::KITSUNEBI;
	tProjectileEffectGroupDesc.m_iProjectileSlot = 0;

	// 정해진 개수만큼 투사체를 생성하여 컨테이너에 보관합니다.
	vector<shared_ptr<CProjectileEffectGroup>> vecProjectileEffects;
	vecProjectileEffects.reserve(5);
	vecProjectileEffects.resize(5);
	m_umapProjectileEffectGroups.emplace(tProjectileEffectGroupDesc.m_iProjectileSlot, vecProjectileEffects);
	for (uint32 iInnerIndex = 0; iInnerIndex < 5; ++iInnerIndex)
	{
		shared_ptr<CProjectileEffectGroup> spProjectileEffectGroup = CProjectileEffectGroup::CreateFromJson(make_shared<FProjectileEffectGroupDesc>(tProjectileEffectGroupDesc), _wstrProjectileEffectGroupName);
		CHECKF(spProjectileEffectGroup, L"Failed to Create CProjectileEffect : CTaskKitsunebiAttackLoop");

		spProjectileEffectGroup->SetCollisionOption(_eCollisionLayer, _eCollisionFlag);
		spProjectileEffectGroup->SetPosition(Vector3(0.f, -1000.f, 0.f));

		shared_ptr<CLevel> spLevel = CWorld::GetInstance()->GetPostFrontLevel();
		spLevel->AddGameObject(spProjectileEffectGroup);

		m_umapProjectileEffectGroups[tProjectileEffectGroupDesc.m_iProjectileSlot][iInnerIndex] = spProjectileEffectGroup;
	}
}

HRESULT Client::CTaskKitsunebiAttackLoop::ActivateProjectileEffectGroup(int32 _iProjectileSlot, std::shared_ptr<CCreature> _spTarget, const Vector3 _vActivatedPosition)
{
	if (!m_umapProjectileEffectGroups.contains(_iProjectileSlot)) { return E_FAIL; }

	for (auto& projectile : m_umapProjectileEffectGroups[_iProjectileSlot])
	{
		if (!projectile->IsActive())
		{
			projectile->SetPosition(_vActivatedPosition);
			projectile->SetTarget(_spTarget);
			projectile->SetDestinationPos(_spTarget->GetPosition().value());
			projectile->Reset();
			projectile->LookAt(_spTarget->GetPosition().value() + Vector3(0.f, 0.6f, 0.f));
			return S_OK;
		}
	}

	return E_FAIL;
}
