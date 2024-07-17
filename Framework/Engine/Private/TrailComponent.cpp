// Default
#include "pch.h"
#include "TrailComponent.h"

// Module, Manager
#include "EngineModule.h"
#include "DeviceManager.h"
#include "TextureHandler.h"
#include "ShaderHandler.h"
#include "RenderManager.h"
#include "CameraManager.h"

// GameObject
#include "GameObject.h"

// etc
#include "EffectShader.h"
#include "TrailBuffer.h"

shared_ptr<CTrailComponent> Engine::CTrailComponent::Create(const shared_ptr<FComponentDesc>& _spComponentDesc)
{
	const shared_ptr<CTrailComponent> spTrailComponent = make_shared<CTrailComponent>();

	ENSUREF(spTrailComponent->InitializeComponent(_spComponentDesc), L"Failed to Create CTrailComponent : CTrailComponent");

	return spTrailComponent;
}

HRESULT Engine::CTrailComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// etc
	shared_ptr<CDeviceManager> spDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_cpDevice = spDeviceManager->GetDevice();
	m_cpDeviceContext = spDeviceManager->GetDeviceContext();

	// effect component
	shared_ptr<FTrailComponentDesc> spTrailComponentDesc = static_pointer_cast<FTrailComponentDesc>(_spComponentDesc);

	m_iNumRects = spTrailComponentDesc->m_iNumRects;
	m_fMaxLifeTime = spTrailComponentDesc->m_fMaxLifeTime;
	m_vColor = spTrailComponentDesc->m_vColor;

	/* 텍스처 */
	m_bUseDiffuse = spTrailComponentDesc->m_bUseDiffuse;
	m_bUseNoise = spTrailComponentDesc->m_bUseNoise;
	m_bUseMask = spTrailComponentDesc->m_bUseMask;
	m_bUseDistortion = spTrailComponentDesc->m_bUseDistortion;
	m_bUseDissolve = spTrailComponentDesc->m_bUseDissolve;

	m_spEffectShader = CShaderHandler::GetInstance()->FindEffectShader(spTrailComponentDesc->m_wstrEffectShaderName);
	CHECKF(m_spEffectShader, L"Failed to FindEffectShader : CTrailComponent");

	// Bind Variable 변수를 가져옴
	m_cpVariable_EffectInfo = m_spEffectShader->GetRawVariableByName("g_tEffectInfo");
	m_cpVariable_Textures = m_spEffectShader->GetShaderResourceVariableByName("g_Textures");

	m_vecSRVs.resize(5, CTextureHandler::GetInstance()->FindShaderResourceView(L"Default", ETextureType::DIFFUSE).Get());
	if (m_bUseDiffuse) { m_vecSRVs[0] = CTextureHandler::GetInstance()->FindShaderResourceView(spTrailComponentDesc->m_wstrDiffuseTextureName, ETextureType::DIFFUSE).Get(); }
	if (m_bUseNoise) { m_vecSRVs[1] = CTextureHandler::GetInstance()->FindShaderResourceView(spTrailComponentDesc->m_wstrNoiseTextureName, ETextureType::NOISE).Get(); }
	if (m_bUseMask) { m_vecSRVs[2] = CTextureHandler::GetInstance()->FindShaderResourceView(spTrailComponentDesc->m_wstrMaskTextureName, ETextureType::MASK).Get(); }
	if (m_bUseDistortion) { m_vecSRVs[3] = CTextureHandler::GetInstance()->FindShaderResourceView(spTrailComponentDesc->m_wstrDistortionTextureName, ETextureType::DISTORTION).Get(); }
	if (m_bUseDissolve) { m_vecSRVs[4] = CTextureHandler::GetInstance()->FindShaderResourceView(spTrailComponentDesc->m_wstrDissolveTextureName, ETextureType::DISSOLVE).Get(); }
	//CHECKF(m_cpDiffuseTexture, L"Failed to Find DiffuseTexture : CTrailComponent");
	//CHECKF(m_cpNoiseTexture, L"Failed to Find NoiseTexture : CTrailComponent");
	//CHECKF(m_cpMaskTexture, L"Failed to Find MaskTexture : CTrailComponent");
	//CHECKF(m_cpDistortionTexture, L"Failed to Find DistortionTexture : CTrailComponent");
	//CHECKF(m_cpDissolveTexture, L"Failed to Find DissolveTexture : CTrailComponent");

	m_vecLifeTime.reserve(m_iNumRects);
	m_vecLifeTime.resize(m_iNumRects);

	InitializeTrailBuffer();

	// etc
	m_wpRenderManager = CEngineModule::GetInstance()->GetRenderManager();

	return S_OK;
}

HRESULT Engine::CTrailComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CTrailComponent::PreTickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CTrailComponent::TickComponent(float _fDeltaSeconds)
{
	m_fTempAcc += _fDeltaSeconds;

	return 0;
}

int32 Engine::CTrailComponent::PostTickComponent(float _fDeltaSeconds)
{
	// 렌더 매니저에 트레일 컴포넌트를 추가합니다.
	m_wpRenderManager.lock()->AddComponent(shared_from_this(), m_eRenderType);
	return 0;
}

HRESULT Engine::CTrailComponent::EndPlay()
{
	return S_OK;
}

void Engine::CTrailComponent::Render(int32 _iShaderPath)
{
	ENSUREF(BindVariable(), L"Failed to BindVariable : CTrailComponent");
	ENSUREF(m_spEffectShader->BeginPass(m_iPassIndex), L"Failed to BeginPass : CTrailComponent");
	ENSUREF(m_spTrailBuffer->Render(), L"Failed to Render : CTrailComponent");
}

void Engine::CTrailComponent::Release()
{
	m_cpVariable_EffectInfo = nullptr;
	m_cpVariable_Textures = nullptr;

	m_spTrailBuffer->Release();
	m_spTrailBuffer = nullptr;
}

