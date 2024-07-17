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
	//	// 볼링 게임을 시작합니다.
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
	// 퀘스트 추가
	{
		//FQuestGroup tQuest;

		//tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_PAL;
		//tQuest.m_arrQuest[0].m_eDstMonsterType = EMonsterType::PINK_CAT;
		//tQuest.m_arrQuest[0].m_wstrQuestText = L"희귀한 찌릿도치를 포획해오자";
		//tQuest.m_arrQuest[0].m_iDstObjectCount = 1;
	}

	// 사이클 등록
	{
		FNpcCycle tCycle;
		
		tCycle.m_vecNpcCycle.resize(4);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"어라, 못 보던 얼굴인데.";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"너 볼링 칠 줄 알아?";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"나랑 대결하지 않을래?";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"여긴 사람이 없어서 너무 심심하거든.\r\n너가 이기면 좋은 걸 줄게.";
		}

		m_umapCycle.emplace(L"Cycle_Start", tCycle);
	}

	// 승리
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(4);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"뭐야? 너 좀 치는구나?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"덕분에 오랜만에 즐거웠어.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"선물로 이 알록새 모자를 줄게.";
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

	// 승리 이후
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(2);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"알록새 모자 귀엽지?\r\n내가 직접 만든 거야.";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"다음에 또 놀러오도록 해.";
		}

		m_umapCycle.emplace(L"Cycle_SuccessAfter", tCycle);
	}

	// 실패
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(3);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"푸하하하하!";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"아직 날 따라잡으려면 멀었네.";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"더 연습하고 도전해보라고.";
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

	// 재도전
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(2);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"다시 도전하려고?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"실력 발휘 좀 해볼까?";
		}

		m_umapCycle.emplace(L"Cycle_ReStart", tCycle);
	}

	// 최초 사이클 등록
	m_wstrCurrentCycle = L"Cycle_Start";
}

void Client::CNpc_Quest03::Interact()
{
	// 다이얼로그가 꺼져있다면
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
		// 마지막 대사였다면 dialog를 종료한다
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
