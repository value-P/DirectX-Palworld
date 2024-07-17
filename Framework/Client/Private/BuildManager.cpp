// Default
#include "pch.h"
#include "BuildManager.h"

// Module, Manager
#include "EngineModule.h"
#include "ClientModule.h"
#include "CameraManager.h"
#include "PhysXManager.h"
#include "InputManager.h"
#include "GameManager.h"
#include "UIManager.h"
#include "SoundManager.h"

// Level
#include "Level.h"

// GameObject
#include "ArchitecturePrefab.h"
#include "HousingPrefab.h"
#include "StonePitPrefab.h"
#include "LumberYardPrefab.h"
#include "LumberYardPrefab.h"

#include "Housing.h"
#include "HousingLump.h"
#include "Door.h"

#include "WorkBench.h"
#include "PalBox.h"
#include "Chest.h"
#include "IronChest.h"
#include "RepairBench.h"
#include "HumanBed.h"
#include "PalBed.h"
#include "FeedingBox.h"
#include "Ranch.h"
#include "BerryFarm.h"
#include "BlastFurnace.h"
#include "SphereTable.h"
#include "WheatFarm.h"
#include "Pulverizer.h"
#include "KitchenMedieval.h"
#include "WeaponWorkBench.h"
#include "StonePit.h"
#include "LumberYard.h"

// Component
#include "ColliderComponent.h"

// System
#include "InventorySystem.h"

// Etc
#include "Mineral.h"
#include "Panel.h"

IMPLEMENT_SINGLETON(CBuildManager)

HRESULT Client::CBuildManager::Initialize()
{
	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();
	m_wpPhysXManager = CEngineModule::GetInstance()->GetPhysXManager();
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();

	ENSUREF(InitializeHousingPrefabs(), L"Failed to InitializeHousingPrafabs : CBuildManager");
	ENSUREF(InitializeNormalArchitecturePrefabs(), L"Failed to InitializeNormalArchitecturePrefabs : CBuildManager");

	return S_OK;
}

HRESULT Client::CBuildManager::BeginPlay()
{
	for (auto& pair : m_umapArchitecturePrefabs)
	{
		CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(pair.second);
		pair.second->SetActive(false);
	}

	m_wpInventorySystem = CClientModule::GetInstance()->GetGameManager()->GetInventorySystem();
	m_wpUIManager = CUIManager::GetInstance();

	return S_OK;
}

int32 Client::CBuildManager::Tick(float _fDeltaSeconds)
{
	MonitoringWaitQue();
	BuildInput();
	DestroyInput();
	SaveBowlingAlley();

	return 0;
}

void Client::CBuildManager::Release()
{
	for (auto& pair : m_umapHousingLumps)
	{
		// pair.second->Release();
		pair.second = nullptr;
	}

	for (auto& pair : m_umapArchitecturePrefabs)
	{
		pair.second->Release();
		pair.second = nullptr;
	}

	if (m_spCurArchitecturePrefab)
	{
		m_spCurArchitecturePrefab->Release();
		m_spCurArchitecturePrefab = nullptr;
	}

	m_spInstance.reset();
	m_spInstance = nullptr;
}

HRESULT Client::CBuildManager::InitializeHousingPrefabs()
{
#pragma region 하우징
	shared_ptr<CHousingPrefab> spHousingFreefab = nullptr;
	FHousingPrefabDesc tHousingPrefabDesc = {};
	unordered_map<EBuildType, EBuildType> umapNeededBuildTypes;

	// 공용
	tHousingPrefabDesc.m_bActive = false;
	tHousingPrefabDesc.m_spPlayer = nullptr;
	tHousingPrefabDesc.m_wstrShaderName = L"FX_NonAnimMesh_PNT2T";
	tHousingPrefabDesc.m_tBuildStatus.m_bBuildCompleted = true;

	// WOODEN_FOUNDATION
	tHousingPrefabDesc.m_wstrGameObjectName = L"Wooden Foundation";

	tHousingPrefabDesc.m_fPivotScaleRatio = 0.01f;
	tHousingPrefabDesc.m_iArchitectureUnlockLevel = 0;
	tHousingPrefabDesc.m_eArchitectureType = EArchitectureType::WOODEN_FOUNDATION;

#pragma region 필요 재료

	vector<shared_ptr<CMineral>> vecMinerals;
	vector<uint32> vecMineralCounts;
	vecMinerals.reserve(1);
	vecMinerals.resize(1);
	vecMineralCounts.reserve(1);
	vecMineralCounts.resize(1);

	FMineralDesc tMineralDesc;
	tMineralDesc.m_eCurItemType = EItemType::WOOD;
	tMineralDesc.m_eCurMineralType = EMineralType::WOOD;
	tMineralDesc.m_iItemCount = 999;

	// 나무 2개
	vecMinerals[0] = CMineral::Create(make_shared<FMineralDesc>(tMineralDesc));
	vecMineralCounts[0] = 2;

	tHousingPrefabDesc.m_vecNeededMinerals = vecMinerals;
	tHousingPrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

#pragma endregion

	tHousingPrefabDesc.m_vColliderExtents = Vector3(2.f, 0.5f, 2.f);
	tHousingPrefabDesc.m_vColliderCenter = Vector3(0.f, -0.5f, 0.f);
	tHousingPrefabDesc.m_vColliderRotate = Vector3(0.f, 0.f, 0.f);

	tHousingPrefabDesc.m_eBuildType = EBuildType::FOUNDATION;
	umapNeededBuildTypes.clear();
	umapNeededBuildTypes.emplace(EBuildType::NORMAL, EBuildType::NORMAL);
	umapNeededBuildTypes.emplace(EBuildType::FOUNDATION, EBuildType::FOUNDATION);
	tHousingPrefabDesc.m_umapNeededBuildTypes		= umapNeededBuildTypes;
	tHousingPrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/Housing/Floor_Wood.dat";

	spHousingFreefab = CHousingPrefab::Create(make_shared<FHousingPrefabDesc>(tHousingPrefabDesc));
	CHECKF(spHousingFreefab, L"Failed to Create [Wooden Foundation] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WOODEN_FOUNDATION, spHousingFreefab);

	// WOODEN_WALL
	tHousingPrefabDesc.m_wstrGameObjectName			= L"Wooden Wall";

	tHousingPrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tHousingPrefabDesc.m_iArchitectureUnlockLevel	= 0;
	tHousingPrefabDesc.m_eArchitectureType			= EArchitectureType::WOODEN_WALL;

	#pragma region 필요 재료

	vecMinerals.reserve(1);
	vecMinerals.resize(1);
	vecMineralCounts.reserve(1);
	vecMineralCounts.resize(1);

	tMineralDesc.m_eCurItemType		= EItemType::WOOD;
	tMineralDesc.m_eCurMineralType	= EMineralType::WOOD;
	tMineralDesc.m_iItemCount		= 999;

	// 나무 2개 
	vecMinerals[0] = CMineral::Create(make_shared<FMineralDesc>(tMineralDesc));
	vecMineralCounts[0] = 2;

	tHousingPrefabDesc.m_vecNeededMinerals = vecMinerals;
	tHousingPrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tHousingPrefabDesc.m_vColliderExtents			= Vector3(0.1f, 1.5f, 2.f);
	tHousingPrefabDesc.m_vColliderCenter			= Vector3(0.f, 1.5f, 0.f);
	tHousingPrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tHousingPrefabDesc.m_eBuildType					= EBuildType::WALL;
	umapNeededBuildTypes.clear();
	umapNeededBuildTypes.emplace(EBuildType::FOUNDATION, EBuildType::FOUNDATION);
	umapNeededBuildTypes.emplace(EBuildType::ROOF, EBuildType::ROOF);
	umapNeededBuildTypes.emplace(EBuildType::WALL, EBuildType::WALL);
	tHousingPrefabDesc.m_umapNeededBuildTypes		= umapNeededBuildTypes;
	tHousingPrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/Housing/Wall_Wood.dat";

	spHousingFreefab = CHousingPrefab::Create(make_shared<FHousingPrefabDesc>(tHousingPrefabDesc));
	CHECKF(spHousingFreefab, L"Failed to Create [Wooden Wall] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WOODEN_WALL, spHousingFreefab);


	// WOODEN_ROOF
	tHousingPrefabDesc.m_wstrGameObjectName			= L"Wooden Roof";

	tHousingPrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tHousingPrefabDesc.m_iArchitectureUnlockLevel	= 0;
	tHousingPrefabDesc.m_eArchitectureType			= EArchitectureType::WOODEN_ROOF;

	#pragma region 필요 재료

	vecMinerals.reserve(1);
	vecMinerals.resize(1);
	vecMineralCounts.reserve(1);
	vecMineralCounts.resize(1);

	tMineralDesc.m_eCurItemType		= EItemType::WOOD;
	tMineralDesc.m_eCurMineralType	= EMineralType::WOOD;
	tMineralDesc.m_iItemCount		= 999;

	// 나무 2개
	vecMinerals[0] = CMineral::Create(make_shared<FMineralDesc>(tMineralDesc));
	vecMineralCounts[0] = 2;

	tHousingPrefabDesc.m_vecNeededMinerals = vecMinerals;
	tHousingPrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tHousingPrefabDesc.m_vColliderExtents			= Vector3(2.f, 0.2f, 2.f);
	tHousingPrefabDesc.m_vColliderCenter			= Vector3(0.f, -0.1f, 0.f);
	tHousingPrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tHousingPrefabDesc.m_eBuildType					= EBuildType::ROOF;
	umapNeededBuildTypes.clear();
	umapNeededBuildTypes.emplace(EBuildType::FOUNDATION, EBuildType::FOUNDATION);
	umapNeededBuildTypes.emplace(EBuildType::ROOF, EBuildType::ROOF);
	umapNeededBuildTypes.emplace(EBuildType::WALL, EBuildType::WALL);
	umapNeededBuildTypes.emplace(EBuildType::SLOPEDROOF, EBuildType::SLOPEDROOF);
	tHousingPrefabDesc.m_umapNeededBuildTypes		= umapNeededBuildTypes;
	tHousingPrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/Housing/Roof_Wood.dat";

	spHousingFreefab = CHousingPrefab::Create(make_shared<FHousingPrefabDesc>(tHousingPrefabDesc));
	CHECKF(spHousingFreefab, L"Failed to Create [Wooden Roof] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WOODEN_ROOF, spHousingFreefab);


	// WOODEN_STAIRS
	tHousingPrefabDesc.m_wstrGameObjectName = L"Wooden Stairs";

	tHousingPrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tHousingPrefabDesc.m_iArchitectureUnlockLevel	= 0;
	tHousingPrefabDesc.m_eArchitectureType			= EArchitectureType::WOODEN_STAIRS;

	#pragma region 필요 재료

	vecMinerals.reserve(1);
	vecMinerals.resize(1);
	vecMineralCounts.reserve(1);
	vecMineralCounts.resize(1);

	tMineralDesc.m_eCurItemType		= EItemType::WOOD;
	tMineralDesc.m_eCurMineralType	= EMineralType::WOOD;
	tMineralDesc.m_iItemCount		= 999;

	// 나무 2개
	vecMinerals[0] = CMineral::Create(make_shared<FMineralDesc>(tMineralDesc));
	vecMineralCounts[0] = 2;

	tHousingPrefabDesc.m_vecNeededMinerals = vecMinerals;
	tHousingPrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tHousingPrefabDesc.m_vColliderExtents			= Vector3(2.5f, 0.5f, 2.5f);
	tHousingPrefabDesc.m_vColliderCenter			= Vector3(0.f, 1.f, 0.f);
	tHousingPrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 39.f);

	tHousingPrefabDesc.m_eBuildType					= EBuildType::STAIRS;
	umapNeededBuildTypes.clear();
	umapNeededBuildTypes.emplace(EBuildType::FOUNDATION, EBuildType::FOUNDATION);
	umapNeededBuildTypes.emplace(EBuildType::ROOF, EBuildType::ROOF);
	umapNeededBuildTypes.emplace(EBuildType::STAIRS, EBuildType::STAIRS);
	tHousingPrefabDesc.m_umapNeededBuildTypes		= umapNeededBuildTypes;
	tHousingPrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/Housing/Stair_Wood.dat";

	spHousingFreefab = CHousingPrefab::Create(make_shared<FHousingPrefabDesc>(tHousingPrefabDesc));
	CHECKF(spHousingFreefab, L"Failed to Create [Wooden Stairs] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WOODEN_STAIRS, spHousingFreefab);


	// WOODEN_DOOR
	tHousingPrefabDesc.m_wstrGameObjectName = L"Wooden Door";

	tHousingPrefabDesc.m_fPivotScaleRatio = 0.01f;
	tHousingPrefabDesc.m_iArchitectureUnlockLevel = 0;
	tHousingPrefabDesc.m_eArchitectureType = EArchitectureType::WOODEN_DOOR;

	#pragma region 필요 재료

	vecMinerals.reserve(1);
	vecMinerals.resize(1);
	vecMineralCounts.reserve(1);
	vecMineralCounts.resize(1);

	tMineralDesc.m_eCurItemType		= EItemType::WOOD;
	tMineralDesc.m_eCurMineralType	= EMineralType::WOOD;
	tMineralDesc.m_iItemCount		= 999;

	// 나무 2개
	vecMinerals[0] = CMineral::Create(make_shared<FMineralDesc>(tMineralDesc));
	vecMineralCounts[0] = 2;

	tHousingPrefabDesc.m_vecNeededMinerals = vecMinerals;
	tHousingPrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tHousingPrefabDesc.m_vColliderExtents = Vector3(0.1f, 0.5f, 2.f);
	tHousingPrefabDesc.m_vColliderCenter = Vector3(0.f, 2.75f, 0.f);
	tHousingPrefabDesc.m_vColliderRotate = Vector3(0.f, 0.f, 0.f);

	tHousingPrefabDesc.m_eBuildType = EBuildType::WALL;
	umapNeededBuildTypes.clear();
	umapNeededBuildTypes.emplace(EBuildType::FOUNDATION, EBuildType::FOUNDATION);
	umapNeededBuildTypes.emplace(EBuildType::ROOF, EBuildType::ROOF);
	umapNeededBuildTypes.emplace(EBuildType::WALL, EBuildType::WALL);
	tHousingPrefabDesc.m_umapNeededBuildTypes = umapNeededBuildTypes;
	tHousingPrefabDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Architecture/Housing/Doorbase_Wood.dat";

	spHousingFreefab = CHousingPrefab::Create(make_shared<FHousingPrefabDesc>(tHousingPrefabDesc));
	CHECKF(spHousingFreefab, L"Failed to Create [Wooden Door] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WOODEN_DOOR, spHousingFreefab);


	// WOODEN_WINDOW_WALL
	tHousingPrefabDesc.m_wstrGameObjectName = L"Wooden Window Wall";

	tHousingPrefabDesc.m_fPivotScaleRatio = 0.01f;
	tHousingPrefabDesc.m_iArchitectureUnlockLevel = 0;
	tHousingPrefabDesc.m_eArchitectureType = EArchitectureType::WOODEN_WINDOW_WALL;

	#pragma region 필요 재료

	vecMinerals.reserve(1);
	vecMinerals.resize(1);
	vecMineralCounts.reserve(1);
	vecMineralCounts.resize(1);

	tMineralDesc.m_eCurItemType		= EItemType::WOOD;
	tMineralDesc.m_eCurMineralType	= EMineralType::WOOD;
	tMineralDesc.m_iItemCount		= 999;

	// 나무 2개
	vecMinerals[0] = CMineral::Create(make_shared<FMineralDesc>(tMineralDesc));
	vecMineralCounts[0] = 2;

	tHousingPrefabDesc.m_vecNeededMinerals = vecMinerals;
	tHousingPrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tHousingPrefabDesc.m_vColliderExtents = Vector3(0.1f, 1.5f, 2.f);
	tHousingPrefabDesc.m_vColliderCenter = Vector3(0.f, 1.5f, 0.f);
	tHousingPrefabDesc.m_vColliderRotate = Vector3(0.f, 0.f, 0.f);

	tHousingPrefabDesc.m_eBuildType = EBuildType::WALL;
	umapNeededBuildTypes.clear();
	umapNeededBuildTypes.emplace(EBuildType::FOUNDATION, EBuildType::FOUNDATION);
	umapNeededBuildTypes.emplace(EBuildType::ROOF, EBuildType::ROOF);
	umapNeededBuildTypes.emplace(EBuildType::WALL, EBuildType::WALL);
	tHousingPrefabDesc.m_umapNeededBuildTypes = umapNeededBuildTypes;
	tHousingPrefabDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Architecture/Housing/WallWindow_Wood.dat";

	spHousingFreefab = CHousingPrefab::Create(make_shared<FHousingPrefabDesc>(tHousingPrefabDesc));
	CHECKF(spHousingFreefab, L"Failed to Create [Wooden Window Wall] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WOODEN_WINDOW_WALL, spHousingFreefab);


	// WOODEN_TRYANGLE_WALL
	tHousingPrefabDesc.m_wstrGameObjectName = L"Wooden Tryangle Wall";

	tHousingPrefabDesc.m_fPivotScaleRatio = 0.01f;
	tHousingPrefabDesc.m_iArchitectureUnlockLevel = 0;
	tHousingPrefabDesc.m_eArchitectureType = EArchitectureType::WOODEN_TRYANGLE_WALL;

	#pragma region 필요 재료

	vecMinerals.reserve(1);
	vecMinerals.resize(1);
	vecMineralCounts.reserve(1);
	vecMineralCounts.resize(1);

	tMineralDesc.m_eCurItemType		= EItemType::WOOD;
	tMineralDesc.m_eCurMineralType	= EMineralType::WOOD;
	tMineralDesc.m_iItemCount		= 999;

	// 나무 2개
	vecMinerals[0] = CMineral::Create(make_shared<FMineralDesc>(tMineralDesc));
	vecMineralCounts[0] = 2;

	tHousingPrefabDesc.m_vecNeededMinerals = vecMinerals;
	tHousingPrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tHousingPrefabDesc.m_vColliderExtents = Vector3(0.1f, 1.2f, 1.2f);
	tHousingPrefabDesc.m_vColliderCenter = Vector3(0.f, 1.5f, 0.f);
	tHousingPrefabDesc.m_vColliderRotate = Vector3(0.f, 0.f, 0.f);

	tHousingPrefabDesc.m_eBuildType = EBuildType::WALL;
	umapNeededBuildTypes.clear();
	umapNeededBuildTypes.emplace(EBuildType::FOUNDATION, EBuildType::FOUNDATION);
	umapNeededBuildTypes.emplace(EBuildType::ROOF, EBuildType::ROOF);
	umapNeededBuildTypes.emplace(EBuildType::WALL, EBuildType::WALL);
	tHousingPrefabDesc.m_umapNeededBuildTypes = umapNeededBuildTypes;
	tHousingPrefabDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Architecture/Housing/WallTryangle_Wood.dat";

	spHousingFreefab = CHousingPrefab::Create(make_shared<FHousingPrefabDesc>(tHousingPrefabDesc));
	CHECKF(spHousingFreefab, L"Failed to Create [Wooden Tryangle Wall] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WOODEN_TRYANGLE_WALL, spHousingFreefab);


	// WOODEN_SLOPE_ROOF
	tHousingPrefabDesc.m_wstrGameObjectName = L"Wooden Slope Roof";

	tHousingPrefabDesc.m_fPivotScaleRatio = 0.01f;
	tHousingPrefabDesc.m_iArchitectureUnlockLevel = 0;
	tHousingPrefabDesc.m_eArchitectureType = EArchitectureType::WOODEN_SLOPE_ROOF;

	#pragma region 필요 재료

	vecMinerals.reserve(1);
	vecMinerals.resize(1);
	vecMineralCounts.reserve(1);
	vecMineralCounts.resize(1);

	tMineralDesc.m_eCurItemType		= EItemType::WOOD;
	tMineralDesc.m_eCurMineralType	= EMineralType::WOOD;
	tMineralDesc.m_iItemCount		= 999;

	// 나무 2개
	vecMinerals[0] = CMineral::Create(make_shared<FMineralDesc>(tMineralDesc));
	vecMineralCounts[0] = 2;

	tHousingPrefabDesc.m_vecNeededMinerals = vecMinerals;
	tHousingPrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tHousingPrefabDesc.m_vColliderExtents = Vector3(2.6f, 0.1f, 2.f);
	tHousingPrefabDesc.m_vColliderCenter = Vector3(0.f, 1.5f, 0.f);
	tHousingPrefabDesc.m_vColliderRotate = Vector3(0.f, 0.f, 39.f);

	tHousingPrefabDesc.m_eBuildType = EBuildType::SLOPEDROOF;
	umapNeededBuildTypes.clear();
	umapNeededBuildTypes.emplace(EBuildType::FOUNDATION, EBuildType::FOUNDATION);
	umapNeededBuildTypes.emplace(EBuildType::ROOF, EBuildType::ROOF);
	umapNeededBuildTypes.emplace(EBuildType::WALL, EBuildType::WALL);
	umapNeededBuildTypes.emplace(EBuildType::SLOPEDROOF, EBuildType::SLOPEDROOF);
	tHousingPrefabDesc.m_umapNeededBuildTypes = umapNeededBuildTypes;
	tHousingPrefabDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Architecture/Housing/RoofB_Wood.dat";

	spHousingFreefab = CHousingPrefab::Create(make_shared<FHousingPrefabDesc>(tHousingPrefabDesc));
	CHECKF(spHousingFreefab, L"Failed to Create [Wooden Slope Wall] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WOODEN_SLOPE_ROOF, spHousingFreefab);
	#pragma endregion

	return S_OK;
}

HRESULT Client::CBuildManager::InitializeNormalArchitecturePrefabs()
{
#pragma region 일반 건축물
	shared_ptr<CArchitecturePrefab> spArchitecturePrefab = nullptr;
	FArchitecturePrefabDesc tArchitecturePrefabDesc = {};

	// 공용
	tArchitecturePrefabDesc.m_bActive		 = false;
	tArchitecturePrefabDesc.m_spPlayer		 = nullptr;
	tArchitecturePrefabDesc.m_wstrShaderName = L"FX_NonAnimMesh_PNT2T";
	tArchitecturePrefabDesc.m_tBuildStatus.m_bBuildCompleted = true;

#pragma region 작업대
	// WorkBench
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"WorkBench";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 0;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::WORKBENCH;

	#pragma region 필요 재료

	vector<shared_ptr<CMineral>> vecMinerals;
	vector<uint32> vecMineralCounts;
	vecMinerals.reserve(1);
	vecMinerals.resize(1);
	vecMineralCounts.reserve(1);
	vecMineralCounts.resize(1);

	FMineralDesc tWorkBenchMineralDesc;
	tWorkBenchMineralDesc.m_eCurItemType	= EItemType::WOOD;
	tWorkBenchMineralDesc.m_eCurMineralType	= EMineralType::WOOD;
	tWorkBenchMineralDesc.m_iItemCount		= 999;

	// 나무 2개
	vecMinerals[0] = CMineral::Create(make_shared<FMineralDesc>(tWorkBenchMineralDesc));
	vecMineralCounts[0] = 2;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(0.5f, 0.5f, 0.7f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.5f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/WorkBenchPrimitive.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [WorkBench] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WORKBENCH, spArchitecturePrefab);
#pragma endregion 작업대
#pragma region 팰 상자
	// PalBox
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"PalBox";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::PAL_BOX;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 8개
	FMineralDesc tPalBoxMineralDesc1;
	tPalBoxMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tPalBoxMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tPalBoxMineralDesc1.m_iItemCount		= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tPalBoxMineralDesc1));
	vecMineralCounts[0] = 8;

	// 돌 3개
	FMineralDesc tPalBoxMineralDesc2;
	tPalBoxMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tPalBoxMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tPalBoxMineralDesc2.m_iItemCount		= 999;

	vecMinerals[1]		= CMineral::Create(make_shared<FMineralDesc>(tPalBoxMineralDesc2));
	vecMineralCounts[1] = 3;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(0.25f, 0.75f, 0.75f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.75f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/PalBox.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [PalBox] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::PAL_BOX, spArchitecturePrefab);
#pragma endregion 팰 상자
#pragma region 나무 상자
	// Wooden Chest
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Wooden Chest";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::WOODEN_CHEST;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 15개
	FMineralDesc tWoodenChestMineralDesc1;
	tWoodenChestMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tWoodenChestMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tWoodenChestMineralDesc1.m_iItemCount		= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tWoodenChestMineralDesc1));
	vecMineralCounts[0] = 5;

	// 돌 5개
	FMineralDesc tWoodenChestMineralDesc2;
	tWoodenChestMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tWoodenChestMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tWoodenChestMineralDesc2.m_iItemCount		= 999;

	vecMinerals[1]		= CMineral::Create(make_shared<FMineralDesc>(tWoodenChestMineralDesc2));
	vecMineralCounts[1] = 2;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(0.4f, 0.4f, 0.6f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.25f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/ChestWood.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Wooden Chest] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WOODEN_CHEST, spArchitecturePrefab);
#pragma endregion 나무 상자
#pragma region 금속 상자

	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Iron Chest";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.0001f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::IRON_BOX;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 30개
	FMineralDesc tIronChestMineralDesc1;
	tIronChestMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tIronChestMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tIronChestMineralDesc1.m_iItemCount			= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tIronChestMineralDesc1));
	vecMineralCounts[0] = 10;

	// 철 15개
	FMineralDesc tIronChestMineralDesc2;
	tIronChestMineralDesc2.m_eCurItemType		= EItemType::IRON_INGOT;
	tIronChestMineralDesc2.m_eCurMineralType	= EMineralType::IRON_INGOT;
	tIronChestMineralDesc2.m_iItemCount			= 999;

	vecMinerals[1]		= CMineral::Create(make_shared<FMineralDesc>(tIronChestMineralDesc2));
	vecMineralCounts[1] = 3;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(0.55f, 0.45f, 0.75f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.45f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/ChestCopper.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Iron Chest] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::IRON_BOX, spArchitecturePrefab);
