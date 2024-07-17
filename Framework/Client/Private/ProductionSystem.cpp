#include "pch.h"
#include "ProductionSystem.h"

#include "Panel.h"
#include "ImageComponent.h"
#include "TextComponent.h"
#include "ButtonComponent.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "SoundManager.h"

#include "GameManager.h"
#include "UIManager.h"
#include "InventorySystem.h"

#include "Player.h"
#include "Architecture.h"
#include "Factory.h"
#include "WorkBench.h"

std::shared_ptr<CProductionSystem> Client::CProductionSystem::Create(const std::shared_ptr<FProductionSystemDesc>& _spProductionSystemDesc)
{
	shared_ptr<CProductionSystem> spInstance = make_shared<CProductionSystem>();
	CHECKF(spInstance, L"CProductionSystem : Create Failed");
	spInstance->Initialize(_spProductionSystemDesc);
	return spInstance;
}

HRESULT Client::CProductionSystem::Initialize(const std::shared_ptr<FProductionSystemDesc>& _spProductionSystemDesc)
{
	m_spUIManager = CUIManager::GetInstance();

	m_spProductionWorkBenchUI = m_spUIManager->FindParentsPanel(L"WorkBenchUI");
	m_spCloseButton = m_spProductionWorkBenchUI->FindComponent<CButtonComponent>(L"CloseButton");
	m_spLayerImage = m_spProductionWorkBenchUI->FindComponent<CImageComponent>(L"LayerImage");
	AddInfo();

	return S_OK;
}

HRESULT Client::CProductionSystem::BeginPlay()
{
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();
	m_wpPlayer = CGameManager::GetInstance()->GetPlayer();
	m_spInventorySystem = CGameManager::GetInstance()->GetInventorySystem();
	return S_OK;
}

int32 Client::CProductionSystem::PreTick(float _fDeltaSeconds)
{
	return 0;
}

