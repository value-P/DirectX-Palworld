#include "pch.h"
#include "Npc_Quest02.h"

#include "QuestSystem.h"

#include "GameManager.h"
#include "InventorySystem.h"
#include "Player.h"

shared_ptr<CNpc_Quest02> Client::CNpc_Quest02::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CNpc_Quest02> spInstance = make_shared<CNpc_Quest02>();
	CHECKF(spInstance, L"CNpc_Quest02 : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CNpc_Quest02::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CNpc::Initialize(_spGameObjectDesc);

	ChangeDefaultAnimation(ENPCANIM::AS_NPC_Female_Talking);

	return S_OK;
}

HRESULT Client::CNpc_Quest02::BeginPlay()
{
	CNpc::BeginPlay();

	return S_OK;
}

int32 Client::CNpc_Quest02::PreTick(float _fDeltaSeconds)
{
	CNpc::PreTick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest02::Tick(float _fDeltaSeconds)
{
	CNpc::Tick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest02::PostTick(float _fDeltaSeconds)
{
	CNpc::PostTick(_fDeltaSeconds);

	return S_OK;
}

HRESULT Client::CNpc_Quest02::EndPlay()
{
	CNpc::EndPlay();

	return S_OK;
}

void Client::CNpc_Quest02::Release()
{
	CNpc::Release();
}

void Client::CNpc_Quest02::InitCycle()
{
	// 퀘스트 추가
	{
		FQuestGroup tQuest;

		tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_PAL;
		tQuest.m_arrQuest[0].m_eDstMonsterType = EMonsterType::PINK_CAT;
		tQuest.m_arrQuest[0].m_wstrQuestText = L"까부냥을 1마리 포획하자";
		tQuest.m_arrQuest[0].m_iDstObjectCount = 1;

		tQuest.m_arrQuest[1].m_eQuestType = EQuestType::QUEST_PAL;
		tQuest.m_arrQuest[1].m_eDstMonsterType = EMonsterType::SHEEP_BALL;
		tQuest.m_arrQuest[1].m_wstrQuestText = L"도로롱을 1마리 포획하자";
		tQuest.m_arrQuest[1].m_iDstObjectCount = 1;

		m_wpQuestSystem.lock()->AddQuest(L"Quest_002", tQuest);
	}

	// 퀘스트 시작
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(5);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"음?... 무슨 얘기를 하고 있냐고?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"지금 도로롱과 까부냥 중 어떤 팰이 더 귀여운지 토론 중이었어.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"까부냥이 더 귀여운게 당연하잖아?\r\n그런데 저 녀석들은 계속 도로롱이 더 귀엽다고 한단 말이야.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"아무래도 직접 비교해서 승복시켜야겠어.\r\n너... 한가해 보이는데 까부냥과 도로롱을 잡아와주지 않을래?";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"각각 1마리씩만 잡아오면 될 거 같아.";
		}

		// Anim
		{
			tCycle.m_vecNpcCycle[0].m_iAnimID = (int32)ENPCANIM::AS_Player_Female_Idle_None;
		}
		
		// Function
		{
			tCycle.m_vecNpcCycle[0].m_funcEvent = [this]() { ChangeDefaultAnimation(ENPCANIM::AS_Player_Female_Idle_None); };
			tCycle.m_vecNpcCycle[2].m_funcEvent = [this]() {NpcPlaySound("Player_Female_TryingSleep_Marina_EN_01_188344366"); };
			tCycle.m_vecNpcCycle[4].m_funcEvent = [this]() 
				{
					if (m_wpQuestSystem.lock()->IsCurrentQuestEmpty())
					{
						m_wpQuestSystem.lock()->ActiveQuest(L"Quest_002");
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

		tCycle.m_vecNpcCycle.resize(6);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"잡아왔구나!\r\n..역시 까부냥이 더 귀엽잖아? 너도 그렇게 생각하지?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"?! 내 정신좀 봐... 고생했으니 보상을 줘야지.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"보니까 뛰어다니느라 힘들어 보이던데...\r\n그런 너에게 윈드디어를 줄게!";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"이 윈드디어가 있다면 어디든 빠르게 갈 수 있을 거야.\r\n이제는 다리 아프게 걸어다니지 말라구.";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"마침 화살도 조금 있어서 넣어놨으니 잘 사용하라구.";
			tCycle.m_vecNpcCycle[5].m_wstrText = L"나는 마저 저 사람들을 승복시켜야겠어.";
		}

		// Anim
		{
			tCycle.m_vecNpcCycle[1].m_iAnimID = (int32)ENPCANIM::AS_NPC_Female_Surprise;
			tCycle.m_vecNpcCycle[2].m_iAnimID = (int32)ENPCANIM::AS_NPC_Female_Give;
		}

		// Function
		{
			tCycle.m_vecNpcCycle[0].m_funcEvent = [this]() {NpcPlaySound("Player_Female_WaveHand_Marina_EN_03_993525373"); };

			tCycle.m_vecNpcCycle[2].m_funcEvent = [this]() 
				{ 
					CGameManager::GetInstance()->GetPlayer()->UnlockRiding(ERidingObjectType::WIND_DEER);
					m_wpInventorySystem.lock()->AddItemToContainer(EItemType::ARROW, 15);
					AddExp();
				};
			tCycle.m_vecNpcCycle[5].m_funcEvent = [this]() { ChangeDefaultAnimation(ENPCANIM::AS_NPC_Female_Talking); ChangeLookToInitial(); };
		}

		// 사이클 등록
		m_umapCycle.emplace(L"Cycle_002", tCycle);
	}

	// 최초 사이클 등록
	m_wstrCurrentCycle = L"Cycle_001";

}
