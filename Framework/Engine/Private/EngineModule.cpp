#include "pch.h"
#include "EngineModule.h"

#include "CameraManager.h"
#include "DeviceManager.h"
#include "InputManager.h"
#include "LightManager.h"
#include "RenderManager.h"
#include "FrameManager.h"
#include "PipelineManager.h"
#include "PhysXManager.h"
#include "ThreadManager.h"
#include "TimerManager.h"
#include "PickManager.h"
#include "EventManager.h"
#include "SoundManager.h"

#include "ShaderHandler.h"
#include "TextureHandler.h"
#include "FontHandler.h"
#include "SoundHandler.h"
#include "ModelHandler.h"
#include "MeshHandler.h"

IMPLEMENT_SINGLETON(CEngineModule)

HRESULT Engine::CEngineModule::Initialize(HWND _hWnd, HINSTANCE _hInst, int32 _iWindowSizeX, int32 _iWindowSizeY)
{
	// 디바이스 매니저를 생성하고 초기화합니다.
	m_spDeviceManager = CDeviceManager::Create<CDeviceManager>();
	m_spDeviceManager->Initialize(_hWnd, _hInst, _iWindowSizeX, _iWindowSizeY);

	// 스레드 매니저를 생성하고 초기화합니다.
	m_spThreadManager = CThreadManager::Create<CThreadManager>();
	m_spThreadManager->Initialize();

	// 피직스 매니저
	m_spPhysXManager = CPhysXManager::Create<CPhysXManager>();
	m_spPhysXManager->Initialize();

	// 핸들러를 생성하고 초기화합니다.
	m_spShaderHandler = CShaderHandler::GetInstance();
	m_spShaderHandler->Initialize();

	m_spTextureHandler = CTextureHandler::GetInstance();
	m_spTextureHandler->Initialize();

	m_spFontHandler = CFontHandler::GetInstance();
	m_spFontHandler->Initialize();

	m_spSoundHandler = CSoundHandler::GetInstance();
	m_spSoundHandler->Initialize();

	m_spModelHandler = CModelHandler::GetInstance();
	m_spModelHandler->Initialize();

	m_spMeshHandler = CMeshHandler::GetInstance();
	m_spMeshHandler->Initialize();

	// 기타 매니저를 생성하고 초기화합니다.
	m_spPipelineManager = CPipelineManager::GetInstance();
	m_spPipelineManager->Initialize();

	// 이벤트 매니저
	m_spEventManager = CEventManager::Create<CEventManager>();
	m_spEventManager->Initialize();

	// 프레임 매니저
	m_spFrameManager = CFrameManager::Create<CFrameManager>();
	m_spFrameManager->Initialize();

	// 타이머 매니저
	m_spTimerManager = CTimerManager::Create<CTimerManager>();
	m_spTimerManager->Initialize();

	// 입력 매니저
	m_spInputManager = CInputManager::Create<CInputManager>();
	m_spInputManager->Initialize();

	// 카메라 매니저
	m_spCameraManager = CCameraManager::Create<CCameraManager>();
	m_spCameraManager->Initialize();

	m_spLightManager = CLightManager::Create<CLightManager>();
	m_spLightManager->Initialize();

	m_spRenderManager = CRenderManager::Create<CRenderManager>();
	m_spRenderManager->Initialize();

	m_spPickManager = CPickManager::Create<CPickManager>();
	m_spPickManager->Initialize(_hWnd, _iWindowSizeX, _iWindowSizeY);

	m_spSoundManager = CSoundManager::Create<CSoundManager>();
	m_spSoundManager->Initialize();

	return S_OK;
}

int32 Engine::CEngineModule::PreTick(float* _pDeltaSeconds)
{
	// 델타 세컨드를 계산합니다.
	m_fSumDeltaSeconds += m_spFrameManager->Tick(EFrameLoopType::OUTER);
	if (m_fSumDeltaSeconds < m_fFrameRate)
	{
		*_pDeltaSeconds = 0.0f;
		return -1;
	}

	m_fSumDeltaSeconds = 0.0f;
	*_pDeltaSeconds = m_spFrameManager->Tick(EFrameLoopType::INNER);

	// 입력 매니저를 업데이트합니다.
	m_spInputManager->Tick(*_pDeltaSeconds);
	m_spTimerManager->Tick(*_pDeltaSeconds);
	return 0;
}

