#include "pch.h"
#include "ClientModule.h"

#include "AsyncManager.h"
#include "BuildManager.h"
#include "EffectManager.h"
#include "GameManager.h"
#include "NotifyManager.h"
#include "PoolManager.h"
#include "TreeManager.h"
#include "UIManager.h"

IMPLEMENT_SINGLETON(CClientModule)

HRESULT Client::CClientModule::Initialize(int32 _iLevelType)
{
	switch (static_cast<ELevelType>(_iLevelType))
	{
	case Client::ELevelType::LOGO:
	case Client::ELevelType::CUSTOMIZE:
	{
		m_spUIManager = CUIManager::GetInstance();

		if (!m_spUIManager->m_bInitialized)
		{
			ENSUREF(m_spUIManager->Initialize(), L"Failed to initialize ui manager.");
			m_spUIManager->m_bInitialized = true;
		}
	}
	break;

	case Client::ELevelType::SAMPLE:
	case Client::ELevelType::ISLAND:
	{
		m_spTreeManager = CTreeManager::GetInstance();
		if (!m_spTreeManager->m_bInitialized)
		{
			//ENSUREF(m_spTreeManager->Initialize(), L"Failed to initialize tree manager.");
			m_spTreeManager->m_bInitialized = true;
		}

		m_spAsyncManager = CAsyncManager::GetInstance();
		if (!m_spAsyncManager->m_bInitialized)
		{
			//ENSUREF(m_spAsyncManager->Initialize(), L"Failed to initialize async manager.");
			m_spAsyncManager->m_bInitialized = true;
		}

		m_spGameManager = CGameManager::GetInstance();
		if (!m_spGameManager->m_bInitialized)
		{
			ENSUREF(m_spGameManager->Initialize(), L"Failed to initialize game manager.");
			m_spGameManager->m_bInitialized = true;
		}

		m_spEffectManager = CEffectManager::GetInstance();
		if (!m_spEffectManager->m_bInitialized)
		{
			ENSUREF(m_spEffectManager->Initialize(), L"Failed to initialize effect manager.");// json 파일 읽기
			m_spEffectManager->m_bInitialized = true;
		}

		m_spNotifyManager = CNotifyManager::GetInstance();
		if (!m_spNotifyManager->m_bInitialized)
		{
			ENSUREF(m_spNotifyManager->Initialize(), L"Failed to initialize notify manager.");
			m_spNotifyManager->m_bInitialized = true;
		}

		m_spBuildManager = CBuildManager::GetInstance();
		if (!m_spBuildManager->m_bInitialized)
		{
			ENSUREF(m_spBuildManager->Initialize(), L"Failed to initialize build manager.");// 프리팹 초기화
			m_spBuildManager->m_bInitialized = true;
		}
	}
	break;

	case Client::ELevelType::TOWER:
	{
		// 트리 매니저를 해제합니다.
		m_spTreeManager = CTreeManager::GetInstance();
		if (m_spTreeManager->m_bInitialized)
		{
			m_spTreeManager->Release();
			m_spTreeManager->m_bInitialized = false;
			m_spTreeManager = nullptr;
		}

		// 비동기 매니저를 해제합니다.
		m_spAsyncManager = CAsyncManager::GetInstance();
		if (m_spAsyncManager->m_bInitialized)
		{
			m_spAsyncManager->Release();
			m_spAsyncManager->m_bInitialized = false;
			m_spAsyncManager = nullptr;
		}

		m_spGameManager = CGameManager::GetInstance();
		if (!m_spGameManager->m_bInitialized)
		{
			ENSUREF(m_spGameManager->Initialize(), L"Failed to initialize game manager.");
			m_spGameManager->m_bInitialized = true;
		}

		m_spEffectManager = CEffectManager::GetInstance();
		if (!m_spEffectManager->m_bInitialized)
		{
			ENSUREF(m_spEffectManager->Initialize(), L"Failed to initialize effect manager.");// json 파일 읽기
			m_spEffectManager->m_bInitialized = true;
		}

		m_spNotifyManager = CNotifyManager::GetInstance();
		if (!m_spNotifyManager->m_bInitialized)
		{
			ENSUREF(m_spNotifyManager->Initialize(), L"Failed to initialize notify manager.");
			m_spNotifyManager->m_bInitialized = true;
		}

		m_spBuildManager = CBuildManager::GetInstance();
		if (!m_spBuildManager->m_bInitialized)
		{
			ENSUREF(m_spBuildManager->Initialize(), L"Failed to initialize build manager.");// 프리팹 초기화
			m_spBuildManager->m_bInitialized = true;
		}
	}
	break;

	default:
		CHECKF(false, L"Level type is not designated.");
		break;
	}

	return S_OK;
}

