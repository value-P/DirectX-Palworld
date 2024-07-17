
// Default
#include "pch.h"
#include "ParticleComponent.h"
#include "StringUtility.h"
#include "MathUtility.h"

// Module, Handler, Manager
#include "EngineModule.h"
#include "DeviceManager.h"
#include "CameraManager.h"
#include "RenderManager.h"
#include "ShaderHandler.h"
#include "TextureHandler.h"

// GameObject
#include "GameObject.h"

// etc
#include "Instancing.h"
#include "EffectShader.h"
#include "Material.h"
#include "Model.h"
#include "Bone.h"

std::shared_ptr<CParticleComponent> Engine::CParticleComponent::Create(const std::shared_ptr<FParticleComponentDesc>& _spParticleComponentDesc, FParticleInfo _tParticleInfo)
{
	const std::shared_ptr<CParticleComponent> spParticleComponent = make_shared<CParticleComponent>();

	spParticleComponent->SetParticleInfo(_tParticleInfo);

	ENSUREF(spParticleComponent->InitializeComponent(_spParticleComponentDesc), L"Failed to initialize particle component");
	return spParticleComponent;
}
std::shared_ptr<CParticleComponent> Engine::CParticleComponent::CreateFromJson(const std::shared_ptr<FParticleComponentDesc>& _spParticleComponentDesc, const std::wstring& _wstrJsonFileName)
{
	const std::shared_ptr<CParticleComponent> spParticleComponent = make_shared<CParticleComponent>();

	spParticleComponent->LoadFromJsonFile(_wstrJsonFileName);

	ENSUREF(spParticleComponent->InitializeComponent(_spParticleComponentDesc), L"Failed to initialize particle component");
	return spParticleComponent;
}

HRESULT Engine::CParticleComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// effect component
	if (FAILED(CEffectComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// render type reinitialize
	m_eRenderType = m_tParticleInfo.m_eRenderType;

	// etc
	shared_ptr<CDeviceManager> spDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_cpDevice = spDeviceManager->GetDevice();
	m_cpDeviceContext = spDeviceManager->GetDeviceContext();

	FindEffectShader();
	FindShaderResourceViews();
	InitializeBuffer();
	InitializeInstanceValue();

	m_vOriginPos = m_wpOwnerGameObject.lock()->GetPosition().value() + m_tParticleInfo.m_vOffset;
	return S_OK;
}
HRESULT Engine::CParticleComponent::BeginPlay()
{
	if (FAILED(CEffectComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}
int32 Engine::CParticleComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CEffectComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}
int32 Engine::CParticleComponent::TickComponent(float _fDeltaSeconds)
{
	if (m_bPause) { return CEffectComponent::TickComponent(_fDeltaSeconds); }

	ResetOwnerPosition();

	// 이펙트 진행
	PlayEffect(_fDeltaSeconds);

	return CEffectComponent::TickComponent(_fDeltaSeconds);
}
int32 Engine::CParticleComponent::PostTickComponent(float _fDeltaSeconds)
{
	m_eRenderType = m_tParticleInfo.m_eRenderType;

	switch (CEffectComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}
HRESULT Engine::CParticleComponent::EndPlay()
{
	if (FAILED(CEffectComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}
void Engine::CParticleComponent::Render(int32 _iShaderPath)
{
	if (m_fTrackPosition < m_tParticleInfo.m_vLifeTime.x || m_bCulled) { return; }

	ENSUREF(BindVariable(), L"Failed to BindVariable : CParticleComponent");
	ENSUREF(m_wpEffectShader.lock()->BeginPass(m_iPassIndex), L"Failed to BeginPass : CParticleComponent");
	ENSUREF(m_spInstancing->Render(), L"Failed to Render : CParticleComponent");

	CEffectComponent::Render(_iShaderPath);
}
void Engine::CParticleComponent::Release()
{
	m_spInstancing->Release();
	m_spInstancing = nullptr;

	CEffectComponent::Release();
}
HRESULT Engine::CParticleComponent::BindVariable()
{
	/* World */
	m_tEffectInfoParticle.matWorld = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();

	/* Texture Flags*/
	m_tEffectInfoParticle.bDiffuse = m_tParticleInfo.m_bDiffuse;
	m_tEffectInfoParticle.bMask = m_tParticleInfo.m_bMask;

	/* Option */
	m_tEffectInfoParticle.bBillboard = m_tParticleInfo.m_bBillboard;
	m_tEffectInfoParticle.bDirectional = m_tParticleInfo.m_bDirectional;
	m_tEffectInfoParticle.fSizeRatio = m_tLerpedData.m_fSizeRatio;

	/* Size */
	m_tEffectInfoParticle.vSize = m_tParticleInfo.m_vSize;
	m_tEffectInfoParticle.fParticlePad1 = 0.f;

	/* Lerp */
	if (m_tParticleInfo.m_vecKeyFrames.empty() || 2 > m_tParticleInfo.m_vecKeyFrames.size())
	{
		m_tEffectInfoParticle.vColor1 = m_tParticleInfo.m_vColor1;
		m_tEffectInfoParticle.vColor2 = m_tParticleInfo.m_vColor2;
		m_tEffectInfoParticle.vColor3 = m_tParticleInfo.m_vColor3;

		m_tEffectInfoParticle.fSizeRatio = 1.f;
	}
	else
	{
		m_tEffectInfoParticle.vColor1 = m_tLerpedData.m_vColor1;
		m_tEffectInfoParticle.vColor2 = m_tLerpedData.m_vColor2;
		m_tEffectInfoParticle.vColor3 = m_tLerpedData.m_vColor3;

		m_tEffectInfoParticle.fSizeRatio = m_tLerpedData.m_fSizeRatio;
	}

	/* TrackPosition */
	m_tEffectInfoParticle.fTrackPosition = m_fTrackPosition;

	// HRESULT hr = m_cpVariable_MeshInfo->SetRawValue(&m_tEffectInfoParticle, 0, sizeof(FEffectInfo_Particle));

	ENSURE(m_cpVariable_MeshInfo->SetRawValue(&m_tEffectInfoParticle, 0, sizeof(m_tEffectInfoParticle)));
	ENSURE(m_cpVariable_Textures->SetResourceArray(m_vecSRVs.data(), 0, (int32)m_vecSRVs.size()));


	return S_OK;
}

void Engine::CParticleComponent::InitializeInstanceValue()
{
	/* Render Type */
	m_eRenderType = m_tParticleInfo.m_eRenderType;

	/* Vector Reserve & Resize */
	m_vecSpeed.reserve(m_tParticleInfo.m_iNumInstance);
	m_vecSpeed.resize(m_tParticleInfo.m_iNumInstance);

	m_vecLifeTime.reserve(m_tParticleInfo.m_iNumInstance);
	m_vecLifeTime.resize(m_tParticleInfo.m_iNumInstance);

	m_vecLifeTimeAcc.reserve(m_tParticleInfo.m_iNumInstance);
	m_vecLifeTimeAcc.resize(m_tParticleInfo.m_iNumInstance);

	m_vecSpriteLifeTimeAcc.reserve(m_tParticleInfo.m_iNumInstance);
	m_vecSpriteLifeTimeAcc.resize(m_tParticleInfo.m_iNumInstance);

	m_vecRotation.reserve(m_tParticleInfo.m_iNumInstance);
	m_vecRotation.resize(m_tParticleInfo.m_iNumInstance);

	m_vecDirection.reserve(m_tParticleInfo.m_iNumInstance);
	m_vecDirection.resize(m_tParticleInfo.m_iNumInstance);

	m_vecAlphaDecreaseAcc.reserve(m_tParticleInfo.m_iNumInstance);
	m_vecAlphaDecreaseAcc.resize(m_tParticleInfo.m_iNumInstance);

	ResetAllInstanceValue();
}
HRESULT Engine::CParticleComponent::FindEffectShader()
{
	shared_ptr<CEffectShader> spEffectShader = CShaderHandler::GetInstance()->FindEffectShader(m_tParticleInfo.m_wstrEffectShaderName);
	CHECKF(spEffectShader, L"Failed to FindEffectShader : CParticleComponent");

	m_wpEffectShader = spEffectShader;

	m_cpVariable_MeshInfo = nullptr;
	m_cpVariable_Textures = nullptr;

	m_cpVariable_MeshInfo = m_wpEffectShader.lock()->GetRawVariableByName("g_tEffectInfo_Particle");
	m_cpVariable_Textures = m_wpEffectShader.lock()->GetShaderResourceVariableByName("g_Textures");

	return S_OK;
}
HRESULT Engine::CParticleComponent::FindShaderResourceViews()
{
	m_vecSRVs.resize(2, CTextureHandler::GetInstance()->FindShaderResourceView(L"Default", ETextureType::DIFFUSE).Get());
	if (m_tParticleInfo.m_bDiffuse)
	{
		m_vecSRVs[0] = CTextureHandler::GetInstance()->FindShaderResourceView(m_tParticleInfo.m_wstrDiffuseTextureName, ETextureType::DIFFUSE).Get();
		CHECKF(m_vecSRVs[0], L"Failed to Find DiffuseTexture : CParticleComponent");
	}
	if (m_tParticleInfo.m_bMask)
	{
		m_vecSRVs[1] = CTextureHandler::GetInstance()->FindShaderResourceView(m_tParticleInfo.m_wstrMaskTextureName, ETextureType::MASK).Get();
		CHECKF(m_vecSRVs[1], L"Failed to Find MaskTexture : CParticleComponent");
	}
	if (m_tParticleInfo.m_bBloom)
	{
		// 디졸브 텍스처
		m_cpDissolveTexture = CTextureHandler::GetInstance()->FindShaderResourceView(L"Eff_Noise_25", ETextureType::NOISE);
		CHECKF(m_cpDissolveTexture.Get(), L"Failed to Find DissolveTexture : CParticleComponent");
	}

	return S_OK;
}
HRESULT Engine::CParticleComponent::InitializeBuffer()
{
	m_spInstancing = CInstancing::Create(m_tParticleInfo);

	return S_OK;
}

void Engine::CParticleComponent::PlayEffect(float _fDeltaSeconds)
{
	m_fTrackPosition += _fDeltaSeconds;

	// 1. 트랙포지션이 최대 라이프 타임을 넘어갔을 경우(종료)
	if (m_fTrackPosition >= m_tParticleInfo.m_vLifeTime.y)
	{
		if (m_tParticleInfo.m_bLoop)
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
	else if (m_fTrackPosition < m_tParticleInfo.m_vLifeTime.x)
	{
		NoneActivate();
		return;
	}
	// 3. 트랙포지션이 라이프 타임 사이에 있고, 첫 번째 키프레임 타임보다 큰 경우(활성화)
	else if (m_fTrackPosition >= m_tParticleInfo.m_vLifeTime.x)
	{
		m_wpOwnerGameObject.lock()->SetCulled(false);
		InterpolationBetweenKeyFrame(_fDeltaSeconds);
		TickIntegrated(_fDeltaSeconds);
	}

	return;
}
void Engine::CParticleComponent::InterpolationBetweenKeyFrame(float _fDeltaSeconds)
{
	// 키프레임 컨테이너가 비어있는 경우와 컨테이너 크기가 2미만인 경우 보간을 시행하지 않습니다.
	if (m_tParticleInfo.m_vecKeyFrames.empty() || 2 > m_tParticleInfo.m_vecKeyFrames.size())
	{
		// 러프 데이터 초기값으로 초기화
		m_tLerpedData = FParticleKeyFrame{};
		return;
	}

	// 트랙포지션이 0인 경우, 현재 키프레임을 0으로 설정합니다.
	if (0.f == m_fTrackPosition) { m_iCurKeyFrame = 0; }

	// 마지막 키프레임
	FParticleKeyFrame tLastKeyFrame = m_tParticleInfo.m_vecKeyFrames.back();

	// 트랙포지션이 마지막 키프레임의 시간을 넘어간 경우
	if (m_fTrackPosition > tLastKeyFrame.m_fTime)
	{
		if (m_tParticleInfo.m_bLoop)
		{
			// Loop();
			m_fTrackPosition = 0.f;
			m_iCurKeyFrame = 0;
			m_tLerpedData = FParticleKeyFrame{};
			EndLifeTime();
		}
		else
		{
			EndLifeTime();
		}
	}
	else
	{
		// 현재 트랙포지션이 다음 키프레임의 타임을 넘어간 경우, 키프레임을 1 증가시킵니다.
		if (m_fTrackPosition >= m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fTime) { ++m_iCurKeyFrame; }

		/* 패스 인덱스 */
		m_iPassIndex = m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame].m_iPassIndex;

		/* 렌더 컬 */
		// m_bCulled = m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame].m_bRenderCulled;

		/* 부모 오브젝트의 포지션 */
		m_wpOwnerGameObject.lock()->SetPosition(m_vOriginPos); // 파티클을 런타임에 이동시키고 싶다면, m_vOriginpos를 객체 위치로 갱신시켜주던가 m_vOffset을 이동시켜주면 됩니다.

		// 보간 비율 : 현재 키프레임과 다음 키프레임 사이에서 트랙포지션의 비율((트랙 포지션 - 현재 키프레임 타임) / (다음 키프레임 타임 - 현재 키프레임 타임))
		float fInterpolationRatio = (m_fTrackPosition - m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame].m_fTime) / (m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fTime - m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame].m_fTime);
		if (0.99f <= fInterpolationRatio)
		{
			fInterpolationRatio = 1.f;
		}

		/* ※※ 키프레임 사이의 보간할 값들을 아래에 추가하면 됩니다. ※※ */
		/* Color1 */
		m_tLerpedData.m_vColor1 = EaseByType(m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eColor1EasingType,
											 m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame].m_vColor1,
											 m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_vColor1,
											 fInterpolationRatio);
		/* Color2 */
		m_tLerpedData.m_vColor2 = EaseByType(m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eColor2EasingType,
											 m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame].m_vColor2,
											 m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_vColor2,
											 fInterpolationRatio);
		/* Color3 */
		m_tLerpedData.m_vColor3 = EaseByType(m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eColor3EasingType,
											 m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame].m_vColor3,
											 m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_vColor3,
											 fInterpolationRatio);

		/* 크기 비율 */
		m_tLerpedData.m_fSizeRatio = EaseByType(m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eSizeEasingType,
												m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame].m_fSizeRatio,
												m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fSizeRatio,
												fInterpolationRatio);

		/* 속도 비율 */
		m_tLerpedData.m_fSpeedRatio = EaseByType(m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_eSpeedEasingType,
												 m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame].m_fSpeedRatio,
												 m_tParticleInfo.m_vecKeyFrames[m_iCurKeyFrame + 1].m_fSpeedRatio,
												 fInterpolationRatio);
	}
}

