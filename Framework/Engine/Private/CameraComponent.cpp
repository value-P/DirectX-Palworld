// default
#include "pch.h"
#include "CameraComponent.h"

// module
#include "EngineModule.h"

// manager
#include "MathUtility.h"
#include "CameraManager.h"
#include "InputManager.h"

// GameObject
#include "GameObject.h"

// world
#include "World.h"

std::shared_ptr<CCameraComponent> Engine::CCameraComponent::Create(const std::shared_ptr<FCameraComponentDesc>& _spCameraComponentDesc)
{
	const std::shared_ptr<CCameraComponent> spCameraComponent = make_shared<CCameraComponent>();
	ENSUREF(spCameraComponent->InitializeComponent(_spCameraComponentDesc), L"Failed to initialize camera component");
	return spCameraComponent;
}

HRESULT Engine::CCameraComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component desc
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// camera component desc
	shared_ptr<FCameraComponentDesc> spCameraComponentDesc = static_pointer_cast<FCameraComponentDesc>(_spComponentDesc);
	m_eCameraType = spCameraComponentDesc->m_eCameraType;
	m_bMouseControl = spCameraComponentDesc->m_bMouseControl;
	m_bKeyboardControl = spCameraComponentDesc->m_bKeyboardControl;
	m_fMouseSensitivity = spCameraComponentDesc->m_fMouseSensitivity;
	m_fKeyboardSensitivity = spCameraComponentDesc->m_fKeyboardSensitivity;

	m_eProjectionType = spCameraComponentDesc->m_eProjectionType;
	m_fAspectRatio = spCameraComponentDesc->m_fAspectRatio;
	m_fCurFieldOfView = spCameraComponentDesc->m_fFieldOfView;
	m_fFarPlane = spCameraComponentDesc->m_fFarPlane;
	m_fNearPlane = spCameraComponentDesc->m_fNearPlane;

	m_vSphericalCameraAxisOffset = spCameraComponentDesc->m_vSphericalCameraAxisOffset;
	m_vSphericalCameraEyeOffset = spCameraComponentDesc->m_vSphericalCameraEyeOffset;
	m_fCurSphericalCameraRadius = spCameraComponentDesc->m_fCurSphericalCameraRadius;
	m_fMinElevationDegree = spCameraComponentDesc->m_fMinElevationDegree;
	m_fMaxElevationDegree = spCameraComponentDesc->m_fMaxElevationDegree;

	m_tCameraCullFrustum.Near = spCameraComponentDesc->m_fCullFrustumNear;
	m_tCameraCullFrustum.Far = spCameraComponentDesc->m_fCullFrustumFar;

	// 준석; 절두체 컬링을 확인하기 위해 좌우 기울기를 늘려놓았습니다.
	m_tCameraCullFrustum.TopSlope		= spCameraComponentDesc->m_fCullFrustumSlops.x;
	m_tCameraCullFrustum.BottomSlope	= spCameraComponentDesc->m_fCullFrustumSlops.y;
	m_tCameraCullFrustum.LeftSlope		= spCameraComponentDesc->m_fCullFrustumSlops.z;
	m_tCameraCullFrustum.RightSlope		= spCameraComponentDesc->m_fCullFrustumSlops.w;

	m_tCameraCullSphere.Center = Vector3(0.0f, 0.0f, 0.0f);
	m_tCameraCullSphere.Radius = spCameraComponentDesc->m_fCullSphereRadius;

	// etc
	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();

	// 카메라 매니저에 카메라 컴포넌트를 추가합니다.
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();
	m_wpCameraManager.lock()->AddCameraComponent(m_wstrComponentName, dynamic_pointer_cast<CCameraComponent>(shared_from_this()));
	return S_OK;
}

