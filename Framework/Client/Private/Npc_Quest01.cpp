#include "pch.h"
#include "Npc_Quest01.h"

#include "QuestSystem.h"

#include "GameManager.h"
#include "InventorySystem.h"


shared_ptr<CNpc_Quest01> Client::CNpc_Quest01::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CNpc_Quest01> spInstance = make_shared<CNpc_Quest01>();
	CHECKF(spInstance, L"CNpc_Quest01 : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CNpc_Quest01::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CNpc::Initialize(_spGameObjectDesc);

	return S_OK;
}

HRESULT Client::CNpc_Quest01::BeginPlay()
{
	CNpc::BeginPlay();

	return S_OK;
}

int32 Client::CNpc_Quest01::PreTick(float _fDeltaSeconds)
{
	CNpc::PreTick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest01::Tick(float _fDeltaSeconds)
{
	CNpc::Tick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest01::PostTick(float _fDeltaSeconds)
{
	CNpc::PostTick(_fDeltaSeconds);

	return S_OK;
}

HRESULT Client::CNpc_Quest01::EndPlay()
{
	CNpc::EndPlay();

	return S_OK;
}

void Client::CNpc_Quest01::Release()
{
	CNpc::Release();
}

void Client::CNpc_Quest01::InitCycle()
{
	// 퀘스트 추가
	{
		FQuestGroup tQuest;

		tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_BUILDING;
		tQuest.m_arrQuest[0].m_eDstBuildingType = EArchitectureType::WORKBENCH;
		tQuest.m_arrQuest[0].m_wstrQuestText = L"재료를 모아 제작대를 설치하자";

		tQuest.m_arrQuest[1].m_eQuestType = EQuestType::QUEST_BUILDING;
		tQuest.m_arrQuest[1].m_eDstBuildingType = EArchitectureType::PAL_BOX;
		tQuest.m_arrQuest[1].m_wstrQuestText = L"재료를 모아 팰박스를 설치하자";

		m_wpQuestSystem.lock()->AddQuest(L"TraderQuest_001",tQuest);
	}

	// 퀘스트 내주기
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(7);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"...조난자인가?\r\n팰 냄새가 나지 않는 인간은 오랜만인데...";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"이 섬은 지옥이야... 나랑 함께 왔던 녀석들은 이미 이 세상에 없어.\r\n...사나운 팰에게 모조리 당해 버렸거든.\r\n너도 방심하지 마.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"최소한의 물자 정도는 나눠주겠지만...\r\n살아남으려면 네 힘을 스스로 키워야 해.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"아니, 너... 고대 단말기를 가지고 있는 거야!?\r\n그게 있으면 팰 스피어를 만들 수 있다고.\r\n팰 스피어가 있으면 팰을 포획할 수 있지.";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"네게서 팰 조련사의 자질이 엿보이는걸.\r\n팰 스피어를 제작하고 팰을 포획해 함께 싸워 봐.\r\n팰 조련사는 팰을 잡을수록 레벨이 올라가거든...";
			tCycle.m_vecNpcCycle[5].m_wstrText = L"힘을 키우기 전까지 저 탑엔 얼씬도 안 하는 게 좋아.\r\n...그러기 위해선 기본적인 도구들을 만들어야 할 거야.";
			tCycle.m_vecNpcCycle[6].m_wstrText = L"제작대와 팰박스를 만들어 온다면 좋은 걸 주도록 하지.";
		}

		// Anim
		{
			tCycle.m_vecNpcCycle[3].m_iAnimID = (int32)ENPCANIM::AS_NPC_Female_Surprise;
		}

		// Function
		{
			tCycle.m_vecNpcCycle[1].m_funcEvent = [this]() {NpcPlaySound("VillagerNPC_Male_Handover_Kitamura_EN_01_568378163"); };
			tCycle.m_vecNpcCycle[6].m_funcEvent = [this]()
				{
					if(m_wpQuestSystem.lock()->IsCurrentQuestEmpty()) 
					{
						m_wpQuestSystem.lock()->ActiveQuest(L"TraderQuest_001"); 
					}
					else if (m_wpQuestSystem.lock()->ClearCurrentQuest())
					{
						ChangeCycle(L"Trader_Cycle_002"); 
					}
				};
		}

		// 사이클 등록
		m_umapCycle.emplace(L"Trader_Cycle_001", tCycle);
	}

	// 퀘스트 달성
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(4);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"음?...전부 만들어왔구나!";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"그냥 애송인줄 알았는데 생각보다 제법이구나";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"팰 조련사는 많이 잡을수록 성장한다.\r\n이걸 줄 테니, 열심히 해 봐.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"네가 이 가혹한 환경에서\r\n살아남기를 바랄게.";
		}

		// Anim
		{
			tCycle.m_vecNpcCycle[0].m_iAnimID = (int32)ENPCANIM::AS_NPC_Female_Surprise;
			tCycle.m_vecNpcCycle[2].m_iAnimID = (int32)ENPCANIM::AS_NPC_Female_Give;
		}

		// Function
		{
			tCycle.m_vecNpcCycle[0].m_funcEvent = [this]() {NpcPlaySound("TraderNPC_Male_Conversation_Kitamura_EN_01_388380978"); };
			tCycle.m_vecNpcCycle[2].m_funcEvent = [this]() 
				{
					NpcPlaySound("TraderNPC_Male_Handover_Kitamura_EN_02_897434120");
					m_wpInventorySystem.lock()->AddItemToContainer(EItemType::BLUE_SPHERE, 10); 
					m_wpInventorySystem.lock()->AddItemToContainer(EItemType::WOOD, 15);
					m_wpInventorySystem.lock()->AddItemToContainer(EItemType::STONE, 15);
					m_wpInventorySystem.lock()->AddItemToContainer(EItemType::PALDIUM_FRAGMENT, 15);
					AddExp();
				};
		}

		// 사이클 등록
		m_umapCycle.emplace(L"Trader_Cycle_002", tCycle);
	}

	// 최초 사이클 등록
	m_wstrCurrentCycle = L"Trader_Cycle_001";
}
