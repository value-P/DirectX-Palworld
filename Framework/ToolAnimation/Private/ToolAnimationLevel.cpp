#include "pch.h"
#include "ToolAnimationLevel.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "CameraComponent.h"

#include "Camera.h"

HRESULT ToolAnimation::CToolAnimationLevel::Initialize()
{
	// light
	{
		FDirectionalLight tDirLight;

		tDirLight.vDirection = Vector3(0.0f, -1.0f, 1.0f);
		tDirLight.vDirection.Normalize();
		tDirLight.vColor = Vector3(1, 1, 1);
		tDirLight.fIntensity = 1;

		CEngineModule::GetInstance()->GetLightManager()->AddLight(L"TestLight", make_shared<FDirectionalLight>(tDirLight), true);
	}

	// CEngineModule::GetInstance()->GetLightManager()->AddLight(L"TestLight", m_tDirLight);
	CEngineModule::GetInstance()->GetLightManager()->SetActiveLight(L"TestLight", ELightType::DIRECTIONAL, true);

	FCameraDesc tCameraObjectDesc;
	tCameraObjectDesc.m_vInitPosition = Vector3(0.f, 1.f, 5.f);
	tCameraObjectDesc.m_vInitRotate = Vector3(0.f, 180.f, 0.f);
	m_spCamera = CCamera::Create(make_shared<FCameraDesc>(tCameraObjectDesc));
	CHECKF(m_spCamera, L"SampleLevel : Initialize Failed");
	AddGameObject(m_spCamera);
	m_spCamera->SetKeyboardSensitivity(10.f);

	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();

	return CLevel::Initialize();
}

HRESULT ToolAnimation::CToolAnimationLevel::BeginPlay()
{
	ENSURE(m_spCamera->SetOnAir());
	m_spCamera->LockCameraViewDirection(true);
	return CLevel::BeginPlay();
}

int32 ToolAnimation::CToolAnimationLevel::PreTick(float _fDeltaSeconds)
{
	LockCameraViewDir();

	return CLevel::PreTick(_fDeltaSeconds);
}

int32 ToolAnimation::CToolAnimationLevel::Tick(float _fDeltaSeconds)
{
	return CLevel::Tick(_fDeltaSeconds);
}

int32 ToolAnimation::CToolAnimationLevel::PostTick(float _fDeltaSeconds)
{
	return CLevel::PostTick(_fDeltaSeconds);
}

void ToolAnimation::CToolAnimationLevel::Release()
{
	if (m_spCamera) { m_spCamera = nullptr; }

	CLevel::Release();
}

HRESULT ToolAnimation::CToolAnimationLevel::LockCameraViewDir()
{
	// Lock Camera
	if (m_wpInputManager.lock()->IsKeyDown(DIK_F1))
	{
		m_bLockCameraViewDir = !m_bLockCameraViewDir;
		m_spCamera->LockCameraViewDirection(m_bLockCameraViewDir);
	}

	if (m_wpInputManager.lock()->IsKeyDown(DIK_GRAVE))
	{
		m_bLockKeyBoardInput = !m_bLockKeyBoardInput;
		CEngineModule::GetInstance()->GetCameraManager()->GetCurCamera()->SetKeybardControl(m_bLockKeyBoardInput);
	}

	return S_OK;
}
