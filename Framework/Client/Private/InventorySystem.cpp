#include "pch.h"
#include "InventorySystem.h"

// Client Manager
#include "ClientLoop.h"
#include "UIManager.h"

// Engine Manager
#include "EngineModule.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "GameManager.h"
#include "UIManager.h"
#include "SoundManager.h"

// GameObject
#include "Panel.h"
#include "Camera.h"
#include "Player.h"
#include "CameraComponent.h"

// Component
#include "TextComponent.h"
#include "ImageComponent.h"
#include "ProgressBarComponent.h"

//Test
#include "Item.h"
#include "Weapon.h"
#include "Armor.h"
#include "Mineral.h"
#include "Ingredient.h"
#include "PalBoxSystem.h"


std::shared_ptr<CInventorySystem> Client::CInventorySystem::Create(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc)
{
	shared_ptr<CInventorySystem> spInstance = make_shared<CInventorySystem>();
	CHECKF(spInstance, L"CInventory : Create Failed");
	spInstance->Initialize(_spContainerSystemDesc);

	return spInstance;
}

HRESULT Client::CInventorySystem::Initialize(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc)
{
	if (FAILED(CContainerSystem::Initialize(_spContainerSystemDesc))) { return E_FAIL; }

	// from sample level initialize to inventory system initialize.
	{
		m_spContainerParentPanel    = m_spUIManager->FindParentsPanel(L"InventoryPanel");
		m_spItemInfoParentPanel		= m_spUIManager->FindParentsPanel(L"InventoryItemInfo");
		m_spItemInfoFoodParentPanel = m_spUIManager->FindParentsPanel(L"InventoryItemInfoFood");
		m_spFeedWhoParentPanel		= m_spUIManager->FindParentsPanel(L"InventoryFeedWho");
		m_spContainerChildPanel     = m_spUIManager->FindChildPanel(L"InventoryPanel", L"Inventory_slot");
		m_spMouseChildPanel		    = m_spUIManager->FindChildPanel(L"InventoryPanel", L"Mouse_slot");
		m_spFeedWhoChildPanel		= m_spUIManager->FindChildPanel(L"InventoryFeedWho", L"FeedWhoMain"); 
		m_spFeedWhoDetailInfoPanel	= m_spUIManager->FindChildPanel(L"InventoryFeedWho", L"FeedWhoDetailPalInfo");
		
		m_wpItemSlotHoverImage		= m_spContainerChildPanel->FindComponent<CRectComponent>(L"item_slot_hover");
		m_wpDropDiscardHoverImage	= m_spContainerChildPanel->FindComponent<CRectComponent>(L"drop_discard_hover");
		m_wpSortHoverImage			= m_spContainerChildPanel->FindComponent<CRectComponent>(L"sort_slot_hover");
		m_wpFeedWhoHoverImage		= m_spFeedWhoChildPanel->FindComponent<CRectComponent>(L"feed_who_hover");
		m_wpCurMouseSlotComponent	= m_spMouseChildPanel->FindComponent<CImageComponent>(L"mouse_slot");


		m_vecFeedWhoDetailInfoPanel.resize(49);
		m_vecFeedWhoDetailInfoPanel[0] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"pal_icon_00");
		m_vecFeedWhoDetailInfoPanel[1] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"pal_element_00");
		m_vecFeedWhoDetailInfoPanel[2] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"exp_bar_00");
		m_vecFeedWhoDetailInfoPanel[3] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"hp_bar_00");
		m_vecFeedWhoDetailInfoPanel[4] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"hunger_bar_00");
		m_vecFeedWhoDetailInfoPanel[5] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"pal_name_00");
		m_vecFeedWhoDetailInfoPanel[6] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"pal_level_num_00");
		m_vecFeedWhoDetailInfoPanel[7] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"pal_exp_num_00");
		m_vecFeedWhoDetailInfoPanel[8] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"pal_hp_num_00");
		m_vecFeedWhoDetailInfoPanel[9] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"pal_hunger_num_00"); //9
		m_vecFeedWhoDetailInfoPanel[10] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"pal_atk_num_00"); // 10
		m_vecFeedWhoDetailInfoPanel[11] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"pal_def_num_00");
		m_vecFeedWhoDetailInfoPanel[12] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"pal_workspeed_num_00");
		m_vecFeedWhoDetailInfoPanel[13] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_00"); // 13
		m_vecFeedWhoDetailInfoPanel[14] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_01");
		m_vecFeedWhoDetailInfoPanel[15] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_02");
		m_vecFeedWhoDetailInfoPanel[16] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_03");
		m_vecFeedWhoDetailInfoPanel[17] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_04");
		m_vecFeedWhoDetailInfoPanel[18] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_05");
		m_vecFeedWhoDetailInfoPanel[19] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_06");
		m_vecFeedWhoDetailInfoPanel[20] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_07"); // 20
		m_vecFeedWhoDetailInfoPanel[21] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_08");
		m_vecFeedWhoDetailInfoPanel[22] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_09");
		m_vecFeedWhoDetailInfoPanel[23] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_10");
		m_vecFeedWhoDetailInfoPanel[24] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"palwork_11"); //24
		m_vecFeedWhoDetailInfoPanel[25] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"cur_mission_name_00"); // 25
		m_vecFeedWhoDetailInfoPanel[26] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"food_amount_00");
		m_vecFeedWhoDetailInfoPanel[27] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"food_amount_01");
		m_vecFeedWhoDetailInfoPanel[28] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"food_amount_02");
		m_vecFeedWhoDetailInfoPanel[29] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"food_amount_03");
		m_vecFeedWhoDetailInfoPanel[30] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"food_amount_04"); // 30
		m_vecFeedWhoDetailInfoPanel[31] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"food_amount_05");
		m_vecFeedWhoDetailInfoPanel[32] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"food_amount_06");
		m_vecFeedWhoDetailInfoPanel[33] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"food_amount_07");
		m_vecFeedWhoDetailInfoPanel[34] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"food_amount_08");
		m_vecFeedWhoDetailInfoPanel[35] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"food_amount_09"); // 35
		m_vecFeedWhoDetailInfoPanel[36] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"partner_skill_name_00");
		m_vecFeedWhoDetailInfoPanel[37] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"partner_skill_level_00");
		m_vecFeedWhoDetailInfoPanel[38] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"active_skill_element_00");
		m_vecFeedWhoDetailInfoPanel[39] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"active_skill_element_10");
		m_vecFeedWhoDetailInfoPanel[40] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"active_skill_element_20");
		m_vecFeedWhoDetailInfoPanel[41] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"active_skill_level_00");
		m_vecFeedWhoDetailInfoPanel[42] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"active_skill_level_10");
		m_vecFeedWhoDetailInfoPanel[43] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"active_skill_level_20");
		m_vecFeedWhoDetailInfoPanel[44] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"active_skill_name_00");
		m_vecFeedWhoDetailInfoPanel[45] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"active_skill_name_10");
		m_vecFeedWhoDetailInfoPanel[46] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"active_skill_name_20");
		m_vecFeedWhoDetailInfoPanel[47] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"passive_skill_name_00");
		m_vecFeedWhoDetailInfoPanel[48] = m_spFeedWhoDetailInfoPanel->FindComponent<CRectComponent>(L"passive_skill_name_01"); 
	}

	m_wpPlayerHpBarProgressBarxComponent = m_spContainerChildPanel->FindComponent<CProgressBarComponent>(L"hp_bar_00");
	m_wpPlayerHpBarNumTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(L"hp_bar_num_00");
	m_wpPlayerHpNumTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(L"hp_num_00");

	m_wpInventoryWeightBarProgressBarxComponent = m_spContainerChildPanel->FindComponent<CProgressBarComponent>(L"weight_bar_00");
	m_wpInventoryWeightBarNumTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(L"weight_bar_num_00");
	m_wpInventoryWeightNumTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(L"weight_num_00");

	return S_OK;
}

HRESULT Client::CInventorySystem::BeginPlay()
{
	if (FAILED(CContainerSystem::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CInventorySystem::PreTick(float _fDeltaSeconds)
{
	switch(CContainerSystem::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CInventorySystem::Tick(float _fDeltaSeconds)
{
	switch (CContainerSystem::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	
	// test 특정 이벤트 발생(아이템과 충돌, 아이템 get)
	{
		if (m_spInputManager->IsKeyDown(DIK_I))
		{	
			AddItemToContainer(EItemType::RED_BERRY, 25);
			AddItemToContainer(EItemType::COMMON_SHIELD, 1);
			AddItemToContainer(EItemType::SHEEPBALL_MEAT, 8);
			AddItemToContainer(EItemType::WOOD, 300);
			AddItemToContainer(EItemType::WOOL, 60);
			AddItemToContainer(EItemType::STONE, 300);
			AddItemToContainer(EItemType::PALDIUM_FRAGMENT, 11);
			AddItemToContainer(EItemType::BAKED_BERRY, 20);
			AddItemToContainer(EItemType::STONE_AXE, 1);
			AddItemToContainer(EItemType::STONE_PICKAXE, 1);
			AddItemToContainer(EItemType::STONE_SPEAR, 1);
			AddItemToContainer(EItemType::HAND_HELD_TORCH, 1);
			AddItemToContainer(EItemType::Outfit_Hunter001, 1);
			AddItemToContainer(EItemType::GLIDER, 1);
			AddItemToContainer(EItemType::OLD_BOW, 1);
			//AddItemToContainer(EItemType::STONE_AXE, 1);
			AddItemToContainer(EItemType::ARROW, 100);
			AddItemToContainer(EItemType::IRON, 100);
			AddItemToContainer(EItemType::IRON_INGOT, 100);
			//AddItemToContainer(EItemType::BLUE_SPHERE, 10);
			//AddItemToContainer(EItemType::HAND_HELD_TORCH, 1);
			AddItemToContainer(EItemType::ASSAULT_RIFLE, 1);
			AddItemToContainer(EItemType::Outfit_Platinum001_v02, 1);
			AddItemToContainer(EItemType::HeadEquip019, 1);
			AddItemToContainer(EItemType::HeadEquip018, 1);
			//AddItemToContainer(EItemType::HeadEquip016, 1);
			//AddItemToContainer(EItemType::BAKED_BERRY, 100);
		}
		if (m_spInputManager->IsKeyDown(DIK_J))
		{
			AddItemToContainer(EItemType::COMMON_SHIELD, 1);
			AddItemToContainer(EItemType::OLD_BOW, 1);
			AddItemToContainer(EItemType::ARROW, 100);
			AddItemToContainer(EItemType::Outfit_Platinum001_v02, 1);
			AddItemToContainer(EItemType::HeadEquip018, 1);
			AddItemToContainer(EItemType::ASSAULT_RIFLE, 1);
			AddItemToContainer(EItemType::RIFLE_BULLET, 600);
			AddItemToContainer(EItemType::GLIDER, 1);
		}
	}
	CaculatePlayerHp();
	CalculateNumWeight();
	OpenInventoryWidget(DIK_TAB);
	
	if (m_spUIManager->GetTopPanel() == GetContainerParentPanel() ||
		m_spUIManager->GetTopPanel() == m_spFeedWhoParentPanel)
	{		
		UpdateCurMouseSlotPos();
		ActiveSlotHover(); 
		ActiveItemInfoWindow();
		ActiveFeedWhoWindow();
		MoveItemUI();
		ShowEquippedItemUI();

	}
	else if(m_spUIManager->GetTopPanel() != GetContainerParentPanel() ||
			m_spUIManager->GetTopPanel() != m_spFeedWhoParentPanel)
	{
		m_spItemInfoParentPanel->SetActive(false);
		m_spItemInfoFoodParentPanel->SetActive(false);
	}
	
	return 0;
}

int32 Client::CInventorySystem::PostTick(float _fDeltaSeconds)
{
	switch(CContainerSystem::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CInventorySystem::EndPlay()
{
	if (FAILED(CContainerSystem::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CInventorySystem::Release()
{	
	m_spItemInfoParentPanel = nullptr;
	m_spItemInfoFoodParentPanel = nullptr;
	m_spFeedWhoParentPanel = nullptr;
	m_spFeedWhoChildPanel = nullptr;
	m_spFeedWhoDetailInfoPanel = nullptr;

	CContainerSystem::Release();
}

void Client::CInventorySystem::SetPlayerStatus()
{
	m_spContainerChildPanel->FindComponent<CProgressBarComponent>(L"exp_bar_00")->SetRatio(m_wpPlayer.lock()->GetCurExp() / m_wpPlayer.lock()->GetMaxExp());
	//m_spContainerChildPanel->FindComponent<CProgressBarComponent>(L"shield_bar_00")->SetRatio(m_wpPlayer.lock()->GetCurExp() * 0.01f);
	m_spContainerChildPanel->FindComponent<CProgressBarComponent>(L"hp_bar_00")->SetRatio(m_wpPlayer.lock()->GetCurHp() / m_wpPlayer.lock()->GetMaxHp());
	m_spContainerChildPanel->FindComponent<CProgressBarComponent>(L"hunger_bar_00")->SetRatio(m_wpPlayer.lock()->GetCurHungerGauge() / m_wpPlayer.lock()->GetMaxHungerGauge());
	m_spContainerChildPanel->FindComponent<CTextComponent>(L"player_level_num_00")->SetText(std::to_wstring(m_wpPlayer.lock()->GetCurCreatureLevel()));
	m_spContainerChildPanel->FindComponent<CTextComponent>(L"exp_bar_num_00")->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurExp())));
	//m_spContainerChildPanel->FindComponent<CTextComponent>(L"shield_bar_num_00")->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurExp())));
	m_spContainerChildPanel->FindComponent<CTextComponent>(L"hp_bar_num_00")->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHp())));
	m_spContainerChildPanel->FindComponent<CTextComponent>(L"hunger_bar_num_00")->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHungerGauge())));
	m_spContainerChildPanel->FindComponent<CTextComponent>(L"hp_num_00")->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHp())));
	//m_spContainerChildPanel->FindComponent<CTextComponent>(L"shield_num_00")->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHp())));
	m_spContainerChildPanel->FindComponent<CTextComponent>(L"energy_num_00")->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurStaminaGauge())));
	m_spContainerChildPanel->FindComponent<CTextComponent>(L"atk_num_00")->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetAttackPower())));
	m_spContainerChildPanel->FindComponent<CTextComponent>(L"def_num_00")->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetDefensePower())));
	m_spContainerChildPanel->FindComponent<CTextComponent>(L"workspeed_num_00")->SetText(L"20");
	m_spContainerChildPanel->FindComponent<CTextComponent>(L"money_num_00")->SetText(L"100,000");
}

void Client::CInventorySystem::SetEquippedWeaponItemSlots(FInventoryItemInfo _tInventoryItemInfo)
{
	_tInventoryItemInfo.m_iIndexX = 0;
	_tInventoryItemInfo.m_iIndexY = 0;
	m_arrWeaponSlots[0] = _tInventoryItemInfo;
}

