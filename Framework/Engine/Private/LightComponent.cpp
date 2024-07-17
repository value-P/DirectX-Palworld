#include "pch.h"
#include "LightComponent.h"

#include "EngineModule.h"
#include "LightManager.h"
#include "GameObject.h"

shared_ptr<CLightComponent> Engine::CLightComponent::Create(const std::shared_ptr<FLightComponentDesc>& _spLightComponentDesc)
{
	const shared_ptr<CLightComponent> spLightComponent = make_shared<CLightComponent>();
	spLightComponent->InitializeComponent(_spLightComponentDesc);
	return spLightComponent;
}

HRESULT Engine::CLightComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component desc
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// light component desc	
	shared_ptr<FLightComponentDesc> spLightComponentDesc = static_pointer_cast<FLightComponentDesc>(_spComponentDesc);
	m_eLightType		= spLightComponentDesc->m_eLightType;
	m_vAmbient			= spLightComponentDesc->m_vAmbient;
	m_vDiffuse			= spLightComponentDesc->m_vDiffuse;
	m_vSpecular			= spLightComponentDesc->m_vSpecular;
	m_vPosition			= spLightComponentDesc->m_vPosition;
	m_vDirection		= spLightComponentDesc->m_vDirection;
	m_vStrength			= spLightComponentDesc->m_vStrength;
	m_fFallOffBegin		= spLightComponentDesc->m_fFallOffBegin;
	m_fFallOffEnd		= spLightComponentDesc->m_fFallOffEnd;
	m_fRange			= spLightComponentDesc->m_fRange;
	m_fPower			= spLightComponentDesc->m_fPower;


	// 라이트 매니저에 라이트 컴포넌트를 추가합니다.
	m_wpLightManager = CEngineModule::GetInstance()->GetLightManager();
	m_wpLightManager.lock()->AddLightComponent(m_wstrComponentName, dynamic_pointer_cast<CLightComponent>(shared_from_this()), m_eLightType);
	return S_OK;
}

HRESULT Engine::CLightComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CLightComponent::PreTickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CLightComponent::TickComponent(float _fDeltaSeconds)
{
	switch (m_eLightType)
	{
	case ELightType::SPOT:
		m_vPosition = m_wpOwnerGameObject.lock()->GetPosition().value();
		break;

	case ELightType::POINT:
		m_vPosition = m_wpOwnerGameObject.lock()->GetPosition().value();
		break;

	case ELightType::DIRECTIONAL:
		break;

	default:
		break;
	}

	return 0;
}

int32 Engine::CLightComponent::PostTickComponent(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Engine::CLightComponent::EndPlay()
{
	return S_OK;
}

void Engine::CLightComponent::Render()
{
	return;
}

void Engine::CLightComponent::Release()
{
	switch (m_eLightType)
	{
	case Engine::ELightType::DIRECTIONAL:
		break;

	case Engine::ELightType::POINT:
		break;

	case Engine::ELightType::SPOT:
		break;

	default:
		CHECKF(false, L"Light type is not designated.")
		break;
	}
}

HRESULT Engine::CLightComponent::BindVariable()
{
	return S_OK;
}
