// Default
#include "pch.h"
#include "Chest.h"

// Module, Manager
#include "ClientModule.h"
#include "EffectManager.h"
#include "GameManager.h"

// Component
#include "MeshComponent.h"
#include "BoxComponent.h"

// system
#include "ChestBoxSystem.h"

std::shared_ptr<CChest> Client::CChest::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CChest> spChest = make_shared<CChest>();
	ENSUREF(spChest->Initialize(_spGameObjectDesc), L"Failed to initialize chest box.");
	return spChest;
}

HRESULT Client::CChest::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// architecture desc
	if (FAILED(CArchitecture::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// chest desc
	shared_ptr<FChestDesc> spChestDesc = static_pointer_cast<FChestDesc>(_spGameObjectDesc);

	// etc
	m_wpChestBoxSystem = CGameManager::GetInstance()->GetChestBoxSystem();
	ENSURE(InitializeMeshComponent());
	ENSURE(InitializeColliderComponent());
	return S_OK;
}

HRESULT Client::CChest::BeginPlay()
{
	if (FAILED(CArchitecture::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CChest::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CChest::Tick(float _fDeltaSeconds)
{
	switch (CArchitecture::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CChest::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CChest::EndPlay()
{
	if (FAILED(CArchitecture::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CChest::Release()
{
	m_spSelectPanel = nullptr;
	m_spChestWindow = nullptr;

	CArchitecture::Release();
}

HRESULT Client::CChest::InitializeMeshComponent()
{
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName	= L"ChestMeshComponent";
	tMeshComponentDesc.m_eRenderType		= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType		= EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath	= m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), XMConvertToRadians(-90.f)));
	tMeshComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tMeshComponentDesc.m_wstrShaderName		= m_wstrShaderName;

	shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));

	m_umapMeshComponents.emplace(L"ChestMeshComponent", spMeshComponent);
	return AddComponent(L"ChestMeshComponent", spMeshComponent);
}

HRESULT Client::CChest::InitializeColliderComponent()
{
	// box component desc
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
	tBoxComponentDesc.m_bDeferredAdd		= true;

	// component desc
	tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tBoxComponentDesc.m_wstrComponentName	= L"BoxComponent";
	tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(L"BoxComponent", m_spBoxComponent);
}

void Client::CChest::AddItemToWoodenChest(EItemType _eItemType, int32 _iNumItems)
{
	// 나무 상자에 아이템을 넣습니다.
	m_wpChestBoxSystem.lock()->InsertItemToChestBox(EArchitectureType::WOODEN_CHEST, m_iGameObjectID, _eItemType, _iNumItems);
}
