#include "pch.h"
#include "Stone.h"

#include "SphereComponent.h"
#include "MeshComponent.h"

std::shared_ptr<CStone> Client::CStone::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	const std::shared_ptr<CStone> spStone = make_shared<CStone>();
	ENSUREF(spStone->Initialize(_spGameObjectDesc), L"Failed to initialize stone.");
	return spStone;
}

HRESULT Client::CStone::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// nature desc
	if (FAILED(CNature::Initialize(_spGameObjectDesc))) { return E_FAIL; };

	// stone desc
	shared_ptr<FStoneDesc> spStoneDesc = static_pointer_cast<FStoneDesc>(_spGameObjectDesc);
	m_fRadius = spStoneDesc->m_fRadius;
	m_vCenter = spStoneDesc->m_vCenter;

	{
		// sphere component desc
		FSphereComponentDesc tSphereComponentDesc;
		tSphereComponentDesc.m_fRadius				= m_fRadius;

		// collider component desc
		tSphereComponentDesc.m_eMobilityType		= EMobilityType::STATIC;
		tSphereComponentDesc.m_eColliderType		= EColliderType::SPHERE;
		tSphereComponentDesc.m_eCollisionLayer		= ECollisionLayer::NATURE;
		tSphereComponentDesc.m_eCollisionFlag		= ECollisionFlag::NATURE;
		tSphereComponentDesc.m_fDensity				= 1.0f;
		tSphereComponentDesc.m_fMass				= 0.0f;
		tSphereComponentDesc.m_bGravity				= false;
		tSphereComponentDesc.m_bTrigger				= true;
		tSphereComponentDesc.m_bDeferredAdd			= true;
		tSphereComponentDesc.m_iAxisLockType		= 0;
		tSphereComponentDesc.m_vLocalPosition		= m_vCenter;
		tSphereComponentDesc.m_vMaterial			= Vector3(0.5f, 0.5f, 0.5f);

		// component desc
		tSphereComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tSphereComponentDesc.m_eComponentType		= EComponentType::SPHERE;
		tSphereComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tSphereComponentDesc.m_wstrComponentName	= L"SphereComponent";

		m_spSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		AddComponent(L"SphereComponent", m_spSphereComponent);
	}

	{
		// mesh component
		FMeshComponentDesc tMeshComponentDesc;
		tMeshComponentDesc.m_matPivot				= Matrix::CreateScale(0.01f);
		tMeshComponentDesc.m_spParentMeshComponent	= nullptr;
		tMeshComponentDesc.m_wstrModelFilePath		= spStoneDesc->m_wstrModelFilePath;
		tMeshComponentDesc.m_wstrShaderName			= L"";

		// component desc
		tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
		tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
		tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();
		tMeshComponentDesc.m_wstrComponentName		= L"MeshComponent";

		m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
		AddComponent(L"MeshComponent", m_spMeshComponent);
	}

	return S_OK;
}

HRESULT Client::CStone::BeginPlay()
{
	if (FAILED(CNature::BeginPlay())) { return E_FAIL; };

	return S_OK;
}

int32 Client::CStone::PreTick(float _fDeltaSeconds)
{
	CNature::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CStone::Tick(float _fDeltaSeconds)
{
	CNature::Tick(_fDeltaSeconds);

	return 0;
}

int32 Client::CStone::PostTick(float _fDeltaSeconds)
{
	CNature::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CStone::EndPlay()
{
	if (FAILED(CNature::EndPlay())) { return E_FAIL; };

	return S_OK;
}

void Client::CStone::Release()
{
	m_spSphereComponent = nullptr;

	CNature::Release();
}

HRESULT Client::CStone::AddHabitatToScene()
{
	return m_spSphereComponent->AddRigidActorToScene();
}

HRESULT Client::CStone::RemoveHabitatFromScene()
{
	return m_spSphereComponent->RemoveRigidActorFromScene();
}
