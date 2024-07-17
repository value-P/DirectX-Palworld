// default
#include "pch.h"
#include "TransformComponent.h"

#include "GameObject.h"

#include "World.h"
#include "EngineModule.h"
#include "MathUtility.h"

std::shared_ptr<CTransformComponent> Engine::CTransformComponent::Create(const std::shared_ptr<FTransformComponentDesc>& _spTransformComponentDesc)
{
	const shared_ptr<CTransformComponent> spTransformComponent = make_shared<CTransformComponent>();
	spTransformComponent->InitializeComponent(_spTransformComponentDesc);
	return spTransformComponent;
}

HRESULT Engine::CTransformComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component desc
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// transform component desc
	std::shared_ptr<FTransformComponentDesc> spTransformComponentDesc = static_pointer_cast<FTransformComponentDesc>(_spComponentDesc);
	SetScale(spTransformComponentDesc->m_vLocalScale);

	m_matTransform *= Matrix::CreateFromAxisAngle(Vector3::UnitX, XMConvertToRadians(spTransformComponentDesc->m_vLocalRotate.x));
	m_matTransform *= Matrix::CreateFromAxisAngle(Vector3::UnitY, XMConvertToRadians(spTransformComponentDesc->m_vLocalRotate.y));
	m_matTransform *= Matrix::CreateFromAxisAngle(Vector3::UnitZ, XMConvertToRadians(spTransformComponentDesc->m_vLocalRotate.z));
	
	SetPosition(spTransformComponentDesc->m_vLocalTranslate);
	return S_OK;
}

HRESULT Engine::CTransformComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CTransformComponent::PreTickComponent(float _fDeltaSeconds)
{
	if (!m_bActive) { return 0; }

	m_fDeltaSeconds = _fDeltaSeconds;
	return 0;
}

int32 Engine::CTransformComponent::TickComponent(float _fDeltaSeconds)
{
	if (!m_bActive) { return 0; }

	return 0;
}

int32 Engine::CTransformComponent::PostTickComponent(float _fDeltaSeconds)
{
	if (!m_bActive) { return 0; }

	return 0;
}

HRESULT Engine::CTransformComponent::EndPlay()
{
	return S_OK;
}

void Engine::CTransformComponent::Render(int32 _iShaderPath)
{
	return;
}

void Engine::CTransformComponent::Release()
{
	return;
}

HRESULT Engine::CTransformComponent::BindVariable()
{
	return S_OK;
}

