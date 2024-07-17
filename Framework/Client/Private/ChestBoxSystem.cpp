#include "pch.h"
#include "ChestBoxSystem.h"

// manager
#include "ClientLoop.h"
#include "GameManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "SoundManager.h"

// system
#include "InventorySystem.h"

// gameObject
#include "Player.h"
#include "Panel.h"
#include "Item.h"
#include "Weapon.h"
#include "Armor.h"
#include "Mineral.h"
#include "Ingredient.h"

// component
#include "ProgressBarComponent.h"
#include "CameraComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"

std::shared_ptr<CChestBoxSystem> Client::CChestBoxSystem::Create(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc)
{
	shared_ptr<CChestBoxSystem> spInstance = make_shared<CChestBoxSystem>();
	CHECKF(spInstance, L"CChestBoxSystem : Create Failed");
	spInstance->Initialize(_spContainerSystemDesc);

	return spInstance;
}

HRESULT Client::CChestBoxSystem::Initialize(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc)
{
	if (FAILED(CContainerSystem::Initialize(_spContainerSystemDesc))) { return E_FAIL; }

	// parent
	m_spContainerParentPanel	= m_spUIManager->FindParentsPanel(L"WoodenChestPanel");
	m_spItemInfoParentPanel		= m_spUIManager->FindParentsPanel(L"InventoryItemInfo");
	m_spItemInfoFoodParentPanel = m_spUIManager->FindParentsPanel(L"InventoryItemInfoFood");

	// child
	m_spInventorySlotChildPanel   = m_spUIManager->FindChildPanel(L"WoodenChestPanel", L"Inventory_slot");
	m_spWoodenChestSlotChildPanel = m_spUIManager->FindChildPanel(L"WoodenChestPanel", L"WoodenChest_slot");

	// component
	m_wpCurMouseSlotComponent = m_spContainerParentPanel->FindComponent<CImageComponent>(L"mouse_slot");
	m_wpItemSlotHoverImage    = m_spContainerParentPanel->FindComponent<CImageComponent>(L"item_slot_hover");
	m_wpSortHoverImage		  = m_spContainerParentPanel->FindComponent<CImageComponent>(L"sort_slot_hover");

	m_mapChestBoxs.insert({ EArchitectureType::IRON_BOX, {} });
	m_mapChestBoxs.insert({ EArchitectureType::WOODEN_CHEST, {} }); 
	m_mapChestBoxsSlots.insert({ EArchitectureType::IRON_BOX, {} });
	m_mapChestBoxsSlots.insert({ EArchitectureType::WOODEN_CHEST, {} }); 

	return S_OK;
}

HRESULT Client::CChestBoxSystem::BeginPlay()
{
	if (FAILED(CContainerSystem::BeginPlay())) { return E_FAIL; }
	
	// parent panel
	//weight_bar

	//// inventoryChildPanel
		//inventory_slot_00
	//	inventory_slot_num_03
		//inventory_sort_none
		//weight_num_00


	return S_OK;
}

