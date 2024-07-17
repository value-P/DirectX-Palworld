#include "pch.h"
#include "PalBoxSystem.h"

// client manager
#include "ClientLoop.h"
#include "BuildManager.h"
#include "UIManager.h"
#include "GameManager.h"

// gameobject
#include "Player.h"
#include "Monster.h"
#include "Panel.h"

// component
#include "RectComponent.h"
#include "ImageComponent.h"
#include "ProgressBarComponent.h"
#include "TextComponent.h"
#include "ColliderComponent.h"

// monster
#include "BerryGoat.h"
#include "Boar.h"
#include "ChickenPal.h"
#include "DreamDemon.h"
#include "HedgeHog.h"
#include "Kitsunebi.h"
#include "Penguin.h"
#include "PinkCat.h"
#include "SheepBall.h"
#include "WoolFox.h"

// engine manager
#include "EngineModule.h"
#include "InputManager.h"

// engine
#include "World.h"
#include "Level.h"

std::shared_ptr<CPalBoxSystem> Client::CPalBoxSystem::Create(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc)
{
	shared_ptr<CPalBoxSystem> spInstance = make_shared<CPalBoxSystem>();
	CHECKF(spInstance, L"CPalBoxSystem : Create Failed");
	spInstance->Initialize(_spContainerSystemDesc);

	return spInstance;
}

HRESULT Client::CPalBoxSystem::Initialize(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc)
{
	if (FAILED(CContainerSystem::Initialize(_spContainerSystemDesc))) { return E_FAIL; }

	// from sample level initialize to inventory system initialize.
	{
		m_spContainerParentPanel		= CUIManager::GetInstance()->FindParentsPanel(L"PalboxPanel");
		m_spPalboxLeftChildPanel		= CUIManager::GetInstance()->FindChildPanel(L"PalboxPanel", L"PalboxLeft");
		m_spPalboxRightChildPanel		= CUIManager::GetInstance()->FindChildPanel(L"PalboxPanel", L"PalboxRight");
		m_spPalboxRightNodataChildPanel = CUIManager::GetInstance()->FindChildPanel(L"PalboxPanel", L"PalboxRightNodata");
		m_spPalboxMidChildPanel			= CUIManager::GetInstance()->FindChildPanel(L"PalboxPanel", L"PalboxMid");
		m_spPalboxBottomChildPanel		= CUIManager::GetInstance()->FindChildPanel(L"PalboxPanel", L"PalboxBottom");
		m_spMouseChildPanel				= CUIManager::GetInstance()->FindChildPanel(L"PalboxPanel", L"PalboxMouse");
	}

	{
		m_wpLeftHoverImage		  = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"T_palbox_left_hover");
		m_wpMidHoverImage		  = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"mid_pal_hover");
		m_wpBottomHoverImage	  = m_spPalboxBottomChildPanel->FindComponent<CRectComponent>(L"bottom_pal_hover");
		m_wpSortHoverImage		  = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"mid_sort_hover");
		m_wpCurMouseSlotComponent = m_spMouseChildPanel->FindComponent<CImageComponent>(L"mouse_pal_slot");

		m_wpLeftHoverImage.lock()->SetActive(false);
		m_wpMidHoverImage.lock()->SetActive(false);
		m_wpBottomHoverImage.lock()->SetActive(false);
		m_wpSortHoverImage.lock()->SetActive(false);
		m_wpCurMouseSlotComponent.lock()->SetActive(true);
	}

	{
		m_vecPageChangeHovers.resize(16);
		m_vecPageChangeHovers[0] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_0");
		m_vecPageChangeHovers[1] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_1");
		m_vecPageChangeHovers[2] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_2");
		m_vecPageChangeHovers[3] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_3");
		m_vecPageChangeHovers[4] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_4");
		m_vecPageChangeHovers[5] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_5");
		m_vecPageChangeHovers[6] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_6");
		m_vecPageChangeHovers[7] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_7");
		m_vecPageChangeHovers[8] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_8");
		m_vecPageChangeHovers[9] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_9");
		m_vecPageChangeHovers[10] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_10");
		m_vecPageChangeHovers[11] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_11");
		m_vecPageChangeHovers[12] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_12");
		m_vecPageChangeHovers[13] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_13");
		m_vecPageChangeHovers[14] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_14");
		m_vecPageChangeHovers[15] = m_spPalboxMidChildPanel->FindComponent<CRectComponent>(L"page_change_hover_15");

		for (int32 i = 0; i < m_vecPageChangeHovers.size(); i++)
		{
			m_vecPageChangeHovers[i].lock()->SetActive(false);
		}
	}

	// left panel
	{
		m_vecLeftPanelPalStatus.resize(30);
		m_vecLeftPanelPalStatus[0] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_hp_00");
		m_vecLeftPanelPalStatus[1] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_hp_10");
		m_vecLeftPanelPalStatus[2] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_hp_20");
		m_vecLeftPanelPalStatus[3] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_hp_30");
		m_vecLeftPanelPalStatus[4] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_hp_40");
		m_vecLeftPanelPalStatus[5] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_hunger_00");
		m_vecLeftPanelPalStatus[6] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_hunger_10");
		m_vecLeftPanelPalStatus[7] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_hunger_20");
		m_vecLeftPanelPalStatus[8] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_hunger_30");
		m_vecLeftPanelPalStatus[9] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_hunger_40");
		m_vecLeftPanelPalStatus[10] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_icon_00");
		m_vecLeftPanelPalStatus[11] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_icon_10");
		m_vecLeftPanelPalStatus[12] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_icon_20");
		m_vecLeftPanelPalStatus[13] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_icon_30");
		m_vecLeftPanelPalStatus[14] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_icon_40");
		m_vecLeftPanelPalStatus[15] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_level_00");
		m_vecLeftPanelPalStatus[16] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_level_10");
		m_vecLeftPanelPalStatus[17] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_level_20");
		m_vecLeftPanelPalStatus[18] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_level_30");
		m_vecLeftPanelPalStatus[19] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_level_40");
		m_vecLeftPanelPalStatus[20] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_name_00");
		m_vecLeftPanelPalStatus[21] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_name_10");
		m_vecLeftPanelPalStatus[22] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_name_20");
		m_vecLeftPanelPalStatus[23] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_name_30");
		m_vecLeftPanelPalStatus[24] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_name_40");
		m_vecLeftPanelPalStatus[25] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_status_00");
		m_vecLeftPanelPalStatus[26] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_status_10");
		m_vecLeftPanelPalStatus[27] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_status_20");
		m_vecLeftPanelPalStatus[28] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_status_30");
		m_vecLeftPanelPalStatus[29] = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(L"left_pal_status_40");

		for (int32 i = 0; i < m_vecLeftPanelPalStatus.size(); i++)
		{
			m_vecLeftPanelPalStatus[i].lock()->SetActive(false);
		}
	}

	{
		// right panel info window
		{
			m_vecRightPanelPalStatus.resize(52);
			m_vecRightPanelPalStatus[0] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_Info");
			m_vecRightPanelPalStatus[1] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_info_level");
			m_vecRightPanelPalStatus[2] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_info_name");
			m_vecRightPanelPalStatus[3] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_exp");
			m_vecRightPanelPalStatus[4] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_info_exp_num");
			m_vecRightPanelPalStatus[5] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_element");
			m_vecRightPanelPalStatus[6] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_icon");
			m_vecRightPanelPalStatus[7] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_hp");
			m_vecRightPanelPalStatus[8] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_hp_cur_num");
			m_vecRightPanelPalStatus[9] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_hp_total_num");
			m_vecRightPanelPalStatus[10] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_hunger");
			m_vecRightPanelPalStatus[11] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_hunger_cur_num");
			m_vecRightPanelPalStatus[12] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_hunger_total_num");
			m_vecRightPanelPalStatus[13] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_atk_num");
			m_vecRightPanelPalStatus[14] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_def_num");
			m_vecRightPanelPalStatus[15] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_workspeed_num");
			m_vecRightPanelPalStatus[16] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_0");
			m_vecRightPanelPalStatus[17] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_1");
			m_vecRightPanelPalStatus[18] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_2");
			m_vecRightPanelPalStatus[19] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_3");
			m_vecRightPanelPalStatus[20] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_4");
			m_vecRightPanelPalStatus[21] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_5");
			m_vecRightPanelPalStatus[22] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_6");
			m_vecRightPanelPalStatus[23] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_7");
			m_vecRightPanelPalStatus[24] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_8");
			m_vecRightPanelPalStatus[25] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_9");
			m_vecRightPanelPalStatus[26] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_10");
			m_vecRightPanelPalStatus[27] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_palwork_11");
			m_vecRightPanelPalStatus[28] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_cur_mission");
			m_vecRightPanelPalStatus[29] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_foodamount_0");
			m_vecRightPanelPalStatus[30] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_foodamount_1");
			m_vecRightPanelPalStatus[31] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_foodamount_2");
			m_vecRightPanelPalStatus[32] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_foodamount_3");
			m_vecRightPanelPalStatus[33] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_foodamount_4");
			m_vecRightPanelPalStatus[34] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_foodamount_5");
			m_vecRightPanelPalStatus[35] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_foodamount_6");
			m_vecRightPanelPalStatus[36] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_foodamount_7");
			m_vecRightPanelPalStatus[37] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_foodamount_8");
			m_vecRightPanelPalStatus[38] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_foodamount_9");
			m_vecRightPanelPalStatus[39] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_partner_skill_name");
			m_vecRightPanelPalStatus[40] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_skill_level");
			m_vecRightPanelPalStatus[41] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_active_skill_name_0");
			m_vecRightPanelPalStatus[42] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_active_skill_name_1");
			m_vecRightPanelPalStatus[43] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_active_skill_name_2");
			m_vecRightPanelPalStatus[44] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_skill_element_0");
			m_vecRightPanelPalStatus[45] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_skill_element_1");
			m_vecRightPanelPalStatus[46] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_skill_element_2");
			m_vecRightPanelPalStatus[47] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_active_skill_level_0");
			m_vecRightPanelPalStatus[48] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_active_skill_level_1");
			m_vecRightPanelPalStatus[49] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_pal_active_skill_level_2");
			m_vecRightPanelPalStatus[50] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_passive_skill_name_0");
			m_vecRightPanelPalStatus[51] = m_spPalboxRightChildPanel->FindComponent<CRectComponent>(L"right_passive_skill_name_1");
		}
	}

	m_wpBuildManager = CBuildManager::GetInstance();

	return S_OK;
}

HRESULT Client::CPalBoxSystem::BeginPlay()
{
	if (FAILED(CContainerSystem::BeginPlay())) { return E_FAIL; }
	
	m_arrLeftPanelPals = m_wpPlayer.lock()->GetPartnerPalPtr();

	return S_OK;
}