#pragma endregion 금속 상자
#pragma region 수리대
	// Repair Bench
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Repair Bench";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::REPAIR_BENCH;

#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 10개
	FMineralDesc tRepairBenchMineralDesc1;
	tRepairBenchMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tRepairBenchMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tRepairBenchMineralDesc1.m_iItemCount		= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tRepairBenchMineralDesc1));
	vecMineralCounts[0] = 5;

	// 돌 10개
	FMineralDesc tRepairBenchMineralDesc2;
	tRepairBenchMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tRepairBenchMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tRepairBenchMineralDesc2.m_iItemCount		= 999;

	vecMinerals[1]		= CMineral::Create(make_shared<FMineralDesc>(tRepairBenchMineralDesc2));
	vecMineralCounts[1] = 5;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents = Vector3(0.5f, 0.5f, 0.65f);
	tArchitecturePrefabDesc.m_vColliderCenter = Vector3(0.f, 0.5f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate = Vector3(0.f, 0.f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/RepairTable.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Repair Bench] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::REPAIR_BENCH, spArchitecturePrefab);
#pragma endregion 수리대
#pragma region 침대
	// HUMAN_BED
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Human Bed";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::HUMAN_BED;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	//*나무 20개
	FMineralDesc tHumanBedMineralDesc1;
	tHumanBedMineralDesc1.m_eCurItemType	= EItemType::WOOD;
	tHumanBedMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tHumanBedMineralDesc1.m_iItemCount		= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tRepairBenchMineralDesc1));
	vecMineralCounts[0] = 10;

	// 섬유 5개
	FMineralDesc tHumanBedMineralDesc2;
	tHumanBedMineralDesc2.m_eCurItemType	= EItemType::STONE;
	tHumanBedMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tHumanBedMineralDesc2.m_iItemCount		= 999;

	vecMinerals[1]		= CMineral::Create(make_shared<FMineralDesc>(tRepairBenchMineralDesc2));
	vecMineralCounts[1] = 2;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(1.5f, 0.4f, 0.8f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.4f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/BedPrimitive.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Human Bed] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::HUMAN_BED, spArchitecturePrefab);
#pragma endregion 침대
#pragma region 팰 침대
	// HUMAN_BED
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Pal Bed";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::PAL_BED;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 10개
	FMineralDesc tPalBedMineralDesc1;
	tPalBedMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tPalBedMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tPalBedMineralDesc1.m_iItemCount		= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tPalBedMineralDesc1));
	vecMineralCounts[0] = 5;

	// 섬유 5개
	FMineralDesc tPalBedMineralDesc2;
	tPalBedMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tPalBedMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tPalBedMineralDesc2.m_iItemCount		= 999;

	vecMinerals[1]		= CMineral::Create(make_shared<FMineralDesc>(tPalBedMineralDesc2));
	vecMineralCounts[1] = 2;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(0.8f, 0.1f, 0.8f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.1f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.0f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/PalBedPrimitive.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Pal Bed] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::PAL_BED, spArchitecturePrefab);
#pragma endregion 팰 침대
#pragma region 먹이 상자
	// HUMAN_BED
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Feeding Box";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::FEEDING_BOX;

#pragma region 필요 재료

	vecMinerals.reserve(1);
	vecMinerals.resize(1);
	vecMineralCounts.reserve(1);
	vecMineralCounts.resize(1);

	// 나무 10개
	FMineralDesc tFeedingBoxMineralDesc1;
	tFeedingBoxMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tFeedingBoxMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tFeedingBoxMineralDesc1.m_iItemCount		= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tFeedingBoxMineralDesc1));
	vecMineralCounts[0] = 5;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(0.6f, 0.15f, 0.6f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.15f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.0f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/PalManger.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Feeding Box] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::FEEDING_BOX, spArchitecturePrefab);
