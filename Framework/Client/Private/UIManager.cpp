#include "pch.h"
#include "UIManager.h"

#include "Panel.h"

#include "RectComponent.h"
#include "ButtonComponent.h"
#include "SliderComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"
#include "ProgressBarComponent.h"
#include "AnimationImageComponent.h"

#include "EngineModule.h"
#include "SoundManager.h"
#include "InputManager.h"
#include "TimerManager.h"
#include "Date.h"

#include "World.h"
#include "Level.h"

// system
#include "GameManager.h"
#include "InventorySystem.h"
#include "PalBoxSystem.h"
#include "ArchitectureSystem.h"
#include "PalInteractionSystem.h"
#include "ChestBoxSystem.h"

#include "Player.h"
#include "ClientLoop.h"

#include "DamageFontObject.h"

IMPLEMENT_SINGLETON(CUIManager)

HRESULT Client::CUIManager::Initialize()
{
	// hud load
	LoadPanel(L"HUD");
	LoadPanel(L"CatchBeforeUI");
	LoadPanel(L"TestUI");
	LoadPanel(L"LogoUI"); 
	LoadPanel(L"LoadingUI");
	LoadPanel(L"WorkBenchUI");
	LoadPanel(L"CharacterCreationUI");	
	LoadPanel(L"StaminaUI");

	// �̵��� container system
	LoadPanel(L"InventoryPanel");
	LoadPanel(L"InventoryItemInfo");
	LoadPanel(L"InventoryItemInfoFood");
	LoadPanel(L"InventoryFeedWho");
	LoadPanel(L"PalboxPanel");
	LoadPanel(L"WoodenChestPanel");
	LoadPanel(L"GetItemPanel");

	// �̵��� minigame
	LoadPanel(L"CollectingCoinPanel");


	// minigame bowling
	LoadPanel(L"BowlingUI");
	LoadPanel(L"BowlingPowerUI");
	LoadPanel(L"BowlingAngleUI");
	LoadPanel(L"BowlingPositionUI");
	LoadPanel(L"BowlingGameStartUI");

	LoadPanel(L"PalInteractionUI");
	LoadPanel(L"RidingUI");
	LoadPanel(L"MapNameUI");

	// ��ȣ�ۿ�
	LoadPanel(L"InteractionUI");
	m_spInteractionPanel = FindParentsPanel(L"InteractionUI");

	// ���� hp
	LoadPanel(L"BossHPUI");
	LoadPanel(L"BossDieEventUI");

	//
	LoadPanel(L"LockOnLineUI");
	LoadPanel(L"LockOnUI");
	m_spLockOnUI = FindParentsPanel(L"LockOnUI");

	LoadPanel(L"SlaughterUI");
	m_spSlaughterUI = FindParentsPanel(L"SlaughterUI");

	LoadPanel(L"HungerUI");
	m_spHungerUI = FindParentsPanel(L"HungerUI");

	// hud find
	InitHUDInfo();
	InitBossInfo();

	// load + find
 	RecycleUIs(L"WorkArchitectureUI", L"WorkArchitectureUI", 10, m_listProductionArchitectureUI, m_iWorkArchitectureCount);
	// ���� ������ ���۽� ���� ���� �г��� ����ϴ�.
	//SetWorkUI(L"WorkUI", L"WorkUI", 10);
	RecycleUIs(L"WorkUI", L"WorkUI", 10, m_listWorkUI, m_iWorkCount);
	//SetCaptureUI(L"CatchAfterUI", L"CatchAfterUI", 10);
	RecycleUIs(L"CatchAfterUI", L"CatchAfterUI", 10, m_listCaptureUI, m_iCaptureCount);
	RecycleUIs(L"PalHPWorldUI", L"PalHPWorldUI", 10, m_listPalHPUI, m_iPalHPCount);
	//SetPalHPUI(L"PalHPWorldUI", L"PalHPWorldUI", 10);
	RecycleUIs(L"CatchSuccessUI", L"CatchSuccessUI", 10, m_listCaptureSuccessUI, m_iCaptureSuccessCount);

	RecycleUIs(L"PalWorkWorldUI", L"PalWorkWorldUI", 10, m_listPalWorkUI, m_iPalWorkCount);
	// damage font
	RecycleDamageFonts(80);

	return S_OK;
}

HRESULT Client::CUIManager::BeginPlay()
{
	m_wpWorld = CWorld::GetInstance();

	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	if ((int32)ELevelType::ISLAND == m_wpWorld.lock()->GetPostFrontLevel()->GetLevelType())
	{
		m_spParentsHUDPanel->SetActive(true);

		m_spStaminaPanel = FindParentsPanel(L"StaminaUI");
		m_spStaminaPanel->SetActive(true);
	}
	return S_OK;
}

int32 Client::CUIManager::Tick(float _fDeltaSeconds)
{
 	shared_ptr<CGameManager> spGameManager = CGameManager::GetInstance();

	if (nullptr != m_spTopPanel)
	{
		if ((int32)ELevelType::ISLAND == CWorld::GetInstance()->GetFrontLevel()->GetLevelType())
		{
			if (spGameManager->GetArchitectureSystem()->GetArchitecturePanel() == m_spTopPanel
				|| spGameManager->GetPalInteractionSystem()->GetPalInteractionPanel() == m_spTopPanel)
			{
				CircleUIMouseHover();
				CircleUIMouseClick();
			}
			else if (spGameManager->GetInventorySystem()->GetContainerParentPanel() == m_spTopPanel ||
					 spGameManager->GetPalBoxSystem()->GetContainerParentPanel() == m_spTopPanel ||
					 spGameManager->GetChestBoxSystem()->GetContainerParentPanel() == m_spTopPanel)
			{
				ContainerUIMouseLeftDown();
				ContainerUIMouseRightDown();
				ContainerUIMouseHover ();
			}
			else
			{
				UIMouseDown();
				UIMouseHover();
				UIMousePress();
				UIMouseUp();
			}
		}
		else
		{
			UIMouseDown();
			UIMouseHover();
			UIMousePress();
			UIMouseUp();
		}
	}

	if ((int32)ELevelType::ISLAND == m_wpWorld.lock()->GetFrontLevel()->GetLevelType())
	{
		if (m_spParentsHUDPanel)
		{
			shared_ptr<CPlayer> spPlayer = CGameManager::GetInstance()->GetPlayer();

/*			m_spLockOnUI->SetActive(true);
			m_spLockOnUI->SetPivotWorldPoint(spPlayer->GetPosition().value());
	*/		m_spStaminaPanel->SetPivotWorldPoint(Vector3(spPlayer->GetPosition().value().x, spPlayer->GetPosition().value().y + 1.2f, spPlayer->GetPosition().value().z));

			// �ð� ���� ���¸� �����մϴ�.
			UpdateTimeProgress();
		}
	}

	return 0;
} 

void Client::CUIManager::Release()
{
	if (m_spTopPanel)
	{
		m_spTopPanel = nullptr;
	}

	m_spInteractionPanel = nullptr;

	m_spLockOnUI = nullptr;

	m_spHungerUI = nullptr;

	{
		m_spBossHPInfo->spBossPanel = nullptr;
		m_spBossHPInfo->spBossHPProgress = nullptr;
		m_spBossHPInfo->spCurHPText = nullptr;
		// �� �־��� �� /�� �־��־���մϴ�.
		m_spBossHPInfo->SPMaxHPText = nullptr;
		m_spBossHPInfo->spNameImage = nullptr;

		m_spBossHPInfo = nullptr;
	}

	{
		m_tHUDHPInfo.spChildHPPanel = nullptr;
		m_tHUDHPInfo.spShieldProgressComponent = nullptr;
		m_tHUDHPInfo.spBackShieldImageComponent = nullptr;
		m_tHUDHPInfo.spHPProgressComponent = nullptr;
		m_tHUDHPInfo.spHungerProgressComponent = nullptr;

		m_tHUDHPInfo.spTimezoneImageComponent = nullptr;
		m_tHUDHPInfo.spTimeCircleProgressComponent = nullptr;

		m_tHUDHPInfo.spMaxHPTextComponent = nullptr;
		m_tHUDHPInfo.spCurHPTextComponent = nullptr;
	}

	{
		m_tHUDTutorialInfo.spChildTutorialPanel = nullptr;
		m_tHUDTutorialInfo.spTutorialTextComponent0 = nullptr;
		m_tHUDTutorialInfo.spTutorialTextComponent1 = nullptr;
		m_tHUDTutorialInfo.spTutorialTextComponent2 = nullptr;

		m_tHUDTutorialInfo.spTutorialCheckBoxComponent0 = nullptr;
		m_tHUDTutorialInfo.spTutorialCheckBoxComponent1 = nullptr;
		m_tHUDTutorialInfo.spTutorialCheckBoxComponent2 = nullptr;
	}

	{
		m_tHUDWeaponInfo.spChildWeaponPanel = nullptr;
		m_tHUDWeaponInfo.spPalSphereNumComponent = nullptr;
		m_tHUDWeaponInfo.spWeaponNameComponent = nullptr;
		m_tHUDWeaponInfo.spWeaponCurNumComponent = nullptr;
		m_tHUDWeaponInfo.spWeaponOnceNumComponent = nullptr;

		m_tHUDWeaponInfo.spWeaponImageComponent = nullptr;
	}

	{
		m_tHUDDirectionInfo.spChildDirectionPanel = nullptr;
		m_tHUDDirectionInfo.spDirectionImageComponent = nullptr;
	}

	{
		m_tHUDPalInfo.spChildPalPanel = nullptr;

		// ���� ���Ժ��� 0, 1, 2 ����
		m_tHUDPalInfo.spPalSlotComponent0 = nullptr;
		m_tHUDPalInfo.spPalSlotComponent1 = nullptr;
		m_tHUDPalInfo.spPalSlotComponent2 = nullptr;
	}

	m_spHUDWeaponInfo = nullptr;
	m_spParentsHUDPanel = nullptr;
	m_spQuestInfo = nullptr;


	for (auto& pair : m_mapParentPanels)
	{
		pair.second->Release();
	}
	m_mapParentPanels.clear();

	for (auto& iter : m_listProductionArchitectureUI)
	{
		iter->Release();
	}
	m_listProductionArchitectureUI.clear();

	for (auto& iter : m_listUsingProductionArchitectureUI)
	{
		iter->Release();
		//iter = nullptr;
	}
	m_listUsingProductionArchitectureUI.clear();

	for (auto& iter : m_listWorkUI)
	{
		iter->Release();
	}
	m_listWorkUI.clear();

	for (auto& iter : m_listUsingWorkUI)
	{
		iter->Release();
	}
	m_listUsingWorkUI.clear();

	for (auto& iter : m_listCaptureUI)
	{
		iter->Release();
	}
	m_listCaptureUI.clear();

	for (auto& iter : m_listUsingCaptureUI)
	{
		iter->Release();
	}
	m_listUsingCaptureUI.clear();

	for (auto& iter : m_listCaptureSuccessUI)
	{
		iter->Release();
	}
	m_listCaptureSuccessUI.clear();

	for (auto& iter : m_listUsingCaptureSuccessUI)
	{
		iter->Release();
	}
	m_listUsingCaptureSuccessUI.clear();

	for (auto& iter : m_listPalWorkUI)
	{
		iter->Release();
	}
	m_listPalWorkUI.clear();

	for (auto& iter : m_listUsingPalWorkUI)
	{
		iter->Release();
	}
	m_listUsingPalWorkUI.clear();

	// �̱��� �Ŵ����� �޸𸮿��� �����մϴ�.
	m_spInstance = nullptr;
}

void Client::CUIManager::AddParentPanel(const std::wstring& _wstrParentName, shared_ptr<CPanel>& _spParentPanel)
{
	if (m_mapParentPanels.contains(_wstrParentName)) { return; }

	_spParentPanel->SetGameObjectName(_wstrParentName);

	m_mapParentPanels.emplace(_wstrParentName, _spParentPanel);
}

std::shared_ptr<CPanel> Client::CUIManager::FindChildPanel(const std::wstring& _wstrParentPanelName, const std::wstring& _wstrChildPanelName)
{
	shared_ptr<CPanel> spParentPanel = FindParentsPanel(_wstrParentPanelName);

	shared_ptr<CPanel> spChildPanel = static_pointer_cast<CPanel>(spParentPanel->FindChildGameObject(_wstrChildPanelName));

	return spChildPanel;
}