void Engine::CParticleComponent::UVMove(float _fDeltaSeconds)
{

}

void Engine::CParticleComponent::TickIntegrated(float _fDeltaSeconds)
{
	D3D11_MAPPED_SUBRESOURCE tSubResource = {};

	m_cpDeviceContext->Map(m_spInstancing->GetInstanceBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);

	FRightUpLookPositionColor* pInstance = ((FRightUpLookPositionColor*)tSubResource.pData);

	for (int32 i = 0; i < m_tParticleInfo.m_iNumInstance; ++i)
	{
		pInstance[i].m_vPrevTranslation = pInstance[i].m_vTranslation;

		Vector4 vRight	= pInstance[i].m_vRight;
		Vector4 vUp		= pInstance[i].m_vUp;
		Vector4 vLook	= pInstance[i].m_vLook;
		Vector4 vPivot = GetOwnerPosition();

		// 회전 변환
		Matrix matRotation = XMMatrixIdentity();
		matRotation = matRotation * XMMatrixRotationX(XMConvertToRadians(m_vecRotation[i].x * _fDeltaSeconds));
		matRotation = matRotation * XMMatrixRotationY(XMConvertToRadians(m_vecRotation[i].y * _fDeltaSeconds));
		matRotation = matRotation * XMMatrixRotationZ(XMConvertToRadians(m_vecRotation[i].z * _fDeltaSeconds));
		XMStoreFloat4(&pInstance[i].m_vRight, XMVector4Transform(vRight, matRotation));
		XMStoreFloat4(&pInstance[i].m_vUp, XMVector4Transform(vUp, matRotation));
		XMStoreFloat4(&pInstance[i].m_vLook, XMVector4Transform(vLook, matRotation));

		Vector4 vOrigin = Vector4(m_vOriginPos.x, m_vOriginPos.y, m_vOriginPos.z, 1.f);
		// 이동 변환
		if (EParticleType::REVOLUTION == m_tParticleInfo.m_eParticleType)
		{
			Vector3 vDirFromPivot = XMVector4Normalize(pInstance[i].m_vTranslation - vPivot);
			Vector4 vDirFromPivotV4 = Vector4(vDirFromPivot.x, vDirFromPivot.y, vDirFromPivot.z, 0.f);

			Vector3 vRevolutionDir = vDirFromPivot.Cross(Vector3(0.f, 1.f, 0.f));
			vRevolutionDir.Normalize();

			Vector3 vReverseDir = vRevolutionDir.Cross(Vector3(0.f, 1.f, 0.f));
			vReverseDir.Normalize();

			Vector4 vRevolutionDirV4 = Vector4(vRevolutionDir.x, vRevolutionDir.y, vRevolutionDir.z, 0.f);
			Vector4 vReverseDirV4 = Vector4(vReverseDir.x, vReverseDir.y, vReverseDir.z, 0.f);

			m_vecDirection[i] = vRevolutionDirV4;
			pInstance[i].m_vTranslation = Vector4(pInstance[i].m_vTranslation) + m_vecDirection[i] * m_vecSpeed[i] * m_tLerpedData.m_fSpeedRatio * _fDeltaSeconds;
		}
		else if (EParticleType::TONADO == m_tParticleInfo.m_eParticleType)
		{
			Vector3 vDirFromPivot = XMVector4Normalize(pInstance[i].m_vTranslation - vPivot);
			Vector4 vDirFromPivotV4 = Vector4(vDirFromPivot.x, vDirFromPivot.y, vDirFromPivot.z, 0.f);

			Vector3 vRevolutionDir = vDirFromPivot.Cross(Vector3(0.f, 1.f, 0.f));
			vRevolutionDir.Normalize();

			Vector3 vReverseDir = vRevolutionDir.Cross(Vector3(0.f, 1.f, 0.f));
			vReverseDir.Normalize();

			Vector4 vRevolutionDirV4 = Vector4(vRevolutionDir.x, vRevolutionDir.y, vRevolutionDir.z, 0.f);
			Vector4 vReverseDirV4 = Vector4(vReverseDir.x, vReverseDir.y, vReverseDir.z, 0.f);

			m_vecDirection[i] = vRevolutionDirV4;
			pInstance[i].m_vTranslation = Vector4(pInstance[i].m_vTranslation) + m_vecDirection[i] * m_vecSpeed[i] * m_tLerpedData.m_fSpeedRatio * _fDeltaSeconds + vReverseDirV4 * _fDeltaSeconds /* + vDirFromPivotV4 * _fDeltaSeconds*//* + Vector4(0.f, 1.f, 0.f, 0.f) * _fDeltaSeconds*/;
		}
		else if (EParticleType::FOLLOW == m_tParticleInfo.m_eParticleType)
		{
			pInstance[i].m_vTranslation = vOrigin;
		}
		else if(EParticleType::SPREAD == m_tParticleInfo.m_eParticleType && m_tParticleInfo.m_bBloom)
		{
			// 여기서 피벗 갱신해주자
			Vector3 vPivot = Vector3(0.f);
			Vector3 vOwnerPos = GetOwnerPositionV3();
			Matrix matOwner = GetOwnerMatrix();

			vPivot = vOwnerPos +
				matOwner.Right() * m_tParticleInfo.m_vPivot.x +
				matOwner.Up() * m_tParticleInfo.m_vPivot.y +
				matOwner.Forward() * m_tParticleInfo.m_vPivot.z;

			Vector4 vPivotV4 = Vector4(vPivot.x, vPivot.y, vPivot.z, 1.f);

			m_vecDirection[i] = XMVector4Normalize(pInstance[i].m_vTranslation - vPivotV4);
			m_vecDirection[i].w = 0.f;

			pInstance[i].m_vTranslation = Vector4(pInstance[i].m_vTranslation) + m_vecDirection[i] * m_vecSpeed[i] * m_tLerpedData.m_fSpeedRatio * _fDeltaSeconds;
		}
		else
		{
			pInstance[i].m_vTranslation = Vector4(pInstance[i].m_vTranslation) + m_vecDirection[i] * m_vecSpeed[i] * m_tLerpedData.m_fSpeedRatio * _fDeltaSeconds;
		}

		// 색상
		if (m_tParticleInfo.m_vecKeyFrames.empty() || 2 > m_tParticleInfo.m_vecKeyFrames.size())
		{
			pInstance[i].m_vColor = m_tParticleInfo.m_vColor1;
		}
		else
		{
			pInstance[i].m_vColor = m_tLerpedData.m_vColor1;
		}
		if (EParticleType::ALPHADECREASE == m_tParticleInfo.m_eParticleType)
		{
			pInstance[i].m_vColor.w -= m_vecAlphaDecreaseAcc[i];
			m_vecAlphaDecreaseAcc[i] += _fDeltaSeconds;
		}

		// 중력
		float fGravityAcceleration = 0.5f * m_tParticleInfo.m_fGravityValue * powf(m_vecLifeTimeAcc[i], 2.f);
		if (fabsf(fGravityAcceleration) > m_vecSpeed[i] * 1.5f)
		{
			if (fGravityAcceleration < 0.f)
			{
				fGravityAcceleration = -m_vecSpeed[i] * 1.5f;
			}
			else
			{
				fGravityAcceleration = m_vecSpeed[i] * 1.5f;
			}
		}

		if (m_tParticleInfo.m_bGravity)
		{
			pInstance[i].m_vTranslation.y -= fGravityAcceleration;
		}

		if (EParticleType::CHARGE == m_tParticleInfo.m_eParticleType)
		{
			Vector4 vPivot = GetOwnerPosition() + Vector4(m_tParticleInfo.m_vPivot.x, m_tParticleInfo.m_vPivot.y, m_tParticleInfo.m_vPivot.z, 1.f);
			vPivot.w = 1.f;

			if (0.5f > Vector4::Distance(pInstance[i].m_vTranslation, vPivot))
			{
				pInstance[i].m_vTranslation = vPivot - m_vecDirection[i];
			}
		}

		// 시간
		m_vecLifeTimeAcc[i] += _fDeltaSeconds;
		m_vecSpriteLifeTimeAcc[i] += _fDeltaSeconds;

		// 스프라이트
		if (m_tParticleInfo.m_bSprite)
		{
			TickSprite(tSubResource, i);
		}
		else
		{
			pInstance[i].m_vMinUV = Vector2(0.f, 0.f);
			pInstance[i].m_vMaxUV = Vector2(1.f, 1.f);
		}

		// 파티클 하나의 라이프 타임이 끝난 경우
		if (m_vecLifeTimeAcc[i] > m_vecLifeTime[i])
		{
			if (m_tParticleInfo.m_bLoop)
			{
				ResetInstanceValue(tSubResource, i);
			}
			else
			{
				pInstance[i].m_vColor.w = 0.f;
			}
		}
	}

	m_cpDeviceContext->Unmap(m_spInstancing->GetInstanceBuffer().Get(), 0);
}
void Engine::CParticleComponent::TickSprite(D3D11_MAPPED_SUBRESOURCE& _tSubResource, uint32 _iIndex)
{
	FRightUpLookPositionColor* pInstance = ((FRightUpLookPositionColor*)_tSubResource.pData);

	if (m_vecSpriteLifeTimeAcc[_iIndex] > m_tParticleInfo.m_fSpriteSpeed)
	{
		m_vecSpriteLifeTimeAcc[_iIndex] = 0.f;

		pInstance[_iIndex].m_vMinUV.x += 1.f / (float)m_tParticleInfo.m_iSpriteRow;
		pInstance[_iIndex].m_vMaxUV.x += 1.f / (float)m_tParticleInfo.m_iSpriteRow;

		// 한 행을 모두 진행한 경우
		if (1.f < pInstance[_iIndex].m_vMaxUV.x)
		{
			// 아래로 한 칸 이동
			pInstance[_iIndex].m_vMinUV.x = 0.f;
			pInstance[_iIndex].m_vMinUV.y += 1.f / (float)m_tParticleInfo.m_iSpriteCol;

			pInstance[_iIndex].m_vMaxUV.x = 1.f / (float)m_tParticleInfo.m_iSpriteRow;
			pInstance[_iIndex].m_vMaxUV.y += 1.f / (float)m_tParticleInfo.m_iSpriteCol;
		}

		// 마지막 행 이였던 경우
		if (1.f < pInstance[_iIndex].m_vMaxUV.y)
		{
			pInstance[_iIndex].m_vMinUV = Vector2(0.f, 0.f);
			pInstance[_iIndex].m_vMaxUV = Vector2(1.f / (float)m_tParticleInfo.m_iSpriteRow, 1.f / (float)m_tParticleInfo.m_iSpriteCol);
		}
	}
}
void Engine::CParticleComponent::RecycleFromPool()
{
	// 오브젝트 풀로부터 꺼내왔을 때 자동으로 실행될 함수입니다.
	// -> 이펙트 시작 시에 초기화 시켜야할 값이나 설정들을 여기에 작성하면 됩니다.

	/* GameObject */
	CHECKF(!m_wpOwnerGameObject.expired(), L"m_wpOwnerGameObject is nullptr : CEffectMeshComponent");
	m_wpOwnerGameObject.lock()->SetActive(true);
	m_wpOwnerGameObject.lock()->SetCulled(false);

	ResetOwnerPosition();

	/* Component */
	m_bFinished = false;
	m_fTrackPosition = 0.f;
	m_iCurKeyFrame = 0;

	/* 보간 데이터 초기화 */
	if (m_tParticleInfo.m_vecKeyFrames.empty() || 2 > m_tParticleInfo.m_vecKeyFrames.size())
	{
		m_tLerpedData = FParticleKeyFrame{};
	}
	else
	{
		m_tLerpedData = m_tParticleInfo.m_vecKeyFrames.front();
	}

	ResetInstanceBuffer();
	ResetAllInstanceValue();
}
void Engine::CParticleComponent::NoneActivate()
{
	// m_bActive = false;
	m_wpOwnerGameObject.lock()->SetCulled(true);
}
void Engine::CParticleComponent::EndLifeTime()
{
	CHECKF(!m_wpOwnerGameObject.expired(), L"m_wpOwnerGameObject is nullptr : CEffectMeshComponent");
	m_wpOwnerGameObject.lock()->SetActive(false);
	m_wpOwnerGameObject.lock()->SetCulled(true);

	m_bFinished = true;
	m_fTrackPosition = 0.f;
	m_iCurKeyFrame = 0;

	/* 보간 데이터 초기화 */
	if (m_tParticleInfo.m_vecKeyFrames.empty() || 2 > m_tParticleInfo.m_vecKeyFrames.size())
	{
		m_tLerpedData = FParticleKeyFrame{};
	}
	else
	{
		m_tLerpedData = m_tParticleInfo.m_vecKeyFrames.front();
	}
}
void Engine::CParticleComponent::Loop()
{
	if (!m_wpOwnerGameObject.lock()->GetParentGameObject())
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 없는 경우
		m_vOriginPos = m_tParticleInfo.m_vOffset;
		m_wpOwnerGameObject.lock()->SetPosition(m_vOriginPos);
	}
	else
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 있는 경우
		m_vOriginPos = m_tParticleInfo.m_vOffset;
		m_wpOwnerGameObject.lock()->SetPosition(m_vOriginPos);
	}

	m_fTrackPosition = 0.f;
	m_iCurKeyFrame = 0;

	// ResetInstanceBuffer();
	// ResetAllInstanceValue();

	/* 보간 데이터 초기화 */
	if (m_tParticleInfo.m_vecKeyFrames.empty() || 2 > m_tParticleInfo.m_vecKeyFrames.size())
	{
		m_tLerpedData = FParticleKeyFrame{};
	}
	else
	{
		m_tLerpedData = m_tParticleInfo.m_vecKeyFrames.front();
	}
}