int32 Client::CPalBoxSystem::PreTick(float _fDeltaSeconds)
{
	switch (CContainerSystem::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CPalBoxSystem::Tick(float _fDeltaSeconds)
{
	switch (CContainerSystem::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	
	// 팰을 동적으로 포획했을 상황을 가정한 기능 테스트용 코드입니다.
	if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_K))
	{
		// 왼쪽 패널 파트너 팰
		/*
		FPalBoxPalInfo tPalBoxPalInfo = {};
		tPalBoxPalInfo.m_eMonsterType = EMonsterType::PINK_CAT;
		tPalBoxPalInfo.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tPalBoxPalInfo.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
		tPalBoxPalInfo.m_eMonsterElementType = EMonsterElementType::ICE;
		tPalBoxPalInfo.m_wstrMonsterName = L"까부냥";
		tPalBoxPalInfo.m_iMonsterLevel = 5;
		tPalBoxPalInfo.m_fCurHp = 80.0f;
		tPalBoxPalInfo.m_fCurHungerGauge = 20.0f;
		tPalBoxPalInfo.m_fCurExp = 8.0f;
		tPalBoxPalInfo.m_eToDoType = EToDoType::ENUM_END; 
		tPalBoxPalInfo.m_fAttackPower = 10.0f;
		tPalBoxPalInfo.m_fDefensePower = 6.0f;
		tPalBoxPalInfo.m_fWorkSpeed = 8.0f;
		tPalBoxPalInfo.m_iFoodAmount = 3;
		tPalBoxPalInfo.m_iPartnerSkillLevel = 2;
		tPalBoxPalInfo.m_iActiveSkillLevelA = 9;
		tPalBoxPalInfo.m_iActiveSkillLevelB = 1;
		tPalBoxPalInfo.m_iActiveSkillLevelC = 2;
		tPalBoxPalInfo.m_wstrPartnerSkillName = L"얼음 총알";
		tPalBoxPalInfo.m_wstrActiveSkillNameA = L"스킬 없음";
		tPalBoxPalInfo.m_wstrActiveSkillNameB = L"스킬 없음";
		tPalBoxPalInfo.m_wstrActiveSkillNameC = L"스킬 없음";
		tPalBoxPalInfo.m_eActiveSkillElementA = EMonsterElementType::ICE;
		tPalBoxPalInfo.m_eActiveSkillElementB = EMonsterElementType::ELECTRIC;;
		tPalBoxPalInfo.m_eActiveSkillElementC = EMonsterElementType::WATER;;
		tPalBoxPalInfo.m_wstrPassiveSkillNameA = L"스킬 없음";
		tPalBoxPalInfo.m_wstrPassiveSkillNameB = L"스킬 없음";

		AddPalToPalBoxLeftPanel(tPalBoxPalInfo);
		*/

		// 가운데 패널
		// Dream demon
		{
			FPalBoxPalInfo tPalBoxPalInfo = {};
			tPalBoxPalInfo.m_eMonsterType = EMonsterType::DREAM_DEMON;
			tPalBoxPalInfo.m_wstrTextureFileName = CClientLoop::s_spClientDataSheet->k_mapPalIconTextureNames.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
			tPalBoxPalInfo.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPalBoxPalInfo.m_eMonsterElementType = CClientLoop::s_spClientDataSheet->k_mapPalElementTypes.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrMonsterName = CClientLoop::s_spClientDataSheet->k_mapPalName.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iMonsterLevel = 2;
			tPalBoxPalInfo.m_fCurHp = 10.0f;
			tPalBoxPalInfo.m_fCurHungerGauge = 20.0f;
			tPalBoxPalInfo.m_fCurExp = 64.0f;
			tPalBoxPalInfo.m_fAttackPower = 20.0f;
			tPalBoxPalInfo.m_fDefensePower = 30.0f;
			tPalBoxPalInfo.m_eToDoType = EToDoType::ENUM_END;
			tPalBoxPalInfo.m_fWorkSpeed = CClientLoop::s_spClientDataSheet->k_mapPalDefaultToDoRatios.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iFoodAmount = CClientLoop::s_spClientDataSheet->k_mapPalFoodAmounts.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iPartnerSkillLevel = 2;
			tPalBoxPalInfo.m_iActiveSkillLevelA = 1;
			tPalBoxPalInfo.m_iActiveSkillLevelB = 3;
			tPalBoxPalInfo.m_iActiveSkillLevelC = 4;
			tPalBoxPalInfo.m_wstrPartnerSkillName = CClientLoop::s_spClientDataSheet->k_mapPalPartnerSkillName.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameB.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameC.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeB.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeC.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameA = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameB = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameB.at(tPalBoxPalInfo.m_eMonsterType);

			AddTestPalToPalBoxMidPanel(tPalBoxPalInfo);
		}
		
		// kitsunebi
		{
			FPalBoxPalInfo tPalBoxPalInfo = {};
			tPalBoxPalInfo.m_eMonsterType = EMonsterType::KITSUNEBI;
			tPalBoxPalInfo.m_wstrTextureFileName = CClientLoop::s_spClientDataSheet->k_mapPalIconTextureNames.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
			tPalBoxPalInfo.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPalBoxPalInfo.m_eMonsterElementType = CClientLoop::s_spClientDataSheet->k_mapPalElementTypes.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrMonsterName = CClientLoop::s_spClientDataSheet->k_mapPalName.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iMonsterLevel = 2;
			tPalBoxPalInfo.m_fCurHp = 50.0f;
			tPalBoxPalInfo.m_fCurHungerGauge = 20.0f;
			tPalBoxPalInfo.m_fCurExp = 64.0f;
			tPalBoxPalInfo.m_fAttackPower = 20.0f;
			tPalBoxPalInfo.m_fDefensePower = 30.0f;
			tPalBoxPalInfo.m_eToDoType = EToDoType::ENUM_END;
			tPalBoxPalInfo.m_fWorkSpeed = CClientLoop::s_spClientDataSheet->k_mapPalDefaultToDoRatios.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iFoodAmount = CClientLoop::s_spClientDataSheet->k_mapPalFoodAmounts.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iPartnerSkillLevel = 2;
			tPalBoxPalInfo.m_iActiveSkillLevelA = 1;
			tPalBoxPalInfo.m_iActiveSkillLevelB = 3;
			tPalBoxPalInfo.m_iActiveSkillLevelC = 4;
			tPalBoxPalInfo.m_wstrPartnerSkillName = CClientLoop::s_spClientDataSheet->k_mapPalPartnerSkillName.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameB.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameC.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeB.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeC.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameA = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameB = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameB.at(tPalBoxPalInfo.m_eMonsterType);

			AddTestPalToPalBoxMidPanel(tPalBoxPalInfo);
		}
		
		// penguin
		{
			FPalBoxPalInfo tPalBoxPalInfo = {};
			tPalBoxPalInfo.m_eMonsterType = EMonsterType::PENGUIN;
			tPalBoxPalInfo.m_wstrTextureFileName = CClientLoop::s_spClientDataSheet->k_mapPalIconTextureNames.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
			tPalBoxPalInfo.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPalBoxPalInfo.m_eMonsterElementType = CClientLoop::s_spClientDataSheet->k_mapPalElementTypes.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrMonsterName = CClientLoop::s_spClientDataSheet->k_mapPalName.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iMonsterLevel = 2;
			tPalBoxPalInfo.m_fCurHp = 30.0f;
			tPalBoxPalInfo.m_fCurHungerGauge = 20.0f;
			tPalBoxPalInfo.m_fCurExp = 64.0f;
			tPalBoxPalInfo.m_fAttackPower = 20.0f;
			tPalBoxPalInfo.m_fDefensePower = 30.0f;
			tPalBoxPalInfo.m_eToDoType = EToDoType::ENUM_END;
			tPalBoxPalInfo.m_fWorkSpeed = CClientLoop::s_spClientDataSheet->k_mapPalDefaultToDoRatios.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iFoodAmount = CClientLoop::s_spClientDataSheet->k_mapPalFoodAmounts.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iPartnerSkillLevel = 2;
			tPalBoxPalInfo.m_iActiveSkillLevelA = 1;
			tPalBoxPalInfo.m_iActiveSkillLevelB = 3;
			tPalBoxPalInfo.m_iActiveSkillLevelC = 4;
			tPalBoxPalInfo.m_wstrPartnerSkillName = CClientLoop::s_spClientDataSheet->k_mapPalPartnerSkillName.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameB.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameC.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeB.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeC.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameA = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameB = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameB.at(tPalBoxPalInfo.m_eMonsterType);

			AddTestPalToPalBoxMidPanel(tPalBoxPalInfo);
		}
	
		// pink cat
		{
			FPalBoxPalInfo tPalBoxPalInfo = {};
			tPalBoxPalInfo.m_eMonsterType = EMonsterType::PINK_CAT;
			tPalBoxPalInfo.m_wstrTextureFileName = CClientLoop::s_spClientDataSheet->k_mapPalIconTextureNames.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
			tPalBoxPalInfo.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPalBoxPalInfo.m_eMonsterElementType = CClientLoop::s_spClientDataSheet->k_mapPalElementTypes.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrMonsterName = CClientLoop::s_spClientDataSheet->k_mapPalName.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iMonsterLevel = 2;
			tPalBoxPalInfo.m_fCurHp = 60.0f;
			tPalBoxPalInfo.m_fCurHungerGauge = 20.0f;
			tPalBoxPalInfo.m_fCurExp = 64.0f;
			tPalBoxPalInfo.m_fAttackPower = 20.0f;
			tPalBoxPalInfo.m_fDefensePower = 30.0f;
			tPalBoxPalInfo.m_eToDoType = EToDoType::ENUM_END;
			tPalBoxPalInfo.m_fWorkSpeed = CClientLoop::s_spClientDataSheet->k_mapPalDefaultToDoRatios.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iFoodAmount = CClientLoop::s_spClientDataSheet->k_mapPalFoodAmounts.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iPartnerSkillLevel = 2;
			tPalBoxPalInfo.m_iActiveSkillLevelA = 1;
			tPalBoxPalInfo.m_iActiveSkillLevelB = 3;
			tPalBoxPalInfo.m_iActiveSkillLevelC = 4;
			tPalBoxPalInfo.m_wstrPartnerSkillName = CClientLoop::s_spClientDataSheet->k_mapPalPartnerSkillName.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameB.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameC.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeB.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeC.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameA = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameB = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameB.at(tPalBoxPalInfo.m_eMonsterType);

			AddTestPalToPalBoxMidPanel(tPalBoxPalInfo);
		}

		// sheep ball
		{
			FPalBoxPalInfo tPalBoxPalInfo = {};
			tPalBoxPalInfo.m_eMonsterType = EMonsterType::SHEEP_BALL;
			tPalBoxPalInfo.m_wstrTextureFileName = CClientLoop::s_spClientDataSheet->k_mapPalIconTextureNames.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
			tPalBoxPalInfo.m_eMonsterPersonalityType = EMonsterPersonalityType::STUPID;
			tPalBoxPalInfo.m_eMonsterElementType = CClientLoop::s_spClientDataSheet->k_mapPalElementTypes.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrMonsterName = CClientLoop::s_spClientDataSheet->k_mapPalName.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iMonsterLevel = 2;
			tPalBoxPalInfo.m_fCurHp = 80.0f;
			tPalBoxPalInfo.m_fCurHungerGauge = 20.0f;
			tPalBoxPalInfo.m_fCurExp = 64.0f;
			tPalBoxPalInfo.m_fAttackPower = 20.0f;
			tPalBoxPalInfo.m_fDefensePower = 30.0f;
			tPalBoxPalInfo.m_eToDoType = EToDoType::ENUM_END;
			tPalBoxPalInfo.m_fWorkSpeed = CClientLoop::s_spClientDataSheet->k_mapPalDefaultToDoRatios.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iFoodAmount = CClientLoop::s_spClientDataSheet->k_mapPalFoodAmounts.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_iPartnerSkillLevel = 2;
			tPalBoxPalInfo.m_iActiveSkillLevelA = 1;
			tPalBoxPalInfo.m_iActiveSkillLevelB = 3;
			tPalBoxPalInfo.m_iActiveSkillLevelC = 4;
			tPalBoxPalInfo.m_wstrPartnerSkillName = CClientLoop::s_spClientDataSheet->k_mapPalPartnerSkillName.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameB.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameC.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeB.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeC.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameA = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameA.at(tPalBoxPalInfo.m_eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameB = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameB.at(tPalBoxPalInfo.m_eMonsterType);

			AddTestPalToPalBoxMidPanel(tPalBoxPalInfo);
		}
	}

	if (m_spUIManager->GetTopPanel() == GetContainerParentPanel())
	{
		UpdateCurMouseSlotPos();
		ActiveSlotHover();
		ShowPartnerPalInfoOnLeftPanel();
		ActivePalInfoWindow();
		MovePalSlotUI();
		SortPalSlots();
	}

	return 0;
}

int32 Client::CPalBoxSystem::PostTick(float _fDeltaSeconds)
{
	switch (CContainerSystem::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CPalBoxSystem::EndPlay()
{
	if (FAILED(CContainerSystem::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CPalBoxSystem::Release()
{
	m_spPalboxLeftChildPanel = nullptr;
	m_spPalboxRightChildPanel = nullptr;
	m_spPalboxRightNodataChildPanel = nullptr;
	m_spPalboxMidChildPanel = nullptr;
	m_spPalboxBottomChildPanel = nullptr;

	m_arrLeftPanelPals = nullptr;

	for (auto& row : m_arrBottomPanelPals)
	{
		for (auto& monster : row) 
		{
			monster = nullptr;
		}
	}

	CContainerSystem::Release();
}

int32 Client::CPalBoxSystem::GetPalboxPalTotalNum(EMonsterType _eMonsterType)
{
	auto iter = m_umapPartnerPalInfos.find(_eMonsterType);
	if (iter != m_umapPartnerPalInfos.end()) { return m_umapPartnerPalInfos[_eMonsterType]; }
	return 0;
}

void Client::CPalBoxSystem::AddPalboxPalCount(EMonsterType _eMonsterType)
{
	// 플레이어 담당자의 요청에 따라, 한종류의 팰을 총 몇마리들고 있는지를 알게하기 위한
	auto iter = m_umapPartnerPalInfos.find(_eMonsterType);
	if (iter != m_umapPartnerPalInfos.end())
	{
		m_umapPartnerPalInfos[_eMonsterType]++;
	}
	else
	{
		m_umapPartnerPalInfos.insert({ _eMonsterType, 1 });
	}
}

void Client::CPalBoxSystem::MovePalSlotUI()
{
	if (m_wpCurMouseSlotComponent.expired() || m_wpCurLeftClickedSlotComponent.expired()) { return; }
	if (m_wpCurLeftClickedSlotComponent.lock()->GetComponentName() == L"mid_sort_none") { return; }

	std::wstring wstrMouseSlotTextureName			 = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotTextureName	 = m_wpCurLeftClickedSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);
	std::wstring wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(m_wstrPreClickedComponentName, 2);
	std::pair<int32, int32> prPreClickedSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);
	std::pair<int32, int32> prCurClickedSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);

	/* 마우스 슬롯에 이미지가 없을 경우 */ 
	if (wstrMouseSlotTextureName == L"T_pal_icon_none") 
	{
		if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"left_none_hover_")
		{
			std::wstring wstrLeftPanelIndexs = std::to_wstring(prCurClickedSlotIndexs.first) + std::to_wstring(0);
			std::wstring wstrLeftPanelIconComponentName = L"left_pal_icon_" + wstrLeftPanelIndexs;
			std::weak_ptr<CImageComponent> wpIconComponent = m_spPalboxLeftChildPanel->FindComponent<CImageComponent>(wstrLeftPanelIconComponentName);

			if (wpIconComponent.lock()->GetTextureName() != L"")
			{
				m_wstrPreClickedComponentName = wstrCurLeftClickedSlotComponentName;
				m_wstrPreClickedTextureName = wpIconComponent.lock()->GetTextureName();
			}
			else
			{
				m_wstrPreClickedComponentName = wstrCurLeftClickedSlotComponentName;
				m_wstrPreClickedTextureName = L"T_pal_icon_none";
			}
		}
		else
		{
			m_wstrPreClickedComponentName = wstrCurLeftClickedSlotComponentName;
			m_wstrPreClickedTextureName   = wstrCurLeftClickedSlotTextureName;
		}

		m_wpCurMouseSlotComponent.lock()->SetTextureFileName(m_wstrPreClickedTextureName);
		m_spUIManager->SetFocusedControlComponent(nullptr);
	}
	/* 마우스 슬롯에 이미지가 있을 경우 */ 
	else 
	{
		std::wstring wstrLeftPanelIndexs = std::to_wstring(prCurClickedSlotIndexs.first) + std::to_wstring(0);
		std::wstring wstrLeftPanelIconComponentName = L"left_pal_icon_" + wstrLeftPanelIndexs;
		std::weak_ptr<CImageComponent> wpIconComponent = m_spPalboxLeftChildPanel->FindComponent<CImageComponent>(wstrLeftPanelIconComponentName);

		/* 클릭한 슬롯에 사진이 없을 경우 */
		if ( wstrCurLeftClickedSlotTextureName == L"T_pal_icon_none" ||
			(wpIconComponent.lock()->IsActive() == false && wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"left_none_hover_"))
		{
			/* LEFT PANEL */
			if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"left_none_hover_")
			{
				MovePalToEmptySlotFromLeftPanel(); // ok
			}
			/* BOTTOM PANEL */
			else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"bottom_pal_slot_")
			{
				MovePalToEmptySlotFromBottomPanel();
			}
			/* MID PANEL */
			else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"mid_pal_slot_")
			{
				MovePalToEmptySlotFromMidPanel(); // ok
			}
		}
		/* 사진이 있을 경우 */
		else 
		{
			/* LEFT PANEL */
			if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"left_none_hover_")
			{
				MovePalToOccupiedSlotFromLeftPanel();
			}
			/* BOTTOM PANEL */
			else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"bottom_pal_slot_")
			{
				MovePalToOccupiedSlotFromBottomPanel();
			}
			/* MID PANEL */
			else if (wstrPreLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"mid_pal_slot_")
			{
				MovePalToOccupiedSlotFromMidPanel();
			}
		}	

		m_wpCurMouseSlotComponent.lock()->SetTextureFileName(L"T_pal_icon_none");
		m_spUIManager->SetFocusedControlComponent(nullptr);
	}
}

void Client::CPalBoxSystem::ExchangeSlotsUI(std::shared_ptr<CPanel> _spPreClickedPanel, std::shared_ptr<CPanel> _spCurClickedPanel, const std::wstring& _wstrPreSlotExchangeTextureName, const std::wstring& _wstrCurSlotExchangeTextureName)
{
	if (m_wpCurLeftClickedSlotComponent.expired()) { return; }

	// Pal UI를 바꿔줍니다.
	wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();

	std::pair<int32, int32> prPreClickedSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);
	_spPreClickedPanel->FindComponent<CImageComponent>(m_wstrPreClickedComponentName)->SetTextureFileName(_wstrPreSlotExchangeTextureName);

	std::pair<int32, int32> prCurClickedSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);
	_spCurClickedPanel->FindComponent<CImageComponent>(wstrCurLeftClickedSlotComponentName)->SetTextureFileName(_wstrCurSlotExchangeTextureName);
}