HRESULT Engine::CTrailComponent::InitializeTrailBuffer()
{
	FTrailBufferDesc tTrailBufferDesc;
	tTrailBufferDesc.m_iNumRect = m_iNumRects;

	m_spTrailBuffer = CTrailBuffer::Create(make_shared<FTrailBufferDesc>(tTrailBufferDesc));
	CHECKF(m_spTrailBuffer, L"Failed to Create CTrailBuffer : CTrailComponent");

	return S_OK;
}

void Engine::CTrailComponent::TickTrail(const Vector3& _vStartPos, const Vector3 _vEndPos)
{
	if (m_fMaxLifeTime > m_fTempAcc) return;

	// 트레일의 정점을 갱신해줍니다.
	pair<Vector3, Vector3> prStartEnd = { _vStartPos, _vEndPos };

	m_lstTrailPositions.push_back(prStartEnd);

	if (m_iNumRects < m_lstTrailPositions.size())
	{
		m_lstTrailPositions.pop_front();
	}

	if (2 < m_lstTrailPositions.size())
	{
		D3D11_MAPPED_SUBRESOURCE tSubResource = {};

		m_cpDeviceContext->Map(m_spTrailBuffer->GetVertexBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);
		FPositionTexCoord2* pVertices = ((FPositionTexCoord2*)tSubResource.pData);

		uint32 iVertexCnt = 0;
		for (auto& pair : m_lstTrailPositions)
		{
			float fURatio = (float)iVertexCnt / (float)(m_spTrailBuffer->GetNumVertices() - 2);

			if (iVertexCnt == m_spTrailBuffer->GetNumVertices() - 4) { fURatio = 1.f; }

			// Start
			pVertices[iVertexCnt].m_vPosition = pair.first;
			pVertices[iVertexCnt].m_vPosition.y = (pair.first.y + pair.second.y) * 0.5f + fURatio * fabs((pair.first.y - pair.second.y) * 0.5f);
			pVertices[iVertexCnt].m_vTexCoord2 = Vector2(1.f - fURatio, 1.f);
			++iVertexCnt;

			// End
			pVertices[iVertexCnt].m_vPosition = pair.second;
			pVertices[iVertexCnt].m_vPosition.y = (pair.first.y + pair.second.y) * 0.5f - fURatio * fabs((pair.first.y - pair.second.y) * 0.5f);
			pVertices[iVertexCnt].m_vTexCoord2 = Vector2(1.f - fURatio, 0.f);
			++iVertexCnt;
		}
		m_cpDeviceContext->Unmap(m_spTrailBuffer->GetVertexBuffer().Get(), 0);
	}

	m_fTempAcc = 0.f;
}

void Engine::CTrailComponent::ClearRectList(const Vector3& _vStartPos, const Vector3 _vEndPos)
{
	m_lstTrailPositions.clear();

	D3D11_MAPPED_SUBRESOURCE tSubResource = {};

	m_cpDeviceContext->Map(m_spTrailBuffer->GetVertexBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);
	FPositionTexCoord2* pVertices = ((FPositionTexCoord2*)tSubResource.pData);

	for (uint32 iVertexCnt = 0; iVertexCnt < m_spTrailBuffer->GetNumVertices();)
	{
		float fURatio = (float)iVertexCnt / (float)(m_spTrailBuffer->GetNumVertices());

		pair<Vector3, Vector3> prStartEnd = {};

		// Start
		pVertices[iVertexCnt].m_vPosition = _vStartPos;
		pVertices[iVertexCnt].m_vPosition.y = (_vStartPos.y + _vEndPos.y) * 0.5f + fURatio * fabs((_vStartPos.y - _vEndPos.y) * 0.5f);
		pVertices[iVertexCnt].m_vTexCoord2 = Vector2(1.f - fURatio, 1.f);
		prStartEnd.first = pVertices[iVertexCnt].m_vPosition;
		++iVertexCnt;

		// End
		pVertices[iVertexCnt].m_vPosition = _vEndPos;
		pVertices[iVertexCnt].m_vPosition.y = (_vStartPos.y + _vEndPos.y) * 0.5f - fURatio * fabs((_vStartPos.y - _vEndPos.y) * 0.5f);
		pVertices[iVertexCnt].m_vTexCoord2 = Vector2(1.f - fURatio, 0.f);
		prStartEnd.second = pVertices[iVertexCnt].m_vPosition;
		++iVertexCnt;

		m_lstTrailPositions.push_back(prStartEnd);
	}
	m_cpDeviceContext->Unmap(m_spTrailBuffer->GetVertexBuffer().Get(), 0);
}

HRESULT Engine::CTrailComponent::BindVariable()
{
	/* World */
	m_tEffectInfoTrail.matWorld = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();

	/* Diffuse */
	m_tEffectInfoTrail.bUseDiffuse = m_bUseDiffuse;

	/* Noise */
	m_tEffectInfoTrail.bUseNoise = m_bUseNoise;

	/* Mask */
	m_tEffectInfoTrail.bUseMask = m_bUseMask;

	/* Distortion */
	m_tEffectInfoTrail.bUseDistortion = m_bUseDistortion;

	/* Dissolve */
	m_tEffectInfoTrail.bUseDissolve = m_bUseDissolve;

	/* Default Color */
	m_tEffectInfoTrail.vColor = m_vColor;

	ENSURE(m_cpVariable_EffectInfo->SetRawValue(&m_tEffectInfoTrail, 0, sizeof(m_tEffectInfoTrail)));
	ENSURE(m_cpVariable_Textures->SetResourceArray(m_vecSRVs.data(), 0, (int32)m_vecSRVs.size()));

	return S_OK;
}
