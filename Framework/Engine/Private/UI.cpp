#include "pch.h"
#include "UI.h"

#include "EngineModule.h"
#include "CameraManager.h"
HRESULT Engine::CUI::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// game object desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// ui desc
	
	// Ÿ�� ���ο� ���� ������ ������ �����մϴ�. 
	switch (m_eProjectionType)
	{
	// ������ ���õ� ���� �����մϴ�.
	case Engine::EProjectionType::ORTHOGONAL:
		break;

	// ���ٰ� ���õ� ���� �����մϴ�.
	case Engine::EProjectionType::PERSPECTIVE:
		break;
	}

	return S_OK;
}

HRESULT Engine::CUI::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CUI::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	// Ÿ�� ���ο� ���� ������ ������ �����մϴ�. 
	switch (m_eProjectionType)
	{
		// ������ ���õ� ���� �����մϴ�.
	case Engine::EProjectionType::ORTHOGONAL:
		m_matView = Matrix::Identity;
		m_matProjection = CEngineModule::GetInstance()->GetCameraManager()->GetCurOrthogonalProjectionMatrix();
		break;
		// ���ٰ� ���õ� ���� �����մϴ�.
	case Engine::EProjectionType::PERSPECTIVE:
		m_matView = CEngineModule::GetInstance()->GetCameraManager()->GetCurViewMatrix();
		m_matProjection = CEngineModule::GetInstance()->GetCameraManager()->GetCurPerspectiveProjectionMatrix();
		break;
	}

	return 0;
}

int32 Engine::CUI::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CUI::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CUI::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CUI::Release()
{
	CGameObject::Release();
}
