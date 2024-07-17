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
			// ų ��
			if (false == m_spArchitecturePanel->GetActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("KeyInputSound");
				m_spArchitecturePanel->SetActive(true);
				m_wpUIManager.lock()->SetTopPanel(m_spArchitecturePanel);
			}
			// �� ��
			else
			{
				m_wpSoundManager.lock()->PlaySoundEx("KeyInputSound");
				m_spArchitecturePanel->SetActive(false);
				m_wpUIManager.lock()->ResetTopPanel();
			}
		}
	}
	// ���� ��� ���
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

		// ���� top�г��� ���� �г��� �� �����մϴ�.
		if (CEngineModule::GetInstance()->GetInputManager()->IsMouseDown(EMouseActionType::LEFT_BUTTON))
		{
			// Ŭ�� ���带 ����մϴ�.
			m_wpSoundManager.lock()->PlaySoundEx("NormalClickSound");

			if (-1 == m_iCurIndex)
			{
				return 0;
			}

			if (true == m_spCenterFailImageComponent->IsActive() || L"" == m_spCenterTextComponent->GetText())
			{
				return 0;
			}

			// �г��� ���ݴϴ�.
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

			// ���⼭ ���๰ ���⸦ �����մϴ�.
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

	// text�� �ѱ۷� ��ȯ�� �־��ݴϴ�.
	m_spArchitecturePanel->FindComponent<CTextComponent>(L"TabText0")->SetText(L"����");
	m_spArchitecturePanel->FindComponent<CTextComponent>(L"TabText1")->SetText(L"�ķ�ǰ");
	m_spArchitecturePanel->FindComponent<CTextComponent>(L"TabText2")->SetText(L"������");
	m_spArchitecturePanel->FindComponent<CTextComponent>(L"TabText3")->SetText(L"����");

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
				tCircleInfo.spTextComponent->SetText(L"���� ���");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_FOUNDATION;
				tCircleInfo.wstrDescriptionText = L"���� ���̳� �ü��� ���� �� �ִ� ���.\n������ ���� ���ϴ�.";
				break;
			case 1:
				tCircleInfo.spTextComponent->SetText(L"���� ��");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_WALL;
				tCircleInfo.wstrDescriptionText = L"��� ���� ���� ħ���� ������ ��.\n������ ���� ���ϴ�.";
				break;
			case 2:
				tCircleInfo.spTextComponent->SetText(L"â�� ���� ���� ��");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_WINDOW_WALL;
				tCircleInfo.wstrDescriptionText = L"��� ���� ���� â���� ���� ħ���� ������ ��.\n������ ���� ���ϴ�.";
				break;
			case 3:
				tCircleInfo.spTextComponent->SetText(L"���� �ﰢ ��");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_TRYANGLE_WALL;
				tCircleInfo.wstrDescriptionText = L"��� ���� ���� ���� �ﰢ ��.\n������ ���� ���ϴ�.";
				break;
			case 4:
				tCircleInfo.spTextComponent->SetText(L"���� ����");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_ROOF;
				tCircleInfo.wstrDescriptionText = L"�� ���� ���� ��ٶ� ������ ����.\n������ ���� ���ϴ�.";
				break;
			case 5:
				tCircleInfo.spTextComponent->SetText(L"���� ��� ����");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_SLOPE_ROOF;
				tCircleInfo.wstrDescriptionText = L"�� ���� ���� ��ٶ� ������ ��� ����.\n������ ���� ���ϴ�.";
				break;
			case 6:
				tCircleInfo.spTextComponent->SetText(L"���� ���");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_STAIRS;
				tCircleInfo.wstrDescriptionText = L"��볪 ���� �׿� ��ġ�� ���������� ���.\n������ ���� ���ϴ�.";
				break;
			case 7:
				tCircleInfo.spTextComponent->SetText(L"���� ��");
				tCircleInfo.wstrText = tCircleInfo.spTextComponent->GetText();
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_DOOR;
				tCircleInfo.wstrDescriptionText = L"��� ���� ���� �Ա��� ��.\n������ ���� ���ϴ�.";
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
				tCircleInfo.wstrText = L"�� ����";
				tCircleInfo.eArchitectureType = EArchitectureType::PAL_BOX;
				tCircleInfo.wstrDescriptionText = L"��ȹ�� ���� �����صδ� �ü�.\n�ڽ� �� ���� õõ�� HP�� ȸ���Ѵ�.\n�� �ü��� �߽����� �� ������ ������ �ȴ�.";
				break;
			case 1:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_ItemChest";
				tCircleInfo.wstrText = L"���� ����";
				tCircleInfo.eArchitectureType = EArchitectureType::WOODEN_CHEST;
				tCircleInfo.wstrDescriptionText = L"�������� ������ �� �ִ�.\n������ ���� ���ϴ�.\n����ǰ�� �ֱ⿣ �� �Ҿ��ϴ�.";
				break;
			case 2:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_ItemChest_02";
				tCircleInfo.wstrText = L"�ݼ� ����";
				tCircleInfo.eArchitectureType = EArchitectureType::IRON_BOX;
				tCircleInfo.wstrDescriptionText = L"�������� ������ �� �ִ�.\n�ݼ����� ������ �� �� ưư�ϴ�.\nĿ�� ��ŭ ���� �뷮�� �þ��.";
				break;
			case 3:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_PlayerBed_02";
				tCircleInfo.wstrText = L"�ϱ� ħ��";
				tCircleInfo.eArchitectureType = EArchitectureType::HUMAN_BED;
				tCircleInfo.wstrDescriptionText = L"�ΰ��� �ʴ��ʴ��� ħ��.\n���ưų� �㿡 �� �� ����Ѵ�.\n���� ������ ������ �Ҿ��ؼ� ��� ���� ����.";
				break;
			case 4:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_MedicalPalBed_02";
				tCircleInfo.wstrText = L"¤ �� ħ��";
				tCircleInfo.eArchitectureType = EArchitectureType::PAL_BED;
				tCircleInfo.wstrDescriptionText = L"�ӿ� ¤ ħ��.\n���� ���ưų� �㿡 �� �� ����Ѵ�.\n���������� ���� �ͺ��ٴ� ����.";
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
				tCircleInfo.wstrText = L"���� �۾���";
				tCircleInfo.eArchitectureType = EArchitectureType::WORKBENCH;
				tCircleInfo.wstrDescriptionText = L"������ �������� �����ϴ� �������� �۾���.\n���۾��� ������ ���� �ʿ��ϴ�.";
				break;
			case 1:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_WeaponFactoryBase";
				tCircleInfo.wstrText = L"���� ���۴�";
				tCircleInfo.eArchitectureType = EArchitectureType::WEAPON_WORKBENCH;
				tCircleInfo.wstrDescriptionText = L"���⳪ ź���� �����ϴ� ����.\n���۾��� ������ ���� �ʿ��ϴ�.";
				break;
			case 2:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_RepairBench";
				tCircleInfo.wstrText = L"������";
				tCircleInfo.eArchitectureType = EArchitectureType::REPAIR_BENCH;
				tCircleInfo.wstrDescriptionText = L"�ļյ� ������ �����ϴ� �۾���.\n���縦 �Ҹ��Ͽ� ������ �� �ִ�.";
				break;
			case 3:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_StationDeforest2";
				tCircleInfo.wstrText = L"������";
				tCircleInfo.eArchitectureType = EArchitectureType::LUMBERYARD;
				tCircleInfo.wstrDescriptionText = L"������(��) �����ϴ� �ü�.\n������ �ڸ��� �۾��� ��ǹǷ� ü���� �ʿ��ϴ�.\n���� �ڽ� �ִ� �ӿ��� �ñ���.";
				break;
			case 4:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_StonePit";
				tCircleInfo.wstrText = L"ä����";
				tCircleInfo.eArchitectureType = EArchitectureType::QUARRY;
				tCircleInfo.wstrDescriptionText = L"����(��) �����ϴ� �ü�.\n����(��) �ĳ��� �۾��� ��ǹǷ� ü���� �ʿ��ϴ�.\nä���� �ڽ� �ִ� �ӿ��� �ñ���.";
				break;
			case 5:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_FlourMill";
				tCircleInfo.wstrText = L"���б�";
				tCircleInfo.eArchitectureType = EArchitectureType::MILL;
				tCircleInfo.wstrDescriptionText = L"����(��) ���� �а�����(��)\n������ �� �ִ� �ü�.\n������Ƹ� ������ \"����\" ������ ���� �ʿ��ϴ�.";
				break;
			case 6:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_BlastFurnace";
				tCircleInfo.wstrText = L"�������� ȭ��";
				tCircleInfo.eArchitectureType = EArchitectureType::BLAST_FURNACE;
				tCircleInfo.wstrDescriptionText = L"�ݼ� �ֱ���(��) ������ �� �ִ�.\nǰ���� �� ���� �ʰ� �ӵ��� ������.\nȭ�� ���� ���� �ٿ���� �Ѵ�.";
				break;
			case 7:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_SphereFactory_Black_Base";
				tCircleInfo.wstrText = L"���Ǿ� ���۴�";
				tCircleInfo.eArchitectureType = EArchitectureType::SPHERE_TABLE;
				tCircleInfo.wstrDescriptionText = L"�� ��ȹ�� ���Ǿ� ���� ����.\n���� �۾����� ������ ���Ǿ ������ �����ϱ�� �����.\n���۾��� ������ ���� �ʿ��ϴ�.";
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
				tCircleInfo.wstrText = L"��ں�";
				tCircleInfo.eArchitectureType = EArchitectureType::CAMPFIRE;
				tCircleInfo.wstrDescriptionText = L"����Ḧ ������ �� �ʿ��� �ü�.\n������ ��Ḧ ���ش� ������ �����ϴ�.\nȭ�� ���� ���� �ٿ���� �Ѵ�.";
				break;
			case 1:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_CookingStove";
				tCircleInfo.wstrText = L"���� ����";
				tCircleInfo.eArchitectureType = EArchitectureType::POT;
				tCircleInfo.wstrDescriptionText = L"����Ḧ ������ �� �ʿ��� �ü�.\n���� Ȱ���� ���п� ������ ���� �о�����.\nȭ�� ���� ���� �ٿ���� �Ѵ�.";
				break;
			case 2:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_PalFoodBox";
				tCircleInfo.wstrText = L"���� ����";
				tCircleInfo.eArchitectureType = EArchitectureType::FEEDING_BOX;
				tCircleInfo.wstrDescriptionText = L"���� ���̸� �־�δ� ������.\n�谡 ���� ���� ���⼭ ���̸� �Դ´�.\n���̰� �������� �ʰ� ��������.";
				break;
			case 3:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_FarmBlockV2_Berries";
				tCircleInfo.wstrText = L"���� ���";
				tCircleInfo.eArchitectureType = EArchitectureType::BERRY_FARM;
				tCircleInfo.wstrDescriptionText = L"���� ������(��) �⸦ �� �ִ� ���.\n�ݹ� ��Ȯ�� �� ������ �Ծ �״�ġ �谡 ���� �ʴ´�.\n���� �� ����, ��Ȯ �۾��� ���� ������ ���� �ʿ��ϴ�.";
				break;
			case 4:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_FarmBlock";
				tCircleInfo.wstrText = L"�� ���";
				tCircleInfo.eArchitectureType = EArchitectureType::WHEAT_FARM;
				tCircleInfo.wstrDescriptionText = L"����(��) �⸦ �� �ִ� ���.\n��Ȯ ���� �ɸ��� �ð��� ���.\n���� �� ����, ��Ȯ �۾��� ���� ������ ���� �ʿ��ϴ�.";
				break;
			case 5:
				tCircleInfo.wstrTextureName = L"T_icon_buildObject_MonsterFarm";
				tCircleInfo.wstrText = L"�������";
				tCircleInfo.eArchitectureType = EArchitectureType::RANCH;
				tCircleInfo.wstrDescriptionText = L"���̳� �� ���� ���� ����ϴ� ����.\nƯ���� ��Ʈ�� ��ų�� ���� ����\n��ġ�ϸ� �������� �����Ѵ�.";
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
			// �ش� Ÿ�Կ� �´� ������ ���� ������ ã�ƿɴϴ�.
			std::optional<pair<std::shared_ptr<CMineral>, uint32>> prItem = CBuildManager::GetInstance()->GetArchitectureNeededMinerals(_eArchitectureType, i);
			// ������ Ÿ��
			EItemType eItemType = m_wpUIManager.lock()->ChangeMineralType(prItem.value().first->GetMineralType());
			m_vecItemBoxIndex[i].spItemImageComponent->SetTextureFileName(m_wpUIManager.lock()->FindItemUITextureName(eItemType));
			// ������ �̸�
			m_vecItemBoxIndex[i].spItemTitleTextComponent->SetText(m_wpUIManager.lock()->FindItemName(eItemType));
			// ����
			m_vecItemBoxIndex[i].spItemMaxCountTextComponent->SetText(to_wstring(prItem.value().second));

			// �κ��丮�� �ִ� ������ ������ �ؽ�Ʈ�� ������Ʈ ���ݴϴ�.
			int32 iCurNum = m_spInventorySystem->FindInventoryItemTotalNum(m_wpUIManager.lock()->ChangeMineralType(prItem.value().first->GetMineralType()));
			m_vecItemBoxIndex[i].spItemCurCountTextComponent->SetText(to_wstring(iCurNum));
			// �κ��丮 ������ �����Ǿ����� ������, �ƴϸ� ���������� �ٲ��ݴϴ�.
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
		wstrTextName = L"��ȭ";
		break;
	case Client::EMineralType::STONE:
		wstrImageName = L"T_itemicon_Material_Stone";
		wstrTextName = L"��";
		break;
	case Client::EMineralType::WOOD:
		wstrImageName = L"T_itemicon_Material_Wood";
		wstrTextName = L"����";
		break;
	case Client::EMineralType::LEATHER:
		wstrImageName = L"T_itemicon_Material_Leather";
		wstrTextName = L"����";
		break;
	case Client::EMineralType::COAL:
		wstrImageName = L"T_itemicon_Material_Coal";
		wstrTextName = L"��ź";
		break;
	case Client::EMineralType::WOOL:
		wstrImageName = L"T_itemicon_Material_Wool";
		wstrTextName = L"����";
		break;
	case Client::EMineralType::IRON_INGOT:
		wstrImageName = L"T_itemicon_Material_CopperIngot";
		wstrTextName = L"���� �ֱ�";
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