HRESULT Client::CClientModule::BeginPlay(int32 _iLevelType)
{
	switch (static_cast<ELevelType>(_iLevelType))
	{
	case Client::ELevelType::PRE_LOAD:
	case Client::ELevelType::LOAD:
	case Client::ELevelType::PERSISTENT:
		break;

	case Client::ELevelType::LOGO:
	{
		ENSURE(m_spUIManager->BeginPlay());
	}
	break;

	case Client::ELevelType::SAMPLE:
	case Client::ELevelType::ISLAND:
	{
		ENSURE(m_spUIManager->BeginPlay());

		if (!m_spTreeManager->m_bBeginPlayed.load())
		{
			ENSURE(m_spTreeManager->BeginPlay());
			m_spTreeManager->m_bBeginPlayed.store(true);
		}
		
		if (!m_spAsyncManager->m_bBeginPlayed.load())
		{
			ENSURE(m_spAsyncManager->BeginPlay());
			m_spAsyncManager->m_bBeginPlayed.store(true);
		}

		if (!m_spGameManager->m_bBeginPlayed.load())
		{
			ENSURE(m_spGameManager->BeginPlay());
			m_spGameManager->m_bBeginPlayed.store(true);
		}
		
		if (!m_spEffectManager->m_bBeginPlayed.load())
		{
			ENSURE(m_spEffectManager->BeginPlay());
			m_spEffectManager->m_bBeginPlayed.store(true);
		}

		if (!m_spNotifyManager->m_bBeginPlayed.load())
		{
			ENSURE(m_spNotifyManager->BeginPlay());
			m_spNotifyManager->m_bBeginPlayed.store(true);
		}

		if (!m_spBuildManager->m_bBeginPlayed.load())
		{
			ENSURE(m_spBuildManager->BeginPlay());
			m_spBuildManager->m_bBeginPlayed.store(true);
		}
	}
	break;

	case Client::ELevelType::TOWER:
	{
		if (!m_spGameManager->m_bBeginPlayed.load())
		{
			ENSURE(m_spGameManager->BeginPlay());
			m_spGameManager->m_bBeginPlayed.store(true);
		}

		if (!m_spEffectManager->m_bBeginPlayed.load())
		{
			ENSURE(m_spEffectManager->BeginPlay());
			m_spEffectManager->m_bBeginPlayed.store(true);
		}

		if (!m_spNotifyManager->m_bBeginPlayed.load())
		{
			ENSURE(m_spNotifyManager->BeginPlay());
			m_spNotifyManager->m_bBeginPlayed.store(true);
		}

		if (!m_spBuildManager->m_bBeginPlayed.load())
		{
			ENSURE(m_spBuildManager->BeginPlay());
			m_spBuildManager->m_bBeginPlayed.store(true);
		}
	}
	break;

	default:
		break;
	}

	return S_OK;
}

HRESULT Client::CClientModule::Initialize()
{
	m_spUIManager = CUIManager::GetInstance();
	ENSUREF(m_spUIManager->Initialize(), L"Failed to initialize ui manager.");
	m_spUIManager->m_bInitialized = true;

	m_spTreeManager = CTreeManager::GetInstance();
	ENSUREF(m_spTreeManager->Initialize(), L"Failed to initialize tree manager.");
	m_spTreeManager->m_bInitialized = true;
	
	m_spAsyncManager = CAsyncManager::GetInstance();
	ENSUREF(m_spAsyncManager->Initialize(), L"Failed to initialize async manager.");
	m_spAsyncManager->m_bInitialized = true;
	
	m_spGameManager = CGameManager::GetInstance();
	ENSUREF(m_spGameManager->Initialize(), L"Failed to initialize game manager.");
	m_spGameManager->m_bInitialized = true;

	m_spBuildManager = CBuildManager::GetInstance();
	ENSUREF(m_spBuildManager->Initialize(), L"Failed to initialize build manager.");
	m_spBuildManager->m_bInitialized = true;

	m_spEffectManager = CEffectManager::GetInstance();
	ENSUREF(m_spEffectManager->Initialize(), L"Failed to initialize effect manager.");
	m_spEffectManager->m_bInitialized = true;
	
	m_spNotifyManager = CNotifyManager::GetInstance();
	ENSUREF(m_spNotifyManager->Initialize(), L"Failed to initialize notify manager.");
	m_spNotifyManager->m_bInitialized = true;

	return S_OK;
}

