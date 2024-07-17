#include "pch.h"
#include "SampleLevel.h"

#include "EngineModule.h"
#include "ClientModule.h"
#include "AsyncManager.h"
#include "InputManager.h"
#include "GameManager.h"
#include "UIManager.h"

#include "ShaderHandler.h"
#include "Camera.h"
#include "Player.h"
#include "Landscape.h"
#include "EnvMesh.h"
#include "HousingLump.h"
#include "HousingPrefab.h"
#include "ParticleObject.h"

// 팰 테스트를 위한 임시 코드입니다.
#include "SheepBall.h"
#include "PinkCat.h"
#include "CustomizeObject.h"

// 나무 채집 테스트를 위한 임시 코드입니다. (24-04-16)
// #include "InstanceTree.h"
#include "Tree.h"
#include "Rock.h"
#include "ContainerSystem.h"
#include "InventorySystem.h"
#include "PalBoxSystem.h"
#include "ToDoSystem.h"

// 이미지 호버 테스트를 위한 임시 코드입니다.
#include "Panel.h"
#include "RectComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"
#include "ArchitectureSystem.h"
#include "ProductionSystem.h"

HRESULT Client::CSampleLevel::Initialize()
{
	// light
	{
		FDirectionalLight tDirLight;

		tDirLight.vDirection = Vector3(0.0f, -1.0f, 1.0f);
		tDirLight.vDirection.Normalize();
		tDirLight.vColor = Vector3(1, 1, 1);
		tDirLight.fIntensity = 1;

		CEngineModule::GetInstance()->GetLightManager()->AddLight(L"TestLight", make_shared<FDirectionalLight>(tDirLight), true);
	}

	{
		CUIManager::GetInstance()->LoadPanel(L"HUD");
		CUIManager::GetInstance()->InitHUDInfo();
		CUIManager::GetInstance()->LoadPanel(L"CatchBeforeUI");
		//CUIManager::GetInstance()->SetProductionArchitectureUI(L"WorkArchitectureUI", L"WorkArchitectureUI", 10);
	}

	// player
	{
		FPlayerDesc tPlayerDesc;
		tPlayerDesc.m_vInitPosition = Vector3(-3530.f, 74.f, -2712.f);
		tPlayerDesc.m_eCreatureType = ECreatureType::PLAYER;

		m_spSamplePlayer = CPlayer::Create(make_shared<FPlayerDesc>(tPlayerDesc));
		AddGameObject(m_spSamplePlayer);
	}
		
	//// customizeModel Test
	//{
	//	FCustomizeObjectDesc tCustomObjDesc;
	//	tCustomObjDesc.m_vInitPosition = Vector3(-3530.f, 71.f, -2712.f);
	//
	//	m_spCustomizeObject = CCustomizeObject::Create(make_shared<FCustomizeObjectDesc>(tCustomObjDesc));
	//	AddGameObject(m_spCustomizeObject);
	//}
	

	// game manager
	{
		CClientModule::GetInstance()->GetGameManager()->SetCurLevel(shared_from_this());
		CClientModule::GetInstance()->GetGameManager()->SetPlayer(m_spSamplePlayer);
	}

	shared_ptr<CToDoSystem> spToDoSystem = CGameManager::GetInstance()->GetToDoSystem();

	// sheepball

	for (int32 iIndex = 0; iIndex < 3; ++iIndex)
	{
		// gameobject desc
		FSheepBallDesc tSheepBallDesc;
		tSheepBallDesc.m_wstrGameObjectName = L"SheepBall";
		tSheepBallDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		//tSheepBallDesc.m_vInitPosition = Vector3(-3550.0f + 3.0f * iIndex, 80.0f, -2720.0f);
		tSheepBallDesc.m_vInitPosition = Vector3(-3550.0f + 3.0f , 80.0f, -2720.0f);

		//	// character desc

			// creature desc
		tSheepBallDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc
		tSheepBallDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;
		tSheepBallDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;

		m_spSheep = CSheepBall::Create(make_shared<FSheepBallDesc>(tSheepBallDesc));
		AddGameObject(m_spSheep);

		//spToDoSystem->AddSlavePal(m_spSheep);
		// 파트너 팰
		// m_spSamplePlayer->SetPartnerPal(m_spSheep);
	}


	//// pinkcat
	//{
	//	// 방해해서 주석 쳐놓습니다. -동연-
	//	// gameobject desc
	//	FPinkCatDesc tPinkCatDesc;
	//	tPinkCatDesc.m_wstrGameObjectName = L"PinkCat";
	//	tPinkCatDesc.m_eMobilityType = EMobilityType::DYNAMIC;
	//	tPinkCatDesc.m_vInitPosition = Vector3(-3545.0f, 80.0f, -2720.0f);

	//	// character desc

	//	// creature desc
	//	tPinkCatDesc.m_eCreatureType = ECreatureType::PAL;

	//	// monster desc
	//	tPinkCatDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

	//	// pinkcat desc
	//	m_spPinkCat = CPinkCat::Create(make_shared<FPinkCatDesc>(tPinkCatDesc));
	//	AddGameObject(m_spPinkCat);
	//}


	// landscape
	{
		// gameobject desc
		FLandscapeDesc tLandscapeDesc;
		tLandscapeDesc.m_vInitScale = Vector3(1.0f, 1.0f, 1.0f);
		tLandscapeDesc.m_vInitRotate = Vector3(0.0f, 0.0f, 0.0f);
		tLandscapeDesc.m_vInitPosition = Vector3(0.0f, 0.0f, 0.0f);
		tLandscapeDesc.m_eMobilityType = EMobilityType::STATIC;
		tLandscapeDesc.m_wstrGameObjectName = L"Landscape0";
		tLandscapeDesc.m_bActive = true;

		// landscape desc
		tLandscapeDesc.m_wstrLandscapeFilePath = L"../../Resource/Models/Dat/Map_Landscape/Landscape_Gameplay_0.dat";
		tLandscapeDesc.m_iWidth = 1021;
		tLandscapeDesc.m_iHeight = 1021;
		tLandscapeDesc.m_vLandscapeScale = Vector3(1.0f, 1.0f, 1.0f);
		tLandscapeDesc.m_vLandscapeRotate = Vector3(0.0f, 0.0f, 0.0f);
		tLandscapeDesc.m_vLandscapeTranslate = Vector3(-4080.0f, 0.0f, -3060.0f);

		m_spSampleLandscape = CLandscape::Create(make_shared<FLandscapeDesc>(tLandscapeDesc));
		AddGameObject(m_spSampleLandscape);
	}

	/*
	// camera
	{
		FCameraDesc tCameraObjectDesc;
		m_spCamera = CCamera::Create(make_shared<FCameraDesc>(tCameraObjectDesc));
		m_spCamera->SetPosition({ -3590.0f, 110.0f, -2550.f });
		AddGameObject(m_spCamera);
	}
	*/

	// asyncmanager
	{
		shared_ptr<CAsyncManager> spAsyncManager = CAsyncManager::GetInstance();
		// spAsyncManager->SetAsyncTarget(m_spSamplePlayer);
	}


	{
		std::vector<std::wstring> vecEnvMeshFilePaths;
		/*vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_02.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_03.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_10.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_11.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_12.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_13.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_20.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_21.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_22.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_23.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_32.dat");*/
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_33.dat");
		/*vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_00.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_01.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_02.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_03.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_10.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_11.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_12.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_13.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_20.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_21.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_22.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_23.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_31.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_32.dat");*/
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_33.dat");

		m_vecEnvMeshes.reserve(vecEnvMeshFilePaths.size());
		int32 iSize = (int32)vecEnvMeshFilePaths.size();
		for (int32 iIndex = 0; iIndex < iSize; ++iIndex)
		{
			// gameobject desc
			FEnvMeshDesc tEnvMeshDesc;
			tEnvMeshDesc.m_vInitScale = Vector3(1.0f, 1.0f, 1.0f);
			tEnvMeshDesc.m_vInitRotate = Vector3(0.0f, 0.0f, 0.0f);
			tEnvMeshDesc.m_vInitPosition = Vector3(0.0f, 0.0f, 0.0f);
			tEnvMeshDesc.m_eMobilityType = EMobilityType::STATIC;
			tEnvMeshDesc.m_wstrGameObjectName = L"EnvMesh";
			tEnvMeshDesc.m_bActive = true;

			// env mesh desc
			tEnvMeshDesc.m_wstrEnvMeshFilePath = vecEnvMeshFilePaths[iIndex];

			std::shared_ptr<CEnvMesh> spEnvMesh = CEnvMesh::Create(make_shared<FEnvMeshDesc>(tEnvMeshDesc));
			m_vecEnvMeshes.emplace_back(spEnvMesh);
			AddGameObject(spEnvMesh);
		}
	}


	/*
	{
		// gameobject desc
		FTreeDesc tTreeDesc;
		tTreeDesc.m_vInitScale		= Vector3(1.0f,1.38f,1.0f);
		tTreeDesc.m_vInitRotate		= Vector3(0.0f, 0.0f, 0.0f);
		tTreeDesc.m_vInitPosition	= Vector3(-3530.f, 71.f, -2712.f);
		tTreeDesc.m_wstrGameObjectName = L"Tree";
		tTreeDesc.m_bActive = true;

		// habitat desc
		tTreeDesc.m_eMobilityType = EMobilityType::STATIC;
		tTreeDesc.m_eHabitatType = EHabitatType::NATURE;

		// nature desc
		tTreeDesc.m_eNatureType = ENatureType::TREE;

		// tree desc
		tTreeDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Tree/tree0.dat";
		tTreeDesc.m_vCenter = Vector3(0.0f, 0.0f, 0.0f);
		tTreeDesc.m_vExtents = Vector3(1.0f, 4.0f, 1.0f);
		tTreeDesc.m_fCurHp = 100.0f;

		m_spTree = CTree::Create(make_shared<FTreeDesc>(tTreeDesc));
		AddGameObject(m_spTree);
	}
	*/

	/*
	{
		FInstanceTreeDesc tTreeDesc;
		tTreeDesc.m_vInitScale = Vector3(1.0f, 1.38f, 1.0f);
		tTreeDesc.m_vInitRotate = Vector3(0.0f, 0.0f, 0.0f);
		tTreeDesc.m_vInitPosition = Vector3(0.f, 0.f, 0.f);
		tTreeDesc.m_wstrGameObjectName = L"Tree";
		tTreeDesc.m_bActive = true;

		// habitat desc
		tTreeDesc.m_eMobilityType = EMobilityType::STATIC;
		tTreeDesc.m_eHabitatType = EHabitatType::NATURE;

		// nature desc
		tTreeDesc.m_eNatureType = ENatureType::TREE;

		// tree desc
		tTreeDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Tree/tree0.dat";
		tTreeDesc.m_vCenter = Vector3(0.0f, 0.0f, 0.0f);
		tTreeDesc.m_vExtents = Vector3(1.0f, 4.0f, 1.0f);
		tTreeDesc.m_fCurHp = 100.0f;

		m_spInstanceTree = CInstanceTree::Create(make_shared<FInstanceTreeDesc>(tTreeDesc));
		AddGameObject(m_spInstanceTree);
	}
	*/

	/*
	{
		// gameobject desc
		FRockDesc tRockDesc;
		tRockDesc.m_vInitScale		= Vector3(1.0f, 1.38f, 1.0f);
		tRockDesc.m_vInitRotate		= Vector3(0.0f, 0.0f, 0.0f);
		tRockDesc.m_vInitPosition	= Vector3(-3550.f, 71.f, -2712.f);
		tRockDesc.m_wstrGameObjectName = L"Rock";
		tRockDesc.m_bActive = true;

		// habitat desc
		tRockDesc.m_eMobilityType = EMobilityType::STATIC;
		tRockDesc.m_eHabitatType = EHabitatType::NATURE;

		// nature desc
		tRockDesc.m_eNatureType = ENatureType::ROCK;

		// rock desc
		tRockDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Rock/pal_b00_rock_MineableRocks.dat";
		tRockDesc.m_vCenter = Vector3(0.0f, 0.0f, 0.0f);
		tRockDesc.m_fRadius = 2.0f;
		tRockDesc.m_fCurHP = 0;

		// rock desc
		tRockDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Rock/pal_b00_rock_MineableRocks.dat";
		tRockDesc.m_vCenter = Vector3(0.0f, 0.0f, 0.0f);
		tRockDesc.m_fRadius = 2.0f;
		tRockDesc.m_fCurHP = 0;

		m_spRock = CRock::Create(make_shared<FRockDesc>(tRockDesc));
		AddGameObject(m_spRock);
	}
	*/

	{

}
	// 24-04-22 이동준 추가
	{
		// load panel ui
		CUIManager::GetInstance()->LoadPanel(L"InventoryPanel");

		wstring wstrInventoryParentPanelName = L"InventoryPanel";
		wstring wstrInventoryChildPanelName = L"Inventory_slot";
		wstring wstrMouseChildPanelName = L"Mouse_slot";
		wstring wstrItemInfoChildPanelName = L"Item_info_slot";
		wstring wstrFeedWhoChildPanelName = L"Food_feed_who";

		shared_ptr<CPanel> spInventoryPanel = CUIManager::GetInstance()->FindParentsPanel(wstrInventoryParentPanelName);
		shared_ptr<CPanel> spInventoryChildPanel = CUIManager::GetInstance()->FindChildPanel(wstrInventoryParentPanelName, wstrInventoryChildPanelName);
		shared_ptr<CPanel> spMouseChildPanel = CUIManager::GetInstance()->FindChildPanel(wstrInventoryParentPanelName, wstrMouseChildPanelName);
		shared_ptr<CPanel> spItemInfoChildPanel = CUIManager::GetInstance()->FindChildPanel(wstrInventoryParentPanelName, wstrItemInfoChildPanelName);
		shared_ptr<CPanel> spFeedWhoChildPanel = CUIManager::GetInstance()->FindChildPanel(wstrInventoryParentPanelName, wstrFeedWhoChildPanelName);

		CGameManager::GetInstance()->GetInventorySystem()->SetContainerParentPanel(spInventoryPanel);
		CGameManager::GetInstance()->GetInventorySystem()->SetContainerChildPanel(spInventoryChildPanel);
		CGameManager::GetInstance()->GetInventorySystem()->SetMouseChildPanel(spMouseChildPanel);
		CGameManager::GetInstance()->GetInventorySystem()->SetItemInfoChildPanel(spItemInfoChildPanel);
		CGameManager::GetInstance()->GetInventorySystem()->SetFeedWhoChildPanel(spFeedWhoChildPanel);

		spInventoryPanel->SetActive(false);

		// load palbox panel ui
		CUIManager::GetInstance()->LoadPanel(L"PalboxPanel");

		wstring wstrPalboxParentPanelName = L"PalboxPanel";
		wstring wstrPalboxLeftChildPanelName = L"PalboxLeft";
		wstring wstrPalboxRightChildPanelName = L"PalboxRight";
		wstring wstrPalboxRightNodataChildPanelName = L"PalboxRightNodata";
		wstring wstrPalboxMidChildPanelName = L"PalboxMid";
		wstring wstrPalboxBottomChildPanelName = L"PalboxBottom";
		wstring wstrPalboxMouseChildPanelName = L"PalboxMouse";

		shared_ptr<CPanel> spPalboxPanel = CUIManager::GetInstance()->FindParentsPanel(wstrPalboxParentPanelName);
		shared_ptr<CPanel> spPalboxLeftChildPanel = CUIManager::GetInstance()->FindChildPanel(wstrPalboxParentPanelName, wstrPalboxLeftChildPanelName);
		shared_ptr<CPanel> spPalboxRightChildPanel = CUIManager::GetInstance()->FindChildPanel(wstrPalboxParentPanelName, wstrPalboxRightChildPanelName);
		shared_ptr<CPanel> spPalboxRightNodataChildPanel = CUIManager::GetInstance()->FindChildPanel(wstrPalboxParentPanelName, wstrPalboxRightNodataChildPanelName);
		shared_ptr<CPanel> spPalboxMidChildPanel = CUIManager::GetInstance()->FindChildPanel(wstrPalboxParentPanelName, wstrPalboxMidChildPanelName);
		shared_ptr<CPanel> spPalboxBottomChildPanel = CUIManager::GetInstance()->FindChildPanel(wstrPalboxParentPanelName, wstrPalboxBottomChildPanelName);
		shared_ptr<CPanel> spPalboxMouseChildPanel = CUIManager::GetInstance()->FindChildPanel(wstrPalboxParentPanelName, wstrPalboxMouseChildPanelName);

		CGameManager::GetInstance()->GetPalBoxSystem()->SetContainerParentPanel(spPalboxPanel);
		CGameManager::GetInstance()->GetPalBoxSystem()->SetPalboxLeftChildPanel(spPalboxLeftChildPanel);
		CGameManager::GetInstance()->GetPalBoxSystem()->SetPalboxRightChildPanel(spPalboxRightChildPanel);
		CGameManager::GetInstance()->GetPalBoxSystem()->SetPalboxRightNodataChildPanel(spPalboxRightNodataChildPanel);
		CGameManager::GetInstance()->GetPalBoxSystem()->SetPalboxMidChildPanel(spPalboxMidChildPanel);
		CGameManager::GetInstance()->GetPalBoxSystem()->SetPalboxBottomChildPanel(spPalboxBottomChildPanel);
		CGameManager::GetInstance()->GetPalBoxSystem()->SetMouseChildPanel(spPalboxMouseChildPanel);

		spPalboxPanel->SetActive(false);
	}

	// 건축 시스템 UI를 불러옵니다.
	{
		CUIManager::GetInstance()->LoadPanel(L"TestUI");
		std::shared_ptr<CPanel> spArchitecturePanel = CUIManager::GetInstance()->FindParentsPanel(L"TestUI");
		spArchitecturePanel->SetActive(false);
		CGameManager::GetInstance()->GetArchitectureSystem()->SetArchitecturePanel(spArchitecturePanel);
		CGameManager::GetInstance()->GetArchitectureSystem()->SetHoverImageComponent(spArchitecturePanel->FindComponent<CRectComponent>(L"HoverImage"));
		spArchitecturePanel->FindComponent<CRectComponent>(L"HoverImage")->SetActive(false);

		CGameManager::GetInstance()->GetArchitectureSystem()->AddIndex();
	}

	return CLevel::Initialize();
}

