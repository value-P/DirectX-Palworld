
// Default
#include "pch.h"
#include "EffectMeshComponent.h"
#include "StringUtility.h"
#include "MathUtility.h"

// Module, Handler, Manager
#include "EngineModule.h"
#include "ShaderHandler.h"
#include "TextureHandler.h"
#include "CameraManager.h"
#include "DeviceManager.h"
#include "RenderManager.h"

// GameObject
#include "GameObject.h"
#include "MeshBuffer.h"

// etc
#include "EffectShader.h"
#include "Model.h"

std::shared_ptr<CEffectMeshComponent> Engine::CEffectMeshComponent::Create(const std::shared_ptr<FEffectMeshComponentDesc>& _spEffectMeshComponentDesc, FEffectMeshInfo _tEffectMeshInfo)
{
	const std::shared_ptr<CEffectMeshComponent> spEffectMeshComponent = make_shared<CEffectMeshComponent>();

	spEffectMeshComponent->SetEffectMeshInfo(_tEffectMeshInfo);

	ENSUREF(spEffectMeshComponent->InitializeComponent(_spEffectMeshComponentDesc), L"Failed to InitializeComponent Effect Mesh Component");
	return spEffectMeshComponent;
}
std::shared_ptr<CEffectMeshComponent> Engine::CEffectMeshComponent::CreateFromJson(const std::shared_ptr<FEffectMeshComponentDesc>& _spEffectMeshComponentDesc, const std::wstring& _wstrJsonFileName)
{
	const std::shared_ptr<CEffectMeshComponent> spEffectMeshComponent = make_shared<CEffectMeshComponent>();

	spEffectMeshComponent->LoadFromJsonFile(_wstrJsonFileName);

	ENSUREF(spEffectMeshComponent->InitializeComponent(_spEffectMeshComponentDesc), L"Failed to InitializeComponent Effect Mesh Component");
	return spEffectMeshComponent;
}

