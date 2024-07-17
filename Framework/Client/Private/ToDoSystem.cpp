#include "pch.h"
#include "ToDoSystem.h"

// engine manager
#include "EngineModule.h"
#include "InputManager.h"

// client manager
#include "ClientLoop.h"
#include "BuildManager.h"

// gameobject
#include "Architecture.h"
#include "Item.h"
#include "Monster.h"
#include "PalBed.h"
#include "Chest.h"
#include "IronChest.h"
#include "FeedingBox.h"
#include "PalBox.h"

std::shared_ptr<CToDoSystem> Client::CToDoSystem::Create(const std::shared_ptr<FToDoSystemDesc>& _spToDoSystemDesc)
{
	shared_ptr<CToDoSystem> spToDoSystem = make_shared<CToDoSystem>();
	ENSUREF(spToDoSystem->Initialize(_spToDoSystemDesc), L"Failed to initialize todo system.");
	return spToDoSystem;
}

HRESULT Client::CToDoSystem::Initialize(const std::shared_ptr<FToDoSystemDesc>& _spToDoSystemDesc)
{
	return S_OK;
}

HRESULT Client::CToDoSystem::BeginPlay()
{
	m_wpBuildManager = CBuildManager::GetInstance();

	return S_OK;
}

int32 Client::CToDoSystem::PreTick(float _fDeltaSeconds)
{
	return 0;
}

int32 Client::CToDoSystem::Tick(float _fDeltaSeconds)
{
	return 0;
}

