// default
#include "pch.h"
#include "RenderManager.h"

#include "EngineModule.h"
#include "DeviceManager.h"
#include "TextureHandler.h"
#include "PipelineManager.h"
#include "LightManager.h"
#include "TimerManager.h"
#include "CameraManager.h"
#include "MeshHandler.h"
#ifdef _DEBUG
#include "InputManager.h"
#endif

#include "World.h"
#include "Level.h"
#include "GameObject.h"
#include "Component.h"
#include "RectBuffer.h"
#include "ShaderHandler.h"
#include "EffectShader.h"
#include "TreeNode.h"
#include "CameraComponent.h"
#include "Model.h"
#include "MeshBuffer.h"

// ui 자체 정렬을 위한 추가입니다.
#include "Panel.h"
#include "RectComponent.h"

HRESULT Engine::CRenderManager::Initialize()
{
	// Managers
	m_wpPipelineManager = CEngineModule::GetInstance()->GetPipelineManager();
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();
	m_wpLightManager = CEngineModule::GetInstance()->GetLightManager();
	m_wpTimeManager = CEngineModule::GetInstance()->GetTimerManager();
	m_wpMeshHandler = CMeshHandler::GetInstance();
	m_wpShaderHandler = CShaderHandler::GetInstance();
	m_cpDevice = CEngineModule::GetInstance()->GetDeviceManager()->GetDevice();
	m_cpDeviceContext = CEngineModule::GetInstance()->GetDeviceManager()->GetDeviceContext();

#ifdef _DEBUG
	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();
#endif

	// Buffers
	m_spRectBuffer = CRectBuffer::Create();

	// Binds
	D3D11_VIEWPORT Viewport;
	uint32 iNumViewport = 1;
	m_cpDeviceContext->RSGetViewports(&iNumViewport, &Viewport);

	Matrix matWorld, matView, matProj;
	XMStoreFloat4x4(&matWorld, XMMatrixScaling(Viewport.Width, Viewport.Height, 1.f));
	XMStoreFloat4x4(&matView, XMMatrixIdentity());
	XMStoreFloat4x4(&matProj, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f));

	m_tPBR.matWorld = m_tEffectInfoDeffered.matWorld = matWorld;
	m_tPBR.matView = m_tEffectInfoDeffered.matView = matView;
	m_tPBR.matProj = m_tEffectInfoDeffered.matProj = matProj;

	// SRVs - PBR
	m_vecSRVs_PBR.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::DIFFUSE).Get());
	m_vecSRVs_PBR.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::NORMAL).Get());
	m_vecSRVs_PBR.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::MRSO).Get());
	m_vecSRVs_PBR.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::EMISSION).Get());
	m_vecSRVs_PBR.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::DEPTH).Get());
	m_vecSRVs_PBR.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::WORLDPOSITION).Get());

	// SRVs - Effect
	m_vecSRVs_Effect.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::GLOW).Get());
	m_vecSRVs_Effect.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::BLOOM).Get());
	m_vecSRVs_Effect.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::BLUR_X).Get());
	m_vecSRVs_Effect.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::BLUR_Y).Get());
	m_vecSRVs_Effect.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::BACKBUFFER_COPY).Get());
	m_vecSRVs_Effect.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::GLOW_RESULT).Get());
	m_vecSRVs_Effect.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::DISTORTION).Get());
	m_vecSRVs_Effect.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::MOSAIC).Get());
	m_vecSRVs_Effect.push_back(m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::MOSAIC_RESULT).Get());

	m_spSkyDome = CModel::Create(L"../../Resource/Models/Dat/NonAnim/SkyDome.dat", Matrix::CreateScale(0.00225f), EMeshManageType::UNIQUE, nullptr);

	return S_OK;
}