void Client::CUIManager::UIMouseDown()
{
	// �� ��� Ŭ���� �� �ִ� �г��� �ϳ������ϱ� ������ ����â�� �г��� Ȯ���ϰ� ������ �˴ϴ�.
	if (CEngineModule::GetInstance()->GetInputManager()->IsMouseDown(EMouseActionType::LEFT_BUTTON))
	{
		for (auto& pair : m_spTopPanel->GetMapComponents())
		{
			if (EComponentType::RECT != pair.second->GetComponentType()) { continue; }
			if (EUIType::BUTTON == static_pointer_cast<CRectComponent>(pair.second)->GetUIType() ||
				EUIType::SLIDER == static_pointer_cast<CRectComponent>(pair.second)->GetUIType())
			{
				// �� �� ���콺 ��ġ�� ��Ʈ�� ������ ��ġ�ϸ鼭 �̹��� �� ����Ʈ�� �ƴ� ������Ʈ�� �˻��մϴ�.
				if (static_pointer_cast<CRectComponent>(pair.second)->IsInMouse() && true == pair.second->IsActive())
				{
					m_wpSoundManager.lock()->PlaySoundEx("NormalClickSound");
					static_pointer_cast<CRectComponent>(pair.second)->SetFocusedControl(true);
					m_wpFocusedControlComponent = static_pointer_cast<CRectComponent>(pair.second);
				}
			}
			else if (EUIType::STATIC_IMAGE == static_pointer_cast<CRectComponent>(pair.second)->GetUIType())
			{
				// �� �� ���콺 ��ġ�� ��Ʈ�� ������ ��ġ�ϸ鼭 �̹��� �� ����Ʈ�� �ƴ� ������Ʈ�� �˻��մϴ�.
				if (static_pointer_cast<CRectComponent>(pair.second)->IsInMouse() &&
					true == static_pointer_cast<CRectComponent>(pair.second)->IsInteraction() && true == pair.second->IsActive())
				{
					m_wpSoundManager.lock()->PlaySoundEx("NormalClickSound");
					static_pointer_cast<CRectComponent>(pair.second)->SetFocusedControl(true);
					m_wpFocusedControlComponent = static_pointer_cast<CRectComponent>(pair.second);

					m_wpClickImageComponent = static_pointer_cast<CRectComponent>(pair.second);
				}
			}
			else { continue; }
		}

		for (auto& iter : m_spTopPanel->GetVecChildGameObjects())
		{
			for (auto& pair : static_pointer_cast<CPanel>(iter)->GetMapComponents())
			{
				if (EComponentType::RECT != pair.second->GetComponentType()) { continue; }
				if (EUIType::BUTTON == static_pointer_cast<CRectComponent>(pair.second)->GetUIType() ||
					EUIType::SLIDER == static_pointer_cast<CRectComponent>(pair.second)->GetUIType())
				{
					// �� �� ���콺 ��ġ�� ��Ʈ�� ������ ��ġ�ϸ鼭 �̹��� �� ����Ʈ�� �ƴ� ������Ʈ�� �˻��մϴ�.
					if (static_pointer_cast<CRectComponent>(pair.second)->IsInMouse() && true == pair.second->IsActive())
					{
						m_wpSoundManager.lock()->PlaySoundEx("NormalClickSound");
						static_pointer_cast<CRectComponent>(pair.second)->SetFocusedControl(true);
						m_wpFocusedControlComponent = static_pointer_cast<CRectComponent>(pair.second);
					}
				}
				else if (EUIType::STATIC_IMAGE == static_pointer_cast<CRectComponent>(pair.second)->GetUIType())
				{
					// �� �� ���콺 ��ġ�� ��Ʈ�� ������ ��ġ�ϸ鼭 �̹��� �� ����Ʈ�� �ƴ� ������Ʈ�� �˻��մϴ�.
					if (static_pointer_cast<CRectComponent>(pair.second)->IsInMouse() &&
						true == static_pointer_cast<CRectComponent>(pair.second)->IsInteraction() && true == pair.second->IsActive())
					{
						m_wpSoundManager.lock()->PlaySoundEx("NormalClickSound");
						static_pointer_cast<CRectComponent>(pair.second)->SetFocusedControl(true);
						m_wpFocusedControlComponent = static_pointer_cast<CRectComponent>(pair.second);

						m_wpClickImageComponent = static_pointer_cast<CRectComponent>(pair.second);
					}
				}
				else { continue; }
			}
		}
	}
}

void Client::CUIManager::UIMousePress()
{
	// ��ư�� ��� ������ ����� ���µǾ���ϰ� �����̴��� ���� ��� ���ӵǾ���մϴ�.
	// �̹���(�κ��丮 ������)�� ��� ���콺�� ����ٴϰ� �˴ϴ�.
	if (nullptr != m_wpFocusedControlComponent.lock())
	{
		if (EUIType::SLIDER == m_wpFocusedControlComponent.lock()->GetUIType() && true == m_wpFocusedControlComponent.lock()->IsActive())
		{
			static_pointer_cast<CSliderComponent>(m_wpFocusedControlComponent.lock())->IsDragged();
		}
	}
}

void Client::CUIManager::UIMouseUp()
{
	// ���콺�� ������ �� ��Ŀ�� ��Ʈ���� �ʱ�ȭ���ݴϴ�.
	if (nullptr != m_wpFocusedControlComponent.lock())
	{
		if (CEngineModule::GetInstance()->GetInputManager()->IsMouseUp(EMouseActionType::LEFT_BUTTON))
		{
			if (EUIType::BUTTON == m_wpFocusedControlComponent.lock()->GetUIType())
			{
				//dynamic_pointer_cast<CButtonComponent>(m_wpFocusedControlComponent.lock())->AddButtonClickEvent([this]() { this->UIMouseClick(); });
				static_pointer_cast<CButtonComponent>(m_wpFocusedControlComponent.lock())->IsClicked();
			}

			m_wpFocusedControlComponent.lock()->SetMouseReset(false);
			m_wpFocusedControlComponent.lock()->SetFocusedControl(false);
			m_wpFocusedControlComponent.reset();
		}
	}
}

void Client::CUIManager::UIMouseHover()
{
	bool bHoverImage = false;
	// ������ ������Ʈ�� �ִ� ����Դϴ�.
	if (nullptr != m_wpFocusedControlComponent.lock())
	{
		if (EUIType::BUTTON == m_wpFocusedControlComponent.lock()->GetUIType())
		{
			if (static_pointer_cast<CButtonComponent>(m_wpFocusedControlComponent.lock())->IsHoverd())
			{
				if (m_wpPreHoverComponent.lock() != m_wpFocusedControlComponent.lock())
				{
					m_wpSoundManager.lock()->PlaySoundEx("HoverSound");
					m_wpPreHoverComponent = m_wpFocusedControlComponent;
				}
			}
			else if (false == static_pointer_cast<CButtonComponent>(m_wpFocusedControlComponent.lock())->IsHoverd())
			{
				if (m_wpPreHoverComponent.lock() == m_wpFocusedControlComponent.lock())
				{
					m_wpPreHoverComponent.reset();
				}
			}
		}
	}
	// ���� ������ ������Ʈ�� ���� ����Դϴ�.
	else
	{
		for (auto& pair : m_spTopPanel->GetMapComponents())
		{
			if (EComponentType::RECT != pair.second->GetComponentType()) { continue; }
			if (EUIType::BUTTON == static_pointer_cast<CRectComponent>(pair.second)->GetUIType())
			{
				if (static_pointer_cast<CButtonComponent>(pair.second)->IsHoverd())
				{
					if (m_wpPreHoverComponent.lock() != pair.second)
					{
						m_wpSoundManager.lock()->PlaySoundEx("HoverSound");
						m_wpPreHoverComponent = static_pointer_cast<CRectComponent>(pair.second);
					}
				}
				else if (false == static_pointer_cast<CButtonComponent>(pair.second)->IsHoverd())
				{
					if (m_wpPreHoverComponent.lock() == pair.second)
					{
						m_wpPreHoverComponent.reset();
					}
				}
			}
			else if (EUIType::STATIC_IMAGE == static_pointer_cast<CRectComponent>(pair.second)->GetUIType())
			{
				if (static_pointer_cast<CRectComponent>(pair.second)->IsInMouse() && true == static_pointer_cast<CRectComponent>(pair.second)->GetInteraction() && true == pair.second->IsActive())
				{
					m_wpHoverImageComponent = static_pointer_cast<CRectComponent>(pair.second);
					bHoverImage = true;
				}
				//else if(false == dynamic_pointer_cast<CRectComponent>(pair.second)->IsInMouse() && true == dynamic_pointer_cast<CRectComponent>(pair.second)->GetInteraction())
				//{
				//	m_wpHoverImageComponent.reset();
				//	//m_wpHoverImageComponent.lock()->SetMouseReset(false);
				//}
			}
		}

		for (auto& iter : m_spTopPanel->GetVecChildGameObjects())
		{
			for (auto& pair : static_pointer_cast<CPanel>(iter)->GetMapComponents())
			{
				if (EComponentType::RECT != pair.second->GetComponentType()) { continue; }
				if (EUIType::BUTTON == static_pointer_cast<CRectComponent>(pair.second)->GetUIType())
				{
					if (static_pointer_cast<CButtonComponent>(pair.second)->IsHoverd())
					{
						if (m_wpPreHoverComponent.lock() != pair.second)
						{
							m_wpSoundManager.lock()->PlaySoundEx("HoverSound");
							m_wpPreHoverComponent = static_pointer_cast<CRectComponent>(pair.second);
						}
					}
					else if(false == static_pointer_cast<CButtonComponent>(pair.second)->IsHoverd())
					{
						if (m_wpPreHoverComponent.lock() == pair.second)
						{
							m_wpPreHoverComponent.reset();
						}
					}
				}
				else if (EUIType::STATIC_IMAGE == static_pointer_cast<CRectComponent>(pair.second)->GetUIType())
				{
					if (static_pointer_cast<CRectComponent>(pair.second)->IsInMouse() && true == static_pointer_cast<CRectComponent>(pair.second)->GetInteraction() && true == pair.second->IsActive())
					{
						m_wpHoverImageComponent = static_pointer_cast<CRectComponent>(pair.second);
						bHoverImage = true;
					}
					//else if (false == dynamic_pointer_cast<CRectComponent>(pair.second)->IsInMouse() && true == dynamic_pointer_cast<CRectComponent>(pair.second)->GetInteraction())
					//{
					//	m_wpHoverImageComponent.reset();
					//	m_wpHoverImageComponent.lock()->SetMouseReset(false);
					//}
				}
			}
		}
		
		if (false == bHoverImage)
		{
			m_wpHoverImageComponent.reset();
		}
	}
}

void Client::CUIManager::CircleUIMouseHover()
{
	Vector2 vUIScreenPosition = m_spTopPanel->GetPivotCenterPoint();

	std::pair<int32, int32> prMousePosition = CEngineModule::GetInstance()->GetInputManager()->GetCursorScreenPosition();

	Vector2 vMousePosition((float)prMousePosition.first, (float)prMousePosition.second);

	float fDistance = Vector2::Distance(vUIScreenPosition, vMousePosition);

	if (fDistance < 75.0f)
	{
		m_fCircleDegreeAngle = -1.0f;
		return;
	}

	float fRadianAngle = atan2f((prMousePosition.second - vUIScreenPosition.y), (prMousePosition.first - vUIScreenPosition.x));

	float fDegreeAngle = XMConvertToDegrees(fRadianAngle);

	{
		fDegreeAngle = fDegreeAngle + 180.0f;

		if (fDegreeAngle >= 0.0f && fDegreeAngle < 90.0f)
		{
			fDegreeAngle = fDegreeAngle + 270.f;
		}
		else if (fDegreeAngle >= 90.0f && fDegreeAngle <= 360.f)
		{
			fDegreeAngle = fDegreeAngle - 90.f;
		}
	}

	m_fCircleDegreeAngle = fDegreeAngle;
}

void Client::CUIManager::CircleUIMouseClick()
{
	if (CEngineModule::GetInstance()->GetInputManager()->IsMouseDown(EMouseActionType::LEFT_BUTTON))
	{

	}
}

void Client::CUIManager::LoadPanel(const std::wstring& _wstrLoadFileName)
{
	shared_ptr<CPanel> spPanel = CPanel::CreateFromJson(_wstrLoadFileName);
	if (nullptr == spPanel)
	{
		MESSAGE_BOX(L"There are no panels on the list");
		return;
	}
	
	// �г��� ���� ������ �־��ݴϴ�.
	CWorld::GetInstance()->GetPersistentLevel()->AddGameObject(spPanel);
	
	// �г��� ��Ȱ��ȭ�մϴ�.
	spPanel->SetActive(false);
	AddParentPanel(spPanel->GetGameObjectName(), spPanel);
}