int32 Client::CChestBoxSystem::PreTick(float _fDeltaSeconds)
{
	switch (CContainerSystem::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CChestBoxSystem::Tick(float _fDeltaSeconds)
{
	switch (CContainerSystem::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	if (m_spInputManager->IsKeyDown(DIK_I))
	{
		MakeChestBox(EArchitectureType::IRON_BOX, 1);
		MakeChestBox(EArchitectureType::WOODEN_CHEST, 2);
		MakeChestBox(EArchitectureType::WOODEN_CHEST, 1);

		EItemType eItemType = EItemType::BOAR_MEAT;
		InsertItemToChestBox(EArchitectureType::WOODEN_CHEST, 2, eItemType, 5);
	}

	//OpenSpecificChestBox(EArchitectureType::WOODEN_CHEST, 2, DIK_H);

	if (m_spUIManager->GetTopPanel() == GetContainerParentPanel() ||
		m_spUIManager->GetTopPanel() == m_spGameManager->GetInventorySystem()->GetContainerParentPanel())
	{
		UpdateCurMouseSlotPos();
		ActiveItemInfoWindow(); 
		ActiveSlotHover();
		MoveItemUI();
	}
	else if (m_spUIManager->GetTopPanel() != GetContainerParentPanel() ||
			 m_spUIManager->GetTopPanel() != m_spGameManager->GetInventorySystem()->GetContainerParentPanel())
	{
		m_spItemInfoParentPanel->SetActive(false);
		m_spItemInfoFoodParentPanel->SetActive(false);
	}

	return 0;
}

int32 Client::CChestBoxSystem::PostTick(float _fDeltaSeconds)
{
	switch (CContainerSystem::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CChestBoxSystem::EndPlay()
{
	if (FAILED(CContainerSystem::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CChestBoxSystem::Release()
{
	CContainerSystem::Release();
}

void Client::CChestBoxSystem::AddItemToContainer(EArchitectureType _eArchitectureType, int32 _iWoodenChestID, const std::shared_ptr<CItem>& _spItem)
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& arrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();

	if (_spItem == nullptr) { return; }

	FInventoryItemInfo tInventoryItemInfo = {};
	EItemKindType eItemKindType = _spItem->GetItemKindType();

	switch (eItemKindType)
	{
	case EItemKindType::WEAPON:
	{
		weak_ptr<CWeapon> wpWeapon = static_pointer_cast<CWeapon>(_spItem);
		FInventoryItemInfo tWeaponItemInfo = {};
		tWeaponItemInfo.m_eItemType = wpWeapon.lock()->GetItemType();
		tWeaponItemInfo.m_eItemKindType = EItemKindType::WEAPON;
		tWeaponItemInfo.m_iIndexX = 0;
		tWeaponItemInfo.m_iIndexY = 0;
		tWeaponItemInfo.m_iNumItem = wpWeapon.lock()->GetNumItems();
		tWeaponItemInfo.m_eWeaponType = wpWeapon.lock()->GetWeaponType();
		tWeaponItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tWeaponItemInfo.m_eItemType);
		tWeaponItemInfo.m_fAttackPower = wpWeapon.lock()->GetAttackPower();
		tWeaponItemInfo.m_fDurability = 5;
		tWeaponItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(tWeaponItemInfo.m_eItemType);

		tInventoryItemInfo = tWeaponItemInfo;
	} break;

	case EItemKindType::ARMOR:
	{
		weak_ptr<CArmor> wpArmor = static_pointer_cast<CArmor>(_spItem);
		FInventoryItemInfo tArmorItemInfo = {};
		tArmorItemInfo.m_eItemType = wpArmor.lock()->GetItemType();
		tArmorItemInfo.m_eItemKindType = EItemKindType::ARMOR;
		tArmorItemInfo.m_iIndexX = 0;
		tArmorItemInfo.m_iIndexY = 0;
		tArmorItemInfo.m_iNumItem = wpArmor.lock()->GetNumItems();
		tArmorItemInfo.m_eArmorType = wpArmor.lock()->GetCurArmorType();
		tArmorItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tArmorItemInfo.m_eItemType);
		tArmorItemInfo.m_fDefensePower = wpArmor.lock()->GetDefensePower();
		tArmorItemInfo.m_fDurability = 5;
		tArmorItemInfo.m_fShieldCoolTime = wpArmor.lock()->GetDefenseCoolTime();
		tArmorItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(tArmorItemInfo.m_eItemType);

		tInventoryItemInfo = tArmorItemInfo;
	} break;

	case EItemKindType::MINERAL:
	{
		weak_ptr<CMineral> wpMineral = static_pointer_cast<CMineral>(_spItem);
		FInventoryItemInfo tMineralItemInfo = {};
		tMineralItemInfo.m_eItemType = wpMineral.lock()->GetItemType();
		tMineralItemInfo.m_eItemKindType = EItemKindType::MINERAL;
		tMineralItemInfo.m_iIndexX = 0;
		tMineralItemInfo.m_iIndexY = 0;
		tMineralItemInfo.m_iNumItem = wpMineral.lock()->GetNumItems();
		tMineralItemInfo.m_eMineralType = wpMineral.lock()->GetMineralType();
		tMineralItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tMineralItemInfo.m_eItemType);
		tMineralItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(tMineralItemInfo.m_eItemType);

		tInventoryItemInfo = tMineralItemInfo;
	} break;

	case EItemKindType::INGREDIENT:
	{
		weak_ptr<CIngredient> wpIngredient = static_pointer_cast<CIngredient>(_spItem);
		FInventoryItemInfo tIngredientItemInfo = {};
		tIngredientItemInfo.m_eItemType = wpIngredient.lock()->GetItemType();
		tIngredientItemInfo.m_eItemKindType = EItemKindType::INGREDIENT;
		tIngredientItemInfo.m_iIndexX = 0;
		tIngredientItemInfo.m_iIndexY = 0;
		tIngredientItemInfo.m_iNumItem = wpIngredient.lock()->GetNumItems();
		tIngredientItemInfo.m_eIngredientType = wpIngredient.lock()->GetIngredientType();
		tIngredientItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tIngredientItemInfo.m_eItemType);
		tIngredientItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(tIngredientItemInfo.m_eItemType);
		tIngredientItemInfo.m_iNutrition = CClientLoop::s_spClientDataSheet->k_mapIngredientDefaultNutrition.at(tIngredientItemInfo.m_eIngredientType);

		tInventoryItemInfo = tIngredientItemInfo;
	} break;

	default: break;
	}

	_spItem->Release();

	EItemType eItemType = tInventoryItemInfo.m_eItemType;

	// 나무 상자는 있는데, 나무 상자 안에 해당 아이템이 없을 경우
	auto iter = m_mapChestBoxs[_eArchitectureType][_iWoodenChestID].find(eItemType);
	if (iter == m_mapChestBoxs[_eArchitectureType][_iWoodenChestID].end())
	{
		int32 iFlag = 0;

		for (int32 iOuterIndex = 0; iOuterIndex < 7; iOuterIndex++)
		{
			for (int32 iInnerIndex = 0; iInnerIndex < 6; iInnerIndex++)
			{
				if (m_mapChestBoxsSlots[_eArchitectureType][_iWoodenChestID][iOuterIndex][iInnerIndex].m_eItemType == EItemType::ENUM_END)
				{
					// 2중 for문 탈출을 위한 flag입니다.
					iFlag = 1;

					tInventoryItemInfo.m_iIndexX = iOuterIndex;
					tInventoryItemInfo.m_iIndexY = iInnerIndex;

					m_mapChestBoxs[_eArchitectureType][_iWoodenChestID].insert({ {tInventoryItemInfo.m_eItemType, tInventoryItemInfo} });
					m_mapChestBoxsSlots[_eArchitectureType][_iWoodenChestID][iOuterIndex][iInnerIndex] = tInventoryItemInfo;

					break;
				}
			}

			if (iFlag == 1) { break; }
		}
	}
	// 나무 상자는 있는데, 나무 상자 안에 해당 아이템이 있는 경우
	else
	{
		FInventoryItemInfo tMapInventoryItemInfo = iter->second;
		tMapInventoryItemInfo.m_iNumItem += tInventoryItemInfo.m_iNumItem;
		tMapInventoryItemInfo.m_fWeight += (CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(eItemType) * tMapInventoryItemInfo.m_iNumItem);
		m_mapChestBoxsSlots[_eArchitectureType][_iWoodenChestID][iter->second.m_iIndexX][iter->second.m_iIndexY] = tMapInventoryItemInfo; // 2차원 벡터에 넣어줌으로써, map과 vector의 정보 동기화
		iter->second = tMapInventoryItemInfo;
	}
}

void Client::CChestBoxSystem::InsertItemToChestBox(EArchitectureType _eArchitectureType, int32 _iWoodenChestID, EItemType _eItemType, int32 _iNumItem)
{
	FInventoryItemInfo tInventoryItemInfo = {};
	EItemKindType eItemKindType = EItemKindType::ENUM_END;
	int32 iItemType = static_cast<int32>(_eItemType);

	if (iItemType >= 1 && iItemType <= 12) { eItemKindType = EItemKindType::WEAPON; }
	else if (iItemType >= 13 && iItemType <= 25) { eItemKindType = EItemKindType::ARMOR; }
	else if (iItemType >= 26 && iItemType <= 39) { eItemKindType = EItemKindType::MINERAL; }
	else if (iItemType >= 40 && iItemType <= 49) { eItemKindType = EItemKindType::INGREDIENT; }

	switch (eItemKindType)
	{
	case EItemKindType::WEAPON:
	{
		FInventoryItemInfo tWeaponItemInfo = {};
		tWeaponItemInfo.m_eItemType = _eItemType;
		tWeaponItemInfo.m_eItemKindType = EItemKindType::WEAPON;
		tWeaponItemInfo.m_iIndexX = 0;
		tWeaponItemInfo.m_iIndexY = 0;
		tWeaponItemInfo.m_iNumItem = _iNumItem;
		tWeaponItemInfo.m_eWeaponType = static_cast<EWeaponType>(_eItemType);
		tWeaponItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tWeaponItemInfo.m_eItemType);
		tWeaponItemInfo.m_fAttackPower = CClientLoop::s_spClientDataSheet->k_mapWeaponDefaultAttackPowers.at(_eItemType);
		tWeaponItemInfo.m_fDurability = 5;
		tWeaponItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(_eItemType);

		tInventoryItemInfo = tWeaponItemInfo;
	} break;

	case EItemKindType::ARMOR:
	{
		FInventoryItemInfo tArmorItemInfo = {};
		tArmorItemInfo.m_eItemType = _eItemType;
		tArmorItemInfo.m_eItemKindType = EItemKindType::ARMOR;
		tArmorItemInfo.m_iIndexX = 0;
		tArmorItemInfo.m_iIndexY = 0;
		tArmorItemInfo.m_iNumItem = _iNumItem;
		tArmorItemInfo.m_eArmorType = static_cast<EArmorType>(_eItemType);
		tArmorItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tArmorItemInfo.m_eItemType);
		tArmorItemInfo.m_fDefensePower = CClientLoop::s_spClientDataSheet->k_mapArmorDefaultDefensePowers.at(_eItemType);
		tArmorItemInfo.m_fDurability = 5;
		tArmorItemInfo.m_fShieldCoolTime = 50.f;
		tArmorItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(_eItemType);

		tInventoryItemInfo = tArmorItemInfo;
	} break;

	case EItemKindType::MINERAL:
	{
		FInventoryItemInfo tMineralItemInfo = {};
		tMineralItemInfo.m_eItemType = _eItemType;
		tMineralItemInfo.m_eItemKindType = EItemKindType::MINERAL;
		tMineralItemInfo.m_iIndexX = 0;
		tMineralItemInfo.m_iIndexY = 0;
		tMineralItemInfo.m_iNumItem = _iNumItem;
		tMineralItemInfo.m_eMineralType = static_cast<EMineralType>(_eItemType);
		tMineralItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tMineralItemInfo.m_eItemType);
		tMineralItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(_eItemType);

		tInventoryItemInfo = tMineralItemInfo;
	} break;

	case EItemKindType::INGREDIENT:
	{
		FInventoryItemInfo tIngredientItemInfo = {};
		tIngredientItemInfo.m_eItemType = _eItemType;
		tIngredientItemInfo.m_eItemKindType = EItemKindType::INGREDIENT;
		tIngredientItemInfo.m_iIndexX = 0;
		tIngredientItemInfo.m_iIndexY = 0;
		tIngredientItemInfo.m_iNumItem = _iNumItem;
		tIngredientItemInfo.m_eIngredientType = static_cast<EIngredientType>(_eItemType);
		tIngredientItemInfo.m_wstrTextureFileName = m_spUIManager->FindItemUITextureName(tIngredientItemInfo.m_eItemType);
		tIngredientItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(_eItemType);
		tIngredientItemInfo.m_iNutrition = CClientLoop::s_spClientDataSheet->k_mapIngredientDefaultNutrition.at(tIngredientItemInfo.m_eIngredientType);

		tInventoryItemInfo = tIngredientItemInfo;
	} break;

	default: break;
	}
	
	EItemType eItemType = tInventoryItemInfo.m_eItemType;

	// 나무 상자는 있는데, 나무 상자 안에 해당 아이템이 없을 경우
	auto iter = m_mapChestBoxs[_eArchitectureType][_iWoodenChestID].find(eItemType);
	if (iter == m_mapChestBoxs[_eArchitectureType][_iWoodenChestID].end())
	{
		int32 iFlag = 0;

		for (int32 iOuterIndex = 0; iOuterIndex < 7; iOuterIndex++)
		{
			for (int32 iInnerIndex = 0; iInnerIndex < 6; iInnerIndex++)
			{
				if (m_mapChestBoxsSlots[_eArchitectureType][_iWoodenChestID][iOuterIndex][iInnerIndex].m_eItemType == EItemType::ENUM_END)
				{
					// 2중 for문 탈출을 위한 flag입니다.
					iFlag = 1;

					tInventoryItemInfo.m_iIndexX = iOuterIndex;
					tInventoryItemInfo.m_iIndexY = iInnerIndex;

					m_mapChestBoxs[_eArchitectureType][_iWoodenChestID].insert({ {tInventoryItemInfo.m_eItemType, tInventoryItemInfo} });
					m_mapChestBoxsSlots[_eArchitectureType][_iWoodenChestID][iOuterIndex][iInnerIndex] = tInventoryItemInfo;

					break;
				}
			}

			if (iFlag == 1) { break; }
		}
	}
	// 나무 상자는 있는데, 나무 상자 안에 해당 아이템이 있는 경우
	else
	{
		FInventoryItemInfo tMapInventoryItemInfo = iter->second;
		tMapInventoryItemInfo.m_iNumItem += tInventoryItemInfo.m_iNumItem;
		tMapInventoryItemInfo.m_fWeight += (CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(eItemType) * tMapInventoryItemInfo.m_iNumItem);
		m_mapChestBoxsSlots[_eArchitectureType][_iWoodenChestID][iter->second.m_iIndexX][iter->second.m_iIndexY] = tMapInventoryItemInfo; // 2차원 벡터에 넣어줌으로써, map과 vector의 정보 동기화
		iter->second = tMapInventoryItemInfo;
	}
}