void Client::CPalBoxSystem::MovePalToEmptySlotFromLeftPanel()
{
	std::wstring wstrMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);

	std::pair<int32, int32> prPreClickedSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);
	std::pair<int32, int32> prCurClickedSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);

	int32 iLeftPanelPreIndex = prPreClickedSlotIndexs.first;
	FPalBoxPalInfo tLeftPanelPrePalInfo = (*m_arrLeftPanelPals)[iLeftPanelPreIndex].first;
	shared_ptr<CMonster> spLeftPanelPrePal = (*m_arrLeftPanelPals)[iLeftPanelPreIndex].second;

	// left -> mid
	if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"mid_pal_slot_")
	{
		int32 iMidPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iMidPanelCurIndexY = prCurClickedSlotIndexs.second;

		// 이전 인덱스에 있는 값들을 초기화시킵니다.
		(*m_arrLeftPanelPals)[iLeftPanelPreIndex].first = {};
		(*m_arrLeftPanelPals)[iLeftPanelPreIndex].second = nullptr;

		// 현재 클릭한 슬롯에 이전 인덱스에서 복사해둔 구조체를 넣어줍니다.
		tLeftPanelPrePalInfo.m_iIndexX = iMidPanelCurIndexX;
		tLeftPanelPrePalInfo.m_iIndexY = iMidPanelCurIndexY;
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY] = tLeftPanelPrePalInfo; 

		// 파트널 팰을 삭제합니다.
		if (m_wpPlayer.lock()->GetPartnerPal() == spLeftPanelPrePal) { m_wpPlayer.lock()->SetPartnerPal(nullptr); }
		m_spGameManager->GetCurLevel()->RemoveGameObject(spLeftPanelPrePal);
		spLeftPanelPrePal = nullptr;
	
		HiddenPartnerPalInfoOnLeftPanel(iLeftPanelPreIndex); 
		ExchangeSlotsUI(m_spPalboxLeftChildPanel, m_spPalboxMidChildPanel, L"T_palbox_left_none_hover", wstrMouseSlotTextureName);
	}
	// left -> bottom
	else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"bottom_pal_slot_")
	{
		int32 iBottomPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iBottomPanelCurIndexY = prCurClickedSlotIndexs.second;

		// 이전 인덱스에 있던 값들을 복사한 후 초기화 시킵니다.
		(*m_arrLeftPanelPals)[iLeftPanelPreIndex].first = {};
		(*m_arrLeftPanelPals)[iLeftPanelPreIndex].second = nullptr;
		
		// 이전 인덱스에 있던 객체를 현재 클릭한 슬롯에 집어넣어줍니다.
		m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY] = spLeftPanelPrePal;
		m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY]->SetActive(true);

		// BT를 거점 팰로 초기화합니다.
		shared_ptr<CMonster> spPartnerPal = m_wpPlayer.lock()->GetPartnerPal();
		if (spPartnerPal != spLeftPanelPrePal)
		{
			Vector3 vPalBoxPos = m_wpBuildManager.lock()->GetPalBoxPos();
			spLeftPanelPrePal->GetBodyColliderComponent()->RequestTransform(Vector3(vPalBoxPos.x, vPalBoxPos.y + 10.0f, vPalBoxPos.z + 5.0f));
		}
		else
		{
			m_wpPlayer.lock()->SetPartnerPal(nullptr);
		}
		spLeftPanelPrePal->ReInitializePalBT(EMonsterAIType::SLAVE_PAL);

		HiddenPartnerPalInfoOnLeftPanel(iLeftPanelPreIndex);
		ExchangeSlotsUI(m_spPalboxLeftChildPanel, m_spPalboxBottomChildPanel, L"T_palbox_left_none_hover", wstrMouseSlotTextureName);
	}
	// left -> left
	else
	{
		int32 iLeftPanelCurIndex = prCurClickedSlotIndexs.first;

		// 이전 인덱스에 있던 값들을 복사한 후 초기화 시킵니다.
		(*m_arrLeftPanelPals)[iLeftPanelPreIndex].first = {}; 
		(*m_arrLeftPanelPals)[iLeftPanelPreIndex].second = nullptr;

		// 이전 인덱스에서 복사해둔 구조체와 객체를 현재 클릭한 인덱스 슬롯에 넣어줍니다.
		tLeftPanelPrePalInfo.m_iIndexX = iLeftPanelCurIndex;
		tLeftPanelPrePalInfo.m_iIndexY = 0;
		(*m_arrLeftPanelPals)[iLeftPanelCurIndex].first = tLeftPanelPrePalInfo; 
		(*m_arrLeftPanelPals)[iLeftPanelCurIndex].second= spLeftPanelPrePal; 

		HiddenPartnerPalInfoOnLeftPanel(iLeftPanelPreIndex);
	}

	m_wpPlayer.lock()->UpdatePalIndexUI();
}

void Client::CPalBoxSystem::MovePalToEmptySlotFromMidPanel()
{
	std::wstring wstrMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);

	std::pair<int32, int32> prPreClickedSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);
	std::pair<int32, int32> prCurClickedSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);

	int32 iMidPanelPreIndexX = prPreClickedSlotIndexs.first;
	int32 iMidPanelPreIndexY = prPreClickedSlotIndexs.second;
	FPalBoxPalInfo tMidPanelPrePalInfo = m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY];

	// mid -> left
	if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"left_none_hover_")
	{
		int32 iLeftPaneCurlIndex = prCurClickedSlotIndexs.first;

		// 이전 클릭한 슬롯에 있던 값들을 복사한 후 초기화 시킵니다.
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY] = {};  

		// 이전에 복사해둔 값들을 이용하여 새로운 구조체와 객체를 만들어 현재 클릭한 슬롯에 넣어줍니다.
		tMidPanelPrePalInfo.m_iIndexX = iLeftPaneCurlIndex;
		tMidPanelPrePalInfo.m_iIndexY = 0;
		tMidPanelPrePalInfo.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		InsertPalInfoAndPalIntoPartnerPalArr(tMidPanelPrePalInfo);				 

		ExchangeSlotsUI(m_spPalboxMidChildPanel, m_spPalboxLeftChildPanel, L"T_pal_icon_none", L"T_palbox_left_none_hover");
	}
	// mid -> bottom
	else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"bottom_pal_slot_")
	{
		int32 iBottomPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iBottomPanelCurIndexY = prCurClickedSlotIndexs.second;

		// 이전 클릭한 슬롯에 있던 값들을 복사한 후 초기화 시킵니다.
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY] = {};

		// 이전에 복사해둔 값들을 이용하여 새로운 객체를 만들어 현재 클릭한 슬롯에 넣어줍니다.
		tMidPanelPrePalInfo.m_iIndexX = iBottomPanelCurIndexX;
		tMidPanelPrePalInfo.m_iIndexY = iBottomPanelCurIndexY;
		AddPalToBaseCamp(tMidPanelPrePalInfo);

		ExchangeSlotsUI(m_spPalboxMidChildPanel, m_spPalboxBottomChildPanel, L"T_pal_icon_none", wstrMouseSlotTextureName);
	}
	// mid -> mid
	else
	{
		int32 iMidPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iMidPanelCurIndexY = prCurClickedSlotIndexs.second;

		// 이전 클릭한 슬롯에 있던 값들을 복사한 후 초기화 시킵니다.
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY] = {};

		// 이전에 복사해둔 구조체를 현재 클릭한 슬롯에 넣어줍니다.
		tMidPanelPrePalInfo.m_iIndexX = iMidPanelCurIndexX;
		tMidPanelPrePalInfo.m_iIndexY = iMidPanelCurIndexY;
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY] = tMidPanelPrePalInfo;

		ExchangeSlotsUI(m_spPalboxMidChildPanel, m_spPalboxMidChildPanel, L"T_pal_icon_none", wstrMouseSlotTextureName);
	}

	m_wpPlayer.lock()->UpdatePalIndexUI();
}

void Client::CPalBoxSystem::MovePalToEmptySlotFromBottomPanel()
{
	std::wstring wstrMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);

	std::pair<int32, int32> prPreClickedSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);
	std::pair<int32, int32> prCurClickedSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);

	int32 iBottomPanelPreIndexX = prPreClickedSlotIndexs.first;
	int32 iBottomPanelPreIndexY = prPreClickedSlotIndexs.second;
	std::shared_ptr<CMonster> spPreClickedMonster = m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY];

	// bottom -> left
	if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"left_none_hover_")
	{
		int32 iLeftPaneCurlIndexX = prCurClickedSlotIndexs.first;

		// 이전 클릭한 슬롯에 있던 객체 포인터를 복사한 후 null로 초기화 시킵니다.
		m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY] = nullptr;

		// 이전 클릭한 슬롯에 있던 객체를 이용하여, 구조체를 만든 후, 기존 객체와 함께 현재 클릭한 슬롯에 넣어줍니다.
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndexX].first = GeneratePalInfoStruct(spPreClickedMonster, prCurClickedSlotIndexs);
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndexX].second = spPreClickedMonster;
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndexX].second->SetActive(false);

		// BT를 파트너 팰로 초기화합니다.
		spPreClickedMonster->PauseToDoCommand();
		spPreClickedMonster->ReInitializePalBT(EMonsterAIType::PARTNER_PAL);

		ExchangeSlotsUI(m_spPalboxBottomChildPanel, m_spPalboxLeftChildPanel, L"T_pal_icon_none", L"T_palbox_left_none_hover");
	}
	// bottom -> mid
	else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"mid_pal_slot_")
	{
		int32 iMidPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iMidPanelCurIndexY = prCurClickedSlotIndexs.second;

		// 이전 클릭한 슬롯에 있던 객체 포인터를 복사한 후 null로 초기화 시킵니다.
		m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY] = nullptr;

		// 이전 클릭한 슬롯에 있던 객체를 이용하여, 구조체를 만든 후, 현재 클릭한 슬롯에 넣어줍니다.
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY] = GeneratePalInfoStruct(spPreClickedMonster, prCurClickedSlotIndexs);

		// 레벨에서 삭제해줍니다.
		spPreClickedMonster->RemoveSlavePalFromToDoSystem();

		m_spGameManager->GetCurLevel()->RemoveGameObject(spPreClickedMonster);
		spPreClickedMonster = nullptr;

		ExchangeSlotsUI(m_spPalboxBottomChildPanel, m_spPalboxMidChildPanel, L"T_pal_icon_none", wstrMouseSlotTextureName);
	}
	// bottom -> bottom
	else
	{
		int32 iBottomPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iBottomPanelCurIndexY = prCurClickedSlotIndexs.second;
		std::shared_ptr<CMonster> spCurClickedMonster = m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY];

		// 이전 클릭한 슬롯에 있던 객체를 복사한 후 null로 초기화 시킨뒤, 현재 클릭한 슬롯에서 복사해둔 객체를 넣어줍니다.
		m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY] = nullptr;
		m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY] = spCurClickedMonster;

		// 현재 클릭한 슬롯에 있던 객체를 복사한 후 null로 초기화 시킨뒤, 이전 클릭한 슬롯에서 복사해둔 객체를 넣어줍니다.
		m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY] = nullptr;
		m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY] = spPreClickedMonster;

		ExchangeSlotsUI(m_spPalboxBottomChildPanel, m_spPalboxBottomChildPanel, L"T_pal_icon_none", wstrMouseSlotTextureName);
	}

	m_wpPlayer.lock()->UpdatePalIndexUI();
}

void Client::CPalBoxSystem::MovePalToOccupiedSlotFromLeftPanel()
{
	std::wstring wstrMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotTextureName = m_wpCurLeftClickedSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);

	std::pair<int32, int32> prPreClickedSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);
	std::pair<int32, int32> prCurClickedSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);

	int32 iLeftPanelPrelIndex = prPreClickedSlotIndexs.first;
	FPalBoxPalInfo tLeftPanelPrePalInfo = (*m_arrLeftPanelPals)[iLeftPanelPrelIndex].first;
	shared_ptr<CMonster> spLeftPanelPrePal = (*m_arrLeftPanelPals)[iLeftPanelPrelIndex].second;

	// left -> mid
	if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"mid_pal_slot_")
	{
		int32 iMidPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iMidPanelCurIndexY = prCurClickedSlotIndexs.second;
		FPalBoxPalInfo tMidPanelCurPalInfo = m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY];

		// 이전, 현재 클릭한 슬롯에 있던 2개의 구조체와 1개의 객체를 복사한 뒤, 초기화 시킵니다.
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].first = {};
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].second = nullptr;
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY] = {};

		// 현재 클릭한 슬롯에 있던 구조체를 이용하여, 객체를 만들고, 함께 이전 클릭한 슬롯에 넣어줍니다.
		tMidPanelCurPalInfo.m_iIndexX = iLeftPanelPrelIndex;
		tMidPanelCurPalInfo.m_iIndexY = 0;
		InsertPalInfoAndPalIntoPartnerPalArr(tMidPanelCurPalInfo);					 

		// 이전 클릭한 슬롯에 있던 구조체를 현재 클릭한 슬롯에 넣어줍니다.
		tLeftPanelPrePalInfo.m_iIndexX = iMidPanelCurIndexX;
		tLeftPanelPrePalInfo.m_iIndexY = iMidPanelCurIndexY;
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY] = tLeftPanelPrePalInfo;

		// 파트널 팰을 삭제합니다.
		if (m_wpPlayer.lock()->GetPartnerPal() == spLeftPanelPrePal)
		{
			m_wpPlayer.lock()->SetPartnerPal(nullptr);
		}
		m_spGameManager->GetCurLevel()->RemoveGameObject(spLeftPanelPrePal);
		spLeftPanelPrePal = nullptr;

		ExchangeSlotsUI(m_spPalboxLeftChildPanel, m_spPalboxMidChildPanel, L"T_palbox_left_none_hover", wstrMouseSlotTextureName);
	}
	// left -> bottom
	else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"bottom_pal_slot_")
	{
		int32 iBottomPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iBottomPanelCurIndexY = prCurClickedSlotIndexs.second;
		shared_ptr<CMonster> spCurClickedMonster = m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY];

		// 이전, 현재 클릭한 슬롯에 있던 2개의 구조체와 1개의 객체를 복사한 뒤, 초기화 시킵니다.
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].first = {};
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].second = nullptr;
		m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY] = nullptr;

		// 현재 클릭한 슬롯에 있던 객체를 이용하여, 구조체를 만들고, 그 객체와 함께 넣어줍니다.
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].first = GeneratePalInfoStruct(spCurClickedMonster, prCurClickedSlotIndexs);
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].first.m_iIndexX = iLeftPanelPrelIndex;
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].first.m_iIndexY = 0;
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].second = spCurClickedMonster;
		spCurClickedMonster->SetPosition(Vector3(0.0f, -100.0f, 0.0f));
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].second->SetActive(false);
		InsertPalInfoAndPalIntoPartnerPalArr((*m_arrLeftPanelPals)[iLeftPanelPrelIndex].first);

		// 거점에서 파트너 팰로
		spCurClickedMonster->PauseToDoCommand();
		spCurClickedMonster->ReInitializePalBT(EMonsterAIType::PARTNER_PAL);


		// 이전에 클릭한 슬롯에 있던 객체를 현재 클릭한 슬롯에 넣어줍니다.
		m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY] = spLeftPanelPrePal;
		m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY]->SetActive(true);

		// 파트너 팰에서 거점으로
		// BT를 거점 팰로 초기화합니다.
		shared_ptr<CMonster> spPartnerPal = m_wpPlayer.lock()->GetPartnerPal();
		if (spPartnerPal != spLeftPanelPrePal)
		{
			Vector3 vPalBoxPos = m_wpBuildManager.lock()->GetPalBoxPos();
			spLeftPanelPrePal->GetBodyColliderComponent()->RequestTransform(Vector3(vPalBoxPos.x, vPalBoxPos.y + 10.0f, vPalBoxPos.z + 5.0f));
		}
		else
		{
			m_wpPlayer.lock()->SetPartnerPal(nullptr);
		}
		spLeftPanelPrePal->ReInitializePalBT(EMonsterAIType::SLAVE_PAL);

		ExchangeSlotsUI(m_spPalboxLeftChildPanel, m_spPalboxBottomChildPanel, L"T_palbox_left_none_hover", wstrMouseSlotTextureName);
	}
	// left -> left
	else
	{
		int32 iLeftPanelCurlIndex = prCurClickedSlotIndexs.first;
		FPalBoxPalInfo tLeftPanelCurPalInfo = (*m_arrLeftPanelPals)[iLeftPanelCurlIndex].first;
		shared_ptr<CMonster> spLeftPanelCurPal = (*m_arrLeftPanelPals)[iLeftPanelCurlIndex].second;

		// 이전, 현재 클릭한 슬롯에 있는 구조체와 객체를 초기화 시킵니다.
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].first = {};
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].second = nullptr;
		(*m_arrLeftPanelPals)[iLeftPanelCurlIndex].first = {};
		(*m_arrLeftPanelPals)[iLeftPanelCurlIndex].second = nullptr;

		// 이전 클릭한 슬롯에 현재 클릭한 슬롯에 있던 구조체와 객체 사본을 넣어줍니다.
		tLeftPanelCurPalInfo.m_iIndexX = iLeftPanelPrelIndex;
		tLeftPanelCurPalInfo.m_iIndexY = 0;
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].first = tLeftPanelCurPalInfo; // Pre <- Cur
		(*m_arrLeftPanelPals)[iLeftPanelPrelIndex].second = spLeftPanelCurPal; // Pre <- Cur
		
		// 현재 클릭한 슬롯에 이전 클릭한 슬롯에 있던 구조체와 객체 사본을 넣어줍니다.
		tLeftPanelPrePalInfo.m_iIndexX = iLeftPanelCurlIndex;
		tLeftPanelPrePalInfo.m_iIndexY = 0;
		(*m_arrLeftPanelPals)[iLeftPanelCurlIndex].first = tLeftPanelPrePalInfo; // Cur <- Pre
		(*m_arrLeftPanelPals)[iLeftPanelCurlIndex].second = spLeftPanelPrePal;	 // Cur <- Pre
	}

	m_wpPlayer.lock()->UpdatePalIndexUI();
}