int32 Client::CProductionSystem::Tick(float _fDeltaSeconds)
{
	if (m_bActive && m_eCurProductionSystemType != m_ePreProductionSystemType)
	{
		// 벡터를 리셋해줍니다.
		for (int32 i = 0; i < 4; i++)
		{
			m_vecCurSlotInfos[i].clear();
		}

		// 현재 패널이 무엇인지에 따라 벡터를 결정해줍니다.
		switch (m_eCurProductionSystemType)
		{
		case Client::EProductionSystemType::WORKBENCH:
			// 벡터에 따른 슬롯을 리셋해줍니다.
			ActiveSlot(m_vecWorkBenchSlotInfos, CheckPlayerLevel(m_eCurProductionSystemType));
			// 벡터를 바꿔줍니다.
			for (int32 iOuterIndex = 0; iOuterIndex < 4; iOuterIndex++)
			{
				m_vecCurSlotInfos[iOuterIndex] = m_vecWorkBenchSlotInfos[iOuterIndex];
				for (int32 iInnerIndex = 0; iInnerIndex < 5; iInnerIndex++)
				{
					m_vecCurSlotInfos[iOuterIndex][iInnerIndex].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[iOuterIndex][iInnerIndex].eItemType));
				}
			}
			// 벡터에 따른 레이어 이미지를 바꿔줍니다.
			m_spLayerImage->SetTextureFileName(m_wstrWorkBenchLayerImage);
			break;
		case Client::EProductionSystemType::STOVE:
			// 벡터에 따른 슬롯을 리셋해줍니다.
			ActiveSlot(m_vecStoveSlotInfos, CheckPlayerLevel(m_eCurProductionSystemType));
			for (int32 iOuterIndex = 0; iOuterIndex < 4; iOuterIndex++)
			{
				m_vecCurSlotInfos[iOuterIndex] = m_vecStoveSlotInfos[iOuterIndex];
				for (int32 iInnerIndex = 0; iInnerIndex < 5; iInnerIndex++)
				{
					m_vecCurSlotInfos[iOuterIndex][iInnerIndex].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecStoveSlotInfos[iOuterIndex][iInnerIndex].eItemType));
				}
			}
			// 벡터에 따른 레이어 이미지를 바꿔줍니다.
			m_spLayerImage->SetTextureFileName(m_wstrStoveLayerImage);
			break;
		case Client::EProductionSystemType::FOOD:
			for (int32 iOuterIndex = 0; iOuterIndex < 4; iOuterIndex++)
			{
				m_vecCurSlotInfos[iOuterIndex] = m_vecFoodSlotInfos[iOuterIndex];
				for (int32 iInnerIndex = 0; iInnerIndex < 5; iInnerIndex++)
				{
					m_vecCurSlotInfos[iOuterIndex][iInnerIndex].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecFoodSlotInfos[iOuterIndex][iInnerIndex].eItemType));
				}
			}
			// 벡터에 따른 슬롯을 리셋해줍니다.
			ActiveSlot(m_vecCurSlotInfos, CheckPlayerLevel(m_eCurProductionSystemType));
			// 벡터에 따른 레이어 이미지를 바꿔줍니다.
			m_spLayerImage->SetTextureFileName(m_wstrFoodLayerImage);
			break;
		case Client::EProductionSystemType::EQUIPMENT:
			for (int32 iOuterIndex = 0; iOuterIndex < 4; iOuterIndex++)
			{
				m_vecCurSlotInfos[iOuterIndex] = m_vecEquipmentSlotInfos[iOuterIndex];
				for (int32 iInnerIndex = 0; iInnerIndex < 5; iInnerIndex++)
				{
					m_vecCurSlotInfos[iOuterIndex][iInnerIndex].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[iOuterIndex][iInnerIndex].eItemType));
				}
			}   
			// 벡터에 따른 슬롯을 리셋해줍니다.
			ActiveSlot(m_vecCurSlotInfos, CheckPlayerLevel(m_eCurProductionSystemType));
			// 벡터에 따른 레이어 이미지를 바꿔줍니다.
			m_spLayerImage->SetTextureFileName(m_wstrEquipmentLayerImage);
			break;
		}

		m_spUIManager->ResetClickImageComponent();
		m_spSelect->SetCenterPosition(Vector2(0.0f, 0.0f));
		m_spSelect->SetActive(false);

		m_tChildProductionSystemInfo.spChildProductionPanel->SetActive(false);

		m_spProductionWorkBenchUI->SetActive(true);
		m_spUIManager->SetTopPanel(m_spProductionWorkBenchUI);

		m_eCurProductionSystemType = m_ePreProductionSystemType;
		{
			FSlotInfo tSlotInfo;
			for (int32 i = 0; i < 4; i++)
			{
				for (auto& pair : m_vecCurSlotInfos[i])
				{
					int32 iIndex = 0;
					int32 iSuccessCount = 0;
					tSlotInfo = pair;
					for (auto& pair : m_vecMineralBoxs)
					{
						if (iIndex < tSlotInfo.vecDataSheet.size())
						{
							SetActiveMineralBox(true, pair);
							// 재료 아이템 텍스처
							pair.spItemImage->SetTextureFileName(m_spUIManager->FindItemUITextureName(tSlotInfo.vecDataSheet[iIndex].first));
							// 재료 아이템 이름
							pair.spItemNameText->SetText(m_spUIManager->FindItemName(tSlotInfo.vecDataSheet[iIndex].first));

							int32 iCurItemNum = m_spInventorySystem->FindInventoryItemTotalNum(tSlotInfo.vecDataSheet[iIndex].first);

							pair.spItemCurNumText->SetText(to_wstring(iCurItemNum));

							int32 iMaxNum = tSlotInfo.vecDataSheet[iIndex].second;

							pair.spItemMaxNumText->SetText(L"/" + to_wstring(iMaxNum));

							// 현재 개수와 목표 개수를 비교해줍니다.
							if (iCurItemNum < tSlotInfo.vecDataSheet[iIndex].second)
							{
							}
							else
							{
								iSuccessCount++;
							}
						}
						else
						{
						}

						iIndex++;
					}

					if (iSuccessCount == tSlotInfo.vecDataSheet.size())
					{
						tSlotInfo.spSlot->SetMaskTextureFileName(m_wstritemSuccessImage);
					}
					else
					{
						tSlotInfo.spSlot->SetMaskTextureFileName(m_wstritemFailImage);
					}
				}
			}
			m_tCurSlotInfo = tSlotInfo;
		}

	}

	if (m_spProductionWorkBenchUI == m_spUIManager->GetTopPanel())
	{
		ActiveHoverImage();

		if (nullptr != m_spUIManager->GetClickImageComponent().lock())
		{
			if (false == m_tChildProductionSystemInfo.spChildProductionPanel->IsActive())
			{
				m_tChildProductionSystemInfo.spChildProductionPanel->SetActive(true);
			}

			if (m_spSelect->GetCenterPosition() != m_spUIManager->GetClickImageComponent().lock()->GetCenterPosition())
			{
				m_spSelect->SetActive(true);
				m_iProductionNum = 0;
				m_tChildProductionSystemInfo.spProductionItemNumText->SetText(L"000");
				m_spSelect->SetCenterPosition(m_spUIManager->GetClickImageComponent().lock()->GetCenterPosition());

				// 선택된 컴포넌트가 있을 때 자식패널의 정보를 바꿔줍니다.
				{

					FSlotInfo tSlotInfo;
					for (int32 i = 0; i < 4; i++)
					{
						for (auto& pair : m_vecCurSlotInfos[i])
						{
							if (pair.spSlot == m_spUIManager->GetClickImageComponent().lock())
							{
								tSlotInfo = pair;
								break;
							}
						}
					}

					m_eItemType = tSlotInfo.eItemType;
					m_tCurSlotInfo = tSlotInfo;

					m_tChildProductionSystemInfo.spItemNameText->SetText(m_spUIManager->FindItemName(tSlotInfo.eItemType));
					m_tChildProductionSystemInfo.spSelectItemImage->SetTextureFileName(m_spUIManager->FindItemUITextureName(tSlotInfo.eItemType));

					// 필요 재료를 업데이트 해줍니다.
					{
						int32 iIndex = 0;
						int32 iSuccessCount = 0;
						for (auto& pair : m_vecMineralBoxs)
						{
							if (iIndex < tSlotInfo.vecDataSheet.size())
							{
								SetActiveMineralBox(true, pair);
								// 재료 아이템 텍스처
								pair.spItemImage->SetTextureFileName(m_spUIManager->FindItemUITextureName(tSlotInfo.vecDataSheet[iIndex].first));
								// 재료 아이템 이름
								pair.spItemNameText->SetText(m_spUIManager->FindItemName(tSlotInfo.vecDataSheet[iIndex].first));

								int32 iCurItemNum = m_spInventorySystem->FindInventoryItemTotalNum(tSlotInfo.vecDataSheet[iIndex].first);

								pair.spItemCurNumText->SetText(to_wstring(iCurItemNum));

								int32 iMaxNum = tSlotInfo.vecDataSheet[iIndex].second;

								if (m_iProductionNum > 1)
								{
									iMaxNum = tSlotInfo.vecDataSheet[iIndex].second * m_iProductionNum;
								}

								pair.spItemMaxNumText->SetText(L"/" + to_wstring(iMaxNum));

								// 현재 개수와 목표 개수를 비교해줍니다.
								if (iCurItemNum < tSlotInfo.vecDataSheet[iIndex].second)
								{
									// 아이템 개수가 모자른 상태이므로 빨간색으로 바꿔줍니다.
									pair.spBackGroundImage->SetColor(pair.vFailColor);
									pair.spItemCurNumText->SetColor(pair.vFailColor);
								}
								else
								{
									// 아이템 개수가 충족된 상태이므로 원래색으로 바꿔줍니다.
									pair.spBackGroundImage->SetColor(pair.vBackGroundSuccessColor);
									pair.spItemCurNumText->SetColor(pair.vItemCurCountSuccessColor);
									iSuccessCount++;
								}
							}
							else
							{
								SetActiveMineralBox(false, pair);
							}

							iIndex++;
						}

						if (iSuccessCount == tSlotInfo.vecDataSheet.size())
						{
							// 모든 재료 아이템이 충족되었을 경우 생산 버튼을 켜줍니다.
							m_tChildProductionSystemInfo.spProductionStartButton->SetActive(true);
						}
						else
						{
							m_tChildProductionSystemInfo.spProductionStartButton->SetActive(false);
						}
					}
				}
			}
		}
		else
		{
			m_spSelect->SetActive(false);
		}
		
		CheckButtonEvent();
	}

	return 0;
}

