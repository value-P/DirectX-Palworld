#include "pch.h"
#include "QuestSystem.h"

#include "EngineModule.h"
#include "SoundManager.h"
#include "GameManager.h"
#include "BuildManager.h"
#include "PalBoxSystem.h"
#include "InventorySystem.h"
#include "Panel.h"
#include "UIManager.h"
#include "ImageComponent.h"
#include "TextComponent.h"

std::shared_ptr<CQuestSystem> Client::CQuestSystem::Create(const std::shared_ptr<FQuestSystemDesc>& _spQuestSystemDesc)
{
	shared_ptr<CQuestSystem> spInstance = make_shared<CQuestSystem>();
	CHECKF(spInstance, L"CQuestSystem : Create Failed");
	spInstance->Initialize(_spQuestSystemDesc);

	return spInstance;
}

HRESULT Client::CQuestSystem::Initialize(const std::shared_ptr<FQuestSystemDesc>& _spQuestSystemDesc)
{
	m_wpBuildManager = CBuildManager::GetInstance();
	m_wpInventorySystem = CGameManager::GetInstance()->GetInventorySystem();
	m_wpPalboxSystem = CGameManager::GetInstance()->GetPalBoxSystem();

	m_umapQuestSheet = _spQuestSystemDesc->m_umapQuestSheet;

	return S_OK;
}

HRESULT Client::CQuestSystem::BeginPlay()
{
	// 퀘스트 UI포인터
	m_wpQuestUI = CUIManager::GetInstance()->GetQuestInfo();
	m_wpQuestUI.lock()->spChildTutorialPanel->SetActive(false);

	return S_OK;
}

int32 Client::CQuestSystem::PreTick(float _fDeltaSeconds)
{
	return 0;
}

int32 Client::CQuestSystem::Tick(float _fDeltaSeconds)
{
	ComputeFinishedQuest();

	return 0;
}