void Client::CChestBoxSystem::MakeChestBox(EArchitectureType _eArchitectureType, int32 _iChestBoxID)
{
	if (_eArchitectureType == EArchitectureType::WOODEN_CHEST)
	{
		m_mapChestBoxs[_eArchitectureType].insert({_iChestBoxID, {} });
		m_mapChestBoxsSlots[_eArchitectureType][_iChestBoxID] = std::vector<std::vector<FInventoryItemInfo>>(2, std::vector<FInventoryItemInfo>(6));
	}
	else if (_eArchitectureType == EArchitectureType::IRON_BOX)
	{
		m_mapChestBoxs[_eArchitectureType].insert({ _iChestBoxID, {} });
		m_mapChestBoxsSlots[_eArchitectureType][_iChestBoxID] = std::vector<std::vector<FInventoryItemInfo>>(4, std::vector<FInventoryItemInfo>(6));
	}
}

void Client::CChestBoxSystem::SortItemsInContainer(ESortingType _eSortingType)
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	int32 iIndex = 0;
	std::array<FInventoryItemInfo, 42> arrItems{};

	for (int32 i = 0; i < pArrInventorySlots.size(); i++)
	{
		for (int32 j = 0; j < pArrInventorySlots[i].size(); j++)
		{
			arrItems[iIndex++] = pArrInventorySlots[i][j];
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
	for (int32 i = 0; i < pArrInventorySlots.size(); i++)
	{
		for (int32 j = 0; j < pArrInventorySlots[i].size(); j++)
		{
			wstring wstrChestBoxInventorySlotImageComponentName = L"inventory_slot_" + std::to_wstring(i) + std::to_wstring(j);
			wstring wstrChestBoxInventorySlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(i) + std::to_wstring(j);
			weak_ptr<CImageComponent> wpChestBoxInventorySlotImageComponent = m_spInventorySlotChildPanel->FindComponent<CImageComponent>(wstrChestBoxInventorySlotImageComponentName);
			weak_ptr<CTextComponent>  wpChestBoxInventorySlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrChestBoxInventorySlotTextComponentName);
			wpChestBoxInventorySlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpChestBoxInventorySlotTextComponent.lock()->SetText(L"");

			pArrInventorySlots[i][j] = arrItems[iIndex++];

			if (pArrInventorySlots[i][j].m_eItemType == EItemType::ENUM_END) { continue; }

			// 데이터 교환
			EItemType eItemType = pArrInventorySlots[i][j].m_eItemType;
			pMapInventoryItemInfos.find(eItemType)->second = pArrInventorySlots[i][j];

			wpChestBoxInventorySlotImageComponent.lock()->SetTextureFileName(pArrInventorySlots[i][j].m_wstrTextureFileName);
			wpChestBoxInventorySlotTextComponent.lock()->SetText(std::to_wstring(pArrInventorySlots[i][j].m_iNumItem));
		}
	}
}