void Client::CInventorySystem::SetEquippedAccessoryItemSlots(FInventoryItemInfo _tInventoryItemInfo)
{
	_tInventoryItemInfo.m_iIndexX = 0;
	_tInventoryItemInfo.m_iIndexY = 0;
	m_arrAccessorySlots[0] = _tInventoryItemInfo;
}

void Client::CInventorySystem::SetEquippedFoodItemSlots(FInventoryItemInfo _tInventoryItemInfo)
{
	_tInventoryItemInfo.m_iIndexX = 0;
	_tInventoryItemInfo.m_iIndexY = 0;
	m_arrFoodSlots[0] = _tInventoryItemInfo; 
}

int32 Client::CInventorySystem::FindInventoryItemTotalNum(EItemType _eItemType) const
{
	if (!m_umapInventoryNumItem.contains(_eItemType)) { return 0; }

	return m_umapInventoryNumItem.find(_eItemType)->second;
}

void Client::CInventorySystem::ReduceItemNum(EItemType _eItemType, int32 _iNumItem)
{
	if (!m_mapInventoryItemInfos.contains(_eItemType)) { return; }

	int32 iIndexX = m_mapInventoryItemInfos[_eItemType].m_iIndexX;
	int32 iIndexY = m_mapInventoryItemInfos[_eItemType].m_iIndexY;

	m_umapInventoryNumItem[_eItemType] -= _iNumItem;
	m_mapInventoryItemInfos[_eItemType].m_iNumItem -= _iNumItem;
	m_arrInventorySlots[iIndexX][iIndexY].m_iNumItem -= _iNumItem;

	int32 iNumItem = m_arrInventorySlots[iIndexX][iIndexY].m_iNumItem;

	m_spContainerChildPanel->FindComponent<CTextComponent>(L"item_slot_num_" + std::to_wstring(iIndexX) + std::to_wstring(iIndexY))->SetText(std::to_wstring(iNumItem));

	if (iNumItem <= 0)
	{  
		if (_eItemType >= EItemType::EGG && _eItemType <= EItemType::CHICKEN_ROAST) 
		{
			DiscardFoodItemFromInventory(std::pair{ iIndexX, iIndexY }); 
			m_spFeedWhoParentPanel->SetActive(false);
			m_spUIManager->SetRightClickFocusedControlComponent(nullptr);
			m_spUIManager->SetTopPanel(m_spContainerParentPanel);
		}
		else 
		{
			int32 iIndexX = m_mapInventoryItemInfos[_eItemType].m_iIndexX;
			int32 iIndexY = m_mapInventoryItemInfos[_eItemType].m_iIndexY;

			m_wstrPreClickedComponentName = L"item_slot_" + std::to_wstring(iIndexX) + std::to_wstring(iIndexY);
			DiscardItemFromInventory(std::pair{ iIndexX, iIndexY }); 
		}
	}
}

void Client::CInventorySystem::AddItemToContainer(const std::shared_ptr<CItem>& _spItem)
{
	if (nullptr == _spItem) { return; }
	EItemType eItemType = _spItem->GetItemType();

	// 인벤토리에 해당 아이템이 없을 경우 
	auto iter = m_mapInventoryItemInfos.find(eItemType);
	if (iter == m_mapInventoryItemInfos.end())
	{
		int32 iFlag = 0;

		for (int32 iOuterIndex = 0; iOuterIndex < 7; iOuterIndex++)
		{
			for (int32 iInnerIndex = 0; iInnerIndex < 6; iInnerIndex++)
			{
				if (m_arrInventorySlots[iOuterIndex][iInnerIndex].m_eItemType == EItemType::ENUM_END)
				{
					// 2중 for문 탈출을 위한 flag입니다.
					iFlag = 1;

					EItemKindType eItemKindType = _spItem->GetItemKindType();

					switch (eItemKindType)
					{
					case EItemKindType::WEAPON:
					{
						weak_ptr<CWeapon> wpWeapon = static_pointer_cast<CWeapon>(_spItem);
						FInventoryItemInfo tWeaponItemInfo		  = {};
						tWeaponItemInfo.m_eItemType			  = wpWeapon.lock()->GetItemType();
						tWeaponItemInfo.m_eItemKindType	      = EItemKindType::WEAPON;
						tWeaponItemInfo.m_iIndexX			  = iOuterIndex;
						tWeaponItemInfo.m_iIndexY			  = iInnerIndex;
						tWeaponItemInfo.m_iNumItem			  = wpWeapon.lock()->GetNumItems();
						tWeaponItemInfo.m_eWeaponType		  = wpWeapon.lock()->GetWeaponType();
						tWeaponItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tWeaponItemInfo.m_eItemType);
						tWeaponItemInfo.m_fAttackPower		  = wpWeapon.lock()->GetAttackPower();
						tWeaponItemInfo.m_fDurability		  = 5;
						tWeaponItemInfo.m_fWeight			  = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(eItemType);

						m_mapInventoryItemInfos.insert({ eItemType, tWeaponItemInfo });
						m_arrInventorySlots[iOuterIndex][iInnerIndex] = tWeaponItemInfo;

					} break;

					case EItemKindType::ARMOR:
					{
						weak_ptr<CArmor> wpArmor = static_pointer_cast<CArmor>(_spItem);
						FInventoryItemInfo tArmorItemInfo	     = {};
						tArmorItemInfo.m_eItemType		     = wpArmor.lock()->GetItemType();
						tArmorItemInfo.m_eItemKindType       = EItemKindType::ARMOR;
						tArmorItemInfo.m_iIndexX		     = iOuterIndex;
						tArmorItemInfo.m_iIndexY		     = iInnerIndex;
						tArmorItemInfo.m_iNumItem		     = wpArmor.lock()->GetNumItems();
						tArmorItemInfo.m_eArmorType		     = wpArmor.lock()->GetCurArmorType();
						tArmorItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tArmorItemInfo.m_eItemType);
						tArmorItemInfo.m_fDefensePower	     = wpArmor.lock()->GetDefensePower();
						tArmorItemInfo.m_fDurability	     = 5;
						tArmorItemInfo.m_fShieldCoolTime     = wpArmor.lock()->GetDefenseCoolTime();
						tArmorItemInfo.m_fWeight			 = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(eItemType);

						m_mapInventoryItemInfos.insert({ eItemType, tArmorItemInfo });
						m_arrInventorySlots[iOuterIndex][iInnerIndex] = tArmorItemInfo;

					} break;

					case EItemKindType::MINERAL:
					{
						weak_ptr<CMineral> wpMineral = static_pointer_cast<CMineral>(_spItem);
						FInventoryItemInfo tMineralItemInfo      = {};
						tMineralItemInfo.m_eItemType		   = wpMineral.lock()->GetItemType();
						tMineralItemInfo.m_eItemKindType       = EItemKindType::MINERAL;
						tMineralItemInfo.m_iIndexX		       = iOuterIndex;
						tMineralItemInfo.m_iIndexY		       = iInnerIndex;
						tMineralItemInfo.m_iNumItem		       = wpMineral.lock()->GetNumItems();
						tMineralItemInfo.m_eMineralType        = wpMineral.lock()->GetMineralType();
						tMineralItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tMineralItemInfo.m_eItemType);
						tMineralItemInfo.m_fWeight			   = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(eItemType);

						m_mapInventoryItemInfos.insert({ eItemType, tMineralItemInfo });
						m_arrInventorySlots[iOuterIndex][iInnerIndex] = tMineralItemInfo;

					} break;

					case EItemKindType::INGREDIENT:
					{
						weak_ptr<CIngredient> wpIngredient = static_pointer_cast<CIngredient>(_spItem);
						FInventoryItemInfo tIngredientItemInfo   = {};
						tIngredientItemInfo.m_eItemType			  = wpIngredient.lock()->GetItemType();
						tIngredientItemInfo.m_eItemKindType       = EItemKindType::INGREDIENT;
						tIngredientItemInfo.m_iIndexX			  = iOuterIndex;
						tIngredientItemInfo.m_iIndexY			  = iInnerIndex;
						tIngredientItemInfo.m_iNumItem			  = wpIngredient.lock()->GetNumItems();
						tIngredientItemInfo.m_eIngredientType	  = wpIngredient.lock()->GetIngredientType();
						tIngredientItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tIngredientItemInfo.m_eItemType);
						tIngredientItemInfo.m_fWeight			  = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(eItemType);
						tIngredientItemInfo.m_iNutrition		  = CClientLoop::s_spClientDataSheet->k_mapIngredientDefaultNutrition.at(tIngredientItemInfo.m_eIngredientType);

						m_mapInventoryItemInfos.insert({ eItemType, tIngredientItemInfo });
						m_arrInventorySlots[iOuterIndex][iInnerIndex] = tIngredientItemInfo;

					} break;

					default: break;
					}

					// 객체 해제 테스트 필요
					_spItem->Release();
				
					m_umapInventoryNumItem.insert({ eItemType,  _spItem->GetNumItems() });

					break;
				}
			}

			if (iFlag == 1) { break; }
		}
	}
	// 인벤토리에 해당 아이템이 있을 경우
	else
	{
		FInventoryItemInfo tMapInventoryItemInfo = iter->second;
		tMapInventoryItemInfo.m_iNumItem += _spItem->GetNumItems();
		tMapInventoryItemInfo.m_fWeight += (CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(eItemType) * tMapInventoryItemInfo.m_iNumItem); 
		m_arrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY] = tMapInventoryItemInfo; // 2차원 벡터에 넣어줌으로써, map과 vector의 정보 동기화
		iter->second = tMapInventoryItemInfo;

		// 객체 해제해주기
		_spItem->Release();

		m_umapInventoryNumItem[eItemType] += _spItem->GetNumItems();
	}
}

void Client::CInventorySystem::SortItemsInContainer(ESortingType _eSortingType)
{
	int32 iIndex = 0;
	std::array<FInventoryItemInfo, 42> arrItems{};

	for (int32 i = 0; i < m_arrInventorySlots.size(); i++)
	{
		for (int32 j = 0; j < m_arrInventorySlots[i].size(); j++)
		{
			arrItems[iIndex++] = m_arrInventorySlots[i][j];
		}
	}

	// 정렬
	std::sort(arrItems.begin(), arrItems.end(), [&](const FInventoryItemInfo& _tInventoryItemInfoA, const FInventoryItemInfo& _tInventoryItemInfoB) -> bool
		{
			switch (_eSortingType)
			{
			case ESortingType::RANDOM: { return _tInventoryItemInfoA.m_eItemType != EItemType::ENUM_END && _tInventoryItemInfoB.m_eItemType != EItemType::ENUM_END; }
			case ESortingType::ITEM_TYPE: { return _tInventoryItemInfoA.m_eItemType > _tInventoryItemInfoB.m_eItemType; }
			case ESortingType::ITEM_COUNT: { return _tInventoryItemInfoA.m_iNumItem > _tInventoryItemInfoB.m_iNumItem; }
			}
			return 0;
		});

	iIndex = 0;
	for (int32 i = 0; i < m_arrInventorySlots.size(); i++)
	{
		for (int32 j = 0; j < m_arrInventorySlots[i].size(); j++)
		{
			wstring wstrItemSlotImageComponentName = L"item_slot_" + std::to_wstring(i) + std::to_wstring(j);
			wstring wstrItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(i) + std::to_wstring(j);
			weak_ptr<CImageComponent> wpItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrItemSlotImageComponentName);
			weak_ptr<CTextComponent>  wpItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrItemSlotTextComponentName);
			wpItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpItemSlotTextComponent.lock()->SetText(L"");

			m_arrInventorySlots[i][j] = arrItems[iIndex++];

			if (m_arrInventorySlots[i][j].m_eItemType == EItemType::ENUM_END) { continue; }

			// 데이터 교환
			EItemType eItemType = m_arrInventorySlots[i][j].m_eItemType;
			m_mapInventoryItemInfos.find(eItemType)->second = m_arrInventorySlots[i][j];

			wpItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[i][j].m_wstrTextureFileName);
			wpItemSlotTextComponent.lock()->SetText(std::to_wstring(m_arrInventorySlots[i][j].m_iNumItem));
		}
	}
}

void Client::CInventorySystem::DropItemFromInventory(std::shared_ptr<CItem>& _spItem)
{  
	if (nullptr != m_spUIManager->GetFocusedControlComponent().lock())
	{
		/*if (L"item_drop_slot_0" == m_spUIManager->GetHoverImageComponent().lock()->GetComponentName()
			&& m_mapItemTextureFileName.find(EItemType::NONE)->second == dynamic_pointer_cast<CImageComponent>(m_spUIManager->GetFocusedControlComponent().lock())->GetTextureName())
		{
			int a = 0;
		}*/
	}
}

void Client::CInventorySystem::DiscardItemFromInventory(std::pair<int32, int32> _prPreClickedItemSlotIndexs)
{
	int32 iPreClickedIndexX = _prPreClickedItemSlotIndexs.first;
	int32 iPreClickedIndexY = _prPreClickedItemSlotIndexs.second;
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[iPreClickedIndexX][iPreClickedIndexY];

	// 데이터를 삭제합니다,
	{
		EItemType m_ePreClickedItemType = tPreInventoryItemInfo.m_eItemType;
		m_mapInventoryItemInfos.erase(m_ePreClickedItemType);
		m_arrInventorySlots[iPreClickedIndexX][iPreClickedIndexY] = {};
		m_umapInventoryNumItem.erase(m_ePreClickedItemType);
	}

	// UI 삭제합니다.
	{
		// 슬롯의 이미지를 투명 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

		// 슬롯의 숫자 텍스트를 교체합니다.
		wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iPreClickedIndexX) + std::to_wstring(iPreClickedIndexY);
		weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
		wpPreClickedItemSlotTextComponent.lock()->SetText(L"");
	}	
}

void Client::CInventorySystem::DiscardFoodItemFromInventory(std::pair<int32, int32> _prFoodItemIndexs)
{
	int32 iPreClickedIndexX = _prFoodItemIndexs.first;
	int32 iPreClickedIndexY = _prFoodItemIndexs.second;
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[iPreClickedIndexX][iPreClickedIndexY];

	// 데이터를 삭제합니다,
	{
		EItemType m_ePreClickedItemType = tPreInventoryItemInfo.m_eItemType;
		m_mapInventoryItemInfos.erase(m_ePreClickedItemType);
		m_arrInventorySlots[iPreClickedIndexX][iPreClickedIndexY] = {};

		m_umapInventoryNumItem.erase(m_ePreClickedItemType);
	}

	// UI 삭제합니다.
	{
		// 슬롯의 이미지를 투명 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(L"item_slot_" + std::to_wstring(iPreClickedIndexX) + std::to_wstring(iPreClickedIndexY));
		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

		// 슬롯의 숫자 텍스트를 교체합니다.
		wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iPreClickedIndexX) + std::to_wstring(iPreClickedIndexY);
		weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
		wpPreClickedItemSlotTextComponent.lock()->SetText(L"");
	}
}