int32 Engine::CRenderManager::Tick(float _fDeltaSeconds)
{
	if (!m_bActive) return 0;

	if (m_spSkyShader == nullptr)
	{
		m_spSkyShader = m_wpShaderHandler.lock()->FindEffectShader(L"FX_SkyDome_PNT2T");

		// Bind Variable 변수들을 가져옴
		m_cpVariable_Sky = m_spSkyShader->GetRawVariableByName("g_tMeshInfo");
		m_cpVariable_TexturesSky = m_spSkyShader->GetShaderResourceVariableByName("g_Textures");
	}
	if (m_spPbrShader == nullptr)
	{
		m_spPbrShader = m_wpShaderHandler.lock()->FindEffectShader(L"FX_PBR_PT2");

		// Bind Variable 변수들을 가져옴
		m_cpVariable_PBR = m_spPbrShader->GetRawVariableByName("g_tPBR");
		m_cpVariable_TexturesPBR = m_spPbrShader->GetShaderResourceVariableByName("g_Textures");
	}
	if (m_spEffectShader == nullptr)
	{
		m_spEffectShader = m_wpShaderHandler.lock()->FindEffectShader(L"FX_Effect_PT2");

		// Bind Variable 변수들을 가져옴
		m_cpVariable_EffectDeffered = m_spEffectShader->GetRawVariableByName("g_tEffectDeffered");
		m_cpVariable_TexturesEffectDeffered = m_spEffectShader->GetShaderResourceVariableByName("g_Textures");
	}
	if (m_vecSRVs_Effect.back() == nullptr)
	{
		m_vecSRVs_Effect.back() = m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::MOSAIC_RESULT).Get();
	}

	m_tFrameInfo.fDeltaTime = _fDeltaSeconds;
	m_tFrameInfo.fTotalTime = m_wpTimeManager.lock()->GetWorldDeltaSeconds();

#ifdef _DEBUG
	if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPAD0))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::BACKBUFFER;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPAD1))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::DIFFUSE;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPAD2))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::NORMAL;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPADENTER))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::METALIC;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPADPLUS))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::ROUGHNESS;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPADMINUS))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::SPECULAR;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPADSTAR))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::OCCLUSION;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPAD3))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::MRSO;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPAD4))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::EMISSION;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPAD5))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::DEPTH;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPAD6))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::WORLDPOSITION;
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_NUMPAD7))
	{
		m_tFrameInfo.iDebugRenderTarget = (int32)EViewType::OBJECTID;
	}

#endif // _DEBUG

	return 0;
}

void Engine::CRenderManager::Render()
{
	if (!m_bActive) return;
	m_wpMeshHandler.lock()->RefreshInstances();

	m_wpShaderHandler.lock()->BindFrameInfo(m_tFrameInfo);
	m_wpShaderHandler.lock()->BindCameraInfo(m_wpCameraManager.lock()->GetCurCameraInfo());

	m_wpPipelineManager.lock()->ClearPipelineViews();
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::DNMEDW);

	RenderPre();

	// DNMEDW
	RenderNblend();
	RenderNblend_NonCull();
	RenderLblend();
	RenderLblend_NonCull();
	RenderLblend_Add();
	RenderLblend_Add_NonCull();
	RenderLblend_Reverse();
	RenderLblend_Reverse_NonCull();

	// BackBuffer
	RenderPbr();
	RenderNLight();
	RenderNLight_NonCull();
	RenderBlend_NonCull();
	RenderBlend_Add();
	RenderBlend_Add_NonCull();
	RenderBlend_Reverse();
	RenderBlend_Reverse_NonCull();

	RenderGlow();
	RenderBlend();
	// RenderDistortion();

	RenderMosaic();

	RenderUi();
	RenderPost();
	RenderPostEffect();
	RenderPostUI();

#ifdef _DEBUG
	RenderDebug();
#endif
}