int32 Client::CProductionSystem::PostTick(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Client::CProductionSystem::EndPlay()
{
	return S_OK;
}

void Client::CProductionSystem::Release()
{
	for (auto& pair : m_vecMineralBoxs)
	{
		pair.spBackGroundImage = nullptr;
		pair.spItemCurNumText = nullptr;
		pair.spItemImage = nullptr;
		pair.spItemMaxNumText = nullptr;
		pair.spItemNameText = nullptr;
		pair.spLayerImage = nullptr;
	}
	m_vecMineralBoxs.clear();

	for (int32 i = 0; i < 4; i++)
	{
		for (auto& pair : m_vecWorkBenchSlotInfos[i])
		{
			pair.spSlot = nullptr;
			pair.spBaseSlot = nullptr;
		}
		m_vecWorkBenchSlotInfos[i].clear();
	}

	m_tChildProductionSystemInfo.spChildProductionPanel = nullptr;
	m_tChildProductionSystemInfo.spDownButton = nullptr;
	m_tChildProductionSystemInfo.spItemNameText = nullptr;
	m_tChildProductionSystemInfo.spProductionStartButton = nullptr;
	m_tChildProductionSystemInfo.spSelectItemImage = nullptr;
	m_tChildProductionSystemInfo.spUpButton = nullptr;
	m_tChildProductionSystemInfo.spProductionItemNumText = nullptr;

	m_tChildItemProductionSystemInfo.spChildItemPanel = nullptr;

	m_spHover = nullptr;
	m_spSelect = nullptr;

	m_spProductionWorkBenchUI = nullptr;

	m_spInventorySystem = nullptr;

	m_spUIManager = nullptr;
}

void Client::CProductionSystem::AddChildItemProductionInfo()
{
	m_tChildItemProductionSystemInfo.spChildItemPanel = static_pointer_cast<CPanel>(m_spProductionWorkBenchUI->FindChildGameObject(L"ChildItemPanel"));
}

void Client::CProductionSystem::AddSlot()
{
	for (int32 iOuterIndex = 0; iOuterIndex < 4; iOuterIndex++)
	{
		for (int32 iInnerIndex = 0; iInnerIndex < 5; iInnerIndex++)
		{
			std::wstring wstrSlotImage = L"Slot_" + to_wstring(iOuterIndex) + L"_" + to_wstring(iInnerIndex);

			std::wstring wstrBaseSlotImage = L"Slot_Base_" + to_wstring(iOuterIndex) + L"_" + to_wstring(iInnerIndex);

			FSlotInfo tSlotInfo;
			tSlotInfo.spSlot = m_tChildItemProductionSystemInfo.spChildItemPanel->FindComponent<CImageComponent>(wstrSlotImage);
			tSlotInfo.spBaseSlot = m_tChildItemProductionSystemInfo.spChildItemPanel->FindComponent<CImageComponent>(wstrBaseSlotImage);
			m_vecWorkBenchSlotInfos[iOuterIndex].push_back(tSlotInfo);
			m_vecStoveSlotInfos[iOuterIndex].push_back(tSlotInfo);
			m_vecFoodSlotInfos[iOuterIndex].push_back(tSlotInfo);
			m_vecEquipmentSlotInfos[iOuterIndex].push_back(tSlotInfo);
		}
	}
}

void Client::CProductionSystem::AddInfo()
{
	AddProductionComponent();
	AddChildProductionInfo();
	AddChildItemProductionInfo();
	AddSlot();
	AddWorkBenchSlot();
	AddStoveSlot();
	AddFoodSlot();
	AddEquipmentSlot();
	AddMineralBox();
}

void Client::CProductionSystem::AddProductionComponent()
{
	m_spHover = m_spProductionWorkBenchUI->FindComponent<CImageComponent>(L"HoverImage");
	m_spSelect = m_spProductionWorkBenchUI->FindComponent<CImageComponent>(L"SelectImage");
	m_spSelect->SetCenterPosition(Vector2(0.0f, 0.0f));
}

void Client::CProductionSystem::AddChildProductionInfo()
{
	m_tChildProductionSystemInfo.spChildProductionPanel = static_pointer_cast<CPanel>(m_spProductionWorkBenchUI->FindChildGameObject(L"ChildProductionPanel"));
	m_tChildProductionSystemInfo.spChildProductionPanel->SetActive(false);

	m_tChildProductionSystemInfo.spDownButton = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CButtonComponent>(L"ProductionDownButton");
	m_tChildProductionSystemInfo.spUpButton = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CButtonComponent>(L"ProductionUpButton");
	m_tChildProductionSystemInfo.spProductionStartButton = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CButtonComponent>(L"ProductionActiveButton");

	m_tChildProductionSystemInfo.spSelectItemImage = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CImageComponent>(L"ProductionItemImage");;

	m_tChildProductionSystemInfo.spItemNameText = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CTextComponent>(L"ProductionItemText");
	m_tChildProductionSystemInfo.spProductionItemNumText = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CTextComponent>(L"ProductionItemCountText");
}

void Client::CProductionSystem::AddMineralBox()
{
	for (int32 i = 0; i < 4; i++)
	{
		FMineralBoxInfo tMineralBoxInfo;

		std::wstring wstrBackImage = L"MineralBaseImage" + to_wstring(i);
		std::wstring wstrLayerImage = L"MineralLayerImage" + to_wstring(i);
		std::wstring wstrItemImage = L"MineralItemImage" + to_wstring(i);

		std::wstring wstrItemNameText = L"MineralItemNameText" + to_wstring(i);
		std::wstring wstrCurText = L"MineralItemCurText" + to_wstring(i);
		std::wstring wstrMaxText = L"MineralItemMaxText" + to_wstring(i);

		tMineralBoxInfo.spBackGroundImage = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CImageComponent>(wstrBackImage);
		tMineralBoxInfo.spLayerImage = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CImageComponent>(wstrLayerImage);
		tMineralBoxInfo.spItemImage = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CImageComponent>(wstrItemImage);

		tMineralBoxInfo.spItemNameText = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CTextComponent>(wstrItemNameText);
		tMineralBoxInfo.spItemCurNumText = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CTextComponent>(wstrCurText);
		tMineralBoxInfo.spItemMaxNumText = m_tChildProductionSystemInfo.spChildProductionPanel->FindComponent<CTextComponent>(wstrMaxText);

		tMineralBoxInfo.vBackGroundSuccessColor = tMineralBoxInfo.spBackGroundImage->GetColor();
		tMineralBoxInfo.vItemCurCountSuccessColor = tMineralBoxInfo.spItemCurNumText->GetColor();
		tMineralBoxInfo.vFailColor = Vector4(1.0f, 0.2f, 0.2f, 1.0f);

		m_vecMineralBoxs.push_back(tMineralBoxInfo);
	}
}

void Client::CProductionSystem::AddWorkBenchSlot()
{
	//0_0 //블루 스피어
	{
		m_vecWorkBenchSlotInfos[0][0].eItemType = EItemType::BLUE_SPHERE;
		m_vecWorkBenchSlotInfos[0][0].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::BLUE_SPHERE);
		m_vecWorkBenchSlotInfos[0][0].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[0][0].eItemType));
	}
	//0_1
	{
		m_vecWorkBenchSlotInfos[0][1].eItemType = EItemType::Outfit_Cloth001;
		m_vecWorkBenchSlotInfos[0][1].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::Outfit_Cloth001);
		m_vecWorkBenchSlotInfos[0][1].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[0][1].eItemType));
	}
	//0_2
	{
		m_vecWorkBenchSlotInfos[0][2].eItemType = EItemType::GLIDER;
		m_vecWorkBenchSlotInfos[0][2].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::GLIDER);
		m_vecWorkBenchSlotInfos[0][2].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[0][2].eItemType));
	}
	//0_3
	{
		m_vecWorkBenchSlotInfos[0][3].eItemType = EItemType::ARROW;
		m_vecWorkBenchSlotInfos[0][3].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::ARROW);
		m_vecWorkBenchSlotInfos[0][3].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[0][3].eItemType));
	}
	//0_4
	{
		m_vecWorkBenchSlotInfos[0][4].eItemType = EItemType::Outfit_Hunter001;
		m_vecWorkBenchSlotInfos[0][4].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::Outfit_Hunter001);
		m_vecWorkBenchSlotInfos[0][4].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[0][4].eItemType));
	}

	//1_0
	{
		m_vecWorkBenchSlotInfos[1][0].eItemType = EItemType::HeadEquip021;
		m_vecWorkBenchSlotInfos[1][0].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::HeadEquip021);
		m_vecWorkBenchSlotInfos[1][0].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[1][0].eItemType));
	}
	//1_1
	{
		m_vecWorkBenchSlotInfos[1][1].eItemType = EItemType::HeadEquip016;
		m_vecWorkBenchSlotInfos[1][1].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::HeadEquip016);
		m_vecWorkBenchSlotInfos[1][1].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[1][1].eItemType));
	}
	//1_2
	{
		m_vecWorkBenchSlotInfos[1][2].eItemType = EItemType::GREEN_SPHERE;
		m_vecWorkBenchSlotInfos[1][2].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::GREEN_SPHERE);
		m_vecWorkBenchSlotInfos[1][2].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[1][2].eItemType));
	}
	//1_3
	{
		m_vecWorkBenchSlotInfos[1][3].eItemType = EItemType::HeadEquip019;
		m_vecWorkBenchSlotInfos[1][3].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::HeadEquip019);
		m_vecWorkBenchSlotInfos[1][3].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[1][3].eItemType));
	}
	//1_4
	{
		m_vecWorkBenchSlotInfos[1][4].eItemType = EItemType::HeadEquip018;
		m_vecWorkBenchSlotInfos[1][4].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::HeadEquip018);
		m_vecWorkBenchSlotInfos[1][4].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[1][4].eItemType));
	}

	//2_0
	{
		m_vecWorkBenchSlotInfos[2][0].eItemType = EItemType::RIFLE_BULLET;
		m_vecWorkBenchSlotInfos[2][0].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::RIFLE_BULLET);
		m_vecWorkBenchSlotInfos[2][0].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[2][0].eItemType));
	}
	//2_1
	{
		m_vecWorkBenchSlotInfos[2][1].eItemType = EItemType::Outfit_Platinum001_v02;
		m_vecWorkBenchSlotInfos[2][1].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::Outfit_Platinum001_v02);
		m_vecWorkBenchSlotInfos[2][1].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecWorkBenchSlotInfos[2][1].eItemType));
	}

	//ActiveSlot(m_vecWorkBenchSlotInfos, 12);
}