void Engine::CTransformComponent::LookAt(const Vector3& _vPosition)
{
	Vector3 vScale = GetScale();
	Vector3 vPosition = GetPosition();

	Vector3 vForward = _vPosition - vPosition;
	Vector3 vRight = XMVector3Cross(Vector4::UnitY, vForward);
	Vector3 vUp = XMVector3Cross(vForward, vRight);

	SetTransform(ETransformType::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	SetTransform(ETransformType::UP, XMVector3Normalize(vUp) * vScale.y);
	SetTransform(ETransformType::FORWARD, XMVector3Normalize(vForward) * vScale.z);
}

void Engine::CTransformComponent::Turn(const Vector3& _vAxis, float _fAngularSpeed)
{
	if (_vAxis == Vector3::Zero) { return; }

	Vector4 vAxis = CMathUtility::DirVec3ToVec4(_vAxis);
	Matrix matRotate = XMMatrixRotationAxis(vAxis, _fAngularSpeed * m_fDeltaSeconds);

	for (int32 iInnerIndex = (int32)ETransformType::RIGHT; iInnerIndex < (int32)ETransformType::POSITION; ++iInnerIndex)
	{
		ETransformType eTransformType = static_cast<ETransformType>(iInnerIndex);

		Vector4 vDirection = GetTransform(eTransformType);
		SetTransform(eTransformType, XMVector4Transform(vDirection, matRotate));
	}
}

void Engine::CTransformComponent::MoveForward(float _fSpeed)
{
	Vector3 vPosition = GetPosition();
	Vector3 vForward = GetForwardVector();
	vPosition += XMVector3Normalize(vForward) * _fSpeed * m_fDeltaSeconds;

	SetPosition(vPosition);
}

void Engine::CTransformComponent::MoveBackward(float _fSpeed)
{
	Vector3 vPosition = GetPosition();
	Vector3 vForward = GetForwardVector();
	vPosition -= XMVector3Normalize(vForward) * _fSpeed * m_fDeltaSeconds;

	SetPosition(vPosition);
}

void Engine::CTransformComponent::MoveRight(float _fSpeed)
{
	Vector3 vPosition = GetPosition();
	Vector3 vRight = GetRightVector();
	vPosition += XMVector3Normalize(vRight) * _fSpeed * m_fDeltaSeconds;

	SetPosition(vPosition);
}

void Engine::CTransformComponent::MoveLeft(float _fSpeed)
{
	Vector3 vPosition = GetPosition();
	Vector3 vRight = GetRightVector();
	vPosition -= XMVector3Normalize(vRight) * _fSpeed * m_fDeltaSeconds;

	SetPosition(vPosition);
}

void Engine::CTransformComponent::MoveUp(float _fSpeed)
{
	Vector3 vPosition = GetPosition();
	Vector3 vUp = GetUpVector();
	vPosition += XMVector3Normalize(vUp) * _fSpeed * m_fDeltaSeconds;

	SetPosition(vPosition);
}

void Engine::CTransformComponent::MoveDown(float _fSpeed)
{
	Vector3 vPosition = GetPosition();
	Vector3 vUp = GetUpVector();
	vPosition -= XMVector3Normalize(vUp) * _fSpeed * m_fDeltaSeconds;

	SetPosition(vPosition);
}

void Engine::CTransformComponent::SetScale(const Vector3& _vScale)
{
	SetTransform(ETransformType::RIGHT,		XMVector3Normalize(GetTransform(ETransformType::RIGHT))		* _vScale.x);
	SetTransform(ETransformType::UP,		XMVector3Normalize(GetTransform(ETransformType::UP))		* _vScale.y);
	SetTransform(ETransformType::FORWARD,	XMVector3Normalize(GetTransform(ETransformType::FORWARD))	* _vScale.z);
}

void Engine::CTransformComponent::SetRotate(const Vector3& _vAxis, float _fDegree)
{
	if (_vAxis == Vector3::Zero) { return; }

	Vector3 vAxis = _vAxis;
	vAxis.Normalize();

	Vector4 v4Axis = CMathUtility::DirVec3ToVec4(vAxis);

	Matrix matRotate = XMMatrixRotationAxis(v4Axis, XMConvertToRadians(_fDegree));
	Vector3 vScale = GetScale();

	for (size_t iIndex = 0; iIndex < 3; ++iIndex)
	{
		Vector4 vTemp = Vector4::Zero;

		*((float*)&vTemp + iIndex) = 1.0f * *((float*)&vScale + iIndex);

		SetTransform(static_cast<ETransformType>(iIndex), XMVector4Transform(vTemp, matRotate));
	}
}

void Engine::CTransformComponent::SetRotate(const Vector3& _vRotate)
{
	Vector3 vScale = GetScale();

	for (size_t iIndex = 0; iIndex < 3; ++iIndex)
	{
		Vector4 vTemp = Vector4::Zero;

		*((float*)&vTemp + iIndex) = 1.0f * *((float*)&vScale + iIndex);

		SetTransform(static_cast<ETransformType>(iIndex), XMVector4Transform(vTemp, Matrix::Identity));
	}

	Matrix matRotate = XMMatrixRotationAxis(Vector3(1.0f, 0.0f, 0.0f), XMConvertToRadians(_vRotate.x));
	matRotate *= XMMatrixRotationAxis(Vector3(0.0f, 1.0f, 0.0f), XMConvertToRadians(_vRotate.y));
	matRotate *= XMMatrixRotationAxis(Vector3(0.0f, 0.0f, 1.0f), XMConvertToRadians(_vRotate.z));

	for (size_t iIndex = 0; iIndex < 3; ++iIndex)
	{
		Vector4 vTemp = GetTransform((ETransformType)iIndex);

		SetTransform(static_cast<ETransformType>(iIndex), XMVector4Transform(vTemp, matRotate));
	}
}

void Engine::CTransformComponent::SetRotateAccumulated(const Vector3& _vAxis, float _fDegree)
{
	if (_vAxis == Vector3::Zero) { return; }

	Vector3 vAxis = _vAxis;
	vAxis.Normalize();

	Vector4 v4Axis = CMathUtility::DirVec3ToVec4(vAxis);

	Matrix matRotate = XMMatrixRotationAxis(v4Axis, XMConvertToRadians(_fDegree));

	for (size_t iIndex = 0; iIndex < 3; ++iIndex)
	{
		Vector4 vTemp = GetTransform((ETransformType)iIndex);

		SetTransform(static_cast<ETransformType>(iIndex), XMVector4Transform(vTemp, matRotate));
	}
}

void Engine::CTransformComponent::SetPosition(const Vector3& _vTranlate)
{
	Vector4 v4Translate = { _vTranlate.x, _vTranlate.y, _vTranlate.z, 1.0f };
	SetTransform(ETransformType::POSITION, v4Translate);
}

void Engine::CTransformComponent::SetTransformMatrix(const Matrix& _matTransform)
{
	m_matTransform = _matTransform;
}

Vector3 Engine::CTransformComponent::GetScale()
{
	return Vector3
	(
		XMVector3Length(GetTransform(ETransformType::RIGHT)).m128_f32[0],
		XMVector3Length(GetTransform(ETransformType::UP)).m128_f32[0],
		XMVector3Length(GetTransform(ETransformType::FORWARD)).m128_f32[0]
	);
}

Vector3 Engine::CTransformComponent::GetRotate()
{
	Vector3 vScale = Vector3::One;
	Quaternion vQuaternion = Quaternion::Identity;
	Vector3 vPosition = Vector3::Zero;

	m_matTransform.Decompose(vScale, vQuaternion, vPosition);
	Vector3 vRadians = vQuaternion.ToEuler();
	return { XMConvertToDegrees(vRadians.x), XMConvertToDegrees(vRadians.y), XMConvertToDegrees(vRadians.z) };
}

Quaternion Engine::CTransformComponent::GetQuaternion()
{
	Vector3 vScale = Vector3::One;
	Quaternion vQuaternion = Quaternion::Identity;
	Vector3 vPosition = Vector3::Zero;

	m_matTransform.Decompose(vScale, vQuaternion, vPosition);
	return vQuaternion;
}

Vector3 Engine::CTransformComponent::GetPosition()
{
	return m_matTransform.Translation();
}

void Engine::CTransformComponent::SetTransform(ETransformType _eTransformType, const Vector4& _vTransform)
{
	::memcpy(&m_matTransform.m[(int32)_eTransformType], &_vTransform, sizeof(Vector4));
}

Vector4 Engine::CTransformComponent::GetTransform(ETransformType _eTransformType) const
{
	Vector4 vTemp = Vector4::Zero;
	::memcpy(&vTemp, &m_matTransform.m[(int32)_eTransformType], sizeof(Vector4));
	return vTemp;
}