void Engine::CRenderManager::Release()
{
	// 렌더할 게임 오브젝트들을 널포인터로 초기화하여 레퍼런스 카운트를 감소시킵니다.
	for (size_t iIndex = 0; iIndex < (uint8)ERenderType::ENUM_END; ++iIndex)
	{
		for (auto& spGameObject : m_arrComponents[iIndex]) { spGameObject = nullptr; }
		m_arrComponents[iIndex].clear();
	}

	m_cpVariable_PBR = nullptr;
	m_cpVariable_TexturesPBR = nullptr;
	m_cpVariable_EffectDeffered = nullptr;
	m_cpVariable_TexturesEffectDeffered = nullptr;

	m_spRectBuffer = nullptr;
	m_spPbrShader = nullptr;
	m_spEffectShader = nullptr;
	m_cpNoiseTexture = nullptr;
}

HRESULT Engine::CRenderManager::AddComponent(const std::shared_ptr<CComponent>& _spComponent, ERenderType _eRenderType)
{
	m_arrComponents[static_cast<uint8>(_eRenderType)].push_back(_spComponent);

	if (ERenderType::RENDER_UI == _eRenderType || ERenderType::RENDER_POST_UI == _eRenderType)
	{
		m_arrComponents[static_cast<uint8>(_eRenderType)].sort([](const shared_ptr<CComponent>& _spComponent1, const shared_ptr<CComponent>& _spComponent2)
			{
				const auto spRectComponent1 = std::static_pointer_cast<CRectComponent>(_spComponent1);
				const auto spRectComponent2 = std::static_pointer_cast<CRectComponent>(_spComponent2);

				if (static_pointer_cast<CPanel>(_spComponent1->GetOwnerGameObject())->GetDepth() != static_pointer_cast<CPanel>(_spComponent2->GetOwnerGameObject())->GetDepth())
				{
					return static_pointer_cast<CPanel>(_spComponent1->GetOwnerGameObject())->GetDepth() < static_pointer_cast<CPanel>(_spComponent2->GetOwnerGameObject())->GetDepth();
				}

				return spRectComponent1->GetDepth() < spRectComponent2->GetDepth();
			});
	}

	return S_OK;
}

HRESULT Engine::CRenderManager::AddTreeNode(const std::shared_ptr<CTreeNode>& _spTreeNode)
{
	m_lstTreeNodes.push_back(_spTreeNode);
	return S_OK;
}

void Engine::CRenderManager::RenderPre()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_PRE])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render(); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_PRE].clear();
}

void Engine::CRenderManager::RenderNblend()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_NBLEND])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::DEFAULT); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_NBLEND].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_NBLEND, (uint8)EShaderPath::DEFAULT);
}

void Engine::CRenderManager::RenderNblend_NonCull()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_NBLEND_NONCULL])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::NONCULL); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_NBLEND_NONCULL].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_NBLEND_NONCULL, (uint8)EShaderPath::NONCULL);
}

void Engine::CRenderManager::RenderLblend()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_LBLEND])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::ALPHABLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_LBLEND].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_LBLEND, (uint8)EShaderPath::ALPHABLEND);
}

void Engine::CRenderManager::RenderLblend_NonCull()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_LBLEND_NONCULL])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::NONCULL_ALPHABLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_LBLEND_NONCULL].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_LBLEND_NONCULL, (uint8)EShaderPath::NONCULL_ALPHABLEND);
}

void Engine::CRenderManager::RenderLblend_Add()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_LBLEND_ADD])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::BLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_LBLEND_ADD].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_LBLEND_ADD, (uint8)EShaderPath::BLEND);
}

void Engine::CRenderManager::RenderLblend_Add_NonCull()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_LBLEND_ADD_NONCULL])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::NONCULL_BLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_LBLEND_ADD_NONCULL].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_LBLEND_ADD_NONCULL, (uint8)EShaderPath::NONCULL_BLEND);
}

void Engine::CRenderManager::RenderLblend_Reverse()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_LBLEND_REVERSE])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::REVERSE_ALHPABLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_LBLEND_REVERSE].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_LBLEND_REVERSE, (uint8)EShaderPath::REVERSE_ALHPABLEND);
}