shared_ptr<CPanel> Client::CUIManager::LoadPanel(const std::wstring& _wstrLoadFileName, const std::wstring& _wstrMapKeyName)
{
	shared_ptr<CPanel> spPanel = CPanel::CreateFromJson(_wstrLoadFileName);
	if (nullptr == spPanel)
	{
		MESSAGE_BOX(L"There are no panels on the list");
		return shared_ptr<CPanel>();
	}

	// �г��� ���� ������ �־��ݴϴ�.
	CWorld::GetInstance()->GetBackLevel()->AddGameObject(spPanel);

	// �г��� ��Ȱ��ȭ�մϴ�.
	spPanel->SetActive(false);
	return spPanel;
}

shared_ptr<CDamageFontObject> Client::CUIManager::LoadDamageFont(const std::wstring& _wstrLoadFileName, const std::wstring& _wstrMapKeyName)
{
	shared_ptr<CDamageFontObject> spDamageFont = CDamageFontObject::CreateFromJson(_wstrLoadFileName);
	if (nullptr == spDamageFont)
	{
		MESSAGE_BOX(L"There are no panels on the list");
		return shared_ptr<CDamageFontObject>();
	}

	// �г��� ���� ������ �־��ݴϴ�.
	CWorld::GetInstance()->GetBackLevel()->AddGameObject(spDamageFont);

	// �г��� ��Ȱ��ȭ�մϴ�.
	spDamageFont->SetActive(false);
	return spDamageFont;
}

bool Client::CUIManager::MinusHP(shared_ptr<CProgressBarComponent> _spHPComponent, float _fPreHP, float _fCurHP, float _fMaxHP, float* _fHPLerpTime, float fDeltaSeconds)
{
	if (EUIType::LINEAR_INTERPOLATION_PROGRESSBAR == _spHPComponent->GetUIType())
	{
		float fOtherRatio = (_fMaxHP - _fCurHP) / _fMaxHP;
		_spHPComponent->SetOtherRatio(1.0f - fOtherRatio);
	}

	*_fHPLerpTime += fDeltaSeconds;
	float fRatio = lerp(_fPreHP, _fCurHP, *_fHPLerpTime);
	fRatio /= _fMaxHP;
	_spHPComponent->SetRatio(fRatio);

	if (*_fHPLerpTime >= 1.0f)
	{
		*_fHPLerpTime = 0.0f;
		return true;
	}
	
	return false;
}

bool Client::CUIManager::PlusHP(shared_ptr<CProgressBarComponent> _spHPComponent, shared_ptr<CTextComponent> _spCurHPTextComponent, float _fPreHP, float _fCurHP, float _fMaxHP, float* _fHPLerpTime, float fDeltaSeconds)
{
	if (EUIType::LINEAR_INTERPOLATION_PROGRESSBAR == _spHPComponent->GetUIType())
	{
		float fOtherRatio = (_fMaxHP - _fCurHP) / _fMaxHP;
		_spHPComponent->SetOtherRatio(1.0f - fOtherRatio);
	}

	*_fHPLerpTime += fDeltaSeconds;
	float fRatio = lerp(_fPreHP, _fCurHP, *_fHPLerpTime);
	_spCurHPTextComponent->SetText(to_wstring(static_cast<int32>(fRatio)));
	fRatio /= _fMaxHP;
	_spHPComponent->SetRatio(fRatio);

	if (*_fHPLerpTime >= 1.0f)
	{
		*_fHPLerpTime = 0.0f;
		return true;
	}

	return false;
}