HRESULT Client::CSampleLevel::BeginPlay()
{
	return CLevel::BeginPlay();
}

int32 Client::CSampleLevel::PreTick(float _fDeltaSeconds)
{
	// cerr << 1.0f / _fDeltaSeconds << endl;

	switch (CLevel::PreTick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

int32 Client::CSampleLevel::Tick(float _fDeltaSeconds)
{
	switch (CLevel::Tick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	

	return 0;
}

int32 Client::CSampleLevel::PostTick(float _fDeltaSeconds)
{
	switch (CLevel::PostTick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	// 이미지 호버 테스트를 위한 임시 코드입니다.
	/*
	if (nullptr != CUIManager::GetInstance()->GetHoverImageComponent().lock())
	{
		m_spTestPanel->FindComponent<CRectComponent>(L"HoverImage")->SetCenterPosition(CUIManager::GetInstance()->GetHoverImageComponent().lock()->GetCenterPosition());
		m_spTestPanel->FindComponent<CRectComponent>(L"HoverImage")->SetActive(true);
	}
	else
	{
		m_spTestPanel->FindComponent<CRectComponent>(L"HoverImage")->SetActive(false);
	}
	*/

	return 0;
}

void Client::CSampleLevel::Release()
{
	if (m_spSamplePlayer) { m_spSamplePlayer = nullptr; }
	if (m_spSampleLandscape) { m_spSampleLandscape = nullptr; }
	if (m_spCamera) { m_spCamera = nullptr; }
	if (m_spSheep) { m_spSheep = nullptr; }
	if (m_spCustomizeObject) { m_spCustomizeObject = nullptr; }
	if (m_spSampleNpc) { m_spSampleNpc = nullptr; }

	for (auto& spEnvMesh : m_vecEnvMeshes) { spEnvMesh = nullptr; }
	m_vecEnvMeshes.clear();

	/* 24-04-16 임시 이동준 추가 */
	m_spTree.reset();
	m_spTree = nullptr;
	m_spRock.reset();
	m_spRock = nullptr;

	CLevel::Release();
}

void Client::CSampleLevel::AddSheepBall()
{
	// sheepball
	for (int32 iIndex = 0; iIndex < 1; ++iIndex)
	{
		// gameobject desc
		FSheepBallDesc tSheepBallDesc;
		tSheepBallDesc.m_wstrGameObjectName = L"SheepBall";
		tSheepBallDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		tSheepBallDesc.m_vInitPosition = Vector3(-3540.0f + 1.0f * iIndex, 80.0f, -2720.0f);

		// character desc

		// creature desc
		tSheepBallDesc.m_eCreatureType = ECreatureType::PAL;

		// monster desc

		// sheep ball desc

		shared_ptr<CSheepBall> spSheepBall = CSheepBall::Create(make_shared<FSheepBallDesc>(tSheepBallDesc));
		m_vecSheepBalls.emplace_back(spSheepBall);
		AddGameObject(spSheepBall);
	}
}

void Client::CSampleLevel::RemovESHEEPBALL()
{
	for (auto& spSheepBall : m_vecSheepBalls)
	{
		RemoveGameObject(spSheepBall);
		spSheepBall = nullptr;
	}
	m_vecSheepBalls.clear();
}
