// default
#include "pch.h"
#include "CameraManager.h"

// component
#include "CameraComponent.h"

HRESULT Engine::CCameraManager::Initialize()
{
	return S_OK;
}

int32 Engine::CCameraManager::Tick(float _fDeltaSeconds)
{
	if (m_wpCurCameraComponent.expired()) { return 0; }

	m_wpCurCameraComponent.lock()->RecalcCameraFrustum();
	m_wpCurCameraComponent.lock()->RecalcCameraMatrix();

	m_matCurView = m_wpCurCameraComponent.lock()->GetViewMatrix();
	m_matCurPerspectiveProjection = m_wpCurCameraComponent.lock()->GetPerspectiveProjectionMatrix();
	m_matCurOrthogonalProjection = m_wpCurCameraComponent.lock()->GetOrthogonalProjectionMatrix();

	m_tFrustum = m_wpCurCameraComponent.lock()->GetCameraFrustum();
	m_tSphere = m_wpCurCameraComponent.lock()->GetCameraCullSphere();
	return 0;
}

void Engine::CCameraManager::Release()
{
	CManager::Release();
}

HRESULT Engine::CCameraManager::SetCurCamera(const std::wstring& _wstrCameraComponentName)
{
	if (m_umapCameraComponents.find(_wstrCameraComponentName) == m_umapCameraComponents.end()) { return E_FAIL; }

	m_wpCurCameraComponent = m_umapCameraComponents[_wstrCameraComponentName];
	return S_OK;
}

Vector3 Engine::CCameraManager::GetCurCameraWorldPosition() const
{
	Matrix matWorld = m_matCurView.Invert();
	return matWorld.Translation();
}

Vector3 Engine::CCameraManager::GetCurCameraWorldForwardVector() const
{
	Matrix matWorld = m_matCurView.Invert();
	Vector3 vForward = -matWorld.Forward();
	vForward.Normalize();
	return vForward;
}

Vector3 Engine::CCameraManager::GetCurCameraWorldUpVector() const
{
	Matrix matWorld = m_matCurView.Invert();
	Vector3 vUp = matWorld.Up();
	vUp.Normalize();
	return vUp;
}

const FCameraInfo& Engine::CCameraManager::GetCurCameraInfo()
{
	m_tCameraInfo.matView = m_matCurView.Transpose();
	m_tCameraInfo.matProj = m_matCurPerspectiveProjection.Transpose();
	m_tCameraInfo.vCameraPosition = GetCurCameraWorldPosition();
	return m_tCameraInfo;
}

Vector3 Engine::CCameraManager::GetCurCameraWorldRightVector() const
{
	Matrix matWorld = m_matCurView.Invert();
	Vector3 vRight = matWorld.Right();
	vRight.Normalize();
	return vRight;
}

HRESULT Engine::CCameraManager::AddCameraComponent(const std::wstring& _wstrCameraComponentName, const std::shared_ptr<CCameraComponent>& _spCameraComponent)
{
	if (m_umapCameraComponents.find(_wstrCameraComponentName) != m_umapCameraComponents.end()) { return E_FAIL; }

	m_umapCameraComponents.emplace(_wstrCameraComponentName, _spCameraComponent);
	return S_OK;
}

HRESULT Engine::CCameraManager::RemoveCameraComponent(const std::wstring& _wstrCameraComponentName)
{
	if (m_umapCameraComponents.find(_wstrCameraComponentName) == m_umapCameraComponents.end()) { return E_FAIL; }

	m_umapCameraComponents.erase(_wstrCameraComponentName);
	return S_OK;
}

std::shared_ptr<CCameraComponent> Engine::CCameraManager::FindCameraComponent(const std::wstring& _wstrCameraComponentName)
{
	if (!m_umapCameraComponents.contains(_wstrCameraComponentName)) { return nullptr; }

	return m_umapCameraComponents[_wstrCameraComponentName].lock();
}

HRESULT Engine::CCameraManager::ClearCameraManager()
{
	m_umapCameraComponents.clear();
	return S_OK;
}
