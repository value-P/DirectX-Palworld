// default
#include "pch.h"
#include "TaskGrassMammothAttackLoop.h"

// manager
#include "EngineModule.h"
#include "EffectManager.h"
#include "CameraManager.h"
#include "SoundManager.h"

// level
#include "World.h"
#include "Level.h"

// component
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "CameraComponent.h"

// gameobject
#include "Creature.h"
#include "Monster.h"
#include "GrassMammoth.h"
#include "EffectGroup.h"
#include "ProjectileEffectGroup.h"

// engine
#include "Model.h"

// utility
#include "MathUtility.h"

// clinet
#include "ClientLoop.h"

std::shared_ptr<CTaskGrassMammothAttackLoop> Client::CTaskGrassMammothAttackLoop::Create(const std::shared_ptr<FTaskGrassMammothAttackLoopDesc>& _spTaskGrassMammothAttackLoopDesc)
{
	shared_ptr<CTaskGrassMammothAttackLoop> spTaskNode = make_shared<CTaskGrassMammothAttackLoop>();
	ENSUREF(spTaskNode->Initialize(_spTaskGrassMammothAttackLoopDesc), L"Failed to initialize grass mammoth attack loop task node.");
	return spTaskNode;
}

HRESULT Client::CTaskGrassMammothAttackLoop::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	// attack loop task desc
	shared_ptr<FTaskGrassMammothAttackLoopDesc> spTaskAttackLoopDesc = static_pointer_cast<FTaskGrassMammothAttackLoopDesc>(_spNodeDesc);
	m_iStartAnimation			= spTaskAttackLoopDesc->m_iStartAnimationID;
	m_iStartLoopAnimation		= spTaskAttackLoopDesc->m_iStartLoopAnimationID;
	m_iActionAnimationID		= spTaskAttackLoopDesc->m_iActionAnimationID;
	m_iActionLoopAnimationID	= spTaskAttackLoopDesc->m_iActionLoopAnimationID;
	m_iEndAnimationID			= spTaskAttackLoopDesc->m_iEndAnimationID;

	m_iEarthImpactAnimationID	= spTaskAttackLoopDesc->m_iEarthImpactAnimationID;
	m_iRunAnimationID			= spTaskAttackLoopDesc->m_iRunAnimationID;
	m_eCurGrassMammothSkillType = spTaskAttackLoopDesc->m_eCurGrassMammothSkillType;
	m_wpTarget					= spTaskAttackLoopDesc->m_spTarget;

	// etc
	m_spGrassMammoth					= dynamic_pointer_cast<CGrassMammoth>(m_wpOwnerGameObject.lock());
	m_spMeshComponent					= m_spGrassMammoth->GetMeshComponent();
	m_spDashColliderComponent			= m_spGrassMammoth->GetDashColliderComponent();
	m_spEarthImpactColliderComponent	= m_spGrassMammoth->GetEarthImpactBoxComponent();

	m_fRunSpeed		= CClientLoop::s_spClientDataSheet->k_mapPalDefaultRunSpeeds[EMonsterType::GRASS_MAMMOTH];
	m_fMoveSpeed	= CClientLoop::s_spClientDataSheet->k_mapPalDefaultMoveSpeeds[EMonsterType::GRASS_MAMMOTH];
	m_fCreepSpeed	= CClientLoop::s_spClientDataSheet->k_mapPalDefaultCreepSpeeds[EMonsterType::GRASS_MAMMOTH];

	InitializeProjectileEffectGroups(0, L"Group_GreenMoss_EnergyBall", spTaskAttackLoopDesc->m_eProjectileCollisionLayer, spTaskAttackLoopDesc->m_eProjectileCollsionFlag);
	InitializeProjectileEffectGroups(1, L"Group_GreenMoss_Tonado", spTaskAttackLoopDesc->m_eProjectileCollisionLayer, spTaskAttackLoopDesc->m_eProjectileCollsionFlag);

	return S_OK;
}