HRESULT Engine::CEffectMeshComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// effect component
	if (FAILED(CEffectComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	shared_ptr<FEffectMeshComponentDesc> spEffectMeshComponentDesc = static_pointer_cast<FEffectMeshComponentDesc>(_spComponentDesc);

	// render type reinitialize
	m_eRenderType = m_tEffectMeshInfo.m_eRenderType;
	m_eEffectType = m_tEffectMeshInfo.m_eEffectType;

	m_matPivot = spEffectMeshComponentDesc->m_matPivot;

	// etc
	FindEffectShader();
	FindShaderResourceViews();
	InitializeBuffer();

	m_vOriginPos = m_tEffectMeshInfo.m_vTranslation;

	return S_OK;
}
HRESULT Engine::CEffectMeshComponent::BeginPlay()
{
	if (FAILED(CEffectComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}
int32 Engine::CEffectMeshComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CEffectComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}
int32 Engine::CEffectMeshComponent::TickComponent(float _fDeltaSeconds)
{
	if (m_bPause) { return CEffectComponent::TickComponent(_fDeltaSeconds); }

	// 이펙트 진행
	PlayEffect(_fDeltaSeconds);
	PlayTrail(_fDeltaSeconds);

	return CEffectComponent::TickComponent(_fDeltaSeconds);
}
int32 Engine::CEffectMeshComponent::PostTickComponent(float _fDeltaSeconds)
{
	m_eRenderType = m_tEffectMeshInfo.m_eRenderType;

	switch (CEffectComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}
HRESULT Engine::CEffectMeshComponent::EndPlay()
{
	if (FAILED(CEffectComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CEffectMeshComponent::Render(int32 _iShaderPath)
{
	if (m_fTrackPosition < m_tEffectMeshInfo.m_vLifeTime.x || m_bCulled) { return; }

	ENSUREF(BindVariable(), L"Failed to BindVariable : CEffectMeshComponent");
	//	ENSUREF(m_wpEffectShader.lock()->BeginPass(0), L"Failed to BeginPass : CEffectMeshComponent");

	auto& umapMeshes = m_spModel->GetMeshInfos();

	for (auto& tMeshInfo : umapMeshes)
	{
		for (auto& spMeshBuffer : tMeshInfo.second)
		{
			if (m_tEffectMeshInfo.m_bTrail)
			{
				float fColorAlpha = m_tMeshInfoEffect.vColor1.w;

				for (auto& matrix : m_lstMeshMatrices)
				{
					m_tMeshInfoEffect.matWorld = matrix;
					m_tMeshInfoEffect.vColor1.w = fColorAlpha;

					ENSUREF(m_cpVariable_Transform->SetMatrix(reinterpret_cast<float*>(&spMeshBuffer->m_matTransform)), L"Failed to BindVariable : CEffectMeshComponent");
					ENSURE(m_cpVariable_MeshInfo->SetRawValue(&m_tMeshInfoEffect, 0, sizeof(m_tMeshInfoEffect)));
					ENSUREF(m_wpEffectShader.lock()->BeginPass(m_iPassIndex), L"Failed to BeginPass : CEffectMeshComponent");
					spMeshBuffer->m_spMeshBuffer->Render();

					fColorAlpha -= (m_tMeshInfoEffect.vColor1.w / m_tEffectMeshInfo.m_iNumTrails);
					fColorAlpha += 0.2f;
				}
			}
			else
			{
				ENSUREF(m_cpVariable_Transform->SetMatrix(reinterpret_cast<float*>(&spMeshBuffer->m_matTransform)), L"Failed to BindVariable : CEffectMeshComponent");
				ENSURE(m_cpVariable_MeshInfo->SetRawValue(&m_tMeshInfoEffect, 0, sizeof(m_tMeshInfoEffect)));
				ENSUREF(m_wpEffectShader.lock()->BeginPass(m_iPassIndex), L"Failed to BeginPass : CEffectMeshComponent");
				spMeshBuffer->m_spMeshBuffer->Render();
			}
		}
	}

	CEffectComponent::Render(_iShaderPath);
}

void Engine::CEffectMeshComponent::Release()
{
	m_cpVariable_Transform = nullptr;

	m_vecSRVs.clear();

	CEffectComponent::Release();
}
HRESULT Engine::CEffectMeshComponent::BindVariable()
{
	/* W V P */
	if (!m_wpOwnerGameObject.lock()->GetParentGameObject())
	{
		m_tMeshInfoEffect.matGroupWorld = Matrix::Identity;
	}
	else
	{
		m_tMeshInfoEffect.matGroupWorld = m_wpOwnerGameObject.lock()->GetParentGameObject()->GetTransformMatrix().value();
	}
	m_tMeshInfoEffect.matWorld = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();

	/* Effect Flags */
	m_tMeshInfoEffect.bDiffuse = m_tEffectMeshInfo.m_bDiffuse;
	m_tMeshInfoEffect.bNoise = m_tEffectMeshInfo.m_bNoise;
	m_tMeshInfoEffect.bMask = m_tEffectMeshInfo.m_bMask;
	m_tMeshInfoEffect.bDistortion = m_tEffectMeshInfo.m_bDistortion;
	m_tMeshInfoEffect.bDissolve = m_tEffectMeshInfo.m_bDissolve;

	/* 라이프 타임 */
	m_tMeshInfoEffect.fTrackPosition = m_fTrackPosition;
	m_tMeshInfoEffect.vLifeTime = m_tEffectMeshInfo.m_vLifeTime;

	/* UV */
	m_tMeshInfoEffect.bUVMoveLoop = m_tEffectMeshInfo.m_bUVMoveLoop;
	m_tMeshInfoEffect.vUVAcc = m_vUVAcc;

	/* 스프라이트 */
	m_tMeshInfoEffect.bSprite = m_tEffectMeshInfo.m_bSprite;
	m_tMeshInfoEffect.vMinUV = m_vMinUV;
	m_tMeshInfoEffect.vMaxUV = m_vMaxUV;

	/* 글로우 */
	m_tMeshInfoEffect.bGlow = m_tEffectMeshInfo.m_bGlow;
	m_tMeshInfoEffect.fGlowWeight = m_tEffectMeshInfo.m_fGlowWeight;

	/* 블룸 */
	m_tMeshInfoEffect.bBloom = m_tEffectMeshInfo.m_bBloom;
	m_tMeshInfoEffect.fBloomWeight = m_tEffectMeshInfo.m_fBloomWeight;

	/* 알파 감쇄 */
	m_tMeshInfoEffect.bAlphaAttenuation = m_tEffectMeshInfo.m_bAlphaAttenuation;
	m_tMeshInfoEffect.fAlphaAttenuationWeight = m_tEffectMeshInfo.m_fAlphaAttenuationWeight;

	/* 셰이딩 가중치 */
	m_tMeshInfoEffect.fDistortionWeight = m_tEffectMeshInfo.m_fDistortionWeight;
	m_tMeshInfoEffect.fDissolveWeight = m_tEffectMeshInfo.m_fDissolveWeight;

	/* 색상 */
	if (m_tEffectMeshInfo.m_vecKeyFrames.empty() || 2 > m_tEffectMeshInfo.m_vecKeyFrames.size())
	{
		m_tMeshInfoEffect.vColor1 = m_tEffectMeshInfo.m_vDiffuseColor1;
	}
	else
	{
		m_tMeshInfoEffect.vColor1 = m_tLerpedData.m_vColor1;
		m_tMeshInfoEffect.vColor2 = m_tLerpedData.m_vColor2;
		m_tMeshInfoEffect.vColor3 = m_tLerpedData.m_vColor3;
	}

	/* 트레일 */
	m_tMeshInfoEffect.bTrail = m_tEffectMeshInfo.m_bTrail;

	m_tMeshInfoEffect.fAlphaDiscardValue = m_tEffectMeshInfo.m_fAlphaDiscardValue;

	ENSURE(m_cpVariable_MeshInfo->SetRawValue(&m_tMeshInfoEffect, 0, sizeof(m_tMeshInfoEffect)));
	ENSURE(m_cpVariable_Textures->SetResourceArray(m_vecSRVs.data(), 0, (int32)m_vecSRVs.size()));

	return S_OK;
}

HRESULT Engine::CEffectMeshComponent::InitializeBuffer()
{
	// 이펙트 메시의 경우 매터리얼도 없고, 애니메이션도 없고, 본도 없기 때문에,
	// CModel에 메시만 채워져 있을 것이다. 매터리얼은 경우에 따라 들어있을 수도 있지만,
	// 사용하지만 않으면 상관 없음. 텍스처는 렌더 시점에 CEffectMeshComponent에서 일일이
	// 바인딩 해주도록 한다.
	m_spModel = CModel::Create(m_tEffectMeshInfo.m_wstrModelFilePath, m_matPivot, EMeshManageType::UNIQUE, nullptr);
	CHECKF(m_spModel, L"Failed to Create Model : CEffectMeshComponent");

	return S_OK;
}
HRESULT Engine::CEffectMeshComponent::FindEffectShader()
{
	m_wpEffectShader = CShaderHandler::GetInstance()->FindEffectShader(m_tEffectMeshInfo.m_wstrEffectShaderName);
	CHECKF(!m_wpEffectShader.expired(), L"Failed to FindEffectShader : CEffectMeshComponent");

	m_cpVariable_MeshInfo = nullptr;
	m_cpVariable_Transform = nullptr;
	m_cpVariable_Textures = nullptr;

	m_cpVariable_MeshInfo = m_wpEffectShader.lock()->GetRawVariableByName("g_tMeshInfo");
	m_cpVariable_Transform = m_wpEffectShader.lock()->GetMatrixVariableByName("g_matTransform");
	m_cpVariable_Textures = m_wpEffectShader.lock()->GetShaderResourceVariableByName("g_Textures");

	return S_OK;
}
HRESULT Engine::CEffectMeshComponent::FindShaderResourceViews()
{
	m_vecSRVs.resize(5, CTextureHandler::GetInstance()->FindShaderResourceView(L"Default", ETextureType::DIFFUSE).Get());
	if (m_tEffectMeshInfo.m_bDiffuse)
	{
		m_vecSRVs[0] = CTextureHandler::GetInstance()->FindShaderResourceView(m_tEffectMeshInfo.m_wstrDiffuseTextureName, ETextureType::DIFFUSE).Get();
		CHECKF(m_vecSRVs[0], L"Failed to FindShaderResourceView DiffuseTexture : CEffectMeshComponent");
	}
	if (m_tEffectMeshInfo.m_bNoise)
	{
		m_vecSRVs[1] = CTextureHandler::GetInstance()->FindShaderResourceView(m_tEffectMeshInfo.m_wstrNoiseTextureName, ETextureType::NOISE).Get();
		CHECKF(m_vecSRVs[1], L"Failed to FindShaderResourceView NoiseTexture : CEffectMeshComponent");
	}
	if (m_tEffectMeshInfo.m_bMask)
	{
		m_vecSRVs[2] = CTextureHandler::GetInstance()->FindShaderResourceView(m_tEffectMeshInfo.m_wstrMaskTextureName, ETextureType::MASK).Get();
		CHECKF(m_vecSRVs[2], L"Failed to FindShaderResourceView MaskTexture : CEffectMeshComponent");
	}
	if (m_tEffectMeshInfo.m_bDistortion)
	{
		m_vecSRVs[3] = CTextureHandler::GetInstance()->FindShaderResourceView(m_tEffectMeshInfo.m_wstrDistortionTextureName, ETextureType::DISTORTION).Get();
		CHECKF(m_vecSRVs[3], L"Failed to FindShaderResourceView DistortionTexture : CEffectMeshComponent");
	}
	if (m_tEffectMeshInfo.m_bDissolve)
	{
		//m_vecSRVs[4] = CTextureHandler::GetInstance()->FindShaderResourceView(m_tEffectMeshInfo.m_wstrDissolveTextureName, ETextureType::DISSOLVE).Get();
		//CHECKF(m_vecSRVs[4], L"Failed to FindShaderResourceView DissolveTexture : CEffectMeshComponent");
	}

	return S_OK;
}

void Engine::CEffectMeshComponent::PlayEffect(float _fDeltaSeconds)
{
	m_fTrackPosition += _fDeltaSeconds;

	// 1. 트랙포지션이 최대 라이프 타임을 넘어갔을 경우(종료)
	if (m_fTrackPosition >= m_tEffectMeshInfo.m_vLifeTime.y)
	{
		if (m_tEffectMeshInfo.m_bLoop)
		{
			Loop();
			return;
		}
		else
		{
			EndLifeTime();
			return;
		}
	}
	// 2. 트랙포지션이 최소 라이프 타임보다 작을 경우(렌더 비활성화)
	else if (m_fTrackPosition < m_tEffectMeshInfo.m_vLifeTime.x)
	{
		NoneActivate();
		return;
	}
	// 3. 트랙포지션이 라이프 타임 사이에 있고, 현재 키프레임 타임보다 큰 경우(활성화)
	else if (m_fTrackPosition >= m_tEffectMeshInfo.m_vLifeTime.x)
	{
		m_wpOwnerGameObject.lock()->SetCulled(false);
		UVMove(_fDeltaSeconds);
		InterpolationBetweenKeyFrame(_fDeltaSeconds);
		if (m_tEffectMeshInfo.m_bSprite)
		{
			TickSprite(_fDeltaSeconds);
		}
		return;
	}

	return;
}
void Engine::CEffectMeshComponent::PlayTrail(float _fDeltaSeconds)
{
	m_fTrailTimeAcc += _fDeltaSeconds;
	if (m_tEffectMeshInfo.m_fTrailTerm < m_fTrailTimeAcc)
	{
		m_fTrailTimeAcc = 0.f;

		// 그룹에 속한 경우
		if (m_wpOwnerGameObject.lock()->GetParentGameObject())
		{
			Matrix matMesh = m_wpOwnerGameObject.lock()->GetTransformMatrix().value() * m_wpOwnerGameObject.lock()->GetParentGameObject()->GetTransformMatrix().value();
			m_lstMeshMatrices.push_front(matMesh);
		}
		// 그룹에 속하지 않은 경우
		else
		{
			Matrix matMesh = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
			m_lstMeshMatrices.push_front(matMesh);
		}

		if (m_tEffectMeshInfo.m_iNumTrails < m_lstMeshMatrices.size())
			m_lstMeshMatrices.pop_back();
	}
}
void Engine::CEffectMeshComponent::UVMove(float _fDeltaSeconds)
{
	m_vUVAcc.x += m_tEffectMeshInfo.m_vUVMoveSpeed.x * _fDeltaSeconds;
	m_vUVAcc.y += m_tEffectMeshInfo.m_vUVMoveSpeed.y * _fDeltaSeconds;
}
void Engine::CEffectMeshComponent::TickSprite(float _fDeltaSeconds)
{
	m_fSpriteLifeTimeAcc += _fDeltaSeconds;

	if (m_fSpriteLifeTimeAcc > m_tEffectMeshInfo.m_fSpriteSpeed)
	{
		m_fSpriteLifeTimeAcc = 0.f;

		m_vMinUV.x += 1.f / (float)m_tEffectMeshInfo.m_iSpriteRow;
		m_vMaxUV.x += 1.f / (float)m_tEffectMeshInfo.m_iSpriteRow;

		// 한 행을 모두 진행한 경우
		if (1.f < m_vMaxUV.x)
		{
			// 아래로 한 칸 이동
			m_vMinUV.x = 0.f;
			m_vMinUV.y += 1.f / (float)m_tEffectMeshInfo.m_iSpriteCol;

			m_vMaxUV.x = 1.f / (float)m_tEffectMeshInfo.m_iSpriteRow;
			m_vMaxUV.y += 1.f / (float)m_tEffectMeshInfo.m_iSpriteCol;
		}

		// 마지막 행 이였던 경우
		if (1.f < m_vMaxUV.y)
		{
			m_vMinUV = Vector2(0.f, 0.f);
			m_vMaxUV = Vector2(1.f / (float)m_tEffectMeshInfo.m_iSpriteRow, 1.f / (float)m_tEffectMeshInfo.m_iSpriteCol);
		}
	}
}
void Engine::CEffectMeshComponent::InterpolationBetweenKeyFrame(float _fDeltaSeconds)
{
	// 키프레임 컨테이너가 비어있는 경우와 컨테이너 크기가 2미만인 경우 보간을 시행하지 않습니다.
	if (m_tEffectMeshInfo.m_vecKeyFrames.empty() || 2 > m_tEffectMeshInfo.m_vecKeyFrames.size())
	{
		// 러프 데이터 초기값으로 초기화
		m_tLerpedData = FEffectMeshKeyFrame{};
		m_tLerpedData.m_vColor1 = Color(1.f, 1.f, 1.f, 1.f);
		m_tLerpedData.m_vColor2 = Color(1.f, 1.f, 1.f, 1.f);
		m_tLerpedData.m_vColor3 = Color(1.f, 1.f, 1.f, 1.f);
		return;
	}

	// 트랙포지션이 0인 경우, 현재 키프레임을 0으로 설정합니다.
	if (0.f == m_fTrackPosition) 
	{
		m_iCurKeyFrame = 0; 
	}

	// 마지막 키프레임
	FEffectMeshKeyFrame tLastKeyFrame = m_tEffectMeshInfo.m_vecKeyFrames.back();

	// 트랙포지션이 마지막 키프레임의 시간을 넘어간 경우
	if (m_fTrackPosition > tLastKeyFrame.m_fTime)
	{
		if (m_tEffectMeshInfo.m_bLoop)
		{
			Loop();
		}
		else
		{
			EndLifeTime();
		}
	}
	else
	{
		// 현재 트랙포지션이 다음 키프레임의 타임을 넘어간 경우, 키프레임을 1 증가시킵니다.
		if (m_fTrackPosition >= m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fTime) { ++m_iCurKeyFrame; }

		/* 패스 인덱스 */
		m_iPassIndex = m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_iPassIndex;

		/* 렌더 컬 */
		m_bCulled = m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_bRenderCulled;

		// 보간 비율 : 현재 키프레임과 다음 키프레임 사이에서 트랙포지션의 비율((트랙 포지션 - 현재 키프레임 타임) / (다음 키프레임 타임 - 현재 키프레임 타임))
		float fInterpolationRatio = (m_fTrackPosition - m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_fTime) / (m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fTime - m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_fTime);

		/* ※※ 키프레임 사이의 보간할 값들을 아래에 추가하면 됩니다. ※※ */
		/* Color */
		if (!m_tEffectMeshInfo.m_bDiffuse)
		{
			m_tLerpedData.m_vColor1 = EaseByType(m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eColor1EasingType,
				m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_vColor1,
				m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_vColor1,
				fInterpolationRatio);

			m_tLerpedData.m_vColor2 = EaseByType(m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eColor2EasingType,
				m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_vColor2,
				m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_vColor2,
				fInterpolationRatio);

			m_tLerpedData.m_vColor3 = EaseByType(m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eColor3EasingType,
				m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_vColor3,
				m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_vColor3,
				fInterpolationRatio);
		}

		/* Scale */
		m_tLerpedData.m_vScale = EaseByType(m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eScaleEasingType,
			m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_vScale,
			m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_vScale,
			fInterpolationRatio);

		m_wpOwnerGameObject.lock()->SetScale(m_tLerpedData.m_vScale);

		/* Rotation */
		m_tLerpedData.m_vRotation = EaseByType(m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eRotationEasingType,
			m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_vRotation,
			m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_vRotation,
			fInterpolationRatio);

		float fRotationWeight = EaseByType(m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eRotationEasingType,
			m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_fRotateWeight,
			m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fRotateWeight,
			fInterpolationRatio);

		m_wpOwnerGameObject.lock()->Turn(m_tLerpedData.m_vRotation, m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fRotateWeight);

		/* Translation */
		m_tLerpedData.m_vTranslation = EaseByType(m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eTranslationEasingType,
			m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame].m_vTranslation,
			m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_vTranslation,
			fInterpolationRatio);

		m_wpOwnerGameObject.lock()->SetPosition(m_tEffectMeshInfo.m_vTranslation + m_tLerpedData.m_vTranslation);
	}
}

void Engine::CEffectMeshComponent::RecycleFromPool()
{
	/* GameObject */
	ENSUREF(!m_wpOwnerGameObject.expired(), L"m_wpOwnerGameObject is nullptr : CEffectMeshComponent");
	m_wpOwnerGameObject.lock()->SetActive(true);
	m_wpOwnerGameObject.lock()->SetCulled(false);

	/* Component */
	m_fTrackPosition = 0.f;
	m_iCurKeyFrame = 0;

	m_vMinUV = Vector2::Zero;
	m_vMaxUV = Vector2::One;

	m_vUVAcc.x = 0.f;
	m_vUVAcc.y = 0.f;

	if (m_tEffectMeshInfo.m_vecKeyFrames.empty() || 2 > m_tEffectMeshInfo.m_vecKeyFrames.size())
	{
		m_tLerpedData = FEffectMeshKeyFrame{};
	}
	else
	{
		m_tLerpedData = m_tEffectMeshInfo.m_vecKeyFrames.front();
		m_wpOwnerGameObject.lock()->SetScale(m_tLerpedData.m_vScale);
		m_wpOwnerGameObject.lock()->Turn(m_tLerpedData.m_vRotation, m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fRotateWeight);
		m_wpOwnerGameObject.lock()->SetPosition(m_tEffectMeshInfo.m_vTranslation + m_tLerpedData.m_vTranslation);
	}
}
void Engine::CEffectMeshComponent::NoneActivate()
{
	// m_bActive = false;
	m_wpOwnerGameObject.lock()->SetCulled(true);
}
void Engine::CEffectMeshComponent::EndLifeTime()
{
	ENSUREF(!m_wpOwnerGameObject.expired(), L"m_wpOwnerGameObject is nullptr : CEffectMeshComponent");
	m_wpOwnerGameObject.lock()->SetActive(false);
	m_wpOwnerGameObject.lock()->SetCulled(true);

	m_fTrackPosition = 0.f;
	m_iCurKeyFrame = 0;

	m_vMinUV = Vector2::Zero;
	m_vMaxUV = Vector2::One;

	m_vUVAcc.x = 0.f;
	m_vUVAcc.y = 0.f;

	if (m_tEffectMeshInfo.m_vecKeyFrames.empty() || 2 > m_tEffectMeshInfo.m_vecKeyFrames.size())
	{
		m_tLerpedData = FEffectMeshKeyFrame{};
	}
	else
	{
		m_tLerpedData = m_tEffectMeshInfo.m_vecKeyFrames.front();
		m_wpOwnerGameObject.lock()->SetScale(m_tLerpedData.m_vScale);
		m_wpOwnerGameObject.lock()->Turn(m_tLerpedData.m_vRotation, m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fRotateWeight);
		m_wpOwnerGameObject.lock()->SetPosition(m_tEffectMeshInfo.m_vTranslation + m_tLerpedData.m_vTranslation);
	}
}
void Engine::CEffectMeshComponent::Loop()
{
	m_fTrackPosition = 0.f;
	m_iCurKeyFrame = 0;

	m_vMinUV = Vector2::Zero;
	m_vMaxUV = Vector2::One;

	m_vUVAcc.x = 0.f;
	m_vUVAcc.y = 0.f;

	if (m_tEffectMeshInfo.m_vecKeyFrames.empty() || 2 > m_tEffectMeshInfo.m_vecKeyFrames.size())
	{
		m_tLerpedData = FEffectMeshKeyFrame{};
	}
	else
	{
		m_tLerpedData = m_tEffectMeshInfo.m_vecKeyFrames.front();
		m_wpOwnerGameObject.lock()->SetScale(m_tLerpedData.m_vScale);
		m_wpOwnerGameObject.lock()->Turn(m_tLerpedData.m_vRotation, m_tEffectMeshInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fRotateWeight);
		m_wpOwnerGameObject.lock()->SetPosition(m_tEffectMeshInfo.m_vTranslation + m_tLerpedData.m_vTranslation);
	}
}

void Engine::CEffectMeshComponent::AddKeyFrame(FEffectMeshKeyFrame _tEffectMeshKeyFrmae)
{
	m_tEffectMeshInfo.m_vecKeyFrames.push_back(_tEffectMeshKeyFrmae);
	++m_tEffectMeshInfo.m_iNumKeyFrames;
}
void Engine::CEffectMeshComponent::SetKeyFrameValue(const FEffectMeshKeyFrame& _tEffectMeshKeyFrame, int32 _iIndex)
{
	if (m_tEffectMeshInfo.m_vecKeyFrames.empty() || _iIndex > (int32)m_tEffectMeshInfo.m_vecKeyFrames.size() - 1 || _iIndex < 0) { return; }

	m_tEffectMeshInfo.m_vecKeyFrames[_iIndex] = _tEffectMeshKeyFrame;
}
const std::optional<FEffectMeshKeyFrame> Engine::CEffectMeshComponent::GetKeyFrameValue(int32 _iIndex)
{
	if (m_tEffectMeshInfo.m_vecKeyFrames.empty() || _iIndex > (int32)m_tEffectMeshInfo.m_vecKeyFrames.size() - 1 || _iIndex < 0) { return nullopt; }

	return m_tEffectMeshInfo.m_vecKeyFrames[_iIndex];
}

void Engine::CEffectMeshComponent::SetEffectShader(const std::wstring& _wstrEffectShaderName)
{
	const shared_ptr<CEffectShader> spEffectShader = CShaderHandler::GetInstance()->FindEffectShader(_wstrEffectShaderName);
	if (!spEffectShader) { return; }

	m_wpEffectShader = spEffectShader;

	m_cpVariable_MeshInfo = nullptr;
	m_cpVariable_Transform = nullptr;
	m_cpVariable_Textures = nullptr;

	m_cpVariable_MeshInfo = m_wpEffectShader.lock()->GetRawVariableByName("g_tMeshInfo");
	m_cpVariable_Transform = m_wpEffectShader.lock()->GetMatrixVariableByName("g_matTransform");
	m_cpVariable_Textures = m_wpEffectShader.lock()->GetShaderResourceVariableByName("g_Textures");
}
void Engine::CEffectMeshComponent::SetTexture(const std::wstring& _wstrTextureName, ETextureType _eTextureType)
{
	const ComPtr<ID3D11ShaderResourceView>& cpTexture = CTextureHandler::GetInstance()->FindShaderResourceView(_wstrTextureName, _eTextureType);
	if (!cpTexture) { return; }

	switch (_eTextureType)
	{
	case ETextureType::DIFFUSE:		m_vecSRVs[0] = cpTexture.Get(); break;
	case ETextureType::NOISE:		m_vecSRVs[1] = cpTexture.Get(); break;
	case ETextureType::MASK:		m_vecSRVs[2] = cpTexture.Get(); break;
	case ETextureType::DISTORTION:	m_vecSRVs[3] = cpTexture.Get(); break;
	case ETextureType::DISSOLVE:	m_vecSRVs[4] = cpTexture.Get(); break;
	default: break;
	}
	return;
}

HRESULT Engine::CEffectMeshComponent::LoadFromJsonFile(const wstring& _wstrJsonFileName)
{
#pragma region RapidJSON

	// 1. JSON문서를 읽기 모드로 연다.
	ifstream isJsonpath(L"../../Resource/Effects/EffectMesh/" + _wstrJsonFileName + L".json");
	if (!isJsonpath.is_open()) { throw runtime_error("failed to open json file : CEffectMeshComponent"); }

	// 2. 문서 내용을 문자열로 변환
	string strJsonfile((istreambuf_iterator<char>(isJsonpath)), (istreambuf_iterator<char>()));

	// 3. JSON문서의 문자열을 파싱해온다.
	rapidjson::Document doc;
	doc.Parse(strJsonfile.c_str());

	// 4. JSON문서로부터 데이터를 읽는다.
	/* Type */
	m_tEffectMeshInfo.m_eRenderType				= (ERenderType)doc["RenderType"].GetUint();
	m_eRenderType								= m_tEffectMeshInfo.m_eRenderType;
	m_tEffectMeshInfo.m_eEffectType				= (EEffectType)doc["EffectType"].GetUint();
	m_tEffectMeshInfo.m_wstrModelFilePath		= CStringUtility::ToWideString(doc["Model File Path"].GetString());
	m_tEffectMeshInfo.m_wstrEffectShaderName	= CStringUtility::ToWideString(doc["Effect Shader Name"].GetString());

	if (doc.HasMember("Is Trail"))
	{
		m_tEffectMeshInfo.m_bTrail = doc["Is Trail"].GetBool();
	}
	if (doc.HasMember("Num Trails"))
	{
		m_tEffectMeshInfo.m_iNumTrails = doc["Num Trails"].GetInt();
	}
	if (doc.HasMember("Trail Term"))
	{
		m_tEffectMeshInfo.m_fTrailTerm = doc["Trail Term"].GetFloat();
	}

	/* Texture */
	m_tEffectMeshInfo.m_vDiffuseColor1.x = doc["Diffuse Color1"][0].GetFloat();
	m_tEffectMeshInfo.m_vDiffuseColor1.y = doc["Diffuse Color1"][1].GetFloat();
	m_tEffectMeshInfo.m_vDiffuseColor1.z = doc["Diffuse Color1"][2].GetFloat();
	m_tEffectMeshInfo.m_vDiffuseColor1.w = doc["Diffuse Color1"][3].GetFloat();

	m_tEffectMeshInfo.m_vDiffuseColor2.x = doc["Diffuse Color2"][0].GetFloat();
	m_tEffectMeshInfo.m_vDiffuseColor2.y = doc["Diffuse Color2"][1].GetFloat();
	m_tEffectMeshInfo.m_vDiffuseColor2.z = doc["Diffuse Color2"][2].GetFloat();
	m_tEffectMeshInfo.m_vDiffuseColor2.w = doc["Diffuse Color2"][3].GetFloat();

	m_tEffectMeshInfo.m_vDiffuseColor3.x = doc["Diffuse Color3"][0].GetFloat();
	m_tEffectMeshInfo.m_vDiffuseColor3.y = doc["Diffuse Color3"][1].GetFloat();
	m_tEffectMeshInfo.m_vDiffuseColor3.z = doc["Diffuse Color3"][2].GetFloat();
	m_tEffectMeshInfo.m_vDiffuseColor3.w = doc["Diffuse Color3"][3].GetFloat();

	m_tEffectMeshInfo.m_wstrDiffuseTextureName		= CStringUtility::ToWideString(doc["Diffuse Texture Name"].GetString());
	m_tEffectMeshInfo.m_wstrNoiseTextureName		= CStringUtility::ToWideString(doc["Noise Texture Name"].GetString());
	m_tEffectMeshInfo.m_wstrMaskTextureName			= CStringUtility::ToWideString(doc["Mask Texture Name"].GetString());
	m_tEffectMeshInfo.m_wstrDistortionTextureName	= CStringUtility::ToWideString(doc["Distortion Texture Name"].GetString());
	m_tEffectMeshInfo.m_wstrDissolveTextureName		= CStringUtility::ToWideString(doc["Dissolve Texture Name"].GetString());

	m_tEffectMeshInfo.m_bSprite			= doc["Is Sprite"].GetBool();
	m_tEffectMeshInfo.m_iSpriteRow		= doc["Sprite Row"].GetInt();
	m_tEffectMeshInfo.m_iSpriteCol		= doc["Sprite Col"].GetInt();
	m_tEffectMeshInfo.m_fSpriteSpeed	= doc["Sprite Speed"].GetFloat();

	/* Option */
	m_tEffectMeshInfo.m_vScale.x		= doc["Scale"][0].GetFloat();
	m_tEffectMeshInfo.m_vScale.y		= doc["Scale"][1].GetFloat();
	m_tEffectMeshInfo.m_vScale.z		= doc["Scale"][2].GetFloat();
	m_tEffectMeshInfo.m_vRotation.x		= doc["Rotation"][0].GetFloat();
	m_tEffectMeshInfo.m_vRotation.y		= doc["Rotation"][1].GetFloat();
	m_tEffectMeshInfo.m_vRotation.z		= doc["Rotation"][2].GetFloat();
	m_tEffectMeshInfo.m_vTranslation.x	= doc["Translation"][0].GetFloat();
	m_tEffectMeshInfo.m_vTranslation.y	= doc["Translation"][1].GetFloat();
	m_tEffectMeshInfo.m_vTranslation.z	= doc["Translation"][2].GetFloat();

	m_tEffectMeshInfo.m_bAnimation	= doc["Is Anim"].GetBool();
	m_tEffectMeshInfo.m_bLoop		= doc["Is Loop"].GetBool();
	m_tEffectMeshInfo.m_bUVMoveLoop = doc["Is UVMove Loop"].GetBool();

	m_tEffectMeshInfo.m_bGlow				= doc["Is Glow"].GetBool();
	m_tEffectMeshInfo.m_bBloom				= doc["Is Bloom"].GetBool();
	m_tEffectMeshInfo.m_bAlphaAttenuation	= doc["Is AlphaAttenuation"].GetBool();

	m_tEffectMeshInfo.m_bDiffuse	= doc["Use Diffuse Texture"].GetBool();
	m_tEffectMeshInfo.m_bNoise		= doc["Use Noise Texture"].GetBool();
	m_tEffectMeshInfo.m_bMask		= doc["Use Mask Texture"].GetBool();
	m_tEffectMeshInfo.m_bDistortion = doc["Use Distortion Texture"].GetBool();
	m_tEffectMeshInfo.m_bDissolve	= doc["Use Dissolve Texture"].GetBool();

	/* Base Value */
	m_tEffectMeshInfo.m_fDistortionWeight		= doc["Distortion Weight"].GetFloat();
	m_tEffectMeshInfo.m_fDissolveWeight			= doc["Dissolve Weight"].GetFloat();
	m_tEffectMeshInfo.m_fGlowWeight				= doc["Glow Weight"].GetFloat();
	m_tEffectMeshInfo.m_fBloomWeight			= doc["Bloom Weight"].GetFloat();
	m_tEffectMeshInfo.m_fAlphaAttenuationWeight = doc["AlphaAttenuation Weight"].GetFloat();
	m_tEffectMeshInfo.m_vLifeTime.x				= doc["Life Time"][0].GetFloat();
	m_tEffectMeshInfo.m_vLifeTime.y				= doc["Life Time"][1].GetFloat();
	m_tEffectMeshInfo.m_vUVMoveSpeed.x			= doc["UVMove Speed"][0].GetFloat();
	m_tEffectMeshInfo.m_vUVMoveSpeed.y			= doc["UVMove Speed"][1].GetFloat();


	/* KeyFrame */
	m_tEffectMeshInfo.m_iNumKeyFrames = doc["Num KeyFrames"].GetUint();
	m_tEffectMeshInfo.m_vecKeyFrames.clear();
	m_tEffectMeshInfo.m_vecKeyFrames.reserve(m_tEffectMeshInfo.m_iNumKeyFrames);
	m_tEffectMeshInfo.m_vecKeyFrames.resize(m_tEffectMeshInfo.m_iNumKeyFrames);

	for (uint32 innerIndex = 0; innerIndex < m_tEffectMeshInfo.m_iNumKeyFrames; ++innerIndex)
	{
		rapidjson::Value& tEffectMeshKeyFrame = doc["KeyFrames"][innerIndex];

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_iPassIndex = tEffectMeshKeyFrame["Effect Shader Pass Index"].GetInt();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fTime = tEffectMeshKeyFrame["Time"].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.x			= tEffectMeshKeyFrame["Color1"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.y			= tEffectMeshKeyFrame["Color1"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.z			= tEffectMeshKeyFrame["Color1"][2].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.w			= tEffectMeshKeyFrame["Color1"][3].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor1Weight		= tEffectMeshKeyFrame["Color1 Weight"].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eColor1EasingType	= (EEasingType)tEffectMeshKeyFrame["Color1 Easing Type"].GetUint();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor1EasingExpo	= tEffectMeshKeyFrame["Color1 Easing Expo"].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.x			= tEffectMeshKeyFrame["Color2"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.y			= tEffectMeshKeyFrame["Color2"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.z			= tEffectMeshKeyFrame["Color2"][2].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.w			= tEffectMeshKeyFrame["Color2"][3].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor2Weight		= tEffectMeshKeyFrame["Color2 Weight"].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eColor2EasingType	= (EEasingType)tEffectMeshKeyFrame["Color2 Easing Type"].GetUint();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor2EasingExpo	= tEffectMeshKeyFrame["Color2 Easing Expo"].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.x			= tEffectMeshKeyFrame["Color3"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.y			= tEffectMeshKeyFrame["Color3"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.z			= tEffectMeshKeyFrame["Color3"][2].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.w			= tEffectMeshKeyFrame["Color3"][3].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor3Weight		= tEffectMeshKeyFrame["Color3 Weight"].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eColor3EasingType	= (EEasingType)tEffectMeshKeyFrame["Color3 Easing Type"].GetUint();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor3EasingExpo	= tEffectMeshKeyFrame["Color3 Easing Expo"].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vScale.x			= tEffectMeshKeyFrame["Scale"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vScale.y			= tEffectMeshKeyFrame["Scale"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vScale.z			= tEffectMeshKeyFrame["Scale"][2].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eScaleEasingType = (EEasingType)tEffectMeshKeyFrame["Scale Easing Type"].GetUint();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fScaleEasingExpo = tEffectMeshKeyFrame["Scale Easing Expo"].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vRotation.x		= tEffectMeshKeyFrame["Rotation"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vRotation.y		= tEffectMeshKeyFrame["Rotation"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vRotation.z		= tEffectMeshKeyFrame["Rotation"][2].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fRotateWeight	= tEffectMeshKeyFrame["Rotate Weight"].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eRotationEasingType = (EEasingType)tEffectMeshKeyFrame["Rotation Easing Type"].GetUint();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fRotationEasingExpo = tEffectMeshKeyFrame["Rotation Easing Expo"].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vTranslation.x	= tEffectMeshKeyFrame["Translation"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vTranslation.y	= tEffectMeshKeyFrame["Translation"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vTranslation.z	= tEffectMeshKeyFrame["Translation"][2].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eTranslationEasingType = (EEasingType)tEffectMeshKeyFrame["Translation Easing Type"].GetUint();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fTranslationEasingExpo = tEffectMeshKeyFrame["Translation Easing Expo"].GetFloat();
	}
#pragma endregion

	return S_OK;
}
HRESULT Engine::CEffectMeshComponent::WriteToJsonFile(const wstring& _wstrJsonFileName, rapidjson::StringBuffer& _strBuffer, rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer)
{
	_writer.StartObject();

	string strName = "Effect Mesh - " + CStringUtility::ToString(GetOwnerGameObject()->GetGameObjectName());
	_writer.Key("Effect Name");
	_writer.String(strName.c_str());

	_writer.Key("RenderType");
	_writer.Int((int32)m_tEffectMeshInfo.m_eRenderType);
	_writer.Key("EffectType");
	_writer.Int((int32)m_tEffectMeshInfo.m_eEffectType);
	_writer.Key("Model File Path");
	_writer.String(CStringUtility::ToString(m_tEffectMeshInfo.m_wstrModelFilePath).c_str());
	_writer.Key("Effect Shader Name");
	_writer.String(CStringUtility::ToString(m_tEffectMeshInfo.m_wstrEffectShaderName).c_str());

	_writer.Key("Is Trail");
	_writer.Bool(m_tEffectMeshInfo.m_bTrail);
	_writer.Key("Num Trails");
	_writer.Int(m_tEffectMeshInfo.m_iNumTrails);
	_writer.Key("Trail Term");
	_writer.Double(m_tEffectMeshInfo.m_fTrailTerm);

	_writer.Key("Scale");
	_writer.StartArray();
	_writer.Double(m_tEffectMeshInfo.m_vScale.x);
	_writer.Double(m_tEffectMeshInfo.m_vScale.y);
	_writer.Double(m_tEffectMeshInfo.m_vScale.z);
	_writer.EndArray();

	_writer.Key("Rotation");
	_writer.StartArray();
	_writer.Double(m_tEffectMeshInfo.m_vRotation.x);
	_writer.Double(m_tEffectMeshInfo.m_vRotation.y);
	_writer.Double(m_tEffectMeshInfo.m_vRotation.z);
	_writer.EndArray();

	_writer.Key("Translation");
	_writer.StartArray();
	_writer.Double(m_tEffectMeshInfo.m_vTranslation.x);
	_writer.Double(m_tEffectMeshInfo.m_vTranslation.y);
	_writer.Double(m_tEffectMeshInfo.m_vTranslation.z);
	_writer.EndArray();

	_writer.Key("Diffuse Color1");
	_writer.StartArray();
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor1.x);
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor1.y);
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor1.z);
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor1.w);
	_writer.EndArray();

	_writer.Key("Diffuse Color2");
	_writer.StartArray();
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor2.x);
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor2.y);
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor2.z);
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor2.w);
	_writer.EndArray();

	_writer.Key("Diffuse Color3");
	_writer.StartArray();
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor3.x);
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor3.y);
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor3.z);
	_writer.Double(m_tEffectMeshInfo.m_vDiffuseColor3.w);
	_writer.EndArray();

	_writer.Key("Diffuse Texture Name");
	_writer.String(CStringUtility::ToString(m_tEffectMeshInfo.m_wstrDiffuseTextureName).c_str());
	_writer.Key("Noise Texture Name");
	_writer.String(CStringUtility::ToString(m_tEffectMeshInfo.m_wstrNoiseTextureName).c_str());
	_writer.Key("Mask Texture Name");
	_writer.String(CStringUtility::ToString(m_tEffectMeshInfo.m_wstrMaskTextureName).c_str());
	_writer.Key("Distortion Texture Name");
	_writer.String(CStringUtility::ToString(m_tEffectMeshInfo.m_wstrDistortionTextureName).c_str());
	_writer.Key("Dissolve Texture Name");
	_writer.String(CStringUtility::ToString(m_tEffectMeshInfo.m_wstrDissolveTextureName).c_str());

	_writer.Key("Is Sprite");
	_writer.Bool(m_tEffectMeshInfo.m_bSprite);
	_writer.Key("Sprite Row");
	_writer.Int(m_tEffectMeshInfo.m_iSpriteRow);
	_writer.Key("Sprite Col");
	_writer.Int(m_tEffectMeshInfo.m_iSpriteCol);
	_writer.Key("Sprite Speed");
	_writer.Double(m_tEffectMeshInfo.m_fSpriteSpeed);

	_writer.Key("Is Anim");
	_writer.Bool(m_tEffectMeshInfo.m_bAnimation);
	_writer.Key("Is Loop");
	_writer.Bool(m_tEffectMeshInfo.m_bLoop);
	_writer.Key("Is UVMove Loop");
	_writer.Bool(m_tEffectMeshInfo.m_bUVMoveLoop);
	_writer.Key("Is Glow");
	_writer.Bool(m_tEffectMeshInfo.m_bGlow);
	_writer.Key("Is Bloom");
	_writer.Bool(m_tEffectMeshInfo.m_bBloom);
	_writer.Key("Is AlphaAttenuation");
	_writer.Bool(m_tEffectMeshInfo.m_bAlphaAttenuation);
	_writer.Key("Use Diffuse Texture");
	_writer.Bool(m_tEffectMeshInfo.m_bDiffuse);
	_writer.Key("Use Noise Texture");
	_writer.Bool(m_tEffectMeshInfo.m_bNoise);
	_writer.Key("Use Mask Texture");
	_writer.Bool(m_tEffectMeshInfo.m_bMask);
	_writer.Key("Use Distortion Texture");
	_writer.Bool(m_tEffectMeshInfo.m_bDistortion);
	_writer.Key("Use Dissolve Texture");
	_writer.Bool(m_tEffectMeshInfo.m_bDissolve);

	_writer.Key("Distortion Weight");
	_writer.Double(m_tEffectMeshInfo.m_fDistortionWeight);
	_writer.Key("Dissolve Weight");
	_writer.Double(m_tEffectMeshInfo.m_fDissolveWeight);
	_writer.Key("Glow Weight");
	_writer.Double(m_tEffectMeshInfo.m_fGlowWeight);
	_writer.Key("Bloom Weight");
	_writer.Double(m_tEffectMeshInfo.m_fBloomWeight);
	_writer.Key("AlphaAttenuation Weight");
	_writer.Double(m_tEffectMeshInfo.m_fAlphaAttenuationWeight);

	_writer.Key("Life Time");
	_writer.StartArray();
	_writer.Double(m_tEffectMeshInfo.m_vLifeTime.x);
	_writer.Double(m_tEffectMeshInfo.m_vLifeTime.y);
	_writer.EndArray();

	_writer.Key("UVMove Speed");
	_writer.StartArray();
	_writer.Double(m_tEffectMeshInfo.m_vUVMoveSpeed.x);
	_writer.Double(m_tEffectMeshInfo.m_vUVMoveSpeed.y);
	_writer.EndArray();

	_writer.Key("Num KeyFrames");
	_writer.Int(m_tEffectMeshInfo.m_iNumKeyFrames);

	_writer.Key("KeyFrames");
	_writer.StartArray();

	const vector<FEffectMeshKeyFrame> vecEffectMeshKeyFrames = GetKeyFrames();

	for (size_t i = 0; i < vecEffectMeshKeyFrames.size(); ++i)
	{
		_writer.StartObject();

		string strKeyFrameIndex = "KeyFrame" + to_string(i);
		_writer.Key("Index");
		_writer.String(strKeyFrameIndex.data());

		_writer.Key("Effect Shader Pass Index");
		_writer.Int(vecEffectMeshKeyFrames[i].m_iPassIndex);

		_writer.Key("Time");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fTime);

		_writer.Key("Render Culled");
		_writer.Bool(vecEffectMeshKeyFrames[i].m_bRenderCulled);

		_writer.Key("Color1");
		_writer.StartArray();
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor1.x);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor1.y);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor1.z);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor1.w);
		_writer.EndArray();
		_writer.Key("Color1 Weight");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fColor1Weight);
		_writer.Key("Color1 Easing Type");
		_writer.Uint((uint32)vecEffectMeshKeyFrames[i].m_eColor1EasingType);
		_writer.Key("Color1 Easing Expo");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fColor1EasingExpo);

		_writer.Key("Color2");
		_writer.StartArray();
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor2.x);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor2.y);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor2.z);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor2.w);
		_writer.EndArray();
		_writer.Key("Color2 Weight");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fColor2Weight);
		_writer.Key("Color2 Easing Type");
		_writer.Uint((uint32)vecEffectMeshKeyFrames[i].m_eColor2EasingType);
		_writer.Key("Color2 Easing Expo");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fColor2EasingExpo);

		_writer.Key("Color3");
		_writer.StartArray();
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor3.x);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor3.y);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor3.z);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vColor3.w);
		_writer.EndArray();
		_writer.Key("Color3 Weight");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fColor3Weight);
		_writer.Key("Color3 Easing Type");
		_writer.Uint((uint32)vecEffectMeshKeyFrames[i].m_eColor3EasingType);
		_writer.Key("Color3 Easing Expo");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fColor3EasingExpo);

		_writer.Key("Scale");
		_writer.StartArray();
		_writer.Double(vecEffectMeshKeyFrames[i].m_vScale.x);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vScale.y);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vScale.z);
		_writer.EndArray();
		_writer.Key("Scale Easing Type");
		_writer.Uint((uint32)vecEffectMeshKeyFrames[i].m_eScaleEasingType);
		_writer.Key("Scale Easing Expo");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fScaleEasingExpo);

		_writer.Key("Rotation");
		_writer.StartArray();
		_writer.Double(vecEffectMeshKeyFrames[i].m_vRotation.x);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vRotation.y);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vRotation.z);
		_writer.EndArray();
		_writer.Key("Rotation Easing Type");
		_writer.Uint((uint32)vecEffectMeshKeyFrames[i].m_eRotationEasingType);
		_writer.Key("Rotation Easing Expo");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fRotationEasingExpo);

		_writer.Key("Translation");
		_writer.StartArray();
		_writer.Double(vecEffectMeshKeyFrames[i].m_vTranslation.x);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vTranslation.y);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vTranslation.z);
		_writer.EndArray();
		_writer.Key("Translation Easing Type");
		_writer.Uint((uint32)vecEffectMeshKeyFrames[i].m_eTranslationEasingType);
		_writer.Key("Translation Easing Expo");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fTranslationEasingExpo);

		_writer.Key("Rotate Weight");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fRotateWeight);

		_writer.EndObject();
	}
	_writer.EndArray();

	_writer.EndObject();

	return S_OK;
}