#pragma endregion 먹이 상자
#pragma region 가축 목장
	// HUMAN_BED
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Ranch";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::RANCH;

	#pragma region 필요 재료

	vecMinerals.reserve(3);
	vecMinerals.resize(3);
	vecMineralCounts.reserve(3);
	vecMineralCounts.resize(3);

	// 나무 50개
	FMineralDesc tRanchMineralDesc1;
	tRanchMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tRanchMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tRanchMineralDesc1.m_iItemCount			= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tRanchMineralDesc1));
	vecMineralCounts[0] = 20;

	// 돌 20개
	FMineralDesc tRanchMineralDesc2;
	tRanchMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tRanchMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tRanchMineralDesc2.m_iItemCount			= 999;

	vecMinerals[1] = CMineral::Create(make_shared<FMineralDesc>(tRanchMineralDesc2));
	vecMineralCounts[1] = 10;

	// 섬유 30개
	FMineralDesc tRanchMineralDesc3;
	tRanchMineralDesc3.m_eCurItemType		= EItemType::WOOL;
	tRanchMineralDesc3.m_eCurMineralType	= EMineralType::WOOL;
	tRanchMineralDesc3.m_iItemCount			= 999;

	vecMinerals[2] = CMineral::Create(make_shared<FMineralDesc>(tRanchMineralDesc3));
	vecMineralCounts[2] = 5;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(4.5f, 1.f, 4.5f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.5f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/PalFarm.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Ranch] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::RANCH, spArchitecturePrefab);
#pragma endregion 가축 목장
#pragma region 열매 농원
	// HUMAN_BED
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Berry Farm";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::BERRY_FARM;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 20개
	FMineralDesc tBerryFarmMineralDesc1;
	tBerryFarmMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tBerryFarmMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tBerryFarmMineralDesc1.m_iItemCount			= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tBerryFarmMineralDesc1));
	vecMineralCounts[0] = 10;

	// 돌 20개
	FMineralDesc tBerryFarmMineralDesc2;
	tBerryFarmMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tBerryFarmMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tBerryFarmMineralDesc2.m_iItemCount			= 999;

	vecMinerals[1] = CMineral::Create(make_shared<FMineralDesc>(tBerryFarmMineralDesc2));
	vecMineralCounts[1] = 10;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(2.8f, 0.2f, 2.8f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, -0.1f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.0f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/FarmGround.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Berry Farm] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::BERRY_FARM, spArchitecturePrefab);
#pragma endregion 열매 농원
#pragma region 원시 화로
	// HUMAN_BED
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Blast Furnace";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::BLAST_FURNACE;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 20개
	FMineralDesc tBlastFurnaceMineralDesc1;
	tBlastFurnaceMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tBlastFurnaceMineralDesc1.m_eCurMineralType		= EMineralType::WOOD;
	tBlastFurnaceMineralDesc1.m_iItemCount			= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tBlastFurnaceMineralDesc1));
	vecMineralCounts[0] = 10;

	// 돌 50개
	FMineralDesc tBlastFurnaceMineralDesc2;
	tBlastFurnaceMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tBlastFurnaceMineralDesc2.m_eCurMineralType		= EMineralType::STONE;
	tBlastFurnaceMineralDesc2.m_iItemCount			= 999;

	vecMinerals[1] = CMineral::Create(make_shared<FMineralDesc>(tBlastFurnaceMineralDesc2));
	vecMineralCounts[1] = 20;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(0.45f, 0.45f, 0.45f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.45f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.0f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/BlastFurnacePrimitive.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Blast Furnace] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::BLAST_FURNACE, spArchitecturePrefab);
#pragma endregion 원시 화로
#pragma region 스피어 제작대
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Sphere Table";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.0001f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::SPHERE_TABLE;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 30개
	FMineralDesc tSphereTableMineralDesc1;
	tSphereTableMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tSphereTableMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tSphereTableMineralDesc1.m_iItemCount		= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tSphereTableMineralDesc1));
	vecMineralCounts[0] = 15;

	// 철 10개
	FMineralDesc tSphereTableMineralDesc2;
	tSphereTableMineralDesc2.m_eCurItemType		= EItemType::IRON_INGOT;
	tSphereTableMineralDesc2.m_eCurMineralType	= EMineralType::IRON_INGOT;
	tSphereTableMineralDesc2.m_iItemCount		= 999;

	vecMinerals[1] = CMineral::Create(make_shared<FMineralDesc>(tSphereTableMineralDesc2));
	vecMineralCounts[1] = 5;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(0.5f, 0.5f, 1.f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.5f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/PalSphereFactoryMedieval.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Sphere Table] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::SPHERE_TABLE, spArchitecturePrefab);
#pragma endregion 스피어 제작대
#pragma region 밀 농원
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Wheat Farm";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.01f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::WHEAT_FARM;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 35개
	FMineralDesc tWheatFarmMineralDesc1;
	tWheatFarmMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tWheatFarmMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tWheatFarmMineralDesc1.m_iItemCount			= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tWheatFarmMineralDesc1));
	vecMineralCounts[0] = 15;

	// 돌 35개
	FMineralDesc tWheatFarmMineralDesc2;
	tWheatFarmMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tWheatFarmMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tWheatFarmMineralDesc2.m_iItemCount			= 999;

	vecMinerals[1] = CMineral::Create(make_shared<FMineralDesc>(tWheatFarmMineralDesc2));
	vecMineralCounts[1] = 15;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(2.8f, 0.2f, 2.8f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, -0.1f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.0f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/FarmGround.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Wheat Farm] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WHEAT_FARM, spArchitecturePrefab);
#pragma endregion 밀 농원
#pragma region 제분기
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Mill";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.0001f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::MILL;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 50개
	FMineralDesc tMillMineralDesc1;
	tMillMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tMillMineralDesc1.m_eCurMineralType		= EMineralType::WOOD;
	tMillMineralDesc1.m_iItemCount			= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tWheatFarmMineralDesc1));
	vecMineralCounts[0] = 20;

	// 돌 40개
	FMineralDesc tMillMineralDesc2;
	tMillMineralDesc2.m_eCurItemType	= EItemType::STONE;
	tMillMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tMillMineralDesc2.m_iItemCount		= 999;

	vecMinerals[1] = CMineral::Create(make_shared<FMineralDesc>(tMillMineralDesc2));
	vecMineralCounts[1] = 20;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(1.25f, 1.25f, 1.25f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 1.f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.0f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/Pulverizer.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Mill] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::MILL, spArchitecturePrefab);
#pragma endregion 제분기
#pragma region 조리 냄비
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Pot";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.0001f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 7;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::POT;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 20개
	FMineralDesc tPotMineralDesc1;
	tPotMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tPotMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tPotMineralDesc1.m_iItemCount		= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tPotMineralDesc1));
	vecMineralCounts[0] = 10;

	// 철 15개
	FMineralDesc tPotMineralDesc2;
	tPotMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tPotMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tPotMineralDesc2.m_iItemCount		= 999;

	vecMinerals[1] = CMineral::Create(make_shared<FMineralDesc>(tPotMineralDesc2));
	vecMineralCounts[1] = 5;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(0.35f, 0.35f, 0.35f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.35f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.0f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/KitchenMedieval.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Pot] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::POT, spArchitecturePrefab);
#pragma endregion 조리 냄비
#pragma region 무기 제작대
	tArchitecturePrefabDesc.m_wstrGameObjectName		= L"Weapon Factory";

	tArchitecturePrefabDesc.m_fPivotScaleRatio			= 0.0001f;
	tArchitecturePrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tArchitecturePrefabDesc.m_eArchitectureType			= EArchitectureType::WEAPON_WORKBENCH;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 50개
	FMineralDesc tWeaponFactoryMineralDesc1;
	tWeaponFactoryMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tWeaponFactoryMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tWeaponFactoryMineralDesc1.m_iItemCount			= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tWeaponFactoryMineralDesc1));
	vecMineralCounts[0] = 25;

	// 돌 10개
	FMineralDesc tWeaponFactoryMineralDesc2;
	tWeaponFactoryMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tWeaponFactoryMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tWeaponFactoryMineralDesc2.m_iItemCount			= 999;

	vecMinerals[1] = CMineral::Create(make_shared<FMineralDesc>(tWeaponFactoryMineralDesc2));
	vecMineralCounts[1] = 10;

	tArchitecturePrefabDesc.m_vecNeededMinerals = vecMinerals;
	tArchitecturePrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tArchitecturePrefabDesc.m_vColliderExtents			= Vector3(0.5f, 0.5f, 1.f);
	tArchitecturePrefabDesc.m_vColliderCenter			= Vector3(0.f, 0.5f, 0.f);
	tArchitecturePrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tArchitecturePrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/WeaponFactoryMedieval.dat";

	spArchitecturePrefab = CArchitecturePrefab::Create(make_shared<FArchitecturePrefabDesc>(tArchitecturePrefabDesc));
	CHECKF(spArchitecturePrefab, L"Failed to Create [Weapon Factory] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::WEAPON_WORKBENCH, spArchitecturePrefab);
#pragma endregion 무기 제작대
#pragma region 벌목장
	// LUMBERYARD
	FLumberYardPrefabDesc tLumberYardPrefabDesc = {};
	tLumberYardPrefabDesc.m_bActive							= false;
	tLumberYardPrefabDesc.m_spPlayer						= nullptr;
	tLumberYardPrefabDesc.m_wstrShaderName					= L"FX_NonAnimMesh_PNT2T";
	tLumberYardPrefabDesc.m_tBuildStatus.m_bBuildCompleted	= true;

	tLumberYardPrefabDesc.m_wstrGameObjectName				= L"Lumber Yard";
	tLumberYardPrefabDesc.m_fPivotScaleRatio			= 0.0001f;
	tLumberYardPrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tLumberYardPrefabDesc.m_eArchitectureType			= EArchitectureType::LUMBERYARD;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 50개
	FMineralDesc tLumberYardMineralDesc1;
	tLumberYardMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tLumberYardMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tLumberYardMineralDesc1.m_iItemCount		= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tLumberYardMineralDesc1));
	vecMineralCounts[0] = 20;

	// 돌 20개
	FMineralDesc tLumberYardMineralDesc2;
	tLumberYardMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tLumberYardMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tLumberYardMineralDesc2.m_iItemCount		= 999;

	vecMinerals[1]		= CMineral::Create(make_shared<FMineralDesc>(tLumberYardMineralDesc2));
	vecMineralCounts[1] = 10;

	tLumberYardPrefabDesc.m_vecNeededMinerals = vecMinerals;
	tLumberYardPrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tLumberYardPrefabDesc.m_vColliderExtents	= Vector3(3.25f, 0.2f, 2.1f);
	tLumberYardPrefabDesc.m_vColliderCenter		= Vector3(0.f, -0.1f, 0.f);
	tLumberYardPrefabDesc.m_vColliderRotate		= Vector3(0.f, 0.f, 0.f);

	tLumberYardPrefabDesc.m_wstrModelFilePath		= L"../../Resource/Models/Dat/NonAnim/Architecture/Loggingcamp_Acce.dat";

	shared_ptr<CLumberYardPrefab> spLumberYardPrefab = CLumberYardPrefab::Create(make_shared<FLumberYardPrefabDesc>(tLumberYardPrefabDesc));
	CHECKF(spLumberYardPrefab, L"Failed to Create [Quarry] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::LUMBERYARD, spLumberYardPrefab);
#pragma endregion 벌목장
#pragma region 채석장
	// QUARRY
	FStonePitPrefabDesc tStonePitPrefabDesc = {};
	tStonePitPrefabDesc.m_bActive							= false;
	tStonePitPrefabDesc.m_spPlayer							= nullptr;
	tStonePitPrefabDesc.m_wstrShaderName					= L"FX_NonAnimMesh_PNT2T";
	tStonePitPrefabDesc.m_tBuildStatus.m_bBuildCompleted	= true;
	tStonePitPrefabDesc.m_wstrGameObjectName				= L"Quarry";

	tStonePitPrefabDesc.m_fPivotScaleRatio			= 0.0001f;
	tStonePitPrefabDesc.m_iArchitectureUnlockLevel	= 1;
	tStonePitPrefabDesc.m_eArchitectureType			= EArchitectureType::QUARRY;

	#pragma region 필요 재료

	vecMinerals.reserve(2);
	vecMinerals.resize(2);
	vecMineralCounts.reserve(2);
	vecMineralCounts.resize(2);

	// 나무 20개
	FMineralDesc tQuarryMineralDesc1;
	tQuarryMineralDesc1.m_eCurItemType		= EItemType::WOOD;
	tQuarryMineralDesc1.m_eCurMineralType	= EMineralType::WOOD;
	tQuarryMineralDesc1.m_iItemCount		= 999;

	vecMinerals[0]		= CMineral::Create(make_shared<FMineralDesc>(tQuarryMineralDesc1));
	vecMineralCounts[0] = 10;

	// 돌 50개
	FMineralDesc tQuarryMineralDesc2;
	tQuarryMineralDesc2.m_eCurItemType		= EItemType::STONE;
	tQuarryMineralDesc2.m_eCurMineralType	= EMineralType::STONE;
	tQuarryMineralDesc2.m_iItemCount		= 999;

	vecMinerals[1]		= CMineral::Create(make_shared<FMineralDesc>(tQuarryMineralDesc2));
	vecMineralCounts[1] = 20;

	tStonePitPrefabDesc.m_vecNeededMinerals = vecMinerals;
	tStonePitPrefabDesc.m_vecNeededMineralCounts = vecMineralCounts;
	vecMinerals.clear();
	vecMineralCounts.clear();

	#pragma endregion

	tStonePitPrefabDesc.m_vColliderExtents			= Vector3(3.25f, 0.2f, 2.1f);
	tStonePitPrefabDesc.m_vColliderCenter			= Vector3(0.f, -0.1f, 0.f);
	tStonePitPrefabDesc.m_vColliderRotate			= Vector3(0.f, 0.f, 0.f);

	tStonePitPrefabDesc.m_wstrModelFilePath			= L"../../Resource/Models/Dat/NonAnim/Architecture/StonePit_SM_StonePit_Acce.dat";

	shared_ptr<CStonePitPrefab> spStonePitPrefab = CStonePitPrefab::Create(make_shared<FStonePitPrefabDesc>(tStonePitPrefabDesc));
	CHECKF(spStonePitPrefab, L"Failed to Create [Quarry] : CBuildManager");

	m_umapArchitecturePrefabs.emplace(EArchitectureType::QUARRY, spStonePitPrefab);
#pragma endregion 채석장

#pragma endregion

	return S_OK;
}

HRESULT Client::CBuildManager::AddHousingLump(uint32 _iObjectID, std::shared_ptr<CHousingLump> _spHousingLumps)
{
	if (m_umapHousingLumps.contains(_iObjectID)) { return E_FAIL; }

	m_umapHousingLumps.emplace(_iObjectID, _spHousingLumps);
	return S_OK;
}

