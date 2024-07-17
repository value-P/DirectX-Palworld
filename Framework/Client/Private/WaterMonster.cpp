#include "pch.h"
#include "WaterMonster.h"

std::shared_ptr<CWaterMonster> Client::CWaterMonster::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    shared_ptr<CWaterMonster> spInstance = make_shared<CWaterMonster>();
    CHECKF(spInstance, L"CWaterMonster : Create Failed");
    spInstance->Initialize(_spGameObjectDesc);

    return spInstance;
}

HRESULT Client::CWaterMonster::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    CMonster::Initialize(_spGameObjectDesc);

    return S_OK;
}

HRESULT Client::CWaterMonster::BeginPlay()
{
    CMonster::BeginPlay();
    return S_OK;
}

int32 Client::CWaterMonster::PreTick(float _fDeltaSeconds)
{
    CMonster::PreTick(_fDeltaSeconds);
    return 0;
}

int32 Client::CWaterMonster::Tick(float _fDeltaSeconds)
{
    CMonster::Tick(_fDeltaSeconds);
    return 0;
}

int32 Client::CWaterMonster::PostTick(float _fDeltaSeconds)
{
    CMonster::PostTick(_fDeltaSeconds);
    return 0;
}

HRESULT Client::CWaterMonster::EndPlay()
{
    CMonster::EndPlay();
    return S_OK;
}

void Client::CWaterMonster::Release()
{
    CMonster::Release();
}