void Client::CProductionSystem::AddStoveSlot()
{
	//0_0
	{
		m_vecStoveSlotInfos[0][0].eItemType = EItemType::COPPER_INGOT;
		m_vecStoveSlotInfos[0][0].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::COPPER_INGOT);
		m_vecStoveSlotInfos[0][0].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecStoveSlotInfos[0][0].eItemType));
	}
	//0_1
	{
		m_vecStoveSlotInfos[0][1].eItemType = EItemType::IRON_INGOT;
		m_vecStoveSlotInfos[0][1].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::IRON_INGOT);
		m_vecStoveSlotInfos[0][1].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecStoveSlotInfos[0][1].eItemType));
	}
	//0_2
	{
		m_vecStoveSlotInfos[0][2].eItemType = EItemType::GUNPOWDER;
		m_vecStoveSlotInfos[0][2].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::GUNPOWDER);
		m_vecStoveSlotInfos[0][2].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecStoveSlotInfos[0][2].eItemType));
	}
	//0_3
	{
		m_vecStoveSlotInfos[0][3].eItemType = EItemType::CARBON;
		m_vecStoveSlotInfos[0][3].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::CARBON);
		m_vecStoveSlotInfos[0][3].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecStoveSlotInfos[0][3].eItemType));
	}
	
	//ActiveSlot(m_vecStoveSlotInfos, 4);
}

