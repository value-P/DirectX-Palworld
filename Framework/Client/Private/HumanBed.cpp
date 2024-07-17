// Default
#include "pch.h"
#include "HumanBed.h"

// Module, Manager
#include "ClientModule.h"
#include "EffectManager.h"

// Component
#include "MeshComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CHumanBed> Client::CHumanBed::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CHumanBed> spHumanBed = make_shared<CHumanBed>();
	CHECKF(spHumanBed, L"Failed to create CHumanBed : CHumanBed");
	spHumanBed->Initialize(_spGameObjectDesc);

	return spHumanBed;
}

HRESULT Client::CHumanBed::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CArchitecture::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr <FHumanBedDesc> spHumanBedDesc = static_pointer_cast<FHumanBedDesc>(_spGameObjectDesc);

	InitializeMeshComponent();
	InitializeColliderComponent();

	return S_OK;
}

HRESULT Client::CHumanBed::BeginPlay()
{
	if (FAILED(CArchitecture::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CHumanBed::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CHumanBed::Tick(float _fDeltaSeconds)
{
	switch (CArchitecture::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CHumanBed::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CHumanBed::EndPlay()
{
	if (FAILED(CArchitecture::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CHumanBed::Release()
{
	CArchitecture::Release();
}

HRESULT Client::CHumanBed::InitializeMeshComponent()
{
	// 건축물 메쉬 생성
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName		= L"MeshComponent";
	tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath		= m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
	tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();
	tMeshComponentDesc.m_wstrShaderName			= m_wstrShaderName;

	shared_ptr<CMeshComponent> spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));

	m_umapMeshComponents.emplace(L"MeshComponent", spMeshCom);
	return AddComponent(L"MeshComponent", spMeshCom);
}

HRESULT Client::CHumanBed::InitializeColliderComponent()
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
	tBoxComponentDesc.m_bTrigger			= false;
	tBoxComponentDesc.m_iAxisLockType		= 0;
	tBoxComponentDesc.m_vMaterial			= Vector3(0.5f, 0.5f, 0.5f);

	// component desc
	tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
	tBoxComponentDesc.m_wstrComponentName	= L"BoxComponent";
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(L"BoxComponent", m_spBoxComponent);
}