void Engine::CParticleComponent::ResetInstanceBuffer()
{
	// 인스턴스 버퍼(컴객체)의 값들을 초기화합니다.
	D3D11_MAPPED_SUBRESOURCE tSubResource = {};

	ResetOwnerPosition();

	Vector4 vOwnerPosV4 = Vector4(m_vOriginPos.x, m_vOriginPos.y, m_vOriginPos.z, 1.f);

	m_cpDeviceContext->Map(m_spInstancing->GetInstanceBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);

	FRightUpLookPositionColor* pInstances = (FRightUpLookPositionColor*)tSubResource.pData;

	for (int32 innerIndex = 0; innerIndex < m_tParticleInfo.m_iNumInstance; ++innerIndex)
	{
		float fSize = CMathUtility::GetRandomFloat(m_tParticleInfo.m_vSize.x, m_tParticleInfo.m_vSize.y);

		pInstances[innerIndex].m_vRight = Vector4(fSize, 0.f, 0.f, 0.f);
		pInstances[innerIndex].m_vUp = Vector4(0.f, fSize, 0.f, 0.f);
		pInstances[innerIndex].m_vLook = Vector4(0.f, 0.f, fSize, 0.f);

		if (EParticleType::REVOLUTION == m_tParticleInfo.m_eParticleType)
		{
			Vector4 vZ = Vector4(0.f, 0.f, m_tParticleInfo.m_vRadius.x, 0.f);
			Vector4 vOffset = Vector4(m_tParticleInfo.m_vOffset.x, m_tParticleInfo.m_vOffset.y, m_tParticleInfo.m_vOffset.z, 0.f);
			vZ += vOffset;

			Matrix matRotationY = Matrix::CreateRotationY(XMConvertToRadians((360.f / m_tParticleInfo.m_iNumInstance) * (float)innerIndex));

			vZ = Vector4::Transform(vZ, matRotationY);
			vZ.w = 1.f;

			pInstances[innerIndex].m_vTranslation = vOwnerPosV4 + vZ;
			pInstances[innerIndex].m_vTranslation.w = 1.f;
		}
		else if (EParticleType::TONADO == m_tParticleInfo.m_eParticleType)
		{
			pInstances[innerIndex].m_vTranslation = vOwnerPosV4 + Vector4(m_tParticleInfo.m_vOffset.x +  CMathUtility::GetRandomFloat(0.f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
																		  m_tParticleInfo.m_vOffset.y +  CMathUtility::GetRandomFloat(0.f, 1.f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
																		  m_tParticleInfo.m_vOffset.z +  CMathUtility::GetRandomFloat(0.f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y), 1.f);
			pInstances[innerIndex].m_vTranslation.w = 1.f;
		}
		else if (EParticleType::GROUNDED == m_tParticleInfo.m_eParticleType)
		{
			pInstances[innerIndex].m_vTranslation = vOwnerPosV4 + Vector4(m_tParticleInfo.m_vOffset.x + CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
																		  m_tParticleInfo.m_vOffset.y + 0.5f,
																		  m_tParticleInfo.m_vOffset.z +  CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y), 1.f);
			pInstances[innerIndex].m_vTranslation.w = 1.f;
		}
		else
		{
			pInstances[innerIndex].m_vTranslation = vOwnerPosV4 + Vector4(m_tParticleInfo.m_vOffset.x +  CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
																		  m_tParticleInfo.m_vOffset.y +  CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
																		  m_tParticleInfo.m_vOffset.z +  CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y), 1.f);
			pInstances[innerIndex].m_vTranslation.w = 1.f;
		}
		pInstances[innerIndex].m_vPrevTranslation = pInstances[innerIndex].m_vTranslation;
		pInstances[innerIndex].m_vPrevTranslation.w = 1.f;

		if (m_tParticleInfo.m_vecKeyFrames.empty() || 2 > m_tParticleInfo.m_vecKeyFrames.size())
		{
			pInstances[innerIndex].m_vColor = m_tParticleInfo.m_vColor1;
		}
		else
		{
			pInstances[innerIndex].m_vColor = m_tLerpedData.m_vColor1;
		}
		if (EParticleType::ALPHADECREASE == m_tParticleInfo.m_eParticleType)
		{
			pInstances[innerIndex].m_vColor.w = CMathUtility::GetRandomFloat(0.5f, 1.f);
		}

		pInstances[innerIndex].m_vMinUV = Vector2(0.f, 0.f);
		pInstances[innerIndex].m_vMaxUV = Vector2(1.f / (float)m_tParticleInfo.m_iSpriteRow, 1.f / (float)m_tParticleInfo.m_iSpriteCol);
	}

	m_cpDeviceContext->Unmap(m_spInstancing->GetInstanceBuffer().Get(), 0);
}