void Client::CProductionSystem::AddFoodSlot()
{
	//0_0
	{
		m_vecFoodSlotInfos[0][0].eItemType = EItemType::BAKED_BERRY;
		m_vecFoodSlotInfos[0][0].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::BAKED_BERRY);
		m_vecFoodSlotInfos[0][0].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecFoodSlotInfos[0][0].eItemType));
	}
	//0_1
	{
		m_vecFoodSlotInfos[0][1].eItemType = EItemType::SHEEPBALL_KEBAB;
		m_vecFoodSlotInfos[0][1].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::SHEEPBALL_KEBAB);
		m_vecFoodSlotInfos[0][1].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecFoodSlotInfos[0][1].eItemType));
	}
	//0_2
	//{
	//	m_vecFoodSlotInfos[0][2].eItemType = EItemType::CHICKEN_KEBAB;
	//	m_vecFoodSlotInfos[0][2].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::CHICKEN_KEBAB);
	//	m_vecFoodSlotInfos[0][2].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecFoodSlotInfos[0][2].eItemType));
	//}
	//0_3
	{
		m_vecFoodSlotInfos[0][2].eItemType = EItemType::FRIED_EGG;
		m_vecFoodSlotInfos[0][2].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::FRIED_EGG);
		m_vecFoodSlotInfos[0][2].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecFoodSlotInfos[0][2].eItemType));
	}
	//0_4
	{
		m_vecFoodSlotInfos[0][3].eItemType = EItemType::SHEEPBALL_ROAST;
		m_vecFoodSlotInfos[0][3].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::SHEEPBALL_ROAST);
		m_vecFoodSlotInfos[0][3].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecFoodSlotInfos[0][3].eItemType));
	}
	//1_0
	{
		m_vecFoodSlotInfos[0][4].eItemType = EItemType::CHICKEN_ROAST;
		m_vecFoodSlotInfos[0][4].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::CHICKEN_ROAST);
		m_vecFoodSlotInfos[0][4].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecFoodSlotInfos[0][4].eItemType));
	}
	
	//ActiveSlot(m_vecFoodSlotInfos, 6);
}

