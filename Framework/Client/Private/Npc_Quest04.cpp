#include "pch.h"
#include "Npc_Quest04.h"

#include "QuestSystem.h"

#include "GameManager.h"
#include "InventorySystem.h"
#include "Player.h"

shared_ptr<CNpc_Quest04> Client::CNpc_Quest04::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CNpc_Quest04> spInstance = make_shared<CNpc_Quest04>();
	CHECKF(spInstance, L"CNpc_Quest04 : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CNpc_Quest04::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CNpc::Initialize(_spGameObjectDesc);

	ChangeDefaultAnimation(ENPCANIM::AS_NPC_Female_UseDevice);

	return S_OK;
}

HRESULT Client::CNpc_Quest04::BeginPlay()
{
	CNpc::BeginPlay();

	return S_OK;
}

int32 Client::CNpc_Quest04::PreTick(float _fDeltaSeconds)
{
	CNpc::PreTick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest04::Tick(float _fDeltaSeconds)
{
	CNpc::Tick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest04::PostTick(float _fDeltaSeconds)
{
	CNpc::PostTick(_fDeltaSeconds);

	return S_OK;
}

HRESULT Client::CNpc_Quest04::EndPlay()
{
	CNpc::EndPlay();

	return S_OK;
}

void Client::CNpc_Quest04::Release()
{
	CNpc::Release();
}

void Client::CNpc_Quest04::InitCycle()
{
	// 퀘스트 추가
	{
		FQuestGroup tQuest;

		tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_CUSTOM;
		tQuest.m_arrQuest[0].m_wstrQuestText = L"전설의 아이스 도치를 데려오자";

		m_wpQuestSystem.lock()->AddQuest(L"Quest_004", tQuest);
	}

	// 퀘스트 시작
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(6);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"오, 못 보던 얼굴인데?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"요즘에 쥬신 밀렵단 놈들이 자꾸 우리 농장 찌릿도치들을 내놓으라고 해서\r\n적자가 이만저만이 아니라 손해를 계산중이였어.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"손해를 메우려면 전설의 아이스 도치라도 찾아야 할 판이야.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"아... 전설의 아이스 도치에 관심이 있구나. \r\n이 주변에서 매우 드물게 나타난다고 하는 특수한 초전도체 찌릿 도치가 있거든.";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"그 녀석만 잡는다면 이 손해를 메우고도 남을 거야. \r\n 만약 네가 찾아준다면 그에 맞는 보상을 줄게.";
			tCycle.m_vecNpcCycle[5].m_wstrText = L"행운을 빌지.";
		}

		// Anim
		{
			tCycle.m_vecNpcCycle[0].m_iAnimID = (int32)ENPCANIM::AS_Player_Female_Idle_None;
		}

		// Function
		{
			tCycle.m_vecNpcCycle[0].m_funcEvent = [this]() { ChangeDefaultAnimation(ENPCANIM::AS_Player_Female_Idle_None); };
			tCycle.m_vecNpcCycle[1].m_funcEvent = [this]() {NpcPlaySound("Player_Male_TryingSleep_Olian_EN_01_108660257"); };

			tCycle.m_vecNpcCycle[5].m_funcEvent = [this]()
				{
					if (m_wpQuestSystem.lock()->IsCurrentQuestEmpty())
					{
						m_wpQuestSystem.lock()->ActiveQuest(L"Quest_004");
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

		tCycle.m_vecNpcCycle.resize(8);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"??..............??";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"?!?@!!$?!!%?!?^!?&!?*?!>!$$#!!?@?";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"말도 안돼!!! 이건 진짜 전설의 찌릿도치잖아?";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"난 이제 부자다 부자!!!! 하하하하하하하하하하";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"아이쿠.. 이런 흠..흠\r\n그럼 약속대로 약속했던 보상을 줘야겠구나";
			tCycle.m_vecNpcCycle[5].m_wstrText = L"이건 제트래곤이라는 팰인데 이녀석만 있다면 어디든 자유롭게 모험 할 수 있을 거야";
			tCycle.m_vecNpcCycle[6].m_wstrText = L"흐흐흐 나는 이제 이 돈으로 뭘 할지 계산해봐야겠어..";
			tCycle.m_vecNpcCycle[7].m_wstrText = L"흐흐흐.....";
		}

		// Anim
		{
			tCycle.m_vecNpcCycle[1].m_iAnimID = (int32)ENPCANIM::AS_NPC_Female_Surprise;
			tCycle.m_vecNpcCycle[3].m_iAnimID = (int32)ENPCANIM::AS_Player_Female_Emote_Dance;
			tCycle.m_vecNpcCycle[4].m_iAnimID = (int32)ENPCANIM::AS_Player_Female_Idle_None;
			tCycle.m_vecNpcCycle[7].m_iAnimID = (int32)ENPCANIM::AS_NPC_Female_UseDevice;
		}

		// Function
		{
			tCycle.m_vecNpcCycle[0].m_funcEvent = [this]() {NpcPlaySound("Player_Male_Surprise_Olian_EN_01_827420737"); };
			tCycle.m_vecNpcCycle[3].m_funcEvent = [this]() 
				{
					ChangeDefaultAnimation(ENPCANIM::AS_Player_Female_Emote_Dance);
					NpcPlaySound("VolcanoBoss_Male_NormalSkill_Olian_EN_03_290188025"); 
				};
			tCycle.m_vecNpcCycle[4].m_funcEvent = [this]() { ChangeDefaultAnimation(ENPCANIM::AS_Player_Female_Idle_None); };
			tCycle.m_vecNpcCycle[5].m_funcEvent = [this]() 
				{
					CGameManager::GetInstance()->GetPlayer()->UnlockRiding(ERidingObjectType::JET_DRAGON); 
					AddExp();
				};
			tCycle.m_vecNpcCycle[7].m_funcEvent = [this]() { ChangeDefaultAnimation(ENPCANIM::AS_NPC_Female_UseDevice); ChangeLookToInitial(); };
		}

		// 사이클 등록
		m_umapCycle.emplace(L"Cycle_002", tCycle);
	}

	// 최초 사이클 등록
	m_wstrCurrentCycle = L"Cycle_001";
}