#include "pch.h"
#include "PartnerPalsphere.h"

#include "EffectManager.h"

#include "PalRetrieveTrail.h"

#include "SphereComponent.h"
#include "MeshComponent.h"
#include "Player.h"
#include "EffectGroup.h"
#include "TrailComponent.h"

std::shared_ptr<CPartnerPalsphere> Client::CPartnerPalsphere::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CPartnerPalsphere> spPalSphere = make_shared<CPartnerPalsphere>();
	ENSUREF(spPalSphere->Initialize(_spGameObjectDesc), L"Failed to initialize CPartnerPalsphere.");
	return spPalSphere;
}

HRESULT Client::CPartnerPalsphere::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CProjectile::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	InitTrailComponent();

	m_fLocalGravity = 25.f;
	m_fDisableTime = 1.5f;
	m_spMeshComponent->SetUseAnimation(false);
	return S_OK;
}

HRESULT Client::CPartnerPalsphere::BeginPlay()
{
	if (FAILED(CProjectile::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CPartnerPalsphere::PreTick(float _fDeltaSeconds)
{
	CProjectile::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CPartnerPalsphere::Tick(float _fDeltaSeconds)
{
	if (m_fDisableTime <= m_fSumThrowSeconds)
	{
		if (!m_wpOwnedPlayer.expired())
		{
			m_wpOwnedPlayer.lock()->SpawnPartnerPal(GetPosition().value());
			ResetDataMember();
			SetActive(false);
		}
	}

	CProjectile::Tick(_fDeltaSeconds);

	return 0;
}

int32 Client::CPartnerPalsphere::PostTick(float _fDeltaSeconds)
{
	CProjectile::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CPartnerPalsphere::EndPlay()
{
	if (FAILED(CProjectile::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CPartnerPalsphere::Release()
{
	CProjectile::Release();
}

void Client::CPartnerPalsphere::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	if (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT ||
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BUILDING)
	{
		m_wpOwnedPlayer.lock()->SpawnPartnerPal(GetPosition().value());
		ResetDataMember();
		SetActive(false);

		// 파트너 팰 소환 이펙트를 출력합니다.
		m_wpEffectManager.lock()->ActivateEffect(L"Group_PalSummon", GetPosition().value());
	}

	// 이펙트 비활성화
	if (!m_wpEffectGroup.expired())
	{
		m_wpEffectGroup.lock()->EndLifeTime();
	}
	if (m_spTrailComponent)
	{
		m_spTrailComponent->ClearRectList(GetPosition().value() + Vector3(0.f, m_fTrailSize, 0.f), GetPosition().value() + Vector3(0.f, -m_fTrailSize, 0.f));
	}
}

void Client::CPartnerPalsphere::ResetDataMember()
{
	CProjectile::ResetDataMember();
}