#include "pch.h"
#include "Socket.h"

std::shared_ptr<CSocket> Engine::CSocket::Create(const std::shared_ptr<FSocketDesc>& _spSocketDesc)
{
    const std::shared_ptr<CSocket> spSocket = make_shared<CSocket>();
    ENSUREF(spSocket->Initialize(_spSocketDesc), L"Failed to initialize socket.");
    return spSocket;
}

HRESULT Engine::CSocket::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    // game object
    if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

    // socket
    shared_ptr<FSocketDesc> spSocketDesc = static_pointer_cast<FSocketDesc>(_spGameObjectDesc);
    m_strBoneName = spSocketDesc->m_strBoneName;

    return S_OK;
}

HRESULT Engine::CSocket::BeginPlay()
{
    if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

    return S_OK;
}

int32 Engine::CSocket::PreTick(float _fDeltaSeconds)
{
    switch (CGameObject::PreTick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return 0;
}

int32 Engine::CSocket::Tick(float _fDeltaSeconds)
{
    switch (CGameObject::Tick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return 0;
}

int32 Engine::CSocket::PostTick(float _fDeltaSeconds)
{
    switch (CGameObject::PostTick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return 0;
}

HRESULT Engine::CSocket::EndPlay()
{
    if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

    return S_OK;
}

void Engine::CSocket::Release()
{
    CGameObject::Release();
}
