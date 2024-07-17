#include "pch.h"
#include "IslandLevel.h"

// engine
#include "World.h"
#include "PersistentLevel.h"

// utility
#include "StringUtility.h"

// engine manager
#include "EngineModule.h"
#include "InputManager.h"
#include "TreeManager.h"
#include "AsyncManager.h"
#include "GameManager.h"
#include "LightManager.h"
#include "ModelHandler.h"
#include "ShaderHandler.h"
#include "RenderManager.h"
#include "UIManager.h"
#include "BuildManager.h"
#include "ClientLoop.h"

// gameobject
#include "Landscape.h"
#include "EnvCollision.h"
#include "EnvMesh.h"
#include "Tree.h"
#include "Rock.h"
#include "Housing.h"
#include "InventoryCustomObject.h"
#include "MarkerObject.h"
#include "Tower.h"
#include "Ranch.h"
#include "ArchitecturePrefab.h"
#include "Door.h"
#include "FeedingBox.h"
#include "KitchenMedieval.h"
#include "Chest.h"
#include "HumanBed.h"
#include "PalBed.h"
#include "BlastFurnace.h"

// monster
#include "Player.h"
#include "SheepBall.h"
#include "PinkCat.h"
#include "ChickenPal.h"
#include "DreamDemon.h"
#include "Penguin.h"
#include "Kitsunebi.h"
#include "HedgeHog.h"
#include "HedgeHogIce.h"
#include "BerryGoat.h"
#include "GrassMammoth.h"
#include "ElecPanda.h"
#include "WoolFox.h"
#include "Boar.h"

// nature
#include "Mineral.h"

// riding
#include "BowlingSystem.h"

// BossRoom
#include "BossRoom.h"

// Npc
#include "NonInteractionNPC.h"
#include "Npc_Quest01.h"
#include "Npc_Quest02.h"
#include "Npc_Quest03.h"
#include "Npc_Quest04.h"
#include "Npc_Quest05.h"
#include "Npc_Quest06.h"


#include "Panel.h"
#include "AnimationImageComponent.h"

HRESULT Client::CIslandLevel::Initialize()
{
	// island level
	{
		m_iLevelType = static_cast<int32>(ELevelType::ISLAND);
		SetLevelName(L"IslandLevel");
	}

	CreateLight();
	CreatePlayer();

	// game manager
	{
		CGameManager::GetInstance()->SetCurLevel(shared_from_this());
		CGameManager::GetInstance()->SetPlayer(m_spPlayer);
	}

	// async manager
	{
		CAsyncManager::GetInstance()->SetSyncTarget(m_spPlayer);
	}

	// CreateLandScape();
	CTreeManager::GetInstance()->Initialize();
	CAsyncManager::GetInstance()->Initialize();

	// CreateNPC();
	CreatePals();
	// CreateEnvironment();
	// CreateBowlingAlley();
	// CreateHut();
	// CreateHutArchitectures();
	// CreateMarker();
	CreateTower();
	InitGetItemUI();

	std::cerr << "섬 레벨이 초기화되었습니다.\n";
	return CLevel::Initialize();
}

HRESULT Client::CIslandLevel::BeginPlay()
{
	if (FAILED(CLevel::BeginPlay())) { return E_FAIL; }

	std::cerr << "섬 레벨이 시작되었습니다.\n";
	return S_OK;
}

int32 Client::CIslandLevel::PreTick(float _fDeltaSeconds)
{
	switch (CLevel::PreTick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

int32 Client::CIslandLevel::Tick(float _fDeltaSeconds)
{
	switch (CLevel::Tick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	if (m_wpGetItemAnimationImageComponentSlot0.lock()->IsFinish())
	{
		m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(false);
	}
	if (m_wpGetItemAnimationImageComponentSlot1.lock()->IsFinish())
	{
		m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(false);
	}


#ifdef _DEBUG
	//cerr << 1.0f / _fDeltaSeconds << endl;
#endif // _DEBUG

	if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_INSERT))
	{
		m_spElecPanda->ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, true);
		m_spPlayer->SetPositionWithController(Vector3(500.f - 1.2f, 500.f + 3.0f, 500.f - 27.0f));
		m_spPlayer->LookAt(Vector3(500.0f, 500.0f, 500.0f));
	}
	if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_HOME))
	{
		// m_spElecPanda->ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, true);
		m_spPlayer->SetPositionWithController(Vector3(-3601.7f, 83.3f, -2690.3f));
	}

	return 0;
}