HRESULT Client::CBuildManager::RemoveHousingLump(uint32 _iObjectID)
{
	if (!m_umapHousingLumps.contains(_iObjectID)) { return E_FAIL; }

	m_umapHousingLumps.erase(_iObjectID);
	return S_OK;
}

shared_ptr<CHousingLump> Client::CBuildManager::FindHousingLump(uint32 _iObjectID)
{
	if (!m_umapHousingLumps.contains(_iObjectID)) { return nullptr; }

	return m_umapHousingLumps[_iObjectID];
}

std::shared_ptr<CArchitecturePrefab> Client::CBuildManager::FindHousingPrefab(EArchitectureType _eArchitectureType)
{
	if (!m_umapArchitecturePrefabs.contains(_eArchitectureType)) { return nullptr; }

	return m_umapArchitecturePrefabs[_eArchitectureType];
}

HRESULT Client::CBuildManager::RemoveArchitecture(EArchitectureType _eArchitectureType, std::shared_ptr<CArchitecture> _spArchitecture)
{
	if (!m_umapBuiltArchitectures.contains(_eArchitectureType)) { return E_FAIL; }

	auto iter = m_umapBuiltArchitectures[_eArchitectureType].begin();

	for (; iter != m_umapBuiltArchitectures[_eArchitectureType].end(); ++iter)
	{
		if (*iter == _spArchitecture)
		{
			m_umapBuiltArchitectures[_eArchitectureType].erase(iter);
			return S_OK;
		}
	}

	return E_FAIL;
}

const std::optional<pair<std::shared_ptr<CMineral>, uint32>> Client::CBuildManager::GetArchitectureNeededMinerals(EArchitectureType _eArchitectureType, uint32 _iIndex)
{
	if (!m_umapArchitecturePrefabs.contains(_eArchitectureType)) { return nullopt; }

	const vector<shared_ptr<CMineral>>& vecNeededMinerals	= m_umapArchitecturePrefabs[_eArchitectureType]->GetNeededMinerals();
	const vector<uint32>& vecNeededMineralsCounts			= m_umapArchitecturePrefabs[_eArchitectureType]->GetNeededMineralCounts();

	if (vecNeededMinerals.size() - 1 < _iIndex) { return nullopt; }
	if (vecNeededMineralsCounts.size() - 1 < _iIndex) { return nullopt; }

	pair<shared_ptr<CMineral>, uint32> pair = {};
	
	pair.first = m_umapArchitecturePrefabs[_eArchitectureType]->GetNeededMinerals()[_iIndex];
	pair.second = m_umapArchitecturePrefabs[_eArchitectureType]->GetNeededMineralCounts()[_iIndex];

	return pair;
}

HRESULT Client::CBuildManager::OnBuildMode(EArchitectureType _eArchitectureType)
{
	if (!m_umapArchitecturePrefabs.contains(_eArchitectureType)) { return E_FAIL; }

	m_bBuildMode = true;
	m_umapArchitecturePrefabs[_eArchitectureType]->RecycleFromPool();

	m_spCurArchitecturePrefab = m_umapArchitecturePrefabs[_eArchitectureType];

	return S_OK;
}

HRESULT Client::CBuildManager::OffBuildMode()
{
	m_bBuildMode = false;
	m_bDestroyMode = false;

	if (m_spCurArchitecturePrefab)
	{
		m_spCurArchitecturePrefab->ReturnToPool();
	}

	m_spCurArchitecturePrefab = nullptr;
	m_wpDestroyExpectedArchitecture.lock() = nullptr;

	return S_OK;
}

HRESULT Client::CBuildManager::OnDestroyMode()
{
	if (m_bBuildMode) { return E_FAIL; }
	if (m_spCurArchitecturePrefab) { return E_FAIL; }

	m_bDestroyMode = true;

	return S_OK;
}

HRESULT Client::CBuildManager::OffDestroyMode()
{
	m_wpDestroyExpectedArchitecture.lock()->OffBluePrint();
	m_wpDestroyExpectedArchitecture.lock() = nullptr;

	m_bDestroyMode = false;

	return S_OK;
}

void Client::CBuildManager::BuildInput()
{
	if (!m_spCurArchitecturePrefab) { return; }

	if (m_wpInputManager.lock()->IsMouseDown(EMouseActionType::RIGHT_BUTTON))
	{
		// 해당 건축물의 재료의 개수가 현재 인벤토리 아이템의 개수만큼 있는지 확인합니다.
		std::vector<std::shared_ptr<CMineral>> vecMinerals = m_spCurArchitecturePrefab->GetNeededMinerals();
		std::vector<uint32> vecCounts = m_spCurArchitecturePrefab->GetNeededMineralCounts();

		for (int32 i = 0; i < vecMinerals.size(); i++)
		{
			EItemType eItemType = m_wpUIManager.lock()->ChangeMineralType(vecMinerals[i]->GetMineralType());
			if (static_cast<int32>(vecCounts[i]) <= m_wpInventorySystem.lock()->FindInventoryItemTotalNum(eItemType)){}
			else
			{
				// 재료가 부족할 경우 건물을 짓지 않고 빌드모드를 해제합니다.
				OffBuildMode();
				return;
			}
		}

		AddArchitectureToWaitQue(m_spCurArchitecturePrefab->GetArchitectureType());
	}
	if (m_wpInputManager.lock()->IsMouseDown(EMouseActionType::LEFT_BUTTON))
	{
		AddArchitectureToWaitQue(m_spCurArchitecturePrefab->GetArchitectureType());
		OffBuildMode();
	}
}

void Client::CBuildManager::DestroyInput()
{
	// 건축 파괴 모드
	if (m_wpInputManager.lock()->IsKeyDown(DIK_T))
	{
		m_bDestroyMode = !m_bDestroyMode;
		if (m_wpUIManager.lock()->GetTopPanel() == m_wpUIManager.lock()->FindParentsPanel(L"TestUI"))
		{
			CEngineModule::GetInstance()->GetSoundManager()->PlaySoundEx("KeyInputSound");
			m_wpUIManager.lock()->FindParentsPanel(L"TestUI")->SetActive(false);
			m_wpUIManager.lock()->ResetTopPanel();
		}
	}

	if (!m_bDestroyMode) { return; }

	PxRaycastHit tRaycastHit;
	int32 iCollisionFlag = (int32)ECollisionLayer::BUILDING;

	Vector3 vCamPosition = m_wpCameraManager.lock()->GetCurCameraWorldPosition();
	Vector3 vCamLook = m_wpCameraManager.lock()->GetCurCameraWorldForwardVector();

	if(m_wpPhysXManager.lock()->SingleRayCast(vCamPosition, vCamLook, 20.f, iCollisionFlag, &tRaycastHit))
	{
		if (!m_wpDestroyExpectedArchitecture.expired()) { m_wpDestroyExpectedArchitecture.lock()->OffBluePrint(); }

		// 건축물이 광선에 맞은 경우
		CColliderComponent* pColliderComponent = static_cast<CColliderComponent*>(tRaycastHit.actor->userData);
		m_wpDestroyExpectedArchitecture = static_pointer_cast<CArchitecture>(pColliderComponent->GetOwnerGameObject());

		// 블루 프린트 색상으로 변경합니다.
		m_wpDestroyExpectedArchitecture.lock()->OnBluePrint();

		if (m_wpInputManager.lock()->IsMouseDown(EMouseActionType::RIGHT_BUTTON))
		{
			m_wpDestroyExpectedArchitecture.lock()->DestroyArchitecture();
		}
		else if(m_wpInputManager.lock()->IsMouseDown(EMouseActionType::LEFT_BUTTON))
		{
			m_wpDestroyExpectedArchitecture.lock()->DestroyArchitecture();
			// OffDestroyMode();
		}
	}
	else
	{
		// 건축물이 광선에 안맞은 경우 블루프린트 모드를 취소합니다.
		if (!m_wpDestroyExpectedArchitecture.expired())
		{
			m_wpDestroyExpectedArchitecture.lock()->OffBluePrint();
			m_wpDestroyExpectedArchitecture.lock() = nullptr;
		}
	}
}

void Client::CBuildManager::SaveBowlingAlley()
{
	//if (m_wpInputManager.lock()->IsKeyDown(DIK_J))
	//{
	//	m_umapHousingLumps.begin()->second->SaveHousingLump();
	//}
}

void Client::CBuildManager::ChangeHousingPrefab()
{
	auto iter = m_umapArchitecturePrefabs.find(m_spCurArchitecturePrefab->GetArchitectureType());
	CHECKF(iter != m_umapArchitecturePrefabs.end(), L"iterator is end iterator : CBuildManager");

	m_spCurArchitecturePrefab->ReturnToPool();

	++iter;
	if (iter == m_umapArchitecturePrefabs.end())
	{
		iter = m_umapArchitecturePrefabs.begin();
	}

	m_spCurArchitecturePrefab = iter->second;
	m_spCurArchitecturePrefab->RecycleFromPool();
}

shared_ptr<CHousing> Client::CBuildManager::FindNearHousing(const Vector3& _vPos, float _fRange)
{
	shared_ptr<CHousing> spNearestHousing = nullptr;
	float fNearest = 1000000.f;

	for (auto& pair : m_umapHousingLumps)
	{
		FindRecursiveCombinedHousing(spNearestHousing, pair.second->GetFirstFoundation(), _vPos, _fRange, &fNearest);
	}

	return spNearestHousing;
}

void Client::CBuildManager::FindRecursiveCombinedHousing(shared_ptr<CHousing>& _spNearestHousing, shared_ptr<CHousing> _spHousing, const Vector3& _vPrefabPos, float _fRange, float* _pNearest)
{
	for (uint32 i = 0; i < (uint32)EBuildDir::ENUM_END; ++i)
	{
		shared_ptr<CHousing> spCombinedHousing = _spHousing->GetCombinedHousing((EBuildDir)i);
		if (nullptr == spCombinedHousing) { continue; }

		FindRecursiveCombinedHousing(_spNearestHousing, spCombinedHousing, _vPrefabPos, _fRange, _pNearest);
	}

	Vector3 vHousingPos = _spHousing->GetPosition().value();
	float fDistance = Vector3::Distance(_vPrefabPos, vHousingPos);

	if (_fRange < fDistance) { return; }
	if (*_pNearest >= fDistance)
	{
		_spNearestHousing = _spHousing;
		*_pNearest = fDistance;
	}

	return;
}

HRESULT Client::CBuildManager::AssignBaseCamp(std::shared_ptr<CPalBox> _spPalBox)
{
	if (m_spPalBox) { return E_FAIL; }

	m_spPalBox = _spPalBox;

	return S_OK;
}

bool Client::CBuildManager::IsExistBaseCamp()
{
	if (m_spPalBox && m_spPalBox->IsBuildComplete()) { return true; }

	return false;
}

bool Client::CBuildManager::IsInBaseCamp(const Vector3& _vBuildPos)
{
	if (!m_spPalBox || !m_spPalBox->IsBuildComplete()) { return false; }

	Vector3 vPalBoxPosRemoveY = m_spPalBox->GetPosition().value();
	vPalBoxPosRemoveY.y = 0.f;

	Vector3 vBuildPosRemoveY = _vBuildPos;
	vBuildPosRemoveY.y = 0.f;

	if (m_spPalBox->GetRadius() < Vector3::Distance(vPalBoxPosRemoveY, vBuildPosRemoveY))
	{
		return false;
	}

	return true;
}

void Client::CBuildManager::MonitoringWaitQue()
{
	/* 건축 대기 큐에 작업이 들어올 경우 건축합니다. */
	while (!m_queBuildWait.empty())
	{
		BuildArchitecture(m_queBuildWait.back());
		m_queBuildWait.pop();
	}
}