void Client::CProductionSystem::AddEquipmentSlot()
{
	//0_0 //블루 스피어
	{
		m_vecEquipmentSlotInfos[0][0].eItemType = EItemType::HAND_HELD_TORCH;
		m_vecEquipmentSlotInfos[0][0].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::HAND_HELD_TORCH);
		m_vecEquipmentSlotInfos[0][0].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[0][0].eItemType));
	}
	//0_1
	{
		m_vecEquipmentSlotInfos[0][1].eItemType = EItemType::STONE_AXE;
		m_vecEquipmentSlotInfos[0][1].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::STONE_AXE);
		m_vecEquipmentSlotInfos[0][1].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[0][1].eItemType));
	}
	//0_2
	{
		m_vecEquipmentSlotInfos[0][2].eItemType = EItemType::STONE_PICKAXE;
		m_vecEquipmentSlotInfos[0][2].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::STONE_PICKAXE);
		m_vecEquipmentSlotInfos[0][2].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[0][2].eItemType));
	}
	//0_3
	{
		m_vecEquipmentSlotInfos[0][3].eItemType = EItemType::STONE_SPEAR;
		m_vecEquipmentSlotInfos[0][3].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::STONE_SPEAR);
		m_vecEquipmentSlotInfos[0][3].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[0][3].eItemType));
	}
	//0_4
	{
		m_vecEquipmentSlotInfos[0][4].eItemType = EItemType::OLD_BOW;
		m_vecEquipmentSlotInfos[0][4].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::OLD_BOW);
		m_vecEquipmentSlotInfos[0][4].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[0][4].eItemType));
	}

	//1_0
	{
		m_vecEquipmentSlotInfos[1][0].eItemType = EItemType::COMMON_SHIELD;
		m_vecEquipmentSlotInfos[1][0].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::COMMON_SHIELD);
		m_vecEquipmentSlotInfos[1][0].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[1][0].eItemType));
	}
	//1_1
	{
		m_vecEquipmentSlotInfos[1][1].eItemType = EItemType::MEAT_CLEAVER;
		m_vecEquipmentSlotInfos[1][1].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::MEAT_CLEAVER);
		m_vecEquipmentSlotInfos[1][1].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[1][1].eItemType));
	}
	//1_2
	{
		m_vecEquipmentSlotInfos[1][2].eItemType = EItemType::GIGA_SHIELD;
		m_vecEquipmentSlotInfos[1][2].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::GIGA_SHIELD);
		m_vecEquipmentSlotInfos[1][2].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[1][2].eItemType));
	}
	//1_3
	{
		m_vecEquipmentSlotInfos[1][3].eItemType = EItemType::ROCKET_LAUNCHER;
		m_vecEquipmentSlotInfos[1][3].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::ROCKET_LAUNCHER);
		m_vecEquipmentSlotInfos[1][3].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[1][3].eItemType));
	}
	//1_4
	{
		m_vecEquipmentSlotInfos[1][4].eItemType = EItemType::ASSAULT_RIFLE;
		m_vecEquipmentSlotInfos[1][4].vecDataSheet = m_spUIManager->FindItemDataSheet(EItemType::ASSAULT_RIFLE);
		m_vecEquipmentSlotInfos[1][4].spSlot->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_vecEquipmentSlotInfos[1][4].eItemType));
	}

	//ActiveSlot(m_vecEquipmentSlotInfos, 10);
}

void Client::CProductionSystem::ActiveHoverImage()
{
	if (nullptr != m_spUIManager->GetHoverImageComponent().lock())
	{
		if (false == m_spHover->IsActive())
		{
			m_spHover->SetActive(true);
			m_wpSoundManager.lock()->PlaySoundEx("HoverSound");
		}
		else if (m_spHover->GetCenterPosition() != m_spUIManager->GetHoverImageComponent().lock()->GetCenterPosition())
		{
			m_wpSoundManager.lock()->PlaySoundEx("HoverSound");
		}


		m_spHover->SetCenterPosition(m_spUIManager->GetHoverImageComponent().lock()->GetCenterPosition());
	}
	else
	{
		m_spHover->SetActive(false);
	}
}

void Client::CProductionSystem::SetActiveMineralBox(bool _isActive, FMineralBoxInfo _tMineralBox)
{
	_tMineralBox.spBackGroundImage->SetActive(_isActive);
	_tMineralBox.spItemCurNumText->SetActive(_isActive);
	_tMineralBox.spItemImage->SetActive(_isActive);
	_tMineralBox.spItemMaxNumText->SetActive(_isActive);
	_tMineralBox.spItemNameText->SetActive(_isActive);
	_tMineralBox.spLayerImage->SetActive(_isActive);
}