bool Client::CUIManager::MinusStamina(shared_ptr<CProgressBarComponent> _spStaminaComponent, float _fPreStamina, float _fCurStamina, float _fMaxStamina, float* _fStaminaLerpTime, float _fDeltaSeconds)
{
	float fMax = 0.83f;
	float fMin = 0.66f;

	if (EUIType::CIRCLE_INTERPOLATION_PROGRESSBAR == _spStaminaComponent->GetUIType())
	{
		float fOtherRatio = (_fMaxStamina - _fCurStamina) / _fMaxStamina;
		fOtherRatio = 1.0f - fOtherRatio;
		float fNormalizeOtherRatio = (fOtherRatio * (fMax - fMin)) + fMin;
		_spStaminaComponent->SetOtherRatio(fNormalizeOtherRatio);
	}

	*_fStaminaLerpTime += _fDeltaSeconds * 1.8f;
	float fRatio = lerp(_fPreStamina, _fCurStamina, *_fStaminaLerpTime);
	fRatio /= _fMaxStamina;

	if (fRatio <= 0.35f)
	{
		_spStaminaComponent->SetColor(Vector4(1.0f, 0.48f, 0.0f, 1.0f));
	}
	else
	{
		_spStaminaComponent->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	float fNormalizeRatio = (fRatio * (fMax - fMin)) + fMin;
	_spStaminaComponent->SetRatio(fNormalizeRatio);

	if (*_fStaminaLerpTime >= 1.0f)
	{
		*_fStaminaLerpTime = 0.0f;
		return true;
	}

	return false;
}

bool Client::CUIManager::PlusStamina(shared_ptr<CProgressBarComponent> _spStaminaComponent, float _fPreStamina, float _fCurStamina, float _fMaxStamina, float* _fStaminaLerpTime, float _fDeltaSeconds)
{
	_spStaminaComponent->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	float fRatio = (_fMaxStamina - _fCurStamina) / _fMaxStamina;
	fRatio = 1.0f - fRatio;
	float fNormalizeRatio = (fRatio * (0.85f - 0.64f)) + 0.64f;

	_spStaminaComponent->SetOtherRatio(fNormalizeRatio);
	_spStaminaComponent->SetRatio(fNormalizeRatio);

	return true;
}

bool Client::CUIManager::MinusShield(shared_ptr<CProgressBarComponent> _spShildComponent, int32 _iPreCount, int32 _iCurCount, float* _fShildLerpTime, float _fDeltaSeconds)
{
	float fOtherRatio;
	if (EUIType::LINEAR_INTERPOLATION_PROGRESSBAR == _spShildComponent->GetUIType())
	{
		fOtherRatio = (5.f - static_cast<float>(_iCurCount)) / 5.f;
		_spShildComponent->SetOtherRatio(1.0f - fOtherRatio);
	}

	*_fShildLerpTime += _fDeltaSeconds;
	float fRatio = lerp(static_cast<float>(_iPreCount), static_cast<float>(_iCurCount), *_fShildLerpTime);
	fRatio /= 5.f;
	_spShildComponent->SetRatio(fRatio);

	if (*_fShildLerpTime >= 1.0f)
	{
		*_fShildLerpTime = 0.0f;
		return true;
	}

	return false;
}

bool Client::CUIManager::MinusBossHP(shared_ptr<CProgressBarComponent> _spHPComponent, shared_ptr<CTextComponent> _spCurHPTextComponent, float _fPreHP, float _fCurHP, float _fMaxHP, float* _fHPLerpTime, float fDeltaSeconds)
{
	if (EUIType::LINEAR_INTERPOLATION_PROGRESSBAR == _spHPComponent->GetUIType())
	{
		float fOtherRatio = (_fMaxHP - _fCurHP) / _fMaxHP;
		_spHPComponent->SetOtherRatio(1.0f - fOtherRatio);
	}

	*_fHPLerpTime += fDeltaSeconds;
	float fRatio = lerp(_fPreHP, _fCurHP, *_fHPLerpTime);
	_spCurHPTextComponent->SetText(to_wstring(static_cast<int32>(fRatio)));
	fRatio /= _fMaxHP;
	_spHPComponent->SetRatio(fRatio);

	if (*_fHPLerpTime >= 1.0f)
	{
		*_fHPLerpTime = 0.0f;
		return true;
	}

	return false;
}

void Client::CUIManager::ResetTopPanel()
{
	m_spTopPanel = nullptr;
}

void Client::CUIManager::ContainerUIMouseLeftDown()
{
	// �� ��� Ŭ���� �� �ִ� �г��� �ϳ������ϱ� ������ ����â�� �г��� Ȯ���ϰ� ������ �˴ϴ�.
	if (CEngineModule::GetInstance()->GetInputManager()->IsMouseUp(EMouseActionType::LEFT_BUTTON))
	{
		for (auto& iter : m_spTopPanel->GetVecChildGameObjects())
		{
			// childPanel�� ������ �ִ� Components�� ��Ƶ� map�����̳��Դϴ�.
			const std::map<std::wstring, std::shared_ptr<CComponent>>& mapChildPanelComponents = dynamic_pointer_cast<CPanel>(iter)->GetMapComponents();

			// map�����̳ʸ� ��ȸ�մϴ�.
			for (auto& pair : mapChildPanelComponents)
			{
				weak_ptr<CComponent> wpChildPanelComponent = pair.second;

				// component Ÿ���� rect�϶��� ���� �����մϴ�.
				if (EComponentType::RECT != wpChildPanelComponent.lock()->GetComponentType()) { continue; }
				else
				{
					weak_ptr<CRectComponent> wpRectComponent = dynamic_pointer_cast<CRectComponent>(wpChildPanelComponent.lock());
					EUIType eUIType = wpRectComponent.lock()->GetUIType();

					// rect component�� uiŸ�Կ� ���� �ٸ� ������ �����մϴ�.
					switch (eUIType)
					{
					case EUIType::STATIC_IMAGE:
					{
						// �� �� ���콺 ��ġ�� ��Ʈ�� ������ ��ġ�ϸ鼭 �̹��� �� ����Ʈ�� �ƴ� ������Ʈ�� �˻��մϴ�.
						if (wpRectComponent.lock()->IsInMouse() && wpRectComponent.lock()->IsInteraction())
						{
							m_wpSoundManager.lock()->PlaySoundEx("InvenClickSound");
							m_wpFocusedControlComponent = wpRectComponent;
							m_wpFocusedControlComponent.lock()->SetFocusedControl(true);
						}

					} break;

					case EUIType::SLIDER:
					{

					} break;

					case EUIType::BUTTON:
					{

					} break;

					default: break;
					}
				}
			}
		}
	}
}

void Client::CUIManager::ContainerUIMouseRightDown()
{
	// �� ��� Ŭ���� �� �ִ� �г��� �ϳ������ϱ� ������ ����â�� �г��� Ȯ���ϰ� ������ �˴ϴ�.
	if (CEngineModule::GetInstance()->GetInputManager()->IsMouseDown(EMouseActionType::RIGHT_BUTTON))
	{
		for (auto& iter : m_spTopPanel->GetVecChildGameObjects())
		{
			// childPanel�� ������ �ִ� Components�� ��Ƶ� map�����̳��Դϴ�.
			map<std::wstring, std::shared_ptr<CComponent>>  mapChildPanelComponents = dynamic_pointer_cast<CPanel>(iter)->GetMapComponents();

			// map�����̳ʸ� ��ȸ�մϴ�.
			for (auto& pair : mapChildPanelComponents)
			{
				weak_ptr<CComponent> wpChildPanelComponent = pair.second;

				// component Ÿ���� rect�϶��� ���� �����մϴ�.
				if (EComponentType::RECT != wpChildPanelComponent.lock()->GetComponentType()) { continue; }
				else
				{
					weak_ptr<CRectComponent> wpRectComponent = dynamic_pointer_cast<CRectComponent>(wpChildPanelComponent.lock());
					EUIType eUIType = wpRectComponent.lock()->GetUIType();

					// rect component�� uiŸ�Կ� ���� �ٸ� ������ �����մϴ�.
					switch (eUIType)
					{
					case EUIType::STATIC_IMAGE:
					{
						// �� �� ���콺 ��ġ�� ��Ʈ�� ������ ��ġ�ϸ鼭 �̹��� �� ����Ʈ�� �ƴ� ������Ʈ�� �˻��մϴ�.
						if (wpRectComponent.lock()->IsInMouse() && wpRectComponent.lock()->IsInteraction())
						{
							m_wpSoundManager.lock()->PlaySoundEx("InvenClickSound");
							m_wpRightClickFocusedControlComponent= wpRectComponent;
						}

					} break;

					case EUIType::SLIDER:
					{

					} break;

					case EUIType::BUTTON:
					{

					} break;

					default: break;
					}
				}
			}
		}
	}
}

void Client::CUIManager::ContainerUIMouseHover()
{
	bool bIsHovered = false;

	// ���� ������ ������Ʈ�� ���� ����Դϴ�.
	if (m_wpFocusedControlComponent.expired())
	{
		for (auto& iter : m_spTopPanel->GetVecChildGameObjects())
		{
			map<std::wstring, std::shared_ptr<CComponent>>  mapChildPanelComponents = static_pointer_cast<CPanel>(iter)->GetMapComponents();

			for (auto& pair : mapChildPanelComponents)
			{
				weak_ptr<CComponent> wpChildPanelComponent = pair.second;

				if (EComponentType::RECT != wpChildPanelComponent.lock()->GetComponentType()) { continue; }
				else
				{
					weak_ptr<CRectComponent> wpRectComponent = static_pointer_cast<CRectComponent>(wpChildPanelComponent.lock());
					EUIType eUIType = wpRectComponent.lock()->GetUIType();

					switch (eUIType)
					{
					case EUIType::STATIC_IMAGE:
					{
						if (wpRectComponent.lock()->IsInMouse() && wpRectComponent.lock()->GetInteraction())
						{
							m_wpHoverImageComponent = wpRectComponent;
							bIsHovered = true;
						}

						if (m_wpPreHoverComponent.lock() != m_wpHoverImageComponent.lock())
						{
							m_wpSoundManager.lock()->PlaySoundEx("HoverSound");
							m_wpPreHoverComponent = m_wpHoverImageComponent;
						}

					} break;
				
					case EUIType::SLIDER:
					{

					} break;

					case EUIType::BUTTON:
					{
						static_pointer_cast<CButtonComponent>(pair.second)->IsHoverd();
					} break;
					
					default: break;
					}
				}
			}
		}
		
		if (!bIsHovered)
		{
			m_wpPreHoverComponent.reset();
			m_wpHoverImageComponent.reset();
		}
	}
}

shared_ptr<CPanel> Client::CUIManager::GetUsingProductionArchitectureUI()
{
	for (auto iter = m_listProductionArchitectureUI.begin(); iter != m_listProductionArchitectureUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingProductionArchitectureUI.push_back(spPanel);
			spPanel->FindComponent<CTextComponent>(L"EndText")->SetText(L"���� �������");
			spPanel->SetActive(true);

			// ����Ʈ�� �ִ� �г��� �����ݴϴ�.
			iter = m_listProductionArchitectureUI.erase(iter);
			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	RecycleUIs(L"WorkArchitectureUI", L"WorkArchitectureUI", 3, m_listProductionArchitectureUI, m_iWorkArchitectureCount);

	for (auto iter = m_listProductionArchitectureUI.begin(); iter != m_listProductionArchitectureUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingProductionArchitectureUI.push_back(spPanel);
			spPanel->FindComponent<CTextComponent>(L"EndText")->SetText(L"���� �������");
			spPanel->SetActive(true);

			// ����Ʈ�� �ִ� �г��� �����ݴϴ�.
			iter = m_listProductionArchitectureUI.erase(iter);
			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	return shared_ptr<CPanel>();
}

void Client::CUIManager::ReturnProductionArchitectureUI(std::shared_ptr<CPanel> _spPanel)
{
	if (nullptr == _spPanel) { return; }

	for (auto iter = m_listUsingProductionArchitectureUI.begin(); iter != m_listUsingProductionArchitectureUI.end();)
	{
		if (_spPanel == *iter)
		{
			_spPanel->SetActive(false);
			if (0 == _spPanel->GetMapComponents().size()) { return; }
			static_pointer_cast<CPanel>(_spPanel->FindChildGameObject(L"ChildPalUI"))->SetActive(false);
			m_listProductionArchitectureUI.push_back(*iter);
			iter = m_listUsingProductionArchitectureUI.erase(iter);
			break;
		}
		else
		{
			iter++;
		}
	}
}

shared_ptr<CPanel> Client::CUIManager::GetUsingWorkUI()
{
	for (auto iter = m_listWorkUI.begin(); iter != m_listWorkUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingWorkUI.push_back(spPanel);
			spPanel->FindComponent<CProgressBarComponent>(L"CircleProgressBar")->SetRatio(0.0f);
			spPanel->FindComponent<CTextComponent>(L"CurCountText")->SetText(to_wstring(0));
			spPanel->SetActive(true);

			iter = m_listWorkUI.erase(iter);
			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	RecycleUIs(L"WorkUI", L"WorkUI", 3, m_listWorkUI, m_iWorkCount);

	for (auto iter = m_listWorkUI.begin(); iter != m_listWorkUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingWorkUI.push_back(spPanel);
			spPanel->FindComponent<CProgressBarComponent>(L"CircleProgressBar")->SetRatio(0.0f);
			spPanel->FindComponent<CTextComponent>(L"CurCountText")->SetText(to_wstring(0));
			spPanel->SetActive(true);

			iter = m_listWorkUI.erase(iter);
			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	return nullptr;
}

void Client::CUIManager::ReturnWorkUI(const std::shared_ptr<CPanel> _spPanel)
{
	if (nullptr == _spPanel) { return; }

	for (auto iter = m_listUsingWorkUI.begin(); iter != m_listUsingWorkUI.end();)
	{
		if (_spPanel == *iter)
		{
			_spPanel->SetActive(false);
			if (0 == _spPanel->GetMapComponents().size()) { return; }
			static_pointer_cast<CPanel>(_spPanel->FindChildGameObject(L"ChildWorkPalUI"))->SetActive(false);
			_spPanel->FindComponent<CProgressBarComponent>(L"CircleProgressBar")->SetRatio(0.0f);
			_spPanel->FindComponent<CTextComponent>(L"CurCountText")->SetText(to_wstring(0));
			
			m_listWorkUI.push_back(*iter);
			
			iter = m_listUsingWorkUI.erase(iter);
			break;
		}
		else
		{
			iter++;
		}
	}
}

shared_ptr<CPanel> Client::CUIManager::GetUsingCaptureUI()
{
	for (auto iter = m_listCaptureUI.begin(); iter != m_listCaptureUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingCaptureUI.push_back(spPanel);
			spPanel->SetActive(true);

			iter = m_listCaptureUI.erase(iter);

			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	RecycleUIs(L"CatchAfterUI", L"CatchAfterUI", 3, m_listCaptureUI, m_iCaptureCount);

	for (auto iter = m_listCaptureUI.begin(); iter != m_listCaptureUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingCaptureUI.push_back(spPanel);
			spPanel->SetActive(true);

			iter = m_listCaptureUI.erase(iter);

			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	return shared_ptr<CPanel>();
}

void Client::CUIManager::ReturnCaptureUI(const std::shared_ptr<CPanel> _spPanel)
{
	if (nullptr == _spPanel) { return; }

	for (auto iter = m_listUsingCaptureUI.begin(); iter != m_listUsingCaptureUI.end();)
	{
		if (_spPanel == *iter)
		{
			_spPanel->SetActive(false);
			if (0 == _spPanel->GetMapComponents().size()) { return; }
			m_listCaptureUI.push_back(*iter);
			iter = m_listUsingCaptureUI.erase(iter);
			break;
		}
		else
		{
			iter++;
		}
	}
}

shared_ptr<CPanel> Client::CUIManager::GetUsingPalHPUI()
{
	for (auto iter = m_listPalHPUI.begin(); iter != m_listPalHPUI.end();)
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingPalHPUI.push_back(spPanel);
			//spPanel->FindComponent<CImageComponent>(L"NameBackImage")->SetActive(false);
			spPanel->SetActive(true);

			iter = m_listPalHPUI.erase(iter);

			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	RecycleUIs(L"PalHPWorldUI", L"PalHPWorldUI", 3, m_listPalHPUI, m_iPalHPCount);

	for (auto iter = m_listPalHPUI.begin(); iter != m_listPalHPUI.end();)
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingPalHPUI.push_back(spPanel);
			//spPanel->FindComponent<CImageComponent>(L"NameBackImage")->SetActive(false);
			spPanel->SetActive(true);

			iter = m_listPalHPUI.erase(iter);

			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	return nullptr;
}

void Client::CUIManager::ReturnPalHPUI(const std::shared_ptr<CPanel> _spPanel)
{
	if (nullptr == _spPanel) { return; }

	for (auto iter = m_listUsingPalHPUI.begin(); iter != m_listUsingPalHPUI.end();)
	{
		if (_spPanel == *iter)
		{
			_spPanel->SetActive(false);
			if (0 == _spPanel->GetMapComponents().size()) { return; }
			m_listPalHPUI.push_back(*iter);
			iter = m_listUsingPalHPUI.erase(iter);
			break;
		}
		else
		{
			iter++;
		}
	}
}

shared_ptr<CPanel> Client::CUIManager::GetUsingCaptureSuccessUI()
{
	for (auto iter = m_listCaptureSuccessUI.begin(); iter != m_listCaptureSuccessUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingCaptureSuccessUI.push_back(spPanel);
			// �ִϸ��̼��� �������ݴϴ�.
			spPanel->FindComponent<CAnimationImageComponent>(L"CircleAnimation")->ResetFrame();
			spPanel->FindComponent<CAnimationImageComponent>(L"SuccessAnimation0")->ResetFrame();
			spPanel->FindComponent<CAnimationImageComponent>(L"SuccessAnimation1")->ResetFrame();
			spPanel->FindComponent<CAnimationImageComponent>(L"SuccessAnimation2")->ResetFrame();
			spPanel->SetActive(true);

			iter = m_listCaptureSuccessUI.erase(iter);

			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	RecycleUIs(L"CatchSuccessUI", L"CatchSuccessUI", 3, m_listCaptureSuccessUI, m_iCaptureSuccessCount);

	for (auto iter = m_listCaptureSuccessUI.begin(); iter != m_listCaptureSuccessUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingCaptureSuccessUI.push_back(spPanel);
			// �ִϸ��̼��� �������ݴϴ�.
			spPanel->FindComponent<CAnimationImageComponent>(L"CircleAnimation")->ResetFrame();
			spPanel->FindComponent<CAnimationImageComponent>(L"SuccessAnimation0")->ResetFrame();
			spPanel->FindComponent<CAnimationImageComponent>(L"SuccessAnimation1")->ResetFrame();
			spPanel->FindComponent<CAnimationImageComponent>(L"SuccessAnimation2")->ResetFrame();
			spPanel->SetActive(true);

			iter = m_listCaptureSuccessUI.erase(iter);

			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	return shared_ptr<CPanel>();
}

void Client::CUIManager::ReturnCaptureSuccessUI(const std::shared_ptr<CPanel> _spPanel)
{
	if (nullptr == _spPanel) { return; }

	for (auto iter = m_listUsingCaptureSuccessUI.begin(); iter != m_listUsingCaptureSuccessUI.end();)
	{
		if (_spPanel == *iter)
		{
			_spPanel->SetActive(false);
			if (0 == _spPanel->GetMapComponents().size()) { return; }
			m_listCaptureSuccessUI.push_back(*iter);
			iter = m_listUsingCaptureSuccessUI.erase(iter);
			break;
		}
		else
		{
			iter++;
		}
	}
}

shared_ptr<CPanel> Client::CUIManager::GetUsingPalWorkUI()
{
	for (auto iter = m_listPalWorkUI.begin(); iter != m_listPalWorkUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingPalWorkUI.push_back(spPanel);
			spPanel->SetActive(true);
			
			iter = m_listPalWorkUI.erase(iter);
			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	RecycleUIs(L"PalWorkWorldUI", L"PalWorkWorldUI", 3, m_listPalWorkUI, m_iPalWorkCount);

	for (auto iter = m_listPalWorkUI.begin(); iter != m_listPalWorkUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CPanel> spPanel = *iter;
			m_listUsingPalWorkUI.push_back(spPanel);
			spPanel->SetActive(true);

			iter = m_listPalWorkUI.erase(iter);
			return spPanel;
		}
		else
		{
			iter++;
		}
	}

	return shared_ptr<CPanel>();
}

void Client::CUIManager::ReturnPalWorkUI(const std::shared_ptr<CPanel> _spPanel)
{
	if (nullptr == _spPanel) { return; }

	for (auto iter = m_listUsingPalWorkUI.begin(); iter != m_listUsingPalWorkUI.end();)
	{
		if (_spPanel == *iter)
		{
			_spPanel->SetActive(false);
			if (0 == _spPanel->GetMapComponents().size()) { return; }
			m_listPalWorkUI.push_back(*iter);
			iter = m_listUsingPalWorkUI.erase(iter);
			break;
		}
		else
		{
			iter++;
		}
	}
}

shared_ptr<CDamageFontObject> Client::CUIManager::GetUsingDamageFontUI()
{
	for (auto iter = m_listDamageUI.begin(); iter != m_listDamageUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CDamageFontObject> spDamageFont = *iter;
			m_listUsingDamageUI.push_back(spDamageFont);
			spDamageFont->SetActive(true);

			iter = m_listDamageUI.erase(iter);
			return spDamageFont;
		}
		else
		{
			iter++;
		}
	}

	// ������ ��Ʈ�� �����Ұ�� �߰��Ͽ� ������ݴϴ�.
	RecycleDamageFonts(80);

	for (auto iter = m_listDamageUI.begin(); iter != m_listDamageUI.end(); )
	{
		if (false == (*iter)->IsActive())
		{
			std::shared_ptr<CDamageFontObject> spDamageFont = *iter;
			m_listUsingDamageUI.push_back(spDamageFont);
			spDamageFont->SetActive(true);

			iter = m_listDamageUI.erase(iter);
			return spDamageFont;
		}
		else
		{
			iter++;
		}
	}

	return shared_ptr<CDamageFontObject>();
}

void Client::CUIManager::ReturnDamageFont(const std::shared_ptr<CDamageFontObject> _spDamageFont)
{
	if (nullptr == _spDamageFont) { return; }

	for (auto iter = m_listUsingDamageUI.begin(); iter != m_listUsingDamageUI.end();)
	{
		if (_spDamageFont == *iter)
		{
			_spDamageFont->SetActive(false);
			//if (0 == _spDamageFont->GetMapComponents().size()) { return; }
			m_listDamageUI.push_back(*iter);
			iter = m_listUsingDamageUI.erase(iter);
			break;
		}
		else
		{
			iter++;
		}
	}
}

std::wstring Client::CUIManager::FindPalIconTextureName(EMonsterType _eMonsterType)
{
	std::wstring wstrTextureName = L"";

	switch (_eMonsterType)
	{
	case Client::EMonsterType::SHEEP_BALL:
		wstrTextureName = L"T_SheepBall_icon_normal";
		break;
	case Client::EMonsterType::CHICKEN_PAL:
		wstrTextureName = L"T_ChickenPal_icon_normal";
		break;
	case Client::EMonsterType::PINK_CAT:
		wstrTextureName = L"T_PinkCat_icon_normal";
		break;
	case Client::EMonsterType::KITSUNEBI:
		wstrTextureName = L"T_Kitsunebi_icon_normal";
		break;
	case Client::EMonsterType::PENGUIN:
		wstrTextureName = L"T_Penguin_icon_normal";
		break;
	case Client::EMonsterType::CARBUNCLO:
		wstrTextureName = L"T_Carbunclo_icon_normal";
		break;
	//case Client::EMonsterType::WEASEL_DRAGON:
	//	wstrTextureName = L"T_WeaselDragon_icon_normal";
	//	break;
	case Client::EMonsterType::HEDGE_HOG:
		wstrTextureName = L"T_Hedgehog_icon_normal";
		break;
	case Client::EMonsterType::DREAM_DEMON:
		wstrTextureName = L"T_DreamDemon_icon_normal";
		break;
	case Client::EMonsterType::JETDRAGON:
		wstrTextureName = L"T_JetDragon_icon_normal";
		break;
	case Client::EMonsterType::ELEC_PANDA:
		wstrTextureName = L"T_ElecPanda_icon_normal";
		break;
	case Client::EMonsterType::BERRY_GOAT:
		wstrTextureName = L"T_BerryGoat_icon_normal";
		break;
	case Client::EMonsterType::FENGYUNDEEPER:
		wstrTextureName = L"T_FengyunDeeper_icon_normal";
		break;
	case Client::EMonsterType::WOOL_FOX:
		wstrTextureName = L"T_WoolFox_icon_normal";
		break;
	case Client::EMonsterType::BOAR:
		wstrTextureName = L"T_Boar_icon_normal";
		break;
	case Client::EMonsterType::ENUM_END:
		break;
	default:
		break;
	}

	return wstrTextureName;
}

std::wstring Client::CUIManager::FindPalName(EMonsterType _eMonsterType)
{
	std::wstring wstrPalName = L"";

	switch (_eMonsterType)
	{
	case Client::EMonsterType::SHEEP_BALL:	wstrPalName = L"���η�";	break;
	case Client::EMonsterType::CHICKEN_PAL:	wstrPalName = L"������";	break;
	case Client::EMonsterType::PINK_CAT:	wstrPalName = L"��γ�";	break;
	case Client::EMonsterType::KITSUNEBI:	wstrPalName = L"����ȣ";	break;
	case Client::EMonsterType::PENGUIN:		wstrPalName = L"��Ű";		break;
	case Client::EMonsterType::CARBUNCLO:	wstrPalName = L"ť�긮��";	break;
	case Client::EMonsterType::HEDGE_HOG:	wstrPalName = L"���ġ";	break;
	case Client::EMonsterType::DREAM_DEMON: wstrPalName = L"��������"; 	break;
	case Client::EMonsterType::ELEC_PANDA:	wstrPalName = L"�Ϸ��Ǵ�";	break;
	case Client::EMonsterType::JETDRAGON:	wstrPalName = L"��Ʈ����";	break;
	case Client::EMonsterType::WOOL_FOX:	wstrPalName = L"����";		break;
	case Client::EMonsterType::BOAR:		wstrPalName = L"��������";	break;
	case Client::EMonsterType::BERRY_GOAT:	wstrPalName = L"������Ʈ";	break;
	case Client::EMonsterType::FENGYUNDEEPER:	wstrPalName = L"������";	break;
	case Client::EMonsterType::ENUM_END:	break;
	default: break;
	}

	return wstrPalName;
}

std::wstring Client::CUIManager::FindPalElementName(EMonsterElementType _eMonsterElementType)
{
	std::wstring wstrElementName = L"";

	switch (_eMonsterElementType)
	{
	case Client::EMonsterElementType::GRASS:		wstrElementName = L"T_Icon_element_s_04";	break;
	case Client::EMonsterElementType::GROUND:		wstrElementName = L"T_Icon_element_s_07";	break;
	case Client::EMonsterElementType::ELECTRIC:		wstrElementName = L"T_Icon_element_s_03";	break;
	case Client::EMonsterElementType::WATER:		wstrElementName = L"T_Icon_element_s_02";	break;
	case Client::EMonsterElementType::FIRE:			wstrElementName = L"T_Icon_element_s_01";	break;
	case Client::EMonsterElementType::ICE:			wstrElementName = L"T_Icon_element_s_08";	break;
	case Client::EMonsterElementType::DRAGON:		wstrElementName = L"T_Icon_element_s_06";	break;
	case Client::EMonsterElementType::DARK:			wstrElementName = L"T_Icon_element_s_05"; 	break;
	case Client::EMonsterElementType::NEUTRAL:		wstrElementName = L"T_Icon_element_s_00";	break;
	default: break;
	}

	return wstrElementName;
}

std::wstring Client::CUIManager::FindPalSkillElementName(EMonsterElementType _eMonsterElementType)
{
	std::wstring wstrPalSkillElementName = L"";

	switch (_eMonsterElementType)
	{
	case Client::EMonsterElementType::GRASS:		wstrPalSkillElementName = L"T_prt_pal_skill_base_element_04";	break;
	case Client::EMonsterElementType::GROUND:		wstrPalSkillElementName = L"T_prt_pal_skill_base_element_07";	break;
	case Client::EMonsterElementType::ELECTRIC:		wstrPalSkillElementName = L"T_prt_pal_skill_base_element_03";	break;
	case Client::EMonsterElementType::WATER:		wstrPalSkillElementName = L"T_prt_pal_skill_base_element_02";	break;
	case Client::EMonsterElementType::FIRE:			wstrPalSkillElementName = L"T_prt_pal_skill_base_element_01";	break;
	case Client::EMonsterElementType::ICE:			wstrPalSkillElementName = L"T_prt_pal_skill_base_element_08";	break;
	case Client::EMonsterElementType::DRAGON:		wstrPalSkillElementName = L"T_prt_pal_skill_base_element_06";	break;
	case Client::EMonsterElementType::DARK:			wstrPalSkillElementName = L"T_prt_pal_skill_base_element_05"; 	break;
	case Client::EMonsterElementType::NEUTRAL:		wstrPalSkillElementName = L"T_prt_pal_skill_base_element_00";	break;
	default: break;
	}

	return wstrPalSkillElementName;
}

std::wstring Client::CUIManager::FindItemUITextureName(EItemType _eItemType)
{
	std::wstring wstrItemUITextureName = L"";
	switch (_eItemType)
	{
	case Client::EItemType::NONE:					wstrItemUITextureName = L""; break;
	case Client::EItemType::PALDIUM_FRAGMENT:		wstrItemUITextureName = L"T_itemicon_Material_Pal_crystal_S"; break;
	case Client::EItemType::GOLD_COIN:				wstrItemUITextureName = L"T_itemicon_Material_Money"; break;
	case Client::EItemType::STONE:					wstrItemUITextureName = L"T_itemicon_Material_Stone"; break;
	case Client::EItemType::WOOD:					wstrItemUITextureName = L"T_itemicon_Material_Wood"; break;
	case Client::EItemType::LEATHER:				wstrItemUITextureName = L"T_itemicon_Material_Leather"; break;
	case Client::EItemType::COAL:					wstrItemUITextureName = L"T_itemicon_Material_Coal"; break;
	case Client::EItemType::WOOL:					wstrItemUITextureName = L"T_itemicon_Material_Wool"; break;
	case Client::EItemType::IRON:					wstrItemUITextureName = L"T_itemicon_Material_CopperOre"; break;
	case Client::EItemType::CLOTH:					wstrItemUITextureName = L"T_itemicon_Material_Cloth"; break;
	case Client::EItemType::COPPER_INGOT:			wstrItemUITextureName = L"T_itemicon_Material_CopperIngot"; break;
	case Client::EItemType::IRON_INGOT:				wstrItemUITextureName = L"T_itemicon_Material_IronIngot"; break;
	case Client::EItemType::GUNPOWDER:				wstrItemUITextureName = L"T_itemicon_Material_Gunpowder2"; break;
	case Client::EItemType::CARBON:					wstrItemUITextureName = L"T_itemicon_Material_CarbonFiber"; break;
	case Client::EItemType::RIFLE_BULLET:			wstrItemUITextureName = L"T_itemicon_Ammo_RifleBullet"; break;
	case Client::EItemType::Outfit_Cloth001:		wstrItemUITextureName = L"T_itemicon_Armor_ClothArmor"; break;
	case Client::EItemType::Outfit_Hunter001:		wstrItemUITextureName = L"T_itemicon_Armor_FurArmor"; break;
	case Client::EItemType::Outfit_Platinum001_v02: wstrItemUITextureName = L"T_itemicon_Armor_IronArmorHeat"; break;
	case Client::EItemType::HeadEquip016:			wstrItemUITextureName = L"T_itemicon_Armor_CopperHelmet"; break;
	case Client::EItemType::HeadEquip018:			wstrItemUITextureName = L"T_itemicon_Armor_StealHelmet"; break;
	case Client::EItemType::HeadEquip019:			wstrItemUITextureName = L"T_itemicon_Armor_Head013"; break;
	case Client::EItemType::HeadEquip021:			wstrItemUITextureName = L"T_itemicon_Armor_Head016"; break;
	case Client::EItemType::BLUE_SPHERE:			wstrItemUITextureName = L"T_itemicon_PalSphere"; break;
	case Client::EItemType::GREEN_SPHERE:			wstrItemUITextureName = L"T_itemicon_PalSphere_Mega"; break;
	case Client::EItemType::COMMON_SHIELD:			wstrItemUITextureName = L"T_itemicon_Armor_Shield_01"; break;
	case Client::EItemType::GIGA_SHIELD:			wstrItemUITextureName = L"T_itemicon_Armor_Shield_03"; break;
	case Client::EItemType::HAND_HELD_TORCH:		wstrItemUITextureName = L"T_itemicon_Weapon_Torch"; break;
	case Client::EItemType::STONE_SPEAR:			wstrItemUITextureName = L"T_itemicon_Weapon_Spear_Tier_00"; break;
	case Client::EItemType::MEAT_CLEAVER:			wstrItemUITextureName = L"T_itemicon_Weapon_MeatCutterKnife"; break;
	case Client::EItemType::OLD_BOW:				wstrItemUITextureName = L"T_itemicon_Weapon_Bow"; break;
	case Client::EItemType::ROCKET_LAUNCHER:		wstrItemUITextureName = L"T_itemicon_Weapon_Launcher_Default"; break;
	case Client::EItemType::STONE_AXE:				wstrItemUITextureName = L"T_itemicon_Weapon_Axe_Tier_00"; break;
	case Client::EItemType::STONE_PICKAXE:			wstrItemUITextureName = L"T_itemicon_Weapon_Pickaxe_Tier_00"; break;
	case Client::EItemType::ASSAULT_RIFLE:			wstrItemUITextureName = L"T_itemicon_Weapon_AssaultRifle_Default1"; break;
	case Client::EItemType::GLIDER:					wstrItemUITextureName = L"T_itemicon_Glider_Glider_Old"; break;
	case Client::EItemType::ARROW:					wstrItemUITextureName = L"T_itemicon_Ammo_Arrow"; break;
	case Client::EItemType::EGG:					wstrItemUITextureName = L"T_itemicon_Food_Egg"; break;
	case Client::EItemType::SHEEPBALL_MEAT:			wstrItemUITextureName = L"T_itemicon_Food_Meat_SheepBall"; break;
	case Client::EItemType::BOAR_MEAT:				wstrItemUITextureName = L"T_itemicon_Food_Meat_Boar"; break;
	case Client::EItemType::RED_BERRY:				wstrItemUITextureName = L"T_itemicon_Food_Berries"; break;
	case Client::EItemType::BAKED_BERRY:			wstrItemUITextureName = L"T_itemicon_Food_Baked_Berries"; break;
	case Client::EItemType::FRIED_EGG:				wstrItemUITextureName = L"T_itemicon_Food_FriedEggs"; break;
	case Client::EItemType::SHEEPBALL_KEBAB:		wstrItemUITextureName = L"T_itemicon_Food_BakedMeat_SheepBall"; break;
	case Client::EItemType::GRASSMAMMOTH_MEAT:		wstrItemUITextureName = L"T_itemicon_Food_Meat_GrassMammoth"; break;
	case Client::EItemType::SHEEPBALL_ROAST:		wstrItemUITextureName = L"T_itemicon_Food_GrilledSheepHerbs"; break;
	case Client::EItemType::CHICKEN_ROAST:			wstrItemUITextureName = L"T_itemicon_Food_BakedMeat_Boar"; break;
	default: break;
	}

	return wstrItemUITextureName;
}

std::wstring Client::CUIManager::FindPlayerWeaponTextureName(EWeaponType _eWeaponType)
{
	std::wstring wstrWeaponTextureName;




	return std::wstring();
}

std::vector<pair<EItemType, int32>> Client::CUIManager::FindItemDataSheet(EItemType _eItemType)
{
	vector<pair<EItemType, int32>> vecCurIndex;

	switch (_eItemType)
	{
	/*case Client::EItemType::PALDIUM_FRAGMENT:
		break;
	case Client::EItemType::GOLD_COIN:
		break;
	case Client::EItemType::STONE:
		break;
	case Client::EItemType::WOOD:
		break;
	case Client::EItemType::LEATHER:
		break;
	case Client::EItemType::COAL:
		break;
	case Client::EItemType::WOOL:
		break;
	case Client::EItemType::IRON:
		break;
	case Client::EItemType::CLOTH:
		break;*/
	case Client::EItemType::COPPER_INGOT:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecCopperIngotSheet;
		break;
	case Client::EItemType::IRON_INGOT:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecIronIngotSheet;
		break;
	case Client::EItemType::GUNPOWDER:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecGunpowderSheet;
		break;
	case Client::EItemType::CARBON:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecCorbonSheet;
		break;
	case Client::EItemType::RIFLE_BULLET:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecRifleBulletSheet;
		break;
		//õ��
	case Client::EItemType::Outfit_Cloth001:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecClothArmorSheet;
		break;
		//���׿�
	case Client::EItemType::Outfit_Hunter001:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecFurArmorSheet;
		break;
	case Client::EItemType::Outfit_Platinum001_v02:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecIronArmorColdSheet;
		break;
	case Client::EItemType::HeadEquip016:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecCopperHelmetSheet;
		break;
	case Client::EItemType::HeadEquip018:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecStealHelmetSheet;
		break;
	case Client::EItemType::HeadEquip019:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecBirdHeadSheet;
		break;
	case Client::EItemType::HeadEquip021:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecPenguinHeadSheet;
		break;
	case Client::EItemType::BLUE_SPHERE:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecPalSphereBlueSheet;
		break;
	case Client::EItemType::GREEN_SPHERE:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecPalSphereGreenSheet;
		break;
	case Client::EItemType::COMMON_SHIELD:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecCommonShieldSheet;
		break;
	case Client::EItemType::GIGA_SHIELD:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecGigaShieldSheet;
		break;
	case Client::EItemType::HAND_HELD_TORCH:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecHandTorchSheet;
		break;
	case Client::EItemType::STONE_SPEAR:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecStoneSpearSheet;
		break;
	case Client::EItemType::MEAT_CLEAVER:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecMeatCleaverSheet;
		break;
	case Client::EItemType::OLD_BOW:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecOldBowSheet;
		break;
	case Client::EItemType::ROCKET_LAUNCHER:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecRocketLauncherSheet;
		break;
	case Client::EItemType::STONE_AXE:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecStoneAxeSheet;
		break;
	case Client::EItemType::STONE_PICKAXE:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecStonePickAxeSheet;
		break;
	case Client::EItemType::ASSAULT_RIFLE:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecAssaultRifleSheet;
		break;
	case Client::EItemType::GLIDER:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecGliderSheet;
		break;
	case Client::EItemType::ARROW:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecArrowSheet;
		break;
	case Client::EItemType::BAKED_BERRY:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecBakedBerriesSheet;
		break;
	case Client::EItemType::FRIED_EGG:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecFriedEggSheet;
		break;
	case Client::EItemType::SHEEPBALL_KEBAB:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecSheepballKebabSheet;
		break;
	case Client::EItemType::GRASSMAMMOTH_MEAT:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecChickenKebabSheet;
		break;
	case Client::EItemType::SHEEPBALL_ROAST:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecSheepballRoastSheet;
		break;
	case Client::EItemType::CHICKEN_ROAST:
		vecCurIndex = CClientLoop::s_spClientDataSheet->k_vecChickenRoastSheet;
		break;
	}

	return vecCurIndex;
}

std::wstring Client::CUIManager::FindItemName(EItemType _eItemType)
{
	std::wstring wstrName;

	switch (_eItemType)
	{
	case Client::EItemType::PALDIUM_FRAGMENT:
		wstrName = L"������ ����";
		break;
	case Client::EItemType::GOLD_COIN:
		wstrName = L"��ȭ";
		break;
	case Client::EItemType::STONE:
		wstrName = L"��";
		break;
	case Client::EItemType::WOOD:
		wstrName = L"����";
		break;
	case Client::EItemType::LEATHER:
		wstrName = L"����";
		break;
	case Client::EItemType::COAL:
		wstrName = L"��ź";
		break;
	case Client::EItemType::WOOL:
		wstrName = L"����";
		break;
	case Client::EItemType::CLOTH:
		wstrName = L"õ";
		break;
	case Client::EItemType::COPPER_INGOT:
		wstrName = L"�ݼ� �ֱ�";
		break;
	case Client::EItemType::IRON_INGOT:
		wstrName = L"���� �ֱ�";
		break;
	case Client::EItemType::GUNPOWDER:
		wstrName = L"ȭ��";
		break;
	case Client::EItemType::CARBON:
		wstrName = L"ī�� ����";
		break;
	case Client::EItemType::RIFLE_BULLET:
		wstrName = L"ź��";
		break;
	case Client::EItemType::Outfit_Cloth001:
		wstrName = L"õ ��";
		break;
	case Client::EItemType::Outfit_Hunter001:
		wstrName = L"���� ����";
		break;
	case Client::EItemType::Outfit_Platinum001_v02:
		wstrName = L"�� �ݼ� ����";
		break;
	case Client::EItemType::HeadEquip016:
		wstrName = L"�ݼ� ����";
		break;
	case Client::EItemType::HeadEquip018:
		wstrName = L"�� �ݼ� ����";
		break;
	case Client::EItemType::HeadEquip019:
		wstrName = L"�˷ϻ� ����";
		break;
	case Client::EItemType::HeadEquip021:
		wstrName = L"��ŷ ����";
		break;
	case Client::EItemType::BLUE_SPHERE:
		wstrName = L"�� ���Ǿ�";
		break;
	case Client::EItemType::GREEN_SPHERE:
		wstrName = L"�ް� ���Ǿ�";
		break;
	case Client::EItemType::COMMON_SHIELD:
		wstrName = L"�Ϲ� ����";
		break;
	case Client::EItemType::GIGA_SHIELD:
		wstrName = L"�Ⱑ ����";
		break;
	case Client::EItemType::HAND_HELD_TORCH:
		wstrName = L"�޴��� ȶ��";
		break;
	case Client::EItemType::STONE_SPEAR:
		wstrName = L"�� â";
		break;
	case Client::EItemType::MEAT_CLEAVER:
		wstrName = L"Ǫ�ְ� Į";
		break;
	case Client::EItemType::OLD_BOW:
		wstrName = L"������ Ȱ";
		break;
	case Client::EItemType::ROCKET_LAUNCHER:
		wstrName = L"��Ű�� ���Ϲ߻��";
		break;
	case Client::EItemType::STONE_AXE:
		wstrName = L"�� ����";
		break;
	case Client::EItemType::STONE_PICKAXE:
		wstrName = L"�� ���";
		break;
	case Client::EItemType::ASSAULT_RIFLE:
		wstrName = L"���� ����";
		break;
	case Client::EItemType::GLIDER:
		wstrName = L"�Ϲ� ���ϻ�";
		break;
	case Client::EItemType::ARROW:
		wstrName = L"ȭ��";
		break;
	case Client::EItemType::EGG:
		wstrName = L"��";
		break;
	case Client::EItemType::SHEEPBALL_MEAT:
		wstrName = L"���η�";
		break;
	case Client::EItemType::BOAR_MEAT:
		wstrName = L"��������";
		break;
	case Client::EItemType::RED_BERRY:
		wstrName = L"���� ����";
		break;
	case Client::EItemType::BAKED_BERRY:
		wstrName = L"���� ����";
		break;
	case Client::EItemType::FRIED_EGG:
		wstrName = L"��� ������";
		break;
	case Client::EItemType::SHEEPBALL_KEBAB:
		wstrName = L"���η� ��ġ";
		break;
	case Client::EItemType::GRASSMAMMOTH_MEAT:
		wstrName = L"�׸���";
		break;
	case Client::EItemType::SHEEPBALL_ROAST:
		wstrName = L"���η� ����";
		break;
	case Client::EItemType::CHICKEN_ROAST:
		wstrName = L"�������� ����";
		break;
	case Client::EItemType::ENUM_END:
		break;
	}

	return wstrName;
}

std::wstring Client::CUIManager::FindItemExplanation(EItemType _eItemType)
{
	std::wstring wstrItemExplanation = L"";
	switch (_eItemType)
	{
	case Client::EItemType::PALDIUM_FRAGMENT:		wstrItemExplanation = L"������� �������� ����ȭ�� ����."; break; 
	case Client::EItemType::GOLD_COIN:				wstrItemExplanation = L"�ŷ��� ���̴� ��ȭ."; break;
	case Client::EItemType::STONE:					wstrItemExplanation = L"���๰�̳� �������� ���� ����."; break; 
	case Client::EItemType::WOOD:					wstrItemExplanation = L"���๰�̳� �������� ���� ����."; break; 
	case Client::EItemType::LEATHER:				wstrItemExplanation = L"�ӿ��Լ� ���� ��������, \r\n���� ��ᰡ �ȴ�."; break;
	case Client::EItemType::COAL:					wstrItemExplanation = L"�������� �߰��� �� �ִ� ��ź."; break; 
	case Client::EItemType::WOOL:					wstrItemExplanation = L"�� �ӿ��Լ� ��� ����. \r\nħ�볪 õ�� ���� �� �ʿ��ϴ�."; break;
	case Client::EItemType::CLOTH:					wstrItemExplanation = L"������ ¥�� ���� õ.\r\n���� ���� �� �ʿ��ϴ�."; break;
	case Client::EItemType::COPPER_INGOT:			wstrItemExplanation = L"�ݼ��� ���⳪ ����\r\n ���� �� �ʿ��� ����."; break;
	case Client::EItemType::IRON_INGOT:				wstrItemExplanation = L"��� �ݼ��� ���⳪ ����\r\n ���� �� �ʿ��� ����."; break; 
	case Client::EItemType::GUNPOWDER:				wstrItemExplanation = L"źȯ�� �߻��� �� ���� ȭ��."; break;
	case Client::EItemType::CARBON:					wstrItemExplanation = L"���縦 �¿�� ���� �� �ִ� ��."; break; 
	case Client::EItemType::RIFLE_BULLET:			wstrItemExplanation = L"���� � ����ϴ� ź."; break;
	case Client::EItemType::Outfit_Cloth001:		wstrItemExplanation = L"õ���� ���� ��.\r\n���� �ѱ⸦ �����ش�."; break;
	case Client::EItemType::Outfit_Hunter001:		wstrItemExplanation = L"�������� ���� ����."; break; 
	case Client::EItemType::Outfit_Platinum001_v02: wstrItemExplanation = L"������ �� �ݼ� ����."; break; 
	case Client::EItemType::HeadEquip016:			wstrItemExplanation = L"�ݼ����� ���� ����."; break; 
	case Client::EItemType::HeadEquip018:			wstrItemExplanation = L"�� �ݼ� �ֱ��� ���� ����. "; break; 
	case Client::EItemType::HeadEquip019:			wstrItemExplanation = L"�˷ϻ��� ���� ����."; break;
	case Client::EItemType::HeadEquip021:			wstrItemExplanation = L"��ŷ�� ���� ����."; break;
	case Client::EItemType::BLUE_SPHERE:			wstrItemExplanation = L"������ ���� ��ȹ�ϴ� ����.\r\n������̶� ���� �ӿ��Ը� ��ȿ�ϴ�."; break;
	case Client::EItemType::GREEN_SPHERE:			wstrItemExplanation = L"������ ���� ��ȹ�ϴ� ����.\r\n������ ���� ���� ���� ���� �� �ִ�."; break;
	case Client::EItemType::COMMON_SHIELD:			wstrItemExplanation = L"������ ����� ź���� �ź��� ����."; break;
	case Client::EItemType::GIGA_SHIELD:			wstrItemExplanation = L"������ ����� ź���� �ź��� ����."; break;
	case Client::EItemType::HAND_HELD_TORCH:		wstrItemExplanation = L"�ֺ��� ȯ������ �޴��� ȶ��."; break; 
	case Client::EItemType::STONE_SPEAR:			wstrItemExplanation = L"���� �������� ������ â."; break;
	case Client::EItemType::MEAT_CLEAVER:			wstrItemExplanation = L"���� ��ü�� �� �ִ� Į."; break; 
	case Client::EItemType::OLD_BOW:				wstrItemExplanation = L"���Ÿ� ������ ������ Ȱ."; break; 
	case Client::EItemType::ROCKET_LAUNCHER:		wstrItemExplanation = L"���� �߻��."; break; 
	case Client::EItemType::STONE_AXE:				wstrItemExplanation = L"������ ���� �뵵�� ����."; break; 
	case Client::EItemType::STONE_PICKAXE:			wstrItemExplanation = L"ä���� ���."; break;
	case Client::EItemType::ASSAULT_RIFLE:			wstrItemExplanation = L"���Ÿ� ������ �˸��� ���� ����."; break; 
	case Client::EItemType::GLIDER:					wstrItemExplanation = L"���ļ� Ȱ���� �� ���� �۶��̴�."; break; 
	case Client::EItemType::ARROW:					wstrItemExplanation = L"Ȱ�� �߻��ϴ� ȭ��."; break;
	case Client::EItemType::EGG:					wstrItemExplanation = L"��������(��) ���� ���."; break;
	case Client::EItemType::SHEEPBALL_MEAT:			wstrItemExplanation = L"���η��� ���."; break; 
	case Client::EItemType::BOAR_MEAT:			wstrItemExplanation = L"���������� ���."; break;
	case Client::EItemType::RED_BERRY:				wstrItemExplanation = L"�������� ���� ����. "; break; 
	case Client::EItemType::BAKED_BERRY:			wstrItemExplanation = L"����� ���簡�� ��������. \r\n�ű��� ������ �������� ����."; break;
	case Client::EItemType::FRIED_EGG:				wstrItemExplanation = L"����(��) ������ �� ���־�����. \r\n�ű��� ������ �������� ���."; break;
	case Client::EItemType::SHEEPBALL_KEBAB:		wstrItemExplanation = L"���⸦ ���� ���� �丮."; break; 
	case Client::EItemType::GRASSMAMMOTH_MEAT:			wstrItemExplanation = L"�׸����� ���."; break;
	case Client::EItemType::SHEEPBALL_ROAST:		wstrItemExplanation = L"���⿡ ���͸� �߶� ���� �丮."; break;
	case Client::EItemType::CHICKEN_ROAST:			wstrItemExplanation = L"������⸦ ��°�� ���� �丮."; break; 
	default: break;
	}

	return wstrItemExplanation;
}

std::wstring Client::CUIManager::FindHUDWeaponTextureName(EItemType _eItemType)
{
	std::wstring prWeapon;

	switch (_eItemType)
	{
	case Client::EItemType::NONE:
		prWeapon = L"";
		break;
	case Client::EItemType::HAND_HELD_TORCH:
		prWeapon = L"T_icon_torch_UI";
		break;
	case Client::EItemType::STONE_SPEAR:
		prWeapon = L"T_Icon_spear_UI";
		break;
	case Client::EItemType::MEAT_CLEAVER:
		prWeapon = L"T_Icon_ChopperKnife_UI";
		break;
	case Client::EItemType::OLD_BOW:
		prWeapon = L"T_icon_Bow_UI";
		break;
	case Client::EItemType::ROCKET_LAUNCHER:
		prWeapon = L"T_Icon_launcher_UI";
		break;
	case Client::EItemType::STONE_AXE:
		prWeapon = L"T_Icon_axe_UI";
		break;
	case Client::EItemType::STONE_PICKAXE:
		prWeapon = L"T_Icon_pixaxe_UI";
		break;
	case Client::EItemType::ASSAULT_RIFLE:
		prWeapon = L"T_Icon_assault_rifle_UI";
		break;
	case Client::EItemType::BLUE_SPHERE:
		prWeapon = L"T_icon_palsphere_0";
		break;
	case Client::EItemType::GREEN_SPHERE:
		prWeapon = L"T_icon_palsphere_2";
		break;
	}

	return prWeapon;
}

std::wstring Client::CUIManager::FindPalWorkTextureName(EToDoType _eTodoType)
{
	std::wstring wstrName;

	switch (_eTodoType)
	{
	case Client::EToDoType::BUILD_ARCHITECTURE:
		wstrName = L"T_icon_pal_work_13";
		break;
	case Client::EToDoType::PRODUCE_ITEM:
		wstrName = L"T_icon_palwork_04";
		break;
	case Client::EToDoType::CUT_DOWN_RAW_TREE:
		wstrName = L"T_icon_palwork_06";
		break;
	case Client::EToDoType::CUT_DOWN_LUMBERYARD:
		wstrName = L"T_icon_palwork_06";
		break;
	case Client::EToDoType::MINE_RAW_ROCK:
		wstrName = L"T_icon_palwork_07";
		break;
	case Client::EToDoType::MINE_STONE_PIT:
		wstrName = L"T_icon_palwork_07";
		break;
	case Client::EToDoType::TRANSPORT_MINERAL:
		wstrName = L"T_icon_palwork_11";
		break;
	case Client::EToDoType::TRANSPORT_INGREDIENT:
		wstrName = L"T_icon_palwork_11";
		break;
	case Client::EToDoType::FARM_BERRY:
		wstrName = L"T_icon_palwork_01";
		break;
	case Client::EToDoType::COOK_FOOD:
		wstrName = L"T_icon_palwork_00";
		break;
	}

	return wstrName;
}

Vector4 Client::CUIManager::ChangeCirCleProgressBarColor(float _fRatio)
{
	Vector4 vColor;

	if (0.2f >= _fRatio)
	{
		vColor = Vector4(0.59f, 0.295f, 1.0f, 1.0f);
	}
	else if (0.4f >= _fRatio)
	{
		vColor = Vector4(0.99f, 0.5f, 1.0f, 1.0f);
	}
	else if (0.6f >= _fRatio)
	{
		vColor = Vector4(1.0f, 0.57f, 0.13f, 1.0f);
	}
	else if (0.8f >= _fRatio)
	{
		vColor = Vector4(0.47f, 1.0f, 0.29f, 1.0f);
	}
	else
	{
		// ��Ʈ��
		vColor = Vector4(0.4f, 0.96f, 1.0f, 1.0f);
	}

	return vColor;
}

void Client::CUIManager::SetActiveInteractionPanel(bool _bActive)
{
	m_spInteractionPanel->SetActive(_bActive);
}

void Client::CUIManager::SetInteractionPivotPoint(Vector3 _vPosition)
{
	m_spInteractionPanel->SetPivotWorldPoint(_vPosition);
}

void Client::CUIManager::InitHUDInfo()
{
	m_spParentsHUDPanel = FindParentsPanel(L"HUD");

	// HP panel�� ������ ����ü�� �����մϴ�.
	{
		m_tHUDHPInfo.spChildHPPanel = static_pointer_cast<CPanel>(m_spParentsHUDPanel->FindChildGameObject(L"ChildHPPanel"));
		m_tHUDHPInfo.spShieldProgressComponent = m_tHUDHPInfo.spChildHPPanel->FindComponent<CProgressBarComponent>(L"ShildProgressBar");
		m_tHUDHPInfo.spHPProgressComponent = m_tHUDHPInfo.spChildHPPanel->FindComponent<CProgressBarComponent>(L"HpProgressBar");
		m_tHUDHPInfo.spHungerProgressComponent = m_tHUDHPInfo.spChildHPPanel->FindComponent<CProgressBarComponent>(L"HungerProgressBar");

		m_tHUDHPInfo.spBackShieldImageComponent = m_tHUDHPInfo.spChildHPPanel->FindComponent<CImageComponent>(L"ShildBackImage");

		m_tHUDHPInfo.spTimezoneImageComponent = m_tHUDHPInfo.spChildHPPanel->FindComponent<CImageComponent>(L"TimeZoneImage");
		m_tHUDHPInfo.spTimezoneImageComponent->SetTextureFileName(L"");
		m_tHUDHPInfo.spTimeCircleProgressComponent = m_tHUDHPInfo.spChildHPPanel->FindComponent<CProgressBarComponent>(L"TimeCircleProgressBar");

		m_tHUDHPInfo.spMaxHPTextComponent = m_tHUDHPInfo.spChildHPPanel->FindComponent<CTextComponent>(L"HPMaxText");
		m_tHUDHPInfo.spCurHPTextComponent = m_tHUDHPInfo.spChildHPPanel->FindComponent<CTextComponent>(L"HPCurText");

		m_tHUDHPInfo.spChildHPPanel->FindComponent<CTextComponent>(L"ArchitectureText")->SetText(L"����");
	}

	// Tutorial panel�� ������ ����ü�� �����մϴ�.
	{
		m_tHUDTutorialInfo.spChildTutorialPanel = static_pointer_cast<CPanel>(m_spParentsHUDPanel->FindChildGameObject(L"ChildTutorialPanel"));
		m_tHUDTutorialInfo.spTutorialTextComponent0 = m_tHUDTutorialInfo.spChildTutorialPanel->FindComponent<CTextComponent>(L"TutorialText0");
		m_tHUDTutorialInfo.spTutorialTextComponent1 = m_tHUDTutorialInfo.spChildTutorialPanel->FindComponent<CTextComponent>(L"TutorialText1");
		m_tHUDTutorialInfo.spTutorialTextComponent2 = m_tHUDTutorialInfo.spChildTutorialPanel->FindComponent<CTextComponent>(L"TutorialText2");

		m_tHUDTutorialInfo.spTutorialCheckBoxComponent0 = m_tHUDTutorialInfo.spChildTutorialPanel->FindComponent<CImageComponent>(L"CheckBoxImage0");
		m_tHUDTutorialInfo.spTutorialCheckBoxComponent1 = m_tHUDTutorialInfo.spChildTutorialPanel->FindComponent<CImageComponent>(L"CheckBoxImage1");
		m_tHUDTutorialInfo.spTutorialCheckBoxComponent2 = m_tHUDTutorialInfo.spChildTutorialPanel->FindComponent<CImageComponent>(L"CheckBoxImage2");

		m_spQuestInfo = make_shared<FHUDTutorialInfo>(m_tHUDTutorialInfo);
	}

	// Weapon panel�� ������ ����ü�� �����մϴ�.
	{
		m_tHUDWeaponInfo.spChildWeaponPanel = static_pointer_cast<CPanel>(m_spParentsHUDPanel->FindChildGameObject(L"ChildWeaponUI"));

		m_tHUDWeaponInfo.spPalSphereNumComponent = m_tHUDWeaponInfo.spChildWeaponPanel->FindComponent<CTextComponent>(L"PalsphereCountText");
		m_tHUDWeaponInfo.spWeaponNameComponent = m_tHUDWeaponInfo.spChildWeaponPanel->FindComponent<CTextComponent>(L"WeaponText");
		m_tHUDWeaponInfo.spWeaponCurNumComponent = m_tHUDWeaponInfo.spChildWeaponPanel->FindComponent<CTextComponent>(L"WeaponMaxCountText");
		m_tHUDWeaponInfo.spWeaponOnceNumComponent = m_tHUDWeaponInfo.spChildWeaponPanel->FindComponent<CTextComponent>(L"WeaponCurCountText");

		m_tHUDWeaponInfo.spWeaponImageComponent = m_tHUDWeaponInfo.spChildWeaponPanel->FindComponent<CImageComponent>(L"WeaponIconImage");
		m_tHUDWeaponInfo.spChildWeaponPanel->FindComponent<CTextComponent>(L"PalSphereText")->SetText(L"�� ���Ǿ�");
		
		m_tHUDWeaponInfo.spWeaponNameComponent->SetText(L"");
		m_tHUDWeaponInfo.spWeaponImageComponent->SetTextureFileName(L"");
		m_tHUDWeaponInfo.spWeaponCurNumComponent->SetActive(false);
		m_tHUDWeaponInfo.spWeaponOnceNumComponent->SetActive(false);

		m_spHUDWeaponInfo = make_shared<FHUDWeaponInfo>(m_tHUDWeaponInfo);
	}

	// Direction panel�� ������ ����ü�� �����մϴ�.
	{
		m_tHUDDirectionInfo.spChildDirectionPanel = static_pointer_cast<CPanel>(m_spParentsHUDPanel->FindChildGameObject(L"ChildDirectionPanel"));

		m_tHUDDirectionInfo.spDirectionImageComponent = m_tHUDDirectionInfo.spChildDirectionPanel->FindComponent<CImageComponent>(L"DirectionLayerImage");
	
		m_tHUDDirectionInfo.spChildDirectionPanel->SetActive(false);
	}

	// Pal panel�� ������ ����ü�� �����մϴ�.
	{
		m_tHUDPalInfo.spChildPalPanel = static_pointer_cast<CPanel>(m_spParentsHUDPanel->FindChildGameObject(L"ChildPalPanel"));

		// ���� ���Ժ��� 0, 1, 2 ����
		m_tHUDPalInfo.spPalSlotComponent0 = m_tHUDPalInfo.spChildPalPanel->FindComponent<CAnimationImageComponent>(L"PalIconAnimation0");
		m_tHUDPalInfo.spPalSlotComponent1 = m_tHUDPalInfo.spChildPalPanel->FindComponent<CAnimationImageComponent>(L"PalIconAnimation1");
		m_tHUDPalInfo.spPalSlotComponent2 = m_tHUDPalInfo.spChildPalPanel->FindComponent<CAnimationImageComponent>(L"PalIconAnimation2");

		m_tHUDPalInfo.spPalSlotComponent0->SetTextureFileName(L"");
		m_tHUDPalInfo.spPalSlotComponent1->SetTextureFileName(L"");
		m_tHUDPalInfo.spPalSlotComponent2->SetTextureFileName(L"");

		m_spHUDPalInfo = make_shared<FHUDPalInfo>(m_tHUDPalInfo);
	}
}

void Client::CUIManager::InitBossInfo()
{
	m_spBossHPInfo = make_shared<FBossHPInfo>();

	m_spBossHPInfo->spBossPanel = FindParentsPanel(L"BossHPUI");
	m_spBossHPInfo->spBossHPProgress = m_spBossHPInfo->spBossPanel->FindComponent<CProgressBarComponent>(L"BossHPProgressBar");
	m_spBossHPInfo->spCurHPText = m_spBossHPInfo->spBossPanel->FindComponent<CTextComponent>(L"HPCurText");
	// �� �־��� �� /�� �־��־���մϴ�.
	m_spBossHPInfo->SPMaxHPText = m_spBossHPInfo->spBossPanel->FindComponent<CTextComponent>(L"HPMaxText");
	m_spBossHPInfo->spNameImage = m_spBossHPInfo->spBossPanel->FindComponent<CImageComponent>(L"BaseNameImage");
}

void Client::CUIManager::UpdateTimeProgress()
{
	std::shared_ptr<CDate> spDate = CEngineModule::GetInstance()->GetTimerManager()->FindDate(L"Date120");
	int32 iMin = spDate->GetMinute();
	int32 iHour = spDate->GetHour24();

	iMin += iHour * 60;

	if (iMin >= 720.0f)
	{
		iMin = iMin - 720;
	}

	float fRatio = iMin / 720.0f;

	m_tHUDHPInfo.spTimeCircleProgressComponent->SetRatio(fRatio);

	// �Ǳ� ������ ������ ������ �Ѿ�� ���ʿ� �ѹ��� �����Ű�� ����
	if (spDate->IsDay())
	{
		if (L"T_icon_timezone_daytime" != m_tHUDHPInfo.spTimezoneImageComponent->GetTextureName())
		{
			//m_wpSoundManager.lock()->StopChannelEx(EChannelType::BACKGROUND_00);
			//m_wpSoundManager.lock()->PlayBGM("Forest_Day_1_528799947");
			m_tHUDHPInfo.spTimezoneImageComponent->SetTextureFileName(L"T_icon_timezone_daytime");
		}
	}
	else
	{
		if (L"T_icon_timezone_night" != m_tHUDHPInfo.spTimezoneImageComponent->GetTextureName())
		{
			//m_wpSoundManager.lock()->StopChannelEx(EChannelType::BACKGROUND_00);
			//m_wpSoundManager.lock()->PlayBGM("Forest_Night_1_1015340320");
			m_tHUDHPInfo.spTimezoneImageComponent->SetTextureFileName(L"T_icon_timezone_night");
		}
	}
}

void Client::CUIManager::SetHUDHPInfoPanel(float _fmaxHp, float _fCurHp)
{
	m_tHUDHPInfo.spCurHPTextComponent->SetText(to_wstring(static_cast<int32>(_fCurHp)));
	m_tHUDHPInfo.spMaxHPTextComponent->SetText(L"/ " + to_wstring(static_cast<int32>(_fmaxHp)));

	m_tHUDHPInfo.spHPProgressComponent->SetRatio(1.f);
	m_tHUDHPInfo.spHPProgressComponent->SetOtherRatio(1.f);
}

void Client::CUIManager::SetActiveShield(bool _bActive)
{
	m_tHUDHPInfo.spShieldProgressComponent->SetActive(_bActive);
	m_tHUDHPInfo.spBackShieldImageComponent->SetActive(_bActive);
}

void Client::CUIManager::SetBossHP(float _fMaxHP, float _fCurHP, EMonsterType _eMonsterType)
{
	if (false == m_spBossHPInfo->spBossPanel->IsActive())
	{
		m_spBossHPInfo->spBossPanel->SetActive(true);
	}
	// ���� ü�� ������ ���� ������Ʈ ���ݴϴ�.
	float fRatio = (_fMaxHP - _fCurHP) / _fMaxHP;
	m_spBossHPInfo->spBossHPProgress->SetRatio(1.f - fRatio);

	m_spBossHPInfo->spCurHPText->SetText(to_wstring(static_cast<int32>(_fCurHP)));
	m_spBossHPInfo->SPMaxHPText->SetText(L"/" + to_wstring(static_cast<int32>(_fMaxHP)));

	if (EMonsterType::GRASS_MAMMOTH == _eMonsterType)
	{
		m_spBossHPInfo->spNameImage->SetTextureFileName(L"T_GrassMammoth_NameImage");
	}
	else if (EMonsterType::ELEC_PANDA == _eMonsterType)
	{
		m_spBossHPInfo->spNameImage->SetTextureFileName(L"T_Boss_NameImage");
	}
}

EItemType Client::CUIManager::ChangeMineralType(EMineralType _eMineralType)
{
	EItemType eItemType = EItemType::ENUM_END;

	switch (_eMineralType)
	{
	case Client::EMineralType::COAL:
		eItemType = EItemType::COAL;
		break;
	case Client::EMineralType::GOLD_COIN:
		eItemType = EItemType::GOLD_COIN;
		break;
	case Client::EMineralType::IRON:
		eItemType = EItemType::IRON;
		break;
	case Client::EMineralType::LEATHER:
		eItemType = EItemType::LEATHER;
		break;
	case Client::EMineralType::STONE:
		eItemType = EItemType::STONE;
		break;
	case Client::EMineralType::WOOD:
		eItemType = EItemType::WOOD;
		break;
	case Client::EMineralType::WOOL:
		eItemType = EItemType::WOOL;
		break;
	case Client::EMineralType::CLOTH:
		eItemType = EItemType::CLOTH;
		break;
	case Client::EMineralType::IRON_INGOT:
		eItemType = EItemType::IRON_INGOT;
		break;
	case Client::EMineralType::COPPER_INGOT: 
		eItemType = EItemType::COPPER_INGOT;
		break;
	}

	return eItemType;
}

void Client::CUIManager::SetTopPanel(shared_ptr<CPanel> _spPanel)
{
	if (nullptr != m_spTopPanel)
	{
		weak_ptr<CGameManager> wpGameManager = CGameManager::GetInstance();

		if (_spPanel != wpGameManager.lock()->GetInventorySystem()->GetInventoryFeedWhoParentPanel())
		{
			if (m_spTopPanel == wpGameManager.lock()->GetInventorySystem()->GetContainerParentPanel())
			{
				weak_ptr<CPlayer> wpPlayer = wpGameManager.lock()->GetPlayer();
				wpPlayer.lock()->SetCustomInvenCustomObject(false);
				m_spTopPanel->SetActive(false);
				m_spTopPanel = nullptr;
			}
			else if (m_spTopPanel == wpGameManager.lock()->GetInventorySystem()->GetInventoryFeedWhoParentPanel())
			{
				m_spTopPanel->SetActive(false);
				m_spTopPanel = nullptr;
			}
			else
			{
				//return;
				m_spTopPanel->SetActive(false);
				m_spTopPanel = nullptr;
			}
		}
	}

	m_spTopPanel = _spPanel;
}  

//void Client::CUIManager::SetProductionArchitectureUI(const std::wstring& _wstrLoadFileName, const std::wstring& _wstrMapKeyName, int32 _iIndex)
//{
//	for (int32 i = 0; i < _iIndex; i++)
//	{
//		wstring wstrName = _wstrMapKeyName + to_wstring(m_iWorkArchitectureCount++);
//		shared_ptr<CPanel> spPanel = LoadPanel(_wstrLoadFileName, wstrName);
//		m_listProductionArchitectureUI.push_back(spPanel);
//	}
//}

//void Client::CUIManager::SetWorkUI(const std::wstring& _wstrLoadFileName, const std::wstring& _wstrMapKeyName, int32 _iIndex)
//{
//	for (int32 i = 0; i < _iIndex; i++)
//	{
//		wstring wstrName = _wstrMapKeyName + to_wstring(m_iWorkCount++);
//		shared_ptr<CPanel> spPanel = LoadPanel(_wstrLoadFileName, wstrName);
//		spPanel->SetActive(false);
//		spPanel->FindComponent<CProgressBarComponent>(L"CircleProgressBar")->SetRatio(0.0f);
//		spPanel->FindComponent<CTextComponent>(L"CurCountText")->SetText(to_wstring(0));
//		m_listWorkUI.push_back(spPanel);
//	}
//}

//void Client::CUIManager::SetCaptureUI(const std::wstring& _wstrLoadFileName, const std::wstring& _wstrMapKeyName, int32 _iIndex)
//{
//	for (int32 i = 0; i < _iIndex; i++)
//	{
//		wstring wstrName = _wstrMapKeyName + to_wstring(m_iCaptureCount++);
//		shared_ptr<CPanel> spPanel = LoadPanel(_wstrLoadFileName, wstrName);
//		spPanel->SetActive(false);
//		m_listCaptureUI.push_back(spPanel);
//	}
//}

//void Client::CUIManager::SetPalHPUI(const std::wstring& _wstrLoadFileName, const std::wstring& _wstrMapKeyName, int32 _iIndex)
//{
//	for (int32 i = 0; i < _iIndex; i++)
//	{
//		wstring wstrName = _wstrMapKeyName + to_wstring(m_iPalHPCount++);
//		shared_ptr<CPanel> spPanel = LoadPanel(_wstrLoadFileName, wstrName);
//		spPanel->SetActive(false);
//		// �̸� �ڿ� �ִ� ��ó�� �̹����� ���ϴ�.
//		spPanel->FindComponent<CImageComponent>(L"NameBackImage")->SetActive(false);
//		m_listPalHPUI.push_back(spPanel);
//	}
//}

void Client::CUIManager::RecycleUIs(const std::wstring& _wstrLoadFileName, const std::wstring& _wstrMapKeyName, int32 _iIndex, std::list<std::shared_ptr<CPanel>>& _listUIs, int32 _ilistCount)
{
	for (int32 i = 0; i < _iIndex; i++)
	{
		wstring wstrName = _wstrMapKeyName + to_wstring(_ilistCount++);
		shared_ptr<CPanel> spPanel = LoadPanel(_wstrLoadFileName, wstrName);
		spPanel->SetActive(false);
		_listUIs.push_back(spPanel);
	}
}

void Client::CUIManager::RecycleDamageFonts(int32 _iIndex)
{
	for (int32 i = 0; i < _iIndex; i++)
	{
		wstring wstrName = L"DamageFontUI" + to_wstring(m_iDamageCount++);
		shared_ptr<CDamageFontObject> spDamageFont = LoadDamageFont(L"DamageFontUI", wstrName);
		spDamageFont->SetDamageTextComponent(spDamageFont->FindComponent<CTextComponent>(L"DamageText"), spDamageFont->FindComponent<CTextComponent>(L"BaseText"));
		spDamageFont->SetActive(false);
		m_listDamageUI.push_back(spDamageFont);
	}
}
