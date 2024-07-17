// Default
#include "pch.h"
#include "WheatFarm.h"

// Module, Manager
#include "ClientModule.h"
#include "EffectManager.h"

// Component
#include "MeshComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CWheatFarm> Client::CWheatFarm::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CWheatFarm> spWheatFarm = make_shared<CWheatFarm>();
	ENSUREF(spWheatFarm->Initialize(_spGameObjectDesc), L"Failed to initialize wheat farm.");
	return spWheatFarm;
}

HRESULT Client::CWheatFarm::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CArchitecture::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FWheatFarmDesc> spWheatFarmDesc = static_pointer_cast<FWheatFarmDesc>(_spGameObjectDesc);

	ENSURE(InitializeMeshComponent());
	ENSURE(InitializeColliderComponent());
	return S_OK;
}

HRESULT Client::CWheatFarm::BeginPlay()
{
	if (FAILED(CArchitecture::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CWheatFarm::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CWheatFarm::Tick(float _fDeltaSeconds)
{
	switch (CArchitecture::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CWheatFarm::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CWheatFarm::EndPlay()
{
	if (FAILED(CArchitecture::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CWheatFarm::Release()
{
	CArchitecture::Release();
}

HRESULT Client::CWheatFarm::InitializeMeshComponent()
{
	// 건축물 메쉬 생성
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName		= L"MeshComponent";
	tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();
	tMeshComponentDesc.m_wstrShaderName			= m_wstrShaderName;
	tMeshComponentDesc.m_wstrModelFilePath		= m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), XMConvertToRadians(-90.f)));

	shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));

	m_umapMeshComponents.emplace(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
	return AddComponent(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
}

HRESULT Client::CWheatFarm::InitializeColliderComponent()
{
	// box compzonent desc
	FBoxComponentDesc tBoxComponentDesc;

	tBoxComponentDesc.m_vExtents			= m_vColliderExtents;
	tBoxComponentDesc.m_vLocalPosition		= m_vColliderCenter;
	tBoxComponentDesc.m_vLocalRotate		= m_vColliderRotate;

	// collider component desc
	tBoxComponentDesc.m_eMobilityType		= EMobilityType::STATIC;
	tBoxComponentDesc.m_eColliderType		= EColliderType::BOX;
	tBoxComponentDesc.m_eCollisionFlag		= ECollisionFlag::BUILDING;
	tBoxComponentDesc.m_eCollisionLayer		= ECollisionLayer::BUILDING;
	tBoxComponentDesc.m_fDensity			= 1.0f;
	tBoxComponentDesc.m_fMass				= 0.0f;
	tBoxComponentDesc.m_bGravity			= false;
	tBoxComponentDesc.m_bTrigger			= true;
	tBoxComponentDesc.m_iAxisLockType		= 0;

	// component desc
	tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
	tBoxComponentDesc.m_wstrComponentName	= L"BoxComponent";

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(L"BoxComponent", m_spBoxComponent);
}