int32 Client::CIslandLevel::PostTick(float _fDeltaSeconds)
{
	switch (CLevel::PostTick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

void Client::CIslandLevel::Release()
{
	m_spPlayer = nullptr;

	CLevel::Release();
}

void Client::CIslandLevel::CreateLight()
{
	FDirectionalLight tDirLight;
	tDirLight.vDirection = Vector3(0.0f, -1.0f, 1.0f);
	tDirLight.vDirection.Normalize();
	tDirLight.vColor = Vector3(1.0f);
	tDirLight.fIntensity = 1;

	CEngineModule::GetInstance()->GetLightManager()->AddLight(L"TestLight", make_shared<FDirectionalLight>(tDirLight), true);
}

void Client::CIslandLevel::CreateNPC()
{
	CUIManager::GetInstance()->LoadPanel(L"DialogueUI");

	{
		FNpcDesc tNpcDesc;
		tNpcDesc.m_vInitPosition = Vector3(-3583.f,79.1f,-2678.f);
		tNpcDesc.m_vInitialLook = Vector3(-0.71f, 0.f, -0.7f);
		tNpcDesc.m_eModelType = ENpcModelType::NPC_Male_Trader03;
		tNpcDesc.m_wstrNpcName = L"의문의 상인";

		AddGameObject(CNpc_Quest01::Create(make_shared<FNpcDesc>(tNpcDesc)));
	}

	{
		FNpcDesc tNpcDesc;
		tNpcDesc.m_vInitPosition = Vector3(-3459.9f, 40.0f, -2649.3f);
		tNpcDesc.m_vInitialLook = Vector3(-0.176f, 0.0f, -0.98f);
		tNpcDesc.m_eModelType = ENpcModelType::NPC_Female_People03;
		tNpcDesc.m_wstrNpcName = L"까부냥 맘";

		AddGameObject(CNpc_Quest02::Create(make_shared<FNpcDesc>(tNpcDesc)));

		FNonInteractionNPC tDesc;
		tDesc.m_vInitPosition = Vector3(-3458.7f,40.f,-2651.f);
		tDesc.m_vInitialLook = Vector3(-0.95f,0.f,0.3f);
		tDesc.m_eModelType = ENpcModelType::NPC_Female_People02;
		tDesc.m_wstrNpcName = L"주민1";
		tDesc.m_eDefaultAnim = ENPCANIM::AS_NPC_Female_Talking;

		AddGameObject(CNonInteractionNPC::Create(make_shared<FNonInteractionNPC>(tDesc)));

		tDesc;
		tDesc.m_vInitPosition = Vector3(-3461.8f,40.f,-2651.f);
		tDesc.m_vInitialLook = Vector3(0.9f,0.f,0.36f);
		tDesc.m_eModelType = ENpcModelType::NPC_Male_Police00;
		tDesc.m_wstrNpcName = L"주민2";
		tDesc.m_eDefaultAnim = ENPCANIM::AS_NPC_Female_Talking;

		AddGameObject(CNonInteractionNPC::Create(make_shared<FNonInteractionNPC>(tDesc)));
	}

	{
		FNpcDesc tNpcDesc;
		tNpcDesc.m_vInitPosition = Vector3(-3471.f, 9.4f, -2507.7f);
		tNpcDesc.m_vInitialLook = Vector3(0.97f, 0.0f, -0.22f);
		tNpcDesc.m_eModelType = ENpcModelType::NPC_Male_DesertPeople01;
		tNpcDesc.m_wstrNpcName = L"찌릿도치 농장주";

		AddGameObject(CNpc_Quest04::Create(make_shared<FNpcDesc>(tNpcDesc)));
	}

	{
		FNpcDesc tNpcDesc;
		tNpcDesc.m_vInitPosition = Vector3(-3396.f,1.f,-2475.f);
		tNpcDesc.m_vInitialLook = Vector3(0.73f,0.f,-0.08f);
		tNpcDesc.m_eModelType = ENpcModelType::NPC_Male_People02;
		tNpcDesc.m_wstrNpcName = L"낭만 나무꾼";

		AddGameObject(CNpc_Quest05::Create(make_shared<FNpcDesc>(tNpcDesc)));
	}

	{
		FNpcDesc tNpcDesc;
		tNpcDesc.m_vInitPosition = Vector3(-3181.23f, -1.13f, -2083.34f);
		tNpcDesc.m_vInitialLook = Vector3(-0.88f, 0.0f, 0.47f);
		tNpcDesc.m_eModelType = ENpcModelType::NPC_Female_Soldier04;
		tNpcDesc.m_wstrNpcName = L"의문의 용병";

		AddGameObject(CNpc_Quest06::Create(make_shared<FNpcDesc>(tNpcDesc)));
	}

	
	{
		FNpcDesc tNpcDesc;
		tNpcDesc.m_vInitPosition = Vector3(-3407.5f, 9.2f, -2497.1f);
		tNpcDesc.m_eModelType = ENpcModelType::NPC_Male_People02;
		tNpcDesc.m_wstrNpcName = L"볼링장 주인";

		std::shared_ptr<CNpc_Quest03> spNpc = CNpc_Quest03::Create(make_shared<FNpcDesc>(tNpcDesc));
		AddGameObject(spNpc);
		CGameManager::GetInstance()->GetBowlingSystem()->SetNPC(spNpc);
	}
	
}

void Client::CIslandLevel::CreateEnvironment()
{
	// env meshes
	 
	{
		std::vector<std::wstring> vecEnvMeshFilePaths;
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_02.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_03.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_10.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_11.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_12.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_13.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_20.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_21.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_22.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_23.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_32.dat");
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
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_32.dat");
		vecEnvMeshFilePaths.push_back(L"../../Resource/Models/Dat/Map_DefaultObject/Foliage_Gameplay_0_33.dat");*/

		int32 iSize = (int32)vecEnvMeshFilePaths.size();
		for (int32 iIndex = 0; iIndex < iSize; ++iIndex)
		{
			// gameobject desc
			FEnvMeshDesc tEnvMeshDesc;
			tEnvMeshDesc.m_eMobilityType = EMobilityType::STATIC;
			tEnvMeshDesc.m_wstrGameObjectName = L"EnvMesh";

			// env mesh desc
			tEnvMeshDesc.m_wstrEnvMeshFilePath = vecEnvMeshFilePaths[iIndex];
			AddGameObject(CEnvMesh::Create(make_shared<FEnvMeshDesc>(tEnvMeshDesc)));
		}
	}
	
}

void Client::CIslandLevel::CreateLandScape()
{
	// gameobject desc
	FLandscapeDesc tLandscapeDesc;
	tLandscapeDesc.m_wstrGameObjectName = L"Landscape0";

	// landscape desc
	tLandscapeDesc.m_wstrLandscapeFilePath = L"../../Resource/Models/Dat/Map_Landscape/Landscape_Gameplay_0.dat";
	tLandscapeDesc.m_iWidth = 1021;
	tLandscapeDesc.m_iHeight = 1021;
	tLandscapeDesc.m_vLandscapeTranslate = Vector3(-4080.0f, 0.0f, -3060.0f);

	shared_ptr<CLandscape> m_spLandscape = CLandscape::Create(make_shared<FLandscapeDesc>(tLandscapeDesc));
	AddGameObject(m_spLandscape);
}

void Client::CIslandLevel::CreatePlayer()
{
	// 인벤 커스텀 오브젝트
	FInventoryCustomObjectDesc tCustomObjectDesc;
	shared_ptr<CInventoryCustomObject> spInvenCustomObject = CInventoryCustomObject::Create(make_shared<FInventoryCustomObjectDesc>(tCustomObjectDesc));
	spInvenCustomObject->SetScale(Vector3(0.05f));
	spInvenCustomObject->SetActive(false);
	CWorld::GetInstance()->GetPersistentLevel()->AddGameObject(spInvenCustomObject);

	// 플레이어
	FPlayerDesc tPlayerDesc;
	// tPlayerDesc.m_vInitPosition = Vector3(-3530.0f, 74.0f, -2715.0f);
	// tPlayerDesc.m_vInitPosition = Vector3(-3370.f, 30.f, -2517.f);

	// 시작 위치
	tPlayerDesc.m_vInitPosition = Vector3(-3601.7f, 83.3f, -2690.3f);
	tPlayerDesc.m_eCreatureType = ECreatureType::PLAYER;
	tPlayerDesc.m_spInvenCustomObject = spInvenCustomObject;
	tPlayerDesc.m_fCurExp = 0.f;

	m_spPlayer = CPlayer::Create(make_shared<FPlayerDesc>(tPlayerDesc));
	CWorld::GetInstance()->GetPersistentLevel()->AddGameObject(m_spPlayer);
}

void Client::CIslandLevel::CreatePals()
{
	// wild pals
	{
		/*
		// wild boar desc
		{
			// gameobject desc
			FBoarDesc tBoarDesc;
			tBoarDesc.m_wstrGameObjectName = L"Boar";
			tBoarDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tBoarDesc.m_vInitPosition = Vector3(-3541.0f, 80.0f, -2722.0f);

			// character desc

			// creature desc
			tBoarDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tBoarDesc.m_eMonsterType = EMonsterType::BOAR;
			tBoarDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::AGGRESSIVE;
			tBoarDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			shared_ptr<CBoar> spBoar = CBoar::Create(make_shared<FBoarDesc>(tBoarDesc));
			spBoar->SetJump(true);
			AddGameObject(spBoar);
		}

		// wild wool fox desc
		{
			// gameobject desc
			FWoolFoxDesc tWoolFoxDesc;
			tWoolFoxDesc.m_wstrGameObjectName = L"WoolFox";
			tWoolFoxDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tWoolFoxDesc.m_vInitPosition = Vector3(-3542.0f, 80.0f, -2718.0f);

			// character desc

			// creature desc
			tWoolFoxDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tWoolFoxDesc.m_eMonsterType = EMonsterType::WOOL_FOX;
			tWoolFoxDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::PEACEFUL;
			tWoolFoxDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			shared_ptr<CWoolFox> spWoolFox = CWoolFox::Create(make_shared<FWoolFoxDesc>(tWoolFoxDesc));
			spWoolFox->SetJump(true);
			AddGameObject(spWoolFox);
		}
		*/

		// wild elec panda
		{
			// gameobject desc
			FElecPandaDesc tElecPandaDesc;
			tElecPandaDesc.m_wstrGameObjectName = L"ElecPanda";
			tElecPandaDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tElecPandaDesc.m_vInitPosition = Vector3(500.0f, 500.0f, 500.0f);

			// character desc

			// creature desc
			tElecPandaDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tElecPandaDesc.m_eMonsterType = EMonsterType::ELEC_PANDA;
			tElecPandaDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::AGGRESSIVE;
			tElecPandaDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			m_spElecPanda = CElecPanda::Create(make_shared<FElecPandaDesc>(tElecPandaDesc));
			AddGameObject(m_spElecPanda);
		}
		
		/*
		// wild pinkcat
		{
			// gameobject desc
			FPinkCatDesc tPinkCatDesc;
			tPinkCatDesc.m_wstrGameObjectName = L"PinkCat";
			tPinkCatDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tPinkCatDesc.m_vInitPosition = Vector3(-3542.0f, 80.0f, -2722.0f);

			// character desc

			// creature desc
			tPinkCatDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tPinkCatDesc.m_eMonsterType = EMonsterType::PINK_CAT;
			tPinkCatDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPinkCatDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			shared_ptr<CPinkCat> spPinkCat = CPinkCat::Create(make_shared<FPinkCatDesc>(tPinkCatDesc));
			spPinkCat->SetJump(true);
			AddGameObject(spPinkCat);
		}
		

		// wild sheepball
		{
			// gameobject desc
			FSheepBallDesc tSheepBallDesc;
			tSheepBallDesc.m_wstrGameObjectName = L"SheepBall";
			tSheepBallDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tSheepBallDesc.m_vInitPosition = Vector3(-3548.0f, 85.0f, -2722.0f);

			// character desc

			// creature desc
			tSheepBallDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tSheepBallDesc.m_eMonsterType = EMonsterType::SHEEP_BALL;
			tSheepBallDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tSheepBallDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			shared_ptr<CSheepBall> spSheepBall = CSheepBall::Create(make_shared<FSheepBallDesc>(tSheepBallDesc));
			spSheepBall->SetJump(true);
			AddGameObject(spSheepBall);
		}

		// wild dreamdemon
		{
			// gameobject desc
			FDreamDemonDesc tDreamDemonDesc;
			tDreamDemonDesc.m_wstrGameObjectName = L"DreamDemon";
			tDreamDemonDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tDreamDemonDesc.m_vInitPosition = Vector3(-3536.0f, 82.0f, -2718.0f);

			// character desc

			// creature desc
			tDreamDemonDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tDreamDemonDesc.m_eMonsterType = EMonsterType::DREAM_DEMON;
			tDreamDemonDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tDreamDemonDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			shared_ptr<CDreamDemon> spDreamDemon = CDreamDemon::Create(make_shared<FDreamDemonDesc>(tDreamDemonDesc));
			spDreamDemon->SetJump(true);
			AddGameObject(spDreamDemon);
		}
		
		/*
		// wild ice hedge hog
		{
			// gameobject desc
			FHedgeHogIceDesc tIceDesc;
			tIceDesc.m_wstrGameObjectName = L"HedgeHogIce";
			tIceDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tIceDesc.m_vInitPosition = Vector3(-3536.0f, 76.0f, -2716.0f);

			// character desc

			// creature desc
			tIceDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tIceDesc.m_eMonsterType = EMonsterType::HEDGE_HOG_ICE;
			tIceDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::PEACEFUL;
			tIceDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			shared_ptr<CHedgeHogIce> spIce = CHedgeHogIce::Create(make_shared<FHedgeHogIceDesc>(tIceDesc));
			spIce->SetJump(true);
			AddGameObject(spIce);
		}

		// wild penguin
		{
			// gameobject desc
			FPenguinDesc tPenguinDesc;
			tPenguinDesc.m_wstrGameObjectName = L"Penguin";
			tPenguinDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tPenguinDesc.m_vInitPosition = Vector3(-3548.0f, 85.0f, -2724.0f);

			// character desc

			// creature desc
			tPenguinDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tPenguinDesc.m_eMonsterType = EMonsterType::PENGUIN;
			tPenguinDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPenguinDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			shared_ptr<CPenguin> spPenguin = CPenguin::Create(make_shared<FPenguinDesc>(tPenguinDesc));
			spPenguin->SetJump(true);
			AddGameObject(spPenguin);
		}

		// wild kitsunebi
		{
			// gameobject desc
			FKitsunebiDesc tKitsunebiDesc;
			tKitsunebiDesc.m_wstrGameObjectName = L"Kitsunebi";
			tKitsunebiDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tKitsunebiDesc.m_vInitPosition = Vector3(-3536.0f, 85.0f, -2724.0f);

			// creature desc
			tKitsunebiDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tKitsunebiDesc.m_eMonsterType = EMonsterType::KITSUNEBI;
			tKitsunebiDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::AGGRESSIVE;
			tKitsunebiDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			shared_ptr<CKitsunebi> spKitsunebi = CKitsunebi::Create(make_shared<FKitsunebiDesc>(tKitsunebiDesc));
			spKitsunebi->SetJump(true);
			AddGameObject(spKitsunebi);
		}

		// wild berry goat
		{
			// gameobject desc
			FBerryGoatDesc tBerryGoatDesc;
			tBerryGoatDesc.m_wstrGameObjectName = L"BerryGoat";
			tBerryGoatDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tBerryGoatDesc.m_vInitPosition = Vector3(-3538.0f, 85.0f, -2720.0f);

			// character desc

			// creature desc
			tBerryGoatDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tBerryGoatDesc.m_eMonsterType = EMonsterType::BERRY_GOAT;
			tBerryGoatDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tBerryGoatDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			shared_ptr<CBerryGoat> spBerryGoat = CBerryGoat::Create(make_shared<FBerryGoatDesc>(tBerryGoatDesc));
			spBerryGoat->SetJump(true);
			AddGameObject(spBerryGoat);
		}

		// wild hedgehog
		{
			// gameobject desc
			FHedgeHogDesc tHedgeHogDesc;
			tHedgeHogDesc.m_wstrGameObjectName = L"HedgeHog";
			tHedgeHogDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tHedgeHogDesc.m_vInitPosition = Vector3(-3538.0f, 85.0f, -2716.0f);

			// character desc

			// creature desc
			tHedgeHogDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tHedgeHogDesc.m_eMonsterType = EMonsterType::HEDGE_HOG;
			tHedgeHogDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tHedgeHogDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

			shared_ptr<CHedgeHog> spHedgeHog = CHedgeHog::Create(make_shared<FHedgeHogDesc>(tHedgeHogDesc));
			spHedgeHog->SetJump(true);
			AddGameObject(spHedgeHog);
		}
		*/

		
		//// wild grass mammoth
		//{
		//	// gameobject desc
		//	FGrassMammothDesc tGrassMammothDesc;
		//	tGrassMammothDesc.m_wstrGameObjectName = L"GrassMammoth";
		//	tGrassMammothDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		//	tGrassMammothDesc.m_vInitPosition = Vector3(-3540.0f, 85.0f, -2720.0f);

		//	// character desc

		//	// creature desc
		//	tGrassMammothDesc.m_eCreatureType = ECreatureType::PAL;

		//	// monster desc
		//	tGrassMammothDesc.m_eMonsterType = EMonsterType::GRASS_MAMMOTH;
		//	tGrassMammothDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
		//	tGrassMammothDesc.m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		//	shared_ptr<CGrassMammoth> spGrassMammoth = CGrassMammoth::Create(make_shared<FGrassMammothDesc>(tGrassMammothDesc));
		//	spGrassMammoth->SetJump(true);
		//	AddGameObject(spGrassMammoth);
		//}
		
	}
	

	// partner pals
	{
		// partner sheepball
		/*
		{
			// gameobject desc
			FSheepBallDesc tSheepBallDesc;
			tSheepBallDesc.m_wstrGameObjectName = L"SheepBall";
			tSheepBallDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tSheepBallDesc.m_vInitPosition = Vector3(-3548.0f, 85.0f, -2722.0f);

			// character desc

			// creature desc
			tSheepBallDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tSheepBallDesc.m_eMonsterType = EMonsterType::SHEEP_BALL;
			tSheepBallDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tSheepBallDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;

			shared_ptr<CSheepBall> spSheepBall = CSheepBall::Create(make_shared<FSheepBallDesc>(tSheepBallDesc));
			AddGameObject(spSheepBall);

			m_spPlayer->SetPartnerPal(spSheepBall);
		}
		*/

		// partner chickenpal
		/*
		{
			// gameobject desc
			FChickenPalDesc tPalDesc;
			tPalDesc.m_wstrGameObjectName = L"ChickenPal";
			tPalDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tPalDesc.m_vInitPosition = Vector3(-3544.0f, 80.0f, -2724.0f);

			// character desc

			// creature desc
			tPalDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tPalDesc.m_eMonsterType = EMonsterType::CHICKEN_PAL;
			tPalDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPalDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;

			shared_ptr<CChickenPal> spPal = CChickenPal::Create(make_shared<FChickenPalDesc>(tPalDesc));
			AddGameObject(spPal);

			m_spPlayer->SetPartnerPal(spPal);
		}
		*/

		// partner dreamdemon
		/*
		{
			// gameobject desc
			FDreamDemonDesc tDreamDemonDesc;
			tDreamDemonDesc.m_wstrGameObjectName = L"DreamDemon";
			tDreamDemonDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tDreamDemonDesc.m_vInitPosition = Vector3(-3546.0f, 85.0f, -2718.0f);

			// character desc

			// creature desc
			tDreamDemonDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tDreamDemonDesc.m_eMonsterType = EMonsterType::DREAM_DEMON;
			tDreamDemonDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tDreamDemonDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;

			shared_ptr<CDreamDemon> spDreamDemon = CDreamDemon::Create(make_shared<FDreamDemonDesc>(tDreamDemonDesc));
			AddGameObject(spDreamDemon);

			m_spPlayer->SetPartnerPal(spDreamDemon);
		}
		*/

		// partner penguin
		/*
		{
			// gameobject desc
			FPenguinDesc tPenguinDesc;
			tPenguinDesc.m_wstrGameObjectName = L"Penguin";
			tPenguinDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tPenguinDesc.m_vInitPosition = Vector3(-3548.0f, 85.0f, -2724.0f);

			// character desc

			// creature desc
			tPenguinDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tPenguinDesc.m_eMonsterType = EMonsterType::PENGUIN;
			tPenguinDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPenguinDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;

			shared_ptr<CPenguin> spPenguin = CPenguin::Create(make_shared<FPenguinDesc>(tPenguinDesc));
			AddGameObject(spPenguin);

			m_spPlayer->SetPartnerPal(spPenguin);
		}
		*/

		// partner kitsunebi
		/*
		{
			// gameobject desc
			FKitsunebiDesc tKitsunebiDesc;
			tKitsunebiDesc.m_wstrGameObjectName = L"Kitsunebi";
			tKitsunebiDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tKitsunebiDesc.m_vInitPosition = Vector3(-3536.0f, 85.0f, -2724.0f);

			// character desc

			// creature desc
			tKitsunebiDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tKitsunebiDesc.m_eMonsterType = EMonsterType::KITSUNEBI;
			tKitsunebiDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::AGGRESSIVE;
			tKitsunebiDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;

			shared_ptr<CKitsunebi> spKitsunebi = CKitsunebi::Create(make_shared<FKitsunebiDesc>(tKitsunebiDesc));
			AddGameObject(spKitsunebi);

			m_spPlayer->SetPartnerPal(spKitsunebi);
		}
		*/

		// partner berry goat
		/*
		{
			// gameobject desc
			FBerryGoatDesc tBerryGoatDesc;
			tBerryGoatDesc.m_wstrGameObjectName = L"BerryGoat";
			tBerryGoatDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tBerryGoatDesc.m_vInitPosition = Vector3(-3538.0f, 85.0f, -2720.0f);

			// character desc

			// creature desc
			tBerryGoatDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tBerryGoatDesc.m_eMonsterType = EMonsterType::BERRY_GOAT;
			tBerryGoatDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tBerryGoatDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;

			shared_ptr<CBerryGoat> spBerryGoat = CBerryGoat::Create(make_shared<FBerryGoatDesc>(tBerryGoatDesc));
			AddGameObject(spBerryGoat);

			m_spPlayer->SetPartnerPal(spBerryGoat);
		}
		*/

		// partner pinkcat
		/*
		{
			// gameobject desc
			FPinkCatDesc tPinkCatDesc;
			tPinkCatDesc.m_wstrGameObjectName = L"PinkCat";
			tPinkCatDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tPinkCatDesc.m_vInitPosition = Vector3(-3542.0f, 80.0f, -2722.0f);

			// character desc

			// creature desc
			tPinkCatDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tPinkCatDesc.m_eMonsterType = EMonsterType::PINK_CAT;
			tPinkCatDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPinkCatDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;

			shared_ptr<CPinkCat> spPinkCat = CPinkCat::Create(make_shared<FPinkCatDesc>(tPinkCatDesc));
			AddGameObject(spPinkCat);

			m_spPlayer->SetPartnerPal(spPinkCat);
		}
		*/

		// partner hedgehog
		/*
		{
			// gameobject desc
			FHedgeHogDesc tPalDesc;
			tPalDesc.m_wstrGameObjectName = L"HedgeHog";
			tPalDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tPalDesc.m_vInitPosition = Vector3(-3550.0f, 80.0f, -2720.0f);

			// character desc

			// creature desc
			tPalDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tPalDesc.m_eMonsterType = EMonsterType::HEDGE_HOG;
			tPalDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPalDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;

			shared_ptr<CHedgeHog> spPal = CHedgeHog::Create(make_shared<FHedgeHogDesc>(tPalDesc));
			AddGameObject(spPal);

			m_spPlayer->SetPartnerPal(spPal);
		}
		*/
	}

	// slave pals
	{
		// slave sheepball
		/*
		{
			// gameobject desc
			FSheepBallDesc tSheepBallDesc;
			tSheepBallDesc.m_wstrGameObjectName = L"SheepBall";
			tSheepBallDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tSheepBallDesc.m_vInitPosition = Vector3(-3536.0f, 85.0f, -2720.0f);

			// creature desc
			tSheepBallDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tSheepBallDesc.m_eMonsterType = EMonsterType::SHEEP_BALL;
			tSheepBallDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tSheepBallDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;

			shared_ptr<CSheepBall> spSheepBall = CSheepBall::Create(make_shared<FSheepBallDesc>(tSheepBallDesc));
			AddGameObject(spSheepBall);
		}

		// slave pinkcat
		{
			// gameobject desc
			FPinkCatDesc tPinkCatDesc;
			tPinkCatDesc.m_wstrGameObjectName = L"PinkCat";
			tPinkCatDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tPinkCatDesc.m_vInitPosition = Vector3(-3538.0f, 85.0f, -2720.0f);

			// character desc

			// creature desc
			tPinkCatDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tPinkCatDesc.m_eMonsterType = EMonsterType::PINK_CAT;
			tPinkCatDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPinkCatDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;

			shared_ptr<CPinkCat> spPinkCat = CPinkCat::Create(make_shared<FPinkCatDesc>(tPinkCatDesc));
			AddGameObject(spPinkCat);
		}
		*/

		// slave dreamdemon
		/*
		{
			// gameobject desc
			FDreamDemonDesc tDreamDemonDesc;
			tDreamDemonDesc.m_wstrGameObjectName = L"DreamDemon";
			tDreamDemonDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tDreamDemonDesc.m_vInitPosition = Vector3(-3540.0f, 85.0f, -2718.0f);

			// character desc

			// creature desc
			tDreamDemonDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tDreamDemonDesc.m_eMonsterType = EMonsterType::DREAM_DEMON;
			tDreamDemonDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tDreamDemonDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;

			shared_ptr<CDreamDemon> spDreamDemon = CDreamDemon::Create(make_shared<FDreamDemonDesc>(tDreamDemonDesc));
			AddGameObject(spDreamDemon);
		}
		*/

		// slave penguin
		/*
		{
			// gameobject desc
			FPenguinDesc tPenguinDesc;
			tPenguinDesc.m_wstrGameObjectName = L"Penguin";
			tPenguinDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tPenguinDesc.m_vInitPosition = Vector3(-3542.0f, 85.0f, -2724.0f);

			// character desc

			// creature desc
			tPenguinDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tPenguinDesc.m_eMonsterType = EMonsterType::PENGUIN;
			tPenguinDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPenguinDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;

			shared_ptr<CPenguin> spPenguin = CPenguin::Create(make_shared<FPenguinDesc>(tPenguinDesc));
			AddGameObject(spPenguin);
		}
		*/

		// slave kitsunebi
		/*
		{
			// gameobject desc
			FKitsunebiDesc tKitsunebiDesc;
			tKitsunebiDesc.m_wstrGameObjectName = L"Kitsunebi";
			tKitsunebiDesc.m_eMobilityType = EMobilityType::DYNAMIC;
			tKitsunebiDesc.m_vInitPosition = Vector3(-3544.0f, 85.0f, -2724.0f);

			// character desc

			// creature desc
			tKitsunebiDesc.m_eCreatureType = ECreatureType::PAL;

			// monster desc
			tKitsunebiDesc.m_eMonsterType = EMonsterType::KITSUNEBI;
			tKitsunebiDesc.m_eMonsterPersonalityType = EMonsterPersonalityType::AGGRESSIVE;
			tKitsunebiDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;

			shared_ptr<CKitsunebi> spKitsunebi = CKitsunebi::Create(make_shared<FKitsunebiDesc>(tKitsunebiDesc));
			AddGameObject(spKitsunebi);
		}
		*/
	}
}

void Client::CIslandLevel::CreateBowlingAlley()
{
	wstring wstrFileName = L"Bowling Alley.json";

	ifstream isJsonpath(L"../../Resource/" + wstrFileName);
	if (!isJsonpath.is_open())
	{
		throw runtime_error("failed to open json file : CEffectGroup");
	}

	string strJsonfile((istreambuf_iterator<char>(isJsonpath)), (istreambuf_iterator<char>()));

	rapidjson::Document doc;
	doc.Parse(strJsonfile.c_str());

	shared_ptr<CHousing> spHousing = nullptr;
	FHousingDesc tHousingDesc = {};

	if (doc["Housings"].IsArray())
	{
		auto iter = doc["Housings"].Begin();

		for (; iter != doc["Housings"].End(); ++iter)
		{
			if (9 == (*iter)["Architecture Type"].GetUint())
			{
				// 문
				shared_ptr<CDoor> spDoor = nullptr;
				FDoorDesc tDoorDesc = {};

				tDoorDesc.m_wstrGameObjectName = L"Hut_Housing";
				tDoorDesc.m_eMobilityType = EMobilityType::STATIC;

				tDoorDesc.m_eArchitectureType = (EArchitectureType)((*iter)["Architecture Type"].GetUint());
				tDoorDesc.m_eBuildType = (EBuildType)((*iter)["Build Type"].GetUint());
				tDoorDesc.m_eBuildDir = (EBuildDir)((*iter)["Build Dir"].GetUint());

				tuple<int32, int32, int32> tpIndex;
				auto& [iFloor, iX, iZ] = tpIndex;

				iFloor = (*iter)["Build Index"][0].GetInt();
				iX = (*iter)["Build Index"][1].GetInt();
				iZ = (*iter)["Build Index"][2].GetInt();

				tDoorDesc.m_tpBuildIndex = tpIndex;

				tDoorDesc.m_vInitScale.x = (*iter)["Scale"][0].GetFloat();
				tDoorDesc.m_vInitScale.y = (*iter)["Scale"][1].GetFloat();
				tDoorDesc.m_vInitScale.z = (*iter)["Scale"][2].GetFloat();
				tDoorDesc.m_fPivotScaleRatio = 0.01f;

				tDoorDesc.m_vInitRotate.x = (*iter)["Rotation"][0].GetFloat();
				tDoorDesc.m_vInitRotate.y = (*iter)["Rotation"][1].GetFloat();
				tDoorDesc.m_vInitRotate.z = (*iter)["Rotation"][2].GetFloat();

				tDoorDesc.m_vInitPosition.x = (*iter)["Position"][0].GetFloat();
				tDoorDesc.m_vInitPosition.y = (*iter)["Position"][1].GetFloat();
				tDoorDesc.m_vInitPosition.z = (*iter)["Position"][2].GetFloat();

				tDoorDesc.m_vColliderExtents.x = (*iter)["Collider Extents"][0].GetFloat();
				tDoorDesc.m_vColliderExtents.y = (*iter)["Collider Extents"][1].GetFloat();
				tDoorDesc.m_vColliderExtents.z = (*iter)["Collider Extents"][2].GetFloat();

				tDoorDesc.m_vColliderCenter.x = (*iter)["Collider Center"][0].GetFloat();
				tDoorDesc.m_vColliderCenter.y = (*iter)["Collider Center"][1].GetFloat();
				tDoorDesc.m_vColliderCenter.z = (*iter)["Collider Center"][2].GetFloat();

				tDoorDesc.m_vColliderRotate.x = (*iter)["Collider Rotate"][0].GetFloat();
				tDoorDesc.m_vColliderRotate.y = (*iter)["Collider Rotate"][1].GetFloat();
				tDoorDesc.m_vColliderRotate.z = (*iter)["Collider Rotate"][2].GetFloat();

				tDoorDesc.m_wstrShaderName = L"FX_NonAnimMesh_PNT2T";
				tDoorDesc.m_wstrModelFilePath = CStringUtility::ToWideString((*iter)["Model File Path"].GetString());

				tDoorDesc.m_tBuildStatus.m_bBuildCompleted = true;

				spDoor = CDoor::Create(make_shared<FDoorDesc>(tDoorDesc));;
				CHECKF(spDoor, L"Failed to Create CDoor : CIslandLevel");
				AddGameObject(spDoor);
			}
			else
			{
				tHousingDesc.m_wstrGameObjectName = L"Bowling Alley Housing";
				tHousingDesc.m_eMobilityType = EMobilityType::STATIC;

				tHousingDesc.m_eArchitectureType = (EArchitectureType)((*iter)["Architecture Type"].GetUint());
				tHousingDesc.m_eBuildType = (EBuildType)((*iter)["Build Type"].GetUint());
				tHousingDesc.m_eBuildDir = (EBuildDir)((*iter)["Build Dir"].GetUint());

				tuple<int32, int32, int32> tpIndex;
				auto& [iFloor, iX, iZ] = tpIndex;

				iFloor = (*iter)["Build Index"][0].GetInt();
				iX = (*iter)["Build Index"][1].GetInt();
				iZ = (*iter)["Build Index"][2].GetInt();

				tHousingDesc.m_tpBuildIndex = tpIndex;

				tHousingDesc.m_vInitScale.x = (*iter)["Scale"][0].GetFloat();
				tHousingDesc.m_vInitScale.y = (*iter)["Scale"][1].GetFloat();
				tHousingDesc.m_vInitScale.z = (*iter)["Scale"][2].GetFloat();
				tHousingDesc.m_fPivotScaleRatio = 0.01f;

				tHousingDesc.m_vInitRotate.x = (*iter)["Rotation"][0].GetFloat();
				tHousingDesc.m_vInitRotate.y = (*iter)["Rotation"][1].GetFloat();
				tHousingDesc.m_vInitRotate.z = (*iter)["Rotation"][2].GetFloat();

				tHousingDesc.m_vInitPosition.x = (*iter)["Position"][0].GetFloat();
				tHousingDesc.m_vInitPosition.y = (*iter)["Position"][1].GetFloat();
				tHousingDesc.m_vInitPosition.z = (*iter)["Position"][2].GetFloat();

				tHousingDesc.m_vColliderExtents.x = (*iter)["Collider Extents"][0].GetFloat();
				tHousingDesc.m_vColliderExtents.y = (*iter)["Collider Extents"][1].GetFloat();
				tHousingDesc.m_vColliderExtents.z = (*iter)["Collider Extents"][2].GetFloat();

				tHousingDesc.m_vColliderCenter.x = (*iter)["Collider Center"][0].GetFloat();
				tHousingDesc.m_vColliderCenter.y = (*iter)["Collider Center"][1].GetFloat();
				tHousingDesc.m_vColliderCenter.z = (*iter)["Collider Center"][2].GetFloat();

				tHousingDesc.m_vColliderRotate.x = (*iter)["Collider Rotate"][0].GetFloat();
				tHousingDesc.m_vColliderRotate.y = (*iter)["Collider Rotate"][1].GetFloat();
				tHousingDesc.m_vColliderRotate.z = (*iter)["Collider Rotate"][2].GetFloat();

				tHousingDesc.m_wstrShaderName = L"FX_NonAnimMesh_PNT2T";
				tHousingDesc.m_wstrModelFilePath = CStringUtility::ToWideString((*iter)["Model File Path"].GetString());

				tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

				spHousing = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));;
				CHECKF(spHousing, L"Failed to Create CHousing : CIslandLevel");
				AddGameObject(spHousing);
			}
		}
	}
}