void Client::CProductionSystem::DownButtonEvent()
{
	if (0 >= m_iProductionNum)
	{
		return;
	}

	int32 iIndex = 0;
	int32 iSuccessCount = 0;
	for (auto& pair : m_vecMineralBoxs)
	{
		if (iIndex < m_tCurSlotInfo.vecDataSheet.size())
		{
			int32 iCurItemNum = m_spInventorySystem->FindInventoryItemTotalNum(m_tCurSlotInfo.vecDataSheet[iIndex].first);
			int32 iMaxNum = m_tCurSlotInfo.vecDataSheet[iIndex].second;

			if (m_iProductionNum - 1 > 1)
			{
				iMaxNum = m_tCurSlotInfo.vecDataSheet[iIndex].second * (m_iProductionNum - 1);
			}

			// 현재 개수와 목표 개수를 비교해줍니다.
			if (iCurItemNum < iMaxNum)
			{
				return;
			}
			else { iSuccessCount++; }

			pair.spItemMaxNumText->SetText(L"/" + to_wstring(iMaxNum));
		}
		iIndex++;
	}

	m_iProductionNum--;
	std::wstring wstrNum;
	if (10 > m_iProductionNum)
	{
		wstrNum = L"00";
	}
	else
	{
		wstrNum = L"0";
	}
	m_tChildProductionSystemInfo.spProductionItemNumText->SetText(wstrNum + to_wstring(m_iProductionNum));
}

void Client::CProductionSystem::UpButtonEvent()
{
	if (99 <= m_iProductionNum)
	{
		return;
	}

	// 인벤토리 아이템 개수가 충족된 경우에만 눌렀을 때 수를 증가시켜줍니다.
	int32 iIndex = 0;
	int32 iSuccessCount = 0;
	for (auto& pair : m_vecMineralBoxs)
	{
		if (iIndex < m_tCurSlotInfo.vecDataSheet.size())
		{
			int32 iCurItemNum = m_spInventorySystem->FindInventoryItemTotalNum(m_tCurSlotInfo.vecDataSheet[iIndex].first);
			int32 iMaxNum = m_tCurSlotInfo.vecDataSheet[iIndex].second;

			if (m_iProductionNum + 1 > 1)
			{
				iMaxNum = m_tCurSlotInfo.vecDataSheet[iIndex].second * (m_iProductionNum + 1);
			}

			// 현재 개수와 목표 개수를 비교해줍니다.
			if (iCurItemNum < iMaxNum)
			{
				return;
			}
			else { iSuccessCount++; }

			pair.spItemMaxNumText->SetText(L"/" + to_wstring(iMaxNum));
		}
		iIndex++;
	}

	m_iProductionNum++;
	std::wstring wstrNum;
	if (10 > m_iProductionNum)
	{
		wstrNum = L"00";
	}
	else
	{
		wstrNum = L"0";
	}
	m_tChildProductionSystemInfo.spProductionItemNumText->SetText(wstrNum + to_wstring(m_iProductionNum));
}

void Client::CProductionSystem::ProductionStartEvent()
{
	// 만일 제작할 아이템 개수가 0이라면 빠져나갑니다.
	if (0 == m_iProductionNum)
	{
		return;
	}

	// 생산 개시를 눌렀을 때  생산 커맨드를 호출해줍니다.
	static_pointer_cast<CFactory>(m_spCurArchitecture)->AddItemToFactory(m_eItemType, m_iProductionNum);
	// 실제 아이템 제작 패널을 넣어줍니다.
	static_pointer_cast<CFactory>(m_spCurArchitecture)->SetWorkPanel(m_spUIManager->GetUsingWorkUI());
	Vector3 vPosition = Vector3(m_spCurArchitecture->GetTransform(ETransformType::POSITION).value().x, m_spCurArchitecture->GetTransform(ETransformType::POSITION).value().y + m_spCurArchitecture->GetColliderExtents().y + 0.3f, m_spCurArchitecture->GetTransform(ETransformType::POSITION).value().z);
	static_pointer_cast<CFactory>(m_spCurArchitecture)->GetWorkPanel()->SetPivotWorldPoint(Vector3(m_spCurArchitecture->GetPosition().value().x, m_spCurArchitecture->GetPosition().value().y + 0.7f, m_spCurArchitecture->GetPosition().value().z));
	// 패널의 바뀌지 않을 정보를 업데이트 해줍니다.
	{
		// 아이템 이미지
		static_pointer_cast<CFactory>(m_spCurArchitecture)->GetWorkPanel()->FindComponent<CImageComponent>(L"ItemImage")->SetTextureFileName(m_spUIManager->FindItemUITextureName(m_eItemType));
		// 아이템 텍스트
		static_pointer_cast<CFactory>(m_spCurArchitecture)->GetWorkPanel()->FindComponent<CTextComponent>(L"TitleText")->SetText(m_spUIManager->FindItemName(m_eItemType));
		// 만들 아이템 개수
		static_pointer_cast<CFactory>(m_spCurArchitecture)->GetWorkPanel()->FindComponent<CTextComponent>(L"MaxCountText")->SetText(to_wstring(m_iProductionNum));
		//static_pointer_cast<CFactory>(m_spCurArchitecture)->GetWorkPanel()->FindChildGameObject(L"ChildWorkPalUI")->SetActive(false);
	}

	// 쓴 재료 아이템들을 인벤토리에서 삭제해줍니다.
	for (int i = 0; i < m_tCurSlotInfo.vecDataSheet.size(); i++)
	{
		m_spInventorySystem->ReduceItemNum(m_tCurSlotInfo.vecDataSheet[i].first, m_tCurSlotInfo.vecDataSheet[i].second * m_iProductionNum);
	}

	// 패널을 꺼줍니다.
	m_bActive = false;
	m_ePreProductionSystemType = EProductionSystemType::ENUM_END;
	if (m_spCurArchitecture)
	{
		m_spCurArchitecture = nullptr;
	}
	m_spUIManager->ResetTopPanel();
	m_spProductionWorkBenchUI->SetActive(false);
}