HRESULT Engine::CCameraComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CCameraComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (m_eCameraType)
	{
	case ECameraType::DEFAULT:
	{
		// 마우스를 입력받는 경우
		if (m_bMouseControl)
		{
			if (int32 iDeltaX = m_wpInputManager.lock()->GetMouseDelta(EMouseAxisType::X_AXIS))
			{
				m_wpOwnerGameObject.lock()->Turn(Vector3::UnitY, iDeltaX * m_fMouseSensitivity);
			}

			if (int32 iDeltaY = m_wpInputManager.lock()->GetMouseDelta(EMouseAxisType::Y_AXIS))
			{
				Vector3 vRightVector = m_wpOwnerGameObject.lock()->GetRightVector().value();
				m_wpOwnerGameObject.lock()->Turn(vRightVector, iDeltaY * m_fMouseSensitivity);
			}
		}

		// 키보드를 입력받는 경우
		if (m_bKeyboardControl)
		{
			if (m_wpInputManager.lock()->IsKeyPressed(DIK_W)) { m_wpOwnerGameObject.lock()->MoveForward(m_fKeyboardSensitivity); }

			if (m_wpInputManager.lock()->IsKeyPressed(DIK_S)) { m_wpOwnerGameObject.lock()->MoveBackward(m_fKeyboardSensitivity); }

			if (m_wpInputManager.lock()->IsKeyPressed(DIK_D)) { m_wpOwnerGameObject.lock()->MoveRight(m_fKeyboardSensitivity); }

			if (m_wpInputManager.lock()->IsKeyPressed(DIK_A)) { m_wpOwnerGameObject.lock()->MoveLeft(m_fKeyboardSensitivity); }

			if (m_wpInputManager.lock()->IsKeyPressed(DIK_E)) { m_wpOwnerGameObject.lock()->MoveUp(m_fKeyboardSensitivity); }

			if (m_wpInputManager.lock()->IsKeyPressed(DIK_Q)) { m_wpOwnerGameObject.lock()->MoveDown(m_fKeyboardSensitivity); }
		}
	}
	break;

	case ECameraType::SPHERICAL:
	{
		// 마우스를 입력받지 않는 경우
		if (!m_bMouseControl) { return 0; }

		// 방위각을 갱신합니다.
		if (int32 iDeltaX = m_wpInputManager.lock()->GetMouseDelta(EMouseAxisType::X_AXIS))
		{
			m_fCurAzimuthRadian += XMConvertToRadians(iDeltaX * m_fMouseSensitivity * _fDeltaSeconds);

			if (m_fCurAzimuthRadian > 2.f * (float)M_PI)
			{
				m_fCurAzimuthRadian -= 2.f * (float)M_PI;
			}
			else if (m_fCurAzimuthRadian < 0.f)
			{
				m_fCurAzimuthRadian += 2.f * (float)M_PI;
			}
		}

		// 앙각을 갱신합니다.
		if (int32 iDeltaY = m_wpInputManager.lock()->GetMouseDelta(EMouseAxisType::Y_AXIS))
		{
			m_fCurElevationRadian += XMConvertToRadians(iDeltaY * m_fMouseSensitivity * _fDeltaSeconds);
			m_fCurElevationRadian = std::clamp(m_fCurElevationRadian, XMConvertToRadians(m_fMinElevationDegree), XMConvertToRadians(m_fMaxElevationDegree));
		}
	}
	break;

	default:
		break;
	}

	return 0;
}

int32 Engine::CCameraComponent::TickComponent(float _fDeltaSeconds)
{
	PreRecalcCameraMatrix();

	PlayShake(_fDeltaSeconds);

	return 0;
}