void Client::CIslandLevel::CreateHut()
{
	// 찌릿도치 농장
	{
		wstring wstrFileName = L"Hut.json";

		ifstream isJsonpath(L"../../Resource/" + wstrFileName);
		if (!isJsonpath.is_open())
		{
			throw runtime_error("failed to open json file : CEffectGroup");
		}

		string strJsonfile((istreambuf_iterator<char>(isJsonpath)), (istreambuf_iterator<char>()));

		rapidjson::Document doc;
		doc.Parse(strJsonfile.c_str());

		if (doc["Housings"].IsArray())
		{
			auto iter = doc["Housings"].Begin();

			for (; iter != doc["Housings"].End(); ++iter)
			{
				if (9 == (*iter)["Architecture Type"].GetUint())
				{
					// 문
					shared_ptr<CDoor> spDoor = nullptr;
					FDoorDesc tDoorDesc = {};

					tDoorDesc.m_wstrGameObjectName = L"Hut_Housing";
					tDoorDesc.m_eMobilityType = EMobilityType::STATIC;

					tDoorDesc.m_eArchitectureType = (EArchitectureType)((*iter)["Architecture Type"].GetUint());
					tDoorDesc.m_eBuildType = (EBuildType)((*iter)["Build Type"].GetUint());
					tDoorDesc.m_eBuildDir = (EBuildDir)((*iter)["Build Dir"].GetUint());

					tuple<int32, int32, int32> tpIndex;
					auto& [iFloor, iX, iZ] = tpIndex;

					iFloor = (*iter)["Build Index"][0].GetInt();
					iX = (*iter)["Build Index"][1].GetInt();
					iZ = (*iter)["Build Index"][2].GetInt();

					tDoorDesc.m_tpBuildIndex = tpIndex;

					tDoorDesc.m_vInitScale.x = (*iter)["Scale"][0].GetFloat();
					tDoorDesc.m_vInitScale.y = (*iter)["Scale"][1].GetFloat();
					tDoorDesc.m_vInitScale.z = (*iter)["Scale"][2].GetFloat();
					tDoorDesc.m_fPivotScaleRatio = 0.01f;

					tDoorDesc.m_vInitRotate.x = (*iter)["Rotation"][0].GetFloat();
					tDoorDesc.m_vInitRotate.y = (*iter)["Rotation"][1].GetFloat();
					tDoorDesc.m_vInitRotate.z = (*iter)["Rotation"][2].GetFloat();

					tDoorDesc.m_vInitPosition.x = (*iter)["Position"][0].GetFloat();
					tDoorDesc.m_vInitPosition.y = (*iter)["Position"][1].GetFloat();
					tDoorDesc.m_vInitPosition.z = (*iter)["Position"][2].GetFloat();

					tDoorDesc.m_vColliderExtents.x = (*iter)["Collider Extents"][0].GetFloat();
					tDoorDesc.m_vColliderExtents.y = (*iter)["Collider Extents"][1].GetFloat();
					tDoorDesc.m_vColliderExtents.z = (*iter)["Collider Extents"][2].GetFloat();

					tDoorDesc.m_vColliderCenter.x = (*iter)["Collider Center"][0].GetFloat();
					tDoorDesc.m_vColliderCenter.y = (*iter)["Collider Center"][1].GetFloat();
					tDoorDesc.m_vColliderCenter.z = (*iter)["Collider Center"][2].GetFloat();

					tDoorDesc.m_vColliderRotate.x = (*iter)["Collider Rotate"][0].GetFloat();
					tDoorDesc.m_vColliderRotate.y = (*iter)["Collider Rotate"][1].GetFloat();
					tDoorDesc.m_vColliderRotate.z = (*iter)["Collider Rotate"][2].GetFloat();

					tDoorDesc.m_wstrShaderName = L"FX_NonAnimMesh_PNT2T";
					tDoorDesc.m_wstrModelFilePath = CStringUtility::ToWideString((*iter)["Model File Path"].GetString());

					tDoorDesc.m_tBuildStatus.m_bBuildCompleted = true;

					spDoor = CDoor::Create(make_shared<FDoorDesc>(tDoorDesc));;
					CHECKF(spDoor, L"Failed to Create CDoor : CIslandLevel");
					AddGameObject(spDoor);
				}
				else
				{
					shared_ptr<CHousing> spHousing = nullptr;
					FHousingDesc tHousingDesc = {};

					tHousingDesc.m_wstrGameObjectName = L"Hut_Housing";
					tHousingDesc.m_eMobilityType = EMobilityType::STATIC;

					tHousingDesc.m_eArchitectureType = (EArchitectureType)((*iter)["Architecture Type"].GetUint());
					tHousingDesc.m_eBuildType = (EBuildType)((*iter)["Build Type"].GetUint());
					tHousingDesc.m_eBuildDir = (EBuildDir)((*iter)["Build Dir"].GetUint());

					tuple<int32, int32, int32> tpIndex;
					auto& [iFloor, iX, iZ] = tpIndex;

					iFloor = (*iter)["Build Index"][0].GetInt();
					iX = (*iter)["Build Index"][1].GetInt();
					iZ = (*iter)["Build Index"][2].GetInt();

					tHousingDesc.m_tpBuildIndex = tpIndex;

					tHousingDesc.m_vInitScale.x = (*iter)["Scale"][0].GetFloat();
					tHousingDesc.m_vInitScale.y = (*iter)["Scale"][1].GetFloat();
					tHousingDesc.m_vInitScale.z = (*iter)["Scale"][2].GetFloat();
					tHousingDesc.m_fPivotScaleRatio = 0.01f;

					tHousingDesc.m_vInitRotate.x = (*iter)["Rotation"][0].GetFloat();
					tHousingDesc.m_vInitRotate.y = (*iter)["Rotation"][1].GetFloat();
					tHousingDesc.m_vInitRotate.z = (*iter)["Rotation"][2].GetFloat();

					tHousingDesc.m_vInitPosition.x = (*iter)["Position"][0].GetFloat();
					tHousingDesc.m_vInitPosition.y = (*iter)["Position"][1].GetFloat();
					tHousingDesc.m_vInitPosition.z = (*iter)["Position"][2].GetFloat();

					tHousingDesc.m_vColliderExtents.x = (*iter)["Collider Extents"][0].GetFloat();
					tHousingDesc.m_vColliderExtents.y = (*iter)["Collider Extents"][1].GetFloat();
					tHousingDesc.m_vColliderExtents.z = (*iter)["Collider Extents"][2].GetFloat();

					tHousingDesc.m_vColliderCenter.x = (*iter)["Collider Center"][0].GetFloat();
					tHousingDesc.m_vColliderCenter.y = (*iter)["Collider Center"][1].GetFloat();
					tHousingDesc.m_vColliderCenter.z = (*iter)["Collider Center"][2].GetFloat();

					tHousingDesc.m_vColliderRotate.x = (*iter)["Collider Rotate"][0].GetFloat();
					tHousingDesc.m_vColliderRotate.y = (*iter)["Collider Rotate"][1].GetFloat();
					tHousingDesc.m_vColliderRotate.z = (*iter)["Collider Rotate"][2].GetFloat();

					tHousingDesc.m_wstrShaderName = L"FX_NonAnimMesh_PNT2T";
					tHousingDesc.m_wstrModelFilePath = CStringUtility::ToWideString((*iter)["Model File Path"].GetString());

					tHousingDesc.m_tBuildStatus.m_bBuildCompleted = true;

					spHousing = CHousing::Create(make_shared<FHousingDesc>(tHousingDesc));;
					CHECKF(spHousing, L"Failed to Create CHousing : CIslandLevel");
					AddGameObject(spHousing);
				}
			}
		}
	}
	
	// 찌릿도치 목장
	{
		std::shared_ptr<CArchitecturePrefab> spRanchPrefab = CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::RANCH);

		FRanchDesc tRanchDesc;
		tRanchDesc.m_bActive = true;
		tRanchDesc.m_wstrGameObjectName = L"Dochi_Ranch";
		tRanchDesc.m_eMobilityType = EMobilityType::STATIC;

		tRanchDesc.m_vInitScale = spRanchPrefab->GetScale().value();
		tRanchDesc.m_vInitRotate = Vector3(-13.0f, 180.0f, 0.0f);
		tRanchDesc.m_vInitPosition = Vector3(-3480.f, 7.3f, -2497.f);

		tRanchDesc.m_fPivotScaleRatio = spRanchPrefab->GetPivotScaleRatio();
		tRanchDesc.m_iArchitectureUnlockLevel = 4;
		tRanchDesc.m_eArchitectureType = EArchitectureType::RANCH;

		tRanchDesc.m_vecNeededMinerals = spRanchPrefab->GetNeededMinerals();
		tRanchDesc.m_vecNeededMineralCounts = spRanchPrefab->GetNeededMineralCounts();

		tRanchDesc.m_vColliderExtents = spRanchPrefab->GetColliderExtents();
		tRanchDesc.m_vColliderCenter = spRanchPrefab->GetColliderCenter();
		tRanchDesc.m_vColliderRotate = spRanchPrefab->GetColliderRotate();

		tRanchDesc.m_wstrShaderName = spRanchPrefab->GetShaderName();
		tRanchDesc.m_wstrModelFilePath = spRanchPrefab->GetModelFilePath();

		tRanchDesc.m_fInteractionRadius = 4.5f;

		tRanchDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::RANCH;
		tRanchDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 30.f;

		tRanchDesc.m_tBuildStatus.m_bBuildCompleted = true;
		tRanchDesc.m_tBuildStatus.m_iCurPercentage = 100.f;

		m_spRanch = CRanch::Create(make_shared<FRanchDesc>(tRanchDesc));
		CHECKF(m_spRanch, L"Failed to Create Architecture_Ranch : CBuildManager");

		AddGameObject(m_spRanch);
	}
}