HRESULT Client::CBuildManager::BuildArchitecture(EArchitectureType _eArchitextureType)
{
	if (!m_umapArchitecturePrefabs.contains(_eArchitextureType)) { return E_FAIL; }
	if (!m_umapArchitecturePrefabs[_eArchitextureType]->IsBuildable()) { return E_FAIL; }

	#pragma region 하우징
	{
		shared_ptr<CHousing> spHousing = nullptr;
		FHousingDesc tHousingDesc = {};

		unordered_map<EBuildType, EBuildType> umapNeededBuildTypes;

		switch (_eArchitextureType)
		{
		#pragma region 나무 토대
		case EArchitectureType::WOODEN_FOUNDATION:
		{
			// 최초의 토대 건축인 경우(다른 하우징에 부착되지 않은 경우)
			if (!static_pointer_cast<CHousingPrefab>(m_umapArchitecturePrefabs[_eArchitextureType])->GetIsAttached())
			{
				/* HousingLump */
				shared_ptr<CHousingLump> spHousingLump = nullptr;
				FHousingLumpDesc tHousingLumpDesc = {};

				tHousingLumpDesc.m_bActive				= true;
				tHousingLumpDesc.m_wstrGameObjectName	= L"Housing Lump";
				tHousingLumpDesc.m_eMobilityType		= EMobilityType::STATIC;

				tHousingLumpDesc.m_vInitScale			= m_umapArchitecturePrefabs[_eArchitextureType]->GetScale().value();
				tHousingLumpDesc.m_vInitRotate			= m_umapArchitecturePrefabs[_eArchitextureType]->GetRotate().value();
				tHousingLumpDesc.m_vInitPosition		= m_umapArchitecturePrefabs[_eArchitextureType]->GetPosition().value();

				spHousingLump = CHousingLump::Create(make_shared<FHousingLumpDesc>(tHousingLumpDesc));
				CHECKF(spHousingLump, L"Failed to Create CHousingLump : CBuildManager");
				CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHousingLump);

				/* Housing */
				shared_ptr<CHousingPrefab> spHousingPrefab = nullptr;
				spHousingPrefab = static_pointer_cast<CHousingPrefab>(m_umapArchitecturePrefabs[_eArchitextureType]);

				tHousingDesc.m_bActive					= true;
				tHousingDesc.m_wstrGameObjectName		= L"Housing_First_Wooden_Foundation";
				tHousingDesc.m_eMobilityType			= EMobilityType::STATIC;

				tHousingDesc.m_vInitScale				= spHousingPrefab->GetScale().value();
				tHousingDesc.m_vInitRotate				= spHousingPrefab->GetRotate().value();
				tHousingDesc.m_vInitPosition			= spHousingPrefab->GetPosition().value();
				tHousingDesc.m_fPivotScaleRatio			= spHousingPrefab->GetPivotScaleRatio();

				tHousingDesc.m_iArchitectureUnlockLevel = 0;
				tHousingDesc.m_eArchitectureType		= EArchitectureType::WOODEN_FOUNDATION;

				tHousingDesc.m_vecNeededMinerals		= spHousingPrefab->GetNeededMinerals();
				tHousingDesc.m_vecNeededMineralCounts	= spHousingPrefab->GetNeededMineralCounts();

				tHousingDesc.m_vColliderExtents			= spHousingPrefab->GetColliderExtents();
				tHousingDesc.m_vColliderCenter			= spHousingPrefab->GetColliderCenter();
				tHousingDesc.m_vColliderRotate			= spHousingPrefab->GetColliderRotate();

				tHousingDesc.m_eBuildType				= EBuildType::FOUNDATION;
				tHousingDesc.m_eBuildDir				= EBuildDir::DOWN;
				tHousingDesc.m_umapNeededBuildTypes		= spHousingPrefab->GetNeededBuildTypes();
				tHousingDesc.m_spHousingLump			= spHousingLump;
				tHousingDesc.m_tpBuildIndex				= tuple(0, 0, 0);
				tHousingDesc.m_iFloor					= spHousingPrefab->GetFloor();
				tHousingDesc.m_wstrShaderName			= spHousingPrefab->GetShaderName();
				tHousingDesc.m_wstrModelFilePath		= spHousingPrefab->GetModelFilePath();

				tHousingDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_FOUNDATION;
				tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

				spHousing = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));
				CHECKF(spHousing, L"Failed to Create CHousing : CBuildManager");
				CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHousing);

				// 하우징끼리 연결시킵니다.
				// spHousingPrefab->GetAttachedHousing()->AddCombinedHousing(spHousingPrefab->GetBuildDir(), spHousing);
				// spHousing->AddCombinedHousing(DecideOpponentBuildDir(spHousingPrefab->GetBuildDir()), spHousingPrefab->GetAttachedHousing());

				// 하우징 덩어리에 토대를 추가합니다.
				spHousingLump->AddHousing(tHousingDesc.m_tpBuildIndex, spHousing);
				return S_OK;
			}
			// 최초의 토대 건축이 아닌 경우(다른 하우징에 부착된 경우)
			else
			{
				/* Housing */
				shared_ptr<CHousingPrefab> spHousingPrefab = nullptr;
				spHousingPrefab = static_pointer_cast<CHousingPrefab>(m_umapArchitecturePrefabs[_eArchitextureType]);

				tHousingDesc.m_bActive					= true;
				tHousingDesc.m_wstrGameObjectName		= L"Housing_Wooden_Foundation";
				tHousingDesc.m_eMobilityType			= EMobilityType::STATIC;

				tHousingDesc.m_vInitScale				= spHousingPrefab->GetScale().value();
				tHousingDesc.m_vInitRotate				= spHousingPrefab->GetRotate().value();
				tHousingDesc.m_vInitPosition			= spHousingPrefab->GetPosition().value();
				tHousingDesc.m_fPivotScaleRatio			= spHousingPrefab->GetPivotScaleRatio();

				tHousingDesc.m_iArchitectureUnlockLevel	= 0;
				tHousingDesc.m_eArchitectureType		= EArchitectureType::WOODEN_FOUNDATION;

				tHousingDesc.m_vecNeededMinerals		= spHousingPrefab->GetNeededMinerals();
				tHousingDesc.m_vecNeededMineralCounts	= spHousingPrefab->GetNeededMineralCounts();

				tHousingDesc.m_vColliderExtents			= spHousingPrefab->GetColliderExtents();
				tHousingDesc.m_vColliderCenter			= spHousingPrefab->GetColliderCenter();
				tHousingDesc.m_vColliderRotate			= spHousingPrefab->GetColliderRotate();

				tHousingDesc.m_eBuildType				= EBuildType::FOUNDATION;
				tHousingDesc.m_eBuildDir				= spHousingPrefab->GetBuildDir();
				tHousingDesc.m_umapNeededBuildTypes		= spHousingPrefab->GetNeededBuildTypes();
				tHousingDesc.m_spHousingLump			= spHousingPrefab->GetAttachedHousing()->GetHousingLump();
				tHousingDesc.m_tpBuildIndex				= spHousingPrefab->GetBuildIndex();
				tHousingDesc.m_iFloor					= spHousingPrefab->GetFloor();
				tHousingDesc.m_wstrShaderName			= spHousingPrefab->GetShaderName();
				tHousingDesc.m_wstrModelFilePath		= spHousingPrefab->GetModelFilePath();

				tHousingDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_FOUNDATION;
				tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

				spHousing = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));
				CHECKF(spHousing, L"Failed to Create CHousing : CBuildManager");
				CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHousing);

				// 하우징끼리 연결시킵니다.
				spHousingPrefab->GetAttachedHousing()->AddCombinedHousing(spHousingPrefab->GetBuildDir(), spHousing);
				spHousing->SetParentHousing(spHousingPrefab->GetBuildDir(), spHousingPrefab->GetAttachedHousing());
				// spHousing->AddCombinedHousing(DecideOpponentBuildDir(spHousingPrefab->GetBuildDir()), spHousingPrefab->GetAttachedHousing());

				// 하우징 덩어리에 토대를 추가합니다.
				tHousingDesc.m_spHousingLump->AddHousing(tHousingDesc.m_tpBuildIndex, spHousing);
			}

			break;
		}
#pragma endregion
		#pragma region 나무 벽
		case EArchitectureType::WOODEN_WALL:
		{
			shared_ptr<CHousingPrefab> spHousingPrefab = nullptr;
			spHousingPrefab = static_pointer_cast<CHousingPrefab>(m_umapArchitecturePrefabs[_eArchitextureType]);

			tHousingDesc.m_bActive					= true;
			tHousingDesc.m_wstrGameObjectName		= L"Housing_Wooden_Wall";
			tHousingDesc.m_eMobilityType			= EMobilityType::STATIC;

			tHousingDesc.m_vInitScale				= spHousingPrefab->GetScale().value();
			tHousingDesc.m_vInitRotate				= spHousingPrefab->GetRotate().value();
			tHousingDesc.m_vInitPosition			= spHousingPrefab->GetPosition().value();
			tHousingDesc.m_fPivotScaleRatio			= spHousingPrefab->GetPivotScaleRatio();

			tHousingDesc.m_iArchitectureUnlockLevel = 0;
			tHousingDesc.m_eArchitectureType		= EArchitectureType::WOODEN_WALL;

			tHousingDesc.m_vecNeededMinerals		= spHousingPrefab->GetNeededMinerals();
			tHousingDesc.m_vecNeededMineralCounts	= spHousingPrefab->GetNeededMineralCounts();

			tHousingDesc.m_vColliderExtents			= spHousingPrefab->GetColliderExtents();
			tHousingDesc.m_vColliderCenter			= spHousingPrefab->GetColliderCenter();
			tHousingDesc.m_vColliderRotate			= spHousingPrefab->GetColliderRotate();

			tHousingDesc.m_eBuildType				= EBuildType::WALL;
			tHousingDesc.m_eBuildDir				= spHousingPrefab->GetBuildDir();
			tHousingDesc.m_umapNeededBuildTypes		= spHousingPrefab->GetNeededBuildTypes();
			tHousingDesc.m_spHousingLump			= spHousingPrefab->GetAttachedHousing()->GetHousingLump();
			tHousingDesc.m_tpBuildIndex				= spHousingPrefab->GetBuildIndex();
			tHousingDesc.m_iFloor					= spHousingPrefab->GetFloor();
			tHousingDesc.m_wstrShaderName			= spHousingPrefab->GetShaderName();
			tHousingDesc.m_wstrModelFilePath		= spHousingPrefab->GetModelFilePath();

			tHousingDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_WALL;
			tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

			spHousing = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));
			CHECKF(spHousing, L"Failed to Create CHousing : CBuildManager");
			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHousing);

			// 하우징끼리 연결시킵니다.
			spHousingPrefab->GetAttachedHousing()->AddCombinedHousing(spHousingPrefab->GetBuildDir(), spHousing);
			spHousing->SetParentHousing(spHousingPrefab->GetBuildDir(), spHousingPrefab->GetAttachedHousing());
			break;
		}
#pragma endregion
		#pragma region 나무 계단
		case EArchitectureType::WOODEN_STAIRS:
		{
			shared_ptr<CHousingPrefab> spHousingPrefab = nullptr;
			spHousingPrefab = static_pointer_cast<CHousingPrefab>(m_umapArchitecturePrefabs[_eArchitextureType]);

			tHousingDesc.m_bActive					= true;
			tHousingDesc.m_wstrGameObjectName		= L"Housing_Wooden_Stairs";
			tHousingDesc.m_eMobilityType			= EMobilityType::STATIC;

			tHousingDesc.m_vInitScale				= spHousingPrefab->GetScale().value();
			tHousingDesc.m_vInitRotate				= spHousingPrefab->GetRotate().value();
			tHousingDesc.m_vInitPosition			= spHousingPrefab->GetPosition().value();
			tHousingDesc.m_fPivotScaleRatio			= spHousingPrefab->GetPivotScaleRatio();

			tHousingDesc.m_iArchitectureUnlockLevel = 0;
			tHousingDesc.m_eArchitectureType		= EArchitectureType::WOODEN_STAIRS;

			tHousingDesc.m_vecNeededMinerals		= spHousingPrefab->GetNeededMinerals();
			tHousingDesc.m_vecNeededMineralCounts	= spHousingPrefab->GetNeededMineralCounts();

			tHousingDesc.m_vColliderExtents			= spHousingPrefab->GetColliderExtents();
			tHousingDesc.m_vColliderCenter			= spHousingPrefab->GetColliderCenter();
			tHousingDesc.m_vColliderRotate			= spHousingPrefab->GetColliderRotate();

			tHousingDesc.m_eBuildType				= EBuildType::STAIRS;
			tHousingDesc.m_eBuildDir				= spHousingPrefab->GetBuildDir();
			tHousingDesc.m_umapNeededBuildTypes		= spHousingPrefab->GetNeededBuildTypes();
			tHousingDesc.m_spHousingLump			= spHousingPrefab->GetAttachedHousing()->GetHousingLump();
			tHousingDesc.m_tpBuildIndex				= spHousingPrefab->GetBuildIndex();
			tHousingDesc.m_iFloor					= spHousingPrefab->GetFloor();
			tHousingDesc.m_wstrShaderName			= spHousingPrefab->GetShaderName();
			tHousingDesc.m_wstrModelFilePath		= spHousingPrefab->GetModelFilePath();

			tHousingDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_STAIRS;
			tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

			spHousing = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));
			CHECKF(spHousing, L"Failed to Create CHousing : CBuildManager");
			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHousing);

			// 하우징끼리 연결시킵니다.
			spHousingPrefab->GetAttachedHousing()->AddCombinedHousing(spHousingPrefab->GetBuildDir(), spHousing);
			spHousing->SetParentHousing(spHousingPrefab->GetBuildDir(), spHousingPrefab->GetAttachedHousing());
			break;
		}
#pragma endregion
		#pragma region 나무 지붕
		case EArchitectureType::WOODEN_ROOF:
		{
			shared_ptr<CHousingPrefab> spHousingPrefab = nullptr;
			spHousingPrefab = static_pointer_cast<CHousingPrefab>(m_umapArchitecturePrefabs[_eArchitextureType]);

			tHousingDesc.m_bActive					= true;
			tHousingDesc.m_wstrGameObjectName		= L"Housing_Wooden_Roof";
			tHousingDesc.m_eMobilityType			= EMobilityType::STATIC;

			tHousingDesc.m_vInitScale				= spHousingPrefab->GetScale().value();
			tHousingDesc.m_vInitRotate				= spHousingPrefab->GetRotate().value();
			tHousingDesc.m_vInitPosition			= spHousingPrefab->GetPosition().value();
			tHousingDesc.m_fPivotScaleRatio			= spHousingPrefab->GetPivotScaleRatio();

			tHousingDesc.m_iArchitectureUnlockLevel = 0;
			tHousingDesc.m_eArchitectureType		= EArchitectureType::WOODEN_ROOF;

			tHousingDesc.m_vecNeededMinerals		= spHousingPrefab->GetNeededMinerals();
			tHousingDesc.m_vecNeededMineralCounts	= spHousingPrefab->GetNeededMineralCounts();

			tHousingDesc.m_vColliderExtents			= spHousingPrefab->GetColliderExtents();
			tHousingDesc.m_vColliderCenter			= spHousingPrefab->GetColliderCenter();
			tHousingDesc.m_vColliderRotate			= spHousingPrefab->GetColliderRotate();

			tHousingDesc.m_eBuildType				= EBuildType::ROOF;
			tHousingDesc.m_eBuildDir				= spHousingPrefab->GetBuildDir();
			tHousingDesc.m_umapNeededBuildTypes		= spHousingPrefab->GetNeededBuildTypes();
			tHousingDesc.m_spHousingLump			= spHousingPrefab->GetAttachedHousing()->GetHousingLump();
			tHousingDesc.m_tpBuildIndex				= spHousingPrefab->GetBuildIndex();
			tHousingDesc.m_iFloor					= spHousingPrefab->GetFloor();
			tHousingDesc.m_wstrShaderName			= spHousingPrefab->GetShaderName();
			tHousingDesc.m_wstrModelFilePath		= spHousingPrefab->GetModelFilePath();

			tHousingDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_ROOF;
			tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

			spHousing = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));
			CHECKF(spHousing, L"Failed to Create CHousing : CBuildManager");
			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHousing);

			// 하우징끼리 연결시킵니다.
			spHousingPrefab->GetAttachedHousing()->AddCombinedHousing(spHousingPrefab->GetBuildDir(), spHousing);
			spHousing->SetParentHousing(spHousingPrefab->GetBuildDir(), spHousingPrefab->GetAttachedHousing());

			// 하우징 덩어리에 지붕을 추가합니다.
			tHousingDesc.m_spHousingLump->AddHousing(tHousingDesc.m_tpBuildIndex, spHousing);
			break;
		}
		#pragma endregion
		#pragma region 나무 문
		case EArchitectureType::WOODEN_DOOR:
		{
			shared_ptr<CHousingPrefab> spHousingPrefab = nullptr;
			spHousingPrefab = static_pointer_cast<CHousingPrefab>(m_umapArchitecturePrefabs[_eArchitextureType]);

			tHousingDesc.m_bActive					= true;
			tHousingDesc.m_wstrGameObjectName		= L"Housing_Wooden_Door";
			tHousingDesc.m_eMobilityType			= EMobilityType::STATIC;

			tHousingDesc.m_vInitScale				= spHousingPrefab->GetScale().value();
			tHousingDesc.m_vInitRotate				= spHousingPrefab->GetRotate().value();
			tHousingDesc.m_vInitPosition			= spHousingPrefab->GetPosition().value();
			tHousingDesc.m_fPivotScaleRatio			= spHousingPrefab->GetPivotScaleRatio();

			tHousingDesc.m_iArchitectureUnlockLevel = 0;
			tHousingDesc.m_eArchitectureType		= EArchitectureType::WOODEN_DOOR;

			tHousingDesc.m_vecNeededMinerals		= spHousingPrefab->GetNeededMinerals();
			tHousingDesc.m_vecNeededMineralCounts	= spHousingPrefab->GetNeededMineralCounts();

			tHousingDesc.m_vColliderExtents			= spHousingPrefab->GetColliderExtents();
			tHousingDesc.m_vColliderCenter			= spHousingPrefab->GetColliderCenter();
			tHousingDesc.m_vColliderRotate			= spHousingPrefab->GetColliderRotate();

			tHousingDesc.m_eBuildType				= EBuildType::WALL;
			tHousingDesc.m_eBuildDir				= spHousingPrefab->GetBuildDir();
			tHousingDesc.m_umapNeededBuildTypes		= spHousingPrefab->GetNeededBuildTypes();
			tHousingDesc.m_spHousingLump			= spHousingPrefab->GetAttachedHousing()->GetHousingLump();
			tHousingDesc.m_tpBuildIndex				= spHousingPrefab->GetBuildIndex();
			tHousingDesc.m_iFloor					= spHousingPrefab->GetFloor();
			tHousingDesc.m_wstrShaderName			= spHousingPrefab->GetShaderName();
			tHousingDesc.m_wstrModelFilePath		= spHousingPrefab->GetModelFilePath();

			tHousingDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_DOOR;
			tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

			spHousing = CDoor::Create(make_shared<FHousingDesc>(tHousingDesc));
			CHECKF(spHousing, L"Failed to Create CHousing : CBuildManager");
			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHousing);

			// 하우징끼리 연결시킵니다.
			spHousingPrefab->GetAttachedHousing()->AddCombinedHousing(spHousingPrefab->GetBuildDir(), spHousing);
			spHousing->SetParentHousing(spHousingPrefab->GetBuildDir(), spHousingPrefab->GetAttachedHousing());
			break;
		}
