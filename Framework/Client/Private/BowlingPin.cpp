#include "pch.h"
#include "BowlingPin.h"

#include "MeshComponent.h"
#include "BoxComponent.h"

#include "ColliderComponent.h"

#include "EngineModule.h"
#include "SoundManager.h"

std::shared_ptr<CBowlingPin> Client::CBowlingPin::Create(const std::shared_ptr<FBowlingPinDesc>& _spBowlingPinDesc)
{
    const std::shared_ptr<CBowlingPin> spBowlingPin = make_shared<CBowlingPin>();
    ENSUREF(spBowlingPin->Initialize(_spBowlingPinDesc), L"Failed to initialize bowlingpin");
    return spBowlingPin;
}

HRESULT Client::CBowlingPin::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

    InitMeshComponent();
    InitColliderComponent();

    m_spMeshComponent->ChangeAnimation((int32)EColorfulBird::AS_ColofulBird_Idle, false);

    //m_spBoxComponent->RequestTransform(std::nullopt, Vector3(0.f, 90.f, 0.f));

    m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

    return S_OK;
}

HRESULT Client::CBowlingPin::BeginPlay()
{
    if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

    return S_OK;
}

int32 Client::CBowlingPin::PreTick(float _fDeltaSeconds)
{
    switch (CGameObject::PreTick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return S_OK;
}

int32 Client::CBowlingPin::Tick(float _fDeltaSeconds)
{
    switch (CGameObject::Tick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return S_OK;
}

int32 Client::CBowlingPin::PostTick(float _fDeltaSeconds)
{
    switch (CGameObject::PostTick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return S_OK;
}

HRESULT Client::CBowlingPin::EndPlay()
{
    if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

    return S_OK;
}

void Client::CBowlingPin::Release()
{
    m_spMeshComponent = nullptr;
    m_spBoxComponent = nullptr;

    CGameObject::Release();
}

void Client::CBowlingPin::OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
    CColliderComponent* pColliderComponent = static_cast<CColliderComponent*>(_pDstComponent);
    if (ECollisionLayer::BOWL == pColliderComponent->GetCollisionLayer() || ECollisionLayer::BOWLINGPIN == pColliderComponent->GetCollisionLayer())
    {
        m_wpSoundManager.lock()->PlaySoundEx("VO_ColorFulBird_05_Pain_v02_393302515");
        m_bColl = true;
    }
}

void Client::CBowlingPin::InitMeshComponent()
{
    FMeshComponentDesc tMeshComponentDesc;
    tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";
    tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
    tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
    tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

    tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/ColorfulBird.dat";
    tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f);// *Matrix::CreateRotationY(XMConvertToRadians(180.f));
    tMeshComponentDesc.m_wstrShaderName = L"";

    m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
    ENSURE(AddComponent(tMeshComponentDesc.m_wstrComponentName, m_spMeshComponent));
}

void Client::CBowlingPin::InitColliderComponent()
{
    FBoxComponentDesc tBoxComponentDesc;
    tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
    tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
    tBoxComponentDesc.m_wstrComponentName = L"SphereComponent";
    tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

    tBoxComponentDesc.m_bTrigger = false;
    tBoxComponentDesc.m_bGravity = true; 
    tBoxComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
    tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
    tBoxComponentDesc.m_bDeferredAdd = true;

    tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BOWLINGPIN;
    tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BOWLINGPIN;

    tBoxComponentDesc.m_fMass = 0.4f;
    tBoxComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
    tBoxComponentDesc.m_vLocalPosition = Vector3(0.0f, 0.7f, 0.0f);
    tBoxComponentDesc.m_vMaterial = Vector3(0.02f, 0.02f, 0.0f);

    tBoxComponentDesc.m_vExtents = Vector3(0.3f, 0.8f, 0.3f);

    m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
    ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxComponent));

    Vector3 vLocalPos = Vector3(0.0f, 0.8f, 0.0f);
    m_spBoxComponent->SetMassLocalPose(vLocalPos);
}

void Client::CBowlingPin::RequestTransform(std::optional<Vector3> _vTranslate, std::optional<Vector3> _vRotate)
{
    if (!m_spBoxComponent) { return; }

    m_spBoxComponent->RequestTransform(_vTranslate, _vRotate);
}

void Client::CBowlingPin::SetLockFlags(int32 _iAxisLockType)
{
    if (!m_spBoxComponent) { return; }

    m_spBoxComponent->SetLockFlags(_iAxisLockType);
}