void Client::CChestBoxSystem::ActiveSlotHover()
{
	if (!m_wpCurHoverSlotComponent.expired())
	{
		wstring wstrCurHoverSlotComponentName = m_wpCurHoverSlotComponent.lock()->GetComponentName();

		if (wstrCurHoverSlotComponentName == L"woodenchest_sort_none" ||
			wstrCurHoverSlotComponentName == L"inventory_sort_none")
		{
			m_wpSortHoverImage.lock()->SetCenterPosition(m_wpCurHoverSlotComponent.lock()->GetCenterPosition());
			m_wpSortHoverImage.lock()->SetActive(true);
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
		m_wpItemSlotHoverImage.lock()->SetActive(false);
	}
}

void Client::CChestBoxSystem::SortItemsInChestBoxs(ESortingType _eSortingType)
{
	std::vector<FInventoryItemInfo> vecItems{};

	for (int32 i = 0; i < m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID].size(); i++)
	{
		for (int32 j = 0; j < m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][i].size(); j++)
		{
			vecItems.push_back(m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][i][j]);
		}
	}

	// 정렬
	std::sort(vecItems.begin(), vecItems.end(), [&](const FInventoryItemInfo& _tInventoryItemInfoA, const FInventoryItemInfo& _tInventoryItemInfoB) -> bool
		{
			switch (_eSortingType)
			{
			case ESortingType::RANDOM: { return _tInventoryItemInfoA.m_eItemType != EItemType::ENUM_END && _tInventoryItemInfoB.m_eItemType != EItemType::ENUM_END; }
			case ESortingType::ITEM_TYPE: { return _tInventoryItemInfoA.m_eItemType > _tInventoryItemInfoB.m_eItemType; }
			case ESortingType::ITEM_COUNT: { return _tInventoryItemInfoA.m_iNumItem > _tInventoryItemInfoB.m_iNumItem; }
			}
			return 0;
		});

	int32 iIndex = 0;
	for (int32 i = 0; i < m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID].size(); i++)
	{
		for (int32 j = 0; j < m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][i].size(); j++)
		{
			wstring wstrItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(i) + std::to_wstring(j);
			wstring wstrItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(i) + std::to_wstring(j);
			weak_ptr<CImageComponent> wpItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrItemSlotImageComponentName);
			weak_ptr<CTextComponent>  wpItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrItemSlotTextComponentName);
			wpItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpItemSlotTextComponent.lock()->SetText(L"");

			m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][i][j] = vecItems[iIndex++];

			if (m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][i][j].m_eItemType == EItemType::ENUM_END) { continue; }

			// 데이터 교환
			EItemType eItemType = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][i][j].m_eItemType;
			m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].find(eItemType)->second = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][i][j];

			wpItemSlotImageComponent.lock()->SetTextureFileName(m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][i][j].m_wstrTextureFileName);
			wpItemSlotTextComponent.lock()->SetText(std::to_wstring(m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][i][j].m_iNumItem));
		}
	}
}

void Client::CChestBoxSystem::ShowInventoryItemInInventorySlots()
{	
	std::array<std::array<FInventoryItemInfo, 6>, 7>& pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	// 인벤토리가 아이템이 없을 경우 반환합니다.
	if (pMapInventoryItemInfos.empty()) { return; }

	for (int32 iOuter = 0; iOuter < pArrInventorySlots.size(); iOuter++)
	{
		for (int32 iInner = 0; iInner < pArrInventorySlots[iOuter].size(); iInner++)
		{
			if (pArrInventorySlots[iOuter][iInner].m_eItemType != EItemType::ENUM_END)
			{
				wstring wstrCurItemSlotImageComponentName = L"inventory_slot_" + std::to_wstring(iOuter) + std::to_wstring(iInner);
				wstring wstrCurItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(iOuter) + std::to_wstring(iInner);

				weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spInventorySlotChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

				wpCurItemSlotImageComponent.lock()->SetTextureFileName(pArrInventorySlots[iOuter][iInner].m_wstrTextureFileName);
				wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(pArrInventorySlots[iOuter][iInner].m_iNumItem));
			}
		}
	}
}

void Client::CChestBoxSystem::RemoveInventoryItemInInventorySlots()
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	if (pMapInventoryItemInfos.empty()) { return; }

	for (int32 iOuter = 0; iOuter < pArrInventorySlots.size(); iOuter++)
	{
		for (int32 iInner = 0; iInner < pArrInventorySlots[iOuter].size(); iInner++)
		{
			wstring wstrCurItemSlotImageComponentName = L"inventory_slot_" + std::to_wstring(iOuter) + std::to_wstring(iInner);
			wstring wstrCurItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(iOuter) + std::to_wstring(iInner);

			weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spInventorySlotChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

			wpCurItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpCurItemSlotTextComponent.lock()->SetText(L"");
		}
	}
}

void Client::CChestBoxSystem::ShowChestBoxItemInChestBoxSlots(EArchitectureType _eArchitectureType, int32 _iChestBoxID)
{
	if (m_mapChestBoxs[_eArchitectureType][_iChestBoxID].empty()) { return; }

	for (int32 iOuter = 0; iOuter < m_mapChestBoxsSlots[_eArchitectureType][_iChestBoxID].size(); iOuter++)
	{
		for (int32 iInner = 0; iInner < m_mapChestBoxsSlots[_eArchitectureType][_iChestBoxID][iOuter].size(); iInner++)
		{
			if (m_mapChestBoxsSlots[_eArchitectureType][_iChestBoxID][iOuter][iInner].m_eItemType != EItemType::ENUM_END)
			{
				wstring wstrCurItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(iOuter) + std::to_wstring(iInner);
				wstring wstrCurItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(iOuter) + std::to_wstring(iInner);

				weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
				weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

				wpCurItemSlotImageComponent.lock()->SetTextureFileName(m_mapChestBoxsSlots[_eArchitectureType][_iChestBoxID][iOuter][iInner].m_wstrTextureFileName);
				wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(m_mapChestBoxsSlots[_eArchitectureType][_iChestBoxID][iOuter][iInner].m_iNumItem));
			}
		}
	}
}

void Client::CChestBoxSystem::RemoveChestBoxItemInChestBoxSlots(EArchitectureType _eArchitectureType, int32 _iChestBoxID)
{
	if (m_mapChestBoxs[_eArchitectureType][_iChestBoxID].empty()) { return; }

	for (int32 iOuter = 0; iOuter < m_mapChestBoxsSlots[_eArchitectureType][_iChestBoxID].size(); iOuter++)
	{
		for (int32 iInner = 0; iInner < m_mapChestBoxsSlots[_eArchitectureType][_iChestBoxID][iOuter].size(); iInner++)
		{
			wstring wstrCurItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(iOuter) + std::to_wstring(iInner);
			wstring wstrCurItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(iOuter) + std::to_wstring(iInner);

			weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

			wpCurItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpCurItemSlotTextComponent.lock()->SetText(L"");
		}
	}
}

