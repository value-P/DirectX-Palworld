// Default
#include "pch.h"
#include "RepairBench.h"

// Module, Manager
#include "ClientModule.h"
#include "EffectManager.h"

// Component
#include "MeshComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CRepairBench> Client::CRepairBench::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CRepairBench> spRepairBench = make_shared<CRepairBench>();
	ENSUREF(spRepairBench->Initialize(_spGameObjectDesc), L"Failed to initialize repair bench.");
	return spRepairBench;
}

HRESULT Client::CRepairBench::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// factory desc
	if (FAILED(CArchitecture::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// repair bench desc
	shared_ptr <FRepairBenchDesc> spRepairBenchDesc = static_pointer_cast<FRepairBenchDesc>(_spGameObjectDesc);

	ENSUREF(InitializeMeshComponent(), L"Failed to initialize mesh component.");
	ENSUREF(InitializeColliderComponent(), L"Failed to initialize collider component.");
	return S_OK;
}

HRESULT Client::CRepairBench::BeginPlay()
{
	if (FAILED(CArchitecture::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CRepairBench::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CRepairBench::Tick(float _fDeltaSeconds)
{
	switch (CArchitecture::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CRepairBench::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CRepairBench::EndPlay()
{
	if (FAILED(CArchitecture::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CRepairBench::Release()
{
	m_spSelectPanel = nullptr;
	m_spRepairWindow = nullptr;

	CArchitecture::Release();
}

HRESULT Client::CRepairBench::InitializeMeshComponent()
{
	// component desc
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName		= L"RepairBenchMeshComponent";
	tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();

	// mesh desc
	tMeshComponentDesc.m_wstrModelFilePath		= m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
	tMeshComponentDesc.m_wstrShaderName			= m_wstrShaderName;

	shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	m_umapMeshComponents.emplace(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
	return AddComponent(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
}

HRESULT Client::CRepairBench::InitializeColliderComponent()
{
	FBoxComponentDesc tBoxComponentDesc;

	tBoxComponentDesc.m_vExtents = m_vColliderExtents;
	tBoxComponentDesc.m_vLocalPosition = m_vColliderCenter;
	tBoxComponentDesc.m_vLocalRotate = m_vColliderRotate;

	// component desc
	tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
	tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tBoxComponentDesc.m_wstrComponentName = L"BodySphereComponent";
	tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

	// collider component desc
	tBoxComponentDesc.m_bTrigger = false;
	tBoxComponentDesc.m_bGravity = true;
	tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
	tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
	tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
	tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;

	shared_ptr<CBoxComponent> spBodyBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(tBoxComponentDesc.m_wstrComponentName, spBodyBoxComponent);
}