void Engine::CRenderManager::RenderLblend_Reverse_NonCull()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_LBLEND_REVERSE_NONCULL])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::NONCULL_REVERSE_ALHPABLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_LBLEND_REVERSE_NONCULL].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_LBLEND_REVERSE_NONCULL, (uint8)EShaderPath::NONCULL_REVERSE_ALHPABLEND);
}

void Engine::CRenderManager::RenderPbr()
{
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::BACKBUFFER);

	m_tPBR.tLightInfo = m_wpLightManager.lock()->GetLightInfo();
	ENSURE(m_cpVariable_PBR->SetRawValue(&m_tPBR, 0, sizeof(m_tPBR)));
	ENSURE(m_cpVariable_TexturesPBR->SetResourceArray(m_vecSRVs_PBR.data(), 0, (int32)m_vecSRVs_PBR.size()));
	ENSURE(m_spPbrShader->BeginPass(0));

	ENSURE(m_spRectBuffer->Render());

	m_wpPipelineManager.lock()->EndPipelineView();
}

void Engine::CRenderManager::RenderNLight()
{
	auto& umapMeshes = m_spSkyDome->GetMeshInfos();

	for (auto& Pair : umapMeshes)
	{
		for (auto& tMeshInfo : Pair.second)
		{
			tMeshInfo->m_spMeshBuffer->SetMatrices(tMeshInfo->m_matTransform, Matrix::Identity);
			tMeshInfo->m_spMeshBuffer->BindVariable(m_cpVariable_Sky, m_cpVariable_TexturesSky);
			m_spSkyShader->BeginPass(0);
			tMeshInfo->m_spMeshBuffer->Render();
		}
	}

	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_NLIGHT])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::DEFAULT); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_NLIGHT].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_NLIGHT, (uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::DEFAULT);
}

void Engine::CRenderManager::RenderNLight_NonCull()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_NLIGHT_NONCULL])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::NONCULL); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_NLIGHT_NONCULL].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_NLIGHT_NONCULL, (uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::NONCULL);
}

void Engine::CRenderManager::RenderBlend()
{
	Vector3 vCamPos = CEngineModule::GetInstance()->GetCameraManager()->GetCurCameraWorldPosition();
	m_arrComponents[(uint8)ERenderType::RENDER_BLEND].sort([&](shared_ptr<CComponent> _spDstComponent, shared_ptr<CComponent> _spSrcComponent)->bool
		{
			if (Vector3::Distance(_spDstComponent->GetOwnerGameObject()->GetPosition().value(), vCamPos) > Vector3::Distance(_spSrcComponent->GetOwnerGameObject()->GetPosition().value(), vCamPos))
			{
				return true;
			}
			return false;
		});

	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_BLEND])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::ALPHABLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_BLEND].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_BLEND, (uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::ALPHABLEND);
}

void Engine::CRenderManager::RenderBlend_NonCull()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_BLEND_NONCULL])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::NONCULL_ALPHABLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_BLEND_NONCULL].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_BLEND_NONCULL, (uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::NONCULL_ALPHABLEND);
}

void Engine::CRenderManager::RenderBlend_Add()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_BLEND_ADD])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::BLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_BLEND_ADD].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_BLEND_ADD, (uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::BLEND);
}

void Engine::CRenderManager::RenderBlend_Add_NonCull()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_BLEND_ADD_NONCULL])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::NONCULL_BLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_BLEND_ADD_NONCULL].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_BLEND_ADD_NONCULL, (uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::NONCULL_BLEND);
}

void Engine::CRenderManager::RenderBlend_Reverse()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_BLEND_REVERSE])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::REVERSE_ALHPABLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_BLEND_REVERSE].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_BLEND_REVERSE, (uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::REVERSE_ALHPABLEND);
}