void Client::CTaskGrassMammothAttackLoop::OnNodeInit(float _fDeltaSecond)
{
	if (shared_ptr<CGameObject> spAttackTarget = m_spGrassMammoth->GetGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET))
	{
		m_wpTarget = dynamic_pointer_cast<CCreature>(spAttackTarget);
	}

	switch (m_eCurGrassMammothSkillType)
	{
	
	case Client::EGrassMammothSkillType::DASH:				// 근거리
	case Client::EGrassMammothSkillType::POWER_BOMB:		// 원거리; 기 모으기
	case Client::EGrassMammothSkillType::GRASS_TORNADO:		// 원거리; 기 모으기
	case Client::EGrassMammothSkillType::SPINE_VINE:		// 원거리
		m_spMeshComponent->ChangeAnimation(m_iStartAnimation, false);
		break;

	case Client::EGrassMammothSkillType::EARTH_IMPACT:		// 근거리
		m_spMeshComponent->ChangeAnimation(m_iEarthImpactAnimationID, false);
		break;

	default:
		CHECKF(false, L"Grass mammoth skill type is not designated.");
		break;
	}

	m_bStartCompleted		= false;
	m_bStartLoopCompleted	= false;
	m_bActionCompleted		= false;
	m_bActionLoopCompleted	= false;
	m_bEndCompleted			= false;

	m_fSumDashDeltaSeconds = false;
	m_fSumStartLoopDeltaSeconds = 0.0f;
}

void Client::CTaskGrassMammothAttackLoop::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spGrassMammoth->ChangeEye(EPalEyeType::ANGRY);
	m_spGrassMammoth->ChangeMouth(EPalMouthType::ANGRY);

	m_spGrassMammoth->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, true);
}

