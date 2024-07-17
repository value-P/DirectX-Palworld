#include "pch.h"
#include "Weapon.h"

#include "MeshComponent.h"
#include "Model.h"
#include "Bone.h"

std::shared_ptr<CWeapon> Client::CWeapon::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    shared_ptr<CWeapon> spInstance = make_shared<CWeapon>();
    CHECKF(spInstance, L"CWeapon : Create Failed");
    spInstance->Initialize(_spGameObjectDesc);

    return spInstance;
}

HRESULT Client::CWeapon::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
    // item desc
    if (FAILED(CItem::Initialize(_spGameObjectDesc))) { return E_FAIL; }

    // weapon desc
    shared_ptr<FWeaponDesc> spWeaponDesc = static_pointer_cast<FWeaponDesc>(_spGameObjectDesc);
    m_wpAttachedBone        = spWeaponDesc->m_spAttachedBone;
    m_eWeaponActionType     = spWeaponDesc->m_eWeaponActionType;
    m_eWeaponType           = spWeaponDesc->m_eWeaponType;
    m_fAttackPower          = spWeaponDesc->m_fAttackPower;
    m_fDurability           = spWeaponDesc->m_fDurability;
    m_iWeaponUnlockLevel    = spWeaponDesc->m_iWeaponUnlockLevel;

    InitMeshComponent(spWeaponDesc);

    return S_OK;
}

HRESULT Client::CWeapon::BeginPlay()
{
    CItem::BeginPlay();

    return S_OK;
}

int32 Client::CWeapon::PreTick(float _fDeltaSeconds)
{
    CItem::PreTick(_fDeltaSeconds);

    return 0;
}

int32 Client::CWeapon::Tick(float _fDeltaSeconds)
{
    CItem::Tick(_fDeltaSeconds);

    return 0;
}

int32 Client::CWeapon::PostTick(float _fDeltaSeconds)
{
    CItem::PostTick(_fDeltaSeconds);

    return 0;
}

HRESULT Client::CWeapon::EndPlay()
{
    if (FAILED(CItem::EndPlay())) { return E_FAIL; }

    return S_OK;
}

void Client::CWeapon::Release()
{
    CItem::Release();
}

void Client::CWeapon::InitMeshComponent(const std::shared_ptr<FWeaponDesc>& _spWeaponDesc)
{
    if (_spWeaponDesc->m_wstrModelFilePath == L"") { return; }

    // component desc
    FMeshComponentDesc tMeshDesc;
    tMeshDesc.m_wstrComponentName = L"MeshComponent";
    tMeshDesc.m_eRenderType = _spWeaponDesc->m_eRenderType;
    tMeshDesc.m_eComponentType = EComponentType::MESH;
    tMeshDesc.m_wpOwnerGameObject = shared_from_this();
    tMeshDesc.m_eMeshType = _spWeaponDesc->m_eMeshManageType;

    // mesh component desc
    tMeshDesc.m_wstrModelFilePath = _spWeaponDesc->m_wstrModelFilePath;
    tMeshDesc.m_matPivot = _spWeaponDesc->m_matPivot;
    tMeshDesc.m_spPivotBone = _spWeaponDesc->m_spAttachedBone;
    if (_spWeaponDesc->m_bIsAnimed)
    {
        tMeshDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
    }
    else
    {
        tMeshDesc.m_wstrShaderName = L"FX_NonAnimMesh_PNT2T";
    }
    
    m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshDesc));
    AddComponent(tMeshDesc.m_wstrComponentName, m_spMeshComponent);
}

Matrix Client::CWeapon::GetBoneWorldMatrix(const wstring& _wstrBonename)
{
    Matrix matResult;

    shared_ptr<CBone> spBone = m_spMeshComponent->GetBonePtr(_wstrBonename);

    if (spBone)
    {
        matResult =  spBone->GetCombinedTransformationMatrix() * GetAttachedWorldMatrix();
    }

    return matResult;
}

void Client::CWeapon::SetUseAnimation(bool _bUseAnimation, bool _bUseIdentityMatrices)
{
    m_spMeshComponent->SetUseAnimation(_bUseAnimation, _bUseIdentityMatrices);
}

void Client::CWeapon::ChangeAnimation(int32 _iAnimIndex)
{
    if (m_spMeshComponent->GetModel()->GetCurrentDownAnimID() != _iAnimIndex &&
        !m_spMeshComponent->GetModel()->IsDownLerping())
    {
        m_spMeshComponent->SetUseAnimation(true);
        m_spMeshComponent->ChangeAnimation(_iAnimIndex, 0.f, false);
    }
}

Vector3 Client::CWeapon::GetAttachedWorldPosition()
{
    Matrix matWorld = GetTransformMatrix().value() * m_wpParentGameObject.lock()->GetTransformMatrix().value();

    if (!m_wpAttachedBone.expired())
    {
        matWorld = m_wpAttachedBone.lock()->GetCombinedTransformationMatrix() * matWorld;
    }

    return matWorld.Translation();
}

Matrix Client::CWeapon::GetAttachedWorldMatrix()
{
    Matrix matWorld = GetTransformMatrix().value() * m_wpParentGameObject.lock()->GetTransformMatrix().value();

    if (!m_wpAttachedBone.expired())
    {
        matWorld = m_wpAttachedBone.lock()->GetCombinedTransformationMatrix() * matWorld;
    }

    return matWorld;
}