#pragma endregion
		#pragma region 나무 창문 벽
		case EArchitectureType::WOODEN_WINDOW_WALL:
		{
			shared_ptr<CHousingPrefab> spHousingPrefab = nullptr;
			spHousingPrefab = static_pointer_cast<CHousingPrefab>(m_umapArchitecturePrefabs[_eArchitextureType]);

			tHousingDesc.m_bActive					= true;
			tHousingDesc.m_wstrGameObjectName		= L"Housing_Wooden_Window_Wall";
			tHousingDesc.m_eMobilityType			= EMobilityType::STATIC;

			tHousingDesc.m_vInitScale				= spHousingPrefab->GetScale().value();
			tHousingDesc.m_vInitRotate				= spHousingPrefab->GetRotate().value();
			tHousingDesc.m_vInitPosition			= spHousingPrefab->GetPosition().value();
			tHousingDesc.m_fPivotScaleRatio			= spHousingPrefab->GetPivotScaleRatio();

			tHousingDesc.m_iArchitectureUnlockLevel = 0;
			tHousingDesc.m_eArchitectureType		= EArchitectureType::WOODEN_WINDOW_WALL;

			tHousingDesc.m_vecNeededMinerals		= spHousingPrefab->GetNeededMinerals();
			tHousingDesc.m_vecNeededMineralCounts	= spHousingPrefab->GetNeededMineralCounts();

			tHousingDesc.m_vColliderExtents			= spHousingPrefab->GetColliderExtents();
			tHousingDesc.m_vColliderCenter			= spHousingPrefab->GetColliderCenter();
			tHousingDesc.m_vColliderRotate			= spHousingPrefab->GetColliderRotate();

			tHousingDesc.m_eBuildType				= EBuildType::WALL;
			tHousingDesc.m_eBuildDir				= spHousingPrefab->GetBuildDir();
			tHousingDesc.m_umapNeededBuildTypes		= spHousingPrefab->GetNeededBuildTypes();
			tHousingDesc.m_spHousingLump			= spHousingPrefab->GetAttachedHousing()->GetHousingLump();
			tHousingDesc.m_tpBuildIndex				= spHousingPrefab->GetBuildIndex();
			tHousingDesc.m_iFloor					= spHousingPrefab->GetFloor();
			tHousingDesc.m_wstrShaderName			= spHousingPrefab->GetShaderName();
			tHousingDesc.m_wstrModelFilePath		= spHousingPrefab->GetModelFilePath();

			tHousingDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_WINDOW_WALL;
			tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

			spHousing = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));
			CHECKF(spHousing, L"Failed to Create CHousing : CBuildManager");
			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHousing);

			// 하우징끼리 연결시킵니다.
			spHousingPrefab->GetAttachedHousing()->AddCombinedHousing(spHousingPrefab->GetBuildDir(), spHousing);
			spHousing->SetParentHousing(spHousingPrefab->GetBuildDir(), spHousingPrefab->GetAttachedHousing());
			break;
		}
		#pragma endregion
		#pragma region 나무 삼각 벽
		case EArchitectureType::WOODEN_TRYANGLE_WALL:
		{
			shared_ptr<CHousingPrefab> spHousingPrefab = nullptr;
			spHousingPrefab = static_pointer_cast<CHousingPrefab>(m_umapArchitecturePrefabs[_eArchitextureType]);

			tHousingDesc.m_bActive					= true;
			tHousingDesc.m_wstrGameObjectName		= L"Housing_Wooden_Tryangle_Wall";
			tHousingDesc.m_eMobilityType			= EMobilityType::STATIC;

			tHousingDesc.m_vInitScale				= spHousingPrefab->GetScale().value();
			tHousingDesc.m_vInitRotate				= spHousingPrefab->GetRotate().value();
			tHousingDesc.m_vInitPosition			= spHousingPrefab->GetPosition().value();
			tHousingDesc.m_fPivotScaleRatio			= spHousingPrefab->GetPivotScaleRatio();

			tHousingDesc.m_iArchitectureUnlockLevel = 0;
			tHousingDesc.m_eArchitectureType		= EArchitectureType::WOODEN_TRYANGLE_WALL;

			tHousingDesc.m_vecNeededMinerals		= spHousingPrefab->GetNeededMinerals();
			tHousingDesc.m_vecNeededMineralCounts	= spHousingPrefab->GetNeededMineralCounts();

			tHousingDesc.m_vColliderExtents			= spHousingPrefab->GetColliderExtents();
			tHousingDesc.m_vColliderCenter			= spHousingPrefab->GetColliderCenter();
			tHousingDesc.m_vColliderRotate			= spHousingPrefab->GetColliderRotate();

			tHousingDesc.m_eBuildType				= EBuildType::WALL;
			tHousingDesc.m_eBuildDir				= spHousingPrefab->GetBuildDir();
			tHousingDesc.m_umapNeededBuildTypes		= spHousingPrefab->GetNeededBuildTypes();
			tHousingDesc.m_spHousingLump			= spHousingPrefab->GetAttachedHousing()->GetHousingLump();
			tHousingDesc.m_tpBuildIndex				= spHousingPrefab->GetBuildIndex();
			tHousingDesc.m_iFloor					= spHousingPrefab->GetFloor();
			tHousingDesc.m_wstrShaderName			= spHousingPrefab->GetShaderName();
			tHousingDesc.m_wstrModelFilePath		= spHousingPrefab->GetModelFilePath();

			tHousingDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_TRYANGLE_WALL;
			tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

			spHousing = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));
			CHECKF(spHousing, L"Failed to Create CHousing : CBuildManager");
			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHousing);

			// 하우징끼리 연결시킵니다.
			spHousingPrefab->GetAttachedHousing()->AddCombinedHousing(spHousingPrefab->GetBuildDir(), spHousing);
			spHousing->SetParentHousing(spHousingPrefab->GetBuildDir(), spHousingPrefab->GetAttachedHousing());
			break;
		}
		#pragma endregion
		#pragma region 나무 경사 지붕
		case EArchitectureType::WOODEN_SLOPE_ROOF:
		{
			shared_ptr<CHousingPrefab> spHousingPrefab = nullptr;
			spHousingPrefab = static_pointer_cast<CHousingPrefab>(m_umapArchitecturePrefabs[_eArchitextureType]);

			tHousingDesc.m_bActive					= true;
			tHousingDesc.m_wstrGameObjectName		= L"Housing_Wooden_Window_Sloped_Roof";
			tHousingDesc.m_eMobilityType			= EMobilityType::STATIC;

			tHousingDesc.m_vInitScale				= spHousingPrefab->GetScale().value();
			tHousingDesc.m_vInitRotate				= spHousingPrefab->GetRotate().value();
			tHousingDesc.m_vInitPosition			= spHousingPrefab->GetPosition().value();
			tHousingDesc.m_fPivotScaleRatio			= spHousingPrefab->GetPivotScaleRatio();

			tHousingDesc.m_iArchitectureUnlockLevel = 0;
			tHousingDesc.m_eArchitectureType		= EArchitectureType::WOODEN_SLOPE_ROOF;

			tHousingDesc.m_vecNeededMinerals		= spHousingPrefab->GetNeededMinerals();
			tHousingDesc.m_vecNeededMineralCounts	= spHousingPrefab->GetNeededMineralCounts();

			tHousingDesc.m_vColliderExtents			= spHousingPrefab->GetColliderExtents();
			tHousingDesc.m_vColliderCenter			= spHousingPrefab->GetColliderCenter();
			tHousingDesc.m_vColliderRotate			= spHousingPrefab->GetColliderRotate();

			tHousingDesc.m_eBuildType				= EBuildType::ROOF;
			tHousingDesc.m_eBuildDir				= spHousingPrefab->GetBuildDir();
			tHousingDesc.m_umapNeededBuildTypes		= spHousingPrefab->GetNeededBuildTypes();
			tHousingDesc.m_spHousingLump			= spHousingPrefab->GetAttachedHousing()->GetHousingLump();
			tHousingDesc.m_tpBuildIndex				= spHousingPrefab->GetBuildIndex();
			tHousingDesc.m_iFloor					= spHousingPrefab->GetFloor();
			tHousingDesc.m_wstrShaderName			= spHousingPrefab->GetShaderName();
			tHousingDesc.m_wstrModelFilePath		= spHousingPrefab->GetModelFilePath();

			tHousingDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_SLOPE_ROOF;
			tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

			spHousing = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));
			CHECKF(spHousing, L"Failed to Create CHousing : CBuildManager");
			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHousing);

			// 하우징끼리 연결시킵니다.
			spHousingPrefab->GetAttachedHousing()->AddCombinedHousing(spHousingPrefab->GetBuildDir(), spHousing);
			spHousing->SetParentHousing(spHousingPrefab->GetBuildDir(), spHousingPrefab->GetAttachedHousing());

			// 하우징 덩어리에 지붕을 추가합니다.
			tHousingDesc.m_spHousingLump->AddHousing(tHousingDesc.m_tpBuildIndex, spHousing);
			break;
		}
		#pragma endregion
		default: break;
		}
	}
	#pragma endregion
	#pragma region 일반 건축물
	{
		shared_ptr<CArchitecturePrefab> spArchitecturePrefab = m_umapArchitecturePrefabs[_eArchitextureType];
		FBuildStatus tBuildStatus = {};

		switch (_eArchitextureType)
		{
		#pragma region 작업대
		case EArchitectureType::WORKBENCH:
		{
			FWorkBenchDesc tWorkBenchDesc = {};

			tWorkBenchDesc.m_bActive					= true;
			tWorkBenchDesc.m_wstrGameObjectName			= L"Architecture_WorkBench";
			tWorkBenchDesc.m_eMobilityType				= EMobilityType::STATIC;

			tWorkBenchDesc.m_vInitScale					= spArchitecturePrefab->GetScale().value();
			tWorkBenchDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tWorkBenchDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tWorkBenchDesc.m_fPivotScaleRatio			= 0.01f;
			tWorkBenchDesc.m_iArchitectureUnlockLevel	= 0;
			tWorkBenchDesc.m_eArchitectureType			= EArchitectureType::WORKBENCH;

			tWorkBenchDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tWorkBenchDesc.m_vecNeededMineralCounts		= spArchitecturePrefab->GetNeededMineralCounts();

			tWorkBenchDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tWorkBenchDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tWorkBenchDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tWorkBenchDesc.m_wstrShaderName				= spArchitecturePrefab->GetShaderName();
			tWorkBenchDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tWorkBenchDesc.m_fInteractionRadius			= 1.2f;

			tWorkBenchDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WORKBENCH;
			tWorkBenchDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 10.f;

			shared_ptr<CWorkBench> spWorkBench = CWorkBench::Create(make_shared<FWorkBenchDesc>(tWorkBenchDesc));
			CHECKF(spWorkBench, L"Failed to Create Architecture_WorkBench : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::WORKBENCH].push_back(spWorkBench);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spWorkBench);
			break;
		}
		#pragma endregion
		#pragma region 팰 박스
		case EArchitectureType::PAL_BOX:
		{
			FPalBoxDesc tPalBoxDesc = {};

			tPalBoxDesc.m_bActive					= true;
			tPalBoxDesc.m_wstrGameObjectName		= L"Architecture_PalBox";
			tPalBoxDesc.m_eMobilityType				= EMobilityType::STATIC;

			tPalBoxDesc.m_vInitScale				= spArchitecturePrefab->GetScale().value();
			tPalBoxDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tPalBoxDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tPalBoxDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tPalBoxDesc.m_iArchitectureUnlockLevel	= 1;
			tPalBoxDesc.m_eArchitectureType			= EArchitectureType::PAL_BOX;

			tPalBoxDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tPalBoxDesc.m_vecNeededMineralCounts	= spArchitecturePrefab->GetNeededMineralCounts();

			tPalBoxDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tPalBoxDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tPalBoxDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tPalBoxDesc.m_wstrShaderName			= spArchitecturePrefab->GetShaderName();
			tPalBoxDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tPalBoxDesc.m_fInteractionRadius		= 2.f;

			tPalBoxDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::PAL_BOX;
			tPalBoxDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 10.f;

			shared_ptr<CPalBox> spPalBox = CPalBox::Create(make_shared<FPalBoxDesc>(tPalBoxDesc));
			CHECKF(spPalBox, L"Failed to Create Architecture_PalBox : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::PAL_BOX].push_back(spPalBox);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spPalBox);
			AssignBaseCamp(spPalBox);
			break;
		}
		#pragma endregion
		#pragma region 나무 상자
		case EArchitectureType::WOODEN_CHEST:
		{
			FChestDesc tChestDesc = {};

			tChestDesc.m_bActive					= true;
			tChestDesc.m_wstrGameObjectName			= L"Architecture_WoodenChest";
			tChestDesc.m_eMobilityType				= EMobilityType::STATIC;

			tChestDesc.m_vInitScale					= spArchitecturePrefab->GetScale().value();
			tChestDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tChestDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tChestDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tChestDesc.m_iArchitectureUnlockLevel	= 2;
			tChestDesc.m_eArchitectureType			= EArchitectureType::WOODEN_CHEST;

			tChestDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tChestDesc.m_vecNeededMineralCounts		= spArchitecturePrefab->GetNeededMineralCounts();

			tChestDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tChestDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tChestDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tChestDesc.m_wstrShaderName				= spArchitecturePrefab->GetShaderName();
			tChestDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tChestDesc.m_fInteractionRadius			= 0.75f;

			tChestDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_CHEST;
			tChestDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 10.f;

			shared_ptr<CChest> spChest = CChest::Create(make_shared<FChestDesc>(tChestDesc));
			CHECKF(spChest, L"Failed to Create Architecture_WoodenChest : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::WOODEN_CHEST].push_back(spChest);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spChest);
			break;
		}
#pragma endregion
		#pragma region 금속 상자
		case EArchitectureType::IRON_BOX:
		{
			FIronChestDesc tChestDesc = {};

			tChestDesc.m_bActive					= true;
			tChestDesc.m_wstrGameObjectName			= L"Architecture_IronChest";
			tChestDesc.m_eMobilityType				= EMobilityType::STATIC;

			tChestDesc.m_vInitScale					= spArchitecturePrefab->GetScale().value();
			tChestDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tChestDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tChestDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tChestDesc.m_iArchitectureUnlockLevel	= 7;
			tChestDesc.m_eArchitectureType			= EArchitectureType::IRON_BOX;

			tChestDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tChestDesc.m_vecNeededMineralCounts		= spArchitecturePrefab->GetNeededMineralCounts();

			tChestDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tChestDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tChestDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tChestDesc.m_wstrShaderName				= spArchitecturePrefab->GetShaderName();
			tChestDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tChestDesc.m_fInteractionRadius			= 0.75f;

			tChestDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::IRON_BOX;
			tChestDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 15.f;

			shared_ptr<CIronChest> spIronChest = CIronChest::Create(make_shared<FIronChestDesc>(tChestDesc));
			CHECKF(spIronChest, L"Failed to Create Architecture_IronChest : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::IRON_BOX].push_back(spIronChest);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spIronChest);
			break;
		}
#pragma endregion
		#pragma region 수리대
		case EArchitectureType::REPAIR_BENCH:
		{
			FRepairBenchDesc tRepairBenchDesc = {};

			tRepairBenchDesc.m_bActive					= true;
			tRepairBenchDesc.m_wstrGameObjectName		= L"Architecture_RepairBench";
			tRepairBenchDesc.m_eMobilityType			= EMobilityType::STATIC;

			tRepairBenchDesc.m_vInitScale				= spArchitecturePrefab->GetScale().value();
			tRepairBenchDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tRepairBenchDesc.m_vInitPosition			= spArchitecturePrefab->GetPosition().value();

			tRepairBenchDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tRepairBenchDesc.m_iArchitectureUnlockLevel	= 2;
			tRepairBenchDesc.m_eArchitectureType		= EArchitectureType::REPAIR_BENCH;

			tRepairBenchDesc.m_vecNeededMinerals		= spArchitecturePrefab->GetNeededMinerals();
			tRepairBenchDesc.m_vecNeededMineralCounts	= spArchitecturePrefab->GetNeededMineralCounts();

			tRepairBenchDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tRepairBenchDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tRepairBenchDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tRepairBenchDesc.m_wstrShaderName			= spArchitecturePrefab->GetShaderName();
			tRepairBenchDesc.m_wstrModelFilePath		= spArchitecturePrefab->GetModelFilePath();

			tRepairBenchDesc.m_fInteractionRadius		= 1.2f;

			tRepairBenchDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::REPAIR_BENCH;
			tRepairBenchDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 10.f;

			shared_ptr<CRepairBench> spRepairBench = CRepairBench::Create(make_shared<FRepairBenchDesc>(tRepairBenchDesc));
			CHECKF(spRepairBench, L"Failed to Create Architecture_RepairBench : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::REPAIR_BENCH].push_back(spRepairBench);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spRepairBench);
			break;
		}
		#pragma endregion
		#pragma region 침대
		case EArchitectureType::HUMAN_BED:
		{
			FHumanBedDesc tHumanBedDesc = {};

			tHumanBedDesc.m_bActive						= true;
			tHumanBedDesc.m_wstrGameObjectName			= L"Architecture_HumanBed";
			tHumanBedDesc.m_eMobilityType				= EMobilityType::STATIC;

			tHumanBedDesc.m_vInitScale					= spArchitecturePrefab->GetScale().value();
			tHumanBedDesc.m_vInitRotate					= spArchitecturePrefab->GetRotate().value();
			tHumanBedDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tHumanBedDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tHumanBedDesc.m_iArchitectureUnlockLevel	= 3;
			tHumanBedDesc.m_eArchitectureType			= EArchitectureType::HUMAN_BED;

			tHumanBedDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tHumanBedDesc.m_vecNeededMineralCounts		= spArchitecturePrefab->GetNeededMineralCounts();

			tHumanBedDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tHumanBedDesc.m_vColliderCenter				= spArchitecturePrefab->GetColliderCenter();
			tHumanBedDesc.m_vColliderRotate				= spArchitecturePrefab->GetColliderRotate();

			tHumanBedDesc.m_wstrShaderName				= spArchitecturePrefab->GetShaderName();
			tHumanBedDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tHumanBedDesc.m_fInteractionRadius			= 1.5f;

			tHumanBedDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::HUMAN_BED;
			tHumanBedDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 10.f;

			shared_ptr<CHumanBed> spHumanBed = CHumanBed::Create(make_shared<FHumanBedDesc>(tHumanBedDesc));
			CHECKF(spHumanBed, L"Failed to Create Architecture_HumanBed : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::HUMAN_BED].push_back(spHumanBed);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spHumanBed);
			break;
		}
		#pragma endregion
		#pragma region 팰 침대
		case EArchitectureType::PAL_BED:
		{
			FPalBedDesc tPalBedDesc = {};

			tPalBedDesc.m_bActive					= true;
			tPalBedDesc.m_wstrGameObjectName		= L"Architecture_PalBed";
			tPalBedDesc.m_eMobilityType				= EMobilityType::STATIC;

			tPalBedDesc.m_vInitScale				= spArchitecturePrefab->GetScale().value();
			tPalBedDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tPalBedDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tPalBedDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tPalBedDesc.m_iArchitectureUnlockLevel	= 3;
			tPalBedDesc.m_eArchitectureType			= EArchitectureType::PAL_BED;

			tPalBedDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tPalBedDesc.m_vecNeededMineralCounts	= spArchitecturePrefab->GetNeededMineralCounts();

			tPalBedDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tPalBedDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tPalBedDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tPalBedDesc.m_wstrShaderName			= spArchitecturePrefab->GetShaderName();
			tPalBedDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tPalBedDesc.m_fInteractionRadius		= 1.f;

			tPalBedDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::PAL_BED;
			tPalBedDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 10.f;

			shared_ptr<CPalBed> spPalBed = CPalBed::Create(make_shared<FPalBedDesc>(tPalBedDesc));
			CHECKF(spPalBed, L"Failed to Create Architecture_PalBed : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::PAL_BED].push_back(spPalBed);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spPalBed);
			break;
		}
		#pragma endregion
		#pragma region 먹이 상자
		case EArchitectureType::FEEDING_BOX:
		{
			FFeedingBoxDesc tFeedingBoxDesc = {};

			tFeedingBoxDesc.m_bActive					= true;
			tFeedingBoxDesc.m_wstrGameObjectName		= L"Architecture_FeedingBox";
			tFeedingBoxDesc.m_eMobilityType				= EMobilityType::STATIC;

			tFeedingBoxDesc.m_vInitScale				= spArchitecturePrefab->GetScale().value();
			tFeedingBoxDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tFeedingBoxDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tFeedingBoxDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tFeedingBoxDesc.m_iArchitectureUnlockLevel	= 3;
			tFeedingBoxDesc.m_eArchitectureType			= EArchitectureType::FEEDING_BOX;

			tFeedingBoxDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tFeedingBoxDesc.m_vecNeededMineralCounts	= spArchitecturePrefab->GetNeededMineralCounts();

			tFeedingBoxDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tFeedingBoxDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tFeedingBoxDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tFeedingBoxDesc.m_wstrShaderName			= spArchitecturePrefab->GetShaderName();
			tFeedingBoxDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tFeedingBoxDesc.m_fInteractionRadius		= 0.75f;

			tFeedingBoxDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::FEEDING_BOX;
			tFeedingBoxDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 10.f;

			shared_ptr<CFeedingBox> spFeedingBox = CFeedingBox::Create(make_shared<FFeedingBoxDesc>(tFeedingBoxDesc));
			CHECKF(spFeedingBox, L"Failed to Create Architecture_FeedingBox : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::FEEDING_BOX].push_back(spFeedingBox);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spFeedingBox);
			break;
		}
		#pragma endregion
		#pragma region 가축 목장
		case EArchitectureType::RANCH:
		{
			FRanchDesc tRanchDesc = {};

			tRanchDesc.m_bActive					= true;
			tRanchDesc.m_wstrGameObjectName			= L"Architecture_Ranch";
			tRanchDesc.m_eMobilityType				= EMobilityType::STATIC;

			tRanchDesc.m_vInitScale					= spArchitecturePrefab->GetScale().value();
			tRanchDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tRanchDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tRanchDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tRanchDesc.m_iArchitectureUnlockLevel	= 4;
			tRanchDesc.m_eArchitectureType			= EArchitectureType::RANCH;

			tRanchDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tRanchDesc.m_vecNeededMineralCounts		= spArchitecturePrefab->GetNeededMineralCounts();

			tRanchDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tRanchDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tRanchDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tRanchDesc.m_wstrShaderName				= spArchitecturePrefab->GetShaderName();
			tRanchDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tRanchDesc.m_fInteractionRadius			= 4.5f;

			tRanchDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::RANCH;
			tRanchDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 30.f;

			shared_ptr<CRanch> spRanch = CRanch::Create(make_shared<FRanchDesc>(tRanchDesc));
			CHECKF(spRanch, L"Failed to Create Architecture_Ranch : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::RANCH].push_back(spRanch);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spRanch);
			break;
		}
		#pragma endregion
		#pragma region 열매 농원
		case EArchitectureType::BERRY_FARM:
		{
			FBerryFarmDesc tBerryFarmDesc = {};

			tBerryFarmDesc.m_bActive					= true;
			tBerryFarmDesc.m_wstrGameObjectName			= L"Architecture_BerryFarm";
			tBerryFarmDesc.m_eMobilityType				= EMobilityType::STATIC;

			tBerryFarmDesc.m_vInitScale					= spArchitecturePrefab->GetScale().value();
			tBerryFarmDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tBerryFarmDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tBerryFarmDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tBerryFarmDesc.m_iArchitectureUnlockLevel	= 4;
			tBerryFarmDesc.m_eArchitectureType			= EArchitectureType::BERRY_FARM;

			tBerryFarmDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tBerryFarmDesc.m_vecNeededMineralCounts		= spArchitecturePrefab->GetNeededMineralCounts();

			tBerryFarmDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tBerryFarmDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tBerryFarmDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tBerryFarmDesc.m_wstrShaderName				= spArchitecturePrefab->GetShaderName();
			tBerryFarmDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tBerryFarmDesc.m_fInteractionRadius			= 3.f;

			tBerryFarmDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::BERRY_FARM;
			tBerryFarmDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 20.f;

			shared_ptr<CBerryFarm> spBerryFarm = CBerryFarm::Create(make_shared<FBerryFarmDesc>(tBerryFarmDesc));
			CHECKF(spBerryFarm, L"Failed to Create Architecture_BerryFarm : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::BERRY_FARM].push_back(spBerryFarm);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spBerryFarm);
			break;
		}
		#pragma endregion
		#pragma region 원시 화로
		case EArchitectureType::BLAST_FURNACE:
		{
			FBlastFurnaceDesc tBlastFurnaceDesc = {};

			tBlastFurnaceDesc.m_bActive						= true;
			tBlastFurnaceDesc.m_wstrGameObjectName			= L"Architecture_BlastFurnace";
			tBlastFurnaceDesc.m_eMobilityType				= EMobilityType::STATIC;

			tBlastFurnaceDesc.m_vInitScale					= spArchitecturePrefab->GetScale().value();
			tBlastFurnaceDesc.m_vInitRotate					= spArchitecturePrefab->GetRotate().value();
			tBlastFurnaceDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tBlastFurnaceDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tBlastFurnaceDesc.m_iArchitectureUnlockLevel	= 5;
			tBlastFurnaceDesc.m_eArchitectureType			= EArchitectureType::BLAST_FURNACE;

			tBlastFurnaceDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tBlastFurnaceDesc.m_vecNeededMineralCounts		= spArchitecturePrefab->GetNeededMineralCounts();

			tBlastFurnaceDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tBlastFurnaceDesc.m_vColliderCenter				= spArchitecturePrefab->GetColliderCenter();
			tBlastFurnaceDesc.m_vColliderRotate				= spArchitecturePrefab->GetColliderRotate();

			tBlastFurnaceDesc.m_wstrShaderName				= spArchitecturePrefab->GetShaderName();
			tBlastFurnaceDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tBlastFurnaceDesc.m_fInteractionRadius			= 0.75f;

			tBlastFurnaceDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::BLAST_FURNACE;
			tBlastFurnaceDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 15.f;

			shared_ptr<CBlastFurnace> spBlastFurnace = CBlastFurnace::Create(make_shared<FBlastFurnaceDesc>(tBlastFurnaceDesc));
			CHECKF(spBlastFurnace, L"Failed to Create Architecture_BlastFurnace : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::BLAST_FURNACE].push_back(spBlastFurnace);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spBlastFurnace);
			break;
		}
		#pragma endregion
		#pragma region 스피어 제작대
		case EArchitectureType::SPHERE_TABLE:
		{
			FSphereTableDesc tSphereTableDesc = {};

			tSphereTableDesc.m_bActive						= true;
			tSphereTableDesc.m_wstrGameObjectName			= L"Architecture_SphereTable";
			tSphereTableDesc.m_eMobilityType				= EMobilityType::STATIC;

			tSphereTableDesc.m_vInitScale					= spArchitecturePrefab->GetScale().value();
			tSphereTableDesc.m_vInitRotate					= spArchitecturePrefab->GetRotate().value();
			tSphereTableDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tSphereTableDesc.m_fPivotScaleRatio				= spArchitecturePrefab->GetPivotScaleRatio();
			tSphereTableDesc.m_iArchitectureUnlockLevel		= 5;
			tSphereTableDesc.m_eArchitectureType			= EArchitectureType::SPHERE_TABLE;

			tSphereTableDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tSphereTableDesc.m_vecNeededMineralCounts		= spArchitecturePrefab->GetNeededMineralCounts();

			tSphereTableDesc.m_vColliderExtents				= spArchitecturePrefab->GetColliderExtents();
			tSphereTableDesc.m_vColliderCenter				= spArchitecturePrefab->GetColliderCenter();
			tSphereTableDesc.m_vColliderRotate				= spArchitecturePrefab->GetColliderRotate();

			tSphereTableDesc.m_wstrShaderName				= spArchitecturePrefab->GetShaderName();
			tSphereTableDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tSphereTableDesc.m_fInteractionRadius			= 2.5f;

			tSphereTableDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::SPHERE_TABLE;
			tSphereTableDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 30.f;

			shared_ptr<CSphereTable> spSphereTable = CSphereTable::Create(make_shared<FSphereTableDesc>(tSphereTableDesc));
			CHECKF(spSphereTable, L"Failed to Create Architecture_SphereTable : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::SPHERE_TABLE].push_back(spSphereTable);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spSphereTable);
			break;
		}
		#pragma endregion
		#pragma region 밀 농원
		case EArchitectureType::WHEAT_FARM:
		{
			FWheatFarmDesc tWheatFarmDesc = {};

			tWheatFarmDesc.m_bActive					= true;
			tWheatFarmDesc.m_wstrGameObjectName			= L"Architecture_WheatFarm";
			tWheatFarmDesc.m_eMobilityType				= EMobilityType::STATIC;

			tWheatFarmDesc.m_vInitScale					= spArchitecturePrefab->GetScale().value();
			tWheatFarmDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tWheatFarmDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tWheatFarmDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tWheatFarmDesc.m_iArchitectureUnlockLevel	= 6;
			tWheatFarmDesc.m_eArchitectureType			= EArchitectureType::WHEAT_FARM;

			tWheatFarmDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tWheatFarmDesc.m_vecNeededMineralCounts		= spArchitecturePrefab->GetNeededMineralCounts();

			tWheatFarmDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tWheatFarmDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tWheatFarmDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tWheatFarmDesc.m_wstrShaderName				= spArchitecturePrefab->GetShaderName();
			tWheatFarmDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tWheatFarmDesc.m_fInteractionRadius			= 3.f;

			tWheatFarmDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WHEAT_FARM;
			tWheatFarmDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 50.f;

			shared_ptr<CWheatFarm> spWheatFarm = CWheatFarm::Create(make_shared<FWheatFarmDesc>(tWheatFarmDesc));
			CHECKF(spWheatFarm, L"Failed to Create Architecture_WheatFarm : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::WHEAT_FARM].push_back(spWheatFarm);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spWheatFarm);
			break;
		}
		#pragma endregion
		#pragma region 제분기
		case EArchitectureType::MILL:
		{
			FPulverizerDesc tPulverizerDesc = {};

			tPulverizerDesc.m_bActive					= true;
			tPulverizerDesc.m_wstrGameObjectName		= L"Architecture_Pulverizer";
			tPulverizerDesc.m_eMobilityType				= EMobilityType::STATIC;

			tPulverizerDesc.m_vInitScale				= spArchitecturePrefab->GetScale().value();
			tPulverizerDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tPulverizerDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tPulverizerDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tPulverizerDesc.m_iArchitectureUnlockLevel	= 7;
			tPulverizerDesc.m_eArchitectureType			= EArchitectureType::MILL;

			tPulverizerDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tPulverizerDesc.m_vecNeededMineralCounts	= spArchitecturePrefab->GetNeededMineralCounts();

			tPulverizerDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tPulverizerDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tPulverizerDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tPulverizerDesc.m_wstrShaderName			= spArchitecturePrefab->GetShaderName();
			tPulverizerDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tPulverizerDesc.m_fInteractionRadius		= 3.f;

			tPulverizerDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::MILL;
			tPulverizerDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 50.f;

			shared_ptr<CPulverizer> spPulverizer = CPulverizer::Create(make_shared<FPulverizerDesc>(tPulverizerDesc));
			CHECKF(spPulverizer, L"Failed to Create Architecture_Pulverizer : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::MILL].push_back(spPulverizer);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spPulverizer);
			break;
		}
		#pragma endregion
		#pragma region 조리 냄비
		case EArchitectureType::POT:
		{
			FKitchenMedievalDesc tKitchenMedievalDesc = {};

			tKitchenMedievalDesc.m_bActive						= true;
			tKitchenMedievalDesc.m_wstrGameObjectName			= L"Architecture_KitchenMedieval";
			tKitchenMedievalDesc.m_eMobilityType				= EMobilityType::STATIC;

			tKitchenMedievalDesc.m_vInitScale					= spArchitecturePrefab->GetScale().value();
			tKitchenMedievalDesc.m_vInitRotate					= spArchitecturePrefab->GetRotate().value();
			tKitchenMedievalDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tKitchenMedievalDesc.m_fPivotScaleRatio				= spArchitecturePrefab->GetPivotScaleRatio();
			tKitchenMedievalDesc.m_iArchitectureUnlockLevel		= 7;
			tKitchenMedievalDesc.m_eArchitectureType			= EArchitectureType::POT;

			tKitchenMedievalDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tKitchenMedievalDesc.m_vecNeededMineralCounts		= spArchitecturePrefab->GetNeededMineralCounts();

			tKitchenMedievalDesc.m_vColliderExtents				= spArchitecturePrefab->GetColliderExtents();
			tKitchenMedievalDesc.m_vColliderCenter				= spArchitecturePrefab->GetColliderCenter();
			tKitchenMedievalDesc.m_vColliderRotate				= spArchitecturePrefab->GetColliderRotate();

			tKitchenMedievalDesc.m_wstrShaderName				= spArchitecturePrefab->GetShaderName();
			tKitchenMedievalDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tKitchenMedievalDesc.m_fInteractionRadius			= 1.5f;

			tKitchenMedievalDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::POT;
			tKitchenMedievalDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 20.f;

			shared_ptr<CKitchenMedieval> spKitchenMedieval = CKitchenMedieval::Create(make_shared<FKitchenMedievalDesc>(tKitchenMedievalDesc));
			CHECKF(spKitchenMedieval, L"Failed to Create Architecture_KitchenMedieval : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::BLAST_FURNACE].push_back(spKitchenMedieval);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spKitchenMedieval);
			break;
		}
		#pragma endregion
		#pragma region 무기 제작대
		case EArchitectureType::WEAPON_WORKBENCH:
		{
			FWeaponWorkBenchDesc tWeaponWorkBenchDesc = {};

			tWeaponWorkBenchDesc.m_bActive					= true;
			tWeaponWorkBenchDesc.m_wstrGameObjectName		= L"Architecture_WeaponWorkBench";
			tWeaponWorkBenchDesc.m_eMobilityType			= EMobilityType::STATIC;

			tWeaponWorkBenchDesc.m_vInitScale				= spArchitecturePrefab->GetScale().value();
			tWeaponWorkBenchDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tWeaponWorkBenchDesc.m_vInitPosition			= spArchitecturePrefab->GetPosition().value();

			tWeaponWorkBenchDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tWeaponWorkBenchDesc.m_iArchitectureUnlockLevel	= 7;
			tWeaponWorkBenchDesc.m_eArchitectureType		= EArchitectureType::WEAPON_WORKBENCH;

			tWeaponWorkBenchDesc.m_vecNeededMinerals		= spArchitecturePrefab->GetNeededMinerals();
			tWeaponWorkBenchDesc.m_vecNeededMineralCounts	= spArchitecturePrefab->GetNeededMineralCounts();

			tWeaponWorkBenchDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tWeaponWorkBenchDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tWeaponWorkBenchDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tWeaponWorkBenchDesc.m_wstrShaderName			= spArchitecturePrefab->GetShaderName();
			tWeaponWorkBenchDesc.m_wstrModelFilePath		= spArchitecturePrefab->GetModelFilePath();

			tWeaponWorkBenchDesc.m_fInteractionRadius		= 2.5f;

			tWeaponWorkBenchDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WEAPON_WORKBENCH;
			tWeaponWorkBenchDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 10.f;

			shared_ptr<CWeaponWorkBench> spWeaponWorkBench = CWeaponWorkBench::Create(make_shared<FWeaponWorkBenchDesc>(tWeaponWorkBenchDesc));
			CHECKF(spWeaponWorkBench, L"Failed to Create Architecture_WeaponWorkBench : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::WEAPON_WORKBENCH].push_back(spWeaponWorkBench);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spWeaponWorkBench);
			break;
		}
		#pragma endregion
		#pragma region 채석장
		case EArchitectureType::QUARRY:
		{
			FStonePitDesc tStonePitDesc = {};

			tStonePitDesc.m_bActive					= true;
			tStonePitDesc.m_wstrGameObjectName		= L"Architecture_StonePit";
			tStonePitDesc.m_eMobilityType			= EMobilityType::STATIC;

			tStonePitDesc.m_vInitScale				= spArchitecturePrefab->GetScale().value();
			tStonePitDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tStonePitDesc.m_vInitPosition			= spArchitecturePrefab->GetPosition().value();

			tStonePitDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tStonePitDesc.m_iArchitectureUnlockLevel	= 5;
			tStonePitDesc.m_eArchitectureType		= EArchitectureType::QUARRY;

			tStonePitDesc.m_vecNeededMinerals		= spArchitecturePrefab->GetNeededMinerals();
			tStonePitDesc.m_vecNeededMineralCounts	= spArchitecturePrefab->GetNeededMineralCounts();

			tStonePitDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tStonePitDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tStonePitDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tStonePitDesc.m_wstrShaderName			= spArchitecturePrefab->GetShaderName();
			tStonePitDesc.m_wstrModelFilePath		= spArchitecturePrefab->GetModelFilePath();

			tStonePitDesc.m_fInteractionRadius		= 2.5f;

			tStonePitDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::QUARRY;
			tStonePitDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 20.f;
			tStonePitDesc.m_tBuildStatus.m_bBuildCompleted = false;

			shared_ptr<CStonePit> spStonePit = CStonePit::Create(make_shared<FStonePitDesc>(tStonePitDesc));
			CHECKF(spStonePit, L"Failed to Create Architecture_StonePit : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::QUARRY].push_back(spStonePit);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spStonePit);
			break;
		}
		#pragma endregion
		#pragma region 벌목장
		case EArchitectureType::LUMBERYARD:
		{
			FLumberYardDesc tLumberYardDesc = {};

			tLumberYardDesc.m_bActive					= true;
			tLumberYardDesc.m_wstrGameObjectName		= L"Architecture_LumberYard";
			tLumberYardDesc.m_eMobilityType				= EMobilityType::STATIC;

			tLumberYardDesc.m_vInitScale				= spArchitecturePrefab->GetScale().value();
			tLumberYardDesc.m_vInitRotate				= spArchitecturePrefab->GetRotate().value();
			tLumberYardDesc.m_vInitPosition				= spArchitecturePrefab->GetPosition().value();

			tLumberYardDesc.m_fPivotScaleRatio			= spArchitecturePrefab->GetPivotScaleRatio();
			tLumberYardDesc.m_iArchitectureUnlockLevel	= 5;
			tLumberYardDesc.m_eArchitectureType			= EArchitectureType::LUMBERYARD;

			tLumberYardDesc.m_vecNeededMinerals			= spArchitecturePrefab->GetNeededMinerals();
			tLumberYardDesc.m_vecNeededMineralCounts	= spArchitecturePrefab->GetNeededMineralCounts();

			tLumberYardDesc.m_vColliderExtents			= spArchitecturePrefab->GetColliderExtents();
			tLumberYardDesc.m_vColliderCenter			= spArchitecturePrefab->GetColliderCenter();
			tLumberYardDesc.m_vColliderRotate			= spArchitecturePrefab->GetColliderRotate();

			tLumberYardDesc.m_wstrShaderName			= spArchitecturePrefab->GetShaderName();
			tLumberYardDesc.m_wstrModelFilePath			= spArchitecturePrefab->GetModelFilePath();

			tLumberYardDesc.m_fInteractionRadius		= 2.5f;

			tLumberYardDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::LUMBERYARD;
			tLumberYardDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 20.f;
			tLumberYardDesc.m_tBuildStatus.m_bBuildCompleted = false;

			shared_ptr<CLumberYard> spLumberYard = CLumberYard::Create(make_shared<FLumberYardDesc>(tLumberYardDesc));
			CHECKF(spLumberYard, L"Failed to Create Architecture_LumberYard : CBuildManager");

			m_umapBuiltArchitectures[EArchitectureType::LUMBERYARD].push_back(spLumberYard);

			CClientModule::GetInstance()->GetGameManager()->GetCurLevel()->AddGameObject(spLumberYard);
			break;
		}
		#pragma endregion
		}
	}
	#pragma endregion

	// 인벤토리에서 재료를 차감합니다.
	std::vector<std::shared_ptr<CMineral>> vecMinerals = m_umapArchitecturePrefabs[_eArchitextureType]->GetNeededMinerals();
	std::vector<uint32> vecCounts = m_umapArchitecturePrefabs[_eArchitextureType]->GetNeededMineralCounts();

	for (int32 i = 0; i < vecMinerals.size(); i++)
	{
		m_wpInventorySystem.lock()->ReduceItemNum(m_wpUIManager.lock()->ChangeMineralType(vecMinerals[i]->GetMineralType()), vecCounts[i]);
	}

	return S_OK;
}

EBuildDir Client::CBuildManager::DecideOpponentBuildDir(EBuildDir _eArchitextureType)
{
	// 건축 되어질 방향의(프리팹의) 건축 방향을 넣으면 지어질 하우징의 어느 연결부위에 AttachedHousing이 붙여질지 결정해줍니다.
	// EBuildDir eBuildDir = EBuildDir::ENUM_END;

	switch (_eArchitextureType)
	{
	case EBuildDir::UP:			return EBuildDir::DOWN;
	case EBuildDir::DOWN:		return EBuildDir::UP;
	case EBuildDir::LEFT:		return EBuildDir::RIGHT;
	case EBuildDir::RIGHT:		return EBuildDir::LEFT;
	case EBuildDir::LOOK:		return EBuildDir::BACK;
	case EBuildDir::BACK:		return EBuildDir::LOOK;
	case EBuildDir::LEFT_UP:
	case EBuildDir::RIGHT_UP:
	case EBuildDir::LOOK_UP:
	case EBuildDir::BACK_UP:	return EBuildDir::DOWN;
	case EBuildDir::LEFT_DOWN:
	case EBuildDir::RIGHT_DOWN:
	case EBuildDir::LOOK_DOWN:
	case EBuildDir::BACK_DOWN:	return EBuildDir::UP;
	}

	return EBuildDir::ENUM_END;
}

bool Client::CBuildManager::IsArchitectureConstructed(EArchitectureType _eArchitectureType)
{
	if (!m_umapBuiltArchitectures.contains(_eArchitectureType)) { return false; }

	for (auto& spAritecture : m_umapBuiltArchitectures[_eArchitectureType])
	{
		if (spAritecture->IsBuildComplete()) { return true; }
	}

	return false;
}

Vector3 Client::CBuildManager::GetPalBoxPos()
{
	return m_spPalBox->GetPosition().value();
}
