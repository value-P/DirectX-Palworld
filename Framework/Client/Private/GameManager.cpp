// default
#include "pch.h"
#include "GameManager.h"

// engine manager
#include "EngineModule.h"
#include "TimerManager.h"

// engine
#include "World.h"
#include "Level.h"
#include "Timer.h"
#include "Date.h"

// system
#include "ArchitectureSystem.h"
#include "CombatSystem.h"
#include "InventorySystem.h"
#include "MerchantSystem.h"
#include "PalBoxSystem.h"
#include "TechTreeSystem.h"
#include "ToDoSystem.h"
#include "ChestBoxSystem.h"
#include "ProductionSystem.h"
#include "QuestSystem.h"
#include "PalInteractionSystem.h"
#include "CollectingCoinSystem.h"
#include "BowlingSystem.h"

#include "InventoryCustomObject.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CGameManager)

HRESULT Client::CGameManager::Initialize()
{
	// 표준 시간
	{
		m_spTimerManager = CEngineModule::GetInstance()->GetTimerManager();

		FDateDesc tDateDesc;
		tDateDesc.m_fDeltaSecondsPerDay = 600.0f;

		m_spDate = CDate::Create(make_shared<FDateDesc>(tDateDesc));
		m_spTimerManager->AddDate(L"Date120", m_spDate);
	}
	
	// 시스템
	{
		FInventorySystemDesc tInventorySystemDesc;
		m_spInventorySystem = CInventorySystem::Create(make_shared<FInventorySystemDesc>(tInventorySystemDesc));
		
		FChestBoxSystemDesc tChestBoxSystemDesc;
		m_spChestBoxSystem = CChestBoxSystem::Create(make_shared<FChestBoxSystemDesc>(tChestBoxSystemDesc));

		FPalBoxSystemDesc tPalBoxSystemDesc;
		m_spPalBoxSystem = CPalBoxSystem::Create(make_shared<FPalBoxSystemDesc>(tPalBoxSystemDesc));

		FMerchantSystemDesc tMerchantSystemDesc;
		m_spMerchantSystem = CMerchantSystem::Create(make_shared<FMerchantSystemDesc>(tMerchantSystemDesc));

		FTechTreeSystemDesc tFTechTreeSystemDesc;
		m_spTechTreeSystem = CTechTreeSystem::Create(make_shared<FTechTreeSystemDesc>(tFTechTreeSystemDesc));

		FArchitectureSystemDesc tArchitectureSystemDesc;
		m_spArchitectureSystem = CArchitectureSystem::Create(make_shared<FArchitectureSystemDesc>(tArchitectureSystemDesc));

		FToDoSystemDesc tToDoSystemDesc;
		m_spToDoSystem = CToDoSystem::Create(make_shared<FToDoSystemDesc>(tToDoSystemDesc));

		FCombatSystemDesc tCombatSystemDesc;
		tCombatSystemDesc.m_spGameManager = static_pointer_cast<CGameManager>(shared_from_this());
		m_spCombatSystem = CCombatSystem::Create(make_shared<FCombatSystemDesc>(tCombatSystemDesc));

		FProductionSystemDesc tProductionSystemDesc;
		m_spProductionSystem = CProductionSystem::Create(make_shared<FProductionSystemDesc>(tProductionSystemDesc));

		FPalInteractionSystemDesc tPalInteractionSystemDesc;
		m_spPalInteractionSystem = CPalInteractionSystem::Create(make_shared<FPalInteractionSystemDesc>(tPalInteractionSystemDesc));

		//FCollectingCoinSystemDesc tCollectingCoinSystemDese;
		//m_spCollectingCoinSystem = CCollectingCoinSystem::Create(make_shared<FCollectingCoinSystemDesc>(tCollectingCoinSystemDese));

		FBowlingSystemDesc tBowlingSystemDesc;
		m_spBowlingSystem = CBowlingSystem::Create(make_shared<FBowlingSystemDesc>(tBowlingSystemDesc));

		InitQuestSystem();
	}

	return S_OK;
}

HRESULT Client::CGameManager::PostInitialize()
{
	// 월드, 레벨이 초기화되었습니다.
	return S_OK;
}

HRESULT Client::CGameManager::BeginPlay()
{
	m_spInventorySystem->BeginPlay();
	m_spChestBoxSystem->BeginPlay();
	m_spPalBoxSystem->BeginPlay();
	m_spMerchantSystem->BeginPlay();
	m_spTechTreeSystem->BeginPlay();
	m_spArchitectureSystem->BeginPlay();
	m_spToDoSystem->BeginPlay();
	m_spCombatSystem->BeginPlay();
	m_spQuestSystem->BeginPlay();
	m_spProductionSystem->BeginPlay();
	m_spPalInteractionSystem->BeginPlay();

	//m_spCollectingCoinSystem->BeginPlay();
	m_spBowlingSystem->BeginPlay();
	return S_OK;
}

