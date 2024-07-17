#include "pch.h"
#include "BossRoom.h"

#include "MeshComponent.h"
#include "ColliderComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CBossRoom> Client::CBossRoom::Create(const std::shared_ptr<FBossRoomDesc>& _spBossRoomDesc)
{
	const std::shared_ptr<CBossRoom> spBossRoom = make_shared<CBossRoom>();
	ENSUREF(spBossRoom->Initialize(_spBossRoomDesc), L"Faild to initialize env mesh object.");
	return spBossRoom;
}

HRESULT Client::CBossRoom::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// gameobject desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// component desc
	shared_ptr<FBossRoomDesc> spTowerDesc = static_pointer_cast<FBossRoomDesc>(_spGameObjectDesc);

	// mesh component
	{
		// component desc
		FMeshComponentDesc tMeshComponentDesc;
		tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";

		// mesh component desc
		tMeshComponentDesc.m_wstrModelFilePath = spTowerDesc->m_wstrBossRoomFilePath;
		tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.01f);
		tMeshComponentDesc.m_spParentMeshComponent = nullptr;
		tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

		m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
		ENSURE(AddComponent(L"MeshComponent", m_spMeshComponent));
	}
	
	// box component floor 00
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = Vector3(30.0f, 1.0f, 30.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(500.f + 0.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::ENVIRONMENT;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::ENVIRONMENT;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName = L"Floor00BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component floor 01
	{
		// box compzonent desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = Vector3(4.0f, 1.0f, 15.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(500.f + 0.0f, 500.f + 0.0f, 500.f + 45.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_wstrComponentName = L"Floor01BoxComponent";
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component wall 00
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = Vector3(1.0f, 30.0f, 30.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(500.f + 31.0f, 500.f + 30.0f, 500.f + 0.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName = L"Wall00BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component wall 01
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = Vector3(30.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(500.f + 0.0f, 500.f + 30.0f, 500.f - 31.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_wstrComponentName = L"Wall01BoxComponent";
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component wall 02
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = Vector3(1.0f, 30.0f, 30.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(500.f - 31.0f, 500.f + 30.0f, 500.f + 0.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_wstrComponentName = L"Wall02BoxComponent";
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component wall 03
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = Vector3(13.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(500.f + 17.0f, 500.f + 30.0f, 500.f + 31.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName = L"Wall03BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component wall 04
	{
		// box compzonent desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = Vector3(13.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(500.f + 17.0f, 500.f + 30.0f, 500.f + 31.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName = L"Wall04BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component wall 05
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = Vector3(4.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(500.f + 0.0f, 500.f + 30.0f, 500.f + 58.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName = L"Wall05BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component wall 06
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;

		tBoxComponentDesc.m_vExtents = Vector3(1.0f, 30.0f, 15.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(500.f + 4.0f, 500.f + 30.0f, 500.f + 45.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_wstrComponentName = L"Wall06BoxComponent";
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component wall 07
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = Vector3(1.0f, 30.0f, 15.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(500.f - 4.0f, 500.f + 30.0f, 500.f + 45.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName = L"Wall07BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component column 00
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents		= Vector3(1.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition	= Vector3(500.f + 14.5f, 500.f + 0.0f, 500.f - 18.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType	= EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType	= EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag	= ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity		= false;
		tBoxComponentDesc.m_bTrigger		= false;
		tBoxComponentDesc.m_iAxisLockType	= 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName	= L"Column00BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component column 01
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents		= Vector3(1.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition	= Vector3(500.f + 14.5f, 500.f + 0.0f, 500.f - 7.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType	= EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType	= EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag	= ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity		= false;
		tBoxComponentDesc.m_bTrigger		= false;
		tBoxComponentDesc.m_iAxisLockType	= 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName	= L"Column01BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component column 02
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents		= Vector3(1.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition	= Vector3(500.f + 14.5f, 500.f + 0.0f, 500.f + 7.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType	= EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType	= EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag	= ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity		= false;
		tBoxComponentDesc.m_bTrigger		= false;
		tBoxComponentDesc.m_iAxisLockType	= 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName	= L"Column02BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component column 03
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents		= Vector3(1.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition	= Vector3(500.f + 14.5f, 500.f + 0.0f, 500.f + 18.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType	= EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType	= EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag	= ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity		= false;
		tBoxComponentDesc.m_bTrigger		= false;
		tBoxComponentDesc.m_iAxisLockType	= 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName	= L"Column03BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component column 04
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents		= Vector3(1.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition	= Vector3(500.f - 14.5f, 500.f + 0.0f, 500.f - 18.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType	= EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType	= EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag	= ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity		= false;
		tBoxComponentDesc.m_bTrigger		= false;
		tBoxComponentDesc.m_iAxisLockType	= 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName	= L"Column04BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component column 05
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents		= Vector3(1.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition	= Vector3(500.f - 14.5f, 500.f + 0.0f, 500.f - 7.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType	= EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType	= EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag	= ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity		= false;
		tBoxComponentDesc.m_bTrigger		= false;
		tBoxComponentDesc.m_iAxisLockType	= 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName	= L"Column05BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component column 06
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents		= Vector3(1.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition	= Vector3(500.f - 14.5f, 500.f + 0.0f, 500.f + 7.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType	= EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType	= EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag	= ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity		= false;
		tBoxComponentDesc.m_bTrigger		= false;
		tBoxComponentDesc.m_iAxisLockType	= 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName	= L"Column06BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	// box component column 07
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents		= Vector3(1.0f, 30.0f, 1.0f);
		tBoxComponentDesc.m_vLocalPosition	= Vector3(500.f - 14.5f, 500.f + 0.0f, 500.f + 18.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType	= EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType	= EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag	= ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_bGravity		= false;
		tBoxComponentDesc.m_bTrigger		= false;
		tBoxComponentDesc.m_iAxisLockType	= 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName	= L"Column07BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	return S_OK;
}

HRESULT Client::CBossRoom::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CBossRoom::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CBossRoom::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CBossRoom::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

HRESULT Client::CBossRoom::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CBossRoom::Release()
{
	m_spMeshComponent = nullptr;

	CGameObject::Release();
}
