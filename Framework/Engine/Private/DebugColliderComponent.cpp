#include "pch.h"
#include "DebugColliderComponent.h"

// Engine
#include "EngineModule.h"

// Handler
#include "ShaderHandler.h"

// Manager
#include "RenderManager.h"
#include "DeviceManager.h"

HRESULT Engine::CDebugColliderComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// collider component
	std::shared_ptr<FDebugColliderComponentDesc> spDebugColliderComponentDesc = static_pointer_cast<FDebugColliderComponentDesc>(_spComponentDesc);
	m_eColliderType = spDebugColliderComponentDesc->m_eColliderType;
	m_eCollisionLayer = spDebugColliderComponentDesc->m_eCollisionLayer;
	m_eCollisionFlag = spDebugColliderComponentDesc->m_eCollisionFlag;
	m_vCenter = spDebugColliderComponentDesc->m_vCenter;

	// 디버그 드로우
	m_cpDeviceContext = CEngineModule::GetInstance()->GetDeviceManager()->GetDeviceContext();
	m_wpRenderManager = CEngineModule::GetInstance()->GetRenderManager();
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();

	shared_ptr<CShaderHandler> spShaderHandler = CShaderHandler::GetInstance();
	m_cpInputLayout = spShaderHandler->GetDebugDrawInputLayout();
	m_spPrimitiveBatch = spShaderHandler->GetPrimitiveBatch();
	m_spBasicEffect = spShaderHandler->GetBasicEffect();

	return S_OK;
}

HRESULT Engine::CDebugColliderComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CDebugColliderComponent::PreTickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CDebugColliderComponent::TickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CDebugColliderComponent::PostTickComponent(float _fDeltaSeconds)
{
	m_wpRenderManager.lock()->AddComponent(shared_from_this(), m_eRenderType);
	return 0;
}

HRESULT Engine::CDebugColliderComponent::EndPlay()
{
	return S_OK;
}

HRESULT Engine::CDebugColliderComponent::BindVariable()
{
	return S_OK;
}

void Engine::CDebugColliderComponent::Render(int32 _iShaderPath)
{
}

void Engine::CDebugColliderComponent::Release()
{
}
