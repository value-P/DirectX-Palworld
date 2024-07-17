
// default
#include "pch.h"
#include "TaskPenguinAttackLoop.h"

// manager
#include "EffectManager.h"
#include "GameManager.h"

// level
#include "World.h"
#include "Level.h"

// component
#include "MeshComponent.h"
#include "TrailComponent.h"

// gameobject
#include "Creature.h"
#include "Monster.h"
#include "Player.h"
#include "EffectGroup.h"
#include "ProjectileEffectGroup.h"

// engine
#include "Model.h"

std::shared_ptr<CTaskPenguinAttackLoop> Client::CTaskPenguinAttackLoop::Create(const std::shared_ptr<FTaskPenguinAttackLoopDesc>& _spTaskAttackLoopDesc)
{
	shared_ptr<CTaskPenguinAttackLoop> spTaskNode = make_shared<CTaskPenguinAttackLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskAttackLoopDesc), L"Failed to initialize penguin attack loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskPenguinAttackLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// attack loop task desc
	shared_ptr<FTaskPenguinAttackLoopDesc> spTaskAttackLoopDesc = static_pointer_cast<FTaskPenguinAttackLoopDesc>(_spNodeDesc);
	m_iActionAnimationID = spTaskAttackLoopDesc->m_iActionAnimationID;
	m_iActionLoopAnimationID = spTaskAttackLoopDesc->m_iActionLoopAnimationID;
	m_wpTarget = spTaskAttackLoopDesc->m_spTarget;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent = m_spMonster->GetMeshComponent();

	InitializeProjectileEffectGroups(spTaskAttackLoopDesc->m_wstrProjectileEffectName, spTaskAttackLoopDesc->m_eProjectileCollisionLayer, spTaskAttackLoopDesc->m_eProjectileCollsionFlag);

	return S_OK;
}

void Client::CTaskPenguinAttackLoop::OnNodeInit(float _fDeltaSecond)
{
	if (shared_ptr<CGameObject> spAttackTarget = m_spMonster->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
	}

	m_bActionCompleted = false;
	m_fSumAttackDeltaSeconds = 0.0f;
}

void Client::CTaskPenguinAttackLoop::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::ANGRY);
	m_spMonster->ChangeMouth(EPalMouthType::ANGRY);

	m_spMeshComponent->ChangeAnimation(m_iActionAnimationID, false);
	m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, true);

	ActivateProjectileEffectGroup(0, m_wpTarget.lock(), m_spMonster->GetPosition().value() + Vector3(0.f, 3.f, 0.f));
}