void Client::CInventorySystem::ActiveSlotHover()
{	
	if (!m_wpCurHoverSlotComponent.expired())
	{
		wstring wstrCurHoverSlotComponentName = m_wpCurHoverSlotComponent.lock()->GetComponentName();

		if (wstrCurHoverSlotComponentName == L"sort_slot_none")
		{
			m_wpSortHoverImage.lock()->SetCenterPosition(m_wpCurHoverSlotComponent.lock()->GetCenterPosition());
			m_wpSortHoverImage.lock()->SetActive(true);
		}
		else if (wstrCurHoverSlotComponentName == L"drop_none" ||
				 wstrCurHoverSlotComponentName == L"discard_none")
		{
			m_wpDropDiscardHoverImage.lock()->SetCenterPosition(m_wpCurHoverSlotComponent.lock()->GetCenterPosition());
			m_wpDropDiscardHoverImage.lock()->SetActive(true);
		}
		else if (wstrCurHoverSlotComponentName == L"status_none_00" ||
				 wstrCurHoverSlotComponentName == L"status_none_10" ||
				 wstrCurHoverSlotComponentName == L"status_none_20" ||
				 wstrCurHoverSlotComponentName == L"status_none_30" ||
				 wstrCurHoverSlotComponentName == L"status_none_40" ||
				 wstrCurHoverSlotComponentName == L"status_none_50")
		{
			m_wpFeedWhoHoverImage.lock()->SetCenterPosition(m_wpCurHoverSlotComponent.lock()->GetCenterPosition());
			m_wpFeedWhoHoverImage.lock()->SetActive(true);
		}
		else
		{
			m_wpItemSlotHoverImage.lock()->SetCenterPosition(m_wpCurHoverSlotComponent.lock()->GetCenterPosition());
			m_wpItemSlotHoverImage.lock()->SetActive(true);
		}
	}
	else
	{
		m_wpSortHoverImage.lock()->SetActive(false);
		m_wpDropDiscardHoverImage.lock()->SetActive(false);
		m_wpItemSlotHoverImage.lock()->SetActive(false);
		m_wpFeedWhoHoverImage.lock()->SetActive(false);
	}
}

void Client::CInventorySystem::AddItemToContainer(EItemType _eItemType, int32 _iNumItem)
{
	// 인벤토리에 해당 아이템이 없을 경우 
	auto iter = m_mapInventoryItemInfos.find(_eItemType);
	if (iter == m_mapInventoryItemInfos.end())
	{
		int32 iFlag = 0;

		for (int32 iOuterIndex = 0; iOuterIndex < 7; iOuterIndex++)
		{
			for (int32 iInnerIndex = 0; iInnerIndex < 6; iInnerIndex++)
			{
				if (m_arrInventorySlots[iOuterIndex][iInnerIndex].m_eItemType == EItemType::ENUM_END)
				{
					// 2중 for문 탈출을 위한 flag입니다.
					iFlag = 1;

					EItemKindType eItemKindType = EItemKindType::ENUM_END;
					int32 iItemType = static_cast<int32>(_eItemType);

					if		(iItemType >= 1										&& iItemType <= (int32)EItemType::Outfit_OldCloth001 - 1) { eItemKindType = EItemKindType::WEAPON; }
					else if (iItemType >= (int32)EItemType::Outfit_OldCloth001	&& iItemType <= (int32)EItemType::PALDIUM_FRAGMENT - 1) { eItemKindType = EItemKindType::ARMOR; }
					else if (iItemType >= (int32)EItemType::PALDIUM_FRAGMENT	&& iItemType <= (int32)EItemType::EGG - 1)				{ eItemKindType = EItemKindType::MINERAL; }
					else if (iItemType >= (int32)EItemType::EGG					&& iItemType <= (int32)EItemType::CHICKEN_ROAST)		{ eItemKindType = EItemKindType::INGREDIENT; }

					switch (eItemKindType)
					{
					case EItemKindType::WEAPON:
					{
						FInventoryItemInfo tWeaponItemInfo = {};
						tWeaponItemInfo.m_eItemType = _eItemType;
						tWeaponItemInfo.m_eItemKindType = EItemKindType::WEAPON;
						tWeaponItemInfo.m_iIndexX = iOuterIndex;
						tWeaponItemInfo.m_iIndexY = iInnerIndex;
						tWeaponItemInfo.m_iNumItem = _iNumItem;
						tWeaponItemInfo.m_eWeaponType = static_cast<EWeaponType>(_eItemType);
						tWeaponItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tWeaponItemInfo.m_eItemType);
						tWeaponItemInfo.m_fAttackPower = CClientLoop::s_spClientDataSheet->k_mapWeaponDefaultAttackPowers.at(_eItemType);
						tWeaponItemInfo.m_fDurability = 5;
						tWeaponItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(_eItemType);

						m_mapInventoryItemInfos.insert({ _eItemType, tWeaponItemInfo });
						m_arrInventorySlots[iOuterIndex][iInnerIndex] = tWeaponItemInfo;

					} break;

					case EItemKindType::ARMOR:
					{
						FInventoryItemInfo tArmorItemInfo = {};
						tArmorItemInfo.m_eItemType = _eItemType;
						tArmorItemInfo.m_eItemKindType = EItemKindType::ARMOR;
						tArmorItemInfo.m_iIndexX = iOuterIndex;
						tArmorItemInfo.m_iIndexY = iInnerIndex;
						tArmorItemInfo.m_iNumItem = _iNumItem;
						tArmorItemInfo.m_eArmorType = static_cast<EArmorType>(_eItemType);
						tArmorItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tArmorItemInfo.m_eItemType);
						tArmorItemInfo.m_fDefensePower = CClientLoop::s_spClientDataSheet->k_mapArmorDefaultDefensePowers.at(_eItemType);
						tArmorItemInfo.m_fDurability = 5;
						tArmorItemInfo.m_fShieldCoolTime = 50.f;
						tArmorItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(_eItemType);

						m_mapInventoryItemInfos.insert({ _eItemType, tArmorItemInfo });
						m_arrInventorySlots[iOuterIndex][iInnerIndex] = tArmorItemInfo;

					} break;

					case EItemKindType::MINERAL:
					{
						FInventoryItemInfo tMineralItemInfo = {};
						tMineralItemInfo.m_eItemType = _eItemType;
						tMineralItemInfo.m_eItemKindType = EItemKindType::MINERAL;
						tMineralItemInfo.m_iIndexX = iOuterIndex;
						tMineralItemInfo.m_iIndexY = iInnerIndex;
						tMineralItemInfo.m_iNumItem = _iNumItem;
						tMineralItemInfo.m_eMineralType = static_cast<EMineralType>(_eItemType);
						tMineralItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tMineralItemInfo.m_eItemType);
						tMineralItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(_eItemType);

						m_mapInventoryItemInfos.insert({ _eItemType, tMineralItemInfo });
						m_arrInventorySlots[iOuterIndex][iInnerIndex] = tMineralItemInfo;

					} break;

					case EItemKindType::INGREDIENT:
					{
						FInventoryItemInfo tIngredientItemInfo = {};
						tIngredientItemInfo.m_eItemType = _eItemType;
						tIngredientItemInfo.m_eItemKindType = EItemKindType::INGREDIENT;
						tIngredientItemInfo.m_iIndexX = iOuterIndex;
						tIngredientItemInfo.m_iIndexY = iInnerIndex;
						tIngredientItemInfo.m_iNumItem = _iNumItem;
						tIngredientItemInfo.m_eIngredientType = static_cast<EIngredientType>(_eItemType);
						tIngredientItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tIngredientItemInfo.m_eItemType);
						tIngredientItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(_eItemType);
						tIngredientItemInfo.m_iNutrition = CClientLoop::s_spClientDataSheet->k_mapIngredientDefaultNutrition.at(tIngredientItemInfo.m_eIngredientType);
						
						m_mapInventoryItemInfos.insert({ _eItemType, tIngredientItemInfo });
						m_arrInventorySlots[iOuterIndex][iInnerIndex] = tIngredientItemInfo;

					} break;

					default: break;
					}

					//// slot image + slot text
					//{
					//	wstring wstrItemSlotImageComponentName = L"item_slot_" + std::to_wstring(iOuterIndex) + std::to_wstring(iInnerIndex);
					//	wstring wstrItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iOuterIndex) + std::to_wstring(iInnerIndex);

					//	weak_ptr<CImageComponent> wpItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrItemSlotImageComponentName);
					//	weak_ptr<CTextComponent> wpItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrItemSlotTextComponentName);

					//	wstring wstrItemTextureFileName = m_arrInventorySlots[iOuterIndex][iInnerIndex].m_wstrTextureFileName;
					//	wstring wstrItemSlotNumber = std::to_wstring(_iNumItem);

					//	wpItemSlotImageComponent.lock()->SetTextureFileName(wstrItemTextureFileName);
					//	wpItemSlotTextComponent.lock()->SetText(wstrItemSlotNumber);
					//}

					m_umapInventoryNumItem.insert({ _eItemType, _iNumItem });

					break;
				}
			}

			if (iFlag == 1) { break; }
		}
	}
	// 인벤토리에 해당 아이템이 있을 경우
	else
	{
		FInventoryItemInfo tMapInventoryItemInfo = iter->second;
		tMapInventoryItemInfo.m_iNumItem += _iNumItem;
		tMapInventoryItemInfo.m_fWeight += (CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(_eItemType) * tMapInventoryItemInfo.m_iNumItem);
		m_arrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY] = tMapInventoryItemInfo; // 2차원 벡터에 넣어줌으로써, map과 vector의 정보 동기화
		iter->second = tMapInventoryItemInfo;

		//// slot text
		//wstring wstrItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
		//weak_ptr<CTextComponent> wpItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrItemSlotTextComponentName);
		//wstring wstrItemSlotNumber = std::to_wstring(m_arrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem);
		//wpItemSlotTextComponent.lock()->SetText(wstrItemSlotNumber);

		m_umapInventoryNumItem[_eItemType]+= _iNumItem;
	}
}

void Client::CInventorySystem::MoveItemUI()
{ 
	if (m_wpCurLeftClickedSlotComponent.expired()) { return; }

	std::wstring wstrMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotTextureName	= m_wpCurLeftClickedSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();

	// 정렬 텍스처
	if (wstrCurLeftClickedSlotComponentName == L"sort_slot_none" && 
		wstrMouseSlotTextureName == m_wstrInvisibleTextureFileName)
	{
		SortItemsInContainer(ESortingType::ITEM_COUNT);
		m_wpCurMouseSlotComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
		m_spUIManager->SetFocusedControlComponent(nullptr);
	}
	// 버리기 텍스처
	else if (wstrCurLeftClickedSlotComponentName == L"drop_none" &&
			 wstrMouseSlotTextureName == m_wstrInvisibleTextureFileName)
	{
		int a = 0;
	}
	// 삭제 텍스처
	else if (wstrCurLeftClickedSlotComponentName == L"discard_none" &&
			 wstrMouseSlotTextureName != m_wstrInvisibleTextureFileName)
	{
		std::pair<int32, int32> prPreClickedItemSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);
		DiscardItemFromInventory(prPreClickedItemSlotIndexs);
		m_wpCurMouseSlotComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
		m_spUIManager->SetFocusedControlComponent(nullptr);
	}
	else
	{
		if (wstrMouseSlotTextureName == m_wstrInvisibleTextureFileName)
		{
			m_wstrPreClickedComponentName = wstrCurLeftClickedSlotComponentName;
			m_wstrPreClickedTextureName = wstrCurLeftClickedSlotTextureName;

			m_wpCurMouseSlotComponent.lock()->SetTextureFileName(m_wstrPreClickedTextureName);
			m_spUIManager->SetFocusedControlComponent(nullptr);
		}
		else
		{
			std::wstring wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(m_wstrPreClickedComponentName, 2);
			std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);
			std::pair<int32, int32> prPreClickedItemSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);
			std::pair<int32, int32> prCurClickedItemSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);

			/* 클릭한 슬롯에 사진이 없을 경우 */
			if (wstrCurLeftClickedSlotTextureName == m_wstrInvisibleTextureFileName)
			{
				// pre clicked component name == item_slot_
				if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
				{
					// cur clicked component name == item_slot_
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveInventorySlotUIToEmptyInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"weapon_slot_") //~3
					{
						MoveInventorySlotUIToEmptyWeaponSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"accessory_slot_")// ~1
					{
						MoveInventorySlotUIToEmptyAccessorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"food_slot_") // ~4
					{
						MoveInventorySlotUIToEmptyFoodSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentName == L"head_slot_00")
					{
						MoveInventorySlotUIToEmptyHeadSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentName == L"body_slot_00")
					{
						MoveInventorySlotUIToEmptyBodySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentName == L"shield_slot_00")
					{
						MoveInventorySlotUIToEmptyShieldSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentName == L"glider_slot_00")
					{
						MoveInventorySlotUIToEmptyGliderSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"weapon_slot_") //~3
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"weapon_slot_") //~3
					{
						MoveWeaponSlotUIToEmptyWeaponSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveWeaponSlotUIToEmptyInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"accessory_slot_")// ~1
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"accessory_slot_") //~3
					{
						MoveAccessorySlotUIToEmptyAccessorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveAccessorySlotUIToEmptyInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"food_slot_") // ~4
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"food_slot_") //~3
					{
						MoveFoodSlotUIToEmptyFoodSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if(wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveFoodSlotUIToEmptyInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (m_wstrPreClickedComponentName == L"head_slot_00")
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"head_slot_00") //~3
					{
						//MoveHeadSlotUIToEmptyHeadSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveHeadSlotUIToEmptyInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (m_wstrPreClickedComponentName == L"body_slot_00")
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"body_slot_00") //~3
					{
						//MoveBodySlotUIToEmptyBodySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveBodySlotUIToEmptyInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (m_wstrPreClickedComponentName == L"shield_slot_00")
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"shield_slot_00") //~3
					{
						//MoveShieldSlotUIToEmptyShieldSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveShieldSlotUIToEmptyInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (m_wstrPreClickedComponentName == L"glider_slot_00")
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"glider_slot_00") //~3
					{
						//MoveGliderSlotUIToEmptyGliderSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveGliderSlotUIToEmptyInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
			}

			/* 사진이 있을 경우 */
			else
			{
				// pre clicked component name == item_slot_
				if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
				{
					// cur clicked component name == item_slot_
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveInventorySlotUIToOccupiedInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"weapon_slot_") //~3
					{
						MoveInventorySlotUIToOccupiedWeaponSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"accessory_slot_")// ~1
					{
						MoveInventorySlotUIToOccupiedAccessorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"food_slot_") // ~4
					{
						MoveInventorySlotUIToOccupiedFoodSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentName == L"head_slot_00")
					{
						MoveInventorySlotUIToOccupiedHeadSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentName == L"body_slot_00")
					{
						MoveInventorySlotUIToOccupiedBodySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentName == L"shield_slot_00")
					{
						MoveInventorySlotUIToOccupiedShieldSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentName == L"glider_slot_00")
					{
						MoveInventorySlotUIToOccupiedGliderSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"weapon_slot_") //~3
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"weapon_slot_") //~3
					{
						MoveWeaponSlotUIToOccupiedWeaponSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveWeaponSlotUIToOccupiedInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"accessory_slot_")// ~1
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"accessory_slot_") //~3
					{
						MoveAccessorySlotUIToOccupiedAccessorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveAccessorySlotUIToOccupiedInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"food_slot_") // ~4
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"food_slot_") //~3
					{
						MoveFoodSlotUIToOccupiedFoodSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
					{
						MoveFoodSlotUIToOccupiedInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (m_wstrPreClickedComponentName == L"head_slot_00")
				{
					MoveHeadSlotUIToOccupiedInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
				}
				else if (m_wstrPreClickedComponentName == L"body_slot_00")
				{
					MoveBodySlotUIToOccupiedInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
				}
				else if (m_wstrPreClickedComponentName == L"shield_slot_00")
				{
					MoveShieldSlotUIToOccupiedInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
				}
				else if (m_wstrPreClickedComponentName == L"glider_slot_00")
				{
					MoveGliderSlotUIToOccupiedInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
				}
			}

			m_wpCurMouseSlotComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			m_spUIManager->SetFocusedControlComponent(nullptr);
		}
	}
}

