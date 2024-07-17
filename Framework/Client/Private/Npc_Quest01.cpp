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
	// ����Ʈ �߰�
	{
		FQuestGroup tQuest;

		tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_BUILDING;
		tQuest.m_arrQuest[0].m_eDstBuildingType = EArchitectureType::WORKBENCH;
		tQuest.m_arrQuest[0].m_wstrQuestText = L"��Ḧ ��� ���۴븦 ��ġ����";

		tQuest.m_arrQuest[1].m_eQuestType = EQuestType::QUEST_BUILDING;
		tQuest.m_arrQuest[1].m_eDstBuildingType = EArchitectureType::PAL_BOX;
		tQuest.m_arrQuest[1].m_wstrQuestText = L"��Ḧ ��� �ӹڽ��� ��ġ����";

		m_wpQuestSystem.lock()->AddQuest(L"TraderQuest_001",tQuest);
	}

	// ����Ʈ ���ֱ�
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(7);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"...�������ΰ�?\r\n�� ������ ���� �ʴ� �ΰ��� �������ε�...";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"�� ���� �����̾�... ���� �Բ� �Դ� �༮���� �̹� �� ���� ����.\r\n...�糪�� �ӿ��� ������ ���� ���Ȱŵ�.\r\n�ʵ� ������� ��.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"�ּ����� ���� ������ �����ְ�����...\r\n��Ƴ������� �� ���� ������ Ű���� ��.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"�ƴ�, ��... ��� �ܸ��⸦ ������ �ִ� �ž�!?\r\n�װ� ������ �� ���Ǿ ���� �� �ִٰ�.\r\n�� ���Ǿ ������ ���� ��ȹ�� �� ����.";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"�װԼ� �� ���û��� ������ �����̴°�.\r\n�� ���Ǿ �����ϰ� ���� ��ȹ�� �Բ� �ο� ��.\r\n�� ���û�� ���� �������� ������ �ö󰡰ŵ�...";
			tCycle.m_vecNpcCycle[5].m_wstrText = L"���� Ű��� ������ �� ž�� ����� �� �ϴ� �� ����.\r\n...�׷��� ���ؼ� �⺻���� �������� ������ �� �ž�.";
			tCycle.m_vecNpcCycle[6].m_wstrText = L"���۴�� �ӹڽ��� ����� �´ٸ� ���� �� �ֵ��� ����.";
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

		// ����Ŭ ���
		m_umapCycle.emplace(L"Trader_Cycle_001", tCycle);
	}

	// ����Ʈ �޼�
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(4);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"��?...���� �����Ա���!";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"�׳� �ּ����� �˾Ҵµ� �������� �����̱���";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"�� ���û�� ���� �������� �����Ѵ�.\r\n�̰� �� �״�, ������ �� ��.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"�װ� �� ��Ȥ�� ȯ�濡��\r\n��Ƴ��⸦ �ٶ���.";
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

		// ����Ŭ ���
		m_umapCycle.emplace(L"Trader_Cycle_002", tCycle);
	}

	// ���� ����Ŭ ���
	m_wstrCurrentCycle = L"Trader_Cycle_001";
}