void Client::CPalBoxSystem::MovePalToOccupiedSlotFromMidPanel()
{
	std::wstring wstrMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotTextureName = m_wpCurLeftClickedSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);

	std::pair<int32, int32> prPreClickedSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);
	std::pair<int32, int32> prCurClickedSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);

	int32 iMidPanelPreIndexX = prPreClickedSlotIndexs.first;
	int32 iMidPanelPreIndexY = prPreClickedSlotIndexs.second;
	FPalBoxPalInfo tMidPanelPrePalInfo = m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY];

	// mid -> left
	if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"left_none_hover_")
	{
		int32 iLeftPaneCurlIndex = prCurClickedSlotIndexs.first;
		FPalBoxPalInfo tLeftPanelCurPalInfo = (*m_arrLeftPanelPals)[iLeftPaneCurlIndex].first;
		shared_ptr<CMonster> spLeftPanelCurPal = (*m_arrLeftPanelPals)[iLeftPaneCurlIndex].second;

		// 이전, 현재 클릭한 슬롯에 있는 구조체와 객체를 초기화 시킵니다.
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY] = {};
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndex].first = {};
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndex].second = nullptr;

		// 이전 클릭한 슬롯에 현재 클릭한 슬롯에 있던 구조체의 사본을 넣어줍니다.
		tLeftPanelCurPalInfo.m_iIndexX = iMidPanelPreIndexX;
		tLeftPanelCurPalInfo.m_iIndexY = iMidPanelPreIndexY;
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY] = tLeftPanelCurPalInfo;

		// 현재 클릭한 슬롯에 이전 클릭한 슬롯에 있던 구조체를 이용하여, 객체를 만들고, 구조체와 같이 넣어줍니다.
		tMidPanelPrePalInfo.m_iIndexX = iLeftPaneCurlIndex;
		tMidPanelPrePalInfo.m_iIndexY = 0;
		tMidPanelPrePalInfo.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		InsertPalInfoAndPalIntoPartnerPalArr(tMidPanelPrePalInfo);	
		
		ExchangeSlotsUI(m_spPalboxMidChildPanel, m_spPalboxLeftChildPanel, tLeftPanelCurPalInfo.m_wstrTextureFileName, L"T_palbox_left_none_hover");
	}
	// mid -> bottom
	else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"bottom_pal_slot_")
	{
		int32 iBottomPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iBottomPanelCurIndexY = prCurClickedSlotIndexs.second;
		shared_ptr<CMonster> spCurClickedMonster = m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY];

		// 이전, 현재 클릭한 슬롯에 있는 구조체와 객체를 초기화 시킵니다.
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY] = {};
		m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY] = {};
		
		// 현재 클릭한 슬롯에 이전 클릭한 슬롯에 있던 구조체를 이용하여, 객체를 만들어 넣어줍니다. 
		tMidPanelPrePalInfo.m_iIndexX = iBottomPanelCurIndexX;
		tMidPanelPrePalInfo.m_iIndexY = iBottomPanelCurIndexY;
		AddPalToBaseCamp(tMidPanelPrePalInfo);

		// 이전 클릭한 슬롯에 현재 클릭한 슬롯에 있던 객체를 구조체로 만들어 넣어줍니다.
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY] = GeneratePalInfoStruct(spCurClickedMonster, prCurClickedSlotIndexs);
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY].m_iIndexX = iMidPanelPreIndexX;
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY].m_iIndexY = iMidPanelPreIndexY;

		ExchangeSlotsUI(m_spPalboxMidChildPanel, m_spPalboxBottomChildPanel, wstrCurLeftClickedSlotTextureName, wstrMouseSlotTextureName);
	}
	// mid -> mid
	else
	{
		int32 iMidPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iMidPanelCurIndexY = prCurClickedSlotIndexs.second;
		FPalBoxPalInfo tMidPanelCurPalInfo = m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY];

		// 이전, 현재 클릭한 슬롯에 있는 구조체와 객체를 초기화 시킵니다.
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY] = {};
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY] = {};

		// 이전 클릭한 슬롯에 현재 클릭한 슬롯에 있던 구조체를 넣어줍니다.
		tMidPanelCurPalInfo.m_iIndexX = iMidPanelPreIndexX;
		tMidPanelCurPalInfo.m_iIndexY = iMidPanelPreIndexY;
		m_arrMidPanelPalInfos[iMidPanelPreIndexX][iMidPanelPreIndexY] = tMidPanelCurPalInfo;

		// 현재 클릭한 슬롯에 이전 클릭한 슬롯에 있던 구조체를 넣어줍니다. 
		tMidPanelPrePalInfo.m_iIndexX = iMidPanelCurIndexX;
		tMidPanelPrePalInfo.m_iIndexY = iMidPanelCurIndexY;
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY] = tMidPanelPrePalInfo;

		ExchangeSlotsUI(m_spPalboxMidChildPanel, m_spPalboxMidChildPanel, wstrCurLeftClickedSlotTextureName, wstrMouseSlotTextureName);
	}

	m_wpPlayer.lock()->UpdatePalIndexUI();
}

void Client::CPalBoxSystem::MovePalToOccupiedSlotFromBottomPanel()
{
	std::wstring wstrMouseSlotTextureName = m_wpCurMouseSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotTextureName = m_wpCurLeftClickedSlotComponent.lock()->GetTextureName();
	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	std::wstring wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurLeftClickedSlotComponentName, 2);

	std::pair<int32, int32> prPreClickedSlotIndexs = ExtractLastChar(m_wstrPreClickedComponentName, 2);
	std::pair<int32, int32> prCurClickedSlotIndexs = ExtractLastChar(wstrCurLeftClickedSlotComponentName, 2);

	int32 iBottomPanelPreIndexX = prPreClickedSlotIndexs.first;
	int32 iBottomPanelPreIndexY = prPreClickedSlotIndexs.second;
	shared_ptr<CMonster> spPreMonster = m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY];

	// bottom -> left
	if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"left_none_hover_")
	{
		int32 iLeftPaneCurlIndex = prCurClickedSlotIndexs.first;
		FPalBoxPalInfo tLeftPanelCurPalInfo = (*m_arrLeftPanelPals)[iLeftPaneCurlIndex].first;
		shared_ptr<CMonster> spLeftPanelCurPal = (*m_arrLeftPanelPals)[iLeftPaneCurlIndex].second;

		// 이전, 현재 클릭한 슬롯에 있는 구조체와 객체를 초기화 시킵니다.
		m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY] = nullptr;
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndex].first = {};
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndex].second = nullptr;

		// 현재 클릭한 슬롯에 이전 클릭한 객체를 이용하여, 구조체를 만들고, 객체와 함께 넣어줍니다.
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndex].first = GeneratePalInfoStruct(spPreMonster, prPreClickedSlotIndexs);
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndex].first.m_iIndexX = iLeftPaneCurlIndex;
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndex].first.m_iIndexY = 0;
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndex].second = spPreMonster;
		spPreMonster->SetPosition(Vector3(0.0f, -100.0f, 0.0f));
		(*m_arrLeftPanelPals)[iLeftPaneCurlIndex].second->SetActive(false);

		// 거점에서 파트너 팰로
		spPreMonster->PauseToDoCommand();
		spPreMonster->ReInitializePalBT(EMonsterAIType::PARTNER_PAL);


		// 이전 클릭한 슬롯에 현재 클릭한 객체를 넣어줍니다.
		m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY] = spLeftPanelCurPal;
		m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY]->SetActive(true);

		// 파트너 팰에서 거점으로
		// BT를 거점 팰로 초기화합니다.
		shared_ptr<CMonster> spPartnerPal = m_wpPlayer.lock()->GetPartnerPal();
		if (spPartnerPal != spLeftPanelCurPal)
		{
			Vector3 vPalBoxPos = m_wpBuildManager.lock()->GetPalBoxPos();
			spLeftPanelCurPal->GetBodyColliderComponent()->RequestTransform(Vector3(vPalBoxPos.x, vPalBoxPos.y + 10.0f, vPalBoxPos.z + 5.0f));
		}
		else
		{
			m_wpPlayer.lock()->SetPartnerPal(nullptr);
		}
		spLeftPanelCurPal->ReInitializePalBT(EMonsterAIType::SLAVE_PAL);
		

		ExchangeSlotsUI(m_spPalboxBottomChildPanel, m_spPalboxLeftChildPanel, tLeftPanelCurPalInfo.m_wstrTextureFileName, L"T_palbox_left_none_hover");
	}
	// bottom -> mid
	else if (wstrCurLeftClickedSlotComponentNammeWithoutLastTwoNumber == L"mid_pal_slot_")
	{
		int32 iMidPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iMidPanelCurIndexY = prCurClickedSlotIndexs.second;
		FPalBoxPalInfo tMidPanelCurPalInfo = m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY];

		// 이전, 현재 클릭한 슬롯에 있는 구조체와 객체를 초기화 시킵니다.
		m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY] = nullptr;
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY] = {};

		// 이전 클릭한 슬롯에 현재 클릭한 구조체를 이용하여, 객체를 만들어 넣어줍니다.
		tMidPanelCurPalInfo.m_iIndexX = iBottomPanelPreIndexX;
		tMidPanelCurPalInfo.m_iIndexY = iBottomPanelPreIndexY;
		AddPalToBaseCamp(tMidPanelCurPalInfo);

		// 현재 클릭한 슬롯에 이전 클릭한 객체를 구조체로 만들어 넣어줍니다.
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY] = GeneratePalInfoStruct(spPreMonster, prPreClickedSlotIndexs);
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY].m_iIndexX = iMidPanelCurIndexX;
		m_arrMidPanelPalInfos[iMidPanelCurIndexX][iMidPanelCurIndexY].m_iIndexY = iMidPanelCurIndexY;

		// 레벨에서 삭제해줍니다.
		spPreMonster->RemoveSlavePalFromToDoSystem();

		m_spGameManager->GetCurLevel()->RemoveGameObject(spPreMonster);
		spPreMonster = nullptr;

		ExchangeSlotsUI(m_spPalboxBottomChildPanel, m_spPalboxMidChildPanel, wstrCurLeftClickedSlotTextureName, wstrMouseSlotTextureName);
	}
	// bottom -> bottom
	else
	{
		int32 iBottomPanelCurIndexX = prCurClickedSlotIndexs.first;
		int32 iBottomPanelCurIndexY = prCurClickedSlotIndexs.second;
		shared_ptr<CMonster> spCurMonster = m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY];
	
		// 이전, 현재 클릭한 슬롯에 있는 구조체와 객체를 초기화 시킵니다.
		m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY] = nullptr;
		m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY] = nullptr;

		m_arrBottomPanelPals[iBottomPanelPreIndexX][iBottomPanelPreIndexY] = spCurMonster;
		m_arrBottomPanelPals[iBottomPanelCurIndexX][iBottomPanelCurIndexY] = spPreMonster;

		ExchangeSlotsUI(m_spPalboxBottomChildPanel, m_spPalboxBottomChildPanel, wstrCurLeftClickedSlotTextureName, wstrMouseSlotTextureName);
	}

	m_wpPlayer.lock()->UpdatePalIndexUI();
}

