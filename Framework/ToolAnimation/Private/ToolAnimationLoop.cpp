#include "pch.h"
#include "ToolAnimationLoop.h"

#include "EngineModule.h"
#include "World.h"
#include "PipelineManager.h"

#include "ImguiMgr.h"
#include "ToolAnimationLevel.h"
#include "ShaderHandler.h"
#include "ModelHandler.h"
#include "TextureHandler.h"
#include "ModelBlackBoard.h"
#include "EffectManager.h"
#include "SoundHandler.h"

HRESULT ToolAnimation::ToolAnimationLoop::Initialize()
{
	// ���� ����� �����ϰ� �ʱ�ȭ�մϴ�.
	m_spEngineModule = CEngineModule::GetInstance();
	m_spEngineModule->Initialize(g_hWnd, g_hInst, g_iWindowSizeX, g_iWindowSizeY);

	// �ڵ鷯 �ε� ����
	CShaderHandler::GetInstance()->LoadEffectShaderFromFile();
	CModelHandler::GetInstance()->LoadEffectMeshPaths();
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::DIFFUSE);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::NOISE);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::MASK);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::DISTORTION);
	CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::DISSOLVE);
	CSoundHandler::GetInstance()->LoadSoundFromFile();

	// CTextureHandler::GetInstance()->LoadShaderResourceViewFromFile(ETextureType::MODEL);

	// ���带 �����ϰ� �ʱ�ȭ�մϴ�.
	shared_ptr<CLevel> spLevel = make_shared<CToolAnimationLevel>();
	m_spWorld = CWorld::GetInstance();
	m_spWorld->Initialize(nullptr, spLevel, nullptr);

	// ModelBlackBoard�� ���� �ϰ� �ε� �� �����͸� �ְ�޴µ� ����Ѵ�
	m_spModelBlackBoard = CModelBlackBoard::GetInstance();
	m_spModelBlackBoard->SetCurrentLevel(spLevel);
	m_spModelBlackBoard->Initialize();

	// �Ŵ��� �ʱ�ȭ
	m_spImguiMgr = CImguiMgr::GetInstance();
	m_spImguiMgr->SetCurrentLevel(spLevel);
	m_spImguiMgr->Initialize();

	// ����Ʈ �Ŵ��� �ʱ�ȭ
	m_spEffectMgr = CEffectManager::GetInstance();
	m_spEffectMgr->SetCurrentLevel(spLevel);
	m_spEffectMgr->Initialize();

	return S_OK;
}

HRESULT ToolAnimation::ToolAnimationLoop::BeginPlay()
{
	m_spEffectMgr->BeginPlay();
	m_spWorld->BeginPlay();
	return S_OK;
}

int32 ToolAnimation::ToolAnimationLoop::Tick()
{
	float fDeltaSeconds = 0.0f;
	if (m_spEngineModule->PreTick(&fDeltaSeconds)) { return -1; }
	if (0.1f <= fDeltaSeconds) { fDeltaSeconds = 0.1f; }

	m_spEngineModule->Tick(fDeltaSeconds * g_fTimeScale);
	m_spImguiMgr->Tick(fDeltaSeconds * g_fTimeScale);
	m_spWorld->Tick(fDeltaSeconds * g_fTimeScale);

	m_spEngineModule->PostTick(fDeltaSeconds * g_fTimeScale);

	return 0;
}

void ToolAnimation::ToolAnimationLoop::Render()
{
	m_spEngineModule->PreRender();
	m_spImguiMgr->Render();
	m_spEngineModule->PostRender();
}

void ToolAnimation::ToolAnimationLoop::Release()
{
	if (m_spImguiMgr)
	{
		m_spImguiMgr->Release();
		m_spImguiMgr = nullptr;
	}

	if (m_spEffectMgr)
	{
		m_spEffectMgr->Release();
		m_spEffectMgr = nullptr;
	}

	if (m_spModelBlackBoard)
	{
		m_spModelBlackBoard->Release();
		m_spModelBlackBoard = nullptr;
	}

	if (m_spWorld)
	{
		m_spWorld->Release();
		m_spWorld = nullptr;
	}

	if (m_spEngineModule)
	{
		m_spEngineModule->Release();
		m_spEngineModule = nullptr;
	}
}