void Client::CChestBoxSystem::OpenSpecificChestBox(EArchitectureType _eArchitectureType, int32 _iChestBoxID, int32 _key)
{
	if (!m_bOpenContainerWidget && m_spInputManager->IsKeyDown(_key))
	{
		m_wpSoundManager.lock()->PlaySoundEx("Mainmenu_Open");
		m_spGameManager->GetPlayer()->GetCameraComponent()->SetMouseControl(false);
		m_bOpenContainerWidget = !m_bOpenContainerWidget;
		m_eArchitectureType = _eArchitectureType;
		m_iChestBoxID = _iChestBoxID;
		m_spContainerParentPanel->SetActive(true);
		m_spUIManager->SetTopPanel(m_spContainerParentPanel);
		RemoveInventoryItemInInventorySlots();
		RemoveChestBoxItemInChestBoxSlots(_eArchitectureType, m_iChestBoxID);
		ShowInventoryItemInInventorySlots();
		ShowChestBoxItemInChestBoxSlots(_eArchitectureType, _iChestBoxID);
		CalculateNumWeight();

		switch (_eArchitectureType)
		{
		case EArchitectureType::WOODEN_CHEST:
		{
			weak_ptr<CImageComponent> wpBaseImageComponent = m_spContainerParentPanel->FindComponent<CImageComponent>(L"base");
			wpBaseImageComponent.lock()->SetTextureFileName(L"T_WoodenChest_base");

			for (int iOuter = 2; iOuter <= 6; ++iOuter)
			{
				for (int iInner = 0; iInner <= 5; ++iInner)
				{
					m_spWoodenChestSlotChildPanel->FindComponent<CRectComponent>(
						L"woodenchest_slot_" + std::to_wstring(iOuter) + std::to_wstring(iInner)
					)->SetActive(false);
				}
			}
		}break;

		case EArchitectureType::IRON_BOX:
		{
			weak_ptr<CImageComponent> wpBaseImageComponent = m_spContainerParentPanel->FindComponent<CImageComponent>(L"base");
			wpBaseImageComponent.lock()->SetTextureFileName(L"T_MetalChest_base");

			for (int iOuter = 4; iOuter <= 6; ++iOuter)
			{
				for (int iInner = 0; iInner <= 5; ++iInner)
				{
					m_spWoodenChestSlotChildPanel->FindComponent<CRectComponent>(
						L"woodenchest_slot_" + std::to_wstring(iOuter) + std::to_wstring(iInner)
					)->SetActive(false);
				}
			}
		} break;

		default: break;
		}
	}
	else if (m_bOpenContainerWidget && m_spInputManager->IsKeyDown(_key))
	{
		m_wpSoundManager.lock()->PlaySoundEx("Mainmenu_Close");
		m_spGameManager->GetPlayer()->GetCameraComponent()->SetMouseControl(true);
		m_bOpenContainerWidget = !m_bOpenContainerWidget;
		m_spContainerParentPanel->SetActive(false);
		m_spUIManager->SetTopPanel(nullptr);
		RemoveInventoryItemInInventorySlots();
		RemoveChestBoxItemInChestBoxSlots(_eArchitectureType, m_iChestBoxID);
		m_iChestBoxID = -1;
		m_eArchitectureType = _eArchitectureType;

		for (int iOuter = 0; iOuter <= 6; ++iOuter)
		{
			for (int iInner = 0; iInner <= 5; ++iInner)
			{
				m_spWoodenChestSlotChildPanel->FindComponent<CRectComponent>(
					L"woodenchest_slot_" + std::to_wstring(iOuter) + std::to_wstring(iInner)
				)->SetActive(true);
			}
		}
	}
}

void Client::CChestBoxSystem::ActiveItemInfoWindow()
{
	if (!m_wpCurHoverSlotComponent.expired())
	{
		std::wstring wstrCurHoverSlotComponentName = m_wpCurHoverSlotComponent.lock()->GetComponentName();
		std::wstring wstrCurHoverSlotTextureName = m_wpCurHoverSlotComponent.lock()->GetTextureName();
		std::wstring wstrCurMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();

		std::wstring wstrCurHoverSlotComponentNameWithoutLastTwoChar = ExceptLastChar(wstrCurHoverSlotComponentName, 2);

		if (wstrCurHoverSlotTextureName != m_wstrInvisibleTextureFileName &&
			wstrCurMouseSlotTextureName == m_wstrInvisibleTextureFileName)
		{
			if (wstrCurHoverSlotComponentName != L"woodenchest_sort_none" &&
				wstrCurHoverSlotComponentName != L"inventory_sort_none")
			{
				if (wstrCurHoverSlotComponentNameWithoutLastTwoChar == L"inventory_slot_")
				{
					std::pair<int32, int32> prHoverSlotIndexs = ExtractLastChar(wstrCurHoverSlotComponentName, 2);
					std::array<std::array<FInventoryItemInfo, 6>, 7>& pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
					EItemKindType eItemKindType = pArrInventorySlots[prHoverSlotIndexs.first][prHoverSlotIndexs.second].m_eItemKindType;

					if (eItemKindType == EItemKindType::INGREDIENT)
					{
						FInventoryItemInfo tIngredientInfo = pArrInventorySlots[prHoverSlotIndexs.first][prHoverSlotIndexs.second];

						UpdateItemInfoFoodWindowPos();
						m_spItemInfoFoodParentPanel->SetActive(true);
						m_spItemInfoParentPanel->SetActive(false);

						m_spItemInfoFoodParentPanel->FindComponent<CImageComponent>(L"food_icon_00")->SetTextureFileName(tIngredientInfo.m_wstrTextureFileName);
						m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_explanation_00")->SetText(m_spUIManager->FindItemExplanation(tIngredientInfo.m_eItemType));
						m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_hunger_point_00")->SetText(std::to_wstring(tIngredientInfo.m_iNutrition));
						m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_weight_num_00")->SetText(std::to_wstring(static_cast<int32>(tIngredientInfo.m_fWeight)));
						m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_name_00")->SetText(m_spUIManager->FindItemName(tIngredientInfo.m_eItemType));
						m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_total_num_00")->SetText(std::to_wstring(tIngredientInfo.m_iNumItem));
					}
					else 
					{
						FInventoryItemInfo tInventoryInfo = pArrInventorySlots[prHoverSlotIndexs.first][prHoverSlotIndexs.second];

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
				else if(wstrCurHoverSlotComponentNameWithoutLastTwoChar == L"woodenchest_slot_")
				{
					std::pair<int32, int32> prHoverSlotIndexs = ExtractLastChar(wstrCurHoverSlotComponentName, 2);
					EItemKindType eItemKindType = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][prHoverSlotIndexs.first][prHoverSlotIndexs.second].m_eItemKindType;

					if (eItemKindType == EItemKindType::INGREDIENT)
					{
						FInventoryItemInfo tIngredientInfo = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][prHoverSlotIndexs.first][prHoverSlotIndexs.second];

						UpdateItemInfoFoodWindowPos();
						m_spItemInfoFoodParentPanel->SetActive(true);
						m_spItemInfoParentPanel->SetActive(false);

						m_spItemInfoFoodParentPanel->FindComponent<CImageComponent>(L"food_icon_00")->SetTextureFileName(tIngredientInfo.m_wstrTextureFileName);
						m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_explanation_00")->SetText(m_spUIManager->FindItemExplanation(tIngredientInfo.m_eItemType));
						m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_hunger_point_00")->SetText(std::to_wstring(tIngredientInfo.m_iNutrition));
						m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_weight_num_00")->SetText(std::to_wstring(static_cast<int32>(tIngredientInfo.m_fWeight)));
						m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_name_00")->SetText(m_spUIManager->FindItemName(tIngredientInfo.m_eItemType));
						m_spItemInfoFoodParentPanel->FindComponent<CTextComponent>(L"food_total_num_00")->SetText(std::to_wstring(tIngredientInfo.m_iNumItem));

					}
					else
					{
						FInventoryItemInfo tInventoryInfo = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][prHoverSlotIndexs.first][prHoverSlotIndexs.second];

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
	}
	else
	{
		m_spItemInfoParentPanel->SetActive(false);
		m_spItemInfoFoodParentPanel->SetActive(false);
	}
}

void Client::CChestBoxSystem::MoveItemUI()
{
	if (m_wpCurLeftClickedSlotComponent.expired()) { return; }

	std::wstring wstrMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotTextureName = m_wpCurLeftClickedSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();

	// 정렬 텍스처
	if ( wstrCurLeftClickedSlotComponentName == L"woodenchest_sort_none" &&
		 wstrMouseSlotTextureName == m_wstrInvisibleTextureFileName)
	{
		// 플레이어가 충돌한 객체의 architecturetype 받아오고, 그 고유 id값을 받아오기
		SortItemsInChestBoxs(ESortingType::ITEM_COUNT);
		m_wpCurMouseSlotComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
		m_spUIManager->SetFocusedControlComponent(nullptr);
	}
	else if ( wstrCurLeftClickedSlotComponentName == L"inventory_sort_none" &&
			  wstrMouseSlotTextureName == m_wstrInvisibleTextureFileName)
	{
		SortItemsInContainer(ESortingType::ITEM_COUNT);
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
				if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"inventory_slot_")
				{
					// cur clicked component name == item_slot_
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"inventory_slot_")
					{
						MoveInventorySlotUIToEmptyInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"woodenchest_slot_") //~3
					{
						MoveInventorySlotUIToEmptyChestBoxSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"woodenchest_slot_") //~3
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"woodenchest_slot_") //~3
					{
						MoveChestBoxSlotUIToEmptyChestBoxSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"inventory_slot_")
					{
						MoveChestBoxSlotUIToEmptyInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
			}

			/* 사진이 있을 경우 */
			else
			{
				// pre clicked component name == item_slot_
				if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"inventory_slot_")
				{
					// cur clicked component name == item_slot_
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"inventory_slot_")
					{
						MoveInventorySlotUIToOccupiedInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"woodenchest_slot_") //~3
					{
						MoveInventorySlotUIToOccupiedChestBoxSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
				else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"woodenchest_slot_") //~3
				{
					if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"woodenchest_slot_") //~3
					{
						MoveChestBoxSlotUIToOccupiedChestBoxSlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
					else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"inventory_slot_")
					{
						MoveChestBoxSlotUIToOccupiedInventorySlotUI(prPreClickedItemSlotIndexs, prCurClickedItemSlotIndexs);
					}
				}
			}

			m_wpCurMouseSlotComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			m_spUIManager->SetFocusedControlComponent(nullptr);
		}
	}
}

