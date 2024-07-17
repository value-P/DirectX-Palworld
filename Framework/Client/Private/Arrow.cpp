// Default
#include "pch.h"
#include "Arrow.h"

// Manager
#include "GameManager.h"
#include "EffectManager.h"

// Gameobject
#include "Player.h"
#include "EffectGroup.h"

// Component
#include "SphereComponent.h"
#include "TrailComponent.h"

std::shared_ptr<CArrow> Client::CArrow::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CArrow> spArrow = make_shared<CArrow>();
	ENSUREF(spArrow->Initialize(_spGameObjectDesc), L"Failed to initialize arrow.");
	return spArrow;
}

HRESULT Client::CArrow::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CProjectile::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_fTrailSize = 0.1f;

	InitTrailComponent();

	return S_OK;
}

HRESULT Client::CArrow::BeginPlay()
{
	if (FAILED(CProjectile::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CArrow::PreTick(float _fDeltaSeconds)
{
	CProjectile::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CArrow::Tick(float _fDeltaSeconds)
{
	CProjectile::Tick(_fDeltaSeconds);

	return 0;
}

int32 Client::CArrow::PostTick(float _fDeltaSeconds)
{
	CProjectile::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CArrow::EndPlay()
{
	if (FAILED(CProjectile::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CArrow::Release()
{
	CProjectile::Release();
}

void Client::CArrow::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	m_wpEffectManager.lock()->ActivateEffect(L"Group_Hit2", GetPosition().value());

	if (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::MONSTER)
	{
		shared_ptr<CCreature> spCreature = static_pointer_cast<CCreature>(pDstColliderComponent->GetOwnerGameObject());

		float fPlayerAtk = m_wpOwnedPlayer.lock()->GetAttackPower();
		float fArrowAtk = 45.f;

		spCreature->TakeDamage(m_wpOwnedPlayer.lock(), fPlayerAtk + fArrowAtk);
	}
	else
	{
		
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

	ResetDataMember();
	SetActive(false);
}

void Client::CArrow::InitTrailComponent()
{
	FTrailComponentDesc tTrailComponentDesc;
	tTrailComponentDesc.m_wstrComponentName = L"TrailComponent";
	tTrailComponentDesc.m_eRenderType		= ERenderType::RENDER_GLOW;
	tTrailComponentDesc.m_eComponentType	= EComponentType::PARTICLE;
	tTrailComponentDesc.m_wpOwnerGameObject = shared_from_this();

	tTrailComponentDesc.m_iNumRects			= 30;
	tTrailComponentDesc.m_fMaxLifeTime		= 0.02f;
	tTrailComponentDesc.m_vColor			= Color(1.f, 1.f, 1.f, 1.f);

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

void Client::CArrow::ResetDataMember()
{
	CProjectile::ResetDataMember();
}

void Client::CArrow::Throw(Vector3 _vActivePos, Vector3 _vDirection, float _fPower, float _fDamage)
{
	CProjectile::Throw(_vActivePos, _vDirection, _fPower, _fDamage);

	shared_ptr<CEffectGroup> spEffect = m_wpEffectManager.lock()->ActivateEffect(L"Group_Arrow_Shot", _vActivePos);
	if (nullptr == spEffect) { return; }

	Vector3 vPlayerLook = CGameManager::GetInstance()->GetPlayer()->GetForwardVector().value();
	vPlayerLook.Normalize();

	spEffect->LookAt(spEffect->GetPosition().value() + 0.1f * vPlayerLook);
	spEffect->SetPosition(spEffect->GetPosition().value() + 0.1f * vPlayerLook);
}
