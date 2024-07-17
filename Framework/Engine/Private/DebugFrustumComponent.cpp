#include "pch.h"
#include "DebugFrustumComponent.h"

// Manager
#include "CameraManager.h"

// GameObject
#include "GameObject.h"

std::shared_ptr<CDebugFrustumComponent> Engine::CDebugFrustumComponent::Create(const std::shared_ptr<FDebugFrustumComponentDesc>& _spDebugFrustumComponentDesc)
{
    const std::shared_ptr<CDebugFrustumComponent> spDebugFrustumComponent = make_shared<CDebugFrustumComponent>();
    ENSUREF(spDebugFrustumComponent->InitializeComponent(_spDebugFrustumComponentDesc), L"Failed to initialize debug box component");
    return spDebugFrustumComponent;
}

HRESULT Engine::CDebugFrustumComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
    // collider component
    if (FAILED(CDebugColliderComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

    shared_ptr<FDebugFrustumComponentDesc> spDebugFrustumComponentDesc = static_pointer_cast<FDebugFrustumComponentDesc>(_spComponentDesc);
    m_vRadians      = spDebugFrustumComponentDesc->m_vRadians;
    m_fRightSlope   = spDebugFrustumComponentDesc->m_fRightSlope;
    m_fLeftSlope    = spDebugFrustumComponentDesc->m_fLeftSlope;
    m_fTopSlope     = spDebugFrustumComponentDesc->m_fTopSlope;
    m_fBottomSlope  = spDebugFrustumComponentDesc->m_fBottomSlope;
    m_fNearPlane    = spDebugFrustumComponentDesc->m_fNearPlane;
    m_fFarPlane     = spDebugFrustumComponentDesc->m_fFarPlane;

    Vector4		vRotation;
    vRotation = XMQuaternionRotationRollPitchYaw(m_vRadians.x, m_vRadians.y, m_vRadians.z);

#ifdef _DEBUG
    // 디버그 드로우
    m_tDxLocalFrustum = BoundingFrustum(m_vCenter, vRotation, m_fRightSlope, m_fLeftSlope, m_fTopSlope, m_fBottomSlope, m_fNearPlane, m_fFarPlane);
#endif // _DEBUG

    return S_OK;
}

HRESULT Engine::CDebugFrustumComponent::BeginPlay()
{
    if (FAILED(CDebugColliderComponent::BeginPlay())) { return E_FAIL; }

    return S_OK;
}

int32 Engine::CDebugFrustumComponent::PreTickComponent(float _fDeltaSeconds)
{
    switch (CDebugColliderComponent::PreTickComponent(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

#ifdef _DEBUG
    Matrix matWorld = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
    m_tDxLocalFrustum.Transform(m_tDxWorldFrustum, matWorld);
#endif // _DEBUG

    return 0;
}

int32 Engine::CDebugFrustumComponent::TickComponent(float _fDeltaSeconds)
{
    switch (CDebugColliderComponent::TickComponent(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return 0;
}

int32 Engine::CDebugFrustumComponent::PostTickComponent(float _fDeltaSeconds)
{
    switch (CDebugColliderComponent::PostTickComponent(_fDeltaSeconds))
    {
    case -1:	return -1;
    default:	break;
    }

    return 0;
}

HRESULT Engine::CDebugFrustumComponent::EndPlay()
{
    if (FAILED(CDebugColliderComponent::EndPlay())) { return E_FAIL; }

    return S_OK;
}

void Engine::CDebugFrustumComponent::Render(int32 _iShaderPath)
{
#ifdef _DEBUG
    m_spBasicEffect->SetWorld(XMMatrixIdentity());

    m_spBasicEffect->SetView(m_wpCameraManager.lock()->GetCurViewMatrix());

    m_spBasicEffect->SetProjection(m_wpCameraManager.lock()->GetCurPerspectiveProjectionMatrix());

    m_spBasicEffect->Apply(m_cpDeviceContext.Get());

    m_cpDeviceContext->IASetInputLayout(m_cpInputLayout.Get());

    m_spPrimitiveBatch->Begin();

    DX::Draw(m_spPrimitiveBatch.get(), m_tDxWorldFrustum, Colors::Green);

    m_spPrimitiveBatch->End();
#endif // _DEBUG

    CDebugColliderComponent::Render(_iShaderPath);
}

void Engine::CDebugFrustumComponent::Release()
{
    CDebugColliderComponent::Release();
}

HRESULT Engine::CDebugFrustumComponent::BindVariable()
{
    return S_OK;
}
