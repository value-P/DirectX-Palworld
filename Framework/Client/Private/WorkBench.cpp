// default
#include "pch.h"
#include "WorkBench.h"

// module, manager
#include "ClientModule.h"
#include "EffectManager.h"

#include "EngineModule.h"
#include "InputManager.h"

// component
#include "MeshComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CWorkBench> Client::CWorkBench::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CWorkBench> spWorkBench = make_shared<CWorkBench>();
	ENSUREF(spWorkBench->Initialize(_spGameObjectDesc), L"Fail to initialize workbench.");
	return spWorkBench;
}

HRESULT Client::CWorkBench::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// factory desc
	if (FAILED(CFactory::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_eFactoryType = EFactoryType::WORK_BENCH;

	// work bench desc
	shared_ptr<FWorkBenchDesc> spWorkBenchDesc = static_pointer_cast<FWorkBenchDesc>(_spGameObjectDesc);
	ENSUREF(InitializeMeshComponent(), L"Failed to initialize mesh component.");
	ENSUREF(InitializeColliderComponent(), L"Failed to initialize collider component.");
	return S_OK;
}

HRESULT Client::CWorkBench::BeginPlay()
{
	if (FAILED(CFactory::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CWorkBench::PreTick(float _fDeltaSeconds)
{
	switch (CFactory::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CWorkBench::Tick(float _fDeltaSeconds)
{
	switch (CFactory::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CWorkBench::PostTick(float _fDeltaSeconds)
{
	switch (CFactory::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CWorkBench::EndPlay()
{
	if (FAILED(CFactory::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CWorkBench::Release()
{
	m_spRequiredAptitudePanel	= nullptr;
	m_spWaitingItemIsNullPanel	= nullptr;
	m_spWaitingItemPanel		= nullptr;
	m_spSelectRecipePanel		= nullptr;
	m_spWorkingWindow			= nullptr;

	CFactory::Release();
}

HRESULT Client::CWorkBench::InitializeMeshComponent()
{
	// component desc
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName		= L"WorkBenchMeshComponent";
	tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();

	// mesh desc
	tMeshComponentDesc.m_wstrModelFilePath		= m_wstrModelFilePath;
	tMeshComponentDesc.m_wstrShaderName			= m_wstrShaderName;
	tMeshComponentDesc.m_matPivot				= Matrix::CreateScale(m_fPivotScaleRatio) * 
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), XMConvertToRadians(-90.f)));

	shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	m_umapMeshComponents.emplace(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
	return AddComponent(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
}

HRESULT Client::CWorkBench::InitializeColliderComponent()
{
	// component desc
	FBoxComponentDesc tBoxComponentDesc;
	tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tBoxComponentDesc.m_wstrComponentName	= L"BodySphereComponent";
	tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	// collider component desc
	tBoxComponentDesc.m_bTrigger			= false;
	tBoxComponentDesc.m_bGravity			= true;
	tBoxComponentDesc.m_eColliderType		= EColliderType::BOX;
	tBoxComponentDesc.m_eMobilityType		= EMobilityType::STATIC;
	tBoxComponentDesc.m_eCollisionFlag		= ECollisionFlag::BUILDING;
	tBoxComponentDesc.m_eCollisionLayer		= ECollisionLayer::BUILDING;
	tBoxComponentDesc.m_vLocalPosition		= Vector3(0.0f, 1.0f, 0.0f);
	
	// box component desc
	tBoxComponentDesc.m_vExtents			= m_vColliderExtents;
	tBoxComponentDesc.m_vLocalPosition		= m_vColliderCenter;
	tBoxComponentDesc.m_vLocalRotate		= m_vColliderRotate;

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxComponent);
}

void Client::CWorkBench::AddItemToFactory(EItemType _eItemType, int32 _iNumItems)
{
	CFactory::AddItemToFactory(_eItemType, _iNumItems);

	m_iCurCommandID = RegisterCommandToSystem(EToDoType::PRODUCE_ITEM, EMonsterTaskType::HANDIWORK);
}

void Client::CWorkBench::RemoveItemFromFactory()
{
	CFactory::RemoveItemFromFactory();

	ENSURE(UnRegisterCommandFromSystem(EToDoType::PRODUCE_ITEM, m_iCurCommandID));
}