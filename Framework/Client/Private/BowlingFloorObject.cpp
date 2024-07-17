#include "pch.h"
#include "BowlingFloorObject.h"

#include "BoxComponent.h"

shared_ptr<CBowlingFloorObject> Client::CBowlingFloorObject::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    std::shared_ptr<CBowlingFloorObject> spInstance = make_shared<CBowlingFloorObject>();
    ENSUREF(spInstance->Initialize(_spGameObjectDesc), L"Failed to initialize CBowlingFloorObject.");
    return spInstance;
}

HRESULT Client::CBowlingFloorObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; };

    {
        FBoxComponentDesc tBoxComponentDesc;
        tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
        tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
        tBoxComponentDesc.m_wstrComponentName = L"BoxComponent";
        tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

        tBoxComponentDesc.m_bTrigger = false;
        tBoxComponentDesc.m_bGravity = false;
        tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
        tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
        tBoxComponentDesc.m_bDeferredAdd = true;

        tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BOWLING_FLOOR;
        tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BOWLING_FLOOR;

        tBoxComponentDesc.m_fMass = 0.7f;
        tBoxComponentDesc.m_fDensity = 1.0f;

        tBoxComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
        tBoxComponentDesc.m_vLocalPosition = Vector3(0.0f, 0.0f, 0.0f);
        tBoxComponentDesc.m_vMaterial = Vector3(0.06f, 0.06f, 0.0f);
        tBoxComponentDesc.m_vExtents = Vector3(13.0f, 0.5f, 11.0f);

        m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
        ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxComponent));
    }

    return S_OK;
}

HRESULT Client::CBowlingFloorObject::BeginPlay()
{
    if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; };

    return S_OK;
}

int32 Client::CBowlingFloorObject::Tick(float _fDeltaSeconds)
{
    CGameObject::Tick(_fDeltaSeconds);

    return 0;
}

HRESULT Client::CBowlingFloorObject::EndPlay()
{
    if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

    return S_OK;
}

void Client::CBowlingFloorObject::Release()
{
    m_spBoxComponent = nullptr;

    CGameObject::Release();
}