void Engine::CRenderManager::RenderBlend_Reverse_NonCull()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_BLEND_REVERSE_NONCULL])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render((uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::NONCULL_REVERSE_ALHPABLEND); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_BLEND_REVERSE_NONCULL].clear();

	m_wpMeshHandler.lock()->RenderInstances(ERenderType::RENDER_BLEND_REVERSE_NONCULL, (uint8)EShaderPath::ENUM_END + (uint8)EShaderPath::NONCULL_REVERSE_ALHPABLEND);
}

void Engine::CRenderManager::RenderUi()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_UI])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render(); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_UI].clear();
}

void Engine::CRenderManager::RenderMosaic()
{
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::MOSAIC);

	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_MOSAIC])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render(); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_MOSAIC].clear();

	m_wpPipelineManager.lock()->EndPipelineView();

	// MOSAIC_RESULT
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::MOSAIC_RESULT);

	ENSURE(m_cpVariable_EffectDeffered->SetRawValue(&m_tEffectInfoDeffered, 0, sizeof(m_tEffectInfoDeffered)));
	ENSURE(m_cpVariable_TexturesEffectDeffered->SetResourceArray(m_vecSRVs_Effect.data(), 0, (int32)m_vecSRVs_Effect.size()));
	ENSURE(m_spEffectShader->BeginPass(8));
	ENSURE(m_spRectBuffer->Render());

	m_wpPipelineManager.lock()->EndPipelineView();
}

void Engine::CRenderManager::RenderPost()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_POST])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render(); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_POST].clear();
}
 
void Engine::CRenderManager::RenderGlow()
{
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::GLOW);

	// RENDER_GLOW의 컴포넌트들을 정렬하여 그립니다.
	Vector3 vCamPos = CEngineModule::GetInstance()->GetCameraManager()->GetCurCameraWorldPosition();
	m_arrComponents[(uint8)ERenderType::RENDER_GLOW].sort([&](shared_ptr<CComponent> _spDstComponent, shared_ptr<CComponent> _spSrcComponent)->bool
		{
			if (Vector3::Distance(_spDstComponent->GetOwnerGameObject()->GetPosition().value(), vCamPos) > Vector3::Distance(_spSrcComponent->GetOwnerGameObject()->GetPosition().value(), vCamPos))
			{
				return true;
			}
			return false;
		});

	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_GLOW])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render(); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_GLOW].clear();

	m_wpPipelineManager.lock()->EndPipelineView();
	// ===============================================================================================
	/* BLOOM */
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::BLOOM);
	m_tEffectInfoDeffered.fDistortionTimer += m_wpTimeManager.lock()->GetWorldDeltaSeconds();
	ENSURE(m_cpVariable_EffectDeffered->SetRawValue(&m_tEffectInfoDeffered, 0, sizeof(m_tEffectInfoDeffered)));
	ENSURE(m_cpVariable_TexturesEffectDeffered->SetResourceArray(m_vecSRVs_Effect.data(), 0, (int32)m_vecSRVs_Effect.size()));
	ENSURE(m_spEffectShader->BeginPass(2));
	ENSURE(m_spRectBuffer->Render());
	m_wpPipelineManager.lock()->EndPipelineView();

	// m_wpPipelineManager.lock()->FindShaderResourceView(EViewType::EMISSION)

	/* BLUR_X */
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::BLUR_X);
	ENSURE(m_spEffectShader->BeginPass(3));
	ENSURE(m_spRectBuffer->Render());
	m_wpPipelineManager.lock()->EndPipelineView();

	/* BLUR_Y */
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::BLUR_Y);
	ENSURE(m_spEffectShader->BeginPass(4));
	ENSURE(m_spRectBuffer->Render());
	m_wpPipelineManager.lock()->EndPipelineView();

	/* GLOW */
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::GLOW_RESULT);
	ENSURE(m_spEffectShader->BeginPass(5));
	ENSURE(m_spRectBuffer->Render());
	m_wpPipelineManager.lock()->EndPipelineView();

	/* MERGE TO BACK_BUFFER_VIEW */
	ENSURE(m_spEffectShader->BeginPass(0));
	ENSURE(m_spRectBuffer->Render());

	//// ========================================================================================================================
	/* BLOOM */
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::BLOOM);
	ENSURE(m_cpVariable_EffectDeffered->SetRawValue(&m_tEffectInfoDeffered, 0, sizeof(m_tEffectInfoDeffered)));
	ENSURE(m_cpVariable_TexturesEffectDeffered->SetResourceArray(m_vecSRVs_Effect.data(), 0, (int32)m_vecSRVs_Effect.size()));
	ENSURE(m_spEffectShader->BeginPass(2));
	ENSURE(m_spRectBuffer->Render());
	m_wpPipelineManager.lock()->EndPipelineView();

	/* BLUR_X */
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::BLUR_X);
	ENSURE(m_spEffectShader->BeginPass(3));
	ENSURE(m_spRectBuffer->Render());
	m_wpPipelineManager.lock()->EndPipelineView();

	/* BLUR_Y */
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::BLUR_Y);
	ENSURE(m_spEffectShader->BeginPass(4));
	ENSURE(m_spRectBuffer->Render());
	m_wpPipelineManager.lock()->EndPipelineView();

	/* GLOW */
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::GLOW_RESULT);
	ENSURE(m_spEffectShader->BeginPass(5));
	ENSURE(m_spRectBuffer->Render());
	m_wpPipelineManager.lock()->EndPipelineView();

	/* MERGE TO BACK_BUFFER_VIEW */
	ENSURE(m_spEffectShader->BeginPass(0));
	ENSURE(m_spRectBuffer->Render());
}

