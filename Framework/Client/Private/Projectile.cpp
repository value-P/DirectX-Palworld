#include "pch.h"
#include "Projectile.h"

#include "ClientModule.h"
#include "EffectManager.h"

#include "Monster.h"
#include "EffectGroup.h"

#include "SphereComponent.h"
#include "TrailComponent.h"
#include "Player.h"

std::shared_ptr<CProjectile> Client::CProjectile::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CProjectile> spInstance = make_shared<CProjectile>();
	CHECKF(spInstance, L"CProjectile : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CProjectile::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CWeapon::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// Manager
	m_wpEffectManager = CClientModule::GetInstance()->GetEffectManager();
	
	// GameObjectDesc
	shared_ptr<FProjectileDesc> spDesc = static_pointer_cast<FProjectileDesc>(_spGameObjectDesc);

	m_wstrEffectName = spDesc->m_wstrEffectName;
	m_wpOwnedPlayer = spDesc->m_spOwnPlayer;

	// Component
	InitColliderComponent(spDesc);
	// InitTrailComponent();

	return S_OK;
}

HRESULT Client::CProjectile::BeginPlay()
{
	if (FAILED(CWeapon::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CProjectile::PreTick(float _fDeltaSeconds)
{
	CWeapon::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CProjectile::Tick(float _fDeltaSeconds)
{
	CWeapon::Tick(_fDeltaSeconds);

	if (m_bThrowing)
	{
		m_fSumThrowSeconds += _fDeltaSeconds;
		
		float fDropSpeed = 0.5f * m_fLocalGravity * pow(m_fSumThrowSeconds, 2.f);
		fDropSpeed = min(fDropSpeed, m_fLocalTerminalSpeed);

		Vector3 vMoveVelocity = m_vThrowDirection* m_fThrowSpeed* _fDeltaSeconds;
		Vector3 vCurPos = GetPosition().value();

		vCurPos += vMoveVelocity;
		vCurPos.y -= fDropSpeed * _fDeltaSeconds;

		SetPosition(vCurPos);

		// 이펙트 회전
		if (!m_wpEffectGroup.expired())
		{
			m_wpEffectGroup.lock()->LookAt(vCurPos + vMoveVelocity + Vector3(0.f, -fDropSpeed * _fDeltaSeconds, 0.f));
			m_wpEffectGroup.lock()->SetPosition(vCurPos);
		}

		if (m_fSumThrowSeconds > m_fDisableTime)
		{
			ResetDataMember();
			SetActive(false);

			// 이펙트 비활성화
			if (!m_wpEffectGroup.expired())
			{
				m_wpEffectGroup.lock()->EndLifeTime();
			}
		}
	}

	TickTrail(m_fTrailSize);

	return 0;
}

int32 Client::CProjectile::PostTick(float _fDeltaSeconds)
{
	CWeapon::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CProjectile::EndPlay()
{
	CWeapon::BeginPlay();

	return S_OK;
}

void Client::CProjectile::Release()
{
	m_spTrailComponent = nullptr;

	CWeapon::Release();
}


void Client::CProjectile::InitColliderComponent(shared_ptr<FProjectileDesc> _spProjectileDesc)
{
	FSphereComponentDesc tSphereDesc;
	tSphereDesc.m_wstrComponentName		= L"SphereCollider";
	tSphereDesc.m_bGravity				= false;
	tSphereDesc.m_bTrigger				= true;
	tSphereDesc.m_eColliderType			= EColliderType::SPHERE;
	tSphereDesc.m_eCollisionFlag		= ECollisionFlag::PLAYER_PROJECTILE;
	tSphereDesc.m_eCollisionLayer		= ECollisionLayer::PLAYER_PROJECTILE;
	tSphereDesc.m_eComponentType		= EComponentType::SPHERE;
	tSphereDesc.m_eMobilityType			= EMobilityType::DYNAMIC;
	tSphereDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
	tSphereDesc.m_fMass					= 1.f;
	tSphereDesc.m_vMaterial				= Vector3(0.f, 0.f, 0.f);
	tSphereDesc.m_fRadius				= _spProjectileDesc->m_fColliderRadius;
	tSphereDesc.m_wpOwnerGameObject		= shared_from_this();
	tSphereDesc.m_bDeferredAdd			= _spProjectileDesc->m_bDeferredAdd;

	shared_ptr<CSphereComponent> spSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereDesc));
	AddComponent(tSphereDesc.m_wstrComponentName, spSphereComponent);
	m_wpCollider = spSphereComponent;
}

void Client::CProjectile::InitTrailComponent()
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

void Client::CProjectile::TickTrail(float _fTrailSize)
{
	if (m_spTrailComponent)
	{
		Vector3 vStart = GetPosition().value() + Vector3(0.f, _fTrailSize, 0.f);
		Vector3 vEnd = GetPosition().value() + Vector3(0.f, -_fTrailSize, 0.f);

		m_spTrailComponent->TickTrail(vStart, vEnd);
	}
}

void Client::CProjectile::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CColliderComponent* pDstCol = dynamic_cast<CColliderComponent*>(_pDstComponent);

	if (pDstCol->GetCollisionLayer() == ECollisionLayer::MONSTER)
	{
		static_pointer_cast<CMonster>(_pDstComponent->GetOwnerGameObject())->TakeDamage(shared_from_this(), m_fAttackPower);
	}

	ResetDataMember();
	SetActive(false);

	// 이펙트 비활성화
	if (!m_wpEffectGroup.expired())
	{
		m_wpEffectGroup.lock()->EndLifeTime();
	}
	if (m_spTrailComponent)
	{
		m_spTrailComponent->ClearRectList(GetPosition().value() + Vector3(0.f, 0.2f, 0.f), GetPosition().value() + Vector3(0.f, -0.2f, 0.f));
	}
}

void Client::CProjectile::SetPosition(const Vector3& _vPosition)
{
	m_wpCollider.lock()->SetWorldPosition(_vPosition);
}

void Client::CProjectile::ResetDataMember()
{
	m_vThrowDirection = Vector3(0.0f);
	m_fThrowSpeed = 0.0f;                                                                                      
	m_fAttackPower = 0.0f;
	m_fSumThrowSeconds = 0.0f;
	m_bThrowing = false;
}

void Client::CProjectile::Throw(Vector3 _vActivePos, Vector3 _vDirection, float _fPower, float _fDamage)
{
	if (!m_wstrEffectName.empty())
	{
		m_wpEffectGroup = m_wpEffectManager.lock()->FindEffect(m_wstrEffectName);
	}

	// 이펙트 활성화
	if (!m_wpEffectGroup.expired())
	{
		m_wpEffectGroup.lock()->SetCombinedOffset(m_vEffectOffset);
		m_wpEffectGroup.lock()->SetCombinedGameObject(shared_from_this());
		m_wpEffectGroup.lock()->SetPosition(GetPosition().value());
		m_wpEffectGroup.lock()->Reset();
	}
	if (m_spTrailComponent)
	{
		m_spTrailComponent->ClearRectList(_vActivePos + Vector3(0.f, m_fTrailSize, 0.f), _vActivePos + Vector3(0.f, -m_fTrailSize, 0.f));
	}

	m_vThrowDirection = _vDirection;
	m_fThrowSpeed = _fPower;
	m_fAttackPower = _fDamage;
	m_fSumThrowSeconds = 0.f;
	m_bThrowing = true;
}