int32 Client::CToDoSystem::PostTick(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Client::CToDoSystem::EndPlay()
{
	return S_OK;
}

void Client::CToDoSystem::Release()
{
}

HRESULT Client::CToDoSystem::AddSlavePal(const std::shared_ptr<CMonster>& _spSlavePal)
{
	const vector<EMonsterTaskType>& vecMonsterTaskTypes = CClientLoop::s_spClientDataSheet->k_mapPalTaskTypes.at(_spSlavePal->GetMonsterType());
	int32 iGameObjectID = _spSlavePal->GetGameObjectID();

	for (const auto& eTaskType : vecMonsterTaskTypes)
	{
		if (m_arrSlavePals[(int32)eTaskType].contains(iGameObjectID)) { continue; }

		m_arrSlavePals[(int32)eTaskType].insert({ iGameObjectID, _spSlavePal });
	}
	
	return S_OK;
}

HRESULT Client::CToDoSystem::RemoveSlavePal(const std::shared_ptr<CMonster>& _spSlavePal)
{
	const vector<EMonsterTaskType>& vecMonsterTaskTypes = CClientLoop::s_spClientDataSheet->k_mapPalTaskTypes.at(_spSlavePal->GetMonsterType());
	int32 iGameObjectID = _spSlavePal->GetGameObjectID();

	for (const auto& eTaskType : vecMonsterTaskTypes)
	{
		if (!m_arrSlavePals[(int32)eTaskType].contains(iGameObjectID)) { continue; }

		m_arrSlavePals[(int32)eTaskType][iGameObjectID] = nullptr;
		m_arrSlavePals[(int32)eTaskType].erase(iGameObjectID);
	}

	return S_OK;
}

void Client::CToDoSystem::CommandToSlavePal(EToDoType _eToDoType)
{
	for (auto& pairCmd : m_arrToDoCommands[(int32)_eToDoType])
	{
		// 이미 작업 중인 일인 경우
		if (pairCmd.second->m_eToDoCommandState == EToDoCommandState::STARTED) { continue; }

		// 작업 완료된 일인 경우
		if (pairCmd.second->m_eToDoCommandState == EToDoCommandState::FINISHED) { continue; }
		
		// 중지되거나 시작하지 않은 일인 경우
		std::shared_ptr<FToDoCommand> spToDoCommand = pairCmd.second;

		// 나무 상자가 없는데 운반하는 작업인 경우
		switch (spToDoCommand->m_eToDoType)
		{
		case EToDoType::TRANSPORT_MINERAL:
		case EToDoType::TRANSPORT_INGREDIENT:
		{
			if (FindArchitecture(EArchitectureType::IRON_BOX) == nullptr
				&& FindArchitecture(EArchitectureType::WOODEN_CHEST) == nullptr)
			{
				continue;
			}
		}
		break;
		}
		
		for (auto& pairPal : m_arrSlavePals[(int32)spToDoCommand->m_eMonsterTaskType])
		{
			if (pairPal.second->GetMonsterToDoState() == EMonsterToDoState::NO_WORK)
			{
				if (pairCmd.second->m_spHabitat)
				{
					pairCmd.second->m_spHabitat->AddWorkingSlavePal(pairPal.second);
				}
				
				else if (pairCmd.second->m_spItem)
				{
					pairCmd.second->m_spItem->AddWorkingSlavePal(pairPal.second);
				}

				pairPal.second->StartToDoCommand(spToDoCommand);
				spToDoCommand->m_eToDoCommandState = EToDoCommandState::STARTED;
				break;
			}
		}
	}
}

void Client::CToDoSystem::CommandToAllSlavePal()
{
	CommandToSlavePal(EToDoType::BUILD_ARCHITECTURE);
	CommandToSlavePal(EToDoType::PRODUCE_ITEM);
	CommandToSlavePal(EToDoType::FARM_BERRY);
	CommandToSlavePal(EToDoType::COOK_FOOD);
	CommandToSlavePal(EToDoType::CUT_DOWN_RAW_TREE);
	CommandToSlavePal(EToDoType::CUT_DOWN_LUMBERYARD);
	CommandToSlavePal(EToDoType::MINE_RAW_ROCK);
	CommandToSlavePal(EToDoType::MINE_STONE_PIT);
	CommandToSlavePal(EToDoType::TRANSPORT_MINERAL);
	CommandToSlavePal(EToDoType::TRANSPORT_INGREDIENT);
}

std::shared_ptr<CArchitecture> Client::CToDoSystem::FindArchitecture(EArchitectureType _eArchitectureType)
{
	const std::vector<std::shared_ptr<CArchitecture>>& vecArchitectures = m_wpBuildManager.lock()->GetBuildtArchitectures(_eArchitectureType);

	switch (_eArchitectureType)
	{
	case Client::EArchitectureType::WOODEN_CHEST:
	{
		for (const auto& spArchitecture : vecArchitectures)
		{
			shared_ptr<CChest> spChest = static_pointer_cast<CChest>(spArchitecture);
			if (spChest->IsBuildComplete())
			{
				return spChest;
			}
		}
	}
	break;

	case Client::EArchitectureType::IRON_BOX:
	{
		for (const auto& spArchitecture : vecArchitectures)
		{
			shared_ptr<CIronChest> spChest = static_pointer_cast<CIronChest>(spArchitecture);
			if (spChest->IsBuildComplete())
			{
				return spChest;
			}
		}
	}
	break;

	case Client::EArchitectureType::PAL_BED:
	{
		for (const auto& spArchitecture : vecArchitectures)
		{
			shared_ptr<CPalBed> spPalBed = static_pointer_cast<CPalBed>(spArchitecture);
			if (spPalBed->IsBuildComplete() && !spPalBed->IsOccupied())
			{
				spPalBed->SetOccupied(true);
				return spPalBed;
			}
		}
	}
	break;

	case Client::EArchitectureType::FEEDING_BOX:
	{
		for (const auto& spArchitecture : vecArchitectures)
		{
			shared_ptr<CFeedingBox> spFeedingBox = static_pointer_cast<CFeedingBox>(spArchitecture);
			if (spFeedingBox->IsBuildComplete())
			{
				return spFeedingBox;
			}
		}
	}
	break;

	case Client::EArchitectureType::PAL_BOX:
	{
		for (const auto& spArchitecture : vecArchitectures)
		{
			shared_ptr<CPalBox> spPalBox = static_pointer_cast<CPalBox>(spArchitecture);
			if (spPalBox->IsBuildComplete())
			{
				return spPalBox;
			}
		}
	}
	break;

	default:
		CHECKF(false, L"Architecture type is not designated.");
		break;
	}

	return nullptr;
}

HRESULT Client::CToDoSystem::SetCommandState(EToDoType _eToDoType, int32 _iCommandID, EToDoCommandState _eToDoCommandState)
{
	if (m_arrToDoCommands[(int32)_eToDoType].contains(_iCommandID))
	{
		m_arrToDoCommands[(int32)_eToDoType][_iCommandID]->m_eToDoCommandState = _eToDoCommandState;
		return S_OK; 
	}

	return E_FAIL;
}

EToDoCommandState Client::CToDoSystem::GetCommandState(EToDoType _eToDoType, int32 _iCommandID)
{
	if (m_arrToDoCommands[(int32)_eToDoType].contains(_iCommandID))
	{
		return m_arrToDoCommands[(int32)_eToDoType][_iCommandID]->m_eToDoCommandState;
	}

	return EToDoCommandState::ENUM_END;
}

int32 Client::CToDoSystem::AddCommandToSystem(FToDoCommand& _tToDoCommand)
{
	int32 iCommandID = ++m_iCommandID;
	_tToDoCommand.m_iCommandID = iCommandID;
	m_arrToDoCommands[(int32)_tToDoCommand.m_eToDoType].insert({ iCommandID, make_shared<FToDoCommand>(_tToDoCommand) });

	// 거점 팰에게 명령을 내립니다.
	CommandToSlavePal(_tToDoCommand.m_eToDoType);
	return iCommandID;
}

/*
HRESULT Client::CToDoSystem::RemoveCommandFromSystem(int32 _iCommandID)
{
	if (m_arrToDoCommands[(int32)EToDoType::BUILD].contains(_iCommandID))
	{
		m_arrToDoCommands[(int32)EToDoType::BUILD][_iCommandID] = nullptr;
		m_arrToDoCommands[(int32)EToDoType::BUILD].erase(_iCommandID);
		return S_OK;
	}

	if (m_arrToDoCommands[(int32)EToDoType::PRODUCE].contains(_iCommandID))
	{
		m_arrToDoCommands[(int32)EToDoType::PRODUCE][_iCommandID] = nullptr;
		m_arrToDoCommands[(int32)EToDoType::PRODUCE].erase(_iCommandID);
		return S_OK;
	}

	return E_FAIL;
}
*/

HRESULT Client::CToDoSystem::RemoveCommandFromSystem(EToDoType _eToDoType, int32 _iCommandID)
{
	if (m_arrToDoCommands[(int32)_eToDoType].contains(_iCommandID))
	{
		m_arrToDoCommands[(int32)_eToDoType][_iCommandID] = nullptr;
		m_arrToDoCommands[(int32)_eToDoType].erase(_iCommandID);
		return S_OK;
	}

	return E_FAIL;
}

void Client::CToDoSystem::PauseCommandFromSlavePal(EToDoType _eToDoType)
{
	// 다른 거점 팰에게 명령을 다시 내립니다.
	CommandToSlavePal(_eToDoType);
}

void Client::CToDoSystem::RequestCommandFromSlavePal()
{
	// 다른 거점 팰에게 명령을 다시 내립니다.
	CommandToAllSlavePal();
}
