#include "pch.h"
#include "BowlingCamera.h"

#include "CameraComponent.h"

std::shared_ptr<CBowlingCamera> Client::CBowlingCamera::Create(const std::shared_ptr<FBowlingCameraDesc>& _spBowlingCameraDesc)
{
	const std::shared_ptr<CBowlingCamera> spBowlingCamera = make_shared<CBowlingCamera>();
	ENSUREF(spBowlingCamera->Initialize(_spBowlingCameraDesc), L"Failed to initialize bowlingcamera");
	return spBowlingCamera;
}

HRESULT Client::CBowlingCamera::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	InitCameraComponent(_spGameObjectDesc);

	return S_OK;
}

HRESULT Client::CBowlingCamera::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CBowlingCamera::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 Client::CBowlingCamera::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}


	return S_OK;
}

int32 Client::CBowlingCamera::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

HRESULT Client::CBowlingCamera::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CBowlingCamera::Release()
{
	m_spCameraComponent = nullptr;

	CGameObject::Release();
}

void Client::CBowlingCamera::InitCameraComponent(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{


	//FCameraComponentDesc tCameraComponentDesc;
	//tCameraComponentDesc.m_wpOwnerGameObject = shared_from_this();
	//tCameraComponentDesc.m_eComponentType = EComponentType::CAMERA;
	//tCameraComponentDesc.m_wstrComponentName = L"BowlingCameraComponent";
	//tCameraComponentDesc.m_eRenderType = ERenderType::NONE;

	//// camera component desc
	//tCameraComponentDesc.m_eCameraType = ECameraType::SPHERICAL;
	//tCameraComponentDesc.m_bMouseControl = false;
	//tCameraComponentDesc.m_bKeyboardControl = false;
	//tCameraComponentDesc.m_eProjectionType = EProjectionType::PERSPECTIVE;

	std::shared_ptr<FBowlingCameraDesc> spCameraDesc = static_pointer_cast<FBowlingCameraDesc>(_spGameObjectDesc);

	// component desc
	std::shared_ptr<FCameraComponentDesc> spCameraComponentDesc = make_shared<FCameraComponentDesc>();

	spCameraComponentDesc->m_wpOwnerGameObject = shared_from_this();
	spCameraComponentDesc->m_eComponentType = EComponentType::CAMERA;
	spCameraComponentDesc->m_wstrComponentName = L"BowlingCameraComponent";
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

	spCameraComponentDesc->m_vSphericalCameraAxisOffset = Vector3(0.f, 0.f, 0.f);
	spCameraComponentDesc->m_vSphericalCameraEyeOffset = Vector3(0.f, 0.f, 0.f);
	spCameraComponentDesc->m_fCurSphericalCameraRadius = 5.0f;
	spCameraComponentDesc->m_fMinElevationDegree = 0.0f;
	spCameraComponentDesc->m_fMaxElevationDegree = 60.0f;

	spCameraComponentDesc->m_fCullFrustumNear = 0.1f;
	spCameraComponentDesc->m_fCullFrustumFar = 100.0f;

	shared_ptr<CCameraComponent> spCameraComponent = CCameraComponent::Create(spCameraComponentDesc);
	AddComponent(spCameraComponentDesc->m_wstrComponentName, spCameraComponent);
	m_spCameraComponent = spCameraComponent;

}

HRESULT Client::CBowlingCamera::SetOnAir()
{
	return m_spCameraComponent->SetOnAir();
}
