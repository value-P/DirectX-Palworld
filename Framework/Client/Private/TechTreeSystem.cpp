#include "pch.h"
#include "TechTreeSystem.h"

#include "Item.h"
#include "Architecture.h"

#include "GameManager.h"
#include "Player.h"

std::shared_ptr<CTechTreeSystem> Client::CTechTreeSystem::Create(const std::shared_ptr<FTechTreeSystemDesc>& _spTechTreeSystemDesc)
{
    shared_ptr<CTechTreeSystem> spInstance = make_shared<CTechTreeSystem>();
    CHECKF(spInstance, L"CTechTreeSystem : Create Failed");
    spInstance->Initialize(_spTechTreeSystemDesc);

    return spInstance;
}

HRESULT Client::CTechTreeSystem::Initialize(const std::shared_ptr<FTechTreeSystemDesc>& _spTechTreeSystemDesc)
{
    // 추후 GameManager에서 플레이어의 주소를 받아옵니다.
    // m_wpPlayer = CGameManager::GetInstance()->GetPlayer();

    //m_iCurPlayerLevel = m_wpPlayer.lock()->GetCurCreatureLevel();
    //m_iPrePlayerLevel = m_iCurPlayerLevel;

    return S_OK;
}

HRESULT Client::CTechTreeSystem::BeginPlay()
{
    return S_OK;
}

int32 Client::CTechTreeSystem::PreTick(float _fDeltaSeconds)
{
    //m_iCurPlayerLevel = m_wpPlayer.lock()->GetCurCreatureLevel();
    //if (m_iPrePlayerLevel == m_iCurPlayerLevel) { return 0; }
    //else { m_iPrePlayerLevel = m_iCurPlayerLevel; }    
   
    switch (m_iCurPlayerLevel)
    {
    case 1:
    {
        UnlockArchitecture(EArchitectureType::WORKBENCH);
        UnlockItem(EItemType::STONE_AXE);
        UnlockItem(EItemType::STONE_PICKAXE);
        UnlockItem(EItemType::HAND_HELD_TORCH);
    }
    break;

    case 2:
    {
        UnlockArchitecture(EArchitectureType::PAL_BOX);
        UnlockArchitecture(EArchitectureType::WOODEN_CHEST);
        UnlockItem(EItemType::BLUE_SPHERE);
        UnlockItem(EItemType::STONE_SPEAR);
    }
    break;

    case 3: break;
    case 4: break;
    case 5: break;
    case 6: break;
    case 7: break;
    case 8: break;
    case 9: break;
    case 10: break;
    case 11: break;
    case 12: break;
    case 13: break;
    case 14: break;
    case 15: break;
    }

    return 0;
}

int32 Client::CTechTreeSystem::Tick(float _fDeltaSeconds)
{
    return 0;
}

int32 Client::CTechTreeSystem::PostTick(float _fDeltaSeconds)
{
    return 0;
}

HRESULT Client::CTechTreeSystem::EndPlay()
{
    return S_OK;
}

void Client::CTechTreeSystem::Release()
{
    m_wpPlayer.reset();
}

void Client::CTechTreeSystem::UnlockItem(EItemType _eItemType)
{
}

void Client::CTechTreeSystem::UnlockArchitecture(EArchitectureType _eArchitectureType)
{
}