void Client::CInventorySystem::ActiveItemInfoWindow()
{
	if (!m_wpCurHoverSlotComponent.expired())
	{
		std::wstring wstrCurHoverSlotComponentName = m_wpCurHoverSlotComponent.lock()->GetComponentName();
		std::wstring wstrCurHoverSlotTextureName = m_wpCurHoverSlotComponent.lock()->GetTextureName();
		std::wstring wstrCurMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
		std::wstring wstrCurHoverSlotComponentNameWithoutLastTwoChar = ExceptLastChar(wstrCurHoverSlotComponentName, 2);

		if (wstrCurHoverSlotTextureName != m_wstrInvisibleTextureFileName &&
			wstrCurMouseSlotTextureName == m_wstrInvisibleTextureFileName &&
			wstrCurHoverSlotTextureName != L"T_Inventory_feed_who_none")
		{
			if (wstrCurHoverSlotComponentName != L"sort_slot_none"		&&
				wstrCurHoverSlotComponentName != L"drop_none"		&&
				wstrCurHoverSlotComponentName != L"discard_none"	&&
				wstrCurHoverSlotTextureName != m_wstrInvisibleTextureFileName)
			{
				m_spItemInfoFoodParentPanel->SetActive(true);
				m_spItemInfoParentPanel->SetActive(true);
				 
				std::pair<int32, int32> prHoverSlotIndexs = ExtractLastChar(wstrCurHoverSlotComponentName, 2);
				EItemKindType eItemKindType = EItemKindType::ENUM_END;
				FInventoryItemInfo tInventoryInfo = {};

				if (wstrCurHoverSlotComponentNameWithoutLastTwoChar == L"item_slot_")
				{
					eItemKindType = m_arrInventorySlots[prHoverSlotIndexs.first][prHoverSlotIndexs.second].m_eItemKindType;
					tInventoryInfo = m_arrInventorySlots[prHoverSlotIndexs.first][prHoverSlotIndexs.second];
				}
				else if (wstrCurHoverSlotComponentNameWithoutLastTwoChar == L"weapon_slot_") //~3
				{
					eItemKindType = m_arrWeaponSlots[prHoverSlotIndexs.first].m_eItemKindType;
					tInventoryInfo = m_arrWeaponSlots[prHoverSlotIndexs.first];
				}
				else if (wstrCurHoverSlotComponentNameWithoutLastTwoChar == L"accessory_slot_")// ~1
				{
					eItemKindType = m_arrAccessorySlots[prHoverSlotIndexs.first].m_eItemKindType;
					tInventoryInfo = m_arrAccessorySlots[prHoverSlotIndexs.first];
				}
				else if (wstrCurHoverSlotComponentNameWithoutLastTwoChar == L"food_slot_") // ~4
				{
					eItemKindType = m_arrFoodSlots[prHoverSlotIndexs.first].m_eItemKindType;
					tInventoryInfo = m_arrFoodSlots[prHoverSlotIndexs.first];
				}
				else if (wstrCurHoverSlotComponentNameWithoutLastTwoChar == L"head_slot_")
				{
					eItemKindType = m_tHeadSlot.m_eItemKindType;
					tInventoryInfo = m_tHeadSlot;
				}
				else if (wstrCurHoverSlotComponentNameWithoutLastTwoChar == L"body_slot_")
				{
					eItemKindType = m_tBodySlot.m_eItemKindType;
					tInventoryInfo = m_tBodySlot;
				}
				else if (wstrCurHoverSlotComponentNameWithoutLastTwoChar == L"shield_slot_")
				{
					eItemKindType = m_tShieldSlot.m_eItemKindType;
					tInventoryInfo = m_tShieldSlot;
				}
				else if (wstrCurHoverSlotComponentNameWithoutLastTwoChar == L"glider_slot_")
				{
					eItemKindType = m_tGliderSlot.m_eItemKindType;
					tInventoryInfo = m_tGliderSlot;
				}

				if (eItemKindType == EItemKindType::INGREDIENT)
				{
					UpdateItemInfoFoodWindowPos();
					m_spItemInfoFoodParentPanel->SetActive(true);
					m_spItemInfoParentPanel->SetActive(false);

					m_spItemInfoFoodParentPanel->FindComponent<CImageComponent>(L"food_icon_00")->SetTextureFileName(tInventoryInfo.m_wstrTextureFileName);
					m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_explanation_00")->SetText(m_spUIManager->FindItemExplanation(tInventoryInfo.m_eItemType));
					m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_hunger_point_00")->SetText(std::to_wstring(tInventoryInfo.m_iNutrition));
					m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_weight_num_00")->SetText(std::to_wstring(static_cast<int32>(tInventoryInfo.m_fWeight)));
					m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_name_00")->SetText(m_spUIManager->FindItemName(tInventoryInfo.m_eItemType));
					m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_total_num_00")->SetText(std::to_wstring(tInventoryInfo.m_iNumItem));
						
				}
				else
				{
					UpdateItemInfoWindowPos();
					m_spItemInfoParentPanel->SetActive(true);
					m_spItemInfoFoodParentPanel->SetActive(false);

					m_spItemInfoParentPanel->FindComponent<CImageComponent>(L"item_icon_00")->SetTextureFileName(tInventoryInfo.m_wstrTextureFileName);
					m_spItemInfoParentPanel->FindComponent<CTextComponent>(L"item_name_00")->SetText(m_spUIManager->FindItemName(tInventoryInfo.m_eItemType));
					m_spItemInfoParentPanel->FindComponent<CTextComponent>(L"item_total_num_00")->SetText(std::to_wstring(tInventoryInfo.m_iNumItem));
					m_spItemInfoParentPanel->FindComponent<CTextComponent>(L"item_weight_num_00")->SetText(std::to_wstring(static_cast<int32>(tInventoryInfo.m_fWeight)));
					m_spItemInfoParentPanel->FindComponent<CTextComponent>(L"item_explanation_00")->SetText(m_spUIManager->FindItemExplanation(tInventoryInfo.m_eItemType));
				}
			}
		}
	}
	else
	{
		m_spItemInfoParentPanel->SetActive(false);
		m_spItemInfoFoodParentPanel->SetActive(false);
	}
}