void Engine::CParticleComponent::ResetInstanceBufferByBonePosition(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel)
{
	CHECKF(!m_wpOwnerGameObject.expired(), L"m_wpOwnerGameObject is nullptr : CEffectMeshComponent");
	m_wpOwnerGameObject.lock()->SetActive(true);
	m_wpOwnerGameObject.lock()->SetCulled(false);

	vector<shared_ptr<CBone>> vecBones = _spModel->GetBones();
	CHECKF(!vecBones.empty(), L"Bone vector is empty : ParticleComponent");

	ResetOwnerPosition();

	/* Component */
	m_bFinished = false;
	m_fTrackPosition = 0.f;
	m_iCurKeyFrame = 0;

	/* 보간 데이터 초기화 */
	if (m_tParticleInfo.m_vecKeyFrames.empty() || 2 > m_tParticleInfo.m_vecKeyFrames.size())
	{
		m_tLerpedData = FParticleKeyFrame{};
	}
	else
	{
		m_tLerpedData = m_tParticleInfo.m_vecKeyFrames.front();
	}

	// 인스턴스 버퍼(컴객체)의 값들을 초기화합니다.
	D3D11_MAPPED_SUBRESOURCE tSubResource = {};

	Vector3 vObjectPos = _spGameObject->GetPosition().value();

	m_cpDeviceContext->Map(m_spInstancing->GetInstanceBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);
	FRightUpLookPositionColor* pInstances = (FRightUpLookPositionColor*)tSubResource.pData;
	{
		int32 iBoneIndex = 0;

		for (int32 innerIndex = 0; innerIndex < m_tParticleInfo.m_iNumInstance; ++innerIndex)
		{
			float fSize = CMathUtility::GetRandomFloat(m_tParticleInfo.m_vSize.x, m_tParticleInfo.m_vSize.y);

			pInstances[innerIndex].m_vRight = Vector4(fSize, 0.f, 0.f, 0.f);
			pInstances[innerIndex].m_vUp = Vector4(0.f, fSize, 0.f, 0.f);
			pInstances[innerIndex].m_vLook = Vector4(0.f, 0.f, fSize, 0.f);

			if (iBoneIndex > vecBones.size() - 1) { iBoneIndex = 0; }
			shared_ptr<CBone> spBone = vecBones[iBoneIndex];

			Matrix matBoneCombined = spBone->GetCombinedTransformationMatrix();
			Matrix matObjectPos = _spGameObject->GetTransformMatrix().value();

			Matrix matFinal = matBoneCombined * matObjectPos;
			Vector3 vFinal = matFinal.Translation();
			Vector4 vFinalV4 = Vector4(vFinal.x, vFinal.y, vFinal.z, 1.f);

			pInstances[innerIndex].m_vTranslation = vFinalV4 + Vector4(CMathUtility::GetRandomFloat(0.f, 0.2f), CMathUtility::GetRandomFloat(0.f, 0.2f), CMathUtility::GetRandomFloat(0.f, 0.2f), 0.f);
			pInstances[innerIndex].m_vTranslation.w = 1.f;
			pInstances[innerIndex].m_vPrevTranslation = pInstances[innerIndex].m_vTranslation;

			++iBoneIndex;

			if (m_tParticleInfo.m_vecKeyFrames.empty() || 2 > m_tParticleInfo.m_vecKeyFrames.size())
			{
				pInstances[innerIndex].m_vColor = m_tParticleInfo.m_vColor1;
			}
			else
			{
				pInstances[innerIndex].m_vColor = m_tLerpedData.m_vColor1;
			}

			pInstances[innerIndex].m_vMinUV = Vector2(0.f, 0.f);
			pInstances[innerIndex].m_vMaxUV = Vector2(1.f / (float)m_tParticleInfo.m_iSpriteRow, 1.f / (float)m_tParticleInfo.m_iSpriteCol);
		}
	}
	m_cpDeviceContext->Unmap(m_spInstancing->GetInstanceBuffer().Get(), 0);

	ResetAllInstanceValue();
}