HRESULT Client::CClientModule::BeginPlay()
{
	ENSUREF(m_spTreeManager->BeginPlay(), L"Failed to initialize tree manager.");
	m_spTreeManager->m_bBeginPlayed = true;
	
	ENSUREF(m_spGameManager->BeginPlay(), L"Failed to initialize game manager.");
	m_spGameManager->m_bBeginPlayed = true;

	ENSUREF(m_spBuildManager->BeginPlay(), L"Failed to initialize notify manager.");
	m_spBuildManager->m_bBeginPlayed = true;

	ENSUREF(m_spEffectManager->BeginPlay(), L"Failed to initialize effect manager.");// json 파일 읽기
	m_spEffectManager->m_bBeginPlayed = true;

	ENSUREF(m_spNotifyManager->BeginPlay(), L"Failed to initialize notify manager.");
	m_spNotifyManager->m_bBeginPlayed = true;

	return S_OK;
}

int32 Client::CClientModule::PreTick(float _fDeltaSeconds)
{
	switch (m_eCurLevelType.load())
	{
	case ELevelType::LOAD:
	case ELevelType::LOGO:
		break;

	case ELevelType::ISLAND:
	case ELevelType::TOWER:
		m_spGameManager->PreTick(_fDeltaSeconds);
		break;

	default:
		break;
	}

	return 0;
}

int32 Client::CClientModule::Tick(float _fDeltaSeconds)
{
	switch (m_eCurLevelType.load())
	{
	case ELevelType::LOAD:
	case ELevelType::LOGO:
		m_spUIManager->Tick(_fDeltaSeconds);
		break;

	case ELevelType::ISLAND:
		m_spAsyncManager->Tick(_fDeltaSeconds);
		m_spBuildManager->Tick(_fDeltaSeconds);
		m_spUIManager->Tick(_fDeltaSeconds);
		m_spGameManager->Tick(_fDeltaSeconds);
		m_spNotifyManager->Tick();
		break;

	case ELevelType::TOWER:
		m_spBuildManager->Tick(_fDeltaSeconds);
		m_spUIManager->Tick(_fDeltaSeconds);
		m_spGameManager->Tick(_fDeltaSeconds);
		m_spNotifyManager->Tick();
		break;

	case ELevelType::CUSTOMIZE:
		m_spUIManager->Tick(_fDeltaSeconds);
		break;

	default:
		break;
	}

	return 0;
}

int32 Client::CClientModule::PostTick(float _fDeltaSeconds)
{
	switch (m_eCurLevelType.load())
	{
	case ELevelType::ISLAND:
	case ELevelType::TOWER:
		m_spTreeManager->Tick(_fDeltaSeconds);
		m_spGameManager->PostTick(_fDeltaSeconds);
		break;

	default:
		break;
	}

	return 0;
}

void Client::CClientModule::Render()
{
}

void Client::CClientModule::Release()
{
	if (m_spUIManager)
	{
		m_spUIManager->Release();
		m_spUIManager = nullptr;
	}
	
	if (m_spGameManager)
	{
		m_spGameManager->Release();
		m_spGameManager = nullptr;
	}

	if (m_spAsyncManager)
	{
		m_spAsyncManager->Release();
		m_spAsyncManager = nullptr;
	}

	if (m_spTreeManager)
	{
		m_spTreeManager->Release();
		m_spTreeManager = nullptr;
	}
	
	if (m_spBuildManager)
	{
		m_spBuildManager->Release();
		m_spBuildManager = nullptr;
	}
	
	if (m_spEffectManager)
	{
		m_spEffectManager->Release();
		m_spEffectManager = nullptr;
	}

	if (m_spNotifyManager)
	{
		m_spNotifyManager->Release();
		m_spNotifyManager = nullptr;
	}
	
	m_spInstance = nullptr;
}