void Client::CPalBoxSystem::AddPalToPalBoxLeftPanel(FPalBoxPalInfo _tPartnerPalInfo)
{
	for (int32 i = 0; i < (int32)(*m_arrLeftPanelPals).size(); i++)
	{
		if ((*m_arrLeftPanelPals)[i].first.m_eMonsterType == EMonsterType::ENUM_END)
		{
			_tPartnerPalInfo.m_iIndexX = i;
			_tPartnerPalInfo.m_iIndexY = 0;
			_tPartnerPalInfo.m_wstrTextureFileName = m_spUIManager->FindPalIconTextureName(_tPartnerPalInfo.m_eMonsterType);
			(*m_arrLeftPanelPals)[i].first = _tPartnerPalInfo;

			InsertPalInfoAndPalIntoPartnerPalArr(_tPartnerPalInfo);

			// 플레이어 담당자의 요청에 따라, 한종류의 팰을 총 몇마리들고 있는지를 알게하기 위한
			auto iter = m_umapPartnerPalInfos.find(_tPartnerPalInfo.m_eMonsterType);
			if (iter != m_umapPartnerPalInfos.end())
			{
				m_umapPartnerPalInfos[_tPartnerPalInfo.m_eMonsterType]++;
			}
			else
			{
				m_umapPartnerPalInfos.insert({ _tPartnerPalInfo.m_eMonsterType, 1});
			}
			break;
		}
	}
}

void Client::CPalBoxSystem::AddPalToPalBoxMidPanel(const std::shared_ptr<CMonster>& _spMonster)
{
	int32 iFlag = 0;

	for (int32 iOuterIndex = 0; iOuterIndex < m_arrMidPanelPalInfos.size(); iOuterIndex++)
	{
		for (int32 iInnderIndex = 0; iInnderIndex < m_arrMidPanelPalInfos[iOuterIndex].size(); iInnderIndex++)
		{
			if (m_arrMidPanelPalInfos[iOuterIndex][iInnderIndex].m_eMonsterType == EMonsterType::ENUM_END)
			{
				iFlag = 1;

				std::pair<int32, int32> prMidPanelIndexs = { iOuterIndex, iInnderIndex };
				FPalBoxPalInfo tPalBoxPalInfo = GeneratePalInfoStruct(_spMonster, prMidPanelIndexs);
				m_arrMidPanelPalInfos[iOuterIndex][iInnderIndex] = tPalBoxPalInfo;
				
				wstring wstrTextureName = CClientLoop::s_spClientDataSheet->k_mapPalIconTextureNames.at(_spMonster->GetMonsterType());
				m_spPalboxMidChildPanel->FindComponent<CImageComponent>(L"mid_pal_slot_"+to_wstring(iOuterIndex) + to_wstring(iInnderIndex))->SetTextureFileName(wstrTextureName);


				// 플레이어 담당자의 요청에 따라, 한종류의 팰을 총 몇마리들고 있는지를 알게하기 위한
				auto iter = m_umapPartnerPalInfos.find(tPalBoxPalInfo.m_eMonsterType);
				if (iter != m_umapPartnerPalInfos.end())
				{
					m_umapPartnerPalInfos[tPalBoxPalInfo.m_eMonsterType]++;
				}
				else
				{
					m_umapPartnerPalInfos.insert({ tPalBoxPalInfo.m_eMonsterType, 1 });
				}
				break;
			}
		}
		if (iFlag == 1) { break; }
	}
}

void Client::CPalBoxSystem::AddTestPalToPalBoxMidPanel(FPalBoxPalInfo _tPartnerPalInfo)
{
	int32 iFlag = 0;

	for (int32 iOuterIndex = 0; iOuterIndex < m_arrMidPanelPalInfos.size(); iOuterIndex++)
	{
		for (int32 iInnderIndex = 0; iInnderIndex < m_arrMidPanelPalInfos[iOuterIndex].size(); iInnderIndex++)
		{
			if (m_arrMidPanelPalInfos[iOuterIndex][iInnderIndex].m_eMonsterType == EMonsterType::ENUM_END)
			{
				iFlag = 1;

				std::pair<int32, int32> prMidPanelIndexs = { iOuterIndex, iInnderIndex };
				_tPartnerPalInfo.m_iIndexX = iOuterIndex;
				_tPartnerPalInfo.m_iIndexY = iInnderIndex;
				m_arrMidPanelPalInfos[iOuterIndex][iInnderIndex] = _tPartnerPalInfo;

				wstring wstrTextureName = _tPartnerPalInfo.m_wstrTextureFileName;
				m_spPalboxMidChildPanel->FindComponent<CImageComponent>(L"mid_pal_slot_" + to_wstring(iOuterIndex) + to_wstring(iInnderIndex))->SetTextureFileName(wstrTextureName);


				// 플레이어 담당자의 요청에 따라, 한종류의 팰을 총 몇마리들고 있는지를 알게하기 위한
				auto iter = m_umapPartnerPalInfos.find(_tPartnerPalInfo.m_eMonsterType);
				if (iter != m_umapPartnerPalInfos.end())
				{
					m_umapPartnerPalInfos[_tPartnerPalInfo.m_eMonsterType]++;
				}
				else
				{
					m_umapPartnerPalInfos.insert({ _tPartnerPalInfo.m_eMonsterType, 1 });
				}
				break;
			}
		}
		if (iFlag == 1) { break; }
	}
}

void Client::CPalBoxSystem::AddPalToBaseCamp(FPalBoxPalInfo _tBaseCampPalInfo)
{
	EMonsterType eMonsterType = _tBaseCampPalInfo.m_eMonsterType;
	Vector3 vPalBoxPos = m_wpBuildManager.lock()->GetPalBoxPos();

	vPalBoxPos.x += 3.0f;
	vPalBoxPos.y += 10.0f;

	switch (eMonsterType)
	{
	case EMonsterType::BOAR:
	{
		FBoarDesc tPalDesc = {};

		// gameobject desc
		tPalDesc.m_vInitPosition				= vPalBoxPos;

		// creature desc
		tPalDesc.m_fCurExp					= _tBaseCampPalInfo.m_fCurExp;
		tPalDesc.m_fCurHp					= _tBaseCampPalInfo.m_fCurHp;
		tPalDesc.m_fCurHungerGauge			= _tBaseCampPalInfo.m_fCurHungerGauge;

		// monster desc
		tPalDesc.m_eMonsterType				= _tBaseCampPalInfo.m_eMonsterType;
		tPalDesc.m_eMonsterAIType			= EMonsterAIType::SLAVE_PAL;
		tPalDesc.m_eMonsterPersonalityType	= _tBaseCampPalInfo.m_eMonsterPersonalityType;
		tPalDesc.m_iMonsterLevel				= _tBaseCampPalInfo.m_iMonsterLevel;

		shared_ptr<CBoar> spPal = CBoar::Create(make_shared<FBoarDesc>(tPalDesc));
		m_arrBottomPanelPals[_tBaseCampPalInfo.m_iIndexX][_tBaseCampPalInfo.m_iIndexY] = spPal;
		m_spGameManager->GetCurLevel()->AddGameObject(spPal);
		spPal->SetJump(true);
	}
	break;

	case EMonsterType::BERRY_GOAT:
	{
		FBerryGoatDesc tPalDesc{};

		// gameobject desc
		tPalDesc.m_vInitPosition			= vPalBoxPos;

		// creature desc
		tPalDesc.m_fCurExp					= _tBaseCampPalInfo.m_fCurExp;
		tPalDesc.m_fCurHp					= _tBaseCampPalInfo.m_fCurHp;
		tPalDesc.m_fCurHungerGauge			= _tBaseCampPalInfo.m_fCurHungerGauge;

		// monster desc
		tPalDesc.m_eMonsterType				= _tBaseCampPalInfo.m_eMonsterType;
		tPalDesc.m_eMonsterAIType			= EMonsterAIType::SLAVE_PAL;
		tPalDesc.m_eMonsterPersonalityType	= _tBaseCampPalInfo.m_eMonsterPersonalityType;
		tPalDesc.m_iMonsterLevel			= _tBaseCampPalInfo.m_iMonsterLevel;

		shared_ptr<CBerryGoat> spPal = CBerryGoat::Create(make_shared<FBerryGoatDesc>(tPalDesc));
		m_arrBottomPanelPals[_tBaseCampPalInfo.m_iIndexX][_tBaseCampPalInfo.m_iIndexY] = spPal;
		m_spGameManager->GetCurLevel()->AddGameObject(spPal);
		spPal->SetJump(true);
	}
	break;

	case EMonsterType::CHICKEN_PAL:
	{	
		FChickenPalDesc tChickenPalDesc{};

		// gameobject desc
		tChickenPalDesc.m_vInitPosition				= vPalBoxPos;

		// creature desc
		tChickenPalDesc.m_fCurExp = _tBaseCampPalInfo.m_fCurExp;
		tChickenPalDesc.m_fCurHp = _tBaseCampPalInfo.m_fCurHp;
		tChickenPalDesc.m_fCurHungerGauge = _tBaseCampPalInfo.m_fCurHungerGauge;

		// monster desc
		tChickenPalDesc.m_eMonsterType = _tBaseCampPalInfo.m_eMonsterType;
		tChickenPalDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;
		tChickenPalDesc.m_eMonsterPersonalityType = _tBaseCampPalInfo.m_eMonsterPersonalityType;
		tChickenPalDesc.m_iMonsterLevel = _tBaseCampPalInfo.m_iMonsterLevel;

		shared_ptr<CChickenPal> spChickenPal = CChickenPal::Create(make_shared<FChickenPalDesc>(tChickenPalDesc));
		m_arrBottomPanelPals[_tBaseCampPalInfo.m_iIndexX][_tBaseCampPalInfo.m_iIndexY] = spChickenPal;
		m_spGameManager->GetCurLevel()->AddGameObject(spChickenPal);
		spChickenPal->SetJump(true);
	}
	break;

	case Client::EMonsterType::DREAM_DEMON:
	{
		FDreamDemonDesc tDreamDemonDesc = {};

		// creature desc
		tDreamDemonDesc.m_vInitPosition				= vPalBoxPos;

		// gameobject desc
		tDreamDemonDesc.m_fCurExp = _tBaseCampPalInfo.m_fCurExp;
		tDreamDemonDesc.m_fCurHp = _tBaseCampPalInfo.m_fCurHp;
		tDreamDemonDesc.m_fCurHungerGauge = _tBaseCampPalInfo.m_fCurHungerGauge;

		// monster desc
		tDreamDemonDesc.m_eMonsterType = _tBaseCampPalInfo.m_eMonsterType;
		tDreamDemonDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;
		tDreamDemonDesc.m_eMonsterPersonalityType = _tBaseCampPalInfo.m_eMonsterPersonalityType;
		tDreamDemonDesc.m_iMonsterLevel = _tBaseCampPalInfo.m_iMonsterLevel;

		shared_ptr<CDreamDemon> spDreamDemonPal = CDreamDemon::Create(make_shared<FDreamDemonDesc>(tDreamDemonDesc));
		m_arrBottomPanelPals[_tBaseCampPalInfo.m_iIndexX][_tBaseCampPalInfo.m_iIndexY] = spDreamDemonPal;
		m_spGameManager->GetCurLevel()->AddGameObject(spDreamDemonPal);
		spDreamDemonPal->SetJump(true);

	} break;

	case Client::EMonsterType::HEDGE_HOG:
	{
		FHedgeHogDesc tHedgeHogDesc = {};

		// gameobject desc
		tHedgeHogDesc.m_vInitPosition			= vPalBoxPos;

		// creature desc
		tHedgeHogDesc.m_fCurExp = _tBaseCampPalInfo.m_fCurExp;
		tHedgeHogDesc.m_fCurHp = _tBaseCampPalInfo.m_fCurHp;
		tHedgeHogDesc.m_fCurHungerGauge = _tBaseCampPalInfo.m_fCurHungerGauge;

		// monster desc
		tHedgeHogDesc.m_eMonsterType = _tBaseCampPalInfo.m_eMonsterType;
		tHedgeHogDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;
		tHedgeHogDesc.m_eMonsterPersonalityType = _tBaseCampPalInfo.m_eMonsterPersonalityType;
		tHedgeHogDesc.m_iMonsterLevel = _tBaseCampPalInfo.m_iMonsterLevel;

		shared_ptr<CHedgeHog> spHedgeHogPal = CHedgeHog::Create(make_shared<FHedgeHogDesc>(tHedgeHogDesc));
		m_arrBottomPanelPals[_tBaseCampPalInfo.m_iIndexX][_tBaseCampPalInfo.m_iIndexY] = spHedgeHogPal;
		m_spGameManager->GetCurLevel()->AddGameObject(spHedgeHogPal);
		spHedgeHogPal->SetJump(true);

	} break;

	case Client::EMonsterType::KITSUNEBI:
	{
		FKitsunebiDesc tKitsunebiDesc = {};

		// gameobject desc
		tKitsunebiDesc.m_vInitPosition				= vPalBoxPos;
		
		// creature desc		
		tKitsunebiDesc.m_eMonsterType = _tBaseCampPalInfo.m_eMonsterType;
		tKitsunebiDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;
		tKitsunebiDesc.m_eMonsterPersonalityType = _tBaseCampPalInfo.m_eMonsterPersonalityType;
		tKitsunebiDesc.m_iMonsterLevel = _tBaseCampPalInfo.m_iMonsterLevel;

		// monster desc
		tKitsunebiDesc.m_fCurExp = _tBaseCampPalInfo.m_fCurExp;
		tKitsunebiDesc.m_fCurHp = _tBaseCampPalInfo.m_fCurHp;
		tKitsunebiDesc.m_fCurHungerGauge = _tBaseCampPalInfo.m_fCurHungerGauge;

		shared_ptr<CKitsunebi> spKitsunebiPal = CKitsunebi::Create(make_shared<FKitsunebiDesc>(tKitsunebiDesc));
		m_arrBottomPanelPals[_tBaseCampPalInfo.m_iIndexX][_tBaseCampPalInfo.m_iIndexY] = spKitsunebiPal;
		m_spGameManager->GetCurLevel()->AddGameObject(spKitsunebiPal);
		spKitsunebiPal->SetJump(true);

	}
	break;

	case Client::EMonsterType::PENGUIN:
	{
		FPenguinDesc tPenguinDesc = {};

		// gameobject desc
		tPenguinDesc.m_vInitPosition			= vPalBoxPos;

		// creature desc
		tPenguinDesc.m_fCurExp = _tBaseCampPalInfo.m_fCurExp;
		tPenguinDesc.m_fCurHp = _tBaseCampPalInfo.m_fCurHp;
		tPenguinDesc.m_fCurHungerGauge = _tBaseCampPalInfo.m_fCurHungerGauge;

		// monster desc
		tPenguinDesc.m_eMonsterType = _tBaseCampPalInfo.m_eMonsterType;
		tPenguinDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;
		tPenguinDesc.m_eMonsterPersonalityType = _tBaseCampPalInfo.m_eMonsterPersonalityType;
		tPenguinDesc.m_iMonsterLevel = _tBaseCampPalInfo.m_iMonsterLevel;

		shared_ptr<CPenguin> spPenguinPal = CPenguin::Create(make_shared<FPenguinDesc>(tPenguinDesc));
		m_arrBottomPanelPals[_tBaseCampPalInfo.m_iIndexX][_tBaseCampPalInfo.m_iIndexY] = spPenguinPal;
		m_spGameManager->GetCurLevel()->AddGameObject(spPenguinPal);
		spPenguinPal->SetJump(true);

	}
	break;

	case Client::EMonsterType::PINK_CAT:
	{
		FPinkCatDesc tPinkCatDesc = {};

		// gameobject desc
		tPinkCatDesc.m_vInitPosition			= vPalBoxPos;
		
		// creature desc
		tPinkCatDesc.m_fCurExp = _tBaseCampPalInfo.m_fCurExp;
		tPinkCatDesc.m_fCurHp = _tBaseCampPalInfo.m_fCurHp;
		tPinkCatDesc.m_fCurHungerGauge = _tBaseCampPalInfo.m_fCurHungerGauge;

		// monster desc
		tPinkCatDesc.m_eMonsterType = _tBaseCampPalInfo.m_eMonsterType;
		tPinkCatDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;
		tPinkCatDesc.m_eMonsterPersonalityType = _tBaseCampPalInfo.m_eMonsterPersonalityType;
		tPinkCatDesc.m_iMonsterLevel = _tBaseCampPalInfo.m_iMonsterLevel;

		shared_ptr<CPinkCat> spPinkCatPal = CPinkCat::Create(make_shared<FPinkCatDesc>(tPinkCatDesc));
		m_arrBottomPanelPals[_tBaseCampPalInfo.m_iIndexX][_tBaseCampPalInfo.m_iIndexY] = spPinkCatPal;
		m_spGameManager->GetCurLevel()->AddGameObject(spPinkCatPal);
		spPinkCatPal->SetJump(true);

	}
	break;

	case Client::EMonsterType::SHEEP_BALL:
	{
		FSheepBallDesc tSheepBallDesc = {};

		// gameobject desc	
		tSheepBallDesc.m_vInitPosition			 = vPalBoxPos;

		// creature desc
		tSheepBallDesc.m_fCurExp = _tBaseCampPalInfo.m_fCurExp;
		tSheepBallDesc.m_fCurHp = _tBaseCampPalInfo.m_fCurHp;
		tSheepBallDesc.m_fCurHungerGauge = _tBaseCampPalInfo.m_fCurHungerGauge;

		// monster desc
		tSheepBallDesc.m_eMonsterType = _tBaseCampPalInfo.m_eMonsterType;
		tSheepBallDesc.m_eMonsterAIType = EMonsterAIType::SLAVE_PAL;
		tSheepBallDesc.m_eMonsterPersonalityType = _tBaseCampPalInfo.m_eMonsterPersonalityType;
		tSheepBallDesc.m_iMonsterLevel = _tBaseCampPalInfo.m_iMonsterLevel;

		shared_ptr<CSheepBall> spSheepBallPal = CSheepBall::Create(make_shared<FSheepBallDesc>(tSheepBallDesc));
		m_arrBottomPanelPals[_tBaseCampPalInfo.m_iIndexX][_tBaseCampPalInfo.m_iIndexY] = spSheepBallPal;
		m_spGameManager->GetCurLevel()->AddGameObject(spSheepBallPal);
		spSheepBallPal->SetJump(true);

	}
	break;

	case EMonsterType::WOOL_FOX:
	{
		FWoolFoxDesc tPalDesc{};

		// gameobject desc
		tPalDesc.m_vInitPosition			= vPalBoxPos;

		// creature desc
		tPalDesc.m_fCurExp					= _tBaseCampPalInfo.m_fCurExp;
		tPalDesc.m_fCurHp					= _tBaseCampPalInfo.m_fCurHp;
		tPalDesc.m_fCurHungerGauge			= _tBaseCampPalInfo.m_fCurHungerGauge;

		// monster desc
		tPalDesc.m_eMonsterType				= _tBaseCampPalInfo.m_eMonsterType;
		tPalDesc.m_eMonsterAIType			= EMonsterAIType::SLAVE_PAL;
		tPalDesc.m_eMonsterPersonalityType	= _tBaseCampPalInfo.m_eMonsterPersonalityType;
		tPalDesc.m_iMonsterLevel			= _tBaseCampPalInfo.m_iMonsterLevel;

		shared_ptr<CWoolFox> spPal = CWoolFox::Create(make_shared<FWoolFoxDesc>(tPalDesc));
		m_arrBottomPanelPals[_tBaseCampPalInfo.m_iIndexX][_tBaseCampPalInfo.m_iIndexY] = spPal;
		m_spGameManager->GetCurLevel()->AddGameObject(spPal);
		spPal->SetJump(true);
	}
	break;

	default: CHECKF(false, L"Monster type is not designated."); break;
	}
}