EBTNodeResult Client::CTaskGrassMammothAttackLoop::ExecuteTaskNode(float _fDeltaSecond)
{
	if (m_wpTarget.expired()) { return EBTNodeResult::SUCCEEDED; }

	switch (m_eCurGrassMammothSkillType)
	{
	case Client::EGrassMammothSkillType::DASH:
	{
		// 공격 대기 시작
		if (!m_bStartCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iStartAnimation)
			{
				m_spMeshComponent->ChangeAnimation(m_iStartLoopAnimation, false);
				m_bStartCompleted = true;
			}

			m_spGrassMammoth->LookAt(m_wpTarget.lock()->GetPosition().value());
		}
		// 공격 대기 중
		else if (!m_bStartLoopCompleted)
		{
			m_fSumStartLoopDeltaSeconds += _fDeltaSecond;
			if (m_fMaxStartLoopDeltaSeconds <= m_fSumStartLoopDeltaSeconds)
			{
				m_spDashColliderComponent->ApplySimulationDirectly(true);
				m_spEarthImpactColliderComponent->ApplySimulationDirectly(false);
				m_spMeshComponent->ChangeAnimation(m_iRunAnimationID, false);
				m_fSumStartLoopDeltaSeconds = 0.0f;
				m_bStartLoopCompleted = true;

				// 카메라 쉐이크
				CEngineModule::GetInstance()->GetCameraManager()->GetCurCamera()->ShakeCamera(EShakeType::SHAKE_FOV, 1.f, 2.5f, 4.f);
			}

			m_spGrassMammoth->LookAt(m_wpTarget.lock()->GetPosition().value());
		}
		// 돌진 중
		else
		{
			m_fSumDashDeltaSeconds += _fDeltaSecond;

			// 돌진 끝
			if (m_fMaxDashDeltaSeconds <= m_fSumDashDeltaSeconds)
			{
				m_spDashColliderComponent->ApplySimulationDirectly(false);
				m_spEarthImpactColliderComponent->ApplySimulationDirectly(false);
				m_fSumDashDeltaSeconds = 0.0f;
				m_bStartCompleted = false;
				m_bStartLoopCompleted = false;

				m_spGrassMammoth->ModifyIntValue(EBlackboardIntValue::INT_SKILL_TYPE, (int32)EGrassMammothSkillType::POWER_BOMB);
				return EBTNodeResult::SUCCEEDED;
			}

			m_spGrassMammoth->MoveForward(m_fRunSpeed);
		}
	}
	break;

	case Client::EGrassMammothSkillType::POWER_BOMB:
	{
		// 공격 대기 시작
		if (!m_bStartCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iStartAnimation)
			{
				m_spMeshComponent->ChangeAnimation(m_iStartLoopAnimation, false);
				m_bStartCompleted = true;
			}

			m_spGrassMammoth->LookAt(m_wpTarget.lock()->GetPosition().value());
		}
		// 공격 대기 중
		else if (!m_bStartLoopCompleted)
		{
			m_fSumStartLoopDeltaSeconds += _fDeltaSecond;
			if (m_fMaxStartLoopDeltaSeconds <= m_fSumStartLoopDeltaSeconds)
			{
				m_spMeshComponent->ChangeAnimation(m_iActionAnimationID, false);
				m_fSumStartLoopDeltaSeconds = 0.0f;
				m_bStartLoopCompleted = true;
			}

			m_spGrassMammoth->LookAt(m_wpTarget.lock()->GetPosition().value());
		}
		// 발사 시작
		else if (!m_bActionCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd
				&& tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iActionAnimationID)
			{
				m_spMeshComponent->ChangeAnimation(m_iEndAnimationID, false);
				m_bActionCompleted = true;

				// 이펙트 생성 시점
				Vector3 vMonsterForward = m_spGrassMammoth->GetForwardVector().value();
				vMonsterForward.Normalize();

				m_wpSoundManager.lock()->PlaySoundEx("PowerBall_01_Cast_641614399", 1.f, false);
				ActivateProjectileEffectGroup(0, m_wpTarget.lock(), m_spGrassMammoth->GetPosition().value() + 2.f * vMonsterForward + Vector3(0.f, 2.f, 0.f));
			}
		}
		// 공격 끝
		else
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd
				&& tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iEndAnimationID)
			{
				m_bStartCompleted = false;
				m_bStartLoopCompleted = false;
				m_bActionCompleted = false;
				m_bEndCompleted = true;

				m_spGrassMammoth->ModifyIntValue(EBlackboardIntValue::INT_SKILL_TYPE, (int32)EGrassMammothSkillType::GRASS_TORNADO);
				m_spGrassMammoth->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
				return EBTNodeResult::SUCCEEDED;
			}
		}
	}
	break;

	case Client::EGrassMammothSkillType::GRASS_TORNADO:
	{
		// 공격 대기 시작
		if (!m_bStartCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iStartAnimation)
			{
				m_spMeshComponent->ChangeAnimation(m_iStartLoopAnimation, false);
				m_bStartCompleted = true;

				Vector3 vMonsterRight = m_spGrassMammoth->GetRightVector().value();
				ActivateProjectileEffectGroup(1, m_wpTarget.lock(), m_spGrassMammoth->GetPosition().value() + 4.f * vMonsterRight);
				ActivateProjectileEffectGroup(1, m_wpTarget.lock(), m_spGrassMammoth->GetPosition().value() - 4.f * vMonsterRight);
			}

			m_spGrassMammoth->LookAt(m_wpTarget.lock()->GetPosition().value());
		}
		// 공격 대기 중
		else if (!m_bStartLoopCompleted)
		{
			m_fSumStartLoopDeltaSeconds += _fDeltaSecond;
			if (m_fMaxStartLoopDeltaSeconds <= m_fSumStartLoopDeltaSeconds)
			{
				m_spMeshComponent->ChangeAnimation(m_iActionAnimationID, false);
				m_fSumStartLoopDeltaSeconds = 0.0f;
				m_bStartLoopCompleted = true;
			}

			m_spGrassMammoth->LookAt(m_wpTarget.lock()->GetPosition().value());
		}
		// 발사 시작
		else if (!m_bActionCompleted)
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd
				&& tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iActionAnimationID)
			{
				m_spMeshComponent->ChangeAnimation(m_iEndAnimationID, false);
				m_bActionCompleted = true;
			}
		}
		// 공격 끝
		else
		{
			const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
			if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iEndAnimationID)
			{
				m_bStartCompleted = false;
				m_bStartLoopCompleted = false;
				m_bActionCompleted = false;
				m_bEndCompleted = true;

				m_spGrassMammoth->ModifyIntValue(EBlackboardIntValue::INT_SKILL_TYPE, (int32)EGrassMammothSkillType::EARTH_IMPACT);
				m_spGrassMammoth->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
				return EBTNodeResult::SUCCEEDED;
			}
		}
	}
	break;

	case Client::EGrassMammothSkillType::EARTH_IMPACT:
	{
		m_fSumEarthImpactDeltaSeconds += _fDeltaSecond;
		if (m_fMaxEarthImpactDeltaSecondsEnter <= m_fSumEarthImpactDeltaSeconds && !m_bEarthImpact)
		{
			m_bEarthImpact = true;
			m_spEarthImpactColliderComponent->ApplySimulationDirectly(true);
			m_spDashColliderComponent->ApplySimulationDirectly(false);
		}
		else if (m_fMaxEarthImpactDeltaSecondsExit <= m_fSumEarthImpactDeltaSeconds)
		{
			m_spEarthImpactColliderComponent->ApplySimulationDirectly(false);
			m_spDashColliderComponent->ApplySimulationDirectly(false);
		}

		// 공격 끝
		const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
		if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iEarthImpactAnimationID)
		{
			m_bEarthImpact = false;
			m_fSumEarthImpactDeltaSeconds = 0.0f;

			// int32 iSkillType = CMathUtility::GetRandomInt((int32)EGrassMammothSkillType::DASH, (int32)EGrassMammothSkillType::SPINE_VINE);
			m_spGrassMammoth->ModifyIntValue(EBlackboardIntValue::INT_SKILL_TYPE, (int32)EGrassMammothSkillType::DASH);
			m_spGrassMammoth->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
			return EBTNodeResult::SUCCEEDED;
		}
	}
	break;

	default:
		CHECKF(false, L"Grass mammoth skill type is not designated.");
		break;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskGrassMammothAttackLoop::Release()
{
	m_spGrassMammoth = nullptr;
	m_wpTarget.reset();
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}