int32 Engine::CCameraComponent::PostTickComponent(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Engine::CCameraComponent::EndPlay()
{
	return S_OK;
}

void Engine::CCameraComponent::Render(int32 _iShaderPath)
{
	return;
}

void Engine::CCameraComponent::Release()
{
	return;
}

HRESULT Engine::CCameraComponent::BindVariable()
{
	return S_OK;
}

HRESULT Engine::CCameraComponent::SetOnAir()
{
	return m_wpCameraManager.lock()->SetCurCamera(m_wstrComponentName);
}

Vector3 Engine::CCameraComponent::RecalcCartesian()
{
	float fX = m_fCurSphericalCameraRadius * cosf(m_fCurElevationRadian) * sinf(m_fCurAzimuthRadian);
	float fY = m_fCurSphericalCameraRadius * sinf(m_fCurElevationRadian);
	float fZ = m_fCurSphericalCameraRadius * cosf(m_fCurElevationRadian) * cosf(m_fCurAzimuthRadian);
	return Vector3(fX, fY, fZ);
}

void Engine::CCameraComponent::PlayShake(float _fDeltaSeconds)
{
	m_bUseShake = false;
	m_bUseFovShake = false;

	m_matShakeRotX = Matrix::Identity;
	m_matShakeRotY = Matrix::Identity;
	m_matShakeRotZ = Matrix::Identity;
	m_vShakeXYZfov = Vector4(0.f);

	for (size_t i = 0; i < m_arrShakeData.size(); ++i)
	{
		if (m_arrShakeData[i].m_bUseShake)
		{
			m_arrShakeData[i].m_fShakeCheck += _fDeltaSeconds;

			if (m_arrShakeData[i].m_fShakeCheck >= m_arrShakeData[i].m_fShakeTime)
			{
				m_arrShakeData[i].m_bUseShake = false;
				m_arrShakeData[i].m_fShakeCheck = 0.f;
				continue;
			}

			m_bUseShake = true;

			EShakeType eType = (EShakeType)i;

			int32 iRandNum = CMathUtility::GetRandomInt(0, 1);
			float fShakeAmount = CMathUtility::GetRandomFloat(m_arrShakeData[i].m_fMinShakePower* 0.5f, m_arrShakeData[i].m_fShakePower * 0.5f);

			fShakeAmount = iRandNum == 0 ? -1.f * fShakeAmount : fShakeAmount;

			switch (eType)
			{
			case Engine::EShakeType::SHAKE_ROTX:
				m_matShakeRotX = Matrix::CreateRotationX(XMConvertToRadians(fShakeAmount));
				break;
			case Engine::EShakeType::SHAKE_ROTY:
				m_matShakeRotY = Matrix::CreateRotationY(XMConvertToRadians(fShakeAmount));
				break;
			case Engine::EShakeType::SHAKE_ROTZ:
				m_matShakeRotZ = Matrix::CreateRotationZ(XMConvertToRadians(fShakeAmount));
				break;
			case Engine::EShakeType::SHAKE_RIGHT:
				m_vShakeXYZfov.x = fShakeAmount;
				break;
			case Engine::EShakeType::SHAKE_UP:
				m_vShakeXYZfov.y = fShakeAmount;
				break;
			case Engine::EShakeType::SHAKE_LOOK:
				m_vShakeXYZfov.z = fShakeAmount;
				break;
			case Engine::EShakeType::SHAKE_FOV:
				m_bUseFovShake = true;
				m_vShakeXYZfov.w = fShakeAmount;
				break;
			default:
				break;
			}
		}
	}
}

Matrix Engine::CCameraComponent::ShakeMatrix(const Matrix& _matCamWorld)
{
	Matrix matLocal = m_matShakeRotZ * m_matShakeRotY* m_matShakeRotX;
	matLocal.Translation(Vector3(m_vShakeXYZfov.x, m_vShakeXYZfov.y, m_vShakeXYZfov.z));
	
	return matLocal * _matCamWorld;
}

void Engine::CCameraComponent::PreRecalcCameraMatrix()
{
	switch (m_eCameraType)
	{
	case Engine::ECameraType::SPHERICAL:
	{
		// 카메라 중심을 갱신합니다.
		Vector3 vOwnerRight = m_wpOwnerGameObject.lock()->GetRightVector().value();
		Vector3 vOwnerUp = m_wpOwnerGameObject.lock()->GetUpVector().value();
		Vector3 vOwnerLook = m_wpOwnerGameObject.lock()->GetForwardVector().value();

		m_vSphericalCameraCenter = m_wpOwnerGameObject.lock()->GetPosition().value() +
			vOwnerRight * m_vSphericalCameraAxisOffset.x + vOwnerUp * m_vSphericalCameraAxisOffset.y + vOwnerLook * m_vSphericalCameraAxisOffset.z;

		// 카메라의 포워드 벡터를 갱신합니다.
		Vector3 vLocalCamPosCartesian = RecalcCartesian();
		m_vSphericalCameraForward = -vLocalCamPosCartesian;
		m_vSphericalCameraForward.Normalize();

		// 카메라의 라이트 벡터를 구합니다.
		m_vSphericalCameraRight = Vector3::Up.Cross(m_vSphericalCameraForward);
		m_vSphericalCameraRight.Normalize();
		Vector3 vEyeUp = m_vSphericalCameraForward.Cross(m_vSphericalCameraRight);
		vEyeUp.Normalize();

		// 구면좌표계상에서 SphericalCameraEyeOffset만큼 떨어진 위치로 Eye를 갱신합니다
		m_vSphericalCameraCartesian = m_vSphericalCameraCenter + vLocalCamPosCartesian +
			m_vSphericalCameraRight * m_vSphericalCameraEyeOffset.x + vEyeUp * m_vSphericalCameraEyeOffset.y + m_vSphericalCameraForward * m_vSphericalCameraEyeOffset.z;
	}
	break;

	default:
		break;
	}

}

void Engine::CCameraComponent::RecalcCameraMatrix()
{
	switch (m_eCameraType)
	{
	case Engine::ECameraType::DEFAULT:
	{
		// 뷰 행렬을 구합니다.
		Vector3 vCameraForward = m_wpOwnerGameObject.lock()->GetForwardVector().value();
		Vector3 vCameraPosition = m_wpOwnerGameObject.lock()->GetPosition().value();
		
		m_matView = XMMatrixLookToLH(vCameraPosition, vCameraForward, Vector3::Up);
	}
	break;

	case Engine::ECameraType::SPHERICAL:
	{
		// 뷰 행렬을 구합니다.
		m_matView = XMMatrixLookToLH(m_vSphericalCameraCartesian, m_vSphericalCameraForward, Vector3::Up);
	}
	break;

	default:
		CHECKF(false, L"Camera type is not designated");
		break;
	}

	if (m_bUseShake)
	{
		Matrix m_matShaked = ShakeMatrix(m_matView.Invert());
		m_matView = m_matShaked.Invert();
	}

	// 원근 투영 행렬을 구합니다.
	if (m_bUseFovShake)
	{
		m_matPerspectiveProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fCurFieldOfView + m_vShakeXYZfov.w), m_fAspectRatio, m_fFarPlane, m_fNearPlane);
	}
	else
	{
		m_matPerspectiveProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fCurFieldOfView), m_fAspectRatio, m_fFarPlane, m_fNearPlane);
	}

	// 직교 투영 행렬을 구합니다.
	m_matOrthogonalProjection = XMMatrixOrthographicLH(1280.0f, 720.0f, 0.1f, 1000.0f);
}

void Engine::CCameraComponent::RecalcCameraFrustum()
{
	Matrix matWorld = m_matView.Invert();
	Vector3 vPosition = Vector3::Zero;
	Vector3 vScale = Vector3::One;
	Quaternion vQuaternion = Quaternion::Identity;

	matWorld.Decompose(vScale, vQuaternion, vPosition);
	m_tCameraCullFrustum.Origin = vPosition;
	m_tCameraCullFrustum.Orientation = vQuaternion;

	m_tCameraCullSphere.Center = m_wpOwnerGameObject.lock()->GetPosition().value();
}

void Engine::CCameraComponent::ShakeCamera(EShakeType _eShakeType, float _fMinPower, float _fMaxPower, float _fTime)
{
	m_arrShakeData[(int32)_eShakeType].m_bUseShake = true;
	m_arrShakeData[(int32)_eShakeType].m_fShakeCheck = 0.f;
	m_arrShakeData[(int32)_eShakeType].m_fMinShakePower = _fMinPower;
	m_arrShakeData[(int32)_eShakeType].m_fShakePower = _fMaxPower;
	m_arrShakeData[(int32)_eShakeType].m_fShakeTime = _fTime;
}