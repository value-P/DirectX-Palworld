// default
#include "pch.h"
#include "LoadLevel.h"

// engine manager
#include "EngineModule.h"
#include "InputManager.h"
#include "ThreadManager.h"
#include "EventManager.h"
#include "RenderManager.h"

#include "ClientModule.h"

// handler
#include "FontHandler.h"
#include "TextureHandler.h"
#include "SoundHandler.h"
#include "ShaderHandler.h"
#include "ModelHandler.h"

// client manager
#include "ClientModule.h"

// level
#include "World.h"
#include "LogoLevel.h"
#include "PersistentLevel.h"
#include "SampleLevel.h"
#include "IslandLevel.h"
#include "TowerLevel.h"
#include "CustomizeLevel.h"

// event
#include "OpenLevelEvent.h"

#include "UIManager.h"
#include "Panel.h"

HRESULT Client::CLoadLevel::Initialize()
{
	{
		m_iLevelType = static_cast<int32>(ELevelType::LOAD);
		SetLevelName(L"LoadLevel");
	}

	m_wpThreadManager = CEngineModule::GetInstance()->GetThreadManager();
	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();
	m_wpEventManager = CEngineModule::GetInstance()->GetEventManager();

	m_wpEngineModule = CEngineModule::GetInstance();
	m_wpClientModule = CClientModule::GetInstance();

	m_vecLoadResourceFutures.resize(12);
	m_vecLoadResourceFutures.reserve(12);
	m_vecLoadResourceCompleted.resize(12);
	m_vecLoadResourceCompleted.reserve(12);

	m_vecInitLevelFutures.resize(12);
	m_vecInitLevelFutures.reserve(12);
	m_vecInitLevelCompleted.resize(12);
	m_vecInitLevelCompleted.reserve(12);

	cerr << "�ε� ������ �ʱ�ȭ�Ǿ����ϴ�.\n";
	return CLevel::Initialize();
}

HRESULT Client::CLoadLevel::BeginPlay()
{
	if (FAILED(CLevel::BeginPlay())) { return E_FAIL; }

	cerr << "�ε� ������ ���۵Ǿ����ϴ�.\n";
	return S_OK;
}