void Client::CIslandLevel::CreateHutArchitectures()
{
	// - 3474, 11.3, -2510 먹이상자
	FFeedingBoxDesc tFeedingBoxDesc = {};
	shared_ptr<CArchitecturePrefab> spArchitecturePrefab = CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::FEEDING_BOX);

	tFeedingBoxDesc.m_bActive = true;
	tFeedingBoxDesc.m_wstrGameObjectName = L"Architecture_FeedingBox";
	tFeedingBoxDesc.m_eMobilityType = EMobilityType::STATIC;

	tFeedingBoxDesc.m_vInitScale = spArchitecturePrefab->GetScale().value();
	tFeedingBoxDesc.m_vInitRotate = spArchitecturePrefab->GetRotate().value();
	tFeedingBoxDesc.m_vInitPosition = Vector3(-3474.f, 11.3f, -2510.f);

	tFeedingBoxDesc.m_fPivotScaleRatio = spArchitecturePrefab->GetPivotScaleRatio();
	tFeedingBoxDesc.m_iArchitectureUnlockLevel = 3;
	tFeedingBoxDesc.m_eArchitectureType = EArchitectureType::FEEDING_BOX;

	tFeedingBoxDesc.m_vecNeededMinerals = spArchitecturePrefab->GetNeededMinerals();
	tFeedingBoxDesc.m_vecNeededMineralCounts = spArchitecturePrefab->GetNeededMineralCounts();

	tFeedingBoxDesc.m_vColliderExtents = spArchitecturePrefab->GetColliderExtents();
	tFeedingBoxDesc.m_vColliderCenter = spArchitecturePrefab->GetColliderCenter();
	tFeedingBoxDesc.m_vColliderRotate = spArchitecturePrefab->GetColliderRotate();

	tFeedingBoxDesc.m_wstrShaderName = spArchitecturePrefab->GetShaderName();
	tFeedingBoxDesc.m_wstrModelFilePath = spArchitecturePrefab->GetModelFilePath();

	tFeedingBoxDesc.m_fInteractionRadius = 0.75f;

	tFeedingBoxDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::FEEDING_BOX;
	tFeedingBoxDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 0.f;

	tFeedingBoxDesc.m_tBuildStatus.m_bBuildCompleted = true;
	tFeedingBoxDesc.m_tBuildStatus.m_iCurPercentage = 100.f;

	shared_ptr<CFeedingBox> spFeedingBox = CFeedingBox::Create(make_shared<FFeedingBoxDesc>(tFeedingBoxDesc));
	CHECKF(spFeedingBox, L"Failed to Create Architecture_FeedingBox : CBuildManager");
	AddGameObject(spFeedingBox);
	spFeedingBox->SetComplete();


	// - 3473, 8.8, -2506 조리 냄비
	FKitchenMedievalDesc tKitchenMedievalDesc = {};
	spArchitecturePrefab = CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::POT);

	tKitchenMedievalDesc.m_bActive = true;
	tKitchenMedievalDesc.m_wstrGameObjectName = L"Architecture_KitchenMedieval";
	tKitchenMedievalDesc.m_eMobilityType = EMobilityType::STATIC;

	tKitchenMedievalDesc.m_vInitScale = spArchitecturePrefab->GetScale().value();
	tKitchenMedievalDesc.m_vInitRotate = spArchitecturePrefab->GetRotate().value();
	tKitchenMedievalDesc.m_vInitPosition = Vector3(-3473.f, 8.8f, -2506.f);

	tKitchenMedievalDesc.m_fPivotScaleRatio = spArchitecturePrefab->GetPivotScaleRatio();
	tKitchenMedievalDesc.m_iArchitectureUnlockLevel = 7;
	tKitchenMedievalDesc.m_eArchitectureType = EArchitectureType::POT;

	tKitchenMedievalDesc.m_vecNeededMinerals = spArchitecturePrefab->GetNeededMinerals();
	tKitchenMedievalDesc.m_vecNeededMineralCounts = spArchitecturePrefab->GetNeededMineralCounts();

	tKitchenMedievalDesc.m_vColliderExtents = spArchitecturePrefab->GetColliderExtents();
	tKitchenMedievalDesc.m_vColliderCenter = spArchitecturePrefab->GetColliderCenter();
	tKitchenMedievalDesc.m_vColliderRotate = spArchitecturePrefab->GetColliderRotate();

	tKitchenMedievalDesc.m_wstrShaderName = spArchitecturePrefab->GetShaderName();
	tKitchenMedievalDesc.m_wstrModelFilePath = spArchitecturePrefab->GetModelFilePath();

	tKitchenMedievalDesc.m_fInteractionRadius = 1.5f;

	tKitchenMedievalDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::POT;
	tKitchenMedievalDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 0.f;

	tKitchenMedievalDesc.m_tBuildStatus.m_bBuildCompleted = true;
	tKitchenMedievalDesc.m_tBuildStatus.m_iCurPercentage = 100.f;

	shared_ptr<CKitchenMedieval> spKitchenMedieval = CKitchenMedieval::Create(make_shared<FKitchenMedievalDesc>(tKitchenMedievalDesc));
	CHECKF(spKitchenMedieval, L"Failed to Create Architecture_KitchenMedieval : CBuildManager");
	AddGameObject(spKitchenMedieval);
	spKitchenMedieval->SetComplete();


	// - 3482, 11.3, -2510 침대
	FHumanBedDesc tHumanBedDesc = {};
	spArchitecturePrefab = CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::HUMAN_BED);

	tHumanBedDesc.m_bActive = true;
	tHumanBedDesc.m_wstrGameObjectName = L"Architecture_HumanBed";
	tHumanBedDesc.m_eMobilityType = EMobilityType::STATIC;

	tHumanBedDesc.m_vInitScale = spArchitecturePrefab->GetScale().value();
	tHumanBedDesc.m_vInitRotate = spArchitecturePrefab->GetRotate().value();
	tHumanBedDesc.m_vInitPosition = Vector3(-3482.f, 11.3f, -2510.f);

	tHumanBedDesc.m_fPivotScaleRatio = spArchitecturePrefab->GetPivotScaleRatio();
	tHumanBedDesc.m_iArchitectureUnlockLevel = 3;
	tHumanBedDesc.m_eArchitectureType = EArchitectureType::HUMAN_BED;

	tHumanBedDesc.m_vecNeededMinerals = spArchitecturePrefab->GetNeededMinerals();
	tHumanBedDesc.m_vecNeededMineralCounts = spArchitecturePrefab->GetNeededMineralCounts();

	tHumanBedDesc.m_vColliderExtents = spArchitecturePrefab->GetColliderExtents();
	tHumanBedDesc.m_vColliderCenter = spArchitecturePrefab->GetColliderCenter();
	tHumanBedDesc.m_vColliderRotate = spArchitecturePrefab->GetColliderRotate();

	tHumanBedDesc.m_wstrShaderName = spArchitecturePrefab->GetShaderName();
	tHumanBedDesc.m_wstrModelFilePath = spArchitecturePrefab->GetModelFilePath();

	tHumanBedDesc.m_fInteractionRadius = 1.5f;

	tHumanBedDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::HUMAN_BED;
	tHumanBedDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 0.f;

	tHumanBedDesc.m_tBuildStatus.m_bBuildCompleted = true;
	tHumanBedDesc.m_tBuildStatus.m_iCurPercentage = 100.f;

	shared_ptr<CHumanBed> spHumanBed = CHumanBed::Create(make_shared<FHumanBedDesc>(tHumanBedDesc));
	CHECKF(spHumanBed, L"Failed to Create Architecture_HumanBed : CBuildManager");
	AddGameObject(spHumanBed);
	spHumanBed->SetComplete();


	// - 3482, 11.3, -2513 나무상자
	FChestDesc tChestDesc = {};
	spArchitecturePrefab = CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::WOODEN_CHEST);

	tChestDesc.m_bActive = true;
	tChestDesc.m_wstrGameObjectName = L"Architecture_WoodenChest";
	tChestDesc.m_eMobilityType = EMobilityType::STATIC;

	tChestDesc.m_vInitScale = spArchitecturePrefab->GetScale().value();
	tChestDesc.m_vInitRotate = spArchitecturePrefab->GetRotate().value();
	tChestDesc.m_vInitPosition = Vector3(-3482.f, 11.3f, -2513.f);

	tChestDesc.m_fPivotScaleRatio = spArchitecturePrefab->GetPivotScaleRatio();
	tChestDesc.m_iArchitectureUnlockLevel = 2;
	tChestDesc.m_eArchitectureType = EArchitectureType::WOODEN_CHEST;

	tChestDesc.m_vecNeededMinerals = spArchitecturePrefab->GetNeededMinerals();
	tChestDesc.m_vecNeededMineralCounts = spArchitecturePrefab->GetNeededMineralCounts();

	tChestDesc.m_vColliderExtents = spArchitecturePrefab->GetColliderExtents();
	tChestDesc.m_vColliderCenter = spArchitecturePrefab->GetColliderCenter();
	tChestDesc.m_vColliderRotate = spArchitecturePrefab->GetColliderRotate();

	tChestDesc.m_wstrShaderName = spArchitecturePrefab->GetShaderName();
	tChestDesc.m_wstrModelFilePath = spArchitecturePrefab->GetModelFilePath();

	tChestDesc.m_fInteractionRadius = 0.75f;

	tChestDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_CHEST;
	tChestDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 0.f;

	tChestDesc.m_tBuildStatus.m_bBuildCompleted = true;
	tChestDesc.m_tBuildStatus.m_iCurPercentage = 100.f;

	shared_ptr<CChest> spChest = CChest::Create(make_shared<FChestDesc>(tChestDesc));
	CHECKF(spChest, L"Failed to Create Architecture_WoodenChest : CBuildManager");
	AddGameObject(spChest);
	spChest->SetComplete();


	// - 3482.5, 11.3, -2515 나무상자
	tChestDesc = {};
	spArchitecturePrefab = CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::WOODEN_CHEST);

	tChestDesc.m_bActive = true;
	tChestDesc.m_wstrGameObjectName = L"Architecture_WoodenChest";
	tChestDesc.m_eMobilityType = EMobilityType::STATIC;

	tChestDesc.m_vInitScale = spArchitecturePrefab->GetScale().value();
	tChestDesc.m_vInitRotate = spArchitecturePrefab->GetRotate().value();
	tChestDesc.m_vInitPosition = Vector3(-3482.5f, 11.3f, -2515.f);

	tChestDesc.m_fPivotScaleRatio = spArchitecturePrefab->GetPivotScaleRatio();
	tChestDesc.m_iArchitectureUnlockLevel = 2;
	tChestDesc.m_eArchitectureType = EArchitectureType::WOODEN_CHEST;

	tChestDesc.m_vecNeededMinerals = spArchitecturePrefab->GetNeededMinerals();
	tChestDesc.m_vecNeededMineralCounts = spArchitecturePrefab->GetNeededMineralCounts();

	tChestDesc.m_vColliderExtents = spArchitecturePrefab->GetColliderExtents();
	tChestDesc.m_vColliderCenter = spArchitecturePrefab->GetColliderCenter();
	tChestDesc.m_vColliderRotate = spArchitecturePrefab->GetColliderRotate();

	tChestDesc.m_wstrShaderName = spArchitecturePrefab->GetShaderName();
	tChestDesc.m_wstrModelFilePath = spArchitecturePrefab->GetModelFilePath();

	tChestDesc.m_fInteractionRadius = 0.75f;

	tChestDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::WOODEN_CHEST;
	tChestDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 0.f;

	tChestDesc.m_tBuildStatus.m_bBuildCompleted = true;
	tChestDesc.m_tBuildStatus.m_iCurPercentage = 100.f;

	spChest = CChest::Create(make_shared<FChestDesc>(tChestDesc));
	CHECKF(spChest, L"Failed to Create Architecture_WoodenChest : CBuildManager");
	AddGameObject(spChest);
	spChest->SetComplete();


	// - 3480, 8.5, -2504 팰 침대
	FPalBedDesc tPalBedDesc = {};
	spArchitecturePrefab = CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::PAL_BED);

	tPalBedDesc.m_bActive = true;
	tPalBedDesc.m_wstrGameObjectName = L"Architecture_PalBed";
	tPalBedDesc.m_eMobilityType = EMobilityType::STATIC;

	tPalBedDesc.m_vInitScale = spArchitecturePrefab->GetScale().value();
	tPalBedDesc.m_vInitRotate = Vector3(-13.0f, 180.0f, 0.0f);
	tPalBedDesc.m_vInitPosition = Vector3(-3480.f, 8.5f, -2504.f);

	tPalBedDesc.m_fPivotScaleRatio = spArchitecturePrefab->GetPivotScaleRatio();
	tPalBedDesc.m_iArchitectureUnlockLevel = 3;
	tPalBedDesc.m_eArchitectureType = EArchitectureType::PAL_BED;

	tPalBedDesc.m_vecNeededMinerals = spArchitecturePrefab->GetNeededMinerals();
	tPalBedDesc.m_vecNeededMineralCounts = spArchitecturePrefab->GetNeededMineralCounts();

	tPalBedDesc.m_vColliderExtents = spArchitecturePrefab->GetColliderExtents();
	tPalBedDesc.m_vColliderCenter = spArchitecturePrefab->GetColliderCenter();
	tPalBedDesc.m_vColliderRotate = spArchitecturePrefab->GetColliderRotate();

	tPalBedDesc.m_wstrShaderName = spArchitecturePrefab->GetShaderName();
	tPalBedDesc.m_wstrModelFilePath = spArchitecturePrefab->GetModelFilePath();

	tPalBedDesc.m_fInteractionRadius = 1.f;

	tPalBedDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::PAL_BED;
	tPalBedDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 0.f;

	tPalBedDesc.m_tBuildStatus.m_bBuildCompleted = true;
	tPalBedDesc.m_tBuildStatus.m_iCurPercentage = 100.f;

	shared_ptr<CPalBed> spPalBed = CPalBed::Create(make_shared<FPalBedDesc>(tPalBedDesc));
	CHECKF(spPalBed, L"Failed to Create Architecture_PalBed : CBuildManager");
	AddGameObject(spPalBed);
	spPalBed->SetComplete();


	// - 3476, 8.54, -2504 팰 침대
	tPalBedDesc = {};
	spArchitecturePrefab = CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::PAL_BED);

	tPalBedDesc.m_bActive = true;
	tPalBedDesc.m_wstrGameObjectName = L"Architecture_PalBed";
	tPalBedDesc.m_eMobilityType = EMobilityType::STATIC;

	tPalBedDesc.m_vInitScale = spArchitecturePrefab->GetScale().value();
	tPalBedDesc.m_vInitRotate = Vector3(-13.0f, 180.0f, 0.0f);
	tPalBedDesc.m_vInitPosition = Vector3(-3476.f, 8.54f, -2504.f);

	tPalBedDesc.m_fPivotScaleRatio = spArchitecturePrefab->GetPivotScaleRatio();
	tPalBedDesc.m_iArchitectureUnlockLevel = 3;
	tPalBedDesc.m_eArchitectureType = EArchitectureType::PAL_BED;

	tPalBedDesc.m_vecNeededMinerals = spArchitecturePrefab->GetNeededMinerals();
	tPalBedDesc.m_vecNeededMineralCounts = spArchitecturePrefab->GetNeededMineralCounts();

	tPalBedDesc.m_vColliderExtents = spArchitecturePrefab->GetColliderExtents();
	tPalBedDesc.m_vColliderCenter = spArchitecturePrefab->GetColliderCenter();
	tPalBedDesc.m_vColliderRotate = spArchitecturePrefab->GetColliderRotate();

	tPalBedDesc.m_wstrShaderName = spArchitecturePrefab->GetShaderName();
	tPalBedDesc.m_wstrModelFilePath = spArchitecturePrefab->GetModelFilePath();

	tPalBedDesc.m_fInteractionRadius = 1.f;

	tPalBedDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::PAL_BED;
	tPalBedDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 0.f;

	tPalBedDesc.m_tBuildStatus.m_bBuildCompleted = true;
	tPalBedDesc.m_tBuildStatus.m_iCurPercentage = 100.f;

	spPalBed = CPalBed::Create(make_shared<FPalBedDesc>(tPalBedDesc));
	CHECKF(spPalBed, L"Failed to Create Architecture_PalBed : CBuildManager");
	AddGameObject(spPalBed);
	spPalBed->SetComplete();


	// - 3472.8, 8.46, -2504 팰 침대
	tPalBedDesc = {};
	spArchitecturePrefab = CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::PAL_BED);

	tPalBedDesc.m_bActive = true;
	tPalBedDesc.m_wstrGameObjectName = L"Architecture_PalBed";
	tPalBedDesc.m_eMobilityType = EMobilityType::STATIC;

	tPalBedDesc.m_vInitScale = spArchitecturePrefab->GetScale().value();
	tPalBedDesc.m_vInitRotate = Vector3(-13.0f, 180.0f, 0.0f);
	tPalBedDesc.m_vInitPosition = Vector3(-3472.8f, 8.46f, -2504.f);

	tPalBedDesc.m_fPivotScaleRatio = spArchitecturePrefab->GetPivotScaleRatio();
	tPalBedDesc.m_iArchitectureUnlockLevel = 3;
	tPalBedDesc.m_eArchitectureType = EArchitectureType::PAL_BED;

	tPalBedDesc.m_vecNeededMinerals = spArchitecturePrefab->GetNeededMinerals();
	tPalBedDesc.m_vecNeededMineralCounts = spArchitecturePrefab->GetNeededMineralCounts();

	tPalBedDesc.m_vColliderExtents = spArchitecturePrefab->GetColliderExtents();
	tPalBedDesc.m_vColliderCenter = spArchitecturePrefab->GetColliderCenter();
	tPalBedDesc.m_vColliderRotate = spArchitecturePrefab->GetColliderRotate();

	tPalBedDesc.m_wstrShaderName = spArchitecturePrefab->GetShaderName();
	tPalBedDesc.m_wstrModelFilePath = spArchitecturePrefab->GetModelFilePath();

	tPalBedDesc.m_fInteractionRadius = 1.f;

	tPalBedDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::PAL_BED;
	tPalBedDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 0.f;

	tPalBedDesc.m_tBuildStatus.m_bBuildCompleted = true;
	tPalBedDesc.m_tBuildStatus.m_iCurPercentage = 100.f;

	spPalBed = CPalBed::Create(make_shared<FPalBedDesc>(tPalBedDesc));
	CHECKF(spPalBed, L"Failed to Create Architecture_PalBed : CBuildManager");
	AddGameObject(spPalBed);
	spPalBed->SetComplete();


	// 원시 화로
	FBlastFurnaceDesc tBlastFurnaceDesc = {};
	spArchitecturePrefab = CBuildManager::GetInstance()->FindHousingPrefab(EArchitectureType::BLAST_FURNACE);

	tBlastFurnaceDesc.m_bActive = true;
	tBlastFurnaceDesc.m_wstrGameObjectName = L"Architecture_BlastFurnace";
	tBlastFurnaceDesc.m_eMobilityType = EMobilityType::STATIC;

	tBlastFurnaceDesc.m_vInitScale = spArchitecturePrefab->GetScale().value();
	tBlastFurnaceDesc.m_vInitRotate = Vector3(0.f);
	tBlastFurnaceDesc.m_vInitPosition = Vector3(-3480.f, 11.3f, -2514.f);

	tBlastFurnaceDesc.m_fPivotScaleRatio = spArchitecturePrefab->GetPivotScaleRatio();
	tBlastFurnaceDesc.m_iArchitectureUnlockLevel = 5;
	tBlastFurnaceDesc.m_eArchitectureType = EArchitectureType::BLAST_FURNACE;

	tBlastFurnaceDesc.m_vecNeededMinerals = spArchitecturePrefab->GetNeededMinerals();
	tBlastFurnaceDesc.m_vecNeededMineralCounts = spArchitecturePrefab->GetNeededMineralCounts();

	tBlastFurnaceDesc.m_vColliderExtents = spArchitecturePrefab->GetColliderExtents();
	tBlastFurnaceDesc.m_vColliderCenter = spArchitecturePrefab->GetColliderCenter();
	tBlastFurnaceDesc.m_vColliderRotate = spArchitecturePrefab->GetColliderRotate();

	tBlastFurnaceDesc.m_wstrShaderName = spArchitecturePrefab->GetShaderName();
	tBlastFurnaceDesc.m_wstrModelFilePath = spArchitecturePrefab->GetModelFilePath();

	tBlastFurnaceDesc.m_fInteractionRadius = 0.75f;

	tBlastFurnaceDesc.m_tBuildStatus.m_eArchitectureType = EArchitectureType::BLAST_FURNACE;
	tBlastFurnaceDesc.m_tBuildStatus.m_fMaxDeltaSeconds = 0.f;

	tBlastFurnaceDesc.m_tBuildStatus.m_bBuildCompleted = true;
	tBlastFurnaceDesc.m_tBuildStatus.m_iCurPercentage = 100.f;

	shared_ptr<CBlastFurnace> spBlastFurnace = CBlastFurnace::Create(make_shared<FBlastFurnaceDesc>(tBlastFurnaceDesc));
	CHECKF(spBlastFurnace, L"Failed to Create Architecture_BlastFurnace : CBuildManager");
	AddGameObject(spBlastFurnace);
	spBlastFurnace->SetComplete();
}