int32 Client::CQuestSystem::PostTick(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Client::CQuestSystem::EndPlay()
{
	return S_OK;
}

void Client::CQuestSystem::Release()
{
}

void Client::CQuestSystem::AddQuest(const wstring& _wstrQuestName, const FQuestGroup& _tQuest)
{
	if (m_umapQuestSheet.contains(_wstrQuestName)) { return; }

	m_umapQuestSheet.emplace(_wstrQuestName, _tQuest);
}

bool Client::CQuestSystem::ActiveQuest(const wstring& _wstrQuestName)
{
	if (m_wstrCurrentQuest != L"") { return false; }

	if (!m_umapQuestSheet.contains(_wstrQuestName)) { return false; }

	m_wstrCurrentQuest = _wstrQuestName;
	FQuestGroup tQuestGroup = m_umapQuestSheet[_wstrQuestName];

	wstring wstrQuestTxt = tQuestGroup.m_arrQuest[0].m_wstrQuestText;
	if (wstrQuestTxt != L"")
	{
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent0->SetActive(true);
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent0->SetActive(true);
		m_wpQuestUI.lock()->spTutorialTextComponent0->SetText(wstrQuestTxt);
	}
	else
	{
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent0->SetActive(false);
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent0->SetActive(false);
	}

	wstrQuestTxt = tQuestGroup.m_arrQuest[1].m_wstrQuestText;
	if (wstrQuestTxt != L"")
	{
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent1->SetActive(true);
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent1->SetActive(true);
		m_wpQuestUI.lock()->spTutorialTextComponent1->SetText(wstrQuestTxt);
	}
	else
	{
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent1->SetActive(false);
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent1->SetActive(false);
	}

	wstrQuestTxt = tQuestGroup.m_arrQuest[2].m_wstrQuestText;
	if (wstrQuestTxt != L"")
	{
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent2->SetActive(true);
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent2->SetActive(true);
		m_wpQuestUI.lock()->spTutorialTextComponent2->SetText(wstrQuestTxt);
	}
	else
	{
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent2->SetActive(false);
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent2->SetActive(false);
	}

	m_wpQuestUI.lock()->spChildTutorialPanel->SetActive(true);

	return true;
}

bool Client::CQuestSystem::ClearCurrentQuest()
{
	if (IsCurrentQuestCleared())
	{	
		CEngineModule::GetInstance()->GetSoundManager()->PlaySoundEx("NewLoc_1_981746342");

		m_wpQuestUI.lock()->spTutorialCheckBoxComponent0->SetTextureFileName(L"T_prt_checkbox_off");
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent1->SetTextureFileName(L"T_prt_checkbox_off");
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent2->SetTextureFileName(L"T_prt_checkbox_off");

		m_wpQuestUI.lock()->spTutorialTextComponent0->SetText(L"");
		m_wpQuestUI.lock()->spTutorialTextComponent1->SetText(L"");
		m_wpQuestUI.lock()->spTutorialTextComponent2->SetText(L"");

		m_wpQuestUI.lock()->spChildTutorialPanel->SetActive(false);
		m_wstrCurrentQuest = L"";

		return true;
	}
	
	return false;
}

bool Client::CQuestSystem::IsCurrentQuestCleared()
{
	if (m_wstrCurrentQuest == L"") return false;

	for (auto tQuest : m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest)
	{
		if (tQuest.m_eQuestType != EQuestType::ENUM_END && !tQuest.m_bIsCleared)
		{
			return false; 
		}
	}

	return true;
}

void Client::CQuestSystem::SetCurrentQuestTrue(int32 _iQuestIndex)
{
	if (m_wstrCurrentQuest == L"") { return; }

	if (_iQuestIndex < 0 || _iQuestIndex > 2) { return; }

	if (m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[_iQuestIndex].m_eQuestType == EQuestType::ENUM_END) { return; }

	m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[_iQuestIndex].m_bIsCleared = true;

	switch (_iQuestIndex)
	{
	case 0:
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent0->SetTextureFileName(L"T_prt_checkbox_on");
		break;
	case 1:
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent1->SetTextureFileName(L"T_prt_checkbox_on");
		break;
	case 2:
		m_wpQuestUI.lock()->spTutorialCheckBoxComponent2->SetTextureFileName(L"T_prt_checkbox_on");
		break;
	default:
		break;
	}

}

void Client::CQuestSystem::ComputeFinishedQuest()
{
	if (m_wstrCurrentQuest == L"") { return; }

	for (size_t i = 0; i < m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest.size(); ++i)
	{
		wstring wstrText = L"";

		if (m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_bIsCleared) { continue; }

		switch (m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_eQuestType)
		{
		case EQuestType::QUEST_ITEM:
		{
			int32 iItemCount = m_wpInventorySystem.lock()->FindInventoryItemTotalNum(m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_eDstItemType);
			int32 iDstCount = m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_iDstObjectCount;

			iItemCount = min(iDstCount, iItemCount);

			wstrText = m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_wstrQuestText + L"(" + to_wstring(iItemCount) + L"/" + to_wstring(iDstCount) + L")";

			if (iItemCount >= iDstCount)
			{
				m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_bIsCleared = true;
			}
		}
			break;
		case EQuestType::QUEST_PAL:
		{
			int32 iPalCount = m_wpPalboxSystem.lock()->GetPalboxPalTotalNum(m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_eDstMonsterType);
			int32 iDstCount = m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_iDstObjectCount;

			wstrText = m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_wstrQuestText + L"(" + to_wstring(iPalCount) + L"/" + to_wstring(iDstCount) + L")";

			if (iPalCount >= iDstCount)
			{
				m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_bIsCleared = true;
			}
		}
			break;
		case EQuestType::QUEST_BUILDING:
		{
			wstrText = m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_wstrQuestText;

			if (m_wpBuildManager.lock()->IsArchitectureConstructed(m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_eDstBuildingType))
			{
				m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_bIsCleared = true;
			}
		}
			break;
		default:
			continue;
		}

		switch (i)
		{
		case 0:
			m_wpQuestUI.lock()->spTutorialTextComponent0->SetText(wstrText);
			break;
		case 1:
			m_wpQuestUI.lock()->spTutorialTextComponent1->SetText(wstrText);
			break;
		case 2:
			m_wpQuestUI.lock()->spTutorialTextComponent2->SetText(wstrText);
			break;
		default:
			break;
		}

		if (m_umapQuestSheet[m_wstrCurrentQuest].m_arrQuest[i].m_bIsCleared)
		{
			switch (i)
			{
			case 0:
				m_wpQuestUI.lock()->spTutorialCheckBoxComponent0->SetTextureFileName(L"T_prt_checkbox_on");
				break;
			case 1:
				m_wpQuestUI.lock()->spTutorialCheckBoxComponent1->SetTextureFileName(L"T_prt_checkbox_on");
				break;
			case 2:
				m_wpQuestUI.lock()->spTutorialCheckBoxComponent2->SetTextureFileName(L"T_prt_checkbox_on");
				break;
			default:
				break;
			}
		}
	}
}