int32 Client::CLoadLevel::PreTick(float _fDeltaSeconds)
{
	switch (CLevel::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CLoadLevel::Tick(float _fDeltaSeconds)
{
	switch (CLevel::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	CheckLoadResourceFuture();
	CheckInitLevelFuture();
	return 0;
}

int32 Client::CLoadLevel::PostTick(float _fDeltaSeconds)
{
	switch (CLevel::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

void Client::CLoadLevel::Release()
{
	m_spPostFrontLevel = nullptr;

	CLevel::Release();
}

void Client::CLoadLevel::LoadResource(int32 _iLevelType)
{
	CEngineModule::GetInstance()->GetRenderManager()->SetActive(false);

	m_iPostLevelType = _iLevelType;

	if (ELevelType::LOGO == static_cast<ELevelType>(_iLevelType))
	{
		m_wpClientModule.lock()->SetCurLevelType(ELevelType::PRE_LOAD);
	}
	else
	{
		m_wpClientModule.lock()->SetCurLevelType(ELevelType::LOAD);
	}

	switch (static_cast<ELevelType>(_iLevelType))
	{
	case Client::ELevelType::LOAD:
		CHECK(false);
		break;

	case Client::ELevelType::PERSISTENT:
		CHECK(false);
		break;

	case Client::ELevelType::LOGO:
	{
		// ���������� ����Ʈ ���̴��� ����� �ε��մϴ�.
		cerr << "�ΰ� ������ �ε��ϴ� ��...\n";
		CShaderHandler::GetInstance()->LoadEffectShaderFromFile();

		std::future<HRESULT> ftResult = m_wpThreadManager.lock()->AddTask([this]()
			{
				if (FAILED(CoInitialize(nullptr))) { return E_FAIL; }

				if (FAILED(LoadLogoLevelResource())) { return E_FAIL; };

				CoUninitialize();
				return S_OK;
			}
		);
		m_vecLoadResourceFutures[_iLevelType] = std::move(ftResult);
	}
	break;

	case Client::ELevelType::ISLAND:
	{
		std::future<HRESULT> ftResult = m_wpThreadManager.lock()->AddTask([this]() { return LoadIslandLevelResource(); });
		m_vecLoadResourceFutures[_iLevelType] = std::move(ftResult);
	}
	break;

	case Client::ELevelType::SAMPLE:
	{
		std::future<HRESULT> ftResult = m_wpThreadManager.lock()->AddTask([this]() { return LoadSampleLevelResource(); });
		m_vecLoadResourceFutures[_iLevelType] = std::move(ftResult);
	}
	break;

	case Client::ELevelType::TOWER:
	{
		cerr << "Ÿ�� ������ �ε��ϴ� ��...\n";
		std::future<HRESULT> ftResult = m_wpThreadManager.lock()->AddTask([this]() { return LoadTowerLevelResource(); });
		m_vecLoadResourceFutures[_iLevelType] = std::move(ftResult);
	}
	break;

	case Client::ELevelType::CUSTOMIZE:
	{
		std::future<HRESULT> ftResult = m_wpThreadManager.lock()->AddTask([this]() { return LoadCustomizeLevelResource(); });
		m_vecLoadResourceFutures[_iLevelType] = std::move(ftResult);
	}
	break;

	default:
		CHECKF(false, L"Level type is not designated.");
		break;
	}

}

void Client::CLoadLevel::CheckLoadResourceFuture()
{
	// ���ҽ��� �ε�Ǿ����� ǻ�ĸ� Ȯ���մϴ�.
	for (size_t iIndex = 0; iIndex < m_vecLoadResourceFutures.size(); ++iIndex)
	{
		if (!m_vecLoadResourceFutures[iIndex].valid()) { continue; }

		if (m_vecLoadResourceFutures[iIndex]._Is_ready())
		{
			HRESULT hResult = m_vecLoadResourceFutures[iIndex].get();
			ENSUREF(hResult, L"Failed to load resource.");

			m_vecLoadResourceCompleted[iIndex] = true;
		}
	}

	// ������ �ʱ�ȭ�մϴ�.
	if (m_vecLoadResourceCompleted[m_iPostLevelType])
	{
		m_spPostFrontLevel = nullptr;

		ELevelType ePostLevelType = static_cast<ELevelType>(m_iPostLevelType);
		switch (ePostLevelType)
		{
		case ELevelType::LOAD:
			// �ε� ������ �̹� �����Ǿ� �ֽ��ϴ�.
			break;

		case ELevelType::LOGO:			m_spPostFrontLevel = make_shared<CLogoLevel>();			break;
		case ELevelType::SAMPLE:		m_spPostFrontLevel = make_shared<CSampleLevel>();		break;
		case ELevelType::ISLAND:		m_spPostFrontLevel = make_shared<CIslandLevel>();		break;
		case ELevelType::TOWER:			m_spPostFrontLevel = make_shared<CTowerLevel>();		break;
		case ELevelType::CUSTOMIZE:		m_spPostFrontLevel = make_shared<CCustomizeLevel>();	break;
		default:						CHECKF(false, L"Level type is not designated.");		break;
		}

		CWorld::GetInstance()->SetPostFrontLevel(m_spPostFrontLevel);

		std::future<HRESULT> ftResult = m_wpThreadManager.lock()->AddTask
		(
			[this]()
			{
				if (FAILED(m_wpClientModule.lock()->Initialize(m_iPostLevelType))) { return E_FAIL; }

				if (FAILED(m_spPostFrontLevel->Initialize())) { return E_FAIL; };

				return S_OK;
			}
		);
		m_vecInitLevelFutures[m_iPostLevelType] = std::move(ftResult);

		m_vecLoadResourceCompleted[m_iPostLevelType] = false;
	}
}

void Client::CLoadLevel::CheckInitLevelFuture()
{
	// ������ �ʱ�ȭ�Ǿ����� ǻ�ĸ� Ȯ���մϴ�.
	for (size_t iIndex = 0; iIndex < m_vecInitLevelFutures.size(); ++iIndex)
	{
		if (!m_vecInitLevelFutures[iIndex].valid()) { continue; }

		if (m_vecInitLevelFutures[iIndex]._Is_ready())
		{
			HRESULT hResult = m_vecInitLevelFutures[iIndex].get();
			ENSUREF(hResult, L"Failed to initialize level.");

			m_vecInitLevelCompleted[iIndex] = true;
		}
	}

	// ������ ��ȯ�մϴ�.
	if (m_vecInitLevelCompleted[m_iPostLevelType] == true)
	{
		if (m_wpInputManager.lock()->IsKeyDown(DIK_SPACE))
		{
			FOpenLevelEventDesc tOpenlevelEventDesc;
			tOpenlevelEventDesc.m_eEventType = EEventType::OPEN_LEVEL;
			tOpenlevelEventDesc.m_eEventInvokeType = EEventInvokeType::FORWARD;
			tOpenlevelEventDesc.m_ePostLevelType = static_cast<ELevelType>(m_iPostLevelType);

			tOpenlevelEventDesc.m_spPostFrontLevel = m_spPostFrontLevel;
			tOpenlevelEventDesc.m_spPostBackLevel = nullptr;

			ENSUREF(m_wpClientModule.lock()->BeginPlay(m_iPostLevelType), L"Failed to beginplay client module.");
			ENSUREF(m_spPostFrontLevel->BeginPlay(), L"Failed to beginplay level.");

			m_wpEventManager.lock()->AddEvent(COpenLevelEvent::Create(make_shared<FOpenLevelEventDesc>(tOpenlevelEventDesc)));
			m_vecInitLevelCompleted[m_iPostLevelType] = false;

			m_spPostFrontLevel = nullptr;
		}
	}
}

HRESULT Client::CLoadLevel::LoadLogoLevelResource()
{
	// �ΰ� ������ �ʱ�ȭ�ϱ� ���� ���ҽ��� �ҷ��ɴϴ�.
	CFontHandler::GetInstance()->LoadFontFromFile();
	CSoundHandler::GetInstance()->LoadSoundFromFile();
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::UI);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::DIFFUSE);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::NOISE);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::MASK);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::DISTORTION);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::DISSOLVE);
	CModelHandler::GetInstance()->LoadEffectMeshPaths();
	return S_OK;
}

HRESULT Client::CLoadLevel::LoadCustomizeLevelResource()
{
	return S_OK;
}

HRESULT Client::CLoadLevel::LoadSampleLevelResource()
{
	// ���� ������ �ʱ�ȭ�ϱ� ���� ���ҽ��� �ҷ��ɴϴ�.

	return S_OK;
}

HRESULT Client::CLoadLevel::LoadIslandLevelResource()
{
	// �� ������ �ʱ�ȭ�ϱ� ���� ���ҽ��� �ҷ��ɴϴ�.

	return S_OK;
}

HRESULT Client::CLoadLevel::LoadTowerLevelResource()
{
	// ž ������ �ʱ�ȭ�ϱ� ���� ���ҽ��� �ҷ��ɴϴ�.

	return S_OK;
}
