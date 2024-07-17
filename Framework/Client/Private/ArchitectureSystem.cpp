#include "pch.h"
#include "ArchitectureSystem.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "UIManager.h"
#include "SoundManager.h"

#include "Panel.h"
#include "RectComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"

#include "ClientModule.h"
#include "BuildManager.h"
#include "ArchitecturePrefab.h"

#include "Mineral.h"

#include "GameManager.h"
#include "InventorySystem.h"

std::shared_ptr<CArchitectureSystem> Client::CArchitectureSystem::Create(const std::shared_ptr<FArchitectureSystemDesc>& _spArchitectureSystemDesc)
{
	shared_ptr<CArchitectureSystem> spInstance = make_shared<CArchitectureSystem>();
	CHECKF(spInstance, L"CArchitectureSystem : Create Failed");
	spInstance->Initialize(_spArchitectureSystemDesc);

	return spInstance;
}

HRESULT Client::CArchitectureSystem::Initialize(const std::shared_ptr<FArchitectureSystemDesc>& _spArchitectureSystemDesc)
{
	// from sample level initialize to inventory system initialize
	{
		m_spArchitecturePanel = CUIManager::GetInstance()->FindParentsPanel(L"TestUI");
		m_spHoverImageComponent = m_spArchitecturePanel->FindComponent<CRectComponent>(L"HoverImage");
		m_spArchitecturePanel->FindComponent<CRectComponent>(L"HoverImage")->SetActive(false);
		AddIndex();
	}

	return S_OK;
}

HRESULT Client::CArchitectureSystem::BeginPlay()
{
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();
	m_wpUIManager = CUIManager::GetInstance();
	m_spInventorySystem = CGameManager::GetInstance()->GetInventorySystem();
	return S_OK;
}

int32 Client::CArchitectureSystem::PreTick(float _fDeltaSeconds)
{
	if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_B))
	{
		if (nullptr != m_spArchitecturePanel)
		{
			// 킬 때
			if (false == m_spArchitecturePanel->GetActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("KeyInputSound");
				m_spArchitecturePanel->SetActive(true);
				m_wpUIManager.lock()->SetTopPanel(m_spArchitecturePanel);
			}
			// 끌 때
			else
			{
				m_wpSoundManager.lock()->PlaySoundEx("KeyInputSound");
				m_spArchitecturePanel->SetActive(false);
				m_wpUIManager.lock()->ResetTopPanel();
			}
		}
	}
	// 빌드 모드 취소
	if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_Y) &&
		CClientModule::GetInstance()->GetBuildManager()->IsBuildMode())
	{
		CBuildManager::GetInstance()->OffBuildMode();
	}

	//if (true == m_spArchitecturePanel->GetActive())
	//{
	//	CClientModule::GetInstance()->GetBuildManager()->DestroyInput();
	//}

	return 0;
}

int32 Client::CArchitectureSystem::Tick(float _fDeltaSeconds)
{
	if (m_wpUIManager.lock()->GetTopPanel() == m_spArchitecturePanel)
	{
		FindIsInIndex();

		// 현재 top패널이 건축 패널일 시 수행합니다.
		if (CEngineModule::GetInstance()->GetInputManager()->IsMouseDown(EMouseActionType::LEFT_BUTTON))
		{
			// 클릭 사운드를 재생합니다.
			m_wpSoundManager.lock()->PlaySoundEx("NormalClickSound");

			if (-1 == m_iCurIndex)
			{
				return 0;
			}

			if (true == m_spCenterFailImageComponent->IsActive() || L"" == m_spCenterTextComponent->GetText())
			{
				return 0;
			}

			// 패널을 꺼줍니다.
			m_spArchitecturePanel->SetActive(false);
			m_wpUIManager.lock()->ResetTopPanel();

			std::vector<pair<int32, FCircleUIInfo>> iVecCurIndex;

			switch (m_eArchitectureSystemType)
			{
			case Client::EArchitectureSystemType::ARCHITECTURE:
				iVecCurIndex = m_vecWoodBulidIndex;
				break;
			case Client::EArchitectureSystemType::GROCERIES:
				iVecCurIndex = m_vecGroceriesIndex;
				break;
			case Client::EArchitectureSystemType::INFRA:
				iVecCurIndex = m_vecInfraIndex;
				break;
			case Client::EArchitectureSystemType::PRODUCTION:
				iVecCurIndex = m_vecProductionIndex;
				break;
			}

			// 여기서 건축물 짓기를 실행합니다.
			CClientModule::GetInstance()->GetBuildManager()->OnBuildMode(iVecCurIndex[m_iCurIndex].second.eArchitectureType);
		}

		if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_1))
		{
			if (m_iTapIndex > 0)
			{
				m_iTapIndex--;
			}
			else
			{
				m_iTapIndex = 3;
			}

			m_wpSoundManager.lock()->PlaySoundEx("TabChangeSound");
			ChangeIndex(static_cast<EArchitectureSystemType>(m_iTapIndex));
		}
		if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_3))
		{
			if (m_iTapIndex < 3)
			{
				m_iTapIndex++;
			}
			else
			{
				m_iTapIndex = 0;
			}

			m_wpSoundManager.lock()->PlaySoundEx("TabChangeSound");
			ChangeIndex(static_cast<EArchitectureSystemType>(m_iTapIndex));
		}
	}

	return 0;
}