void Client::CInventorySystem::ActiveFeedWhoWindow()
{
	if (!m_wpCurRightClickedSlotComponent.expired())
	{
		std::pair<int32, int32> prRightClickedSlotIndexs = ExtractLastChar(m_wpCurRightClickedSlotComponent.lock()->GetComponentName(), 2);

		EItemKindType eItemKindType = m_arrInventorySlots[prRightClickedSlotIndexs.first][prRightClickedSlotIndexs.second].m_eItemKindType;

		if (eItemKindType == EItemKindType::INGREDIENT)
		{
			m_spUIManager->SetTopPanel(m_spFeedWhoParentPanel);
			m_spFeedWhoParentPanel->SetActive(true);
			
			FInventoryItemInfo tIngredient = m_arrInventorySlots[prRightClickedSlotIndexs.first][prRightClickedSlotIndexs.second];
			m_spFeedWhoChildPanel->FindComponent<CImageComponent>(L"feed_who_item_icon_00")->SetTextureFileName(tIngredient.m_wstrTextureFileName);
			m_spFeedWhoChildPanel->FindComponent<CTextComponent>(L"feed_who_item_total_num_00")->SetText(std::to_wstring(tIngredient.m_iNumItem));

			ShowPartnerPalInfoOnFeedWhoPanel(); // 아이템을 보여줍니다.

			if (!m_wpCurHoverSlotComponent.expired())
			{
				wstring wstrFeedWhoPalSlot = m_wpCurHoverSlotComponent.lock()->GetComponentName();
				wstring wstrFeedWhoMainPanelWithoutLastTwoChar = ExceptLastChar(wstrFeedWhoPalSlot, 2);

				if (wstrFeedWhoMainPanelWithoutLastTwoChar == L"status_none_")
				{
					
					//std::array<FPalBoxPalInfo, 5> arrPartnerPals = (*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[0].first;

					if (wstrFeedWhoPalSlot == L"status_none_00")
					{
						if (!m_wpCurLeftClickedSlotComponent.expired())
						{
							if (m_wpCurLeftClickedSlotComponent.lock()->GetComponentName() == L"status_none_00")
							{
								float fPlayerHp = m_wpPlayer.lock()->GetCurHp() + tIngredient.m_iNutrition;
								float fPlayerHunger = m_wpPlayer.lock()->GetCurHungerGauge() + tIngredient.m_iNutrition;
								m_wpPlayer.lock()->SetCurHp(fPlayerHp);
								m_wpPlayer.lock()->SetCurHungerGauge(fPlayerHunger);

								float fHpRatio = fPlayerHp / m_wpPlayer.lock()->GetMaxHp();
								float fHungerRatio = fPlayerHunger / m_wpPlayer.lock()->GetMaxHungerGauge();
								m_wpPlayerHpBarProgressBarxComponent.lock()->SetRatio(fHpRatio);
								m_wpPlayerHpBarNumTextComponent.lock()->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHp())));
								m_wpPlayerHpNumTextComponent.lock()->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHp())));

								m_spContainerChildPanel->FindComponent<CProgressBarComponent>(L"hunger_bar_00")->SetRatio(fHungerRatio);
								m_spContainerChildPanel->FindComponent<CTextComponent>(L"hunger_bar_num_00")->SetText(std::to_wstring(static_cast<int32>(fPlayerHunger)));

								if ( m_spGameManager->GetPlayer()->GetCurHungerGauge() >= 100)
								{
									m_spGameManager->GetPlayer()->SetCurHungerGauge(100);
									m_spContainerChildPanel->FindComponent<CTextComponent>(L"hunger_bar_num_00")->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHungerGauge())));
								}
								if (m_spGameManager->GetPlayer()->GetCurHp() >= 500)
								{
									m_spGameManager->GetPlayer()->SetCurHp(500);
									m_wpPlayerHpBarNumTextComponent.lock()->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHp())));
									m_wpPlayerHpNumTextComponent.lock()->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHp())));
								}

								EItemType eItemType = m_arrInventorySlots[prRightClickedSlotIndexs.first][prRightClickedSlotIndexs.second].m_eItemType;
								ReduceItemNum(eItemType, 1);
							}
						}
					}
					else if (wstrFeedWhoPalSlot == L"status_none_10")
					{
						if ((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[0].first.m_eMonsterType != EMonsterType::ENUM_END)
						{
							m_spFeedWhoDetailInfoPanel->SetActive(true);
							PartnerPalInfoOnFeedWhoWindow((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[0].first);
							
							if (!m_wpCurLeftClickedSlotComponent.expired())
							{
								if (m_wpCurLeftClickedSlotComponent.lock()->GetComponentName() == L"status_none_10")
								{
									(*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[0].first.m_fCurHungerGauge += tIngredient.m_iNutrition;
									if ((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[0].first.m_fCurHungerGauge > 100)
									{
										(*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[0].first.m_fCurHungerGauge = 100;
									}

									EItemType eItemType = m_arrInventorySlots[prRightClickedSlotIndexs.first][prRightClickedSlotIndexs.second].m_eItemType;
									ReduceItemNum(eItemType, 1);

									m_spGameManager->GetPalBoxSystem()->SetPartnerPalInfo(0, (*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[0].first);
								}
							}
						}
						else
						{
							m_spFeedWhoDetailInfoPanel->SetActive(false);
						}
					}
					else if (wstrFeedWhoPalSlot == L"status_none_20")
					{
						if ((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[1].first.m_eMonsterType != EMonsterType::ENUM_END)
						{
							m_spFeedWhoDetailInfoPanel->SetActive(true);
							PartnerPalInfoOnFeedWhoWindow((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[1].first);

							if (!m_wpCurLeftClickedSlotComponent.expired())
							{
								if (m_wpCurLeftClickedSlotComponent.lock()->GetComponentName() == L"status_none_20")
								{
									(*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[1].first.m_fCurHungerGauge += tIngredient.m_iNutrition;
									if ((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[1].first.m_fCurHungerGauge > 100)
									{
										(*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[1].first.m_fCurHungerGauge = 100;
									}

									EItemType eItemType = m_arrInventorySlots[prRightClickedSlotIndexs.first][prRightClickedSlotIndexs.second].m_eItemType;
									ReduceItemNum(eItemType, 1);

									m_spGameManager->GetPalBoxSystem()->SetPartnerPalInfo(1, (*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[1].first);
								}
							}
						}
						else
						{
							m_spFeedWhoDetailInfoPanel->SetActive(false);
						}
					}
					else if (wstrFeedWhoPalSlot == L"status_none_30")
					{
						if ((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[2].first.m_eMonsterType != EMonsterType::ENUM_END)
						{
							m_spFeedWhoDetailInfoPanel->SetActive(true);
							PartnerPalInfoOnFeedWhoWindow((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[2].first);

							if (!m_wpCurLeftClickedSlotComponent.expired())
							{
								if (m_wpCurLeftClickedSlotComponent.lock()->GetComponentName() == L"status_none_30")
								{
									(*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[2].first.m_fCurHungerGauge += tIngredient.m_iNutrition;
									if ((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[2].first.m_fCurHungerGauge > 100)
									{
										(*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[2].first.m_fCurHungerGauge = 100;
									}

									EItemType eItemType = m_arrInventorySlots[prRightClickedSlotIndexs.first][prRightClickedSlotIndexs.second].m_eItemType;
									ReduceItemNum(eItemType, 1);

									m_spGameManager->GetPalBoxSystem()->SetPartnerPalInfo(2, (*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[2].first);
								}
							}
						}
						else
						{
							m_spFeedWhoDetailInfoPanel->SetActive(false);
						}
					}
					else if (wstrFeedWhoPalSlot == L"status_none_40")
					{
						if ((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[3].first.m_eMonsterType != EMonsterType::ENUM_END)
						{
							m_spFeedWhoDetailInfoPanel->SetActive(true);
							PartnerPalInfoOnFeedWhoWindow((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[3].first);

							if (!m_wpCurLeftClickedSlotComponent.expired())
							{
								if (m_wpCurLeftClickedSlotComponent.lock()->GetComponentName() == L"status_none_40")
								{
									(*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[3].first.m_fCurHungerGauge += tIngredient.m_iNutrition;
									if ((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[3].first.m_fCurHungerGauge > 100)
									{
										(*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[3].first.m_fCurHungerGauge = 100;
									}

									EItemType eItemType = m_arrInventorySlots[prRightClickedSlotIndexs.first][prRightClickedSlotIndexs.second].m_eItemType;
									ReduceItemNum(eItemType, 1);

									m_spGameManager->GetPalBoxSystem()->SetPartnerPalInfo(3, (*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[3].first);
								}
							}
						}
						else
						{
							m_spFeedWhoDetailInfoPanel->SetActive(false);
						}
					}
					else if (wstrFeedWhoPalSlot == L"status_none_50")
					{
						if ((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[4].first.m_eMonsterType != EMonsterType::ENUM_END)
						{
							m_spFeedWhoDetailInfoPanel->SetActive(true);
							PartnerPalInfoOnFeedWhoWindow((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[4].first);

							if (!m_wpCurLeftClickedSlotComponent.expired())
							{
								if (m_wpCurLeftClickedSlotComponent.lock()->GetComponentName() == L"status_none_50")
								{
									(*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[4].first.m_fCurHungerGauge += tIngredient.m_iNutrition;
									if ((*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[4].first.m_fCurHungerGauge > 100)
									{
										(*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[4].first.m_fCurHungerGauge = 100;
									}

									EItemType eItemType = m_arrInventorySlots[prRightClickedSlotIndexs.first][prRightClickedSlotIndexs.second].m_eItemType;
									ReduceItemNum(eItemType, 1);

									m_spGameManager->GetPalBoxSystem()->SetPartnerPalInfo(4, (*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[4].first);
								}
							}
						}
						else
						{
							m_spFeedWhoDetailInfoPanel->SetActive(false);
						}
					}
				}	
			}	
			else
			{
				m_spFeedWhoDetailInfoPanel->SetActive(false);
			}

			if (m_spInputManager->IsKeyDown(DIK_ESCAPE) ||
				tIngredient.m_iNumItem <= 0)
			{
				m_spUIManager->SetRightClickFocusedControlComponent(nullptr);
				m_spFeedWhoParentPanel->SetActive(false);
				m_spUIManager->SetTopPanel(m_spContainerParentPanel);
			}
		}
	}
	else
	{
		m_spFeedWhoParentPanel->SetActive(false);
	}
}

void Client::CInventorySystem::ExchangeInventoryItemSlotUI(const std::wstring& _wstrPreClickedItemSlotTextureName, const std::wstring& _wstrPreClickedItemSlotTextNum, const std::wstring& _wstrCurClickedItemSlotTextureName, const std::wstring& _wstrCurClickedItemSlotTextNum)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::pair<int32, int32> prCurClickedItemSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);
	std::pair<int32, int32> prPreClickedItemSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);

	// 슬롯의 이미지를 투명 텍스처로 교체합니다.
	weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
	wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(_wstrPreClickedItemSlotTextureName);

	// 슬롯의 숫자 텍스트를 교체합니다.
	wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(prPreClickedItemSlotIndexs.first) + std::to_wstring(prPreClickedItemSlotIndexs.second);
	weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
	wpPreClickedItemSlotTextComponent.lock()->SetText(_wstrPreClickedItemSlotTextNum);

	// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
	weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
	wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(_wstrCurClickedItemSlotTextureName);

	// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
	wstring wstrCurClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(prCurClickedItemSlotIndexs.first) + std::to_wstring(prCurClickedItemSlotIndexs.second);
	weak_ptr<CTextComponent> wpCurClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurClickedItemSlotTextComponentName);
	wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[prCurClickedItemSlotIndexs.first][prCurClickedItemSlotIndexs.second].m_iNumItem);
	wpCurClickedItemSlotTextComponent.lock()->SetText(_wstrCurClickedItemSlotTextNum);
}

void Client::CInventorySystem::ShowEquippedItemUI()
{
	for (int32 i = 0; i < m_arrWeaponSlots.size(); i++)
	{
		if (m_arrWeaponSlots[i].m_eWeaponType != EWeaponType::ENUM_END)
		{
			wstring wstrWeaponItemSlotComponentName = L"weapon_slot_" + std::to_wstring(i) + std::to_wstring(0);
			weak_ptr<CImageComponent> wpWeaponItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrWeaponItemSlotComponentName);
			wpWeaponItemSlotImageComponent.lock()->SetTextureFileName(m_arrWeaponSlots[i].m_wstrTextureFileName);
		}
	}
	for (int32 i = 0; i < m_arrAccessorySlots.size(); i++)
	{
		// 수정해야함
		if (m_arrAccessorySlots[i].m_eArmorType != EArmorType::ENUM_END)
		{
			wstring wstrAccessoryItemSlotComponentName = L"accessory_slot_" + std::to_wstring(i) + std::to_wstring(0);
			weak_ptr<CImageComponent> wpAccessoryItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrAccessoryItemSlotComponentName);
			wpAccessoryItemSlotImageComponent.lock()->SetTextureFileName(m_arrAccessorySlots[i].m_wstrTextureFileName);
		}
	}
	for (int32 i = 0; i < m_arrFoodSlots.size(); i++)
	{
		if (m_arrFoodSlots[i].m_eIngredientType != EIngredientType::ENUM_END)
		{
			wstring wstrFoodItemSlotComponentName = L"food_slot_" + std::to_wstring(i) + std::to_wstring(0);
			weak_ptr<CImageComponent> wpFoodItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrFoodItemSlotComponentName);
			wpFoodItemSlotImageComponent.lock()->SetTextureFileName(m_arrFoodSlots[i].m_wstrTextureFileName);
		}
	}

	if (m_tHeadSlot.m_eArmorType != EArmorType::ENUM_END)
	{
		wstring wstrHeadItemSlotComponentName = L"head_slot_00";
		weak_ptr<CImageComponent> wpHeadItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrHeadItemSlotComponentName);
		wpHeadItemSlotImageComponent.lock()->SetTextureFileName(m_tHeadSlot.m_wstrTextureFileName);
	}

	if (m_tBodySlot.m_eArmorType != EArmorType::ENUM_END)
	{
		wstring wstrBodyItemSlotComponentName = L"body_slot_00";
		weak_ptr<CImageComponent> wpBodyItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrBodyItemSlotComponentName);
		wpBodyItemSlotImageComponent.lock()->SetTextureFileName(m_tBodySlot.m_wstrTextureFileName);
	}

	if (m_tShieldSlot.m_eArmorType != EArmorType::ENUM_END)
	{
		wstring wstrShieldItemSlotComponentName = L"shield_slot_00";
		weak_ptr<CImageComponent> wpShieldItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrShieldItemSlotComponentName);
		wpShieldItemSlotImageComponent.lock()->SetTextureFileName(m_tShieldSlot.m_wstrTextureFileName);
	}

	if (m_tGliderSlot.m_eArmorType != EArmorType::ENUM_END)
	{
		wstring wstrGliderItemSlotComponentName = L"glider_slot_00";
		weak_ptr<CImageComponent> wpGliderItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrGliderItemSlotComponentName);
		wpGliderItemSlotImageComponent.lock()->SetTextureFileName(m_tGliderSlot.m_wstrTextureFileName);
	}
}

void Client::CInventorySystem::MoveEquippedItemSlotUIToInventoryItemSlotUI()
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::pair<int32, int32> prCurClickedItemSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);

	// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
	weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
	wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

	// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
	weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
	wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[prCurClickedItemSlotIndexs.first][prCurClickedItemSlotIndexs.second].m_wstrTextureFileName);

	// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
	wstring wstrCurClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(prCurClickedItemSlotIndexs.first) + std::to_wstring(prCurClickedItemSlotIndexs.second);
	weak_ptr<CTextComponent> wpCurClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurClickedItemSlotTextComponentName);
	wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[prCurClickedItemSlotIndexs.first][prCurClickedItemSlotIndexs.second].m_iNumItem);
	wpCurClickedItemSlotTextComponent.lock()->SetText(wstrCurClickedItemSlotNumber);
}

void Client::CInventorySystem::ShowPartnerPalInfoOnFeedWhoPanel()
{
	std::array<FPalBoxPalInfo, 5> arrPartnerPals;
	
	for (size_t i = 0; i < 5; i++)
	{
		arrPartnerPals[i] = (*m_spGameManager->GetPalBoxSystem()->GetPalBoxPalFromLeftPanel())[i].first;
	}

	for (int32 i = 0; i < arrPartnerPals.size() + 1; i++)
	{
		wstring wstrIndexs = std::to_wstring(i) + std::to_wstring(0);
		wstring wstrHpComponentName = L"status_hp_bar_" + wstrIndexs;
		wstring wstrHungerComponentName = L"status_hunger_bar_" + wstrIndexs;
		wstring wstrIconComponentName = L"feed_who_pal_icon_" + wstrIndexs;
		wstring wstrLevelComponentName = L"status_level_num_" + wstrIndexs;
		wstring wstrNameComponentName = L"status_name_" + wstrIndexs;
		wstring wstrStatusComponentName = L"feed_who_status_" + wstrIndexs;

		weak_ptr<CProgressBarComponent> wpHpComponent = m_spFeedWhoChildPanel->FindComponent<CProgressBarComponent>(wstrHpComponentName);
		weak_ptr<CProgressBarComponent> wpHungerComponent = m_spFeedWhoChildPanel->FindComponent<CProgressBarComponent>(wstrHungerComponentName);
		weak_ptr<CTextComponent> wpLevelComponent = m_spFeedWhoChildPanel->FindComponent<CTextComponent>(wstrLevelComponentName);
		weak_ptr<CTextComponent> wpNameComponent = m_spFeedWhoChildPanel->FindComponent<CTextComponent>(wstrNameComponentName);
		weak_ptr<CRectComponent> wpStatusComponent = m_spFeedWhoChildPanel->FindComponent<CRectComponent>(wstrStatusComponentName);

		weak_ptr<CImageComponent> wpIconComponent;

		if (i != 0)
		{
			wpIconComponent = m_spFeedWhoChildPanel->FindComponent<CImageComponent>(wstrIconComponentName);
		}

		if (i == 0)
		{
			float fHpRatio = m_wpPlayer.lock()->GetCurHp() / m_wpPlayer.lock()->GetMaxHp();
			float fHungerRatio = m_wpPlayer.lock()->GetCurHungerGauge() / m_wpPlayer.lock()->GetMaxHungerGauge();
			wpHpComponent.lock()->SetRatio(fHpRatio);
			wpHungerComponent.lock()->SetRatio(fHungerRatio);
			wpLevelComponent.lock()->SetText(std::to_wstring(m_wpPlayer.lock()->GetCurCreatureLevel()));
			wpNameComponent.lock()->SetText(L"Player");

			wpHpComponent.lock()->SetActive(true);
			wpHungerComponent.lock()->SetActive(true);
			wpLevelComponent.lock()->SetActive(true);
			wpNameComponent.lock()->SetActive(true);
			wpStatusComponent.lock()->SetActive(true);
		}
		else if ( i > 0 && arrPartnerPals[i-1].m_eMonsterType != EMonsterType::ENUM_END)
		{

			float fMaxHp = CClientLoop::s_spClientDataSheet->k_mapPalHPs[arrPartnerPals[i - 1].m_eMonsterType];
			float fMaxHunger = CClientLoop::s_spClientDataSheet->k_fMaxHungerGauge;
			float fHpRatio	   = arrPartnerPals[i-1].m_fCurHp / fMaxHp;
			float fHungerRatio = arrPartnerPals[i-1].m_fCurHungerGauge / fMaxHunger;
			wpHpComponent.lock()->SetRatio(fHpRatio);
			wpHungerComponent.lock()->SetRatio(fHungerRatio);
			wpIconComponent.lock()->SetTextureFileName(arrPartnerPals[i - 1].m_wstrTextureFileName);
			wpLevelComponent.lock()->SetText(std::to_wstring(arrPartnerPals[i - 1].m_iMonsterLevel));
			wpNameComponent.lock()->SetText(arrPartnerPals[i - 1].m_wstrMonsterName);

			wpHpComponent.lock()->SetActive(true);
			wpHungerComponent.lock()->SetActive(true);
			wpIconComponent.lock()->SetActive(true);
			wpLevelComponent.lock()->SetActive(true);
			wpNameComponent.lock()->SetActive(true);
			wpStatusComponent.lock()->SetActive(true);
		}
		else
		{
			if (i != 0)
			{
				wpIconComponent.lock()->SetActive(false);
			}
			wpHpComponent.lock()->SetActive(false);
			wpHungerComponent.lock()->SetActive(false);
			wpLevelComponent.lock()->SetActive(false);
			wpNameComponent.lock()->SetActive(false);
			wpStatusComponent.lock()->SetActive(false);
		}
	}
}

void Client::CInventorySystem::RemoveInventoryItemSlotUI()
{
	std::pair<int32, int32> prPreClickedItemSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);

	// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
	weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
	wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

	// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
	wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(prPreClickedItemSlotIndexs.first) + std::to_wstring(prPreClickedItemSlotIndexs.second);
	weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
	wstring wstrPreClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[prPreClickedItemSlotIndexs.first][prPreClickedItemSlotIndexs.second].m_iNumItem);
	wpPreClickedItemSlotTextComponent.lock()->SetText(L"");
}

void Client::CInventorySystem::ShowInventoryItemInInventorySlots()
{
	if (m_mapInventoryItemInfos.empty()) { return; }

	for (int32 iOuter = 0; iOuter < m_arrInventorySlots.size(); iOuter++)
	{
		for (int32 iInner = 0; iInner < m_arrInventorySlots[iOuter].size(); iInner++)
		{
			if (m_arrInventorySlots[iOuter][iInner].m_eItemType != EItemType::ENUM_END)
			{
				wstring wstrCurItemSlotImageComponentName = L"item_slot_" + std::to_wstring(iOuter) + std::to_wstring(iInner);
				wstring wstrCurItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iOuter) + std::to_wstring(iInner);

				weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

				wpCurItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[iOuter][iInner].m_wstrTextureFileName);
				wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(m_arrInventorySlots[iOuter][iInner].m_iNumItem));
			}
		}
	}
}

void Client::CInventorySystem::RemoveInventoryItemInInventorySlots()
{
	if (m_mapInventoryItemInfos.empty()) { return; }

	for (int32 iOuter = 0; iOuter < m_arrInventorySlots.size() ; iOuter++)
	{
		for (int32 iInner = 0; iInner < m_arrInventorySlots[iOuter].size(); iInner++)
		{
			wstring wstrCurItemSlotImageComponentName = L"item_slot_" + std::to_wstring(iOuter) + std::to_wstring(iInner);
			wstring wstrCurItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iOuter) + std::to_wstring(iInner);

			weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

			wpCurItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpCurItemSlotTextComponent.lock()->SetText(L"");
		}
	}
}

void Client::CInventorySystem::OpenInventoryWidget(int32 _iKey)
{
	if (!m_bOpenContainerWidget && m_spInputManager->IsKeyDown(_iKey))
	{
		m_wpSoundManager.lock()->PlaySoundEx("Mainmenu_Open");
		m_spGameManager->GetPlayer()->GetCameraComponent()->SetMouseControl(false);
		m_bOpenContainerWidget = !m_bOpenContainerWidget;
		m_spContainerParentPanel->SetActive(true);
		m_spFeedWhoParentPanel->SetActive(false);
		m_spUIManager->SetTopPanel(m_spContainerParentPanel);
		RemoveInventoryItemInInventorySlots();
		ShowInventoryItemInInventorySlots();
		SetPlayerStatus();
		m_wpPlayer.lock()->SetCustomInvenCustomObject(true);
	}
	else if (m_bOpenContainerWidget && m_spInputManager->IsKeyDown(_iKey))
	{
		m_wpSoundManager.lock()->PlaySoundEx("Mainmenu_Close");
		m_spGameManager->GetPlayer()->GetCameraComponent()->SetMouseControl(true);
		m_bOpenContainerWidget = !m_bOpenContainerWidget;
		m_spContainerParentPanel->SetActive(false);
		m_spFeedWhoParentPanel->SetActive(false);
		m_spUIManager->SetTopPanel(nullptr);
		RemoveInventoryItemInInventorySlots();
		m_wpPlayer.lock()->SetCustomInvenCustomObject(false);
	}
}

void Client::CInventorySystem::CalculateNumWeight()
{
	if (m_mapInventoryItemInfos.empty())
	{
		m_wpInventoryWeightBarProgressBarxComponent.lock()->SetRatio(0.0f);
		m_wpInventoryWeightBarNumTextComponent.lock()->SetText(L"0");
		m_wpInventoryWeightNumTextComponent.lock()->SetText(L"0");
	}
	else
	{
		m_fTotalWeight = 0.0f;
		for (auto& pair : m_mapInventoryItemInfos)
		{
			float fWeigth = pair.second.m_fWeight;
			int32 iNumItem = pair.second.m_iNumItem;

			m_fTotalWeight += (fWeigth * iNumItem);
		}

		if (m_fTotalWeight >= 300.0f)
		{
			m_fTotalWeight = 300.0f;
		}

		float fWeightRatio = m_fTotalWeight / 300.f;

		m_wpInventoryWeightBarProgressBarxComponent.lock()->SetRatio(fWeightRatio);
		m_wpInventoryWeightBarNumTextComponent.lock()->SetText(std::to_wstring(static_cast<int32>(m_fTotalWeight)));
		m_wpInventoryWeightNumTextComponent.lock()->SetText(std::to_wstring(static_cast<int32>(m_fTotalWeight)));
	}
}

void Client::CInventorySystem::CaculatePlayerHp()
{
	float fHpRatio = m_wpPlayer.lock()->GetCurHp() / m_wpPlayer.lock()->GetMaxHp();
	m_wpPlayerHpBarProgressBarxComponent.lock()->SetRatio(fHpRatio);
	m_wpPlayerHpBarNumTextComponent.lock()->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHp())));
	m_wpPlayerHpNumTextComponent.lock()->SetText(std::to_wstring(static_cast<int32>(m_wpPlayer.lock()->GetCurHp())));
}

