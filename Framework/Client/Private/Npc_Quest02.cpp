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
	// ����Ʈ �߰�
	{
		FQuestGroup tQuest;

		tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_PAL;
		tQuest.m_arrQuest[0].m_eDstMonsterType = EMonsterType::PINK_CAT;
		tQuest.m_arrQuest[0].m_wstrQuestText = L"��γ��� 1���� ��ȹ����";
		tQuest.m_arrQuest[0].m_iDstObjectCount = 1;

		tQuest.m_arrQuest[1].m_eQuestType = EQuestType::QUEST_PAL;
		tQuest.m_arrQuest[1].m_eDstMonsterType = EMonsterType::SHEEP_BALL;
		tQuest.m_arrQuest[1].m_wstrQuestText = L"���η��� 1���� ��ȹ����";
		tQuest.m_arrQuest[1].m_iDstObjectCount = 1;

		m_wpQuestSystem.lock()->AddQuest(L"Quest_002", tQuest);
	}

	// ����Ʈ ����
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(5);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"��?... ���� ��⸦ �ϰ� �ֳİ�?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"���� ���ηհ� ��γ� �� � ���� �� �Ϳ����� ��� ���̾���.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"��γ��� �� �Ϳ���� �翬���ݾ�?\r\n�׷��� �� �༮���� ��� ���η��� �� �Ϳ��ٰ� �Ѵ� ���̾�.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"�ƹ����� ���� ���ؼ� �º����Ѿ߰ھ�.\r\n��... �Ѱ��� ���̴µ� ��γɰ� ���η��� ��ƿ����� ������?";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"���� 1�������� ��ƿ��� �� �� ����.";
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

		// ����Ŭ ���
		m_umapCycle.emplace(L"Cycle_001", tCycle);
	}

	// ����Ʈ Ŭ����
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(6);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"��ƿԱ���!\r\n..���� ��γ��� �� �Ϳ��ݾ�? �ʵ� �׷��� ��������?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"?! �� ������ ��... ��������� ������ �����.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"���ϱ� �پ�ٴϴ��� ����� ���̴���...\r\n�׷� �ʿ��� ����� �ٰ�!";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"�� ����� �ִٸ� ���� ������ �� �� ���� �ž�.\r\n������ �ٸ� ������ �ɾ�ٴ��� ����.";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"��ħ ȭ�쵵 ���� �־ �־������ �� ����϶�.";
			tCycle.m_vecNpcCycle[5].m_wstrText = L"���� ���� �� ������� �º����Ѿ߰ھ�.";
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

		// ����Ŭ ���
		m_umapCycle.emplace(L"Cycle_002", tCycle);
	}

	// ���� ����Ŭ ���
	m_wstrCurrentCycle = L"Cycle_001";

}