void Client::CProductionSystem::CloseEvent()
{
	m_bActive = false;
	m_ePreProductionSystemType = EProductionSystemType::ENUM_END;
	if (m_spCurArchitecture)
	{
		m_spCurArchitecture = nullptr;
	}
	m_spUIManager->ResetTopPanel();
	m_spProductionWorkBenchUI->SetActive(false);
}

void Client::CProductionSystem::CheckButtonEvent()
{
	if (nullptr != m_spUIManager->GetFocusedControlComponent().lock())
	{
		if (m_tChildProductionSystemInfo.spDownButton == m_spUIManager->GetFocusedControlComponent().lock())
		{
			m_tChildProductionSystemInfo.spDownButton->AddButtonClickEvent([this]() {this->DownButtonEvent(); });
		}
		else if (m_tChildProductionSystemInfo.spUpButton == m_spUIManager->GetFocusedControlComponent().lock())
		{
			m_tChildProductionSystemInfo.spUpButton->AddButtonClickEvent([this]() {this->UpButtonEvent(); });
		}
		else if (m_tChildProductionSystemInfo.spProductionStartButton == m_spUIManager->GetFocusedControlComponent().lock())
		{
			m_tChildProductionSystemInfo.spProductionStartButton->AddButtonClickEvent([this]() {this->ProductionStartEvent(); });
		}
		else if (m_spCloseButton == m_spUIManager->GetFocusedControlComponent().lock())
		{
			m_spCloseButton->AddButtonClickEvent([this]() {this->CloseEvent(); });
		}
	}
}

void Client::CProductionSystem::ActiveSlot(std::vector<FSlotInfo> _vecSlotInfo[4], int32 _iNum)
{
	// 들어온 수로 몫과 나머지를 구해줍니다.
	int32 iOutNum = _iNum / 5;
	int32 iInNum = (_iNum % 5) - 1;

	// 안쓰는 칸을 꺼줍니다.
	for (int32 iOuterIndex = 0; iOuterIndex < 4; iOuterIndex++)
	{
		for (int32 iInnerIndex = 0; iInnerIndex < _vecSlotInfo[iOuterIndex].size(); iInnerIndex++)
		{
			if (iOutNum == iOuterIndex)
			{
				if (iInNum < iInnerIndex)
				{
					_vecSlotInfo[iOuterIndex][iInnerIndex].spBaseSlot->SetActive(false);
					_vecSlotInfo[iOuterIndex][iInnerIndex].spSlot->SetActive(false);
					continue;
				}
			}
			else if (iOutNum < iOuterIndex)
			{
				_vecSlotInfo[iOuterIndex][iInnerIndex].spBaseSlot->SetActive(false);
				_vecSlotInfo[iOuterIndex][iInnerIndex].spSlot->SetActive(false);
				continue;
			}

			_vecSlotInfo[iOuterIndex][iInnerIndex].spBaseSlot->SetActive(true);
			_vecSlotInfo[iOuterIndex][iInnerIndex].spSlot->SetActive(true);
		}
	}
}

int32 Client::CProductionSystem::CheckPlayerLevel(EProductionSystemType _eProductionSystemType)
{
	int32 iNum = 0;

	switch (_eProductionSystemType)
	{
	case Client::EProductionSystemType::WORKBENCH:
		if (1 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 2; }
		else if (2 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 3; }
		else if (3 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 5; }
		else if (4 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 6; }
		else if (5 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 8; }
		else if (6 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 9; }
		else if (7 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 11; }
		else if (8 <= m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 12; }
		break;
	case Client::EProductionSystemType::STOVE:
		if (4 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 1; }
		else if (5 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 1; }
		else if (6 <= m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 4; }
		break;
	case Client::EProductionSystemType::FOOD:
		if (3 <= m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 5; }
		break;
	case Client::EProductionSystemType::EQUIPMENT:
		if (1 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 1; }
		else if (2 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 4; }
		else if (3 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 6; }
		else if (4 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 7; }
		else if (5 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 8; }
		else if (6 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 8; }
		else if (7 == m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 9; }
		else if (8 <= m_wpPlayer.lock()->GetCurCreatureLevel()) { iNum = 10; }
		break;
	}

	return iNum;
}

void Client::CProductionSystem::OnWorkPanel(EArchitectureType _eArchitectureType, std::shared_ptr<CArchitecture> _spArchitecture)
{
	if (m_spUIManager->GetTopPanel() != m_spProductionWorkBenchUI)
	{
		switch (_eArchitectureType)
		{
		case Client::EArchitectureType::WORKBENCH:
			m_eCurProductionSystemType = EProductionSystemType::WORKBENCH;
			break;
		case Client::EArchitectureType::BLAST_FURNACE:
			m_eCurProductionSystemType = EProductionSystemType::STOVE;
			break;
		case Client::EArchitectureType::POT:
			m_eCurProductionSystemType = EProductionSystemType::FOOD;
			break;
		case Client::EArchitectureType::WEAPON_WORKBENCH:
			m_eCurProductionSystemType = EProductionSystemType::EQUIPMENT;
			break;
		}

		if (m_spCurArchitecture)
		{
			m_spCurArchitecture = nullptr;
		}
		m_spCurArchitecture = _spArchitecture;

		m_bActive = true;
	}
}
