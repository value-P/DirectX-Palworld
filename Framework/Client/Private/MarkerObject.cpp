#include "pch.h"
#include "MarkerObject.h"

#include "EngineModule.h"
#include "SoundManager.h"

// client manager
#include "UIManager.h"

#include "Panel.h"
// component
#include "AnimationImageComponent.h"
#include "BoxComponent.h"

shared_ptr<CMarkerObject> Client::CMarkerObject::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    std::shared_ptr<CMarkerObject> spInstance = make_shared<CMarkerObject>();
    ENSUREF(spInstance->Initialize(_spGameObjectDesc), L"Failed to initialize CMarkerObject.");
    return spInstance;
}

HRESULT Client::CMarkerObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; };

    // 구조체 연동
    std::shared_ptr<FMarkerObjectDesc> spMarkerObjectDesc = static_pointer_cast<FMarkerObjectDesc>(_spGameObjectDesc);
    m_eMapNameType = spMarkerObjectDesc->m_eMapNameType;

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

        tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::MINIGAME_GACHA;
        tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::MINIGAME_GACHA;

        tBoxComponentDesc.m_fMass = 0.0f;
        tBoxComponentDesc.m_fDensity = 1.0f;

        tBoxComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
        tBoxComponentDesc.m_vLocalPosition = Vector3(0.0f, 0.0f, 0.0f);
        tBoxComponentDesc.m_vMaterial = Vector3(0.0f, 0.0f, 0.0f);
        tBoxComponentDesc.m_vExtents = Vector3(3.0f, 3.0f, 3.0f);

        m_spBoxTrigger = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
        ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxTrigger));
    }

    return S_OK;
}

HRESULT Client::CMarkerObject::BeginPlay()
{
    if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; };

    m_wpUIManager = CUIManager::GetInstance();
    m_spMapNamePanel = m_wpUIManager.lock()->FindParentsPanel(L"MapNameUI");
    m_spNameAnimation = m_spMapNamePanel->FindComponent<CAnimationImageComponent>(L"NameAnimation");
    m_spNameAnimation->SetTextureFileName(L"");

    return S_OK;
}

int32 Client::CMarkerObject::Tick(float _fDeltaSeconds)
{
    CGameObject::Tick(_fDeltaSeconds);

    if (true == m_spMapNamePanel->IsActive())
    {
        if (true == m_spNameAnimation->IsFinish())
        {
            m_spMapNamePanel->SetActive(false);
        }
    }

    return 0;
}

HRESULT Client::CMarkerObject::EndPlay()
{
    if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

    return S_OK;
}

void Client::CMarkerObject::Release()
{
    m_spMapNamePanel = nullptr;
    m_spNameAnimation = nullptr;
    m_spBoxTrigger = nullptr;

    CGameObject::Release();
}

void Client::CMarkerObject::SetPosition(const Vector3& _vPosition)
{
}

void Client::CMarkerObject::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
    // 컨트롤러 컴포넌트
    if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
    {
        if (false == m_spMapNamePanel->IsActive())
        {
            if (EMapNameType::WIND_HILL == m_eMapNameType)
            {
                m_spNameAnimation->ResetFrame();
                m_spMapNamePanel->SetActive(true);
                if (m_wstrWindTextureName != m_spNameAnimation->GetTextureFileName())
                {
                    CEngineModule::GetInstance()->GetSoundManager()->PlaySoundEx("NewLocationSound");
                    m_spNameAnimation->SetTextureFileName(m_wstrWindTextureName);
                }
                else
                {
                    m_spMapNamePanel->SetActive(false);
                }
            }
            else if (EMapNameType::TOWER == m_eMapNameType)
            {
                m_spNameAnimation->ResetFrame();
                m_spMapNamePanel->SetActive(true);
                if (m_wstrTowerTextureName != m_spNameAnimation->GetTextureFileName())
                {
                    CEngineModule::GetInstance()->GetSoundManager()->PlaySoundEx("NewLocationSound");
                    m_spNameAnimation->SetTextureFileName(m_wstrTowerTextureName);
                }
                else
                {
                    m_spMapNamePanel->SetActive(false);
                }
            }
        }
    }
}

void Client::CMarkerObject::OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
}
