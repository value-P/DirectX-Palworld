#include "pch.h"
#include "DebugBoxComponent.h"


#include "CameraManager.h"

#include "GameObject.h"

std::shared_ptr<CDebugBoxComponent> Engine::CDebugBoxComponent::Create(const std::shared_ptr<FDebugBoxComponentDesc>& _spDebugBoxComponentDesc)
{
    const std::shared_ptr<CDebugBoxComponent> spDebugBoxComponent = make_shared<CDebugBoxComponent>();
    ENSUREF(spDebugBoxComponent->InitializeComponent(_spDebugBoxComponentDesc), L"Failed to initialize debug box component");
    return spDebugBoxComponent;
}

HRESULT Engine::CDebugBoxComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// collider component
	if (FAILED(CDebugColliderComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// box component
	shared_ptr<FDebugBoxComponentDesc> spDebugBoxComponent = static_pointer_cast<FDebugBoxComponentDesc>(_spComponentDesc);
	m_vExtents = spDebugBoxComponent->m_vExtents;
	m_vCenter = spDebugBoxComponent->m_vCenter;

#ifdef _DEBUG
	// 디버그 드로우
	m_tDxLocalOBB = BoundingBox(m_vCenter, m_vExtents);
#endif // _DEBUG

	return S_OK;
}

HRESULT Engine::CDebugBoxComponent::BeginPlay()
{
	if (FAILED(CDebugColliderComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CDebugBoxComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CDebugColliderComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

#ifdef _DEBUG
	Matrix matWorld = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
	m_tDxLocalOBB.Transform(m_tDxWorldOBB, matWorld);
#endif // _DEBUG

	return 0;
}

int32 Engine::CDebugBoxComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CDebugColliderComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CDebugBoxComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CDebugColliderComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CDebugBoxComponent::EndPlay()
{
	if (FAILED(CDebugColliderComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CDebugBoxComponent::Render(int32 _iShaderPath)
{
#ifdef _DEBUG
	m_spBasicEffect->SetWorld(XMMatrixIdentity());

	m_spBasicEffect->SetView(m_wpCameraManager.lock()->GetCurViewMatrix());

	m_spBasicEffect->SetProjection(m_wpCameraManager.lock()->GetCurPerspectiveProjectionMatrix());

	m_spBasicEffect->Apply(m_cpDeviceContext.Get());

	m_cpDeviceContext->IASetInputLayout(m_cpInputLayout.Get());

	m_spPrimitiveBatch->Begin();

	DX::Draw(m_spPrimitiveBatch.get(), m_tDxWorldOBB, Colors::Green);

	m_spPrimitiveBatch->End();
#endif // _DEBUG

	CDebugColliderComponent::Render(_iShaderPath);
}

void Engine::CDebugBoxComponent::Release()
{
	CDebugColliderComponent::Release();
}

HRESULT Engine::CDebugBoxComponent::BindVariable()
{
	return S_OK;
}