void Engine::CRenderManager::RenderDistortion()
{
	m_wpPipelineManager.lock()->BeginPipelineView(EPipelineViewType::DISTORTION);

	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_DISTORTION])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render(); }
		spComponent = nullptr;
	}
	m_wpPipelineManager.lock()->EndPipelineView();

	if (m_arrComponents[(uint8)ERenderType::RENDER_DISTORTION].empty())
	{
		return;
	}
	else
	{
		ENSURE(m_spEffectShader->BeginPass(6));

		ENSURE(m_spRectBuffer->Render());
	}

	m_arrComponents[(uint8)ERenderType::RENDER_DISTORTION].clear();
}

void Engine::CRenderManager::RenderPostEffect()
{
	/* Post_Effect */
	// ENSURE(m_cpVariable_EffectDeffered->SetRawValue(&m_tEffectInfoDeffered, 0, sizeof(m_tEffectInfoDeffered)));
	// ENSURE(m_cpVariable_TexturesEffectDeffered->SetResourceArray(m_vecSRVs_Effect.data(), 0, (int32)m_vecSRVs_Effect.size()));
	// ENSURE(m_spEffectShader->BeginPass(1));
	// ENSURE(m_spRectBuffer->Render());
}

void Engine::CRenderManager::RenderPostUI()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_POST_UI])
	{
		if (!spComponent->GetOwnerGameObject() || spComponent->IsCulled() || !spComponent->IsActive())
		{
			spComponent = nullptr;
			continue;
		}

		if (spComponent != nullptr) { spComponent->Render(); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_POST_UI].clear();
}

#ifdef _DEBUG
void Engine::CRenderManager::RenderDebug()
{
	for (auto& spComponent : m_arrComponents[(uint8)ERenderType::RENDER_DEBUG])
	{
		if (spComponent != nullptr) { spComponent->Render(); }
		spComponent = nullptr;
	}
	m_arrComponents[(uint8)ERenderType::RENDER_DEBUG].clear();

	for (auto& spTreeNode : m_lstTreeNodes)
	{
		if (spTreeNode != nullptr) { spTreeNode->Render(); }
		spTreeNode = nullptr;
	}
	m_lstTreeNodes.clear();
}
#endif