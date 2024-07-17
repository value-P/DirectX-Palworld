#include "pch.h"
#include "Npc_Quest06.h"

#include "QuestSystem.h"

#include "GameManager.h"
#include "InventorySystem.h"


shared_ptr<CNpc_Quest06> Client::CNpc_Quest06::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CNpc_Quest06> spInstance = make_shared<CNpc_Quest06>();
	CHECKF(spInstance, L"CNpc_Quest06 : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CNpc_Quest06::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CNpc::Initialize(_spGameObjectDesc);

	ChangeDefaultAnimation(ENPCANIM::AS_NPC_Female_UseDevice);

	return S_OK;
}

HRESULT Client::CNpc_Quest06::BeginPlay()
{
	CNpc::BeginPlay();

	return S_OK;
}

int32 Client::CNpc_Quest06::PreTick(float _fDeltaSeconds)
{
	CNpc::PreTick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest06::Tick(float _fDeltaSeconds)
{
	CNpc::Tick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest06::PostTick(float _fDeltaSeconds)
{
	CNpc::PostTick(_fDeltaSeconds);

	return S_OK;
}

HRESULT Client::CNpc_Quest06::EndPlay()
{
	CNpc::EndPlay();

	return S_OK;
}

void Client::CNpc_Quest06::Release()
{
	CNpc::Release();
}

void Client::CNpc_Quest06::InitCycle()
{
	// 퀘스트 추가
	{
		FQuestGroup tQuest;

		tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_ITEM;
		tQuest.m_arrQuest[0].m_eDstItemType = EItemType::ASSAULT_RIFLE;
		tQuest.m_arrQuest[0].m_wstrQuestText = L"돌격소총을 제작하자";
		tQuest.m_arrQuest[0].m_iDstObjectCount = 1;

		tQuest.m_arrQuest[1].m_eQuestType = EQuestType::QUEST_ITEM;
		tQuest.m_arrQuest[1].m_eDstItemType = EItemType::Outfit_Platinum001_v02;
		tQuest.m_arrQuest[1].m_wstrQuestText = L"금속갑옷을 제작하자";
		tQuest.m_arrQuest[1].m_iDstObjectCount = 1;

		m_wpQuestSystem.lock()->AddQuest(L"Quest_006", tQuest);
	}

	// 퀘스트 시작
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(4);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"이봐, 너 쥬신 밀렵단에 도전하려고 하는 거야?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"그런 귀여운 장비로는 어림도 없어!";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"더 강한 장비를 착용하고 오도록 해.\r\n네가 알맞은 장비를 착용하고 온다면 이곳에 들어갈 수 있도록 도와줄게.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"더 강해져서 돌아오도록!";
		}

		// Anim
		{
			tCycle.m_vecNpcCycle[0].m_iAnimID = (int32)ENPCANIM::AS_Player_Female_Idle_None;
		}

		// Function
		{
			tCycle.m_vecNpcCycle[0].m_funcEvent = [this]() {
				ChangeDefaultAnimation(ENPCANIM::AS_Player_Female_Idle_None); 
				NpcPlaySound("ForestBoss_Female_NormalSkill_Heidi_EN_02_966114859");
				};
			tCycle.m_vecNpcCycle[3].m_funcEvent = [this]()
				{
					if (m_wpQuestSystem.lock()->IsCurrentQuestEmpty())
					{
						m_wpQuestSystem.lock()->ActiveQuest(L"Quest_006");
					}
					else if (m_wpQuestSystem.lock()->ClearCurrentQuest())
					{
						ChangeCycle(L"Cycle_002");
					}
				};
		}

		// 사이클 등록
		m_umapCycle.emplace(L"Cycle_001", tCycle);
	}

	// 퀘스트 클리어
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(4);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"음.. 이제 좀 볼만한걸";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"총알이 부족할테니 이거 받도록 해";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"문은 열어 뒀으니 알아서 하도록 해";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"가끔은 이런 목숨 아까운 줄 모르는 녀석이 있는것도 재밌는걸...";
		}

		// Anim
		{
			tCycle.m_vecNpcCycle[1].m_iAnimID = (int32)ENPCANIM::AS_NPC_Female_Give;
		}

		// Function
		{
			tCycle.m_vecNpcCycle[1].m_funcEvent = [this]() {
				m_wpInventorySystem.lock()->AddItemToContainer(EItemType::RIFLE_BULLET, 300); 
				AddExp();
				};
		}

		// 사이클 등록
		m_umapCycle.emplace(L"Cycle_002", tCycle);
	}

	// 최초 사이클 등록
	m_wstrCurrentCycle = L"Cycle_001";
}
