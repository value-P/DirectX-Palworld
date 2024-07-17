#include "pch.h"
#include "Camera.h"

#include "EngineModule.h"
#include "InputManager.h"

#include "TransformComponent.h"
#include "CameraComponent.h"

std::shared_ptr<CCamera> Engine::CCamera::Create(const std::shared_ptr<FCameraDesc>& _spCameraDesc)
{
	const std::shared_ptr<CCamera> spCamera = make_shared<CCamera>();
	ENSUREF(spCamera->Initialize(_spCameraDesc), L"Failed to initialize camera.");
	return spCamera;
}

HRESULT Engine::CCamera::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// game object desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// camera desc
	shared_ptr<FCameraDesc> spCameraDesc = static_pointer_cast<FCameraDesc>(_spGameObjectDesc);

	/*
	// transform component
	{
		// component desc
		shared_ptr<CTransformComponent::FTransformComponentDesc> spTransformComponentDesc = make_shared<CTransformComponent::FTransformComponentDesc>();
		spTransformComponentDesc->m_wpOwnerGameObject;
		spTransformComponentDesc->m_eComponentType = EComponentType::TRANSFORM;
		spTransformComponentDesc->m_wstrComponentName = L"TransformComponent";
		spTransformComponentDesc->m_eRenderType = ERenderType::NONE;

		// transform component desc
		spTransformComponentDesc->m_vLocalScale = Vector3::One;
		spTransformComponentDesc->m_vLocalRotate = Vector3::Zero;
		spTransformComponentDesc->m_vLocalTranslate = Vector3::Zero;

		AddComponent(spTransformComponentDesc->m_wstrComponentName, CTransformComponent::Create(spTransformComponentDesc));
	}
	*/

	// camera component
	{
		// component desc
		shared_ptr<FCameraComponentDesc> spCameraComponentDesc = make_shared<FCameraComponentDesc>();
		spCameraComponentDesc->m_wpOwnerGameObject = shared_from_this();
		spCameraComponentDesc->m_eComponentType = EComponentType::CAMERA;
		spCameraComponentDesc->m_wstrComponentName = L"CameraComponent";
		spCameraComponentDesc->m_eRenderType = ERenderType::NONE;

		// camera component desc
		spCameraComponentDesc->m_eCameraType = ECameraType::DEFAULT;
		spCameraComponentDesc->m_bMouseControl = spCameraDesc->m_bUseMouseInput;
		spCameraComponentDesc->m_bKeyboardControl = spCameraDesc->m_bUseKeyInput;
		spCameraComponentDesc->m_fMouseSensitivity = 0.1f;
		spCameraComponentDesc->m_fKeyboardSensitivity = 50.0f;

		spCameraComponentDesc->m_eProjectionType = EProjectionType::PERSPECTIVE;
		spCameraComponentDesc->m_fAspectRatio = 16.0f / 9.0f;
		spCameraComponentDesc->m_fFieldOfView = 60.0f;
		spCameraComponentDesc->m_fFarPlane = 5000.0f;
		spCameraComponentDesc->m_fNearPlane = 0.1f;

		spCameraComponentDesc->m_vSphericalCameraAxisOffset = Vector3::Zero;
		spCameraComponentDesc->m_vSphericalCameraEyeOffset = Vector3::Zero;
		spCameraComponentDesc->m_fCurSphericalCameraRadius = 5.0f;
		spCameraComponentDesc->m_fMinElevationDegree = 0.0f;
		spCameraComponentDesc->m_fMaxElevationDegree = 60.0f;

		spCameraComponentDesc->m_fCullFrustumNear = 0.1f;
		spCameraComponentDesc->m_fCullFrustumFar = 100.0f;

		shared_ptr<CCameraComponent> spCameraComponent = CCameraComponent::Create(spCameraComponentDesc);
		AddComponent(spCameraComponentDesc->m_wstrComponentName, spCameraComponent);
		m_wpCameraComponent = spCameraComponent;
	}
	
	return S_OK;
}

HRESULT Engine::CCamera::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }
	
	return S_OK;
}

int32 Engine::CCamera::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CCamera::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CCamera::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CCamera::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CCamera::Release()
{
	CGameObject::Release();
}

HRESULT Engine::CCamera::SetOnAir()
{
	return m_wpCameraComponent.lock()->SetOnAir();
}

HRESULT Engine::CCamera::LockCameraViewDirection(bool _bLockCameraViewDirection)
{
	CHECK(!m_wpCameraComponent.expired());
	m_wpCameraComponent.lock()->SetMouseControl(!_bLockCameraViewDirection);
	return S_OK;
}

void Engine::CCamera::SetKeyboardSensitivity(float _fKeyboardSensitivity)
{
	m_wpCameraComponent.lock()->SetKeyboardSensitivity(_fKeyboardSensitivity);
}
