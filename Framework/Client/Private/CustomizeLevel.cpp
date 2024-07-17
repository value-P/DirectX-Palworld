#include "pch.h"
#include "CustomizeLevel.h"

#include "World.h"
#include "ClientModule.h"
#include "UIManager.h"
#include "LightManager.h"
#include "EngineModule.h"
#include "ClientLoop.h"
#include "InputManager.h"

#include "CustomizeObject.h"
#include "Panel.h"
#include "ButtonComponent.h"
#include "Camera.h"
#include "Player.h"

HRESULT Client::CCustomizeLevel::Initialize()
{
	m_wpUIManager = CUIManager::GetInstance();
	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();

	// customize level
	{
		m_iLevelType = static_cast<int32>(ELevelType::CUSTOMIZE);
		SetLevelName(L"CustomizeLevel");
	}

	// light
	{
		FDirectionalLight tDirLight;

		tDirLight.vDirection = Vector3(0.0f, -1.0f, 1.0f);
		tDirLight.vDirection.Normalize();
		tDirLight.vColor = Vector3(1, 1, 1);
		tDirLight.fIntensity = 1;

		CEngineModule::GetInstance()->GetLightManager()->AddLight(L"TestLight", make_shared<FDirectionalLight>(tDirLight), true);
	}

	// camera
	{
		FCameraDesc tCameraDesc;
		
		tCameraDesc.m_vInitPosition = Vector3(0.f, 1.f, 2.5f);
		tCameraDesc.m_bUseMouseInput = false;
		tCameraDesc.m_bUseKeyInput = false;

		m_spCamera = CCamera::Create(make_shared<FCameraDesc>(tCameraDesc));
		m_spCamera->SetOnAir();
		m_spCamera->LookAt(Vector3(0.f,1.f,0.f));
		AddGameObject(m_spCamera);
	}

	// UI
	{
		m_spCustomizeBackgroundUI = m_wpUIManager.lock()->FindParentsPanel(L"CharacterCreationUI");
		m_spCustomizeBackgroundUI->SetActive(true);
		m_wpUIManager.lock()->SetTopPanel(m_spCustomizeBackgroundUI);
		m_wpGameStartButton = m_spCustomizeBackgroundUI->FindComponent<CButtonComponent>(L"GameStartButton");
		m_wpGameStartButton.lock()->AddButtonClickEvent([&]() { StartGameLevel(); });
	}

	// CustomObject
	{
		FCustomizeObjectDesc tCustomObjectDesc;

		tCustomObjectDesc.m_spCustomizeBackgroundUI = m_spCustomizeBackgroundUI;
		tCustomObjectDesc.m_spCamera = m_spCamera;

		m_spCustomObject = CCustomizeObject::Create(make_shared<FCustomizeObjectDesc>(tCustomObjectDesc));
		AddGameObject(m_spCustomObject);
	}

	return CLevel::Initialize();
}

HRESULT Client::CCustomizeLevel::BeginPlay()
{
	if (FAILED(CLevel::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CCustomizeLevel::PreTick(float _fDeltaSeconds)
{
	switch (CLevel::PreTick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

int32 Client::CCustomizeLevel::Tick(float _fDeltaSeconds)
{
	switch (CLevel::Tick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	RotatePlayer();

	return 0;
}

int32 Client::CCustomizeLevel::PostTick(float _fDeltaSeconds)
{
	switch (CLevel::PostTick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

void Client::CCustomizeLevel::Release()
{
	SharedNull(m_spCamera);
	SharedNull(m_spCustomObject);
	SharedNull(m_spCustomizeBackgroundUI);

	CLevel::Release();
}

void Client::CCustomizeLevel::StartGameLevel()
{
	CClientLoop::s_spClientDataSheet->k_tCustomData = m_spCustomObject->GetCustomObjectData();

	m_spCustomizeBackgroundUI->SetActive(false);

	CEngineModule::GetInstance()->GetLightManager()->SetActiveLight(L"CustomizeLevelLight", ELightType::DIRECTIONAL, false);

	m_wpUIManager.lock()->ResetTopPanel();

	cerr << "섬 레벨로 전환합니다.\n";
	CWorld::GetInstance()->LoadLevel(static_cast<int32>(ELevelType::ISLAND));
}

void Client::CCustomizeLevel::RotatePlayer()
{
	if (m_wpInputManager.lock()->IsMousePressed(EMouseActionType::RIGHT_BUTTON))
	{
		float fMove = (float)m_wpInputManager.lock()->GetMouseDelta(EMouseAxisType::X_AXIS);

		m_spCustomObject->SetRotateAccumulated(Vector3(0.f, 1.f, 0.f), -fMove);
	}
}