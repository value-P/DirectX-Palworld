#include "pch.h"
#include "Npc_Quest05.h"

#include "QuestSystem.h"

#include "GameManager.h"
#include "InventorySystem.h"
#include "Weapon.h"
#include "MeshComponent.h"

shared_ptr<CNpc_Quest05> Client::CNpc_Quest05::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CNpc_Quest05> spInstance = make_shared<CNpc_Quest05>();
	CHECKF(spInstance, L"CNpc_Quest05 : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CNpc_Quest05::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CNpc::Initialize(_spGameObjectDesc);

	ChangeDefaultAnimation(ENPCANIM::AS_Player_Female_Deforest_None);

	// 도끼
	{
		// component desc
		FWeaponDesc tMeleeDesc;
		tMeleeDesc.m_wstrGameObjectName = L"PlayerWeapon_Axe";
		tMeleeDesc.m_bIsAnimed = false;
		tMeleeDesc.m_eWeaponActionType = EWeaponActionType::MELEE;
		tMeleeDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tMeleeDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeleeDesc.m_eWeaponType = EWeaponType::STONE_AXE;
		tMeleeDesc.m_eCurItemType = EItemType::STONE_AXE;

		// mesh component desc
		tMeleeDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/HarvestTool_002.dat";
		tMeleeDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tMeleeDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		m_spAxe = CWeapon::Create(make_shared<FWeaponDesc>(tMeleeDesc));
		AddChildGameObject(m_spAxe);
	}

	return S_OK;
}

HRESULT Client::CNpc_Quest05::BeginPlay()
{
	CNpc::BeginPlay();

	return S_OK;
}

int32 Client::CNpc_Quest05::PreTick(float _fDeltaSeconds)
{
	CNpc::PreTick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest05::Tick(float _fDeltaSeconds)
{
	CNpc::Tick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNpc_Quest05::PostTick(float _fDeltaSeconds)
{
	CNpc::PostTick(_fDeltaSeconds);

	return S_OK;
}

HRESULT Client::CNpc_Quest05::EndPlay()
{
	CNpc::EndPlay();

	return S_OK;
}

void Client::CNpc_Quest05::Release()
{
	SharedNull(m_spAxe);

	CNpc::Release();
}

void Client::CNpc_Quest05::InitCycle()
{
	// 퀘스트 추가
	{
		FQuestGroup tQuest;

		tQuest.m_arrQuest[0].m_eQuestType = EQuestType::QUEST_ITEM;
		tQuest.m_arrQuest[0].m_eDstItemType = EItemType::GRASSMAMMOTH_MEAT;
		tQuest.m_arrQuest[0].m_wstrQuestText = L"그린모스 고기를 가져다 주자";
		tQuest.m_arrQuest[0].m_iDstObjectCount = 1;

		m_wpQuestSystem.lock()->AddQuest(L"Quest_005", tQuest);
	}

	// 퀘스트 시작
	{
		FNpcCycle tCycle;

		tCycle.m_vecNpcCycle.resize(6);

		// Text
		{
			tCycle.m_vecNpcCycle[0].m_wstrText = L"뭐? 산림 훼손을 그만하라고?";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"그치만 이렇게라도 돈을 벌어야 병든 아내를 살릴 수 있는걸...";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"의사가 병이 나으려면 그린모스의 고기를 요리해서 먹여야 한대.\r\n하지만...";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"쥬신 밀렵단 녀석들이 그린모스 고기를 독점해서 유통하는데\r\n 가격을 너무 비싸게 불러서 도저히 마련할 수가 없어...";
			tCycle.m_vecNpcCycle[4].m_wstrText = L"이봐, 너... 꽤 강해보이는데 혹시 그린모스의 고기를 구해줄 수 있겠어? \r\n너에게 줄 수 있는 건 많이 없지만 제발 부탁할게.";
			tCycle.m_vecNpcCycle[5].m_wstrText = L"정말 고마워.";
		}

		// Anim
		{
			tCycle.m_vecNpcCycle[0].m_iAnimID = (int32)ENPCANIM::AS_Player_Female_Idle_None;
		}

		// Function
		{
			tCycle.m_vecNpcCycle[0].m_funcEvent = [this]() {
				ChangeDefaultAnimation(ENPCANIM::AS_Player_Female_Idle_None);
				m_spAxe->SetActive(false); 
				NpcPlaySound("Player_Male_Surprise_Graham_EN_01_320769937");
				};
			tCycle.m_vecNpcCycle[2].m_funcEvent = [this]() { NpcPlaySound("SnowyMountainBoss_Male_Idle_Kevin_EN_03_170357949"); };

			tCycle.m_vecNpcCycle[5].m_funcEvent = [this]()
				{
					if (m_wpQuestSystem.lock()->IsCurrentQuestEmpty())
					{
						m_wpQuestSystem.lock()->ActiveQuest(L"Quest_005");
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
			tCycle.m_vecNpcCycle[0].m_wstrText = L"고기를 가져왔구나! 정말 고마워";
			tCycle.m_vecNpcCycle[1].m_wstrText = L"덕분에 아내의 병을 고칠 수 있을것 같아";
			tCycle.m_vecNpcCycle[2].m_wstrText = L"가진게 이것밖에 없지만 이거라도 받아줘";
			tCycle.m_vecNpcCycle[3].m_wstrText = L"이 은혜는 꼭 잊지 않을게....";
		}

		// Anim
		{
			tCycle.m_vecNpcCycle[2].m_iAnimID = (int32)ENPCANIM::AS_NPC_Female_Give;
		}

		// Function
		{
			tCycle.m_vecNpcCycle[1].m_funcEvent = [this]() { NpcPlaySound("Player_Male_WaveHand_Graham_EN_02_989420648"); };
			tCycle.m_vecNpcCycle[2].m_funcEvent = [this]() {
				m_wpInventorySystem.lock()->AddItemToContainer(EItemType::IRON_INGOT, 30); 
				AddExp(); 
				};
			
		}

		// 사이클 등록
		m_umapCycle.emplace(L"Cycle_002", tCycle);
	}

	// 최초 사이클 등록
	m_wstrCurrentCycle = L"Cycle_001";
}