void Client::CInventorySystem::UpdateItemInfoWindowPos()
{
	float fItemSlotScaleX = m_wpCurHoverSlotComponent.lock()->GetRectScale().x * 0.5f;
	float fItemSlotScaleY = m_wpCurHoverSlotComponent.lock()->GetRectScale().y * 0.5f;

	float fItemInfoSlotScaleX = m_spItemInfoParentPanel->GetPivotComponent()->GetRectScale().x * 0.5f;
	float fItemInfoSlotScaleY = m_spItemInfoParentPanel->GetPivotComponent()->GetRectScale().y * 0.5f;

	Vector2 vCursorPos = {};
	vCursorPos.x = m_wpCurHoverSlotComponent.lock()->GetCenterPosition().x + fItemSlotScaleX + fItemInfoSlotScaleX;
	vCursorPos.y = m_wpCurHoverSlotComponent.lock()->GetCenterPosition().y + fItemSlotScaleY + fItemInfoSlotScaleY;
	
	m_spItemInfoParentPanel->GetPivotComponent()->SetCenterPosition(vCursorPos);
}

void Client::CInventorySystem::UpdateItemInfoFoodWindowPos()
{
	float fItemSlotScaleX = m_wpCurHoverSlotComponent.lock()->GetRectScale().x * 0.5f;
	float fItemSlotScaleY = m_wpCurHoverSlotComponent.lock()->GetRectScale().y * 0.5f;

	float fItemInfoSlotScaleX = m_spItemInfoFoodParentPanel->GetPivotComponent()->GetRectScale().x * 0.5f;
	float fItemInfoSlotScaleY = m_spItemInfoFoodParentPanel->GetPivotComponent()->GetRectScale().y * 0.5f;

	Vector2 vCursorPos = {};
	vCursorPos.x = m_wpCurHoverSlotComponent.lock()->GetCenterPosition().x + fItemSlotScaleX + fItemInfoSlotScaleX;
	vCursorPos.y = m_wpCurHoverSlotComponent.lock()->GetCenterPosition().y + fItemSlotScaleY + fItemInfoSlotScaleY - 16.0f;

	m_spItemInfoFoodParentPanel->GetPivotComponent()->SetCenterPosition(vCursorPos);
}

void Client::CInventorySystem::PartnerPalInfoOnFeedWhoWindow(FPalBoxPalInfo _tPalBoxPalInfo)
{
	static_pointer_cast<CImageComponent>(m_vecFeedWhoDetailInfoPanel[0].lock())->SetTextureFileName(_tPalBoxPalInfo.m_wstrTextureFileName);// m_spFeedWhoDetailInfoPanel->FindComponent<CImageComponent>(L"pal_icon_00");
	static_pointer_cast<CImageComponent>(m_vecFeedWhoDetailInfoPanel[1].lock())->SetTextureFileName(m_spUIManager->FindPalElementName(_tPalBoxPalInfo.m_eMonsterElementType)); //m_spFeedWhoDetailInfoPanel->FindComponent<CImageComponent>(L"pal_element_00");
	
	float fMaxHp = CClientLoop::s_spClientDataSheet->k_mapPalHPs[_tPalBoxPalInfo.m_eMonsterType];
	float fMaxHunger = CClientLoop::s_spClientDataSheet->k_fMaxHungerGauge;
	static_pointer_cast<CProgressBarComponent>(m_vecFeedWhoDetailInfoPanel[2].lock())->SetRatio(_tPalBoxPalInfo.m_fCurExp / 100); //m_spFeedWhoDetailInfoPanel->FindComponent<CProgressBarComponent>(L"exp_bar_00");
	static_pointer_cast<CProgressBarComponent>(m_vecFeedWhoDetailInfoPanel[3].lock())->SetRatio(_tPalBoxPalInfo.m_fCurHp / fMaxHp); //m_spFeedWhoDetailInfoPanel->FindComponent<CProgressBarComponent>(L"hp_bar_00");
	static_pointer_cast<CProgressBarComponent>(m_vecFeedWhoDetailInfoPanel[4].lock())->SetRatio(_tPalBoxPalInfo.m_fCurHungerGauge / fMaxHunger); //m_spFeedWhoDetailInfoPanel->FindComponent<CProgressBarComponent>(L"hunger_bar_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[5].lock())->SetText(m_spUIManager->FindPalName(_tPalBoxPalInfo.m_eMonsterType)); //m_spFeedWhoDetailInfoPanel->FindComponent<CTextComponent>(L"pal_name_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[6].lock())->SetText(std::to_wstring(_tPalBoxPalInfo.m_iMonsterLevel));  // "pal_level_num_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[7].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fCurExp)));  // "pal_exp_num_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[8].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fCurHp)));  // "pal_hp_num_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[9].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fCurHungerGauge)));  // "pal_hunger_num_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[10].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fAttackPower))); // "pal_atk_num_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[11].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fDefensePower))); // "pal_def_num_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[12].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fWorkSpeed))); // "pal_workspeed_num_00");

	int32 iPalWork = static_cast<int32>(_tPalBoxPalInfo.m_eToDoType);
	for (int32 i = 13; i < 25; i++)
	{
		if (i == iPalWork + 13)
		{
			static_pointer_cast<CImageComponent>(m_vecFeedWhoDetailInfoPanel[i].lock())->SetActive(true);
		}
		else
		{
			static_pointer_cast<CImageComponent>(m_vecFeedWhoDetailInfoPanel[i].lock())->SetActive(false);
		}
	}
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[25].lock())->SetText(L"임무 중");
	int32 iFoodAmount = _tPalBoxPalInfo.m_iFoodAmount;

	for (int32 i = 26; i < 36; i++)
	{
		if (i <= iFoodAmount + 29)
		{
			static_pointer_cast<CImageComponent>(m_vecFeedWhoDetailInfoPanel[i].lock())->SetActive(true);
		}
		else
		{
			static_pointer_cast<CImageComponent>(m_vecFeedWhoDetailInfoPanel[i].lock())->SetActive(false);
		}
	}
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[36].lock())->SetText(_tPalBoxPalInfo.m_wstrPartnerSkillName);
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[37].lock())->SetText(std::to_wstring(_tPalBoxPalInfo.m_iPartnerSkillLevel));
	static_pointer_cast<CImageComponent>(m_vecFeedWhoDetailInfoPanel[38].lock())->SetTextureFileName(m_spUIManager->FindPalSkillElementName(_tPalBoxPalInfo.m_eActiveSkillElementA)); //"active_skill_element_00");
	static_pointer_cast<CImageComponent>(m_vecFeedWhoDetailInfoPanel[39].lock())->SetTextureFileName(m_spUIManager->FindPalSkillElementName(_tPalBoxPalInfo.m_eActiveSkillElementB)); //"active_skill_element_10");
	static_pointer_cast<CImageComponent>(m_vecFeedWhoDetailInfoPanel[40].lock())->SetTextureFileName(m_spUIManager->FindPalSkillElementName(_tPalBoxPalInfo.m_eActiveSkillElementC)); //"active_skill_element_20");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[41].lock())->SetText(std::to_wstring(_tPalBoxPalInfo.m_iActiveSkillLevelA)); //"active_skill_level_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[42].lock())->SetText(std::to_wstring(_tPalBoxPalInfo.m_iActiveSkillLevelB)); //"active_skill_level_10");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[43].lock())->SetText(std::to_wstring(_tPalBoxPalInfo.m_iActiveSkillLevelC)); //"active_skill_level_20");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[44].lock())->SetText(_tPalBoxPalInfo.m_wstrActiveSkillNameA); //"active_skill_name_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[45].lock())->SetText(_tPalBoxPalInfo.m_wstrActiveSkillNameB); //"active_skill_name_10");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[46].lock())->SetText(_tPalBoxPalInfo.m_wstrActiveSkillNameC); //"active_skill_name_20");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[47].lock())->SetText(_tPalBoxPalInfo.m_wstrPassiveSkillNameA);// "passive_skill_name_00");
	static_pointer_cast<CTextComponent>(m_vecFeedWhoDetailInfoPanel[48].lock())->SetText(_tPalBoxPalInfo.m_wstrPassiveSkillNameB);// "passive_skill_name_01");
}




void Client::CInventorySystem::MoveInventorySlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
		m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreInventoryItemInfo;
		// map 안에 있는 구조체를 바꿔줍니다.
		EItemType eCurClickedItemType = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType;
		m_mapInventoryItemInfos.find(eCurClickedItemType)->second = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];
	}
	// UI 교환합니다.
	{
		wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_iNumItem);
		ExchangeInventoryItemSlotUI(m_wstrInvisibleTextureFileName, L"", m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_wstrTextureFileName, wstrCurClickedItemSlotNumber);
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToEmptyWeaponSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::WEAPON)
	{
		int32 iIndex = -1;
		for (int32 i = 0; i < m_arrWeaponSlots.size(); i++)
		{
			if (m_arrWeaponSlots[i].m_eItemType == tPreInventoryItemInfo.m_eItemType)
			{
				iIndex = i; // 있다
			}
		}

		if (iIndex == -1)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
				m_arrWeaponSlots[_prCurClickedItemSlotIndexs.first] = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType eCurClickedItemType = m_arrWeaponSlots[_prCurClickedItemSlotIndexs.first].m_eItemType;
				m_mapInventoryItemInfos.erase(eCurClickedItemType);
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				RemoveInventoryItemSlotUI();
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_arrWeaponSlots[iIndex].m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}

		m_wpPlayer.lock()->CoumputeCurrentWeapon();
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToEmptyAccessorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::ARMOR)
	{
		int32 iIndex = -1;
		for (int32 i = 0; i < m_arrAccessorySlots.size(); i++)
		{
			if (m_arrAccessorySlots[i].m_eItemType == tPreInventoryItemInfo.m_eItemType)
			{
				iIndex = i; // 있다
			}
		}

		if (iIndex == -1)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
				m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first] = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType eCurClickedItemType = m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first].m_eItemType;
				m_mapInventoryItemInfos.erase(eCurClickedItemType);
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				RemoveInventoryItemSlotUI();
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_arrAccessorySlots[iIndex].m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToEmptyFoodSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::INGREDIENT)
	{
		int32 iIndex = -1;
		for (int32 i = 0; i < m_arrFoodSlots.size(); i++)
		{
			if (m_arrFoodSlots[i].m_eItemType == tPreInventoryItemInfo.m_eItemType)
			{
				iIndex = i; // 있다
			}
		}

		if (iIndex == -1)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
				m_arrFoodSlots[_prCurClickedItemSlotIndexs.first] = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType eCurClickedItemType = m_arrFoodSlots[_prCurClickedItemSlotIndexs.first].m_eItemType;
				m_mapInventoryItemInfos.erase(eCurClickedItemType);
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				RemoveInventoryItemSlotUI();
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_arrFoodSlots[iIndex].m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToEmptyHeadSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::ARMOR)
	{
		if (m_tHeadSlot.m_eItemType != tPreInventoryItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
				m_tHeadSlot = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType eCurClickedItemType = m_tHeadSlot.m_eItemType;
				m_mapInventoryItemInfos.erase(eCurClickedItemType);
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				RemoveInventoryItemSlotUI();
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_tHeadSlot.m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}

		m_wpPlayer.lock()->ComputeCurrentHeadEquip();
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToEmptyBodySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::ARMOR)
	{
		if (m_tBodySlot.m_eItemType != tPreInventoryItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
				m_tBodySlot = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType eCurClickedItemType = m_tBodySlot.m_eItemType;
				m_mapInventoryItemInfos.erase(eCurClickedItemType);
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				RemoveInventoryItemSlotUI();
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_tBodySlot.m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}

		m_wpPlayer.lock()->ComputeCurrentCloth();
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToEmptyShieldSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::ARMOR)
	{
		if (m_tShieldSlot.m_eItemType != tPreInventoryItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
				m_tShieldSlot = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType eCurClickedItemType = m_tShieldSlot.m_eItemType;
				m_mapInventoryItemInfos.erase(eCurClickedItemType);
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				RemoveInventoryItemSlotUI();
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_tShieldSlot.m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToEmptyGliderSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::ARMOR)
	{
		if (m_tGliderSlot.m_eItemType != tPreInventoryItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
				m_tGliderSlot = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType eCurClickedItemType = m_tGliderSlot.m_eItemType;
				m_mapInventoryItemInfos.erase(eCurClickedItemType);
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				RemoveInventoryItemSlotUI();
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_tGliderSlot.m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}
	}
}