void Engine::CParticleComponent::ResetAllInstanceValue()
{
	// 모든 인스턴스에 들어있는 값들을 초기화합니다.
	for (int32 iInnerIndex = 0; iInnerIndex < m_tParticleInfo.m_iNumInstance; ++iInnerIndex)
	{
		m_vecSpeed[iInnerIndex] = CMathUtility::GetRandomFloat(m_tParticleInfo.m_vSpeed.x, m_tParticleInfo.m_vSpeed.y);

		if (m_tParticleInfo.m_bRandomLifeTime)
			// m_vecLifeTime[iInnerIndex] = CMathUtility::GetRandomFloat(m_tParticleInfo.m_vLifeTime.x, m_tParticleInfo.m_vLifeTime.y);
			m_vecLifeTime[iInnerIndex] = CMathUtility::GetRandomFloat(0.f, m_tParticleInfo.m_vLifeTime.y - m_tParticleInfo.m_vLifeTime.x);
		else
			m_vecLifeTime[iInnerIndex] = m_tParticleInfo.m_vLifeTime.y - m_tParticleInfo.m_vLifeTime.x;

		m_vecLifeTimeAcc[iInnerIndex] = 0.0f;
		m_vecSpriteLifeTimeAcc[iInnerIndex] = 0.f;

		m_vecRotation[iInnerIndex].x = CMathUtility::GetRandomFloat(0.f, m_tParticleInfo.m_vRotation.x);
		m_vecRotation[iInnerIndex].y = CMathUtility::GetRandomFloat(0.f, m_tParticleInfo.m_vRotation.y);
		m_vecRotation[iInnerIndex].z = CMathUtility::GetRandomFloat(0.f, m_tParticleInfo.m_vRotation.z);

		m_vecAlphaDecreaseAcc[iInnerIndex] = CMathUtility::GetRandomFloat(0.f, 0.5f);
	}

	switch (m_tParticleInfo.m_eParticleType)
	{
	case EParticleType::DROP:
	{
		for (int32 iInnerIndex = 0; iInnerIndex < m_tParticleInfo.m_iNumInstance; ++iInnerIndex)
		{
			m_vecDirection[iInnerIndex] = Vector4(0.f, -1.f, 0.f, 0.f);
		}
		break;
	}
	case EParticleType::SPREAD:
	case EParticleType::ERUPTION:
	{
		Vector3 vPivot = Vector3(0.f);
		Vector3 vOwnerPos = GetOwnerPositionV3();
		Matrix matOwner = GetOwnerMatrix();
		
		vPivot = vOwnerPos +
			matOwner.Right() * m_tParticleInfo.m_vPivot.x +
			matOwner.Up() * m_tParticleInfo.m_vPivot.y +
			matOwner.Forward() * m_tParticleInfo.m_vPivot.z;

		Vector4 vPivotV4 = Vector4(vPivot.x, vPivot.y, vPivot.z, 1.f);

		D3D11_MAPPED_SUBRESOURCE tSubResource = {};
		m_cpDeviceContext->Map(m_spInstancing->GetInstanceBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);

		FRightUpLookPositionColor* pInstances = (FRightUpLookPositionColor*)tSubResource.pData;

		for (int32 iInnerIndex = 0; iInnerIndex < m_tParticleInfo.m_iNumInstance; ++iInnerIndex)
		{
			m_vecDirection[iInnerIndex] = XMVector4Normalize(pInstances[iInnerIndex].m_vTranslation - vPivotV4);
			m_vecDirection[iInnerIndex].w = 0.f;
		}

		m_cpDeviceContext->Unmap(m_spInstancing->GetInstanceBuffer().Get(), 0);

		break;
	}
	case EParticleType::CHARGE:
	{
		Vector4 vPivot = GetOwnerPosition() + m_tParticleInfo.m_vPivot;

		D3D11_MAPPED_SUBRESOURCE tSubResource = {};
		m_cpDeviceContext->Map(m_spInstancing->GetInstanceBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);

		FRightUpLookPositionColor* pInstances = (FRightUpLookPositionColor*)tSubResource.pData;

		for (int32 iInnerIndex = 0; iInnerIndex < m_tParticleInfo.m_iNumInstance; ++iInnerIndex)
		{
			m_vecDirection[iInnerIndex] = XMVector4Normalize(vPivot - pInstances[iInnerIndex].m_vTranslation);
			m_vecDirection[iInnerIndex].w = 0.f;
		}

		m_cpDeviceContext->Unmap(m_spInstancing->GetInstanceBuffer().Get(), 0);

		break;
	}
	case EParticleType::REVOLUTION:
	case EParticleType::TONADO:
	{
		Vector4 vPivot = GetOwnerPosition() + m_tParticleInfo.m_vPivot;

		D3D11_MAPPED_SUBRESOURCE tSubResource = {};
		m_cpDeviceContext->Map(m_spInstancing->GetInstanceBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);

		FRightUpLookPositionColor* pInstances = (FRightUpLookPositionColor*)tSubResource.pData;

		// 공전하는 방향 벡터를 구합니다.
		for (uint32 iInnerIndex = 0; iInnerIndex < (uint32)m_tParticleInfo.m_iNumInstance; ++iInnerIndex)
		{
			Vector4 vDirFromPivot = XMVector4Normalize(pInstances[iInnerIndex].m_vTranslation - vPivot);
			Vector4 vRevolutionDir = vDirFromPivot.Cross(vDirFromPivot, Vector4(0.f, 1.f, 0.f, 0.f));
			vRevolutionDir.Normalize();

			m_vecDirection[iInnerIndex] = vRevolutionDir;
		}

		m_cpDeviceContext->Unmap(m_spInstancing->GetInstanceBuffer().Get(), 0);

		break;
	}
	default:
	{
		Vector4 vPivot = GetOwnerPosition() + m_tParticleInfo.m_vPivot;

		D3D11_MAPPED_SUBRESOURCE tSubResource = {};
		m_cpDeviceContext->Map(m_spInstancing->GetInstanceBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);

		FRightUpLookPositionColor* pInstances = (FRightUpLookPositionColor*)tSubResource.pData;

		for (int32 iInnerIndex = 0; iInnerIndex < m_tParticleInfo.m_iNumInstance; ++iInnerIndex)
		{
			m_vecDirection[iInnerIndex] = XMVector4Normalize(pInstances[iInnerIndex].m_vTranslation - vPivot);
			m_vecDirection[iInnerIndex].w = 0.f;
		}

		m_cpDeviceContext->Unmap(m_spInstancing->GetInstanceBuffer().Get(), 0);

		break;
	}
	}
}
void Engine::CParticleComponent::ResetInstanceValue(D3D11_MAPPED_SUBRESOURCE& _tSubResource, uint32 _iIndex)
{
	Vector4 vOwnerPosV4 = Vector4(m_vOriginPos.x, m_vOriginPos.y, m_vOriginPos.z, 1.f);

	/* Speed */
	m_vecSpeed[_iIndex] = CMathUtility::GetRandomFloat(m_tParticleInfo.m_vSpeed.x, m_tParticleInfo.m_vSpeed.y);

	/* Time Value */
	m_vecLifeTime[_iIndex] = CMathUtility::GetRandomFloat(m_tParticleInfo.m_vLifeTime.x, m_tParticleInfo.m_vLifeTime.y);
	m_vecLifeTimeAcc[_iIndex] = 0.f;
	m_vecSpriteLifeTimeAcc[_iIndex] = 0.f;

	/* Rotation */
	m_vecRotation[_iIndex].x = CMathUtility::GetRandomFloat(0.f, m_tParticleInfo.m_vRotation.x);
	m_vecRotation[_iIndex].y = CMathUtility::GetRandomFloat(0.f, m_tParticleInfo.m_vRotation.y);
	m_vecRotation[_iIndex].z = CMathUtility::GetRandomFloat(0.f, m_tParticleInfo.m_vRotation.z);

	m_vecAlphaDecreaseAcc[_iIndex] = CMathUtility::GetRandomFloat(0.f, 0.5f);

	/* Translation */
	if (EParticleType::REVOLUTION == m_tParticleInfo.m_eParticleType)
	{
		Vector4 vZ = Vector4(0.f, 0.f, m_tParticleInfo.m_vRadius.x, 0.f);
		Vector4 vOffset = Vector4(m_tParticleInfo.m_vOffset.x, m_tParticleInfo.m_vOffset.y, m_tParticleInfo.m_vOffset.z, 0.f);
		vZ += vOffset;

		Matrix matRotationY = Matrix::CreateRotationY(XMConvertToRadians((360.f / m_tParticleInfo.m_iNumInstance) * (float)_iIndex));

		vZ = Vector4::Transform(vZ, matRotationY);
		vZ.w = 1.f;

		((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vTranslation = vOwnerPosV4 + vZ;
		((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vTranslation.w = 1.f;
	}
	else if (EParticleType::TONADO == m_tParticleInfo.m_eParticleType)
	{
		((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vTranslation = vOwnerPosV4 + Vector4(m_tParticleInfo.m_vOffset.x + CMathUtility::GetRandomFloat(0.f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
																										  m_tParticleInfo.m_vOffset.y + CMathUtility::GetRandomFloat(0.f, 1.f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
																										  m_tParticleInfo.m_vOffset.z + CMathUtility::GetRandomFloat(0.f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y), 1.f);
	}
	else if (EParticleType::GROUNDED == m_tParticleInfo.m_eParticleType)
	{
		((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vTranslation = vOwnerPosV4 + Vector4(m_tParticleInfo.m_vOffset.x + CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
																										  m_tParticleInfo.m_vOffset.y + 0.5f,
																										  m_tParticleInfo.m_vOffset.z + CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y), 1.f);
	}
	else
	{
		((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vTranslation = vOwnerPosV4 + Vector4(m_tParticleInfo.m_vOffset.x + CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
																										  m_tParticleInfo.m_vOffset.y + CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
																										  m_tParticleInfo.m_vOffset.z + CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y), 1.f);
		((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vTranslation.w = 1.f;
	}

	if (m_tParticleInfo.m_vecKeyFrames.empty() || 2 > m_tParticleInfo.m_vecKeyFrames.size())
	{
		((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vColor = m_tParticleInfo.m_vColor1;
	}
	else
	{
		((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vColor = m_tLerpedData.m_vColor1;
	}
	if (EParticleType::ALPHADECREASE == m_tParticleInfo.m_eParticleType)
	{
		((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vColor.w = CMathUtility::GetRandomFloat(0.5f, 1.f);
	}

	/* Sprite */
	((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vMinUV = Vector2(0.f, 0.f);
	((FRightUpLookPositionColor*)_tSubResource.pData)[_iIndex].m_vMaxUV = Vector2(1.f / (float)m_tParticleInfo.m_iSpriteRow, 1.f / (float)m_tParticleInfo.m_iSpriteCol);
}

void Engine::CParticleComponent::ResetOwnerPosition()
{
	if (!m_wpOwnerGameObject.lock()->GetParentGameObject())
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 없는 경우
		m_vOriginPos = m_wpOwnerGameObject.lock()->GetPosition().value() + m_tParticleInfo.m_vOffset;
		m_wpOwnerGameObject.lock()->SetPosition(m_vOriginPos);
	}
	else
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 있는 경우
		if (m_wpOwnerGameObject.lock()->GetParentGameObject()->GetPosition().has_value())
		{
			m_vOriginPos = m_wpOwnerGameObject.lock()->GetParentGameObject()->GetPosition().value() + m_tParticleInfo.m_vOffset;
			m_wpOwnerGameObject.lock()->SetPosition(m_vOriginPos);
		}
	}
}

Vector3 Engine::CParticleComponent::GetOwnerPositionV3()
{
	Vector3 vOwnerPos;
	if (!m_wpOwnerGameObject.lock()->GetParentGameObject())
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 없는 경우
		vOwnerPos = m_wpOwnerGameObject.lock()->GetPosition().value() + m_tParticleInfo.m_vOffset;
	}
	else
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 있는 경우
		vOwnerPos = m_wpOwnerGameObject.lock()->GetParentGameObject()->GetPosition().value() + m_tParticleInfo.m_vOffset;
	}

	return vOwnerPos;
}

Vector4 Engine::CParticleComponent::GetOwnerPosition()
{
	Vector3 vOwnerPos;
	if (!m_wpOwnerGameObject.lock()->GetParentGameObject())
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 없는 경우
		vOwnerPos = m_wpOwnerGameObject.lock()->GetPosition().value() + m_tParticleInfo.m_vOffset;
	}
	else
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 있는 경우
		vOwnerPos = m_wpOwnerGameObject.lock()->GetParentGameObject()->GetPosition().value() + m_tParticleInfo.m_vOffset;
	}

	Vector4 vOwnerPosV4 = Vector4(vOwnerPos.x, vOwnerPos.y, vOwnerPos.z, 1.f);

	return vOwnerPosV4;
}

Matrix Engine::CParticleComponent::GetOwnerMatrix()
{
	Matrix matOwner;
	if (!m_wpOwnerGameObject.lock()->GetParentGameObject())
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 없는 경우
		matOwner = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
	}
	else
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 있는 경우
		matOwner = m_wpOwnerGameObject.lock()->GetParentGameObject()->GetTransformMatrix().value();
	}

	return matOwner;
}

void Engine::CParticleComponent::AddKeyFrame(FParticleKeyFrame _tParticleKeyFrame)
{
	m_tParticleInfo.m_vecKeyFrames.push_back(_tParticleKeyFrame);
	++m_tParticleInfo.m_iNumKeyFrames;
}
void Engine::CParticleComponent::SetKeyFrameValue(const FParticleKeyFrame& _tParticleKeyFrame, int32 _iIndex)
{
	if (m_tParticleInfo.m_vecKeyFrames.empty() || _iIndex > (int32)m_tParticleInfo.m_vecKeyFrames.size() - 1 || _iIndex < 0) { return; }

	m_tParticleInfo.m_vecKeyFrames[_iIndex] = _tParticleKeyFrame;
}
const std::optional<FParticleKeyFrame> Engine::CParticleComponent::GetKeyFrameValue(int32 _iIndex)
{
	if (m_tParticleInfo.m_vecKeyFrames.empty() || _iIndex > (int32)m_tParticleInfo.m_vecKeyFrames.size() - 1 || _iIndex < 0) { return nullopt; }

	return m_tParticleInfo.m_vecKeyFrames[_iIndex];
}

void Engine::CParticleComponent::SetEffectShader(const std::wstring& _wstrEffectShaderName)
{
	const shared_ptr<CEffectShader> spEffectShader = CShaderHandler::GetInstance()->FindEffectShader(_wstrEffectShaderName);
	if (!spEffectShader) { return; }

	m_wpEffectShader = spEffectShader;

	m_cpVariable_MeshInfo = nullptr;
	m_cpVariable_Textures = nullptr;

	m_cpVariable_MeshInfo = m_wpEffectShader.lock()->GetRawVariableByName("g_tEffectInfo_Particle");
	m_cpVariable_Textures = m_wpEffectShader.lock()->GetShaderResourceVariableByName("g_Textures");
}
void Engine::CParticleComponent::SetTexture(const std::wstring& _wstrTextureName, ETextureType _eTextureType)
{
	const ComPtr<ID3D11ShaderResourceView> cpTexture = CTextureHandler::GetInstance()->FindShaderResourceView(_wstrTextureName, _eTextureType);
	if (!cpTexture) { return; }

	switch (_eTextureType)
	{
	case ETextureType::DIFFUSE: m_vecSRVs[0] = cpTexture.Get(); break;
	case ETextureType::MASK:	m_vecSRVs[1] = cpTexture.Get(); break;
	default: break;
	}
	return;
}

HRESULT Engine::CParticleComponent::LoadFromJsonFile(const wstring& _wstrJsonFileName)
{
#pragma region RapidJSON

	// 1. JSON문서를 읽기 모드로 연다.
	ifstream isJsonpath(L"../../Resource/Effects/Particle/" + _wstrJsonFileName + L".json");
	if (!isJsonpath.is_open()) { throw runtime_error("failed to open json file : CParticleComponent"); }

	// 2. 문서 내용을 문자열로 변환.
	string strJsonfile((istreambuf_iterator<char>(isJsonpath)), (istreambuf_iterator<char>()));

	// 3. JSON문서의 문자열을 파싱해온다.
	rapidjson::Document doc;
	doc.Parse(strJsonfile.c_str());

	// 4. JSON문서로부터 데이터를 읽는다.
	/* Type */
	m_tParticleInfo.m_eRenderType			= (ERenderType)doc["RenderType"].GetUint();
	m_eRenderType							= m_tParticleInfo.m_eRenderType;
	m_tParticleInfo.m_eEffectType			= (EEffectType)doc["EffectType"].GetUint();
	m_tParticleInfo.m_eParticleType			= (EParticleType)doc["ParticleType"].GetUint();
	m_tParticleInfo.m_wstrEffectShaderName	= CStringUtility::ToWideString(doc["Effect Shader Name"].GetString());

	/* Option */
	m_tParticleInfo.m_bDiffuse					= doc["Use Diffuse Texture"].GetBool();
	m_tParticleInfo.m_wstrDiffuseTextureName	= CStringUtility::ToWideString(doc["Diffuse Texture Name"].GetString());
	m_tParticleInfo.m_bMask						= doc["Use Mask Texture"].GetBool();
	m_tParticleInfo.m_wstrMaskTextureName		= CStringUtility::ToWideString(doc["Mask Texture Name"].GetString());

	m_tParticleInfo.m_bSprite					= doc["Is Sprite"].GetBool();
	m_tParticleInfo.m_iSpriteRow				= doc["Sprite Row"].GetInt();
	m_tParticleInfo.m_iSpriteCol				= doc["Sprite Col"].GetInt();
	m_tParticleInfo.m_fSpriteSpeed				= doc["Sprite Speed"].GetFloat();

	m_tParticleInfo.m_bLoop						= doc["Is Loop"].GetBool();
	m_tParticleInfo.m_bBillboard				= doc["Is Billboard"].GetBool();
	m_tParticleInfo.m_bDirectional				= doc["Is Directional"].GetBool();
	m_tParticleInfo.m_bGravity					= doc["Is Gravity"].GetBool();
	m_tParticleInfo.m_bGlow						= doc["Is Glow"].GetBool();
	m_tParticleInfo.m_bBloom					= doc["Is Bloom"].GetBool();
	m_tParticleInfo.m_bAlphaAttenuation			= doc["Is AlphaAttenuation"].GetBool();
	m_tParticleInfo.m_fGravityValue				= doc["Gravity Value"].GetFloat();
	m_tParticleInfo.m_fGlowWeight				= doc["Glow Weight"].GetFloat();
	m_tParticleInfo.m_fBloomWeight				= doc["Bloom Weight"].GetFloat();
	m_tParticleInfo.m_fAlphaAttenuationWeight	= doc["AlphaAttenuation Weight"].GetFloat();

	/* Base Value */
	m_tParticleInfo.m_bRandomLifeTime = doc["Random Life Time"].GetBool();

	m_tParticleInfo.m_vLifeTime.x	= doc["Life Time"][0].GetFloat();
	m_tParticleInfo.m_vLifeTime.y	= doc["Life Time"][1].GetFloat();

	m_tParticleInfo.m_iNumInstance	= doc["Num Instance"].GetInt();

	m_tParticleInfo.m_vSize.x		= doc["Size"][0].GetFloat();
	m_tParticleInfo.m_vSize.y		= doc["Size"][1].GetFloat();

	m_tParticleInfo.m_vColor1.x		= doc["Color1"][0].GetFloat();
	m_tParticleInfo.m_vColor1.y		= doc["Color1"][1].GetFloat();
	m_tParticleInfo.m_vColor1.z		= doc["Color1"][2].GetFloat();
	m_tParticleInfo.m_vColor1.w		= doc["Color1"][3].GetFloat();

	m_tParticleInfo.m_vColor2.x		= doc["Color2"][0].GetFloat();
	m_tParticleInfo.m_vColor2.y		= doc["Color2"][1].GetFloat();
	m_tParticleInfo.m_vColor2.z		= doc["Color2"][2].GetFloat();
	m_tParticleInfo.m_vColor2.w		= doc["Color2"][3].GetFloat();

	m_tParticleInfo.m_vColor3.x		= doc["Color3"][0].GetFloat();
	m_tParticleInfo.m_vColor3.y		= doc["Color3"][1].GetFloat();
	m_tParticleInfo.m_vColor3.z		= doc["Color3"][2].GetFloat();
	m_tParticleInfo.m_vColor3.w		= doc["Color3"][3].GetFloat();

	m_tParticleInfo.m_vSpeed.x		= doc["Speed"][0].GetFloat();
	m_tParticleInfo.m_vSpeed.y		= doc["Speed"][1].GetFloat();

	m_tParticleInfo.m_vOffset.x		= doc["Offset"][0].GetFloat();
	m_tParticleInfo.m_vOffset.y		= doc["Offset"][1].GetFloat();
	m_tParticleInfo.m_vOffset.z		= doc["Offset"][2].GetFloat();

	m_tParticleInfo.m_vPivot.x		= doc["Pivot"][0].GetFloat();
	m_tParticleInfo.m_vPivot.y		= doc["Pivot"][1].GetFloat();
	m_tParticleInfo.m_vPivot.z		= doc["Pivot"][2].GetFloat();

	m_tParticleInfo.m_vRange.x		= doc["Range"][0].GetFloat();
	m_tParticleInfo.m_vRange.y		= doc["Range"][1].GetFloat();
	m_tParticleInfo.m_vRange.z		= doc["Range"][2].GetFloat();

	m_tParticleInfo.m_vRotation.x	= doc["Rotation Per Second"][0].GetFloat();
	m_tParticleInfo.m_vRotation.y	= doc["Rotation Per Second"][1].GetFloat();
	m_tParticleInfo.m_vRotation.z	= doc["Rotation Per Second"][2].GetFloat();

	m_tParticleInfo.m_vRadius.x		= doc["Radius"][0].GetFloat();
	m_tParticleInfo.m_vRadius.y		= doc["Radius"][1].GetFloat();

	/* KeyFrame */
	m_tParticleInfo.m_iNumKeyFrames = doc["Num KeyFrames"].GetUint();
	m_tParticleInfo.m_vecKeyFrames.clear();
	m_tParticleInfo.m_vecKeyFrames.reserve(m_tParticleInfo.m_iNumKeyFrames);
	m_tParticleInfo.m_vecKeyFrames.resize(m_tParticleInfo.m_iNumKeyFrames);

	for (uint32 innerIndex = 0; innerIndex < m_tParticleInfo.m_iNumKeyFrames; ++innerIndex)
	{
		rapidjson::Value& tParticleKeyFrame = doc["KeyFrames"][innerIndex];

		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_iPassIndex = tParticleKeyFrame["Effect Shader Pass Index"].GetInt();

		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fTime				= tParticleKeyFrame["Time"].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_bRenderCulled		= tParticleKeyFrame["Render Culled"].GetBool();

		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fSizeRatio			= tParticleKeyFrame["Size Ratio"].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_eSizeEasingType	= (EEasingType)tParticleKeyFrame["Size Easing Type"].GetUint();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fSizeEasingExpo	= tParticleKeyFrame["Size Easing Expo"].GetFloat();

		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fSpeedRatio		= tParticleKeyFrame["Speed Ratio"].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_eSpeedEasingType	= (EEasingType)tParticleKeyFrame["Speed Easing Type"].GetUint();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fSpeedEasingExpo	= tParticleKeyFrame["Speed Easing Expo"].GetFloat();

		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor1.x			= tParticleKeyFrame["Color1"][0].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor1.y			= tParticleKeyFrame["Color1"][1].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor1.z			= tParticleKeyFrame["Color1"][2].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor1.w			= tParticleKeyFrame["Color1"][3].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor1Weight		= tParticleKeyFrame["Color1 Weight"].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_eColor1EasingType	= (EEasingType)tParticleKeyFrame["Color1 Easing Type"].GetUint();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor1EasingExpo	= tParticleKeyFrame["Color1 Easing Expo"].GetFloat();

		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor2.x			= tParticleKeyFrame["Color2"][0].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor2.y			= tParticleKeyFrame["Color2"][1].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor2.z			= tParticleKeyFrame["Color2"][2].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor2.w			= tParticleKeyFrame["Color2"][3].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor2Weight		= tParticleKeyFrame["Color2 Weight"].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_eColor2EasingType	= (EEasingType)tParticleKeyFrame["Color2 Easing Type"].GetUint();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor2EasingExpo	= tParticleKeyFrame["Color2 Easing Expo"].GetFloat();

		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor3.x			= tParticleKeyFrame["Color3"][0].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor3.y			= tParticleKeyFrame["Color3"][1].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor3.z			= tParticleKeyFrame["Color3"][2].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor3.w			= tParticleKeyFrame["Color3"][3].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor3Weight		= tParticleKeyFrame["Color3 Weight"].GetFloat();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_eColor3EasingType	= (EEasingType)tParticleKeyFrame["Color3 Easing Type"].GetUint();
		m_tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor3EasingExpo	= tParticleKeyFrame["Color3 Easing Expo"].GetFloat();
	}

#pragma endregion

	return S_OK;
}
HRESULT Engine::CParticleComponent::WriteToJsonFile(const wstring& _wstrJsonFileName, rapidjson::StringBuffer& _strBuffer, rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer)
{
	_writer.StartObject();

	string strName = "Particle - " + CStringUtility::ToString(GetOwnerGameObject()->GetGameObjectName());
	_writer.Key("Effect Name");
	_writer.String(strName.c_str());

	_writer.Key("RenderType");
	_writer.Int((int32)m_tParticleInfo.m_eRenderType);
	_writer.Key("EffectType");
	_writer.Int((int32)m_tParticleInfo.m_eEffectType);
	_writer.Key("ParticleType");
	_writer.Int((int32)m_tParticleInfo.m_eParticleType);
	_writer.Key("Effect Shader Name");
	_writer.String(CStringUtility::ToString(m_tParticleInfo.m_wstrEffectShaderName).c_str());

	_writer.Key("Use Diffuse Texture");
	_writer.Bool(m_tParticleInfo.m_bDiffuse);
	_writer.Key("Diffuse Texture Name");
	_writer.String(CStringUtility::ToString(m_tParticleInfo.m_wstrDiffuseTextureName).c_str());
	_writer.Key("Use Mask Texture");
	_writer.Bool(m_tParticleInfo.m_bMask);
	_writer.Key("Mask Texture Name");
	_writer.String(CStringUtility::ToString(m_tParticleInfo.m_wstrMaskTextureName).c_str());

	_writer.Key("Is Sprite");
	_writer.Bool(m_tParticleInfo.m_bSprite);
	_writer.Key("Sprite Row");
	_writer.Int(m_tParticleInfo.m_iSpriteRow);
	_writer.Key("Sprite Col");
	_writer.Int(m_tParticleInfo.m_iSpriteCol);
	_writer.Key("Sprite Speed");
	_writer.Double(m_tParticleInfo.m_fSpriteSpeed);

	_writer.Key("Is Loop");
	_writer.Bool(m_tParticleInfo.m_bLoop);
	_writer.Key("Is Billboard");
	_writer.Bool(m_tParticleInfo.m_bBillboard);
	_writer.Key("Is Directional");
	_writer.Bool(m_tParticleInfo.m_bDirectional);
	_writer.Key("Is Gravity");
	_writer.Bool(m_tParticleInfo.m_bGravity);
	_writer.Key("Is Glow");
	_writer.Bool(m_tParticleInfo.m_bGlow);
	_writer.Key("Is Bloom");
	_writer.Bool(m_tParticleInfo.m_bBloom);
	_writer.Key("Is AlphaAttenuation");
	_writer.Bool(m_tParticleInfo.m_bAlphaAttenuation);

	_writer.Key("Gravity Value");
	_writer.Double(m_tParticleInfo.m_fGravityValue);
	_writer.Key("Glow Weight");
	_writer.Double(m_tParticleInfo.m_fGlowWeight);
	_writer.Key("Bloom Weight");
	_writer.Double(m_tParticleInfo.m_fBloomWeight);
	_writer.Key("AlphaAttenuation Weight");
	_writer.Double(m_tParticleInfo.m_fAlphaAttenuationWeight);

	_writer.Key("Random Life Time");
	_writer.Bool(m_tParticleInfo.m_bRandomLifeTime);

	_writer.Key("Life Time");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vLifeTime.x);
	_writer.Double(m_tParticleInfo.m_vLifeTime.y);
	_writer.EndArray();

	_writer.Key("Num Instance");
	_writer.Int(m_tParticleInfo.m_iNumInstance);

	_writer.Key("Size");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vSize.x);
	_writer.Double(m_tParticleInfo.m_vSize.y);
	_writer.EndArray();

	_writer.Key("Color1");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vColor1.x);
	_writer.Double(m_tParticleInfo.m_vColor1.y);
	_writer.Double(m_tParticleInfo.m_vColor1.z);
	_writer.Double(m_tParticleInfo.m_vColor1.w);
	_writer.EndArray();

	_writer.Key("Color2");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vColor2.x);
	_writer.Double(m_tParticleInfo.m_vColor2.y);
	_writer.Double(m_tParticleInfo.m_vColor2.z);
	_writer.Double(m_tParticleInfo.m_vColor2.w);
	_writer.EndArray();

	_writer.Key("Color3");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vColor3.x);
	_writer.Double(m_tParticleInfo.m_vColor3.y);
	_writer.Double(m_tParticleInfo.m_vColor3.z);
	_writer.Double(m_tParticleInfo.m_vColor3.w);
	_writer.EndArray();

	_writer.Key("Speed");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vSpeed.x);
	_writer.Double(m_tParticleInfo.m_vSpeed.y);
	_writer.EndArray();

	_writer.Key("Offset");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vOffset.x);
	_writer.Double(m_tParticleInfo.m_vOffset.y);
	_writer.Double(m_tParticleInfo.m_vOffset.z);
	_writer.EndArray();

	//_writer.Key("Center");
	//_writer.StartArray();
	//_writer.Double(m_tParticleInfo.m_vCenter.x);
	//_writer.Double(m_tParticleInfo.m_vCenter.y);
	//_writer.Double(m_tParticleInfo.m_vCenter.z);
	//_writer.EndArray();

	_writer.Key("Pivot");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vPivot.x);
	_writer.Double(m_tParticleInfo.m_vPivot.y);
	_writer.Double(m_tParticleInfo.m_vPivot.z);
	_writer.EndArray();

	_writer.Key("Range");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vRange.x);
	_writer.Double(m_tParticleInfo.m_vRange.y);
	_writer.Double(m_tParticleInfo.m_vRange.z);
	_writer.EndArray();

	_writer.Key("Rotation Per Second");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vRotation.x);
	_writer.Double(m_tParticleInfo.m_vRotation.y);
	_writer.Double(m_tParticleInfo.m_vRotation.z);
	_writer.EndArray();

	_writer.Key("Radius");
	_writer.StartArray();
	_writer.Double(m_tParticleInfo.m_vRadius.x);
	_writer.Double(m_tParticleInfo.m_vRadius.y);
	_writer.EndArray();

	_writer.Key("Num KeyFrames");
	_writer.Int(m_tParticleInfo.m_iNumKeyFrames);

	_writer.Key("KeyFrames");
	_writer.StartArray();

	const vector<FParticleKeyFrame> vecParticleKeyFrames = GetKeyFrames();

	for (size_t i = 0; i < vecParticleKeyFrames.size(); ++i)
	{
		_writer.StartObject();

		string strKeyFrameIndex = "KeyFrame" + to_string(i);
		_writer.Key("Index");
		_writer.String(strKeyFrameIndex.data());

		_writer.Key("Effect Shader Pass Index");
		_writer.Int(vecParticleKeyFrames[i].m_iPassIndex);

		_writer.Key("Time");
		_writer.Double(vecParticleKeyFrames[i].m_fTime);

		_writer.Key("Render Culled");
		_writer.Bool(vecParticleKeyFrames[i].m_bRenderCulled);

		_writer.Key("Size Ratio");
		_writer.Double(vecParticleKeyFrames[i].m_fSizeRatio);
		_writer.Key("Size Easing Type");
		_writer.Uint((uint32)vecParticleKeyFrames[i].m_eSizeEasingType);
		_writer.Key("Size Easing Expo");
		_writer.Double(vecParticleKeyFrames[i].m_fSizeEasingExpo);

		_writer.Key("Speed Ratio");
		_writer.Double(vecParticleKeyFrames[i].m_fSpeedRatio);
		_writer.Key("Speed Easing Type");
		_writer.Uint((uint32)vecParticleKeyFrames[i].m_eSpeedEasingType);
		_writer.Key("Speed Easing Expo");
		_writer.Double(vecParticleKeyFrames[i].m_fSpeedEasingExpo);

		_writer.Key("Color1");
		_writer.StartArray();
		_writer.Double(vecParticleKeyFrames[i].m_vColor1.x);
		_writer.Double(vecParticleKeyFrames[i].m_vColor1.y);
		_writer.Double(vecParticleKeyFrames[i].m_vColor1.z);
		_writer.Double(vecParticleKeyFrames[i].m_vColor1.w);
		_writer.EndArray();
		_writer.Key("Color1 Weight");
		_writer.Double(vecParticleKeyFrames[i].m_fColor1Weight);
		_writer.Key("Color1 Easing Type");
		_writer.Uint((uint32)vecParticleKeyFrames[i].m_eColor1EasingType);
		_writer.Key("Color1 Easing Expo");
		_writer.Double(vecParticleKeyFrames[i].m_fColor1EasingExpo);

		_writer.Key("Color2");
		_writer.StartArray();
		_writer.Double(vecParticleKeyFrames[i].m_vColor2.x);
		_writer.Double(vecParticleKeyFrames[i].m_vColor2.y);
		_writer.Double(vecParticleKeyFrames[i].m_vColor2.z);
		_writer.Double(vecParticleKeyFrames[i].m_vColor2.w);
		_writer.EndArray();
		_writer.Key("Color2 Weight");
		_writer.Double(vecParticleKeyFrames[i].m_fColor2Weight);
		_writer.Key("Color2 Easing Type");
		_writer.Uint((uint32)vecParticleKeyFrames[i].m_eColor2EasingType);
		_writer.Key("Color2 Easing Expo");
		_writer.Double(vecParticleKeyFrames[i].m_fColor2EasingExpo);

		_writer.Key("Color3");
		_writer.StartArray();
		_writer.Double(vecParticleKeyFrames[i].m_vColor3.x);
		_writer.Double(vecParticleKeyFrames[i].m_vColor3.y);
		_writer.Double(vecParticleKeyFrames[i].m_vColor3.z);
		_writer.Double(vecParticleKeyFrames[i].m_vColor3.w);
		_writer.EndArray();
		_writer.Key("Color3 Weight");
		_writer.Double(vecParticleKeyFrames[i].m_fColor3Weight);
		_writer.Key("Color3 Easing Type");
		_writer.Uint((uint32)vecParticleKeyFrames[i].m_eColor3EasingType);
		_writer.Key("Color3 Easing Expo");
		_writer.Double(vecParticleKeyFrames[i].m_fColor3EasingExpo);

		_writer.EndObject();
	}
	_writer.EndArray();

	_writer.EndObject();

	return S_OK;
}