// Default
#include "pch.h"
#include "Door.h"

// Module, Manager
#include "EngineModule.h"
#include "ClientModule.h"
#include "InputManager.h"
#include "BuildManager.h"
#include "GameManager.h"
#include "SoundManager.h"

// Level
#include "Level.h"

// GameObject
#include "Player.h"
#include "HousingPrefab.h"

// Component
#include "MeshComponent.h"
#include "BoxComponent.h"

shared_ptr<CDoor> Client::CDoor::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CDoor> spDoor = make_shared<CDoor>();
	CHECKF(spDoor, L"CDoor : Create Failed");
	spDoor->Initialize(_spGameObjectDesc);

	return spDoor;
}

HRESULT Client::CDoor::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CHousing::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	/* manager */
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	/* component */
	InitializeDoorParts();
	InitializeColliders();

	return S_OK;
}

HRESULT Client::CDoor::BeginPlay()
{
	if (FAILED(CHousing::BeginPlay())) { return E_FAIL; }
	if (FAILED(m_spDoorParts->BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CDoor::PreTick(float _fDeltaSeconds)
{
	switch (CHousing::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	m_spDoorParts->PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CDoor::Tick(float _fDeltaSeconds)
{
	switch (CHousing::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	SynchronizationDoorPartsSRT();
	DoorInteraction();
	m_spDoorParts->Tick(_fDeltaSeconds);

	return 0;
}

int32 Client::CDoor::PostTick(float _fDeltaSeconds)
{
	switch (CHousing::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	m_spDoorParts->PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CDoor::EndPlay()
{
	if (FAILED(CHousing::EndPlay())) { return E_FAIL; }
	if (FAILED(m_spDoorParts->EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CDoor::Release()
{
	m_spDoorParts = nullptr;

	CHousing::Release();
}

HRESULT Client::CDoor::InitializeDoorParts()
{
	// 손잡이가 달린 '진짜 문'
	FHousingDesc tHousingDesc = {};

	shared_ptr<CHousingPrefab> spHousingPrefab = nullptr;
	spHousingPrefab = static_pointer_cast<CHousingPrefab>(CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::WOODEN_DOOR));

	tHousingDesc.m_bActive					= true;
	tHousingDesc.m_wstrGameObjectName		= L"Housing_Wooden_Door";
	tHousingDesc.m_eMobilityType			= EMobilityType::STATIC;
	tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

	tHousingDesc.m_vInitScale				= spHousingPrefab->GetScale().value();
	tHousingDesc.m_vInitRotate				= spHousingPrefab->GetRotate().value();
	tHousingDesc.m_vInitPosition			= GetPosition().value();

	tHousingDesc.m_fPivotScaleRatio			= spHousingPrefab->GetPivotScaleRatio();

	tHousingDesc.m_iArchitectureUnlockLevel = 0;
	tHousingDesc.m_eArchitectureType		= EArchitectureType::WOODEN_DOOR;

	tHousingDesc.m_vColliderExtents			= Vector3(0.1f, 0.1f, 0.1f);
	tHousingDesc.m_vColliderCenter			= spHousingPrefab->GetColliderCenter();
	tHousingDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tHousingDesc.m_eBuildType				= EBuildType::DOOR;
	tHousingDesc.m_eBuildDir				= spHousingPrefab->GetBuildDir();
	tHousingDesc.m_umapNeededBuildTypes		= spHousingPrefab->GetNeededBuildTypes();
	if (spHousingPrefab->GetAttachedHousing())
	{
		tHousingDesc.m_spHousingLump = spHousingPrefab->GetAttachedHousing()->GetHousingLump();
	}
	else
	{

	}
	tHousingDesc.m_tpBuildIndex				= spHousingPrefab->GetBuildIndex();
	tHousingDesc.m_iFloor					= spHousingPrefab->GetFloor();
	tHousingDesc.m_wstrShaderName			= spHousingPrefab->GetShaderName();
	tHousingDesc.m_wstrModelFilePath		= L"../../Resource/Models/Dat/NonAnim/Architecture/Housing/Door_Wood.dat";

	m_spDoorParts = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));
	CHECKF(m_spDoorParts, L"Failed to Create Door Parts : CDoor");

	return S_OK;
}

HRESULT Client::CDoor::InitializeColliders()
{
	FBoxComponentDesc tBoxComponentDesc;

	#pragma region LeftSide
	// box compzonent desc
	tBoxComponentDesc.m_vExtents			= Vector3(0.1f, 1.25f, 0.675f);
	tBoxComponentDesc.m_vLocalPosition		= Vector3(0.f, 1.25f, 1.25f);
	tBoxComponentDesc.m_vLocalRotate		= Vector3(0.f, 0.f, 0.f);

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
	tBoxComponentDesc.m_wstrComponentName	= L"Left_BoxComponent";
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	m_spLeftBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	AddComponent(L"Left_BoxComponent", m_spLeftBoxComponent);
#pragma endregion
	#pragma region RightSide
	// box compzonent desc
	tBoxComponentDesc.m_vExtents			= Vector3(0.1f, 1.25f, 0.675f);
	tBoxComponentDesc.m_vLocalPosition		= Vector3(0.f, 1.25f, -1.25f);
	tBoxComponentDesc.m_vLocalRotate		= Vector3(0.f, 0.f, 0.f);

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
	tBoxComponentDesc.m_wstrComponentName	= L"Right_BoxComponent";
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	m_spRightBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	AddComponent(L"Right_BoxComponent", m_spRightBoxComponent);
#pragma endregion

	return S_OK;
}

void Client::CDoor::SynchronizationDoorPartsSRT()
{
	if (!m_spDoorParts) { return; }

	Vector3 vNormalizedForward	= Vector3(0.f, 0.f, 1.f);
	Vector3 vNormalizedRight	= Vector3(1.f, 0.f, 0.f);
	GetForwardVector().value().Normalize(vNormalizedForward);
	GetRightVector().value().Normalize(vNormalizedRight);

	m_spDoorParts->SetPosition(GetPosition().value() - vNormalizedForward * 0.52f);

	if (m_bDoorOpend)
	{
		m_spDoorParts->LookAt(m_spDoorParts->GetPosition().value() - GetRightVector().value());
	}
	else
	{
		m_spDoorParts->LookAt(m_spDoorParts->GetPosition().value() + GetForwardVector().value());
	}
}

void Client::CDoor::DoorInteraction()
{
	shared_ptr<CPlayer> spPlayer = CGameManager::GetInstance()->GetPlayer();
	if (!spPlayer) { return; }
	if (!m_spDoorParts) { return; }

	if (2.f > Vector3::Distance(GetPosition().value(), spPlayer->GetPosition().value()) &&
		CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_F))
	{
		if (m_bDoorOpend)
		{
			m_wpSoundManager.lock()->PlaySoundEx("DoorClose_BW.13926_653757370.338_189532983", 1.f, false);
		}
		else
		{
			m_wpSoundManager.lock()->PlaySoundEx("DoorOpenHollow_S011FO.338_189532983", 1.f, false);
		}

		m_bDoorOpend = !m_bDoorOpend;
	}
}
