#include "pch.h"
#include "TowerLevel.h"

// engine manager
#include "EngineModule.h"
#include "LightManager.h"

// client manager
#include "GameManager.h"
#include "AsyncManager.h"

// gameobject
#include "BossRoom.h"
#include "Player.h"
#include "InventoryCustomObject.h"

// engine
#include "World.h"

HRESULT Client::CTowerLevel::Initialize()
{
	// island level
	{
		m_iLevelType = static_cast<int32>(ELevelType::TOWER);
		SetLevelName(L"TowerLevel");
	}

	// light
	{
		FDirectionalLight tDirLight;

		tDirLight.vDirection = Vector3(0.0f, -1.0f, 1.0f);
		tDirLight.vDirection.Normalize();
		tDirLight.vColor = Vector3(1.0f);
		tDirLight.fIntensity = 1;

		CEngineModule::GetInstance()->GetLightManager()->AddLight(L"TowerLight", make_shared<FDirectionalLight>(tDirLight), true);
	}

	// game manager
	{
		CGameManager::GetInstance()->SetCurLevel(shared_from_this());
		m_spPlayer = CGameManager::GetInstance()->GetPlayer();
		m_spPlayer->SetPositionWithController(Vector3(0.0f, 10.0f, 0.0f));
	}

	// async manager
	{
		CAsyncManager::GetInstance()->SetSyncTarget(nullptr);
	}

	return CLevel::Initialize();
}

HRESULT Client::CTowerLevel::BeginPlay()
{
	if (FAILED(CLevel::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CTowerLevel::PreTick(float _fDeltaSeconds)
{
	switch (CLevel::PreTick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

int32 Client::CTowerLevel::Tick(float _fDeltaSeconds)
{
	switch (CLevel::Tick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

int32 Client::CTowerLevel::PostTick(float _fDeltaSeconds)
{
	switch (CLevel::PostTick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

void Client::CTowerLevel::Release()
{
	CLevel::Release();
}
