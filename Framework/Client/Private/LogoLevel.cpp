#include "pch.h"
#include "LogoLevel.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "EventManager.h"
#include "UIManager.h"
#include "SoundManager.h"

#include "World.h"
#include "OpenLevelEvent.h"
#include "Panel.h"
#include "ButtonComponent.h"

HRESULT Client::CLogoLevel::Initialize()
{
	m_iLevelType = static_cast<int32>(ELevelType::LOGO);
	SetLevelName(L"LogoLevel");

	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();
	m_wpEventManager = CEngineModule::GetInstance()->GetEventManager();
	m_wpUIManager = CUIManager::GetInstance();

	cerr << "로고 레벨이 초기화되었습니다.\n";
	return CLevel::Initialize();
}

HRESULT Client::CLogoLevel::BeginPlay()
{
	if (FAILED(CLevel::BeginPlay())) { return E_FAIL; }

	CEngineModule::GetInstance()->GetSoundManager()->PlayBGM("BGM_title_01_484325676");

	shared_ptr<CPanel> spPanel = m_wpUIManager.lock()->FindParentsPanel(L"LogoUI");
	spPanel->SetActive(true);
	m_wpUIManager.lock()->SetTopPanel(spPanel);
	m_wpStartButton = spPanel->FindComponent<CButtonComponent>(L"GameStartButton");

	cerr << "로고 레벨이 시작되었습니다.\n";
	return S_OK;
}

int32 Client::CLogoLevel::PreTick(float _fDeltaSeconds)
{
	switch (CLevel::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}
	
	return 0;
}

int32 Client::CLogoLevel::Tick(float _fDeltaSeconds)
{
	switch (CLevel::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	if (nullptr != m_wpUIManager.lock()->GetFocusedControlComponent().lock())
	{
		if (m_wpStartButton.lock() == m_wpUIManager.lock()->GetFocusedControlComponent().lock())
		{
			m_wpStartButton.lock()->AddButtonClickEvent([this]() {this->ClickStartButtonEvent(); });
		}
	}

	return 0;
}

int32 Client::CLogoLevel::PostTick(float _fDeltaSeconds)
{
	switch (CLevel::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

void Client::CLogoLevel::Release()
{
	m_wpUIManager.lock()->FindParentsPanel(L"LogoUI")->SetActive(false);

	CLevel::Release();
}

void Client::CLogoLevel::ClickStartButtonEvent()
{
	m_wpUIManager.lock()->ResetTopPanel();

	CEngineModule::GetInstance()->GetSoundManager()->StopChannelEx(EChannelType::BACKGROUND_00);

	cerr << "섬 레벨로 전환합니다.\n";
	CWorld::GetInstance()->LoadLevel(static_cast<int32>(ELevelType::CUSTOMIZE));
}