FPalBoxPalInfo Client::CPalBoxSystem::GeneratePalInfoStruct(const std::shared_ptr<CMonster>& _spMonster, std::pair<int32, int32> _iCurClickedIndex)
{
	EMonsterType eMonsterType = _spMonster->GetMonsterType();

	FPalBoxPalInfo tPalBoxPalInfo = {};
	tPalBoxPalInfo.m_iIndexX				 = _iCurClickedIndex.first;
	tPalBoxPalInfo.m_iIndexY				 = _iCurClickedIndex.second;
	tPalBoxPalInfo.m_wstrTextureFileName	 = CClientLoop::s_spClientDataSheet->k_mapPalIconTextureNames.at(eMonsterType);
	tPalBoxPalInfo.m_eMonsterType			 = _spMonster->GetMonsterType();
	tPalBoxPalInfo.m_eMonsterAIType			 = _spMonster->GetMonsterAIType();
	tPalBoxPalInfo.m_eMonsterPersonalityType = _spMonster->GetMonsterPersonalityType();
	tPalBoxPalInfo.m_eMonsterElementType	 = CClientLoop::s_spClientDataSheet->k_mapPalElementTypes.at(eMonsterType);
	tPalBoxPalInfo.m_wstrMonsterName		 = CClientLoop::s_spClientDataSheet->k_mapPalName.at(eMonsterType);
	tPalBoxPalInfo.m_iMonsterLevel			 = _spMonster->GetCurCreatureLevel();
	tPalBoxPalInfo.m_fCurHp					 = _spMonster->GetCurHp();
	tPalBoxPalInfo.m_fCurHungerGauge		 = _spMonster->GetCurHungerGauge();
	tPalBoxPalInfo.m_fCurExp				 = _spMonster->GetCurExp();
	tPalBoxPalInfo.m_fAttackPower			 = _spMonster->GetAttackPower();
	tPalBoxPalInfo.m_fDefensePower			 = _spMonster->GetDefensePower();
	tPalBoxPalInfo.m_eToDoType				 = EToDoType::ENUM_END; 
	tPalBoxPalInfo.m_fWorkSpeed				 = CClientLoop::s_spClientDataSheet->k_mapPalDefaultToDoRatios.at(eMonsterType);
	tPalBoxPalInfo.m_iFoodAmount			 = CClientLoop::s_spClientDataSheet->k_mapPalFoodAmounts.at(eMonsterType);
	tPalBoxPalInfo.m_iPartnerSkillLevel		 = _spMonster->GetPartnerSkillLevel();
	tPalBoxPalInfo.m_iActiveSkillLevelA		 = _spMonster->GetActiveSkillLevelA();
	tPalBoxPalInfo.m_iActiveSkillLevelB		 = _spMonster->GetActiveSkillLevelB();
	tPalBoxPalInfo.m_iActiveSkillLevelC		 = _spMonster->GetActiveSkillLevelC();
	tPalBoxPalInfo.m_wstrPartnerSkillName	 = CClientLoop::s_spClientDataSheet->k_mapPalPartnerSkillName.at(eMonsterType);
	tPalBoxPalInfo.m_wstrActiveSkillNameA	 = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameA.at(eMonsterType);
	tPalBoxPalInfo.m_wstrActiveSkillNameB	 = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameB.at(eMonsterType);
	tPalBoxPalInfo.m_wstrActiveSkillNameC	 = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameC.at(eMonsterType);
	tPalBoxPalInfo.m_eActiveSkillElementA	 = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeA.at(eMonsterType);
	tPalBoxPalInfo.m_eActiveSkillElementB	 = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeB.at(eMonsterType);
	tPalBoxPalInfo.m_eActiveSkillElementC	 = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeC.at(eMonsterType);
	tPalBoxPalInfo.m_wstrPassiveSkillNameA   = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameA.at(eMonsterType);
	tPalBoxPalInfo.m_wstrPassiveSkillNameB   = CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameB.at(eMonsterType);
	
	return tPalBoxPalInfo;
}

