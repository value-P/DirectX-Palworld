#include "pch.h"
#include "BowlingTrigger.h"

#include "BoxComponent.h"
#include "MeshComponent.h"

#include "GameManager.h"
#include "BowlingSystem.h"

#include "Bowl.h"
#include "Model.h"

shared_ptr<CBowlingTrigger> Client::CBowlingTrigger::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    std::shared_ptr<CBowlingTrigger> spInstance = make_shared<CBowlingTrigger>();
    ENSUREF(spInstance->Initialize(_spGameObjectDesc), L"Failed to initialize CBowlingTrigger.");
    return spInstance;
}

HRESULT Client::CBowlingTrigger::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; };

    std::shared_ptr<FBowlingTriggerDesc> spBowlingTriggerDesc = static_pointer_cast<FBowlingTriggerDesc>(_spGameObjectDesc);
    
    {
        FBoxComponentDesc tBoxComponentDesc;
        tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
        tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
        tBoxComponentDesc.m_wstrComponentName = L"BoxComponent";
        tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

        tBoxComponentDesc.m_bTrigger = true;
        tBoxComponentDesc.m_bGravity = false;
        tBoxComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
        tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
        tBoxComponentDesc.m_bDeferredAdd = true;

        tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BOWLINGTRIGGER;
        tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BOWLINGTRIGGER;

        tBoxComponentDesc.m_fMass = 0.0f;
        tBoxComponentDesc.m_fDensity = 1.0f;

        tBoxComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
        tBoxComponentDesc.m_vLocalPosition = Vector3(0.0f, 0.0f, 0.0f);
        tBoxComponentDesc.m_vMaterial = Vector3(0.0f, 0.0f, 0.0f);
        tBoxComponentDesc.m_vExtents = spBowlingTriggerDesc->m_vExtents;

        m_spBoxTrigger = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
        ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxTrigger));
    }

    return S_OK;
}

HRESULT Client::CBowlingTrigger::BeginPlay()
{
    if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; };

    return S_OK;
}

int32 Client::CBowlingTrigger::Tick(float _fDeltaSeconds)
{
    CGameObject::Tick(_fDeltaSeconds);

    return 0;
}

HRESULT Client::CBowlingTrigger::EndPlay()
{
    if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

    return S_OK;
}

void Client::CBowlingTrigger::Release()
{
    CGameObject::Release();
}

void Client::CBowlingTrigger::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
    CColliderComponent* pColliderComponent = static_cast<CColliderComponent*>(_pDstComponent);
    if (ECollisionLayer::BOWL == pColliderComponent->GetCollisionLayer())
    {
        // 컬링을 끕니다.
        if (false == pColliderComponent->GetOwnerGameObject()->IsCulled())
        {
            // 볼링공 초기화 파티클을 재생합니다.
            CGameManager::GetInstance()->GetBowlingSystem()->ActiveBowlingPinParticle(pColliderComponent->GetOwnerGameObject(), static_pointer_cast<CBowl>(pColliderComponent->GetOwnerGameObject())->GetMeshComponent()->GetModel());

            pColliderComponent->GetOwnerGameObject()->SetCulled(true);
            CGameManager::GetInstance()->GetBowlingSystem()->SetNext(true);
            //CGameManager::GetInstance()->GetBowlingSystem()->NextTurn();
        }
    }
}

void Client::CBowlingTrigger::OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
}