void Client::CInventorySystem::MoveWeaponSlotUIToEmptyWeaponSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	FInventoryItemInfo tPreWeaponItemInfo = m_arrWeaponSlots[_prPreClickedItemSlotIndexs.first];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreWeaponItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreWeaponItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		m_arrWeaponSlots[_prPreClickedItemSlotIndexs.first] = {};
		m_arrWeaponSlots[_prCurClickedItemSlotIndexs.first] = tPreWeaponItemInfo;
	}
	// 이전 위치에 있는 UI를 없애줍니다.
	{
		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrWeaponSlots[_prCurClickedItemSlotIndexs.first].m_wstrTextureFileName);
	}

	m_wpPlayer.lock()->CoumputeCurrentWeapon();
}

void Client::CInventorySystem::MoveWeaponSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreWeaponItemInfo = m_arrWeaponSlots[_prPreClickedItemSlotIndexs.first];

	auto iter = m_mapInventoryItemInfos.find(tPreWeaponItemInfo.m_eItemType);
	if (iter == m_mapInventoryItemInfos.end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreWeaponItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreWeaponItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			m_arrWeaponSlots[_prPreClickedItemSlotIndexs.first] = {};
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreWeaponItemInfo;
			// map 안에 있는 구조체를 바꿔줍니다.
			EItemType eCurClickedItemType = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType;
			m_mapInventoryItemInfos.insert({ eCurClickedItemType, m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] });
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			MoveEquippedItemSlotUIToInventoryItemSlotUI();
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreWeaponItemInfo.m_iNumItem;
			m_arrWeaponSlots[_prPreClickedItemSlotIndexs.first] = {};
			m_arrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreWeaponItemInfo.m_iNumItem;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"weapon_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}

	m_wpPlayer.lock()->CoumputeCurrentWeapon();
}

void Client::CInventorySystem::MoveAccessorySlotUIToEmptyAccessorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	FInventoryItemInfo tPreAccessoryItemInfo = m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreAccessoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreAccessoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first] = {};
		m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first] = tPreAccessoryItemInfo;
	}
	// 이전 위치에 있는 UI를 없애줍니다.
	{
		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first].m_wstrTextureFileName);
	}
}

void Client::CInventorySystem::MoveAccessorySlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreAccessoryItemInfo = m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first];

	auto iter = m_mapInventoryItemInfos.find(tPreAccessoryItemInfo.m_eItemType);
	if (iter == m_mapInventoryItemInfos.end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreAccessoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreAccessoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first] = {};
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreAccessoryItemInfo;
			// map 안에 있는 구조체를 바꿔줍니다.
			EItemType eCurClickedItemType = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType;
			m_mapInventoryItemInfos.insert({ eCurClickedItemType , m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] });
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			MoveEquippedItemSlotUIToInventoryItemSlotUI();
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreAccessoryItemInfo.m_iNumItem;
			m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first] = {};
			m_arrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreAccessoryItemInfo.m_iNumItem;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"accessory_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}
}

void Client::CInventorySystem::MoveFoodSlotUIToEmptyFoodSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	FInventoryItemInfo tPreFoodItemInfo = m_arrFoodSlots[_prPreClickedItemSlotIndexs.first];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreFoodItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreFoodItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		m_arrFoodSlots[_prPreClickedItemSlotIndexs.first] = {};
		m_arrFoodSlots[_prCurClickedItemSlotIndexs.first] = tPreFoodItemInfo;
	}
	// 이전 위치에 있는 UI를 없애줍니다.
	{
		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrFoodSlots[_prCurClickedItemSlotIndexs.first].m_wstrTextureFileName);
	}
}

void Client::CInventorySystem::MoveFoodSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreFoodItemInfo = m_arrFoodSlots[_prPreClickedItemSlotIndexs.first];

	auto iter = m_mapInventoryItemInfos.find(tPreFoodItemInfo.m_eItemType);
	if (iter == m_mapInventoryItemInfos.end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreFoodItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreFoodItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			m_arrFoodSlots[_prPreClickedItemSlotIndexs.first] = {};
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreFoodItemInfo;
			// map 안에 있는 구조체를 바꿔줍니다.
			EItemType eCurClickedItemType = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType;
			m_mapInventoryItemInfos.insert({ eCurClickedItemType , m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] });
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			MoveEquippedItemSlotUIToInventoryItemSlotUI();
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreFoodItemInfo.m_iNumItem;
			m_arrFoodSlots[_prPreClickedItemSlotIndexs.first] = {};
			m_arrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreFoodItemInfo.m_iNumItem;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"food_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}
}

//void Client::CInventorySystem::MoveHeadSlotUIToEmptyHeadSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
//{
//	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
//	FInventoryItemInfo tPreHeadItemInfo = m_tHeadSlot;
//
//	// 데이터를 교환합니다.
//	{
//		// array를 교체합니다.
//		tPreHeadItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
//		tPreHeadItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
//		m_tHeadSlot = {};
//		m_tHeadSlot = tPreHeadItemInfo;
//	}
//	// 이전 위치에 있는 UI를 없애줍니다.
//	{
//		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
//		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
//		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
//
//		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
//		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
//		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_tHeadSlot.m_wstrTextureFileName);
//	}
//}

void Client::CInventorySystem::MoveHeadSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreHeadItemInfo = m_tHeadSlot;

	auto iter = m_mapInventoryItemInfos.find(tPreHeadItemInfo.m_eItemType);
	if (iter == m_mapInventoryItemInfos.end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreHeadItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreHeadItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			m_tHeadSlot = {};
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreHeadItemInfo;
			// map 안에 있는 구조체를 바꿔줍니다.
			EItemType eCurClickedItemType = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType;
			m_mapInventoryItemInfos.insert({ eCurClickedItemType , m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] });
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			MoveEquippedItemSlotUIToInventoryItemSlotUI();
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreHeadItemInfo.m_iNumItem;
			m_tHeadSlot = {};
			m_arrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreHeadItemInfo.m_iNumItem;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"head_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}

	m_wpPlayer.lock()->ComputeCurrentHeadEquip();
}

//void Client::CInventorySystem::MoveBodySlotUIToEmptyBodySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
//{
//	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
//	FInventoryItemInfo tPreBodyItemInfo = m_tBodySlot;
//
//	// 데이터를 교환합니다.
//	{
//		// array를 교체합니다.
//		tPreBodyItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
//		tPreBodyItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
//		m_tBodySlot = {};
//		m_tBodySlot = tPreBodyItemInfo;
//	}
//	// 이전 위치에 있는 UI를 없애줍니다.
//	{
//		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
//		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
//		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
//
//		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
//		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
//		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_tBodySlot.m_wstrTextureFileName);
//	}
//}

void Client::CInventorySystem::MoveBodySlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreBodyItemInfo = m_tBodySlot;

	auto iter = m_mapInventoryItemInfos.find(tPreBodyItemInfo.m_eItemType);
	if (iter == m_mapInventoryItemInfos.end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreBodyItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreBodyItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			m_tBodySlot = {};
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreBodyItemInfo;
			// map 안에 있는 구조체를 바꿔줍니다.
			EItemType eCurClickedItemType = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType;
			m_mapInventoryItemInfos.insert({ eCurClickedItemType , m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] });
		}
		{
			MoveEquippedItemSlotUIToInventoryItemSlotUI();
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreBodyItemInfo.m_iNumItem;
			m_tBodySlot = {};
			m_arrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreBodyItemInfo.m_iNumItem;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"body_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}

	m_wpPlayer.lock()->ComputeCurrentCloth();
}

//void Client::CInventorySystem::MoveShieldSlotUIToEmptyShieldSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
//{
//	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
//	FInventoryItemInfo tPreShieldItemInfo = m_tShieldSlot;
//
//	// 데이터를 교환합니다.
//	{
//		// array를 교체합니다.
//		tPreShieldItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
//		tPreShieldItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
//		m_tShieldSlot = {};
//		m_tShieldSlot = tPreShieldItemInfo;
//	}
//	// 이전 위치에 있는 UI를 없애줍니다.
//	{
//		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
//		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
//		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
//
//		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
//		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
//		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_tShieldSlot.m_wstrTextureFileName);
//	}
//}

void Client::CInventorySystem::MoveShieldSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreShieldItemInfo = m_tShieldSlot;

	auto iter = m_mapInventoryItemInfos.find(tPreShieldItemInfo.m_eItemType);
	if (iter == m_mapInventoryItemInfos.end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreShieldItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreShieldItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			m_tShieldSlot = {};
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreShieldItemInfo;
			// map 안에 있는 구조체를 바꿔줍니다.
			EItemType eCurClickedItemType = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType;
			m_mapInventoryItemInfos.insert({ eCurClickedItemType , m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] });
		}
		{
			MoveEquippedItemSlotUIToInventoryItemSlotUI();
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreShieldItemInfo.m_iNumItem;
			m_tShieldSlot = {};
			m_arrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreShieldItemInfo.m_iNumItem;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"shield_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}
}

//void Client::CInventorySystem::MoveGliderSlotUIToEmptyGliderSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
//{
//	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
//	FInventoryItemInfo tPreGliderItemInfo = m_tGliderSlot;
//
//	// 데이터를 교환합니다.
//	{
//		// array를 교체합니다.
//		tPreGliderItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
//		tPreGliderItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
//		m_tGliderSlot = {};
//		m_tGliderSlot = tPreGliderItemInfo;
//	}
//	// 이전 위치에 있는 UI를 없애줍니다.
//	{
//		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
//		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
//		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
//
//		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
//		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
//		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_tGliderSlot.m_wstrTextureFileName);
//	}
//}

void Client::CInventorySystem::MoveGliderSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreGliderItemInfo = m_tGliderSlot;

	auto iter = m_mapInventoryItemInfos.find(tPreGliderItemInfo.m_eItemType);
	if (iter == m_mapInventoryItemInfos.end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreGliderItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreGliderItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			m_tGliderSlot = {};
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreGliderItemInfo;
			// map 안에 있는 구조체를 바꿔줍니다.
			EItemType eCurClickedItemType = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType;
			m_mapInventoryItemInfos.insert({ eCurClickedItemType , m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] });
		}
		{
			MoveEquippedItemSlotUIToInventoryItemSlotUI();
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreGliderItemInfo.m_iNumItem;
			m_tGliderSlot = {};
			m_arrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreGliderItemInfo.m_iNumItem;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"glider_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}
}



