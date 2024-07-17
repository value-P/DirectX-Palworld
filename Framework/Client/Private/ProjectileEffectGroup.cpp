// default
#include "pch.h"
#include "ProjectileEffectGroup.h"

// manager
#include "EffectManager.h"

// gameobject
#include "Creature.h"

// component
#include "ControllerComponent.h"
#include "ColliderComponent.h"
#include "TrailComponent.h"

shared_ptr<CProjectileEffectGroup> Client::CProjectileEffectGroup::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CProjectileEffectGroup> spInstance = make_shared<CProjectileEffectGroup>();
	CHECKF(spInstance, L"Failed to Create : CProjectileEffectGroup");

	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}
shared_ptr<CProjectileEffectGroup> Client::CProjectileEffectGroup::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CProjectileEffectGroup> spInstance = make_shared<CProjectileEffectGroup>();
	CHECKF(spInstance, L"Failed to CreateFromJson : CProjectileEffectGroup");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT Client::CProjectileEffectGroup::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CEffectGroup::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// gameobject
	shared_ptr<FProjectileEffectGroupDesc> spProjectileEffectgroupDesc = static_pointer_cast<FProjectileEffectGroupDesc>(_spGameObjectDesc);

	m_wpOwner = spProjectileEffectgroupDesc->m_spOwner;
	m_wpTarget = spProjectileEffectgroupDesc->m_spTarget;
	m_eOwnerMonsterType = spProjectileEffectgroupDesc->m_eOwnerMonsterType;
	m_iProjectileSlot = spProjectileEffectgroupDesc->m_iProjectileSlot;

	if (spProjectileEffectgroupDesc->m_bTrail)
	{
		InitializeTrailCompoonent(spProjectileEffectgroupDesc->m_spTrailCompoonentDesc);
	}

	return S_OK;
}

HRESULT Client::CProjectileEffectGroup::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectGroup::InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// gameobject
	shared_ptr<FProjectileEffectGroupDesc> spProjectileEffectgroupDesc = static_pointer_cast<FProjectileEffectGroupDesc>(_spGameObjectDesc);

	m_wpOwner = spProjectileEffectgroupDesc->m_spOwner;
	m_wpTarget = spProjectileEffectgroupDesc->m_spTarget;
	m_eOwnerMonsterType = spProjectileEffectgroupDesc->m_eOwnerMonsterType;
	m_iProjectileSlot = spProjectileEffectgroupDesc->m_iProjectileSlot;

	if (spProjectileEffectgroupDesc->m_bTrail)
	{
		InitializeTrailCompoonent(spProjectileEffectgroupDesc->m_spTrailCompoonentDesc);
	}

	return S_OK;
}

