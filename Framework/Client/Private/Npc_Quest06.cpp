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
	// ����Ʈ �߰�
	{
		FQuestGroup tQuest;

		tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_ITEM;
		tQuest.m_arrQuest[0].m_eDstItemType = EItemType::ASSAULT_RIFLE;
		tQuest.m_arrQuest[0].m_wstrQuestText = L"���ݼ����� ��������";
		tQuest.m_arrQuest[0].m_iDstObjectCount = 1;

		tQuest.m_arrQuest[1].m_eQuestType = EQuestType::QUEST_ITEM;
		tQuest.m_arrQuest[1].m_eDstItemType = EItemType::Outfit_Platinum001_v02;
		tQuest.m_arrQuest[1].m_wstrQuestText = L"�ݼӰ����� ��������";
		tQuest.m_arrQuest[1].m_iDstObjectCount = 1;

		m_wpQuestSystem.lock()->AddQuest(L"Quest_006", tQuest);
	}

	// ����Ʈ ����
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(4);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"�̺�, �� ��� �зƴܿ� �����Ϸ��� �ϴ� �ž�?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"�׷� �Ϳ��� ���δ� ��� ����!";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"�� ���� ��� �����ϰ� ������ ��.\r\n�װ� �˸��� ��� �����ϰ� �´ٸ� �̰��� �� �� �ֵ��� �����ٰ�.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"�� �������� ���ƿ�����!";
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

		// ����Ŭ ���
		m_umapCycle.emplace(L"Cycle_001", tCycle);
	}

	// ����Ʈ Ŭ����
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(4);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"��.. ���� �� �����Ѱ�";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"�Ѿ��� �������״� �̰� �޵��� ��";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"���� ���� ������ �˾Ƽ� �ϵ��� ��";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"������ �̷� ��� �Ʊ�� �� �𸣴� �༮�� �ִ°͵� ��մ°�...";
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

		// ����Ŭ ���
		m_umapCycle.emplace(L"Cycle_002", tCycle);
	}

	// ���� ����Ŭ ���
	m_wstrCurrentCycle = L"Cycle_001";
}