void Client::CChestBoxSystem::MoveInventorySlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	FInventoryItemInfo tPreInventoryItemInfo = pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
		pArrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreInventoryItemInfo;
		// map 안에 있는 구조체를 바꿔줍니다.
		EItemType eCurClickedItemType = tPreInventoryItemInfo.m_eItemType;
		pMapInventoryItemInfos.find(eCurClickedItemType)->second = tPreInventoryItemInfo;
	}
	// UI 교환합니다.
	{
		wstring wstrCurClickedItemSlotNumber = std::to_wstring(tPreInventoryItemInfo.m_iNumItem);
		ExchangeInventoryItemSlotUI(m_spInventorySlotChildPanel, m_wstrInvisibleTextureFileName, L"", tPreInventoryItemInfo.m_wstrTextureFileName, wstrCurClickedItemSlotNumber);
	}
}

void Client::CChestBoxSystem::MoveInventorySlotUIToEmptyChestBoxSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	FInventoryItemInfo tPreInventoryItemInfo = pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	auto iter = m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].find(tPreInventoryItemInfo.m_eItemType);
	if (iter == m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			pMapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
			pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};

			m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].insert({ tPreInventoryItemInfo.m_eItemType , tPreInventoryItemInfo });
			m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreInventoryItemInfo;
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"inventory_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			wstring wstrPreItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spInventorySlotChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			weak_ptr<CTextComponent>  wpPreItemSlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);

			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpPreItemSlotTextComponent.lock()->SetText(L"");

			// cur clicked slot
			wstring wstrCurItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(tPreInventoryItemInfo.m_iIndexX) + std::to_wstring(tPreInventoryItemInfo.m_iIndexY);
			wstring wstrCurItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(tPreInventoryItemInfo.m_iIndexX) + std::to_wstring(tPreInventoryItemInfo.m_iIndexY);

			weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

			wpCurItemSlotImageComponent.lock()->SetTextureFileName(tPreInventoryItemInfo.m_wstrTextureFileName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(tPreInventoryItemInfo.m_iNumItem));
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
			m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
			pMapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
			pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"inventory_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			wstring wstrPreItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spInventorySlotChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			weak_ptr<CTextComponent>  wpPreItemSlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);

			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpPreItemSlotTextComponent.lock()->SetText(L"");

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}
}

void Client::CChestBoxSystem::MoveChestBoxSlotUIToEmptyChestBoxSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	FInventoryItemInfo tPreWoodenChestItemInfo = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreWoodenChestItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreWoodenChestItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;

		// map
		m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID][tPreWoodenChestItemInfo.m_eItemType] = tPreWoodenChestItemInfo;

		// array
		m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
		m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreWoodenChestItemInfo;
	}
	// 이전 위치에 있는 UI를 없애줍니다.
	{
		// image
		weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
		weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);

		wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
		wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_wstrTextureFileName);
	
		// text
		wstring wstrPreItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
		wstring wstrCurItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);

		weak_ptr<CTextComponent>  wpPreItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);
		weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

		wpPreItemSlotTextComponent.lock()->SetText(L"");
		wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(tPreWoodenChestItemInfo.m_iNumItem));
	}
}

void Client::CChestBoxSystem::MoveChestBoxSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	FInventoryItemInfo tPreWoodenChestItemInfo = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];

	auto iter = pMapInventoryItemInfos.find(tPreWoodenChestItemInfo.m_eItemType);
	if (iter == pMapInventoryItemInfos.end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			tPreWoodenChestItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreWoodenChestItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			pMapInventoryItemInfos.insert({ tPreWoodenChestItemInfo.m_eItemType, tPreWoodenChestItemInfo });
			pArrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreWoodenChestItemInfo;
			// map 안에 있는 구조체를 바꿔줍니다.
			m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].erase(tPreWoodenChestItemInfo.m_eItemType);
			m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			wstring wstrPreItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			weak_ptr<CTextComponent>  wpPreItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);

			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpPreItemSlotTextComponent.lock()->SetText(L"");

			// cur clicked slot
			wstring wstrCurItemSlotImageComponentName = L"inventory_slot_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
			wstring wstrCurItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spInventorySlotChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

			wpCurItemSlotImageComponent.lock()->SetTextureFileName(tPreWoodenChestItemInfo.m_wstrTextureFileName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(tPreWoodenChestItemInfo.m_iNumItem));
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreWoodenChestItemInfo.m_iNumItem;
			pArrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreWoodenChestItemInfo.m_iNumItem;
			m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].erase(tPreWoodenChestItemInfo.m_eItemType);
			m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			wstring wstrPreItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			weak_ptr<CTextComponent>  wpPreItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);

			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpPreItemSlotTextComponent.lock()->SetText(L"");

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}
}

