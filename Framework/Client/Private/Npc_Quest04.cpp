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
	// ����Ʈ �߰�
	{
		FQuestGroup tQuest;

		tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_CUSTOM;
		tQuest.m_arrQuest[0].m_wstrQuestText = L"������ ���̽� ��ġ�� ��������";

		m_wpQuestSystem.lock()->AddQuest(L"Quest_004", tQuest);
	}

	// ����Ʈ ����
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(6);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"��, �� ���� ���ε�?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"���� ��� �зƴ� ����� �ڲ� �츮 ���� ���ġ���� ��������� �ؼ�\r\n���ڰ� �̸������� �ƴ϶� ���ظ� ������̿���.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"���ظ� �޿���� ������ ���̽� ��ġ�� ã�ƾ� �� ���̾�.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"��... ������ ���̽� ��ġ�� ������ �ֱ���. \r\n�� �ֺ����� �ſ� �幰�� ��Ÿ���ٰ� �ϴ� Ư���� ������ü � ��ġ�� �ְŵ�.";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"�� �༮�� ��´ٸ� �� ���ظ� �޿�� ���� �ž�. \r\n ���� �װ� ã���شٸ� �׿� �´� ������ �ٰ�.";
			tCycle.m_vecNpcCycle[5].m_wstrText = L"����� ����.";
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

		// ����Ŭ ���
		m_umapCycle.emplace(L"Cycle_001", tCycle);
	}

	// ����Ʈ Ŭ����
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(8);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"??..............??";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"?!?@!!$?!!%?!?^!?&!?*?!>!$$#!!?@?";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"���� �ȵ�!!! �̰� ��¥ ������ ���ġ�ݾ�?";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"�� ���� ���ڴ� ����!!!! ��������������������";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"������.. �̷� ��..��\r\n�׷� ��Ӵ�� ����ߴ� ������ ��߰ڱ���";
			tCycle.m_vecNpcCycle[5].m_wstrText = L"�̰� ��Ʈ�����̶�� ���ε� �̳༮�� �ִٸ� ���� �����Ӱ� ���� �� �� ���� �ž�";
			tCycle.m_vecNpcCycle[6].m_wstrText = L"������ ���� ���� �� ������ �� ���� ����غ��߰ھ�..";
			tCycle.m_vecNpcCycle[7].m_wstrText = L"������.....";
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

		// ����Ŭ ���
		m_umapCycle.emplace(L"Cycle_002", tCycle);
	}

	// ���� ����Ŭ ���
	m_wstrCurrentCycle = L"Cycle_001";
}