int32 Client::CGameManager::PreTick(float _fDeltaSeconds)
{
	int32 iDay = 0, iHour = 0, iMinute = 0, iSeconds = 0;
	m_spDate->GetTime(iDay, iHour, iMinute, iSeconds);

	m_fDeltaSeconds = _fDeltaSeconds;

	m_spInventorySystem->PreTick(_fDeltaSeconds);
	m_spChestBoxSystem->PreTick(_fDeltaSeconds);
	m_spPalBoxSystem->PreTick(_fDeltaSeconds);
	m_spMerchantSystem->PreTick(_fDeltaSeconds);
	m_spTechTreeSystem->PreTick(_fDeltaSeconds);
	m_spArchitectureSystem->PreTick(_fDeltaSeconds);
	m_spToDoSystem->PreTick(_fDeltaSeconds);
	m_spCombatSystem->PreTick(_fDeltaSeconds);
	m_spProductionSystem->PreTick(_fDeltaSeconds);
	m_spQuestSystem->PreTick(_fDeltaSeconds);
	m_spPalInteractionSystem->PreTick(_fDeltaSeconds);
	//m_spCollectingCoinSystem->PreTick(_fDeltaSeconds);
	m_spBowlingSystem->PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CGameManager::Tick(float _fDeltaSeconds)
{
	m_spInventorySystem->Tick(_fDeltaSeconds);
	m_spChestBoxSystem->Tick(_fDeltaSeconds);
	m_spPalBoxSystem->Tick(_fDeltaSeconds);
	m_spMerchantSystem->Tick(_fDeltaSeconds);
	m_spTechTreeSystem->Tick(_fDeltaSeconds);
	m_spArchitectureSystem->Tick(_fDeltaSeconds);
	m_spToDoSystem->Tick(_fDeltaSeconds);
	m_spCombatSystem->Tick(_fDeltaSeconds);
	m_spQuestSystem->Tick(_fDeltaSeconds);
	m_spProductionSystem->Tick(_fDeltaSeconds);
	//m_spCollectingCoinSystem->Tick(_fDeltaSeconds);
	m_spPalInteractionSystem->Tick(_fDeltaSeconds);
	m_spBowlingSystem->Tick(_fDeltaSeconds);

	return 0;
}

int32 Client::CGameManager::PostTick(float _fDeltaSeconds)
{
	m_spInventorySystem->PostTick(_fDeltaSeconds);
	m_spChestBoxSystem->PostTick(_fDeltaSeconds);
	m_spPalBoxSystem->PostTick(_fDeltaSeconds);
	m_spMerchantSystem->PostTick(_fDeltaSeconds);
	m_spTechTreeSystem->PostTick(_fDeltaSeconds);
	m_spArchitectureSystem->PostTick(_fDeltaSeconds);
	m_spToDoSystem->PostTick(_fDeltaSeconds);
	m_spCombatSystem->PostTick(_fDeltaSeconds);
	m_spProductionSystem->PostTick(_fDeltaSeconds);
	m_spQuestSystem->PostTick(_fDeltaSeconds);
	m_spPalInteractionSystem->PostTick(_fDeltaSeconds);
	//m_spCollectingCoinSystem->PostTick(_fDeltaSeconds);
	m_spBowlingSystem->PostTick(_fDeltaSeconds);

	return 0;
}

void Client::CGameManager::Release()
{
	m_spTimerManager = nullptr;
	m_spPersistentLevel = nullptr;
	m_spCurLevel = nullptr;	
	m_spDate = nullptr;
	m_spPlayer = nullptr;
	
	m_spInventorySystem->Release();
	m_spInventorySystem = nullptr;

	m_spChestBoxSystem->Release();
	m_spChestBoxSystem = nullptr;

	m_spPalBoxSystem->Release();
	m_spPalBoxSystem = nullptr;

	m_spMerchantSystem->Release();
	m_spMerchantSystem = nullptr;

	m_spTechTreeSystem->Release();
	m_spTechTreeSystem = nullptr;

	m_spArchitectureSystem->Release();
	m_spArchitectureSystem = nullptr;

	m_spToDoSystem->Release();
	m_spToDoSystem = nullptr;

	m_spCombatSystem->Release();
	m_spCombatSystem = nullptr;

	m_spProductionSystem->Release();
	m_spProductionSystem = nullptr;

	m_spQuestSystem->Release();
	m_spQuestSystem = nullptr;

	m_spPalInteractionSystem->Release();
	m_spPalInteractionSystem = nullptr;

	//m_spCollectingCoinSystem->Release();
	//m_spCollectingCoinSystem = nullptr;

	m_spBowlingSystem->Release();
	m_spBowlingSystem = nullptr;

	// 싱글턴 매니저를 메모리에서 해제합니다.
	m_spInstance = nullptr;
}

void Client::CGameManager::InitQuestSystem()
{
	FQuestSystemDesc tQuestSystemDesc;
	
	m_spQuestSystem = CQuestSystem::Create(make_shared<FQuestSystemDesc>(tQuestSystemDesc));
}
