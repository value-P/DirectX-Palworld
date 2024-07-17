#include "pch.h"
#include "Bowl.h"

#include "MeshComponent.h"
#include "SphereComponent.h"

std::shared_ptr<CBowl> Client::CBowl::Create(const std::shared_ptr<FBowlDesc>& _spBowlDesc)
{
    const std::shared_ptr<CBowl> spBowl = make_shared<CBowl>();
    ENSUREF(spBowl->Initialize(_spBowlDesc), L"Failed to initialize bowl");
    return spBowl;
}

HRESULT Client::CBowl::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    if(FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

    InitMeshComponent();
    InitColliderComponent();

    m_spMeshComponent->ChangeAnimation((int32)ESheepBall::AS_SheepBall_Shield, false);
    //m_spMeshComponent->ChangeAnimation((int32)ESheepBall::AS_SheepBall_Korogaru_Loop, false);
    
    //m_spSphereComponent->RequestTransform(std::nullopt, Vector3(0.f, 90.f, 0.f));

    return S_OK;
}

HRESULT Client::CBowl::BeginPlay()
{
    if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }


    return S_OK;
}

int32 Client::CBowl::PreTick(float _fDeltaSeconds)
{
    switch (CGameObject::PreTick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return S_OK;
}

int32 Client::CBowl::Tick(float _fDeltaSeconds)
{
    switch (CGameObject::Tick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    //m_spMeshComponent->ChangeAnimation();

    return S_OK;
}

int32 Client::CBowl::PostTick(float _fDeltaSeconds)
{
    switch (CGameObject::PostTick(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return S_OK;
}

HRESULT Client::CBowl::EndPlay()
{
    if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

    return S_OK;
}

void Client::CBowl::Release()
{
    m_spMeshComponent = nullptr;
    m_spSphereComponent = nullptr;

    CGameObject::Release();
}

void Client::CBowl::LookAt(const Vector3& _vPosition)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->LookAt(_vPosition);
}

void Client::CBowl::Turn(const Vector3& _vAxis, float _fAngularSpeed)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->Turn(_vAxis, _fAngularSpeed);
}

void Client::CBowl::MoveForward(float _fSpeed)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->MoveColliderForward(_fSpeed);
}

void Client::CBowl::MoveBackward(float _fSpeed)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->MoveColliderBackward(_fSpeed);
}

void Client::CBowl::MoveRight(float _fSpeed)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->MoveColliderRight(_fSpeed);
}

void Client::CBowl::MoveLeft(float _fSpeed)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->MoveColliderLeft(_fSpeed);
}

void Client::CBowl::MoveUp(float _fSpeed)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->MoveColliderUp(_fSpeed);
}

void Client::CBowl::MoveDown(float _fSpeed)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->MoveColliderDown(_fSpeed);
}

void Client::CBowl::SetPosition(const Vector3& _vPosition)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->SetPosition(_vPosition);
}

void Client::CBowl::AddForce(const Vector3& _vDirection, float _fPower)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->AddForce(_vDirection, _fPower);
}

void Client::CBowl::ChangeAnimation(int32 _iNextAnim)
{
    m_spMeshComponent->ChangeDifferentAnimation(_iNextAnim, false);
}

void Client::CBowl::RequestTransform(std::optional<Vector3> _vTranslate, std::optional<Vector3> _vRotate)
{
    if (!m_spSphereComponent) { return; }

    m_spSphereComponent->RequestTransform(_vTranslate, _vRotate);
}

void Client::CBowl::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
}

void Client::CBowl::InitMeshComponent()
{
    FMeshComponentDesc tMeshComponentDesc;
    tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";
    tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
    tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
    tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

    tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/SheepBall.dat";
    tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(180.f));
    tMeshComponentDesc.m_wstrShaderName = L"";

    m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
    ENSURE(AddComponent(tMeshComponentDesc.m_wstrComponentName, m_spMeshComponent));
}

void Client::CBowl::InitColliderComponent()
{
    FSphereComponentDesc tSphereComponentDesc;
    tSphereComponentDesc.m_eComponentType = EComponentType::SPHERE;
    tSphereComponentDesc.m_wpOwnerGameObject = shared_from_this();
    tSphereComponentDesc.m_wstrComponentName = L"SphereComponent";
    tSphereComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

    tSphereComponentDesc.m_bTrigger = false;
    tSphereComponentDesc.m_bGravity = true;
    tSphereComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
    tSphereComponentDesc.m_eColliderType = EColliderType::SPHERE;
    tSphereComponentDesc.m_bDeferredAdd = true;

    tSphereComponentDesc.m_eCollisionFlag = ECollisionFlag::BOWL;
    tSphereComponentDesc.m_eCollisionLayer = ECollisionLayer::BOWL;

    tSphereComponentDesc.m_fMass = 50.0f;
    tSphereComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
    tSphereComponentDesc.m_vLocalPosition = Vector3(0.0f, 0.4f, 0.0f);
    tSphereComponentDesc.m_vMaterial = Vector3(0.1f, 0.1f, 0.1f);

    tSphereComponentDesc.m_fRadius = 0.5f;

    m_spSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
    ENSURE(AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spSphereComponent));
    //Vector3 vLocalPose = Vector3(0.0f, 3.f, 0.0f);
    //m_spSphereComponent->SetMassLocalPose(vLocalPose);
}