void Client::CChestBoxSystem::MoveInventorySlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	std::wstring wstrMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotTextureName = m_wpCurLeftClickedSlotComponent.lock()->GetTextureName();

	FInventoryItemInfo tPreInventoryItemInfo = pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];
	FInventoryItemInfo tCurInventoryItemInfo = pArrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		tCurInventoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
		tCurInventoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
		pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurInventoryItemInfo;
		pArrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreInventoryItemInfo;
		// map 안에 있는 구조체를 바꿔줍니다.
		EItemType m_ePreClickedItemType = pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_eItemType;
		EItemType m_eCurClickedItemType = pArrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType;
		pMapInventoryItemInfos.find(m_ePreClickedItemType)->second = pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];
		pMapInventoryItemInfos.find(m_eCurClickedItemType)->second = pArrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];
	}
	// UI 교환합니다.
	{
		wstring wstrPreClickedItemSlotNumber = std::to_wstring(pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_iNumItem);
		wstring wstrCurClickedItemSlotNumber = std::to_wstring(pArrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_iNumItem);
		ExchangeInventoryItemSlotUI(m_spInventorySlotChildPanel, wstrCurLeftClickedSlotTextureName, wstrPreClickedItemSlotNumber, wstrMouseSlotTextureName, wstrCurClickedItemSlotNumber);
	}
}

void Client::CChestBoxSystem::MoveInventorySlotUIToOccupiedChestBoxSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	FInventoryItemInfo tPreInventoryItemInfo = pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];
	FInventoryItemInfo tCurWoodenChestItemInfo = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

	auto iter = m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].find(tPreInventoryItemInfo.m_eItemType);
	if (iter == m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			pMapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
			m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].erase(tCurWoodenChestItemInfo.m_eItemType);

			tPreInventoryItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreInventoryItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			tCurWoodenChestItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
			tCurWoodenChestItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
			// array
			pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurWoodenChestItemInfo;
			pMapInventoryItemInfos.insert({ tCurWoodenChestItemInfo.m_eItemType, tCurWoodenChestItemInfo });
			// map
			m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreInventoryItemInfo;
			m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].insert({ tPreInventoryItemInfo.m_eItemType, tPreInventoryItemInfo });
		}
		// UI 교환합니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"inventory_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			wstring wstrPreItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spInventorySlotChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			weak_ptr<CTextComponent>  wpPreItemSlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);

			wpPreItemSlotImageComponent.lock()->SetTextureFileName(tCurWoodenChestItemInfo.m_wstrTextureFileName);
			wpPreItemSlotTextComponent.lock()->SetText(std::to_wstring(tCurWoodenChestItemInfo.m_iNumItem));

			// cur clicked slot
			wstring wstrCurItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
			wstring wstrCurItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

			wpCurItemSlotImageComponent.lock()->SetTextureFileName(tPreInventoryItemInfo.m_wstrTextureFileName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(tPreInventoryItemInfo.m_iNumItem));
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
			m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreInventoryItemInfo.m_iNumItem;
			pMapInventoryItemInfos.erase(tPreInventoryItemInfo.m_eItemType);
			pArrInventorySlots[_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"inventory_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			wstring wstrPreItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spInventorySlotChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			weak_ptr<CTextComponent>  wpPreItemSlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);

			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpPreItemSlotTextComponent.lock()->SetText(L"");

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}
}

void Client::CChestBoxSystem::MoveChestBoxSlotUIToOccupiedChestBoxSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	FInventoryItemInfo tPreWoodenChestItemInfo = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];
	FInventoryItemInfo tCurWoodenChestItemInfo = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

	// 데이터를 교환합니다.
	{
		// array를 교체합니다.
		tPreWoodenChestItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
		tPreWoodenChestItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
		tCurWoodenChestItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
		tCurWoodenChestItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;

		m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurWoodenChestItemInfo;
		m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreWoodenChestItemInfo;
		m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].find(m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second].m_eItemType)->second = tCurWoodenChestItemInfo;
		m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].find(m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second].m_eItemType)->second = tPreWoodenChestItemInfo;
	}

	{
		// pre clicked slot
		wstring wstrPreItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
		wstring wstrPreItemSlotTextComponentName  = L"woodenchest_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);

		weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
		weak_ptr<CTextComponent>  wpPreItemSlotTextComponent  = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);

		wpPreItemSlotImageComponent.lock()->SetTextureFileName(tCurWoodenChestItemInfo.m_wstrTextureFileName);
		wpPreItemSlotTextComponent.lock()->SetText(std::to_wstring(tCurWoodenChestItemInfo.m_iNumItem));

		// cur clicked slot
		wstring wstrCurItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
		wstring wstrCurItemSlotTextComponentName  = L"woodenchest_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);

		weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
		weak_ptr<CTextComponent> wpCurItemSlotTextComponent   = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

		wpCurItemSlotImageComponent.lock()->SetTextureFileName(tPreWoodenChestItemInfo.m_wstrTextureFileName);
		wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(tPreWoodenChestItemInfo.m_iNumItem));
	}
}