int32 Engine::CEngineModule::Tick(float _fDeltaSeconds)
{
	m_spPhysXManager->Tick(_fDeltaSeconds);
	return 0;
}

int32 Engine::CEngineModule::PostTick(float _fDeltaSeconds)
{
	m_spCameraManager->Tick(_fDeltaSeconds);
	m_spLightManager->Tick();
	m_spRenderManager->Tick(_fDeltaSeconds);
	return 0;
}

void Engine::CEngineModule::PreRender()
{
	m_spRenderManager->Render();
}

void Engine::CEngineModule::PostRender()
{
	m_spDeviceManager->Present();
}

void Engine::CEngineModule::HandleEvent()
{
	m_spEventManager->InvockEvent(EEventInvokeType::FORWARD);
	m_spEventManager->ClearEvent();
}

void Engine::CEngineModule::Release()
{
	// 기타 매니저를 해제합니다.
	m_spSoundManager->Release();
	m_spSoundManager = nullptr;

	m_spEventManager->Release();
	m_spEventManager = nullptr;

	m_spPickManager->Release();
	m_spPickManager = nullptr;

	m_spPhysXManager->Release();
	m_spPhysXManager = nullptr;

	m_spRenderManager->Release();
	m_spRenderManager = nullptr;

	m_spLightManager->Release();
	m_spLightManager = nullptr;

	m_spCameraManager->Release();
	m_spCameraManager = nullptr;

	m_spInputManager->Release();
	m_spInputManager = nullptr;

	m_spTimerManager->Release();
	m_spTimerManager = nullptr;

	m_spFrameManager->Release();
	m_spFrameManager = nullptr;

	m_spPipelineManager->Release();
	m_spPipelineManager = nullptr;

	m_spThreadManager->Release();
	m_spThreadManager = nullptr;

	// 핸들러를 해제합니다.
	m_spSoundHandler->Release();
	m_spSoundHandler = nullptr;

	m_spFontHandler->Release();
	m_spFontHandler = nullptr;

	m_spTextureHandler->Release();
	m_spTextureHandler = nullptr;

	m_spModelHandler->Release();
	m_spModelHandler = nullptr;

	m_spMeshHandler->Release();
	m_spMeshHandler = nullptr;

	m_spShaderHandler->Release();
	m_spShaderHandler = nullptr;

	// 디바이스 매니저를 해제합니다.
	m_spDeviceManager->Release();
	m_spDeviceManager = nullptr;

	// 싱글턴 모듈을 해제합니다.
	m_spInstance = nullptr;
}

void Engine::CEngineModule::LoadResources()
{
	CFontHandler::GetInstance()->LoadFontFromFile();
	CShaderHandler::GetInstance()->LoadEffectShaderFromFile();
	CSoundHandler::GetInstance()->LoadSoundFromFile();
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::UI);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::DIFFUSE);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::NOISE);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::MASK);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::DISTORTION);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::DISSOLVE);
	CModelHandler::GetInstance()->LoadEffectMeshPaths();

	/* 24-04-07 이동준 추가 */
	//CModelHandler::GetInstance()->LoadGridLandscapeMeshFromFile();
	//CModelHandler::GetInstance()->LoadGridEnvMeshFromFile();
	//CModelHandler::GetInstance()->LoadGridEnvCollisionFromFile();
	//CModelHandler::GetInstance()->LoadPlaceableTreeMeshFromFile();
	//CModelHandler::GetInstance()->LoadPlaceableRockMeshFromFile();
	//CModelHandler::GetInstance()->LoadPlaceableWoodMeshFromFile();
	//CModelHandler::GetInstance()->LoadPlaceableStoneMeshFromFile();
	//CModelHandler::GetInstance()->LoadPlaceablePalMeshFromFile();
	//CModelHandler::GetInstance()->LoadPlaceableArchitectureMeshFromFile();
	//CModelHandler::GetInstance()->LoadPlaceableLightMeshFromFile();
	//CModelHandler::GetInstance()->LoadPlaceableCameraMeshFromFile();
}
