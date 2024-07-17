#include "pch.h"
#include "DebugSphereComponent.h"

// Manager
#include "CameraManager.h"

// GameObject
#include "GameObject.h"
#include "DebugDraw.h"

#include "MathUtility.h"

std::shared_ptr<CDebugSphereComponent> Engine::CDebugSphereComponent::Create(const std::shared_ptr<FDebugSphereComponentDesc>& _spDebugSphereComponentDesc)
{
	const std::shared_ptr<CDebugSphereComponent> spDebugSphereComponent = make_shared<CDebugSphereComponent>();
	ENSUREF(spDebugSphereComponent->InitializeComponent(_spDebugSphereComponentDesc), L"Failed to initialize debug sphere component.");
	return spDebugSphereComponent;
}

HRESULT Engine::CDebugSphereComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// collider component
	if (FAILED(CDebugColliderComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// sphere component
	shared_ptr<FDebugSphereComponentDesc> spDebugSphereComponentDesc = static_pointer_cast<FDebugSphereComponentDesc>(_spComponentDesc);
	m_fRadius = spDebugSphereComponentDesc->m_fRadius;
	m_vCenter = spDebugSphereComponentDesc->m_vCenter;

#ifdef _DEBUG
	// 디버그 드로우
	m_tDxLocalSphere = BoundingSphere(m_vCenter, m_fRadius);
#endif // _DEBUG

	return S_OK;
}

HRESULT Engine::CDebugSphereComponent::BeginPlay()
{
	if (FAILED(CDebugColliderComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CDebugSphereComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CDebugColliderComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

#ifdef _DEBUG
	Matrix matWorld = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
	Matrix matScaleKilled = CMathUtility::GetScaledKilledMatrix(matWorld);
	m_tDxLocalSphere.Transform(m_tDxWorldSphere, matWorld);
#endif // _DEBUG

	return 0;
}

int32 Engine::CDebugSphereComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CDebugColliderComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CDebugSphereComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CDebugColliderComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CDebugSphereComponent::EndPlay()
{
	if (FAILED(CDebugColliderComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CDebugSphereComponent::Render(int32 _iShaderPath)
{
#ifdef _DEBUG
	m_cpDeviceContext->GSSetShader(nullptr, nullptr, 0);

	m_spBasicEffect->SetWorld(XMMatrixIdentity());

	m_spBasicEffect->SetView(m_wpCameraManager.lock()->GetCurViewMatrix());

	m_spBasicEffect->SetProjection(m_wpCameraManager.lock()->GetCurPerspectiveProjectionMatrix());

	m_spBasicEffect->Apply(m_cpDeviceContext.Get());

	m_cpDeviceContext->IASetInputLayout(m_cpInputLayout.Get());

	m_spPrimitiveBatch->Begin();

	DX::Draw(m_spPrimitiveBatch.get(), m_tDxWorldSphere, Colors::Green);

	m_spPrimitiveBatch->End();
#endif // _DEBUG

	CDebugColliderComponent::Render(_iShaderPath);
}

void Engine::CDebugSphereComponent::Release()
{
	CDebugColliderComponent::Release();
}

HRESULT Engine::CDebugSphereComponent::BindVariable()
{
	return S_OK;
}