void Client::CPalBoxSystem::InsertPalInfoAndPalIntoPartnerPalArr(FPalBoxPalInfo _tPalBoxPalInfo)
{
	EMonsterType eMonsterType = _tPalBoxPalInfo.m_eMonsterType;

	switch (eMonsterType)
	{
	case Client::EMonsterType::BERRY_GOAT:
	{
		// component
		FBerryGoatDesc tBerryGoatDesc{};
		tBerryGoatDesc.m_eMonsterType = EMonsterType::CHICKEN_PAL;
		tBerryGoatDesc.m_vInitPosition = Vector3(0.0f, -100.0f, 0.0f);

		// creature desc
		tBerryGoatDesc.m_fCurExp = _tPalBoxPalInfo.m_fCurExp;
		tBerryGoatDesc.m_fCurHp = _tPalBoxPalInfo.m_fCurHp;
		tBerryGoatDesc.m_fCurHungerGauge = _tPalBoxPalInfo.m_fCurHungerGauge;

		// monster desc
		tBerryGoatDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tBerryGoatDesc.m_eMonsterPersonalityType = _tPalBoxPalInfo.m_eMonsterPersonalityType;
		tBerryGoatDesc.m_iMonsterLevel = _tPalBoxPalInfo.m_iMonsterLevel;

		shared_ptr<CBerryGoat> spBerryGoatPal = CBerryGoat::Create(make_shared<FBerryGoatDesc>(tBerryGoatDesc));
		m_spGameManager->GetCurLevel()->AddGameObject(spBerryGoatPal);

		spBerryGoatPal->SetActive(false);
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].first = _tPalBoxPalInfo;
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].second = spBerryGoatPal;

	} break;

	case Client::EMonsterType::CHICKEN_PAL:
	{
		// component
		FChickenPalDesc tChickenPalDesc{};
		tChickenPalDesc.m_eMonsterType = EMonsterType::CHICKEN_PAL;
		tChickenPalDesc.m_vInitPosition = Vector3(0.0f, -100.0f, 0.0f);

		// creature desc
		tChickenPalDesc.m_fCurExp = _tPalBoxPalInfo.m_fCurExp;
		tChickenPalDesc.m_fCurHp = _tPalBoxPalInfo.m_fCurHp;
		tChickenPalDesc.m_fCurHungerGauge = _tPalBoxPalInfo.m_fCurHungerGauge;

		// monster desc
		tChickenPalDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tChickenPalDesc.m_eMonsterPersonalityType = _tPalBoxPalInfo.m_eMonsterPersonalityType;
		tChickenPalDesc.m_iMonsterLevel = _tPalBoxPalInfo.m_iMonsterLevel;

		shared_ptr<CChickenPal> spChickenPal = CChickenPal::Create(make_shared<FChickenPalDesc>(tChickenPalDesc));
		m_spGameManager->GetCurLevel()->AddGameObject(spChickenPal);

		spChickenPal->SetActive(false);
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].first = _tPalBoxPalInfo;
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].second = spChickenPal;

	} break;

	case Client::EMonsterType::DREAM_DEMON:
	{
		// component
		FDreamDemonDesc tDreamDemonDesc = {};
		tDreamDemonDesc.m_eMonsterType = EMonsterType::DREAM_DEMON;
		tDreamDemonDesc.m_vInitPosition = Vector3(0.0f, -100.0f, 0.0f);

		// creature desc
		tDreamDemonDesc.m_fCurExp = _tPalBoxPalInfo.m_fCurExp;
		tDreamDemonDesc.m_fCurHp = _tPalBoxPalInfo.m_fCurHp;
		tDreamDemonDesc.m_fCurHungerGauge = _tPalBoxPalInfo.m_fCurHungerGauge;

		// monster desc
		tDreamDemonDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tDreamDemonDesc.m_eMonsterPersonalityType = _tPalBoxPalInfo.m_eMonsterPersonalityType;
		tDreamDemonDesc.m_iMonsterLevel = _tPalBoxPalInfo.m_iMonsterLevel;

		shared_ptr<CDreamDemon> spDreamDemonPal = CDreamDemon::Create(make_shared<FDreamDemonDesc>(tDreamDemonDesc));
		m_spGameManager->GetCurLevel()->AddGameObject(spDreamDemonPal);

		spDreamDemonPal->SetActive(false);
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].first = _tPalBoxPalInfo;
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].second = spDreamDemonPal;

	} break;

	case Client::EMonsterType::HEDGE_HOG:
	{
		// component
		FHedgeHogDesc tHedgeHogDesc{};
		tHedgeHogDesc.m_eMonsterType = EMonsterType::HEDGE_HOG;
		tHedgeHogDesc.m_vInitPosition = Vector3(0.0f, -100.0f, 0.0f);

		// creature desc
		tHedgeHogDesc.m_fCurExp = _tPalBoxPalInfo.m_fCurExp;
		tHedgeHogDesc.m_fCurHp = _tPalBoxPalInfo.m_fCurHp;
		tHedgeHogDesc.m_fCurHungerGauge = _tPalBoxPalInfo.m_fCurHungerGauge;

		// monster desc
		tHedgeHogDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tHedgeHogDesc.m_eMonsterPersonalityType = _tPalBoxPalInfo.m_eMonsterPersonalityType;
		tHedgeHogDesc.m_iMonsterLevel = _tPalBoxPalInfo.m_iMonsterLevel;


		shared_ptr<CHedgeHog> spHedgeHogPal = CHedgeHog::Create(make_shared<FHedgeHogDesc>(tHedgeHogDesc));
		m_spGameManager->GetCurLevel()->AddGameObject(spHedgeHogPal);

		spHedgeHogPal->SetActive(false);
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].first = _tPalBoxPalInfo;
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].second = spHedgeHogPal;

	} break;

	case Client::EMonsterType::KITSUNEBI:
	{
		// component
		FKitsunebiDesc tKitsunebiDesc = {};
		tKitsunebiDesc.m_eMonsterType = EMonsterType::KITSUNEBI;
		tKitsunebiDesc.m_vInitPosition = Vector3(0.0f, -100.0f, 0.0f);

		// creature desc
		tKitsunebiDesc.m_fCurExp = _tPalBoxPalInfo.m_fCurExp;
		tKitsunebiDesc.m_fCurHp = _tPalBoxPalInfo.m_fCurHp;
		tKitsunebiDesc.m_fCurHungerGauge = _tPalBoxPalInfo.m_fCurHungerGauge;

		// monster desc
		tKitsunebiDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tKitsunebiDesc.m_eMonsterPersonalityType = _tPalBoxPalInfo.m_eMonsterPersonalityType;
		tKitsunebiDesc.m_iMonsterLevel = _tPalBoxPalInfo.m_iMonsterLevel;

		shared_ptr<CKitsunebi> spKitsunebiPal = CKitsunebi::Create(make_shared<FKitsunebiDesc>(tKitsunebiDesc));
		m_spGameManager->GetCurLevel()->AddGameObject(spKitsunebiPal);

		spKitsunebiPal->SetActive(false);
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].first = _tPalBoxPalInfo;
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].second = spKitsunebiPal;

	} break;

	case Client::EMonsterType::PENGUIN:
	{
		// component
		FPenguinDesc tPenguinDesc = {};
		tPenguinDesc.m_eMonsterType = EMonsterType::PENGUIN;
		tPenguinDesc.m_vInitPosition = Vector3(0.0f, -100.0f, 0.0f);

		// creature desc
		tPenguinDesc.m_fCurExp = _tPalBoxPalInfo.m_fCurExp;
		tPenguinDesc.m_fCurHp = _tPalBoxPalInfo.m_fCurHp;
		tPenguinDesc.m_fCurHungerGauge = _tPalBoxPalInfo.m_fCurHungerGauge;

		// monster desc
		tPenguinDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tPenguinDesc.m_eMonsterPersonalityType = _tPalBoxPalInfo.m_eMonsterPersonalityType;
		tPenguinDesc.m_iMonsterLevel = _tPalBoxPalInfo.m_iMonsterLevel;

		shared_ptr<CPenguin> spPenguinPal = CPenguin::Create(make_shared<FPenguinDesc>(tPenguinDesc));
		m_spGameManager->GetCurLevel()->AddGameObject(spPenguinPal);

		spPenguinPal->SetActive(false);
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].first = _tPalBoxPalInfo;
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].second = spPenguinPal;

	} break;

	case Client::EMonsterType::PINK_CAT:
	{
		// gameobject desc
		FPinkCatDesc tPinkCatDesc{};
		tPinkCatDesc.m_eMonsterType = EMonsterType::PINK_CAT;
		tPinkCatDesc.m_vInitPosition = Vector3(0.0f, -100.0f, 0.0f);

		// creature desc
		tPinkCatDesc.m_fCurExp = _tPalBoxPalInfo.m_fCurExp;
		tPinkCatDesc.m_fCurHp = _tPalBoxPalInfo.m_fCurHp;
		tPinkCatDesc.m_fCurHungerGauge = _tPalBoxPalInfo.m_fCurHungerGauge;

		// monster desc
		tPinkCatDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tPinkCatDesc.m_eMonsterPersonalityType = _tPalBoxPalInfo.m_eMonsterPersonalityType;
		tPinkCatDesc.m_iMonsterLevel = _tPalBoxPalInfo.m_iMonsterLevel;

		shared_ptr<CPinkCat> spPinkCatPal = CPinkCat::Create(make_shared<FPinkCatDesc>(tPinkCatDesc));
		m_spGameManager->GetCurLevel()->AddGameObject(spPinkCatPal);

		spPinkCatPal->SetActive(false);
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].first = _tPalBoxPalInfo;
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].second = spPinkCatPal;

	} break;

	case Client::EMonsterType::SHEEP_BALL:
	{
		// component desc
		FSheepBallDesc tSheepBallDesc{};
		tSheepBallDesc.m_eMonsterType = EMonsterType::SHEEP_BALL;
		tSheepBallDesc.m_vInitPosition = Vector3(0.0f, -100.0f, 0.0f);

		// creature desc
		tSheepBallDesc.m_fCurExp = _tPalBoxPalInfo.m_fCurExp;
		tSheepBallDesc.m_fCurHp = _tPalBoxPalInfo.m_fCurHp;
		tSheepBallDesc.m_fCurHungerGauge = _tPalBoxPalInfo.m_fCurHungerGauge;

		// monster desc
		tSheepBallDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tSheepBallDesc.m_eMonsterPersonalityType = _tPalBoxPalInfo.m_eMonsterPersonalityType;
		tSheepBallDesc.m_iMonsterLevel = _tPalBoxPalInfo.m_iMonsterLevel;

		shared_ptr<CSheepBall> spSheepBallPal = CSheepBall::Create(make_shared<FSheepBallDesc>(tSheepBallDesc));
		m_spGameManager->GetCurLevel()->AddGameObject(spSheepBallPal);

		spSheepBallPal->SetActive(false);
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].first = _tPalBoxPalInfo;
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].second = spSheepBallPal;

	} break;

	case Client::EMonsterType::WOOL_FOX:
	{
		// component desc
		FWoolFoxDesc tWoolFoxDesc{};
		tWoolFoxDesc.m_eMonsterType = EMonsterType::SHEEP_BALL;
		tWoolFoxDesc.m_vInitPosition = Vector3(0.0f, -100.0f, 0.0f);

		// creature desc
		tWoolFoxDesc.m_fCurExp = _tPalBoxPalInfo.m_fCurExp;
		tWoolFoxDesc.m_fCurHp = _tPalBoxPalInfo.m_fCurHp;
		tWoolFoxDesc.m_fCurHungerGauge = _tPalBoxPalInfo.m_fCurHungerGauge;

		// monster desc
		tWoolFoxDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tWoolFoxDesc.m_eMonsterPersonalityType = _tPalBoxPalInfo.m_eMonsterPersonalityType;
		tWoolFoxDesc.m_iMonsterLevel = _tPalBoxPalInfo.m_iMonsterLevel;

		shared_ptr<CWoolFox> spWoolFoxPal = CWoolFox::Create(make_shared<FWoolFoxDesc>(tWoolFoxDesc));
		m_spGameManager->GetCurLevel()->AddGameObject(spWoolFoxPal);

		spWoolFoxPal->SetActive(false);
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].first = _tPalBoxPalInfo;
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].second = spWoolFoxPal;

	} break;

	case Client::EMonsterType::BOAR:
	{
		// component desc
		FBoarDesc tBoarDesc{};
		tBoarDesc.m_eMonsterType = EMonsterType::SHEEP_BALL;
		tBoarDesc.m_vInitPosition = Vector3(0.0f, -100.0f, 0.0f);

		// creature desc
		tBoarDesc.m_fCurHp = _tPalBoxPalInfo.m_fCurHp;
		tBoarDesc.m_fCurExp = _tPalBoxPalInfo.m_fCurExp;
		tBoarDesc.m_fCurHungerGauge = _tPalBoxPalInfo.m_fCurHungerGauge;

		// monster desc
		tBoarDesc.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tBoarDesc.m_eMonsterPersonalityType = _tPalBoxPalInfo.m_eMonsterPersonalityType;
		tBoarDesc.m_iMonsterLevel = _tPalBoxPalInfo.m_iMonsterLevel;

		shared_ptr<CBoar> spBoarPal = CBoar::Create(make_shared<FBoarDesc>(tBoarDesc));
		m_spGameManager->GetCurLevel()->AddGameObject(spBoarPal);

		spBoarPal->SetActive(false);
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].first = _tPalBoxPalInfo;
		(*m_arrLeftPanelPals)[_tPalBoxPalInfo.m_iIndexX].second = spBoarPal;

	} break;

	default: CHECKF(false, L"Monster type is not designated."); break;
	}
}

void Client::CPalBoxSystem::SortPalSlots()
{
	if (m_wpCurLeftClickedSlotComponent.expired()) { return; }

	std::wstring wstrCurLeftClickedSlotComponentName = m_wpCurLeftClickedSlotComponent.lock()->GetComponentName();
	if (wstrCurLeftClickedSlotComponentName == L"mid_sort_none")
	{
		SortPalsInPalBox(ESortingType::MONSTER_TYPE);
		m_wpCurMouseSlotComponent.lock()->SetTextureFileName(L"T_pal_icon_none");
		m_spUIManager->SetFocusedControlComponent(nullptr);
	}
}

void Client::CPalBoxSystem::SortPalsInPalBox(ESortingType _eSortingType)
{
	int32 iIndex = 0;
	std::array<FPalBoxPalInfo, 30> arrPals = {};

	for (int32 i = 0; i < m_arrMidPanelPalInfos.size(); i++)
	{
		for (int32 j = 0; j < m_arrMidPanelPalInfos[i].size(); j++)
		{
			arrPals[iIndex++] = m_arrMidPanelPalInfos[i][j];

			wstring wstrMidPanelComponentName = L"mid_pal_slot_" + std::to_wstring(i) + std::to_wstring(j);
			m_spPalboxMidChildPanel->FindComponent<CImageComponent>(wstrMidPanelComponentName)->SetTextureFileName(L"T_pal_icon_none");
		}
	}

	// 정렬
	std::sort(arrPals.begin(), arrPals.end(), [&](const FPalBoxPalInfo& _spPalBoxPalInfoA, const FPalBoxPalInfo& _spPalBoxPalInfoB) -> bool
		{
			switch (_eSortingType)
			{
			case ESortingType::RANDOM:		{ return _spPalBoxPalInfoA.m_iMonsterLevel < _spPalBoxPalInfoB.m_iMonsterLevel; }
			case ESortingType::MONSTER_TYPE:	{ return _spPalBoxPalInfoA.m_eMonsterType < _spPalBoxPalInfoB.m_eMonsterType; }
			}
			return 0;
		});

	iIndex = 0;
	for (int32 i = 0; i < m_arrMidPanelPalInfos.size(); i++)
	{
		for (int32 j = 0; j < m_arrMidPanelPalInfos[i].size(); j++)
		{
			m_arrMidPanelPalInfos[i][j] = arrPals[iIndex++];

			if (m_arrMidPanelPalInfos[i][j].m_eMonsterType == EMonsterType::ENUM_END) { continue; }

			m_arrMidPanelPalInfos[i][j].m_iIndexX = i;
			m_arrMidPanelPalInfos[i][j].m_iIndexY = j;

			wstring wstrMidPanelComponentName = L"mid_pal_slot_" + std::to_wstring(i) + std::to_wstring(j);
			m_spPalboxMidChildPanel->FindComponent<CImageComponent>(wstrMidPanelComponentName)->SetTextureFileName(m_arrMidPanelPalInfos[i][j].m_wstrTextureFileName);
		}
	}
}

void Client::CPalBoxSystem::ExecuteSlavePal(std::shared_ptr<CMonster> _spMonster)
{
	int32 iFlag = 0;
	for (int32 iIndexX = 0; iIndexX < m_arrBottomPanelPals.size(); iIndexX++)
	{
		for (int32 iIndexY = 0; iIndexY < m_arrBottomPanelPals[iIndexX].size(); iIndexY++)
		{
			if (m_arrBottomPanelPals[iIndexX][iIndexY] == _spMonster)
			{
				// 이전 클릭한 슬롯에 있던 객체 포인터를 복사한 후 null로 초기화 시킵니다.
				m_arrBottomPanelPals[iIndexX][iIndexY] = nullptr;

				// 레벨에서 삭제해줍니다.
				_spMonster->RemoveSlavePalFromToDoSystem();

				m_spGameManager->GetCurLevel()->RemoveGameObject(_spMonster);
				_spMonster = nullptr;

				m_spPalboxBottomChildPanel->FindComponent<CImageComponent>(L"bottom_pal_slot_" + to_wstring(iIndexX) + to_wstring(iIndexY))->SetTextureFileName(L"T_pal_icon_none");
				
				iFlag = 1;

				break;
			}
		}

		if (iFlag == 1) { break; }
	}
}

void Client::CPalBoxSystem::ActiveSlotHover()
{
	if (!m_wpCurHoverSlotComponent.expired())
	{
		wstring wstrCurHoverSlotComponentName = m_wpCurHoverSlotComponent.lock()->GetComponentName();
		wstring wstrCurHoverSlotComponentNammeWithoutLastTwoNumber = ExceptLastChar(wstrCurHoverSlotComponentName, 2);

		if (wstrCurHoverSlotComponentNammeWithoutLastTwoNumber == L"left_none_hover_")
		{
			m_wpLeftHoverImage.lock()->SetCenterPosition(m_wpCurHoverSlotComponent.lock()->GetCenterPosition());
			m_wpLeftHoverImage.lock()->SetActive(true);
		}
		else if (wstrCurHoverSlotComponentName == L"mid_sort_none")
		{
			m_wpSortHoverImage.lock()->SetCenterPosition(m_wpCurHoverSlotComponent.lock()->GetCenterPosition());
			m_wpSortHoverImage.lock()->SetActive(true);
		}
		else if (wstrCurHoverSlotComponentNammeWithoutLastTwoNumber == L"bottom_pal_slot_")
		{
			m_wpBottomHoverImage.lock()->SetCenterPosition(m_wpCurHoverSlotComponent.lock()->GetCenterPosition());
			m_wpBottomHoverImage.lock()->SetActive(true);
		}
		else
		{
			m_wpMidHoverImage.lock()->SetCenterPosition(m_wpCurHoverSlotComponent.lock()->GetCenterPosition());
			m_wpMidHoverImage.lock()->SetActive(true);
		}
	}
	else
	{
		m_wpLeftHoverImage.lock()->SetActive(false);
		m_wpMidHoverImage.lock()->SetActive(false);
		m_wpBottomHoverImage.lock()->SetActive(false);
		m_wpSortHoverImage.lock()->SetActive(false);
	}
}

void Client::CPalBoxSystem::ShowPartnerPalInfoOnLeftPanel()
{
	for (int32 i = 0; i < (*m_arrLeftPanelPals).size(); i++)
	{
		if ((*m_arrLeftPanelPals)[i].first.m_eMonsterType != EMonsterType::ENUM_END)
		{
			wstring wstrLeftPanelIndexs				 = std::to_wstring(i) + std::to_wstring(0);
			wstring wstrLeftPanelHpComponentName	 = L"left_pal_hp_" + wstrLeftPanelIndexs;
			wstring wstrLeftPanelHungerComponentName = L"left_pal_hunger_" + wstrLeftPanelIndexs;
			wstring wstrLeftPanelIconComponentName	 = L"left_pal_icon_" + wstrLeftPanelIndexs;
			wstring wstrLeftPanelLevelComponentName  = L"left_pal_level_" + wstrLeftPanelIndexs;
			wstring wstrLeftPanelNameComponentName	 = L"left_pal_name_" + wstrLeftPanelIndexs;
			wstring wstrLeftPanelStatusComponentName = L"left_pal_status_" + wstrLeftPanelIndexs;

			weak_ptr<CProgressBarComponent> wpHpComponent     = m_spPalboxLeftChildPanel->FindComponent<CProgressBarComponent>(wstrLeftPanelHpComponentName);
			weak_ptr<CProgressBarComponent> wpHungerComponent = m_spPalboxLeftChildPanel->FindComponent<CProgressBarComponent>(wstrLeftPanelHungerComponentName);
			weak_ptr<CImageComponent> wpIconComponent		  = m_spPalboxLeftChildPanel->FindComponent<CImageComponent>(wstrLeftPanelIconComponentName);
			weak_ptr<CTextComponent> wpLevelComponent		  = m_spPalboxLeftChildPanel->FindComponent<CTextComponent>(wstrLeftPanelLevelComponentName);
			weak_ptr<CTextComponent> wpNameComponent		  = m_spPalboxLeftChildPanel->FindComponent<CTextComponent>(wstrLeftPanelNameComponentName);
			weak_ptr<CRectComponent> wpStatusComponent		  = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(wstrLeftPanelStatusComponentName);

			float fPalMaxHp = CClientLoop::s_spClientDataSheet->k_mapPalHPs[(*m_arrLeftPanelPals)[i].first.m_eMonsterType];
			float fPalMaxHunger = CClientLoop::s_spClientDataSheet->k_fMaxHungerGauge;

			float fHpRatio = (*m_arrLeftPanelPals)[i].first.m_fCurHp / fPalMaxHp;
			float fHungerRatio = (*m_arrLeftPanelPals)[i].first.m_fCurHungerGauge / fPalMaxHunger;
			wpHpComponent.lock()->SetRatio(fHpRatio);
			wpHungerComponent.lock()->SetRatio(fHungerRatio);
			wpIconComponent.lock()->SetTextureFileName((*m_arrLeftPanelPals)[i].first.m_wstrTextureFileName);
			wpLevelComponent.lock()->SetText(std::to_wstring((*m_arrLeftPanelPals)[i].first.m_iMonsterLevel));
			wpNameComponent.lock()->SetText((*m_arrLeftPanelPals)[i].first.m_wstrMonsterName);

			wpHpComponent.lock()->SetActive(true);
			wpHungerComponent.lock()->SetActive(true);
			wpIconComponent.lock()->SetActive(true);
			wpLevelComponent.lock()->SetActive(true);
			wpNameComponent.lock()->SetActive(true);
			wpStatusComponent.lock()->SetActive(true);
		}
	}
}

