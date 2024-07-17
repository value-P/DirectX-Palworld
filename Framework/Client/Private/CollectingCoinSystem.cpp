#include "pch.h"
#include "CollectingCoinSystem.h"

// world
#include "World.h"
#include "IslandLevel.h"

// manager
#include "EngineModule.h"
#include "TimerManager.h"
#include "UIManager.h"

// gameobject
#include "Panel.h"
#include "GoldCoin.h"
#include "TreasureBox.h"

//test
#include "Timer.h"
#include "InputManager.h"

std::shared_ptr<CCollectingCoinSystem> Client::CCollectingCoinSystem::Create(const std::shared_ptr<FCollectingCoinSystemDesc >& _spCollectingCoinSystemDesc)
{
    std::shared_ptr<CCollectingCoinSystem> spCollectingCoinSystem = make_shared<CCollectingCoinSystem>();
    CHECKF(spCollectingCoinSystem, L"CCollectingCoinSystem : Create Failed");
    spCollectingCoinSystem->Initialize(_spCollectingCoinSystemDesc);
    return spCollectingCoinSystem;
}

HRESULT Client::CCollectingCoinSystem::Initialize(const std::shared_ptr<FCollectingCoinSystemDesc >& _spCollectingCoinSystemDesc)
{
    // timer�Ŵ������� Ű���� ���� ��ü�� ����մϴ�.
    m_wpTimerManager = CEngineModule::GetInstance()->GetTimerManager();
    m_wpUIManager = CUIManager::GetInstance();
    // ui manager���� load�� json������ �����͸� �ֽ��ϴ�.
    m_spParentPanel = m_wpUIManager.lock()->FindParentsPanel(L"CollectingCoinPanel");
    m_spParentPanel->SetActive(false);

    // create gold coin
    {
        FGoldCoinDesc tGoldCoinDesc;
        tGoldCoinDesc.m_bActive = true;
        tGoldCoinDesc.m_wstrEffectGroupName = L"Group_Minigame_Coin";

        m_vecGoldCoins.reserve(5);
        m_vecGoldCoins.resize(5);
        for (uint32 innerIndex = 0; innerIndex < 5; ++innerIndex)
        {
            tGoldCoinDesc.m_vInitPosition = Vector3(-3562.0f + 2.0f * innerIndex, 72.0f, -2715.0f);
            tGoldCoinDesc.m_vInitScale    = Vector3(0.1f, 0.1f, 0.1f);
            tGoldCoinDesc.m_vInitRotate   = Vector3(90.0f, 0.0f, 0.0f);

            // JSON���Ϸκ��� ���� ��ü�� �����մϴ�.
            shared_ptr<CGoldCoin> spGoldCoin = static_pointer_cast<CGoldCoin>(CGoldCoin::CreateFromJson(make_shared<FGoldCoinDesc>(tGoldCoinDesc), tGoldCoinDesc.m_wstrEffectGroupName));
            CHECKF(spGoldCoin, L"Failed to CreateFromJson : CCollectingCoinSystem");

            // ���Ϳ� ������ �����մϴ�.
            m_vecGoldCoins[innerIndex] = spGoldCoin;
            CWorld::GetInstance()->GetPersistentLevel()->AddGameObject(spGoldCoin);

            // timer�� �߰����ݴϴ�.
            FTimerDesc tTimerDesc = {};
            tTimerDesc.m_bLoop = false;
            tTimerDesc.m_fTime = 10.0f;
            tTimerDesc.m_spListener = m_vecGoldCoins[innerIndex];
            m_spTimer = CTimer::Create(std::make_shared<FTimerDesc>(tTimerDesc));
            m_wpTimerManager.lock()->AddTimer(L"Collecting Coin Timer"+ to_wstring(innerIndex), m_spTimer);
        }
    }

    // create treasure box
    {
        FTreasureBoxDesc tTreasureBoxDesc;
        tTreasureBoxDesc.m_wstrGameObjectName = L"TreasureBox";
        tTreasureBoxDesc.m_vInitPosition = Vector3(-3558.0f, 72.0f, -2715.0f);

        m_spTreasureBox = CTreasureBox::Create(make_shared<FTreasureBoxDesc>(tTreasureBoxDesc));
        CWorld::GetInstance()->GetPersistentLevel()->AddGameObject(m_spTreasureBox);
    }

    return S_OK;
}

HRESULT Client::CCollectingCoinSystem::BeginPlay()
{
    return S_OK;
}

int32 Client::CCollectingCoinSystem::PreTick(float _fDeltaSeconds)
{
    return 0;
}

int32 Client::CCollectingCoinSystem::Tick(float _fDeltaSeconds)
{
    TimeAttackTimer();

    // �׽�Ʈ �ڵ� �Դϴ�.
    /*
    if (CEngineModule::GetInstance()->GetInputManager()->IsKeyUp(DIK_M))
    {
        m_wpUIManager.lock()->GetHUDParentPanel()->SetActive(false);
        m_spParentPanel->SetActive(true);
    }
    */
    return 0;
}

int32 Client::CCollectingCoinSystem::PostTick(float _fDeltaSeconds)
{
    return 0;
}

HRESULT Client::CCollectingCoinSystem::EndPlay()
{
    return S_OK;
}
 
void Client::CCollectingCoinSystem::Release()
{
}

void Client::CCollectingCoinSystem::TimeAttackTimer()
{
    // ���⼭ start time, reset time ����� ���ش�
    m_wpTimerManager.lock()->StartTimer(L"Collecting Coin Timer1");
}