int32 Client::CArchitectureSystem::PostTick(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Client::CArchitectureSystem::EndPlay()
{
	return S_OK;
}

void Client::CArchitectureSystem::Release()
{
	for (auto& iter : m_vecItemBoxIndex)
	{
		iter.spBackGroundImageComponent = nullptr;
		iter.spLayerImageComponent = nullptr;
		iter.spItemImageComponent = nullptr;
		iter.spItemTitleTextComponent = nullptr;
		iter.spItemCurCountTextComponent = nullptr;
		iter.spItemMaxCountTextComponent = nullptr;
		iter.spSlashTextComponent = nullptr;
	}
	m_vecItemBoxIndex.clear();

	for (auto& pair : m_vecWoodBulidIndex)
	{
		pair.second.spImageComponent = nullptr;
		pair.second.spTextComponent = nullptr;
	}
	m_vecWoodBulidIndex.clear();

	for (auto& pair : m_vecGroceriesIndex)
	{
		pair.second.spImageComponent = nullptr;
		pair.second.spTextComponent = nullptr;
	}
	m_vecGroceriesIndex.clear();

	for (auto& pair : m_vecProductionIndex)
	{
		pair.second.spImageComponent = nullptr;
		pair.second.spTextComponent = nullptr;
	}
	m_vecProductionIndex.clear();

	for (auto& pair : m_vecInfraIndex)
	{
		pair.second.spImageComponent = nullptr;
		pair.second.spTextComponent = nullptr;
	}
	m_vecInfraIndex.clear();

	m_vecTabPosition.clear();

	m_spChildTitleTextComponent = nullptr;
	m_spChildDescriptionTextComponent = nullptr;
	m_spChildPanel = nullptr;

	m_spCenterImageComponent = nullptr;
	m_spCenterTextComponent = nullptr;
	m_spTabCurImageComponent = nullptr;
	m_spHoverImageComponent = nullptr;
	m_spArchitecturePanel = nullptr;

	m_spInventorySystem = nullptr;
}

void Client::CArchitectureSystem::AddIndex()
{
	m_spChildPanel = static_pointer_cast<CPanel>(m_spArchitecturePanel->FindChildGameObject(L"ChildInfoPanel"));
	m_spChildPanel->SetActive(false);

	m_spChildTitleTextComponent = m_spChildPanel->FindComponent<CTextComponent>(L"TitleText");
	m_spChildDescriptionTextComponent = m_spChildPanel->FindComponent<CTextComponent>(L"DescriptionText");

	m_vecTabPosition.push_back(static_cast<int32>(m_spArchitecturePanel->FindComponent<CRectComponent>(L"TabBaseImage0")->GetCenterPosition().x));
	m_vecTabPosition.push_back(static_cast<int32>(m_spArchitecturePanel->FindComponent<CRectComponent>(L"TabBaseImage1")->GetCenterPosition().x));
	m_vecTabPosition.push_back(static_cast<int32>(m_spArchitecturePanel->FindComponent<CRectComponent>(L"TabBaseImage2")->GetCenterPosition().x));
	m_vecTabPosition.push_back(static_cast<int32>(m_spArchitecturePanel->FindComponent<CRectComponent>(L"TabBaseImage3")->GetCenterPosition().x));

	SetCenterImageComponent(m_spArchitecturePanel->FindComponent<CImageComponent>(L"CenterImage"));
	SetCenterTextComponent(m_spArchitecturePanel->FindComponent<CTextComponent>(L"CenterText"));
	m_spCenterFailImageComponent = m_spArchitecturePanel->FindComponent<CImageComponent>(L"FailImage");

	m_spCenterImageComponent->SetActive(false);
	m_spCenterTextComponent->SetActive(false);
	m_spCenterFailImageComponent->SetActive(false);

	// text를 한글로 변환해 넣어줍니다.
	m_spArchitecturePanel->FindComponent<CTextComponent>(L"TabText0")->SetText(L"건축");
	m_spArchitecturePanel->FindComponent<CTextComponent>(L"TabText1")->SetText(L"식료품");
	m_spArchitecturePanel->FindComponent<CTextComponent>(L"TabText2")->SetText(L"인프라");
	m_spArchitecturePanel->FindComponent<CTextComponent>(L"TabText3")->SetText(L"생산");

	m_spTabCurImageComponent = m_spArchitecturePanel->FindComponent<CRectComponent>(L"TabCurImage");

	for (int32 i = 0; i < 12; i++)
	{
		FCircleUIInfo tCircleInfo;

		tCircleInfo.fMinAngle = i * 30.0f;
		tCircleInfo.fMaxAngle = (i + 1) * 30.0f;

		std::wstring wstrImageName = L"WoodImage" + to_wstring(i);
		std::wstring wstrTextName = L"WoodText" + to_wstring(i);

		if (i < m_iWoodBuildIndex)
		{
			tCircleInfo.spImageComponent = dynamic_pointer_cast<CImageComponent>(m_spArchitecturePanel->FindComponent<CRectComponent>(wstrImageName));
			tCircleInfo.spTextComponent = dynamic_pointer_cast<CTextComponent>(m_spArchitecturePanel->FindComponent<CRectComponent>(wstrTextName));

			tCircleInfo.wstrTextureName = tCircleInfo.spImageComponent->GetTextureName();
			
			switch (i)
			{
			case 0:
				tCircleInfo.spTextComponent->SetText(L"나무 토대");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_FOUNDATION;
				tCircleInfo.wstrDescriptionText = L"위에 벽이나 시설을 지을 수 있는 토대.\n나무로 만들어서 약하다.";
				break;
			case 1:
				tCircleInfo.spTextComponent->SetText(L"나무 벽");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_WALL;
				tCircleInfo.wstrDescriptionText = L"토대 위에 짓는 침입자 방지용 벽.\n나무로 만들어서 약하다.";
				break;
			case 2:
				tCircleInfo.spTextComponent->SetText(L"창문 딸린 나무 벽");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_WINDOW_WALL;
				tCircleInfo.wstrDescriptionText = L"토대 위에 짓는 창문이 딸린 침입자 방지용 벽.\n나무로 만들어서 약하다.";
				break;
			case 3:
				tCircleInfo.spTextComponent->SetText(L"나무 삼각 벽");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_TRYANGLE_WALL;
				tCircleInfo.wstrDescriptionText = L"토대 위에 짓는 나무 삼각 벽.\n나무로 만들어서 약하다.";
				break;
			case 4:
				tCircleInfo.spTextComponent->SetText(L"나무 지붕");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_ROOF;
				tCircleInfo.wstrDescriptionText = L"벽 위에 짓는 비바람 방지용 지붕.\n나무로 만들어서 약하다.";
				break;
			case 5:
				tCircleInfo.spTextComponent->SetText(L"나무 경사 지붕");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_SLOPE_ROOF;
				tCircleInfo.wstrDescriptionText = L"벽 위에 짓는 비바람 방지용 경사 지붕.\n나무로 만들어서 약하다.";
				break;
			case 6:
				tCircleInfo.spTextComponent->SetText(L"나무 계단");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_STAIRS;
				tCircleInfo.wstrDescriptionText = L"토대나 지붕 및에 배치해 오르내리는 계단.\n나무로 만들어서 약하다.";
				break;
			case 7:
				tCircleInfo.spTextComponent->SetText(L"나무 문");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_DOOR;
				tCircleInfo.wstrDescriptionText = L"토대 위에 짓는 입구용 문.\n나무로 만들어서 약하다.";
				break;
			}
		}

		m_vecWoodBulidIndex.push_back({ i, tCircleInfo });
	}

	AddInfraIndex();
	AddProductionIndex();
	AddGroceriesIndex();

	AddChildItemBoxIndex();
}

void Client::CArchitectureSystem::AddInfraIndex()
{
	for (int32 i = 0; i < 12; i++)
	{
		FCircleUIInfo tCircleInfo;

		tCircleInfo.fMinAngle = i * 30.0f;
		tCircleInfo.fMaxAngle = (i + 1) * 30.0f;

		std::wstring wstrImageName = L"WoodImage" + to_wstring(i);
		std::wstring wstrTextName = L"WoodText" + to_wstring(i);

		if (i < m_iInfraIndex)
		{
			tCircleInfo.spImageComponent = dynamic_pointer_cast<CImageComponent>(m_spArchitecturePanel->FindComponent<CRectComponent>(wstrImageName));
			tCircleInfo.spTextComponent = dynamic_pointer_cast<CTextComponent>(m_spArchitecturePanel->FindComponent<CRectComponent>(wstrTextName));

			switch (i)
			{
			case 0:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_PalBoxV2";
				tCircleInfo.wstrText = L"팰 상자";
				tCircleInfo.eArchitectureType = EArchitectureType::PAL_BOX;
				tCircleInfo.wstrDescriptionText = L"포획한 팰을 보관해두는 시설.\n박스 내 팰은 천천히 HP를 회복한다.\n이 시설을 중심으로 한 범위가 거점이 된다.";
				break;
			case 1:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_ItemChest";
				tCircleInfo.wstrText = L"나무 상자";
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_CHEST;
				tCircleInfo.wstrDescriptionText = L"아이템을 수납할 수 있다.\n나무로 만들어서 약하다.\n귀중품을 넣기엔 좀 불안하다.";
				break;
			case 2:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_ItemChest_02";
				tCircleInfo.wstrText = L"금속 상자";
				tCircleInfo.eArchitectureType = EArchitectureType::IRON_BOX;
				tCircleInfo.wstrDescriptionText = L"아이템을 수납할 수 있다.\n금속으로 보강해 좀 더 튼튼하다.\n커진 만큼 수납 용량도 늘어났다.";
				break;
			case 3:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_PlayerBed_02";
				tCircleInfo.wstrText = L"하급 침대";
				tCircleInfo.eArchitectureType = EArchitectureType::HUMAN_BED;
				tCircleInfo.wstrDescriptionText = L"인간용 너덜너덜한 침대.\n다쳤거나 밤에 잘 때 사용한다.\n위에 지붕이 없으면 불안해서 잠들 수가 없다.";
				break;
			case 4:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_MedicalPalBed_02";
				tCircleInfo.wstrText = L"짚 팰 침대";
				tCircleInfo.eArchitectureType = EArchitectureType::PAL_BED;
				tCircleInfo.wstrDescriptionText = L"팰용 짚 침대.\n팰이 다쳤거나 밤에 잘 때 사용한다.\n딱딱하지만 없는 것보다는 낫다.";
				break;
			}
		}

		m_vecInfraIndex.push_back({ i, tCircleInfo });
	}
}

void Client::CArchitectureSystem::AddProductionIndex()
{
	for (int32 i = 0; i < 12; i++)
	{
		FCircleUIInfo tCircleInfo;

		tCircleInfo.fMinAngle = i * 30.0f;
		tCircleInfo.fMaxAngle = (i + 1) * 30.0f;

		std::wstring wstrImageName = L"WoodImage" + to_wstring(i);
		std::wstring wstrTextName = L"WoodText" + to_wstring(i);

		if (i < m_iProductionIndex)
		{
			tCircleInfo.spImageComponent = dynamic_pointer_cast<CImageComponent>(m_spArchitecturePanel->FindComponent<CRectComponent>(wstrImageName));
			tCircleInfo.spTextComponent = dynamic_pointer_cast<CTextComponent>(m_spArchitecturePanel->FindComponent<CRectComponent>(wstrTextName));

			switch (i)
			{
			case 0:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_WorkBench";
				tCircleInfo.wstrText = L"원시 작업대";
				tCircleInfo.eArchitectureType = EArchitectureType::WORKBENCH;
				tCircleInfo.wstrDescriptionText = L"간단한 아이템을 제작하는 원시적인 작업대.\n수작업이 가능한 팰이 필요하다.";
				break;
			case 1:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_WeaponFactoryBase";
				tCircleInfo.wstrText = L"무기 제작대";
				tCircleInfo.eArchitectureType = EArchitectureType::WEAPON_WORKBENCH;
				tCircleInfo.wstrDescriptionText = L"무기나 탄약을 생산하는 공장.\n수작업이 가능한 팰이 필요하다.";
				break;
			case 2:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_RepairBench";
				tCircleInfo.wstrText = L"수리대";
				tCircleInfo.eArchitectureType = EArchitectureType::REPAIR_BENCH;
				tCircleInfo.wstrDescriptionText = L"파손된 도구를 수리하는 작업대.\n소재를 소모하여 수리할 수 있다.";
				break;
			case 3:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_StationDeforest2";
				tCircleInfo.wstrText = L"벌목장";
				tCircleInfo.eArchitectureType = EArchitectureType::LUMBERYARD;
				tCircleInfo.wstrDescriptionText = L"목재을(를) 생산하는 시설.\n나무를 자르는 작업은 고되므로 체력이 필요하다.\n벌목에 자신 있는 팰에게 맡기자.";
				break;
			case 4:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_StonePit";
				tCircleInfo.wstrText = L"채석장";
				tCircleInfo.eArchitectureType = EArchitectureType::QUARRY;
				tCircleInfo.wstrDescriptionText = L"돌을(를) 생산하는 시설.\n돌을(를) 파내는 작업은 고되므로 체력이 필요하다.\n채굴에 자신 있는 팰에게 맡기자.";
				break;
			case 5:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_FlourMill";
				tCircleInfo.wstrText = L"제분기";
				tCircleInfo.eArchitectureType = EArchitectureType::MILL;
				tCircleInfo.wstrDescriptionText = L"밀을(를) 빻아 밀가루을(를)\n생산할 수 있는 시설.\n물레방아를 돌리는 \"관개\" 적성의 팰이 필요하다.";
				break;
			case 6:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_BlastFurnace";
				tCircleInfo.wstrText = L"원시적인 화로";
				tCircleInfo.eArchitectureType = EArchitectureType::BLAST_FURNACE;
				tCircleInfo.wstrDescriptionText = L"금속 주괴을(를) 제련할 수 있다.\n품질이 썩 좋지 않고 속도가 느리다.\n화염 팰이 불을 붙여줘야 한다.";
				break;
			case 7:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_SphereFactory_Black_Base";
				tCircleInfo.wstrText = L"스피어 제작대";
				tCircleInfo.eArchitectureType = EArchitectureType::SPHERE_TABLE;
				tCircleInfo.wstrDescriptionText = L"팰 포획용 스피어 제작 공장.\n아직 작업장이 협소해 스피어를 빠르게 생산하기는 힘들다.\n수작업이 가능한 팰이 필요하다.";
				break;
			}
		}

		m_vecProductionIndex.push_back({ i, tCircleInfo });
	}
}

void Client::CArchitectureSystem::AddGroceriesIndex()
{
	for (int32 i = 0; i < 12; i++)
	{
		FCircleUIInfo tCircleInfo;

		tCircleInfo.fMinAngle = i * 30.0f;
		tCircleInfo.fMaxAngle = (i + 1) * 30.0f;

		std::wstring wstrImageName = L"WoodImage" + to_wstring(i);
		std::wstring wstrTextName = L"WoodText" + to_wstring(i);

		if (i < m_iGroceriesIndex)
		{
			tCircleInfo.spImageComponent = dynamic_pointer_cast<CImageComponent>(m_spArchitecturePanel->FindComponent<CRectComponent>(wstrImageName));
			tCircleInfo.spTextComponent = dynamic_pointer_cast<CTextComponent>(m_spArchitecturePanel->FindComponent<CRectComponent>(wstrTextName));

			switch (i)
			{
			case 0:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_CampFire";
				tCircleInfo.wstrText = L"모닥불";
				tCircleInfo.eArchitectureType = EArchitectureType::CAMPFIRE;
				tCircleInfo.wstrDescriptionText = L"식재료를 조리할 떄 필요한 시설.\n간단한 재료를 익해는 정도만 가능하다.\n화염 팰이 불을 붙여줘야 한다.";
				break;
			case 1:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_CookingStove";
				tCircleInfo.wstrText = L"조리 냄비";
				tCircleInfo.eArchitectureType = EArchitectureType::POT;
				tCircleInfo.wstrDescriptionText = L"식재료를 조리할 떄 필요한 시설.\n냄비를 활용한 덕분에 조리의 폭이 넓어졌다.\n화염 팰이 불을 붙여줘야 한다.";
				break;
			case 2:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_PalFoodBox";
				tCircleInfo.wstrText = L"먹이 상자";
				tCircleInfo.eArchitectureType = EArchitectureType::FEEDING_BOX;
				tCircleInfo.wstrDescriptionText = L"팰의 먹이를 넣어두는 보관함.\n배가 고픈 팰이 여기서 먹이를 먹는다.\n먹이가 떨어지지 않게 관리하자.";
				break;
			case 3:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_FarmBlockV2_Berries";
				tCircleInfo.wstrText = L"열매 농원";
				tCircleInfo.eArchitectureType = EArchitectureType::BERRY_FARM;
				tCircleInfo.wstrDescriptionText = L"빨간 열매을(를) 기를 수 있는 농원.\n금방 수확할 수 있지만 먹어도 그다치 배가 차지 않는다.\n파종 및 관개, 수확 작업에 여러 마리의 팰이 필요하다.";
				break;
			case 4:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_FarmBlock";
				tCircleInfo.wstrText = L"밀 농원";
				tCircleInfo.eArchitectureType = EArchitectureType::WHEAT_FARM;
				tCircleInfo.wstrDescriptionText = L"밀을(를) 기를 수 있는 농원.\n수확 까지 걸리는 시간은 평균.\n파종 및 관개, 수확 작업에 여러 마리의 팰이 필요하다.";
				break;
			case 5:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_MonsterFarm";
				tCircleInfo.wstrText = L"가축목장";
				tCircleInfo.eArchitectureType = EArchitectureType::RANCH;
				tCircleInfo.wstrDescriptionText = L"양이나 닭 등의 팰을 방목하는 목장.\n특수한 파트너 스킬을 가진 팰을\n배치하면 아이템을 생산한다.";
				break;
			}
		}

		m_vecGroceriesIndex.push_back({ i, tCircleInfo });
	}
}

void Client::CArchitectureSystem::AddChildItemBoxIndex()
{
	for (int32 i = 0; i < 3; i++)
	{
		FInfoBox tInfoBox;
		//CheckBoxImage0
		std::wstring wstrBackgroundImageName = L"CheckBoxImage" + to_wstring(i);
		std::wstring wstrLayerImageName = L"BoxBaseImage" + to_wstring(i);
		std::wstring wstrItemImageName = L"ItemCheckBoxImage" + to_wstring(i);

		std::wstring wstrItemTitleTextName = L"ItemCheckBoxText" + to_wstring(i);
		std::wstring wstrItemCurCountTextName = L"ItemCurCountText" + to_wstring(i);
		std::wstring wstrItemMaxCountTextName = L"ItemMaxCountText" + to_wstring(i);
		std::wstring wstrSlashTextName = L"ItemCountBarText" + to_wstring(i);


		tInfoBox.spBackGroundImageComponent = m_spChildPanel->FindComponent<CImageComponent>(wstrBackgroundImageName);
		tInfoBox.spLayerImageComponent = m_spChildPanel->FindComponent<CImageComponent>(wstrLayerImageName);
		tInfoBox.spItemImageComponent = m_spChildPanel->FindComponent<CImageComponent>(wstrItemImageName);

		tInfoBox.spItemTitleTextComponent = m_spChildPanel->FindComponent<CTextComponent>(wstrItemTitleTextName);
		tInfoBox.spItemCurCountTextComponent = m_spChildPanel->FindComponent<CTextComponent>(wstrItemCurCountTextName);
		tInfoBox.spItemMaxCountTextComponent = m_spChildPanel->FindComponent<CTextComponent>(wstrItemMaxCountTextName);
		tInfoBox.spSlashTextComponent = m_spChildPanel->FindComponent<CTextComponent>(wstrSlashTextName);

		tInfoBox.vBackGroundSuccessColor = tInfoBox.spBackGroundImageComponent->GetColor();
		tInfoBox.vItemCurCountSuccessColor = tInfoBox.spItemCurCountTextComponent->GetColor();
		tInfoBox.vFailColor = Vector4(1.0f, 0.2f, 0.2f, 1.0f);

		m_vecItemBoxIndex.push_back(tInfoBox);
	}
}

bool Client::CArchitectureSystem::IsInAngle(float _fMinAngle, float _fMaxAngle, float _fDegreeAngle)
{
	if (_fMinAngle <= _fDegreeAngle && _fMaxAngle > _fDegreeAngle)
	{
		return true;
	}

	return false;
}

void Client::CArchitectureSystem::FindIsInIndex()
{
	std::vector<pair<int32, FCircleUIInfo>> iVecCurIndex;

	switch (m_eArchitectureSystemType)
	{
	case Client::EArchitectureSystemType::ARCHITECTURE:
		iVecCurIndex = m_vecWoodBulidIndex;
		break;
	case Client::EArchitectureSystemType::GROCERIES:
		iVecCurIndex = m_vecGroceriesIndex;
		break;
	case Client::EArchitectureSystemType::INFRA:
		iVecCurIndex = m_vecInfraIndex;
		break;
	case Client::EArchitectureSystemType::PRODUCTION:
		iVecCurIndex = m_vecProductionIndex;
		break;
	}

	for (auto& pair : iVecCurIndex)
	{
		if (-1.0f < m_wpUIManager.lock()->GetCircleDegreeAngle())
		{
			if (IsInAngle(pair.second.fMinAngle, pair.second.fMaxAngle, m_wpUIManager.lock()->GetCircleDegreeAngle()))
			{
				m_iCurIndex = pair.first;
			}
		}
		else if (-1.0f == m_wpUIManager.lock()->GetCircleDegreeAngle())
		{
			m_iCurIndex = -1;
		}
	}

	ActiveHoverImage(iVecCurIndex);
}

void Client::CArchitectureSystem::ActiveHoverImage(std::vector<pair<int32, FCircleUIInfo>> _vecCurIndex)
{
	if (-1 == m_iCurIndex)
	{
		m_spChildPanel->SetActive(false);

		m_spHoverImageComponent->SetActive(false);
		m_spCenterImageComponent->SetActive(false);
		m_spCenterTextComponent->SetActive(false);
		m_spCenterFailImageComponent->SetActive(false);
		return;
	}

	if (L"" == _vecCurIndex[m_iCurIndex].second.wstrTextureName)
	{
		m_spChildPanel->SetActive(false);
		m_spCenterFailImageComponent->SetActive(false);
	}
	else
	{
		m_spChildTitleTextComponent->SetText(_vecCurIndex[m_iCurIndex].second.wstrText);
		m_spChildDescriptionTextComponent->SetText(_vecCurIndex[m_iCurIndex].second.wstrDescriptionText);
		m_spChildPanel->SetActive(true);
		FindItemBox(_vecCurIndex[m_iCurIndex].second.eArchitectureType);
	}

	m_spHoverImageComponent->SetRectAngle(-(_vecCurIndex[m_iCurIndex].second.fMinAngle));
	m_spHoverImageComponent->SetActive(true);

	m_spCenterImageComponent->SetTextureFileName(_vecCurIndex[m_iCurIndex].second.wstrTextureName);
	m_spCenterTextComponent->SetText(_vecCurIndex[m_iCurIndex].second.wstrText);
	m_spCenterImageComponent->SetActive(true);
	m_spCenterTextComponent->SetActive(true);

	if (m_iPreIndex != m_iCurIndex)
	{
		m_wpSoundManager.lock()->PlaySoundEx("HoverSound");
		m_iPreIndex = m_iCurIndex;
	}
}

void Client::CArchitectureSystem::ChangeIndex(EArchitectureSystemType _eArchitectureSystemType)
{
	if (m_eArchitectureSystemType != _eArchitectureSystemType)
	{
		std::vector<pair<int32, FCircleUIInfo>> vecPreIndex;
		std::vector<pair<int32, FCircleUIInfo>> vecCurIndex;

		switch (_eArchitectureSystemType)
		{
		case Client::EArchitectureSystemType::ARCHITECTURE:
			m_spTabCurImageComponent->SetCenterPosition(Vector2((float)m_vecTabPosition[static_cast<int32>(EArchitectureSystemType::ARCHITECTURE)], (float)m_spTabCurImageComponent->GetCenterPosition().y));
			vecCurIndex = m_vecWoodBulidIndex;
			break;
		case Client::EArchitectureSystemType::GROCERIES:
			m_spTabCurImageComponent->SetCenterPosition(Vector2((float)m_vecTabPosition[static_cast<int32>(EArchitectureSystemType::GROCERIES)], (float)m_spTabCurImageComponent->GetCenterPosition().y));
			vecCurIndex = m_vecGroceriesIndex;
			break;
		case Client::EArchitectureSystemType::INFRA:
			m_spTabCurImageComponent->SetCenterPosition(Vector2((float)m_vecTabPosition[static_cast<int32>(EArchitectureSystemType::INFRA)], (float)m_spTabCurImageComponent->GetCenterPosition().y));
			vecCurIndex = m_vecInfraIndex;
			break;
		case Client::EArchitectureSystemType::PRODUCTION:
			m_spTabCurImageComponent->SetCenterPosition(Vector2((float)m_vecTabPosition[static_cast<int32>(EArchitectureSystemType::PRODUCTION)], (float)m_spTabCurImageComponent->GetCenterPosition().y));
			vecCurIndex = m_vecProductionIndex;
			break;
		default:
			break;
		}

		switch (m_eArchitectureSystemType)
		{
		case Client::EArchitectureSystemType::ARCHITECTURE:
			vecPreIndex = m_vecWoodBulidIndex;
			break;
		case Client::EArchitectureSystemType::GROCERIES:
			vecPreIndex = m_vecGroceriesIndex;
			break;
		case Client::EArchitectureSystemType::INFRA:
			vecPreIndex = m_vecInfraIndex;
			break;
		case Client::EArchitectureSystemType::PRODUCTION:
			vecPreIndex = m_vecProductionIndex;
			break;
		default:
			break;
		}

		for (auto& pair : vecCurIndex)
		{
			if (L"" == pair.second.wstrTextureName)
			{
				if (nullptr != vecPreIndex[pair.first].second.spImageComponent)
				{
					vecPreIndex[pair.first].second.spImageComponent->SetActive(false);
					vecPreIndex[pair.first].second.spTextComponent->SetActive(false);
				}
				continue;
			}

			if (nullptr != pair.second.spImageComponent)
			{
				pair.second.spImageComponent->SetActive(true);
				pair.second.spTextComponent->SetActive(true);

				pair.second.spImageComponent->SetTextureFileName(pair.second.wstrTextureName);
				pair.second.spTextComponent->SetText(pair.second.wstrText);
			}
		}
		m_eArchitectureSystemType = _eArchitectureSystemType;
	}
}

void Client::CArchitectureSystem::IsActiveItemBox(bool _bActive, FInfoBox _tInfoBox)
{
	_tInfoBox.spBackGroundImageComponent->SetActive(_bActive);
	_tInfoBox.spLayerImageComponent->SetActive(_bActive);
	_tInfoBox.spItemImageComponent->SetActive(_bActive);
	_tInfoBox.spItemTitleTextComponent->SetActive(_bActive);
	_tInfoBox.spItemCurCountTextComponent->SetActive(_bActive);
	_tInfoBox.spItemMaxCountTextComponent->SetActive(_bActive);
	_tInfoBox.spSlashTextComponent->SetActive(_bActive);
}

void Client::CArchitectureSystem::FindItemBox(EArchitectureType _eArchitectureType)
{
	if (nullptr == CBuildManager::GetInstance()->FindHousingPrefab(_eArchitectureType))
	{
		for (int32 i = 0; i < 3; i++)
		{
			IsActiveItemBox(false, m_vecItemBoxIndex[i]);
		}

		return;
	}

	int32 iMineralIndex = static_cast<int32>(CBuildManager::GetInstance()->FindHousingPrefab(_eArchitectureType)->GetNeededMineralsVecSize());
	
	int32 iSuccessIndex = 0;
	for (int32 i = 0; i < 3; i++)
	{
		if (i < iMineralIndex)
		{
			IsActiveItemBox(true, m_vecItemBoxIndex[i]);
			// 해당 타입에 맞는 아이템 재료와 갯수를 찾아옵니다.
			std::optional<pair<std::shared_ptr<CMineral>, uint32>> prItem = CBuildManager::GetInstance()->GetArchitectureNeededMinerals(_eArchitectureType, i);
			// 아이템 타입
			EItemType eItemType = m_wpUIManager.lock()->ChangeMineralType(prItem.value().first->GetMineralType());
			m_vecItemBoxIndex[i].spItemImageComponent->SetTextureFileName(m_wpUIManager.lock()->FindItemUITextureName(eItemType));
			// 아이템 이름
			m_vecItemBoxIndex[i].spItemTitleTextComponent->SetText(m_wpUIManager.lock()->FindItemName(eItemType));
			// 개수
			m_vecItemBoxIndex[i].spItemMaxCountTextComponent->SetText(to_wstring(prItem.value().second));

			// 인벤토리에 있는 아이템 개수로 텍스트를 업데이트 해줍니다.
			int32 iCurNum = m_spInventorySystem->FindInventoryItemTotalNum(m_wpUIManager.lock()->ChangeMineralType(prItem.value().first->GetMineralType()));
			m_vecItemBoxIndex[i].spItemCurCountTextComponent->SetText(to_wstring(iCurNum));
			// 인벤토리 개수가 충족되었으면 원래색, 아니면 빨간색으로 바꿔줍니다.
			if (iCurNum < static_cast<int32>(prItem.value().second))
			{
				m_vecItemBoxIndex[i].spBackGroundImageComponent->SetColor(m_vecItemBoxIndex[i].vFailColor);
				m_vecItemBoxIndex[i].spItemCurCountTextComponent->SetColor(m_vecItemBoxIndex[i].vFailColor);
			}
			else
			{
				m_vecItemBoxIndex[i].spBackGroundImageComponent->SetColor(m_vecItemBoxIndex[i].vBackGroundSuccessColor);
				m_vecItemBoxIndex[i].spItemCurCountTextComponent->SetColor(m_vecItemBoxIndex[i].vItemCurCountSuccessColor);
				iSuccessIndex++;
			}
		}
		else
		{
			IsActiveItemBox(false, m_vecItemBoxIndex[i]);
		}
	}

	if (iMineralIndex <= iSuccessIndex)
	{
		m_spCenterFailImageComponent->SetActive(false);
		m_spCenterTextComponent->SetColor(Vector4(1.0f ,1.0f, 1.0f, 1.0f));
	}
	else
	{
		m_spCenterFailImageComponent->SetActive(true);
		m_spCenterTextComponent->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	}

	if (iSuccessIndex == 0)
	{
		m_spCenterFailImageComponent->SetActive(true);
		m_spCenterTextComponent->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	}
	//if (L"" == m_spCenterTextComponent->GetText())
	//{
	//	m_spCenterFailImageComponent->SetActive(false);
	//}
}

const pair<std::wstring, std::wstring> Client::CArchitectureSystem::GetItemTypeImageName(EMineralType _emineralType)
{
	std::wstring wstrImageName = L"";
	std::wstring wstrTextName = L"";

	switch (_emineralType)
	{
	case Client::EMineralType::GOLD_COIN:
		wstrImageName = L"T_itemicon_Material_Money";
		wstrTextName = L"금화";
		break;
	case Client::EMineralType::STONE:
		wstrImageName = L"T_itemicon_Material_Stone";
		wstrTextName = L"돌";
		break;
	case Client::EMineralType::WOOD:
		wstrImageName = L"T_itemicon_Material_Wood";
		wstrTextName = L"목재";
		break;
	case Client::EMineralType::LEATHER:
		wstrImageName = L"T_itemicon_Material_Leather";
		wstrTextName = L"가죽";
		break;
	case Client::EMineralType::COAL:
		wstrImageName = L"T_itemicon_Material_Coal";
		wstrTextName = L"석탄";
		break;
	case Client::EMineralType::WOOL:
		wstrImageName = L"T_itemicon_Material_Wool";
		wstrTextName = L"양털";
		break;
	case Client::EMineralType::IRON_INGOT:
		wstrImageName = L"T_itemicon_Material_CopperIngot";
		wstrTextName = L"제련 주괴";
		break;
	}

	return { wstrImageName,  wstrTextName };
}

//EItemType Client::CArchitectureSystem::ChangeMineralType(EMineralType _eMineralType)
//{
//	EItemType eItemType = EItemType::ENUM_END;
//
//	switch (_eMineralType)
//	{
//	case Client::EMineralType::COAL:
//		eItemType = EItemType::COAL;
//		break;
//	case Client::EMineralType::GOLD_COIN:
//		eItemType = EItemType::GOLD_COIN;
//		break;
//	case Client::EMineralType::IRON:
//		eItemType = EItemType::IRON;
//		break;
//	case Client::EMineralType::LEATHER:
//		eItemType = EItemType::LEATHER;
//		break;
//	case Client::EMineralType::STONE:
//		eItemType = EItemType::STONE;
//		break;
//	case Client::EMineralType::WOOD:
//		eItemType = EItemType::WOOD;
//		break;
//	case Client::EMineralType::WOOL:
//		eItemType = EItemType::WOOL;
//		break;
//	case Client::EMineralType::CLOTH:
//		eItemType = EItemType::CLOTH;
//		break;
//	}
//
//	return eItemType;
//}