void Client::CPalBoxSystem::HiddenPartnerPalInfoOnLeftPanel(int32 _iPreClickedSlotIndex)
{
	wstring wstrLeftPanelIndexs = std::to_wstring(_iPreClickedSlotIndex) + std::to_wstring(0);
	wstring wstrLeftPanelHpComponentName = L"left_pal_hp_" + wstrLeftPanelIndexs;
	wstring wstrLeftPanelHungerComponentName = L"left_pal_hunger_" + wstrLeftPanelIndexs;
	wstring wstrLeftPanelIconComponentName = L"left_pal_icon_" + wstrLeftPanelIndexs;
	wstring wstrLeftPanelLevelComponentName = L"left_pal_level_" + wstrLeftPanelIndexs;
	wstring wstrLeftPanelNameComponentName = L"left_pal_name_" + wstrLeftPanelIndexs;
	wstring wstrLeftPanelStatusComponentName = L"left_pal_status_" + wstrLeftPanelIndexs;

	weak_ptr<CProgressBarComponent> wpHpComponent = m_spPalboxLeftChildPanel->FindComponent<CProgressBarComponent>(wstrLeftPanelHpComponentName);
	weak_ptr<CProgressBarComponent> wpHungerComponent = m_spPalboxLeftChildPanel->FindComponent<CProgressBarComponent>(wstrLeftPanelHungerComponentName);
	weak_ptr<CImageComponent> wpIconComponent = m_spPalboxLeftChildPanel->FindComponent<CImageComponent>(wstrLeftPanelIconComponentName);
	weak_ptr<CTextComponent> wpLevelComponent = m_spPalboxLeftChildPanel->FindComponent<CTextComponent>(wstrLeftPanelLevelComponentName);
	weak_ptr<CTextComponent> wpNameComponent = m_spPalboxLeftChildPanel->FindComponent<CTextComponent>(wstrLeftPanelNameComponentName);
	weak_ptr<CRectComponent> wpStatusComponent = m_spPalboxLeftChildPanel->FindComponent<CRectComponent>(wstrLeftPanelStatusComponentName);

	float fPalMaxHp = CClientLoop::s_spClientDataSheet->k_mapPalHPs[(*m_arrLeftPanelPals)[_iPreClickedSlotIndex].first.m_eMonsterType];
	float fPalMaxHunger = CClientLoop::s_spClientDataSheet->k_fMaxHungerGauge;

	float fHpRatio = (*m_arrLeftPanelPals)[_iPreClickedSlotIndex].first.m_fCurHp / fPalMaxHp;
	float fHungerRatio = (*m_arrLeftPanelPals)[_iPreClickedSlotIndex].first.m_fCurHungerGauge / fPalMaxHunger;

	wpHpComponent.lock()->SetRatio(fHpRatio);
	wpHungerComponent.lock()->SetRatio(fHungerRatio);
	wpIconComponent.lock()->SetTextureFileName(L"T_pal_icon_none"/*(*m_arrLeftPanelPals)[_iPreClickedSlotIndex].m_wstrTextureFileName*/);
	wpLevelComponent.lock()->SetText(std::to_wstring((*m_arrLeftPanelPals)[_iPreClickedSlotIndex].first.m_iMonsterLevel));
	wpNameComponent.lock()->SetText((*m_arrLeftPanelPals)[_iPreClickedSlotIndex].first.m_wstrMonsterName);

	wpHpComponent.lock()->SetActive(false);
	wpHungerComponent.lock()->SetActive(false);
	wpIconComponent.lock()->SetActive(false);
	wpLevelComponent.lock()->SetActive(false);
	wpNameComponent.lock()->SetActive(false);
	wpStatusComponent.lock()->SetActive(false);
}

void Client::CPalBoxSystem::ActivePalInfoWindow()
{
	if (!m_wpCurHoverSlotComponent.expired())
	{
		wstring wstrCurHoverSlotFileName = m_wpCurHoverSlotComponent.lock()->GetTextureName();
		wstring wstrCurHoverSlotComponentName = m_wpCurHoverSlotComponent.lock()->GetComponentName();

		/* hover에 팰이 없는 경우 */
		if ((wstrCurHoverSlotFileName == L"T_pal_icon_none" ||
			 wstrCurHoverSlotFileName == L"T_palbox_sort_none") ||
			((wstrCurHoverSlotComponentName == L"left_none_hover_00" && !m_vecLeftPanelPalStatus[10].lock()->IsActive()) ||
			 (wstrCurHoverSlotComponentName == L"left_none_hover_10" && !m_vecLeftPanelPalStatus[11].lock()->IsActive()) ||
			 (wstrCurHoverSlotComponentName == L"left_none_hover_20" && !m_vecLeftPanelPalStatus[12].lock()->IsActive()) ||
			 (wstrCurHoverSlotComponentName == L"left_none_hover_30" && !m_vecLeftPanelPalStatus[13].lock()->IsActive()) ||
			 (wstrCurHoverSlotComponentName == L"left_none_hover_40" && !m_vecLeftPanelPalStatus[14].lock()->IsActive())))
		{
			m_spPalboxRightChildPanel->SetActive(false);
			m_spPalboxRightNodataChildPanel->SetActive(true);
		}

		/* hover에 팰이 있는 경우 */
		else
		{
			UpdatePartnerPalInfoOnRightPanel();
			m_spPalboxRightChildPanel->SetActive(true);
			m_spPalboxRightNodataChildPanel->SetActive(false);
		}
	}
	else
	{
		m_spPalboxRightChildPanel->SetActive(false);
		m_spPalboxRightNodataChildPanel->SetActive(true);
	}
}

void Client::CPalBoxSystem::UpdatePartnerPalInfoOnRightPanel()
{
	wstring wstrCurHoverSlotFileName = m_wpCurHoverSlotComponent.lock()->GetTextureName();
	wstring wstrCurHoverSlotComponentName = m_wpCurHoverSlotComponent.lock()->GetComponentName();
	wstring wstrCurHoverSlotComponentNameWithoutLastTwoNumber = ExceptLastChar(wstrCurHoverSlotComponentName, 2);
	std::pair<int32, int32> prCurHoverSlotIndexs = ExtractLastChar(wstrCurHoverSlotComponentName, 2);

	// left
	if (wstrCurHoverSlotComponentNameWithoutLastTwoNumber == L"left_none_hover_")
	{
		FPalBoxPalInfo tLeftPanelPalInfo = (*m_arrLeftPanelPals)[prCurHoverSlotIndexs.first].first;
		PartnerPalInfoOnRightPanelPrefab(tLeftPanelPalInfo);
	}
	// mid
	else if (wstrCurHoverSlotComponentNameWithoutLastTwoNumber == L"mid_pal_slot_")
	{
		FPalBoxPalInfo tMidPanelPalInfo = m_arrMidPanelPalInfos[prCurHoverSlotIndexs.first][prCurHoverSlotIndexs.second];
		PartnerPalInfoOnRightPanelPrefab(tMidPanelPalInfo);
	}
	// bottom
	else if (wstrCurHoverSlotComponentNameWithoutLastTwoNumber == L"bottom_pal_slot_")
	{
		shared_ptr<CMonster> spMonster = m_arrBottomPanelPals[prCurHoverSlotIndexs.first][prCurHoverSlotIndexs.second];
		FPalBoxPalInfo tBottomPanelPalInfo = GeneratePalInfoStruct(spMonster, prCurHoverSlotIndexs);
		PartnerPalInfoOnRightPanelPrefab(tBottomPanelPalInfo);
	}
}

void Client::CPalBoxSystem::PartnerPalInfoOnRightPanelPrefab(FPalBoxPalInfo _tPalBoxPalInfo)
{
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[1].lock())->SetText(std::to_wstring(_tPalBoxPalInfo.m_iMonsterLevel)); // L"right_pal_info_level"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[2].lock())->SetText(m_spUIManager->FindPalName(_tPalBoxPalInfo.m_eMonsterType)); // L"right_pal_info_name"));

	float fMaxHp = CClientLoop::s_spClientDataSheet->k_mapPalHPs[_tPalBoxPalInfo.m_eMonsterType];
	float fMaxHunger = CClientLoop::s_spClientDataSheet->k_fMaxHungerGauge;
	static_pointer_cast<CProgressBarComponent>(m_vecRightPanelPalStatus[3].lock())->SetRatio(_tPalBoxPalInfo.m_fCurExp / 100); // L"right_pal_exp"));
	static_pointer_cast<CProgressBarComponent>(m_vecRightPanelPalStatus[7].lock())->SetRatio(_tPalBoxPalInfo.m_fCurHp / fMaxHp); // L"right_pal_hp"));
	static_pointer_cast<CProgressBarComponent>(m_vecRightPanelPalStatus[10].lock())->SetRatio(_tPalBoxPalInfo.m_fCurHungerGauge / fMaxHunger); // L"right_pal_hunger"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[4].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fCurExp))); // L"right_pal_info_exp_num"));
	static_pointer_cast<CImageComponent>(m_vecRightPanelPalStatus[5].lock())->SetTextureFileName(m_spUIManager->FindPalElementName(_tPalBoxPalInfo.m_eMonsterElementType)); // L"right_pal_element"));
	static_pointer_cast<CImageComponent>(m_vecRightPanelPalStatus[6].lock())->SetTextureFileName(_tPalBoxPalInfo.m_wstrTextureFileName); // L"right_pal_icon"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[8].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fCurHp))); // L"right_pal_hp_cur_num"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[9].lock())->SetText(L"/" + to_wstring((int32)fMaxHp)); // L"right_pal_hp_total_num"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[11].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fCurHungerGauge))); // L"right_pal_hunger_cur_num"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[12].lock())->SetText(L"/" + to_wstring((int32)fMaxHunger)); // L"right_pal_hunger_total_num"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[13].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fAttackPower))); // L"right_pal_atk_num"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[14].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fDefensePower))); // L"right_pal_def_num"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[15].lock())->SetText(std::to_wstring(static_cast<int32>(_tPalBoxPalInfo.m_fWorkSpeed))); // L"right_pal_workspeed_num"));

	int32 iPalWork = static_cast<int32>(_tPalBoxPalInfo.m_eToDoType);

	for (int32 i = 16; i < 28; i++)
	{
		if (i == iPalWork + 16)
		{
			m_vecRightPanelPalStatus[i].lock()->SetActive(true);
		}
		else
		{
			m_vecRightPanelPalStatus[i].lock()->SetActive(false);
		}
	}

	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[28].lock())->SetText(L"임무 중"); // L"right_cur_mission"));tLeftPanelPalInfo.m_eToDoType UiManager

	int32 iFoodAmount = _tPalBoxPalInfo.m_iFoodAmount;

	for (int32 i = 29; i < 39; i++)
	{
		if (i <= iFoodAmount + 29)
		{
			m_vecRightPanelPalStatus[i].lock()->SetActive(true);
		}
		else
		{
			m_vecRightPanelPalStatus[i].lock()->SetActive(false);
		}
	}

	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[39].lock())->SetText(_tPalBoxPalInfo.m_wstrPartnerSkillName); // L"right_partner_skill_name"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[40].lock())->SetText(std::to_wstring(_tPalBoxPalInfo.m_iPartnerSkillLevel)); // L"right_pal_skill_level"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[41].lock())->SetText(_tPalBoxPalInfo.m_wstrActiveSkillNameA); // L"right_active_skill_name_0"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[42].lock())->SetText(_tPalBoxPalInfo.m_wstrActiveSkillNameB); // L"right_active_skill_name_1"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[43].lock())->SetText(_tPalBoxPalInfo.m_wstrActiveSkillNameC); // L"right_active_skill_name_2"));
	static_pointer_cast<CImageComponent>(m_vecRightPanelPalStatus[44].lock())->SetTextureFileName(m_spUIManager->FindPalSkillElementName(_tPalBoxPalInfo.m_eActiveSkillElementA)); // L"right_pal_skill_element_0"));
	static_pointer_cast<CImageComponent>(m_vecRightPanelPalStatus[45].lock())->SetTextureFileName(m_spUIManager->FindPalSkillElementName(_tPalBoxPalInfo.m_eActiveSkillElementB)); // L"right_pal_skill_element_1"));
	static_pointer_cast<CImageComponent>(m_vecRightPanelPalStatus[46].lock())->SetTextureFileName(m_spUIManager->FindPalSkillElementName(_tPalBoxPalInfo.m_eActiveSkillElementC)); // L"right_pal_skill_element_2"));

	if (_tPalBoxPalInfo.m_eMonsterType == EMonsterType::WOOL_FOX)
	{
		static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[47].lock())->SetText(L""); // L"right_pal_active_skill_level_0"));
	}
	else
	{
		static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[47].lock())->SetText(std::to_wstring(_tPalBoxPalInfo.m_iActiveSkillLevelA)); // L"right_pal_active_skill_level_0"));
	}
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[48].lock())->SetText(L""); // L"right_pal_active_skill_level_1"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[49].lock())->SetText(L""); // L"right_pal_active_skill_level_2"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[50].lock())->SetText(_tPalBoxPalInfo.m_wstrPassiveSkillNameA); // L"right_passive_skill_name_0"));
	static_pointer_cast<CTextComponent>(m_vecRightPanelPalStatus[51].lock())->SetText(_tPalBoxPalInfo.m_wstrPassiveSkillNameB); // L"right_passive_skill_name_1"));
}
