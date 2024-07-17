#include "pch.h"
#include "ContainerSystem.h"

// Engine 
#include "EngineModule.h"
#include "InputManager.h"
#include "SoundManager.h"

// Client Manager
#include "GameManager.h"
#include "UIManager.h"

// GameObject
#include "Panel.h"
#include "Player.h"

// Component
#include "ImageComponent.h"
#include "CameraComponent.h"

HRESULT Client::CContainerSystem::Initialize(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc)
{
	// initialize manager
	m_spInputManager = CEngineModule::GetInstance()->GetInputManager();
	m_spGameManager = CGameManager::GetInstance();
	m_spUIManager = CUIManager::GetInstance();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	return S_OK;
}

HRESULT Client::CContainerSystem::BeginPlay()
{
	m_wpPlayer = m_spGameManager->GetPlayer();

	return S_OK;
}

int32 Client::CContainerSystem::PreTick(float _fDeltaSeconds)
{
	return 0;
}

int32 Client::CContainerSystem::Tick(float _fDeltaSeconds)
{
	m_wpCurHoverSlotComponent		   = static_pointer_cast<CImageComponent>(m_spUIManager->GetHoverImageComponent().lock());
	m_wpCurLeftClickedSlotComponent    = static_pointer_cast<CImageComponent>(m_spUIManager->GetFocusedControlComponent().lock());
	m_wpCurRightClickedSlotComponent   = static_pointer_cast<CImageComponent>(m_spUIManager->GetRightClickFocusedControlComponent().lock());

	return 0;
}

int32 Client::CContainerSystem::PostTick(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Client::CContainerSystem::EndPlay()
{
	return S_OK;
}

void Client::CContainerSystem::Release()
{
	m_spUIManager = nullptr;
	m_spInputManager = nullptr;
}

void Client::CContainerSystem::AddItemToContainer(const std::shared_ptr<CItem>& _spItem)
{
}

void Client::CContainerSystem::AddItemToContainer(EItemType _eItemType, int32 _iNumItem)
{
}

void Client::CContainerSystem::SortItemsInContainer(ESortingType _eSortingType)
{
}

void Client::CContainerSystem::ActiveSlotHover()
{
	//m_wpSoundManager.lock()->PlaySoundEx("NormalClickSound");
}

void Client::CContainerSystem::OpenContainerWidget(int32 _iKey)
{
	if (!m_bOpenContainerWidget && m_spInputManager->IsKeyDown(_iKey))
	{
		m_wpSoundManager.lock()->PlaySoundEx("Mainmenu_Open");
		m_spGameManager->GetPlayer()->GetCameraComponent()->SetMouseControl(false);
		m_bOpenContainerWidget = !m_bOpenContainerWidget; 
		m_spContainerParentPanel->SetActive(true);
		m_spUIManager->SetTopPanel(m_spContainerParentPanel);
	}
	else if (m_bOpenContainerWidget && m_spInputManager->IsKeyDown(_iKey))
	{
		m_wpSoundManager.lock()->PlaySoundEx("Mainmenu_Close");
		m_spGameManager->GetPlayer()->GetCameraComponent()->SetMouseControl(true);
		m_bOpenContainerWidget = !m_bOpenContainerWidget;
		m_spContainerParentPanel->SetActive(false);
		m_spUIManager->SetTopPanel(nullptr);
	}
}

void Client::CContainerSystem::UpdateCurMouseSlotPos()
{
	if (m_wpCurMouseSlotComponent.expired()) { return; }

	Vector2 vCursorPos;
	vCursorPos.x = static_cast<float>(m_spInputManager->GetCursorScreenPosition().first) + 23.5f;
	vCursorPos.y = static_cast<float>(m_spInputManager->GetCursorScreenPosition().second);

	m_wpCurMouseSlotComponent.lock()->SetCenterPosition(vCursorPos);
}

std::pair<int32, int32> Client::CContainerSystem::ExtractLastChar(const std::wstring& _wstrComponentName, int32 _iNumberWantToExtract)
{
	if (_wstrComponentName.length() < _iNumberWantToExtract) { return pair<int32, int32>(); }
	std::wstring wstrLastTwoChars = _wstrComponentName.substr(_wstrComponentName.length() - _iNumberWantToExtract);
	if (wstrLastTwoChars.length() != _iNumberWantToExtract) { return pair<int32, int32>(); }

	wchar wchFirstChar = wstrLastTwoChars.at(0);
	wchar wchSecondChar = wstrLastTwoChars.at(1);

	std::wstring wstrFirstChar(&wchFirstChar);
	std::wstring wstrSecondChar(&wchSecondChar);

	pair<int32, int32> prIndexs{};
	prIndexs.first = std::stoi(wstrFirstChar);
	prIndexs.second = std::stoi(wstrSecondChar);

	return prIndexs;
}

std::wstring Client::CContainerSystem::ExceptLastChar(const std::wstring& _wstrComponentName, int32 _iNumberWantToExtract)
{
	if (_wstrComponentName.length() < _iNumberWantToExtract) { return _wstrComponentName; }
	return _wstrComponentName.substr(0, _wstrComponentName.length() - _iNumberWantToExtract);
}