void Client::CChestBoxSystem::MoveChestBoxSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs)
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	FInventoryItemInfo tPreWoodenChestItemInfo = m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second];
	FInventoryItemInfo tCurInventoryItemInfo = pArrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second];

	auto iter = pMapInventoryItemInfos.find(tPreWoodenChestItemInfo.m_eItemType);
	if (iter == pMapInventoryItemInfos.end())
	{
		// 데이터를 교환합니다.
		{
			// array를 교체합니다.
			m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].erase(tPreWoodenChestItemInfo.m_eItemType);
			pMapInventoryItemInfos.erase(tCurInventoryItemInfo.m_eItemType);

			tPreWoodenChestItemInfo.m_iIndexX = _prCurClickedItemSlotIndexs.first;
			tPreWoodenChestItemInfo.m_iIndexY = _prCurClickedItemSlotIndexs.second;
			tCurInventoryItemInfo.m_iIndexX = _prPreClickedItemSlotIndexs.first;
			tCurInventoryItemInfo.m_iIndexY = _prPreClickedItemSlotIndexs.second;
			// map
			m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = tCurInventoryItemInfo;
			m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].insert({ tCurInventoryItemInfo.m_eItemType, tCurInventoryItemInfo });
			// array
			pArrInventorySlots[_prCurClickedItemSlotIndexs.first][_prCurClickedItemSlotIndexs.second] = tPreWoodenChestItemInfo;
			pMapInventoryItemInfos.insert({ tPreWoodenChestItemInfo.m_eItemType, tPreWoodenChestItemInfo });
		}
		// UI 교환합니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			wstring wstrPreItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			weak_ptr<CTextComponent>  wpPreItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);

			wpPreItemSlotImageComponent.lock()->SetTextureFileName(tCurInventoryItemInfo.m_wstrTextureFileName);
			wpPreItemSlotTextComponent.lock()->SetText(std::to_wstring(tCurInventoryItemInfo.m_iNumItem));

			// cur clicked slot
			wstring wstrCurItemSlotImageComponentName = L"inventory_slot_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);
			wstring wstrCurItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(_prCurClickedItemSlotIndexs.first) + std::to_wstring(_prCurClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpCurItemSlotImageComponent = m_spInventorySlotChildPanel->FindComponent<CImageComponent>(wstrCurItemSlotImageComponentName);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);

			wpCurItemSlotImageComponent.lock()->SetTextureFileName(tPreWoodenChestItemInfo.m_wstrTextureFileName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(tPreWoodenChestItemInfo.m_iNumItem));
		}
	}
	else
	{
		// 데이터를 교환합니다.
		{
			// 이미 있는 구조체
			iter->second.m_iNumItem += tPreWoodenChestItemInfo.m_iNumItem;
			pArrInventorySlots[iter->second.m_iIndexX][iter->second.m_iIndexY].m_iNumItem += tPreWoodenChestItemInfo.m_iNumItem;
			m_mapChestBoxs[m_eArchitectureType][m_iChestBoxID].erase(tPreWoodenChestItemInfo.m_eItemType);
			m_mapChestBoxsSlots[m_eArchitectureType][m_iChestBoxID][_prPreClickedItemSlotIndexs.first][_prPreClickedItemSlotIndexs.second] = {};
		}
		// 이전 위치에 있는 UI를 없애줍니다.
		{
			// pre clicked slot
			wstring wstrPreItemSlotImageComponentName = L"woodenchest_slot_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);
			wstring wstrPreItemSlotTextComponentName = L"woodenchest_slot_num_" + std::to_wstring(_prPreClickedItemSlotIndexs.first) + std::to_wstring(_prPreClickedItemSlotIndexs.second);

			weak_ptr<CImageComponent> wpPreItemSlotImageComponent = m_spWoodenChestSlotChildPanel->FindComponent<CImageComponent>(wstrPreItemSlotImageComponentName);
			weak_ptr<CTextComponent>  wpPreItemSlotTextComponent = m_spWoodenChestSlotChildPanel->FindComponent<CTextComponent>(wstrPreItemSlotTextComponentName);

			wpPreItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);
			wpPreItemSlotTextComponent.lock()->SetText(L"");

			// cur clicked slot
			wstring wstrCurItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(iter->second.m_iIndexX) + std::to_wstring(iter->second.m_iIndexY);
			weak_ptr<CTextComponent> wpCurItemSlotTextComponent = m_spInventorySlotChildPanel->FindComponent<CTextComponent>(wstrCurItemSlotTextComponentName);
			wpCurItemSlotTextComponent.lock()->SetText(std::to_wstring(iter->second.m_iNumItem));
		}
	}
}

void Client::CChestBoxSystem::ExchangeInventoryItemSlotUI(std::shared_ptr<CPanel> _spChildPanel, const std::wstring& _wstrPreClickedItemSlotTextureName, const std::wstring& _wstrPreClickedItemSlotTextNum, const std::wstring& _wstrCurClickedItemSlotTextureName, const std::wstring& _wstrCurClickedItemSlotTextNum)
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& m_pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& m_pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::pair<int32, int32> prCurClickedItemSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);
	std::pair<int32, int32> prPreClickedItemSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);

	// 슬롯의 이미지를 투명 텍스처로 교체합니다.
	weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = _spChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
	wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(_wstrPreClickedItemSlotTextureName);

	// 슬롯의 숫자 텍스트를 교체합니다.
	wstring wstrPreClickedItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(prPreClickedItemSlotIndexs.first) + std::to_wstring(prPreClickedItemSlotIndexs.second);
	weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = _spChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
	wpPreClickedItemSlotTextComponent.lock()->SetText(_wstrPreClickedItemSlotTextNum);

	// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
	weak_ptr<CImageComponent> wpCurClickedItemSlotImageComponent = _spChildPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName);
	wpCurClickedItemSlotImageComponent.lock()->SetTextureFileName(_wstrCurClickedItemSlotTextureName);

	// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
	wstring wstrCurClickedItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(prCurClickedItemSlotIndexs.first) + std::to_wstring(prCurClickedItemSlotIndexs.second);
	weak_ptr<CTextComponent> wpCurClickedItemSlotTextComponent = _spChildPanel->FindComponent<CTextComponent>(wstrCurClickedItemSlotTextComponentName);
	wstring wstrCurClickedItemSlotNumber = std::to_wstring(m_pArrInventorySlots[prCurClickedItemSlotIndexs.first][prCurClickedItemSlotIndexs.second].m_iNumItem);
	wpCurClickedItemSlotTextComponent.lock()->SetText(_wstrCurClickedItemSlotTextNum);
}

void Client::CChestBoxSystem::RemoveInventoryItemSlotUI()
{
	std::array<std::array<FInventoryItemInfo, 6>, 7>& m_pArrInventorySlots = m_spGameManager->GetInventorySystem()->GetInventorySlotItemsRef();
	std::map<EItemType, FInventoryItemInfo>& m_pMapInventoryItemInfos = m_spGameManager->GetInventorySystem()->GetInventorySlotItemInfosRef();

	std::pair<int32, int32> prPreClickedItemSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);

	// 슬롯의 이미지를 전 슬롯에 있던 텍스처로 교체합니다.
	weak_ptr<CImageComponent> wpPreClickedItemSlotImageComponent = m_spContainerChildPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName);
	wpPreClickedItemSlotImageComponent.lock()->SetTextureFileName(m_wstrInvisibleTextureFileName);

	// 슬롯의 숫자 텍스트를 전 슬롯에 있던 숫자 텍스트로 교체합니다.
	wstring wstrPreClickedItemSlotTextComponentName = L"inventory_slot_num_" + std::to_wstring(prPreClickedItemSlotIndexs.first) + std::to_wstring(prPreClickedItemSlotIndexs.second);
	weak_ptr<CTextComponent> wpPreClickedItemSlotTextComponent = m_spContainerChildPanel->FindComponent<CTextComponent>(wstrPreClickedItemSlotTextComponentName);
	wstring wstrPreClickedItemSlotNumber = std::to_wstring(m_pArrInventorySlots[prPreClickedItemSlotIndexs.first][prPreClickedItemSlotIndexs.second].m_iNumItem);
	wpPreClickedItemSlotTextComponent.lock()->SetText(L"");
}

void Client::CChestBoxSystem::UpdateItemInfoWindowPos()
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

void Client::CChestBoxSystem::UpdateItemInfoFoodWindowPos()
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

void Client::CChestBoxSystem::CalculateNumWeight()
{
	float fTotalWeight = m_spGameManager->GetInventorySystem()->GetInventoryTotalWeight();


	m_spInventorySlotChildPanel->FindComponent<CProgressBarComponent>(L"weight_bar")->SetRatio(fTotalWeight / 300);
	m_spInventorySlotChildPanel->FindComponent<CTextComponent>(L"weight_num_00")->SetText(std::to_wstring(static_cast<int32>(fTotalWeight)));
}