void Client::CInventorySystem::MoveInventorySlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotTextureName = m_wpCurLeftClickedSlotComponent.lock()->GetTextureName();

	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];
	FInventoryItemInfo tCurInventoryItemInfo = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		tCurInventoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
		tCurInventoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
		m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurInventoryItemInfo;
		m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreInventoryItemInfo;
		// map 안에 있는 구조체를 바꿔줍니다.
		EItemType m_ePreClickedItemType = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_eItemType;
		EItemType m_eCurClickedItemType = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType;
		m_mapInventoryItemInfos.find(m_ePreClickedItemType)->second = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];
		m_mapInventoryItemInfos.find(m_eCurClickedItemType)->second = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];
	}
	// UI 교환합니다.
	{
		wstring wstrPreClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_iNumItem);
		wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_iNumItem);
		ExchangeInventoryItemSlotUI(wstrCurLeftClickedSlotTextureName, wstrPreClickedItemSlotNumber, wstrMouseSlotTextureName, wstrCurClickedItemSlotNumber);
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToOccupiedWeaponSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::WEAPON)
	{
		FInventoryItemInfo tCurWeaponItemInfo = m_arrWeaponSlots[_prCurClickedItemSlotIndexs.first];
		
		if (tPreInventoryItemInfo.m_eItemType != tCurWeaponItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				tCurWeaponItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
				tCurWeaponItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurWeaponItemInfo;
				m_arrWeaponSlots[_prCurClickedItemSlotIndexs.first] = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType m_ePreClickedInventoryItemType = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_eItemType;
				EItemType m_eCurClickedWeaponItemType = m_arrWeaponSlots[_prCurClickedItemSlotIndexs.first].m_eItemType;
				m_mapInventoryItemInfos.insert({ m_ePreClickedInventoryItemType , m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] });
				m_mapInventoryItemInfos.erase(m_eCurClickedWeaponItemType);
			}
			// UI 교환합니다.
			{
				// 슬롯의 이미지를 투명 텍스처로 교체합니다.
				weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
				wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_wstrTextureFileName);

				// 슬롯의 숫자 텍스트를 교체합니다.
				wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
				wpPreClickedItemSlotTextComponent.lock()->SetText(std::to_wstring(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_iNumItem));
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_arrWeaponSlots[_prCurClickedItemSlotIndexs.first].m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}

		m_wpPlayer.lock()->CoumputeCurrentWeapon();
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToOccupiedAccessorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::ARMOR)
	{
		FInventoryItemInfo tCurAccessoryItemInfo = m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first];

		if (tPreInventoryItemInfo.m_eItemType != tCurAccessoryItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				tCurAccessoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
				tCurAccessoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurAccessoryItemInfo;
				m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first] = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType m_ePreClickedInventoryItemType = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_eItemType;
				EItemType m_eCurClickedAccessoryItemType = m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first].m_eItemType;
				m_mapInventoryItemInfos.insert({ m_ePreClickedInventoryItemType , m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] });
				m_mapInventoryItemInfos.erase(m_eCurClickedAccessoryItemType);
			}
			// UI 교환합니다.
			{
				// 슬롯의 이미지를 투명 텍스처로 교체합니다.
				weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
				wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_wstrTextureFileName);

				// 슬롯의 숫자 텍스트를 교체합니다.
				wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
				wpPreClickedItemSlotTextComponent.lock()->SetText(std::to_wstring(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_iNumItem));
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first].m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToOccupiedFoodSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::INGREDIENT)
	{
		FInventoryItemInfo tCurIngredientItemInfo = m_arrFoodSlots[_prCurClickedItemSlotIndexs.first];

		if (tPreInventoryItemInfo.m_eItemType != tCurIngredientItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				tCurIngredientItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
				tCurIngredientItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurIngredientItemInfo;
				m_arrFoodSlots[_prCurClickedItemSlotIndexs.first] = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType m_ePreClickedInventoryItemType = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_eItemType;
				EItemType m_eCurClickedFoodItemType = m_arrFoodSlots[_prCurClickedItemSlotIndexs.first].m_eItemType;
				m_mapInventoryItemInfos.insert({ m_ePreClickedInventoryItemType , m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] });
				m_mapInventoryItemInfos.erase(m_eCurClickedFoodItemType);
			}
			// UI 교환합니다.
			{
				// 슬롯의 이미지를 투명 텍스처로 교체합니다.
				weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
				wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_wstrTextureFileName);

				// 슬롯의 숫자 텍스트를 교체합니다.
				wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
				wpPreClickedItemSlotTextComponent.lock()->SetText(std::to_wstring(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_iNumItem));
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_arrFoodSlots[_prCurClickedItemSlotIndexs.first].m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToOccupiedHeadSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::ARMOR)
	{
		FInventoryItemInfo tCurHeadItemInfo = m_tHeadSlot;

		if (tPreInventoryItemInfo.m_eItemType != tCurHeadItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				tCurHeadItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
				tCurHeadItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurHeadItemInfo;
				m_tHeadSlot = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType m_ePreClickedInventoryItemType = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_eItemType;
				EItemType m_eCurClickedHeadItemType = m_tHeadSlot.m_eItemType;
				m_mapInventoryItemInfos.insert({ m_ePreClickedInventoryItemType , m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] });
				m_mapInventoryItemInfos.erase(m_eCurClickedHeadItemType);
			}
			// UI 교환합니다.
			{
				// 슬롯의 이미지를 투명 텍스처로 교체합니다.
				weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
				wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_wstrTextureFileName);

				// 슬롯의 숫자 텍스트를 교체합니다.
				wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
				wpPreClickedItemSlotTextComponent.lock()->SetText(std::to_wstring(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_iNumItem));
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_tHeadSlot.m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}

		m_wpPlayer.lock()->ComputeCurrentHeadEquip();
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToOccupiedBodySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::ARMOR)
	{
		FInventoryItemInfo tCurBodyItemInfo = m_tBodySlot;

		if (tPreInventoryItemInfo.m_eItemType != tCurBodyItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				tCurBodyItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
				tCurBodyItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurBodyItemInfo;
				m_tBodySlot = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType m_ePreClickedInventoryItemType = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_eItemType;
				EItemType m_eCurClickedBodyItemType = m_tBodySlot.m_eItemType;
				m_mapInventoryItemInfos.insert({ m_ePreClickedInventoryItemType , m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] });
				m_mapInventoryItemInfos.erase(m_eCurClickedBodyItemType);
			}
			// UI 교환합니다.
			{
				// 슬롯의 이미지를 투명 텍스처로 교체합니다.
				weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
				wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_wstrTextureFileName);

				// 슬롯의 숫자 텍스트를 교체합니다.
				wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
				wpPreClickedItemSlotTextComponent.lock()->SetText(std::to_wstring(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_iNumItem));
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_tBodySlot.m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}

		m_wpPlayer.lock()->ComputeCurrentCloth();
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToOccupiedShieldSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::ARMOR)
	{
		FInventoryItemInfo tCurShieldItemInfo = m_tShieldSlot;

		if (tPreInventoryItemInfo.m_eItemType != tCurShieldItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				tCurShieldItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
				tCurShieldItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurShieldItemInfo;
				m_tShieldSlot = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType m_ePreClickedInventoryItemType = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_eItemType;
				EItemType m_eCurClickedShieldItemType = m_tShieldSlot.m_eItemType;
				m_mapInventoryItemInfos.insert({ m_ePreClickedInventoryItemType , m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] });
				m_mapInventoryItemInfos.erase(m_eCurClickedShieldItemType);
			}
			// UI 교환합니다.
			{
				// 슬롯의 이미지를 투명 텍스처로 교체합니다.
				weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
				wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_wstrTextureFileName);

				// 슬롯의 숫자 텍스트를 교체합니다.
				wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
				wpPreClickedItemSlotTextComponent.lock()->SetText(std::to_wstring(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_iNumItem));
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_tShieldSlot.m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}
	}
}

void Client::CInventorySystem::MoveInventorySlotUIToOccupiedGliderSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreInventoryItemInfo = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	if (tPreInventoryItemInfo.m_eItemKindType == EItemKindType::WEAPON)
	{
		FInventoryItemInfo tCurGliderItemInfo = m_tGliderSlot;

		if (tPreInventoryItemInfo.m_eItemType != tCurGliderItemInfo.m_eItemType)
		{
			// 데이터를 교환합니다.
			{
				// array를 교체합니다.
				tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
				tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
				tCurGliderItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
				tCurGliderItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurGliderItemInfo;
				m_tGliderSlot = tPreInventoryItemInfo;
				// map 안에 있는 구조체를 바꿔줍니다.
				EItemType m_ePreClickedInventoryItemType = m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_eItemType;
				EItemType m_eCurClickedGliderItemType = m_tGliderSlot.m_eItemType;
				m_mapInventoryItemInfos.insert({ m_ePreClickedInventoryItemType , m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] });
				m_mapInventoryItemInfos.erase(m_eCurClickedGliderItemType);
			}
			// UI 교환합니다.
			{
				// 슬롯의 이미지를 투명 텍스처로 교체합니다.
				weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
				wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_wstrTextureFileName);

				// 슬롯의 숫자 텍스트를 교체합니다.
				wstring wstrPreClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
				wpPreClickedItemSlotTextComponent.lock()->SetText(std::to_wstring(m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_iNumItem));
			}
		}
		else
		{
			// 데이터를 교환합니다.
			{
				m_tGliderSlot.m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
				m_mapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
				m_arrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
			}
			// 이전 위치에 있는 UI를 없애줍니다.
			{
				// pre clicked slot
				wstring wstrPreItemSlotImageComponentName = L"item_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				wstring wstrPreItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
				weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpPreItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
				wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
				wpPreItemSlotTextComponent.lock()->SetText(L"");
			}
		}
	}
}

void Client::CInventorySystem::MoveWeaponSlotUIToOccupiedWeaponSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreWeaponItemInfo = m_arrWeaponSlots[_prPreClickedItemSlotIndexs.first];
	FInventoryItemInfo tCurWeaponItemInfo = m_arrWeaponSlots[_prCurClickedItemSlotIndexs.first];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreWeaponItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreWeaponItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		tCurWeaponItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
		tCurWeaponItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
		m_arrWeaponSlots[_prPreClickedItemSlotIndexs.first] = tCurWeaponItemInfo;
		m_arrWeaponSlots[_prCurClickedItemSlotIndexs.first] = tPreWeaponItemInfo;
	}

	m_wpPlayer.lock()->CoumputeCurrentWeapon();
}

void Client::CInventorySystem::MoveWeaponSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	FInventoryItemInfo tPreWeaponItemInfo = m_arrWeaponSlots[_prPreClickedItemSlotIndexs.first];
	FInventoryItemInfo tCurInventoryItemInfo = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreWeaponItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreWeaponItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		tCurInventoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
		tCurInventoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
		m_arrWeaponSlots[_prPreClickedItemSlotIndexs.first] = tCurInventoryItemInfo;
		m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreWeaponItemInfo;
	}
	// 이전 위치에 있는 UI를 없애줍니다.
	{
		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrWeaponSlots[_prPreClickedItemSlotIndexs.first].m_wstrTextureFileName);

		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_wstrTextureFileName);

		// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
		wstring wstrCurClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
		weak_ptr<CTextComponent> wpCurClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurClickedItemSlotTextComponentName);
		wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_iNumItem);
		wpCurClickedItemSlotTextComponent.lock()->SetText(wstrCurClickedItemSlotNumber);
	}

	m_wpPlayer.lock()->CoumputeCurrentWeapon();
}

void Client::CInventorySystem::MoveAccessorySlotUIToOccupiedAccessorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	FInventoryItemInfo tPreAccessoryItemInfo = m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first];
	FInventoryItemInfo tCurAccessoryItemInfo = m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreAccessoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreAccessoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		tCurAccessoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
		tCurAccessoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
		m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first] = tCurAccessoryItemInfo;
		m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first] = tPreAccessoryItemInfo;
	}
	// 이전 위치에 있는 UI랑 바꿔줍니다.
	{
		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first].m_wstrTextureFileName);

		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrAccessorySlots[_prCurClickedItemSlotIndexs.first].m_wstrTextureFileName);
	}
}

void Client::CInventorySystem::MoveAccessorySlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	FInventoryItemInfo tPreAccessoryItemInfo = m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first];
	FInventoryItemInfo tCurInventoryItemInfo = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreAccessoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreAccessoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		tCurInventoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
		tCurInventoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
		m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first] = tCurInventoryItemInfo;
		m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreAccessoryItemInfo;
	}
	// 이전 위치에 있는 UI를 없애줍니다.
	{
		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrAccessorySlots[_prPreClickedItemSlotIndexs.first].m_wstrTextureFileName);

		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_wstrTextureFileName);

		// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
		wstring wstrCurClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
		weak_ptr<CTextComponent> wpCurClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurClickedItemSlotTextComponentName);
		wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_iNumItem);
		wpCurClickedItemSlotTextComponent.lock()->SetText(wstrCurClickedItemSlotNumber);
	}
}

void Client::CInventorySystem::MoveFoodSlotUIToOccupiedFoodSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	FInventoryItemInfo tPreFoodItemInfo = m_arrFoodSlots[_prPreClickedItemSlotIndexs.first];
	FInventoryItemInfo tCurFoodItemInfo = m_arrFoodSlots[_prCurClickedItemSlotIndexs.first];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreFoodItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreFoodItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		tCurFoodItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
		tCurFoodItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
		m_arrFoodSlots[_prPreClickedItemSlotIndexs.first] = tCurFoodItemInfo;
		m_arrFoodSlots[_prCurClickedItemSlotIndexs.first] = tPreFoodItemInfo;
	}
	// 이전 위치에 있는 UI랑 바꿔줍니다.
	{
		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrFoodSlots[_prPreClickedItemSlotIndexs.first].m_wstrTextureFileName);

		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrFoodSlots[_prCurClickedItemSlotIndexs.first].m_wstrTextureFileName);
	}
}

void Client::CInventorySystem::MoveFoodSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	FInventoryItemInfo tPreFoodItemInfo = m_arrFoodSlots[_prPreClickedItemSlotIndexs.first];
	FInventoryItemInfo tCurInventoryItemInfo = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreFoodItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreFoodItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		tCurInventoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
		tCurInventoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
		m_arrFoodSlots[_prPreClickedItemSlotIndexs.first] = tCurInventoryItemInfo;
		m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreFoodItemInfo;
	}
	// 이전 위치에 있는 UI를 없애줍니다.
	{
		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrFoodSlots[_prPreClickedItemSlotIndexs.first].m_wstrTextureFileName);

		// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_wstrTextureFileName);

		// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
		wstring wstrCurClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
		weak_ptr<CTextComponent> wpCurClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurClickedItemSlotTextComponentName);
		wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_iNumItem);
		wpCurClickedItemSlotTextComponent.lock()->SetText(wstrCurClickedItemSlotNumber);
	}
}

void Client::CInventorySystem::MoveHeadSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);
	FInventoryItemInfo tPreHeadItemInfo = m_tHeadSlot;

	if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
	{
		FInventoryItemInfo tCurInventoryItemInfo = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreHeadItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreHeadItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			tCurInventoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
			tCurInventoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
			m_tHeadSlot = tCurInventoryItemInfo;
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = m_tHeadSlot;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
			weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
			wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_tHeadSlot.m_wstrTextureFileName);

			// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
			weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
			wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_wstrTextureFileName);

			// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
			wstring wstrCurClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
			weak_ptr<CTextComponent> wpCurClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurClickedItemSlotTextComponentName);
			wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_iNumItem);
			wpCurClickedItemSlotTextComponent.lock()->SetText(wstrCurClickedItemSlotNumber);
		}

		m_wpPlayer.lock()->ComputeCurrentHeadEquip();
	}
}

void Client::CInventorySystem::MoveBodySlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);
	FInventoryItemInfo tPreBodyItemInfo = m_tBodySlot;

	if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
	{
		FInventoryItemInfo tCurInventoryItemInfo = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreBodyItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreBodyItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			tCurInventoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
			tCurInventoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
			m_tBodySlot = tCurInventoryItemInfo;
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreBodyItemInfo;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
			weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
			wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_tBodySlot.m_wstrTextureFileName);

			// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
			weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
			wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_wstrTextureFileName);

			// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
			wstring wstrCurClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
			weak_ptr<CTextComponent> wpCurClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurClickedItemSlotTextComponentName);
			wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_iNumItem);
			wpCurClickedItemSlotTextComponent.lock()->SetText(wstrCurClickedItemSlotNumber);
		}

		m_wpPlayer.lock()->ComputeCurrentCloth();
	}
}

void Client::CInventorySystem::MoveShieldSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);
	FInventoryItemInfo tPreShieldItemInfo = m_tShieldSlot;

	if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
	{
		FInventoryItemInfo tCurInventoryItemInfo = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreShieldItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreShieldItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			tCurInventoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
			tCurInventoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
			m_tShieldSlot = tCurInventoryItemInfo;
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreShieldItemInfo;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
			weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
			wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_tShieldSlot.m_wstrTextureFileName);

			// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
			weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
			wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_wstrTextureFileName);

			// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
			wstring wstrCurClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
			weak_ptr<CTextComponent> wpCurClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurClickedItemSlotTextComponentName);
			wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_iNumItem);
			wpCurClickedItemSlotTextComponent.lock()->SetText(wstrCurClickedItemSlotNumber);
		}
	}
}

void Client::CInventorySystem::MoveGliderSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);

	FInventoryItemInfo tPreGliderItemInfo = m_tGliderSlot;

	if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"item_slot_")
	{
		FInventoryItemInfo tCurInventoryItemInfo = m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreGliderItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreGliderItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			tCurInventoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
			tCurInventoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
			m_tGliderSlot = tCurInventoryItemInfo;
			m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreGliderItemInfo;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
			weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
			wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_tGliderSlot.m_wstrTextureFileName);

			// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
			weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
			wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_wstrTextureFileName);

			// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
			wstring wstrCurClickedItemSlotTextComponentName = L"item_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
			weak_ptr<CTextComponent> wpCurClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrCurClickedItemSlotTextComponentName);
			wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_arrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_iNumItem);
			wpCurClickedItemSlotTextComponent.lock()->SetText(wstrCurClickedItemSlotNumber);
		}
	}
}
