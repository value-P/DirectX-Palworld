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
	// ����̽� �Ŵ����� �����ϰ� �ʱ�ȭ�մϴ�.
	m_spDeviceManager = CDeviceManager::Create<CDeviceManager>();
	m_spDeviceManager->Initialize(_hWnd, _hInst, _iWindowSizeX, _iWindowSizeY);

	// ������ �Ŵ����� �����ϰ� �ʱ�ȭ�մϴ�.
	m_spThreadManager = CThreadManager::Create<CThreadManager>();
	m_spThreadManager->Initialize();

	// ������ �Ŵ���
	m_spPhysXManager = CPhysXManager::Create<CPhysXManager>();
	m_spPhysXManager->Initialize();

	// �ڵ鷯�� �����ϰ� �ʱ�ȭ�մϴ�.
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

	// ��Ÿ �Ŵ����� �����ϰ� �ʱ�ȭ�մϴ�.
	m_spPipelineManager = CPipelineManager::GetInstance();
	m_spPipelineManager->Initialize();

	// �̺�Ʈ �Ŵ���
	m_spEventManager = CEventManager::Create<CEventManager>();
	m_spEventManager->Initialize();

	// ������ �Ŵ���
	m_spFrameManager = CFrameManager::Create<CFrameManager>();
	m_spFrameManager->Initialize();

	// Ÿ�̸� �Ŵ���
	m_spTimerManager = CTimerManager::Create<CTimerManager>();
	m_spTimerManager->Initialize();

	// �Է� �Ŵ���
	m_spInputManager = CInputManager::Create<CInputManager>();
	m_spInputManager->Initialize();

	// ī�޶� �Ŵ���
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
	// ��Ÿ �����带 ����մϴ�.
	m_fSumDeltaSeconds += m_spFrameManager->Tick(EFrameLoopType::OUTER);
	if (m_fSumDeltaSeconds < m_fFrameRate)
	{
		*_pDeltaSeconds = 0.0f;
		return -1;
	}

	m_fSumDeltaSeconds = 0.0f;
	*_pDeltaSeconds = m_spFrameManager->Tick(EFrameLoopType::INNER);

	// �Է� �Ŵ����� ������Ʈ�մϴ�.
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
	// ��Ÿ �Ŵ����� �����մϴ�.
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

	// �ڵ鷯�� �����մϴ�.
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

	// ����̽� �Ŵ����� �����մϴ�.
	m_spDeviceManager->Release();
	m_spDeviceManager = nullptr;

	// �̱��� ����� �����մϴ�.
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

	/* 24-04-07 �̵��� �߰� */
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