HRESULT Client::CProjectileEffectGroup::BeginPlay()
{
	if (FAILED(CEffectGroup::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CProjectileEffectGroup::PreTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CProjectileEffectGroup::Tick(float _fDeltaSeconds)
{
	switch (CEffectGroup::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	switch (m_eOwnerMonsterType)
	{
	case EMonsterType::PENGUIN:
	{
		int32 iProgressResult = ProgressPankyProjectile(_fDeltaSeconds);
		if (0 == iProgressResult) { return 0; }

		break;
	}
	case EMonsterType::KITSUNEBI:
	{
		int32 iProgressResult = ProgressKitsunebiProjectile(_fDeltaSeconds);
		if (0 == iProgressResult) { return 0; }

		break;
	}
	case EMonsterType::HEDGE_HOG:
	{

		break;
	}
	case EMonsterType::DREAM_DEMON:
	{
		int32 iProgressResult = ProgressDreamDemonProjectile(_fDeltaSeconds);
		if (0 == iProgressResult) { return 0; }

		break;
	}
	case EMonsterType::BERRY_GOAT:
	{
		int32 iProgressResult = ProgressBerryGoatProjectile(_fDeltaSeconds);
		if (0 == iProgressResult) { return 0; }

		break;
	}
	case EMonsterType::GRASS_MAMMOTH:
	{
		int32 iProgressResult = ProgressGreenMammothProjectile(_fDeltaSeconds);
		if (0 == iProgressResult) { return 0; }

		break;
	}
	case EMonsterType::ELEC_PANDA:
	{
		int32 iProgressResult = ProgressElecPandaProjectile(_fDeltaSeconds);
		if (0 == iProgressResult) { return 0; }

		break;
	}
	}

	if(m_spTrailComponent)
	{
		float fTrailSize = 0.2f;
		if (EMonsterType::ELEC_PANDA == m_eOwnerMonsterType) { fTrailSize = 0.1f; }

		Vector3 vStart = GetPosition().value() + Vector3(0.f, fTrailSize, 0.f);
		Vector3 vEnd = GetPosition().value() + Vector3(0.f, -fTrailSize, 0.f);

		m_spTrailComponent->TickTrail(vStart, vEnd);
	}

	return 0;
}

int32 Client::CProjectileEffectGroup::PostTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CProjectileEffectGroup::EndPlay()
{
	if (FAILED(CEffectGroup::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CProjectileEffectGroup::Release()
{
	m_spTrailComponent = nullptr;

	CEffectGroup::Release();
}

HRESULT Client::CProjectileEffectGroup::InitializeTrailCompoonent(const std::shared_ptr<FTrailComponentDesc>& _spTrailComponentDesc)
{
	_spTrailComponentDesc->m_wpOwnerGameObject = shared_from_this();

	m_spTrailComponent = CTrailComponent::Create(_spTrailComponentDesc);
	CHECKF(m_spTrailComponent, L"Failed to Create TrailComponent : CProjectileEffectGroup");

	AddComponent(_spTrailComponentDesc->m_wstrComponentName, m_spTrailComponent);

	return S_OK;
}

void Client::CProjectileEffectGroup::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CColliderComponent* pDstCol = dynamic_cast<CColliderComponent*>(_pDstComponent);
	CColliderComponent* pSrcCol = dynamic_cast<CColliderComponent*>(_pSrcComponent);

#pragma region 파트너팰의 투사체 -> 야생팰
	if (pDstCol && ECollisionLayer::MONSTER == pDstCol->GetCollisionLayer() &&
		pSrcCol && ECollisionLayer::ALLY_PROJECTILE == pSrcCol->GetCollisionLayer())
	{
		shared_ptr<CCreature> spCreature = static_pointer_cast<CCreature>(pDstCol->GetOwnerGameObject());
		if (!spCreature) { return; }

		spCreature->TakeDamage(m_wpOwner.lock(), spCreature->GetAttackPower());

		if (EMonsterType::GRASS_MAMMOTH == m_eOwnerMonsterType && 1 == m_iProjectileSlot)
		{
			// 토네이도 예외
		}
		else if (EMonsterType::ELEC_PANDA == m_eOwnerMonsterType && 2 == m_iProjectileSlot)
		{
			// 낙뢰 예외
			PlayHitEffect(2);
		}
		else if (EMonsterType::BERRY_GOAT == m_eOwnerMonsterType && 0 == m_iProjectileSlot)
		{
			PlayHitEffect(0);
		}
		else
		{
			PlayHitEffect(0);
			SetCull(true);
			ClearTrail();
		}
	}
#pragma endregion
#pragma region 파트너팰의 투사체 -> 지형지물
	if (pDstCol && (ECollisionLayer::BUILDING == pDstCol->GetCollisionLayer() ||
		ECollisionLayer::ENVIRONMENT == pDstCol->GetCollisionLayer() ||
		ECollisionLayer::NATURE == pDstCol->GetCollisionLayer()) &&
		pSrcCol && ECollisionLayer::ALLY_PROJECTILE == pSrcCol->GetCollisionLayer())
	{
		if (EMonsterType::GRASS_MAMMOTH == m_eOwnerMonsterType && 1 == m_iProjectileSlot)
		{

		}
		else if (EMonsterType::ELEC_PANDA == m_eOwnerMonsterType && 2 == m_iProjectileSlot)
		{
			
		}
		else if (EMonsterType::BERRY_GOAT == m_eOwnerMonsterType && 0 == m_iProjectileSlot)
		{
			PlayHitEffect(0);
		}
		else
		{
			PlayHitEffect(0);
			SetCull(true);
			ClearTrail();
		}
	}
#pragma endregion
#pragma region 야생팰의 투사체 -> 플레이어
	if (EComponentType::CONTROLLER == _pDstComponent->GetComponentType() &&
		ECollisionLayer::MONSTER_PROJECTILE == pSrcCol->GetCollisionLayer())
	{
		CControllerComponent* pDstControllerComponent = dynamic_cast<CControllerComponent*>(_pDstComponent);

		shared_ptr<CCreature> spCreature = static_pointer_cast<CCreature>(pDstControllerComponent->GetOwnerGameObject());
		if (!spCreature) { return; }

		spCreature->TakeDamage(m_wpOwner.lock(), spCreature->GetAttackPower());

		if (EMonsterType::GRASS_MAMMOTH == m_eOwnerMonsterType && 1 == m_iProjectileSlot)
		{
			PlayHitEffect(1);
		}
		else if (EMonsterType::ELEC_PANDA == m_eOwnerMonsterType && 2 == m_iProjectileSlot)
		{
			PlayHitEffect(2);
		}
		else if (EMonsterType::BERRY_GOAT == m_eOwnerMonsterType && 0 == m_iProjectileSlot)
		{
			PlayHitEffect(0);
		}
		else
		{
			PlayHitEffect(0);
			SetCull(true);
			ClearTrail();
		}

		return;
	}
#pragma endregion
#pragma region 야생팰의 투사체 -> 파트너팰
	if (pDstCol && ECollisionLayer::ALLY == pDstCol->GetCollisionLayer() &&
		pSrcCol && ECollisionLayer::MONSTER_PROJECTILE == pSrcCol->GetCollisionLayer())
	{
		shared_ptr<CCreature> spCreature = static_pointer_cast<CCreature>(pDstCol->GetOwnerGameObject());
		if (!spCreature) { return; }

		spCreature->TakeDamage(m_wpOwner.lock(), 0.f);

		if (EMonsterType::GRASS_MAMMOTH == m_eOwnerMonsterType && 1 == m_iProjectileSlot)
		{
			PlayHitEffect(1);
		}
		else if (EMonsterType::ELEC_PANDA == m_eOwnerMonsterType && 2 == m_iProjectileSlot)
		{
			PlayHitEffect(2);
		}
		else if (EMonsterType::BERRY_GOAT == m_eOwnerMonsterType && 0 == m_iProjectileSlot)
		{
			PlayHitEffect(0);
		}
		else
		{
			PlayHitEffect(0);
			SetCull(true);
			ClearTrail();
		}
	}
#pragma endregion
#pragma region 야생팰의 투사체 -> 지형지물
	if (pDstCol &&
		(ECollisionLayer::ENVIRONMENT == pDstCol->GetCollisionLayer() ||
		ECollisionLayer::NATURE == pDstCol->GetCollisionLayer()) &&
		pSrcCol && ECollisionLayer::MONSTER_PROJECTILE == pSrcCol->GetCollisionLayer())
	{
		if (EMonsterType::GRASS_MAMMOTH == m_eOwnerMonsterType && 1 == m_iProjectileSlot)
		{

		}
		else if (EMonsterType::ELEC_PANDA == m_eOwnerMonsterType && 2 == m_iProjectileSlot)
		{

		}
		else if (EMonsterType::BERRY_GOAT == m_eOwnerMonsterType && 0 == m_iProjectileSlot)
		{
			PlayHitEffect(0);
		}
		else
		{
			PlayHitEffect(0);
			SetCull(true);
			ClearTrail();
		}
	}
#pragma endregion
}

void Client::CProjectileEffectGroup::ClearTrail()
{
	if (!m_spTrailComponent) { return; }

	float fTrailSize = 0.2f;
	if (EMonsterType::ELEC_PANDA == m_eOwnerMonsterType)
	{
		fTrailSize = 0.1f;
	}

	m_spTrailComponent->ClearRectList(GetPosition().value() + Vector3(0.f, fTrailSize, 0.f), GetPosition().value() + Vector3(0.f, -fTrailSize, 0.f));
}
void Client::CProjectileEffectGroup::PlayHitEffect(uint32 _iSlotIndex)
{
	Vector3 vActivatePosition = Vector3(0.f);

	// 타겟이 없는 경우
	if (m_wpTarget.expired())
	{
		vActivatePosition = GetPosition().value();
	}
	// 타겟이 있는 경우
	else
	{
		vActivatePosition = GetPosition().value();
	}

	// 몬스터 타입에 따라 다른 이펙트 재생
	switch (m_eOwnerMonsterType)
	{
	case EMonsterType::BERRY_GOAT:
	{
		m_wpEffectManager.lock()->ActivateEffect(L"Group_BerryGoat_WindBlade_Hit1", vActivatePosition);
		break;
	}
	case EMonsterType::CARBUNCLO:
	{
		// 없음
		break;
	}
	case EMonsterType::CHICKEN_PAL:
	{
		// 없음
		break;
	}
	case EMonsterType::DREAM_DEMON:
	{
		m_wpEffectManager.lock()->ActivateEffect(L"Group_DreamDemon_Ball_Hit", vActivatePosition);
		break;
	}
	case EMonsterType::GRASS_MAMMOTH:
	{
		if (0 == _iSlotIndex)
		{
			m_wpEffectManager.lock()->ActivateEffect(L"Group_GreenMoss_EnergyBall_Hit", vActivatePosition);
		}
		else if (1 == _iSlotIndex)
		{
			if (m_wpTarget.expired())
			{
				m_wpEffectManager.lock()->ActivateEffect(L"Group_BerryGoat_WindBlade_Hit1", vActivatePosition);
			}
			else
			{
				m_wpEffectManager.lock()->ActivateEffect(L"Group_BerryGoat_WindBlade_Hit1", m_wpTarget.lock()->GetPosition().value() + Vector3(0.f, 1.f, 0.f));
			}
		}
		break;
	}
	case EMonsterType::HEDGE_HOG:
	{
		// 없음
		break;
	}
	case EMonsterType::KITSUNEBI:
	{
		m_wpEffectManager.lock()->ActivateEffect(L"Group_Fiho_FireBall_Hit", vActivatePosition);
		break;
	}
	case EMonsterType::PENGUIN:
	{
		m_wpEffectManager.lock()->ActivateEffect(L"Group_Panky_IceMissile_Hit", vActivatePosition);
		break;
	}
	case EMonsterType::PINK_CAT:
	{
		// 없음
		break;
	}
	case EMonsterType::SHEEP_BALL:
	{
		// 없음
		break;
	}
	case EMonsterType::ELEC_PANDA:
	{
		if (0 == _iSlotIndex)
		{
			m_wpEffectManager.lock()->ActivateEffect(L"Group_Hit2", vActivatePosition);
		}
		else if(1 == _iSlotIndex)
		{
			m_wpEffectManager.lock()->ActivateEffect(L"Group_ThunderHit01", vActivatePosition);
		}
		else if (2 == _iSlotIndex)
		{
			m_wpEffectManager.lock()->ActivateEffect(L"Group_ThunderHit01", vActivatePosition);
		}

		break;
	}
	}
}
void Client::CProjectileEffectGroup::EndLaunchTime()
{
	m_fLaunchTimeAcc = 0.f;
	m_vGravityVector = Vector3(0.f, 1.f, 0.f);
	EndLifeTime();
	PlayHitEffect(0);
	ClearTrail();
}
int32 Client::CProjectileEffectGroup::ProgressPankyProjectile(float _fDeltaSeconds)
{
	// 펭키
	m_fLaunchTimeAcc += _fDeltaSeconds;
	if (5.f < m_fLaunchTimeAcc)
	{
		EndLaunchTime();
		return 0;
	}

	float fFireSpeed = 15.f;
	Vector3 vLaunchDir = GetForwardVector().value();
	vLaunchDir.Normalize();

	SetPosition(GetPosition().value() + vLaunchDir * fFireSpeed * _fDeltaSeconds);
	return 1;
}
int32 Client::CProjectileEffectGroup::ProgressKitsunebiProjectile(float _fDeltaSeconds)
{
	// 파이호
	m_fLaunchTimeAcc += _fDeltaSeconds;
	if (5.f < m_fLaunchTimeAcc)
	{
		EndLaunchTime();
		return 0;
	}

	float fFireSpeed = 12.5f;

	Vector3 vLaunchDir = GetForwardVector().value();
	vLaunchDir.Normalize();

	SetPosition(GetPosition().value() + vLaunchDir * fFireSpeed * _fDeltaSeconds);
	LookAt(GetPosition().value() + vLaunchDir);
	return 1;
}
int32 Client::CProjectileEffectGroup::ProgressDreamDemonProjectile(float _fDeltaSeconds)
{
	// 몽마둥이
	m_fLaunchTimeAcc += _fDeltaSeconds;
	if (5.f < m_fLaunchTimeAcc)
	{
		EndLaunchTime();
		return 0;
	}

	float fFireSpeed = 2.f;

	Vector3 vLaunchDir = Vector3(0.f);
	if (!m_wpTarget.expired())
	{
		vLaunchDir = (m_wpTarget.lock()->GetPosition().value() + Vector3(0.f, 1.f, 0.f)) - GetPosition().value();
		LookAt(m_wpTarget.lock()->GetPosition().value() + Vector3(0.f, 1.f, 0.f));
	}
	else
	{
		vLaunchDir = GetForwardVector().value();
	}
	vLaunchDir.Normalize();

	SetPosition(GetPosition().value() + vLaunchDir * fFireSpeed * _fDeltaSeconds);
	return 1;
}
int32 Client::CProjectileEffectGroup::ProgressBerryGoatProjectile(float _fDeltaSeconds)
{
	// 베리고트
	m_fLaunchTimeAcc += _fDeltaSeconds;
	if (2.f < m_fLaunchTimeAcc)
	{
		EndLaunchTime();
		return 0;
	}

	float fFireSpeed = 35.f;

	Vector3 vLaunchDir = GetForwardVector().value();
	vLaunchDir.Normalize();

	SetPosition(GetPosition().value() + vLaunchDir * fFireSpeed * _fDeltaSeconds);
	LookAt(GetPosition().value() + vLaunchDir);
	return 1;
}
int32 Client::CProjectileEffectGroup::ProgressGreenMammothProjectile(float _fDeltaSeconds)
{
	switch (m_iProjectileSlot)
	{
	case 0:
	{
		// 파워밤
		m_fLaunchTimeAcc += _fDeltaSeconds;
		if (5.f < m_fLaunchTimeAcc)
		{
			EndLaunchTime();
			return 0;
		}

		float fFireSpeed = 10.f;

		Vector3 vLaunchDir = m_vDestinationPos - m_vSourcePos;
		vLaunchDir.Normalize();

		vLaunchDir += m_vGravityVector;
		vLaunchDir.Normalize();

		m_vGravityVector.y -= _fDeltaSeconds;

		SetPosition(GetPosition().value() + vLaunchDir * fFireSpeed * _fDeltaSeconds);
		LookAt(GetPosition().value() + vLaunchDir);
		break;
	}
	case 1:
	{
		// 토네이도
		m_fLaunchTimeAcc += _fDeltaSeconds;
		if (10.f < m_fLaunchTimeAcc)
		{
			EndLaunchTime();
			return 0;
		}

		float fFireSpeed = 5.f;

		Vector3 vLaunchDir = GetForwardVector().value();
		vLaunchDir.Normalize();

		SetPosition(GetPosition().value() + vLaunchDir * fFireSpeed * _fDeltaSeconds);
		LookAt(GetPosition().value() + vLaunchDir);

		break;
	}
	}
	return 1;
}
int32 Client::CProjectileEffectGroup::ProgressElecPandaProjectile(float _fDeltaSeconds)
{
	// ELECTRIC_ARROW
	if (m_iProjectileSlot == 0)
	{
		m_fLaunchTimeAcc += _fDeltaSeconds;
		if (5.f < m_fLaunchTimeAcc)
		{
			EndLaunchTime();
			return 0;
		}

		float fFireSpeed = 18.f;
		Vector3 vLaunchDir = m_vDestinationPos - m_vSourcePos;
		vLaunchDir.Normalize();
		SetPosition(GetPosition().value() + vLaunchDir * fFireSpeed * _fDeltaSeconds);
	}
	// ELECTRIC_BALL
	else if (m_iProjectileSlot == 1)
	{
		m_fLaunchTimeAcc += _fDeltaSeconds;
		if (5.f < m_fLaunchTimeAcc)
		{
			EndLaunchTime();
			return 0;
		}

		float fFireSpeed = 4.f;

		Vector3 vLaunchDir = Vector3(0.f);
		if (!m_wpTarget.expired())
		{
			vLaunchDir = (m_wpTarget.lock()->GetPosition().value() + Vector3(0.f, 1.f, 0.f)) - GetPosition().value();
			LookAt(m_wpTarget.lock()->GetPosition().value() + Vector3(0.f, 1.f, 0.f));
		}
		else
		{
			vLaunchDir = GetForwardVector().value();
		}
		vLaunchDir.Normalize();

		SetPosition(GetPosition().value() + vLaunchDir * fFireSpeed * _fDeltaSeconds);
	}
	// LIGHTING
	else if (m_iProjectileSlot == 2)
	{
		m_fLaunchTimeAcc += _fDeltaSeconds;
		if (5.f < m_fLaunchTimeAcc)
		{
			EndLaunchTime();
			return 0;
		}
	}

	return 1;
}
