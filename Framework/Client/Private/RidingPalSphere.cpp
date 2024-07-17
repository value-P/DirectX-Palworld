#include "pch.h"
#include "RidingPalSphere.h"

#include "EffectManager.h"

#include "PalRetrieveTrail.h"

#include "SphereComponent.h"
#include "MeshComponent.h"
#include "Player.h"
#include "EffectGroup.h"
#include "TrailComponent.h"

std::shared_ptr<CRidingPalSphere> Client::CRidingPalSphere::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CRidingPalSphere> spPalSphere = make_shared<CRidingPalSphere>();
	ENSUREF(spPalSphere->Initialize(_spGameObjectDesc), L"Failed to initialize CRidingPalSphere.");
	return spPalSphere;
}

HRESULT Client::CRidingPalSphere::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CProjectile::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	InitTrailComponent();

	m_fLocalGravity = 25.f;
	m_fDisableTime = 1.5f;
	m_spMeshComponent->SetUseAnimation(false);
	return S_OK;
}

HRESULT Client::CRidingPalSphere::BeginPlay()
{
	if (FAILED(CProjectile::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CRidingPalSphere::PreTick(float _fDeltaSeconds)
{
	CProjectile::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CRidingPalSphere::Tick(float _fDeltaSeconds)
{
	if (m_fDisableTime <= m_fSumThrowSeconds)
	{
		if (!m_wpOwnedPlayer.expired())
		{
			m_wpOwnedPlayer.lock()->SpawnRidingObject(GetPosition().value() + Vector3(0.f,1.f,0.f));
			ResetDataMember();
			SetActive(false);
		}
	}

	CProjectile::Tick(_fDeltaSeconds);

	return 0;
}

int32 Client::CRidingPalSphere::PostTick(float _fDeltaSeconds)
{
	CProjectile::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CRidingPalSphere::EndPlay()
{
	if (FAILED(CProjectile::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CRidingPalSphere::Release()
{
	CProjectile::Release();
}

void Client::CRidingPalSphere::InitTrailComponent()
{
	FTrailComponentDesc tTrailComponentDesc;
	tTrailComponentDesc.m_wstrComponentName = L"TrailComponent";
	tTrailComponentDesc.m_eRenderType		= ERenderType::RENDER_GLOW;
	tTrailComponentDesc.m_eComponentType	= EComponentType::PARTICLE;
	tTrailComponentDesc.m_wpOwnerGameObject = shared_from_this();

	tTrailComponentDesc.m_iNumRects			= 20;
	tTrailComponentDesc.m_fMaxLifeTime		= 0.001f;
	tTrailComponentDesc.m_vColor			= Color(0.6f, 0.9f, 1.0f, 1.f);

	tTrailComponentDesc.m_wstrEffectShaderName = L"FX_Trail_PT2";

	tTrailComponentDesc.m_bUseDiffuse		= false;
	tTrailComponentDesc.m_bUseNoise			= false;
	tTrailComponentDesc.m_bUseMask			= true;
	tTrailComponentDesc.m_bUseDistortion	= false;
	tTrailComponentDesc.m_bUseDissolve		= false;

	tTrailComponentDesc.m_wstrDiffuseTextureName	= L"";
	tTrailComponentDesc.m_wstrNoiseTextureName		= L"";
	tTrailComponentDesc.m_wstrMaskTextureName		= L"T_VFX_tk_Barrier_deceive_01";
	tTrailComponentDesc.m_wstrDistortionTextureName = L"";
	tTrailComponentDesc.m_wstrDissolveTextureName	= L"";

	m_spTrailComponent = CTrailComponent::Create(make_shared<FTrailComponentDesc>(tTrailComponentDesc));
	CHECKF(m_spTrailComponent, L"Failed to Create TrailComponent : CProjectile");

	AddComponent(tTrailComponentDesc.m_wstrComponentName, m_spTrailComponent);
}

void Client::CRidingPalSphere::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	if (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT ||
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BUILDING)
	{
		m_wpOwnedPlayer.lock()->SpawnRidingObject(GetPosition().value());
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

void Client::CRidingPalSphere::ResetDataMember()
{
	CProjectile::ResetDataMember();
}