void Client::CIslandLevel::CreateMarker()
{
	FMarkerObjectDesc tMarkerObjectDesc;
	tMarkerObjectDesc.m_bActive = true;
	tMarkerObjectDesc.m_vInitPosition = Vector3(-3597.9f, 83.4f, -2686.5f);
	tMarkerObjectDesc.m_eMapNameType = EMapNameType::WIND_HILL;
	tMarkerObjectDesc.m_wstrGameObjectName = L"MarkerHill";

	shared_ptr<CMarkerObject> spHillObject = CMarkerObject::Create(make_shared<FMarkerObjectDesc>(tMarkerObjectDesc));
	AddGameObject(spHillObject);

	tMarkerObjectDesc.m_bActive = true;
	tMarkerObjectDesc.m_vInitPosition = Vector3(3224.5f, -8.5f, -2176.9f);
	tMarkerObjectDesc.m_eMapNameType = EMapNameType::TOWER;
	tMarkerObjectDesc.m_wstrGameObjectName = L"MarkerTower";

	shared_ptr<CMarkerObject> spTowerObject = CMarkerObject::Create(make_shared<FMarkerObjectDesc>(tMarkerObjectDesc));
	AddGameObject(spTowerObject);
}

void Client::CIslandLevel::CreateTower()
{
	// 탑 외부
	FTowerDesc tMapTowerDesc;
	tMapTowerDesc.m_vInitPosition = Vector3(-3185.5f, -1.13f, -2077.f);
	AddGameObject(CTower::Create(make_shared<FTowerDesc>(tMapTowerDesc)));

	// 탑 내부
	FBossRoomDesc tBossRoomDesc;
	tBossRoomDesc.m_wstrGameObjectName = L"Tower";
	tBossRoomDesc.m_wstrBossRoomFilePath = L"../../Resource/Models/Dat/Map_DefaultObject/BossRoom.dat";
	// tBossRoomDesc.m_vInitPosition = Vector3(500.f);
	
	shared_ptr<CBossRoom> spBossRoom = CBossRoom::Create(make_shared<FBossRoomDesc>(tBossRoomDesc));
	spBossRoom->SetPosition(Vector3(500.f));
	AddGameObject(spBossRoom);
}

void Client::CIslandLevel::InitGetItemUI()
{
	m_wpGetItemParentPanel = CUIManager::GetInstance()->FindParentsPanel(L"GetItemPanel");
	m_wpGetItemParentPanel.lock()->SetActive(true);
	m_wpGetItemAnimationImageComponentSlot0 = m_wpGetItemParentPanel.lock()->FindComponent<CAnimationImageComponent>(L"get_item_slot_0");
	m_wpGetItemAnimationImageComponentSlot1 = m_wpGetItemParentPanel.lock()->FindComponent<CAnimationImageComponent>(L"get_item_slot_1");
	m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(false);
	m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(false);
}