EBTNodeResult Client::CTaskPenguinAttackLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	// 공격할 대상이 없거나 죽은 경우
	if (m_wpTarget.expired() || m_wpTarget.lock()->IsDead())
	{
		m_bActionCompleted = false;
		m_fSumAttackDeltaSeconds = 0.0f;

		m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
		return EBTNodeResult::FAILED;
	}

	// 상대 팰이 전투에 참여하지 않는 경우
	ECreatureType eCreatureType = m_wpTarget.lock()->GetCreatureType();
	if (ECreatureType::PAL == eCreatureType)
	{
		if (!m_wpTarget.lock()->IsParticipateInCombat())
		{
			m_bActionCompleted = false;
			m_fSumAttackDeltaSeconds = 0.0f;

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

	if (m_bActionCompleted)
	{
		m_fSumAttackDeltaSeconds += _fDeltaSecond;
		if (m_fMaxAttackDeltaSeconds <= m_fSumAttackDeltaSeconds)
		{
			m_fSumAttackDeltaSeconds = 0.0f;
			m_bActionCompleted = false;
			m_iMissileCount = 0;
			return EBTNodeResult::SUCCEEDED;
		}
	}
	else
	{
		const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
		if (tAnimationData.m_tBaseAnimData.m_bLoopEnd
			&& tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iActionAnimationID)
		{
			m_spMeshComponent->ChangeAnimation(m_iActionLoopAnimationID, false);
			m_bActionCompleted = true;
		}
	}

	if (m_iMissileCount <= m_fSumAttackDeltaSeconds)
	{
		ActivateProjectileEffectGroup(0, m_wpTarget.lock(), m_spMonster->GetPosition().value() + Vector3(0.f, 3.f, 0.f));
		++m_iMissileCount;
	}

	m_spMonster->LookAt(m_wpTarget.lock()->GetPosition().value());
	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskPenguinAttackLoop::Release()
{
	m_spMonster = nullptr;
	m_wpTarget.reset();
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}

void Client::CTaskPenguinAttackLoop::InitializeProjectileEffectGroups(const std::wstring& _wstrProjectileEffectGroupName, ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag)
{
	FProjectileEffectGroupDesc tProjectileEffectGroupDesc;
	{
		// trail component
		FTrailComponentDesc tTrailComponentDesc;

		tTrailComponentDesc.m_wstrComponentName = L"TrailComponent";
		tTrailComponentDesc.m_eRenderType = ERenderType::RENDER_GLOW;
		tTrailComponentDesc.m_eComponentType = EComponentType::PARTICLE;
		// tTrailComponentDesc.m_wpOwnerGameObject = shared_from_this();

		tTrailComponentDesc.m_iNumRects = 50;
		tTrailComponentDesc.m_fMaxLifeTime = 0.005f;
		tTrailComponentDesc.m_vColor = Color(1.f, 1.f, 1.f, 1.f);

		tTrailComponentDesc.m_wstrEffectShaderName = L"FX_Trail_PT2";

		tTrailComponentDesc.m_bUseDiffuse = true;
		tTrailComponentDesc.m_bUseNoise = false;
		tTrailComponentDesc.m_bUseMask = true;
		tTrailComponentDesc.m_bUseDistortion = false;
		tTrailComponentDesc.m_bUseDissolve = false;

		tTrailComponentDesc.m_wstrDiffuseTextureName = L"T_ky_ice02";
		tTrailComponentDesc.m_wstrNoiseTextureName = L"";
		tTrailComponentDesc.m_wstrMaskTextureName = L"T_Trail05";
		tTrailComponentDesc.m_wstrDistortionTextureName = L"";
		tTrailComponentDesc.m_wstrDissolveTextureName = L"";

		// effect group
		tProjectileEffectGroupDesc.m_bActive = false;
		tProjectileEffectGroupDesc.m_wstrEffectGroupName = _wstrProjectileEffectGroupName;

		tProjectileEffectGroupDesc.m_spOwner = m_spMonster;
		tProjectileEffectGroupDesc.m_spTarget = m_wpTarget.lock();
		tProjectileEffectGroupDesc.m_eOwnerMonsterType = EMonsterType::PENGUIN;
		tProjectileEffectGroupDesc.m_iProjectileSlot = 0;

		tProjectileEffectGroupDesc.m_bTrail = true;
		tProjectileEffectGroupDesc.m_spTrailCompoonentDesc = make_shared<FTrailComponentDesc>(tTrailComponentDesc);
	}

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

HRESULT Client::CTaskPenguinAttackLoop::ActivateProjectileEffectGroup(int32 _iProjectileSlot, std::shared_ptr<CCreature> _spTarget, const Vector3 _vActivatedPosition)
{
	if (!m_umapProjectileEffectGroups.contains(_iProjectileSlot)) { return E_FAIL; }

	for (auto& projectile : m_umapProjectileEffectGroups[_iProjectileSlot])
	{
		if (!projectile->IsActive())
		{
			// 펭키 얼음 미사일이 한틱 차이로 사라지는 것 같습니다.
			projectile->SetPosition(_vActivatedPosition);
			projectile->SetTarget(_spTarget);
			projectile->Reset();
			projectile->LookAt(_spTarget->GetPosition().value() + Vector3(0.f, 0.6f, 0.f));
			projectile->ClearTrail();
			return S_OK;
		}
	}

	return E_FAIL;
}