void Client::CTaskGrassMammothAttackLoop::InitializeProjectileEffectGroups(int32 _iIndex, const std::wstring& _wstrProjectileEffectGroupName, ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag)
{
	FProjectileEffectGroupDesc tProjectileEffectGroupDesc;

	tProjectileEffectGroupDesc.m_bActive = false;
	tProjectileEffectGroupDesc.m_wstrEffectGroupName = _wstrProjectileEffectGroupName;

	tProjectileEffectGroupDesc.m_spOwner = m_spGrassMammoth;
	tProjectileEffectGroupDesc.m_spTarget = m_wpTarget.lock();
	tProjectileEffectGroupDesc.m_eOwnerMonsterType = EMonsterType::GRASS_MAMMOTH;
	tProjectileEffectGroupDesc.m_iProjectileSlot = _iIndex;

	// 정해진 개수만큼 투사체를 생성하여 컨테이너에 보관합니다.
	vector<shared_ptr<CProjectileEffectGroup>> vecProjectileEffects;
	vecProjectileEffects.reserve(5);
	vecProjectileEffects.resize(5);
	m_umapProjectileEffectGroups.emplace(tProjectileEffectGroupDesc.m_iProjectileSlot, vecProjectileEffects);
	for (uint32 iInnerIndex = 0; iInnerIndex < 5; ++iInnerIndex)
	{
		shared_ptr<CProjectileEffectGroup> spProjectileEffectGroup = CProjectileEffectGroup::CreateFromJson(make_shared<FProjectileEffectGroupDesc>(tProjectileEffectGroupDesc), _wstrProjectileEffectGroupName);
		CHECKF(spProjectileEffectGroup, L"Failed to Create CProjectileEffect : CTaskDreamDemonAttackLoop");

		spProjectileEffectGroup->SetPosition(Vector3(0.f, -1000.f, 0.f));
		spProjectileEffectGroup->SetCollisionOption(_eCollisionLayer, _eCollisionFlag);

		shared_ptr<CLevel> spLevel = CWorld::GetInstance()->GetPostFrontLevel();
		spLevel->AddGameObject(spProjectileEffectGroup);

		m_umapProjectileEffectGroups[tProjectileEffectGroupDesc.m_iProjectileSlot][iInnerIndex] = spProjectileEffectGroup;
	}
}

HRESULT Client::CTaskGrassMammothAttackLoop::ActivateProjectileEffectGroup(int32 _iProjectileSlot, std::shared_ptr<CCreature> _spTarget, const Vector3 _vActivatedPosition)
{
	if (!m_umapProjectileEffectGroups.contains(_iProjectileSlot)) { return E_FAIL; }

	for (auto& projectile : m_umapProjectileEffectGroups[_iProjectileSlot])
	{
		if (!projectile->IsActive())
		{
			projectile->SetPosition(_vActivatedPosition);
			projectile->SetTarget(_spTarget);
			projectile->Reset();
			projectile->LookAt(_spTarget->GetPosition().value());
			projectile->SetSourcePos(_vActivatedPosition);
			projectile->SetDestinationPos(_spTarget->GetPosition().value());
			return S_OK;
		}
	}

	return E_FAIL;
}
