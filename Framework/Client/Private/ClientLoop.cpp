#include "pch.h"
#include "ClientLoop.h"

#include "EngineModule.h"
#include "ClientModule.h"
#include "World.h"

#include "LogoLevel.h"
#include "LoadLevel.h"
#include "PersistentLevel.h"
#include "SampleLevel.h"
#include "IslandLevel.h"

#include "UIManager.h"

std::shared_ptr<FClientDataSheet> CClientLoop::s_spClientDataSheet = make_shared<FClientDataSheet>();

HRESULT Client::CClientLoop::Initialize()
{
#ifdef _DEBUG
	if (AllocConsole() == TRUE)
	{
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}
#endif

	// ���� ����� �����ϰ� �ʱ�ȭ�մϴ�.
	m_spEngineModule = CEngineModule::GetInstance();
	m_spEngineModule->Initialize(g_hWnd, g_hInst, g_iWindowSizeX, g_iWindowSizeY);

	// Ŭ���̾�Ʈ ����� �����մϴ�.
	m_spClientModule = CClientModule::GetInstance();

	// ���带 �����ϰ� �ʱ�ȭ�մϴ�.
	m_spWorld = CWorld::GetInstance();
	m_spWorld->Initialize
	(
		make_shared<CPersistentLevel>(),
		make_shared<CLoadLevel>()
	);

	return S_OK;
}

HRESULT Client::CClientLoop::BeginPlay()
{
	// ���� ����, �ε� ������ ����(beginplay)�Ǿ����ϴ�.
	m_spWorld->BeginPlay();
	return S_OK;
}

int32 Client::CClientLoop::Tick()
{
	float fDeltaSeconds = 0.0f;
	if (m_spEngineModule->PreTick(&fDeltaSeconds)) { return -1; }
	if (0.1f <= fDeltaSeconds) { fDeltaSeconds = 0.1f; }
	m_spClientModule->PreTick(fDeltaSeconds);

	m_spEngineModule->Tick(fDeltaSeconds);
	m_spClientModule->Tick(fDeltaSeconds);
	m_spWorld->Tick(fDeltaSeconds);

	m_spEngineModule->PostTick(fDeltaSeconds);
	m_spClientModule->PostTick(fDeltaSeconds);
	return 0;
}

void Client::CClientLoop::Render()
{
	m_spEngineModule->PreRender();
	m_spClientModule->Render();
	m_spEngineModule->PostRender();
	
	m_spWorld->Clear();

	// �̺�Ʈ�� ó���մϴ�.
	m_spEngineModule->HandleEvent();
}

void Client::CClientLoop::Release()
{
#ifdef _DEBUG
	FreeConsole();
#endif
	s_spClientDataSheet->Release();
	s_spClientDataSheet = nullptr;

	m_spWorld->Release();
	m_spWorld = nullptr;

	m_spClientModule->Release();
	m_spClientModule = nullptr;

	m_spEngineModule->Release();
	m_spEngineModule = nullptr;
}
