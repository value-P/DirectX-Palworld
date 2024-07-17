// default
#include "pch.h"
#include "Habitat.h"

// manager
#include "GameManager.h"
#include "TreeManager.h"
#include "AsyncManager.h"

// component
#include "MeshComponent.h"

// system
#include "ToDoSystem.h"

// gameobject
#include "Monster.h"

HRESULT Client::CHabitat::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// gameobject desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// habitat desc
	shared_ptr<FHabitatDesc> spHabitatDesc = static_pointer_cast<FHabitatDesc>(_spGameObjectDesc);
	m_eHabitatType = spHabitatDesc->m_eHabitatType;

	// etc
	m_wpGameManager  = CGameManager::GetInstance();
	m_wpTreeManager  = CTreeManager::GetInstance();
	m_wpAsyncManager = CAsyncManager::GetInstance();
	m_wpToDoSystem   = CGameManager::GetInstance()->GetToDoSystem();
	return S_OK;
}

HRESULT Client::CHabitat::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CHabitat::PreTick(float _fDeltaSeconds)
{
	switch(CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CHabitat::Tick(float _fDeltaSeconds)
{
	switch(CGameObject::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CHabitat::PostTick(float _fDeltaSeconds)
{
	switch(CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CHabitat::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CHabitat::Release()
{
	m_spMeshComponent = nullptr;

	for (auto& pair : m_umapMeshComponents)
	{
		if (nullptr != pair.second)
		{
			pair.second->Release();
			pair.second = nullptr;
		}
	}

	CGameObject::Release();
}

const std::wstring& Client::CHabitat::GetModelFilePath()
{
	return m_spMeshComponent->GetModelFilePath();
}

HRESULT Client::CHabitat::AddHabitatToScene()
{
	return S_OK;
}

HRESULT Client::CHabitat::RemoveHabitatFromScene()
{
	return S_OK;
}

int32 Client::CHabitat::RegisterCommandToSystem(EToDoType _eToDoType, EMonsterTaskType _eMonsterTaskType)
{
	FToDoCommand tToDoCommand;
	tToDoCommand.m_eToDoType = _eToDoType;
	tToDoCommand.m_eMonsterTaskType = _eMonsterTaskType;
	tToDoCommand.m_spHabitat = dynamic_pointer_cast<CHabitat>(shared_from_this());

	return m_wpToDoSystem.lock()->AddCommandToSystem(tToDoCommand);
}

HRESULT Client::CHabitat::UnRegisterCommandFromSystem(EToDoType _eToDoType, int32 _iCurCommandID)
{
	if (_iCurCommandID == -1) { _iCurCommandID = m_iCurCommandID; }

	return m_wpToDoSystem.lock()->RemoveCommandFromSystem(_eToDoType, _iCurCommandID);
}

HRESULT Client::CHabitat::ChangeCommandState(EToDoType _eToDoType, EToDoCommandState _eToDoCommandState)
{
	return m_wpToDoSystem.lock()->SetCommandState(_eToDoType, m_iCurCommandID, _eToDoCommandState);
}

HRESULT Client::CHabitat::AddWorkingSlavePal(const std::shared_ptr<CMonster>& _spMonster)
{
	int32 iGameObjectID = _spMonster->GetGameObjectID();
	if (m_umapWorkingSlavePals.contains(iGameObjectID)) { return E_FAIL; }

	m_umapWorkingSlavePals.emplace(iGameObjectID, _spMonster);
	return S_OK;
}

HRESULT Client::CHabitat::RemoveWorkingSlavePal(int32 _iGameObjectID)
{
	if (!m_umapWorkingSlavePals.contains(_iGameObjectID)) { return E_FAIL; }

	m_umapWorkingSlavePals[_iGameObjectID].reset();
	m_umapWorkingSlavePals.erase(_iGameObjectID);
	return S_OK;
}
