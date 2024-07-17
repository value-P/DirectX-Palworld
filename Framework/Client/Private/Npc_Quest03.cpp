#include "pch.h"
#include "Npc_Quest03.h"

#include "QuestSystem.h"

#include "GameManager.h"
#include "InventorySystem.h"
#include "BowlingSystem.h"

shared_ptr<CNpc_Quest03> Client::CNpc_Quest03::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CNpc_Quest03> spInstance = make_shared<CNpc_Quest03>();
	CHECKF(spInstance, L"CNpc_Quest03 : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CNpc_Quest03::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CNpc::Initialize(_spGameObjectDesc);

	m_wpBowlingSystem = CGameManager::GetInstance()->GetBowlingSystem();

	return S_OK;
}

HRESULT Client::CNpc_Quest03::BeginPlay()
{
	CNpc::BeginPlay();

	return S_OK;
}

int32 Client::CNpc_Quest03::PreTick(float _fDeltaSeconds)
{
	CNpc::PreTick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest03::Tick(float _fDeltaSeconds)
{
	CNpc::Tick(_fDeltaSeconds);

	//if (!m_bStartEvent && m_umapCycle[L"Cycle_Start"].IsLast())
	//{
	//	// ���� ������ �����մϴ�.
	//	m_wpBowlingSystem.lock()->SetGameStart();
	//	m_bStartEvent = true;
	//}

	return S_OK;
}

int32 Client::CNpc_Quest03::PostTick(float _fDeltaSeconds)
{
	CNpc::PostTick(_fDeltaSeconds);

	return S_OK;
}

HRESULT Client::CNpc_Quest03::EndPlay()
{
	CNpc::EndPlay();

	return S_OK;
}

void Client::CNpc_Quest03::Release()
{
	CNpc::Release();
}

void Client::CNpc_Quest03::InitCycle()
{
	// ����Ʈ �߰�
	{
		//FQuestGroup tQuest;

		//tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_PAL;
		//tQuest.m_arrQuest[0].m_eDstMonsterType = EMonsterType::PINK_CAT;
		//tQuest.m_arrQuest[0].m_wstrQuestText = L"����� ���ġ�� ��ȹ�ؿ���";
		//tQuest.m_arrQuest[0].m_iDstObjectCount = 1;
	}

	// ����Ŭ ���
	{
		FNpcCycle tCycle;
		
		tCycle.m_vecNpcCycle.resize(4);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"���, �� ���� ���ε�.";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"�� ���� ĥ �� �˾�?";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"���� ������� ������?";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"���� ����� ��� �ʹ� �ɽ��ϰŵ�.\r\n�ʰ� �̱�� ���� �� �ٰ�.";
		}

		m_umapCycle.emplace(L"Cycle_Start", tCycle);
	}

	// �¸�
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(4);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"����? �� �� ġ�±���?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"���п� �������� ��ſ���.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"������ �� �˷ϻ� ���ڸ� �ٰ�.";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"";
		}

		// Function
		{
			tCycle.m_vecNpcCycle[3].m_funcEvent = [this]()
				{
					m_wpInventorySystem.lock()->AddItemToContainer(EItemType::HeadEquip019, 1);
					ChangeCycle(L"Cycle_SuccessAfter");
				};
		}

		m_umapCycle.emplace(L"Cycle_Success", tCycle);
	}

	// �¸� ����
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(2);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"�˷ϻ� ���� �Ϳ���?\r\n���� ���� ���� �ž�.";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"������ �� ������� ��.";
		}

		m_umapCycle.emplace(L"Cycle_SuccessAfter", tCycle);
	}

	// ����
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(3);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"Ǫ��������!";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"���� �� ������������ �־���.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"�� �����ϰ� �����غ����.";
		}

		// Function
		{
			tCycle.m_vecNpcCycle[2].m_funcEvent = [this]()
				{
					ChangeCycle(L"Cycle_ReStart");
				};
		}

		m_umapCycle.emplace(L"Cycle_Fail", tCycle);
	}

	// �絵��
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(2);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"�ٽ� �����Ϸ���?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"�Ƿ� ���� �� �غ���?";
		}

		m_umapCycle.emplace(L"Cycle_ReStart", tCycle);
	}

	// ���� ����Ŭ ���
	m_wstrCurrentCycle = L"Cycle_Start";
}

void Client::CNpc_Quest03::Interact()
{
	// ���̾�αװ� �����ִٸ�
	if (!m_bDialogActivate)
	{
		PlayDialog();

		PlayText();

		if (!m_bInteracted)
		{
			m_vDstLook = m_wpInteractObject.lock()->GetPosition().value() - GetPosition().value();
			m_vDstLook.y = 0.f;
			m_vDstLook.Normalize();
			m_bInteracted = true;
		}
	}
	else
	{
		// ������ ��翴�ٸ� dialog�� �����Ѵ�
		if (m_umapCycle[m_wstrCurrentCycle].IsLast())
		{
			EndDialog();
			std::wstring wstrStartCycle = L"Cycle_Start";
			std::wstring wstrReStartCycle = L"Cycle_ReStart";
			if (m_umapCycle[wstrStartCycle].IsLast() || m_umapCycle[wstrReStartCycle].IsLast())
			{
				m_wpBowlingSystem.lock()->SetGameStart();
			}

			m_umapCycle[m_wstrCurrentCycle].m_bIsLast = false;
		}
		else
		{
			PlayText();
		}
	}
}
