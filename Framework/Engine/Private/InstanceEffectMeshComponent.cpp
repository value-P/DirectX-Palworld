// Default

#include "pch.h"
#include "InstanceEffectMeshComponent.h"
#include "StringUtility.h"
#include "MathUtility.h"

// Module, Handler, Manager
#include "EngineModule.h"
#include "ShaderHandler.h"
#include "TextureHandler.h"
#include "DeviceManager.h"

// GameObject
#include "GameObject.h"

// etc
#include "EffectShader.h"
#include "InstanceEffectModel.h"
#include "MeshBuffer.h"

std::shared_ptr<CInstanceEffectMeshComponent> Engine::CInstanceEffectMeshComponent::Create(const std::shared_ptr<FInstanceEffectMeshComponentDesc>& _spInstanceEffectMeshComponentDesc, FEffectMeshInfo _tEffectMeshInfo, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo)
{
	const std::shared_ptr<CInstanceEffectMeshComponent> spInstanceEffectMeshComponent = make_shared<CInstanceEffectMeshComponent>();

	spInstanceEffectMeshComponent->SetEffectMeshInfo(_tEffectMeshInfo);
	spInstanceEffectMeshComponent->SetInstanceEffectMeshInfo(_tInstanceEffectMeshInfo);

	ENSUREF(spInstanceEffectMeshComponent->InitializeComponent(_spInstanceEffectMeshComponentDesc), L"Failed to Initialize CInstanceEffectMeshComponent");
	return spInstanceEffectMeshComponent;
}

std::shared_ptr<CInstanceEffectMeshComponent> Engine::CInstanceEffectMeshComponent::CreateFromJson(const std::shared_ptr<FInstanceEffectMeshComponentDesc>& _spInstanceEffectMeshComponentDesc, const std::wstring& _wstrJsonFileName)
{
	const std::shared_ptr<CInstanceEffectMeshComponent> spInstanceEffectMeshComponent = make_shared<CInstanceEffectMeshComponent>();

	spInstanceEffectMeshComponent->LoadFromJsonFile(_wstrJsonFileName);

	ENSUREF(spInstanceEffectMeshComponent->InitializeComponent(_spInstanceEffectMeshComponentDesc), L"Failed to Initialize CInstanceEffectMeshComponent");
	return spInstanceEffectMeshComponent;
}

HRESULT Engine::CInstanceEffectMeshComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// effectmesh component
	// if (FAILED(CEffectMeshComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }
	if (FAILED(CEffectComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	shared_ptr<FInstanceEffectMeshComponentDesc> spInstanceMeshComponentDesc = static_pointer_cast<FInstanceEffectMeshComponentDesc>(_spComponentDesc);

	// render type reinitialize
	m_eRenderType = m_tEffectMeshInfo.m_eRenderType;
	m_eEffectType = EEffectType::InstanceEffectMesh;
	m_tEffectMeshInfo.m_eEffectType = EEffectType::InstanceEffectMesh;

	m_matPivot = spInstanceMeshComponentDesc->m_matPivot;

	// etc
	shared_ptr<CDeviceManager> spDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_cpDevice = spDeviceManager->GetDevice();
	m_cpDeviceContext = spDeviceManager->GetDeviceContext();

	FindEffectShader();
	FindShaderResourceViews();
	InitializeBuffer();
	InitializeInstanceValue();

	m_vOriginPos = m_wpOwnerGameObject.lock()->GetPosition().value();

	return S_OK;
}

HRESULT Engine::CInstanceEffectMeshComponent::BeginPlay()
{
	if (FAILED(CEffectComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CInstanceEffectMeshComponent::PreTickComponent(float _fDeltaSeconds)
{
	m_eRenderType = m_tEffectMeshInfo.m_eRenderType;

	switch (CEffectComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CInstanceEffectMeshComponent::TickComponent(float _fDeltaSeconds)
{
	if (m_bPause) { return CEffectComponent::TickComponent(_fDeltaSeconds); }

	// 위치 갱신
	if (!m_wpOwnerGameObject.lock()->GetParentGameObject())
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 없는 경우
		m_vOriginPos = m_wpOwnerGameObject.lock()->GetPosition().value();
	}
	else
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 있는 경우
		m_vOriginPos = m_wpOwnerGameObject.lock()->GetParentGameObject()->GetPosition().value();
	}

	m_wpOwnerGameObject.lock()->SetPosition(m_vOriginPos + m_tEffectMeshInfo.m_vTranslation + m_tLerpedData.m_vTranslation);

	// 이펙트 진행
	PlayEffect(_fDeltaSeconds);

	return CEffectComponent::TickComponent(_fDeltaSeconds);
}

int32 Engine::CInstanceEffectMeshComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CEffectComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CInstanceEffectMeshComponent::EndPlay()
{
	if (FAILED(CEffectComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CInstanceEffectMeshComponent::Render(int32 _iShaderPath)
{
	if (m_fTrackPosition < m_tEffectMeshInfo.m_vLifeTime.x || m_bCulled) { return; }

	ENSUREF(CEffectMeshComponent::BindVariable(), L"Failed to BindVariable : CInstanceEffectMeshComponent");
	ENSUREF(CInstanceEffectMeshComponent::BindVariable(), L"Failed to BindVariable : CInstanceEffectMeshComponent");
	ENSUREF(m_wpEffectShader.lock()->BeginPass(m_iPassIndex), L"Failed to BeginPass : CInstanceEffectMeshComponent");

	auto& umapMeshInfo = m_spInstanceModel->GetMeshInfos();

	for (auto& Pair : umapMeshInfo)
	{
		for (auto& tMeshInfo : Pair.second)
		{
			tMeshInfo->m_spMeshBuffer->Render();
		}
	}
}

void Engine::CInstanceEffectMeshComponent::Release()
{
	m_cpDevice = nullptr;
	m_cpDeviceContext = nullptr;

	CEffectMeshComponent::Release();
}

HRESULT Engine::CInstanceEffectMeshComponent::BindVariable()
{

	return S_OK;
}

HRESULT Engine::CInstanceEffectMeshComponent::InitializeBuffer()
{
	m_spInstanceModel = CInstanceEffectModel::Create(m_tEffectMeshInfo.m_wstrModelFilePath, m_matPivot, nullptr, m_tInstanceEffectMeshInfo);
	CHECKF(m_spInstanceModel, L"Failed to Create InstanceModel : CInstanceEffectMeshComponent");

	return S_OK;
}
void Engine::CInstanceEffectMeshComponent::InitializeInstanceValue()
{
	/* Vector Reserve & Resize */
	m_vecSpeed.reserve(m_tInstanceEffectMeshInfo.m_iNumInstance);
	m_vecSpeed.resize(m_tInstanceEffectMeshInfo.m_iNumInstance);

	m_vecLifeTime.reserve(m_tInstanceEffectMeshInfo.m_iNumInstance);
	m_vecLifeTime.resize(m_tInstanceEffectMeshInfo.m_iNumInstance);

	m_vecLifeTimeAcc.reserve(m_tInstanceEffectMeshInfo.m_iNumInstance);
	m_vecLifeTimeAcc.resize(m_tInstanceEffectMeshInfo.m_iNumInstance);

	m_vecRotation.reserve(m_tInstanceEffectMeshInfo.m_iNumInstance);
	m_vecRotation.resize(m_tInstanceEffectMeshInfo.m_iNumInstance);

	m_vecDirection.reserve(m_tInstanceEffectMeshInfo.m_iNumInstance);
	m_vecDirection.resize(m_tInstanceEffectMeshInfo.m_iNumInstance);

	ResetAllInstanceValue();
}
void Engine::CInstanceEffectMeshComponent::ResetInstanceBuffer()
{
	// 인스턴스 버퍼(컴객체)의 값들을 초기화합니다.
	D3D11_MAPPED_SUBRESOURCE tSubResource = {};

	m_cpDeviceContext->Map(m_spInstanceModel->GetInstanceBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);

	FRightUpLookPositionPrevPosition* pInstances = (FRightUpLookPositionPrevPosition*)tSubResource.pData;

	for (int32 innerIndex = 0; innerIndex < m_tInstanceEffectMeshInfo.m_iNumInstance; ++innerIndex)
	{
		float fSize = CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vSize.x, m_tInstanceEffectMeshInfo.m_vSize.y);

		pInstances[innerIndex].m_vRight = Vector4(fSize, 0.f, 0.f, 0.f);
		pInstances[innerIndex].m_vUp = Vector4(0.f, fSize, 0.f, 0.f);
		pInstances[innerIndex].m_vLook = Vector4(0.f, 0.f, fSize, 0.f);
		pInstances[innerIndex].m_vTranslation = Vector4(CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRange.x * -0.5f, m_tInstanceEffectMeshInfo.m_vRange.x * 0.5f) * CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRadius.x, m_tInstanceEffectMeshInfo.m_vRadius.y),
			CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRange.y * -0.5f, m_tInstanceEffectMeshInfo.m_vRange.y * 0.5f) * CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRadius.x, m_tInstanceEffectMeshInfo.m_vRadius.y),
			CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRange.z * -0.5f, m_tInstanceEffectMeshInfo.m_vRange.z * 0.5f) * CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRadius.x, m_tInstanceEffectMeshInfo.m_vRadius.y), 1.f);
		pInstances[innerIndex].m_vPrevTranslation = Vector4(CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRange.x * -0.5f, m_tInstanceEffectMeshInfo.m_vRange.x * 0.5f) * CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRadius.x, m_tInstanceEffectMeshInfo.m_vRadius.y),
			CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRange.y * -0.5f, m_tInstanceEffectMeshInfo.m_vRange.y * 0.5f) * CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRadius.x, m_tInstanceEffectMeshInfo.m_vRadius.y),
			CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRange.z * -0.5f, m_tInstanceEffectMeshInfo.m_vRange.z * 0.5f) * CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRadius.x, m_tInstanceEffectMeshInfo.m_vRadius.y), 1.f);
	}

	m_cpDeviceContext->Unmap(m_spInstanceModel->GetInstanceBuffer().Get(), 0);
}
void Engine::CInstanceEffectMeshComponent::ResetAllInstanceValue()
{
	// 모든 인스턴스에 들어있는 값들을 초기화합니다.
	for (int32 iInnerIndex = 0; iInnerIndex < m_tInstanceEffectMeshInfo.m_iNumInstance; ++iInnerIndex)
	{
		m_vecSpeed[iInnerIndex] = CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vSpeed.x, m_tInstanceEffectMeshInfo.m_vSpeed.y);

		// if (m_tEffectMeshInfo.m_bRandomLifeTime)
		m_vecLifeTime[iInnerIndex] = CMathUtility::GetRandomFloat(0.f, m_tEffectMeshInfo.m_vLifeTime.y - m_tEffectMeshInfo.m_vLifeTime.x);
		//else
		//	m_vecLifeTime[iInnerIndex] = m_tParticleInfo.m_vLifeTime.y - m_tParticleInfo.m_vLifeTime.x;

		m_vecLifeTimeAcc[iInnerIndex] = 0.0f;

		m_vecRotation[iInnerIndex].x = CMathUtility::GetRandomFloat(-m_tInstanceEffectMeshInfo.m_vRotation.x, m_tInstanceEffectMeshInfo.m_vRotation.x);
		m_vecRotation[iInnerIndex].y = CMathUtility::GetRandomFloat(-m_tInstanceEffectMeshInfo.m_vRotation.y, m_tInstanceEffectMeshInfo.m_vRotation.y);
		m_vecRotation[iInnerIndex].z = CMathUtility::GetRandomFloat(-m_tInstanceEffectMeshInfo.m_vRotation.z, m_tInstanceEffectMeshInfo.m_vRotation.z);
	}

	switch (m_tInstanceEffectMeshInfo.m_eParticleType)
	{
	case EParticleType::DROP:
	{
		for (int32 iInnerIndex = 0; iInnerIndex < m_tInstanceEffectMeshInfo.m_iNumInstance; ++iInnerIndex)
		{
			m_vecDirection[iInnerIndex] = Vector4(0.f, -1.f, 0.f, 0.f);
		}
		break;
	}
	case EParticleType::SPREAD:
	case EParticleType::ERUPTION:
	{
		D3D11_MAPPED_SUBRESOURCE tSubResource = {};
		m_cpDeviceContext->Map(m_spInstanceModel->GetInstanceBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);
		FRightUpLookPositionPrevPosition* pInstances = (FRightUpLookPositionPrevPosition*)tSubResource.pData;

		for (int32 iInnerIndex = 0; iInnerIndex < m_tInstanceEffectMeshInfo.m_iNumInstance; ++iInnerIndex)
		{
			m_vecDirection[iInnerIndex] = Vector4(CMathUtility::GetRandomFloat(-0.5f, 0.5f),
				CMathUtility::GetRandomFloat(-0.5f, 0.5f),
				CMathUtility::GetRandomFloat(-0.5f, 0.5f), 0.f);
		}

		m_cpDeviceContext->Unmap(m_spInstanceModel->GetInstanceBuffer().Get(), 0);

		break;
	}
	}
}
void Engine::CInstanceEffectMeshComponent::ResetInstanceValue(D3D11_MAPPED_SUBRESOURCE& _tSubResource, uint32 _iIndex)
{
	// 인스턴스의 위치(랜덤)와 시간을 초기화 합니다.

	/* Speed */
	m_vecSpeed[_iIndex] = CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vSpeed.x, m_tInstanceEffectMeshInfo.m_vSpeed.y);

	/* Time Value */
	m_vecLifeTime[_iIndex] = CMathUtility::GetRandomFloat(m_tEffectMeshInfo.m_vLifeTime.x, m_tEffectMeshInfo.m_vLifeTime.y);
	m_vecLifeTimeAcc[_iIndex] = 0.f;

	/* Rotation */
	m_vecRotation[_iIndex].x = CMathUtility::GetRandomFloat(-m_tInstanceEffectMeshInfo.m_vRotation.x, m_tInstanceEffectMeshInfo.m_vRotation.x);
	m_vecRotation[_iIndex].y = CMathUtility::GetRandomFloat(-m_tInstanceEffectMeshInfo.m_vRotation.y, m_tInstanceEffectMeshInfo.m_vRotation.y);
	m_vecRotation[_iIndex].z = CMathUtility::GetRandomFloat(-m_tInstanceEffectMeshInfo.m_vRotation.z, m_tInstanceEffectMeshInfo.m_vRotation.z);

	/* Translation */
	((FRightUpLookPositionPrevPosition*)_tSubResource.pData)[_iIndex].m_vTranslation = Vector4(CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRange.x * -0.5f, m_tInstanceEffectMeshInfo.m_vRange.x * 0.5f) * CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRadius.x, m_tInstanceEffectMeshInfo.m_vRadius.y),
		CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRange.y * -0.5f, m_tInstanceEffectMeshInfo.m_vRange.y * 0.5f) * CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRadius.x, m_tInstanceEffectMeshInfo.m_vRadius.y),
		CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRange.z * -0.5f, m_tInstanceEffectMeshInfo.m_vRange.z * 0.5f) * CMathUtility::GetRandomFloat(m_tInstanceEffectMeshInfo.m_vRadius.x, m_tInstanceEffectMeshInfo.m_vRadius.y), 1.f);
}

void Engine::CInstanceEffectMeshComponent::PlayEffect(float _fDeltaSeconds)
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
	// 3. 트랙포지션이 라이프 타임 사이에 있고, 첫 번째 키프레임 타임보다 큰 경우(활성화)
	else if (m_fTrackPosition >= m_tEffectMeshInfo.m_vLifeTime.x)
	{
		m_wpOwnerGameObject.lock()->SetCulled(false);
		UVMove(_fDeltaSeconds);
		InterpolationBetweenKeyFrame(_fDeltaSeconds);
		if (m_tEffectMeshInfo.m_bSprite)
		{
			TickSprite(_fDeltaSeconds);
		}
		TickIntegrated(_fDeltaSeconds);
	}

	return;
}
void Engine::CInstanceEffectMeshComponent::UVMove(float _fDeltaSeconds)
{
	m_vUVAcc.x += m_tEffectMeshInfo.m_vUVMoveSpeed.x * _fDeltaSeconds;
	m_vUVAcc.y += m_tEffectMeshInfo.m_vUVMoveSpeed.y * _fDeltaSeconds;
}
void Engine::CInstanceEffectMeshComponent::TickIntegrated(float _fDeltaSeconds)
{
	D3D11_MAPPED_SUBRESOURCE tSubResource = {};

	m_cpDeviceContext->Map(m_spInstanceModel->GetInstanceBuffer().Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);

	FRightUpLookPositionPrevPosition* pInstance = ((FRightUpLookPositionPrevPosition*)tSubResource.pData);

	for (int32 i = 0; i < m_tInstanceEffectMeshInfo.m_iNumInstance; ++i)
	{
		pInstance[i].m_vPrevTranslation = pInstance[i].m_vTranslation;

		Vector4 vRight = pInstance[i].m_vRight;
		Vector4 vUp = pInstance[i].m_vUp;
		Vector4 vLook = pInstance[i].m_vLook;

		// 회전 변환
		Matrix matRotation = XMMatrixIdentity();
		matRotation = matRotation * XMMatrixRotationX(XMConvertToRadians(m_vecRotation[i].x * _fDeltaSeconds));
		matRotation = matRotation * XMMatrixRotationY(XMConvertToRadians(m_vecRotation[i].y * _fDeltaSeconds));
		matRotation = matRotation * XMMatrixRotationZ(XMConvertToRadians(m_vecRotation[i].z * _fDeltaSeconds));
		XMStoreFloat4(&pInstance[i].m_vRight, XMVector4Transform(vRight, matRotation));
		XMStoreFloat4(&pInstance[i].m_vUp, XMVector4Transform(vUp, matRotation));
		XMStoreFloat4(&pInstance[i].m_vLook, XMVector4Transform(vLook, matRotation));

		// 이동 변환
		pInstance[i].m_vTranslation = Vector4(pInstance[i].m_vTranslation) + m_vecDirection[i] * m_vecSpeed[i] * _fDeltaSeconds;

		// 중력
		if (m_tInstanceEffectMeshInfo.m_bGravity)
			pInstance[i].m_vTranslation.y -= 0.5f * m_tInstanceEffectMeshInfo.m_fGravityValue * powf(m_vecLifeTimeAcc[i], 2.f);

		// 시간
		m_vecLifeTimeAcc[i] += _fDeltaSeconds;

		// 파티클 하나의 라이프 타임이 끝난 경우
		if (m_vecLifeTimeAcc[i] > m_vecLifeTime[i])
		{
			if (m_tEffectMeshInfo.m_bLoop)
			{
				ResetInstanceValue(tSubResource, i);
			}
			else
			{

			}
		}
	}

	m_cpDeviceContext->Unmap(m_spInstanceModel->GetInstanceBuffer().Get(), 0);
}

void Engine::CInstanceEffectMeshComponent::RecycleFromPool()
{
	// 오브젝트 풀로부터 꺼내왔을 때 자동으로 실행될 함수입니다.
	// -> 이펙트 시작 시에 초기화 시켜야할 값이나 설정들을 여기에 작성하면 됩니다.

/* GameObject */
	ENSUREF(!m_wpOwnerGameObject.expired(), L"m_wpOwnerGameObject is nullptr : CEffectMeshComponent");
	m_wpOwnerGameObject.lock()->SetActive(true);
	m_wpOwnerGameObject.lock()->SetCulled(false);

	/* Component */
	m_bFinished = false;
	m_fTrackPosition = 0.f;
	m_iCurKeyFrame = 0;

	if (!m_wpOwnerGameObject.lock()->GetParentGameObject())
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 없는 경우
		m_vOriginPos = m_wpOwnerGameObject.lock()->GetPosition().value();
	}
	else
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 있는 경우
		m_vOriginPos = m_wpOwnerGameObject.lock()->GetParentGameObject()->GetPosition().value();
	}

	ResetInstanceBuffer();
	ResetAllInstanceValue();

	/* 보간 데이터 초기화 */
	m_tLerpedData = FEffectMeshKeyFrame{};
}
void Engine::CInstanceEffectMeshComponent::NoneActivate()
{
	// m_bActive = false;
	m_wpOwnerGameObject.lock()->SetCulled(true);
}
void Engine::CInstanceEffectMeshComponent::EndLifeTime()
{
	ENSUREF(!m_wpOwnerGameObject.expired(), L"m_wpOwnerGameObject is nullptr : CEffectMeshComponent");
	m_wpOwnerGameObject.lock()->SetActive(false);
	m_wpOwnerGameObject.lock()->SetCulled(true);

	m_bFinished = true;
	m_fTrackPosition = 0.f;
	m_iCurKeyFrame = 0;

	/* 보간 데이터 초기화 */
	m_tLerpedData = FEffectMeshKeyFrame{};
}
void Engine::CInstanceEffectMeshComponent::Loop()
{
	if (!m_wpOwnerGameObject.lock()->GetParentGameObject())
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 없는 경우
		m_vOriginPos = m_wpOwnerGameObject.lock()->GetPosition().value();
	}
	else
	{
		// 이펙트의 부모 오브젝트(이펙트 그룹)가 있는 경우
		m_vOriginPos = m_wpOwnerGameObject.lock()->GetParentGameObject()->GetPosition().value();
	}

	m_fTrackPosition = 0.f;
	m_iCurKeyFrame = 0;

	ResetInstanceBuffer();
	ResetAllInstanceValue();

	/* 보간 데이터 초기화 */
	m_tLerpedData = FEffectMeshKeyFrame{};
}

HRESULT Engine::CInstanceEffectMeshComponent::LoadFromJsonFile(const wstring& _wstrJsonFileName)
{
#pragma region RapidJSON

	// 1. JSON문서를 읽기 모드로 연다.
	ifstream isJsonpath(L"../../Resource/Effects/InstanceEffectMesh/" + _wstrJsonFileName + L".json");
	if (!isJsonpath.is_open()) { throw runtime_error("failed to open json file : CInstanceEffectMeshComponent"); }

	// 2. 문서 내용을 문자열로 변환
	string strJsonfile((istreambuf_iterator<char>(isJsonpath)), (istreambuf_iterator<char>()));

	// 3. JSON문서의 문자열을 파싱해온다.
	rapidjson::Document doc;
	doc.Parse(strJsonfile.c_str());

	// 4. JSON문서로부터 데이터를 읽는다.
	/* Type */
	m_tEffectMeshInfo.m_eRenderType = (ERenderType)doc["RenderType"].GetUint();
	m_eRenderType = m_tEffectMeshInfo.m_eRenderType;
	m_tEffectMeshInfo.m_eEffectType = (EEffectType)doc["EffectType"].GetUint();
	m_tEffectMeshInfo.m_wstrModelFilePath = CStringUtility::ToWideString(doc["Model File Path"].GetString());
	m_tEffectMeshInfo.m_wstrEffectShaderName = CStringUtility::ToWideString(doc["Effect Shader Name"].GetString());

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

	m_tEffectMeshInfo.m_wstrDiffuseTextureName = CStringUtility::ToWideString(doc["Diffuse Texture Name"].GetString());
	m_tEffectMeshInfo.m_wstrNoiseTextureName = CStringUtility::ToWideString(doc["Noise Texture Name"].GetString());
	m_tEffectMeshInfo.m_wstrMaskTextureName = CStringUtility::ToWideString(doc["Mask Texture Name"].GetString());
	m_tEffectMeshInfo.m_wstrDistortionTextureName = CStringUtility::ToWideString(doc["Distortion Texture Name"].GetString());
	m_tEffectMeshInfo.m_wstrDissolveTextureName = CStringUtility::ToWideString(doc["Dissolve Texture Name"].GetString());

	m_tEffectMeshInfo.m_bSprite = doc["Is Sprite"].GetBool();
	m_tEffectMeshInfo.m_iSpriteRow = doc["Sprite Row"].GetInt();
	m_tEffectMeshInfo.m_iSpriteCol = doc["Sprite Col"].GetInt();
	m_tEffectMeshInfo.m_fSpriteSpeed = doc["Sprite Speed"].GetFloat();

	/* Option */
	m_tEffectMeshInfo.m_vScale.x = doc["Scale"][0].GetFloat();
	m_tEffectMeshInfo.m_vScale.y = doc["Scale"][1].GetFloat();
	m_tEffectMeshInfo.m_vScale.z = doc["Scale"][2].GetFloat();
	m_tEffectMeshInfo.m_vRotation.x = doc["Rotation"][0].GetFloat();
	m_tEffectMeshInfo.m_vRotation.y = doc["Rotation"][1].GetFloat();
	m_tEffectMeshInfo.m_vRotation.z = doc["Rotation"][2].GetFloat();
	m_tEffectMeshInfo.m_vTranslation.x = doc["Translation"][0].GetFloat();
	m_tEffectMeshInfo.m_vTranslation.y = doc["Translation"][1].GetFloat();
	m_tEffectMeshInfo.m_vTranslation.z = doc["Translation"][2].GetFloat();

	m_tEffectMeshInfo.m_bAnimation = doc["Is Anim"].GetBool();
	m_tEffectMeshInfo.m_bLoop = doc["Is Loop"].GetBool();
	m_tEffectMeshInfo.m_bUVMoveLoop = doc["Is UVMove Loop"].GetBool();

	m_tEffectMeshInfo.m_bGlow = doc["Is Glow"].GetBool();
	m_tEffectMeshInfo.m_bBloom = doc["Is Bloom"].GetBool();
	m_tEffectMeshInfo.m_bAlphaAttenuation = doc["Is AlphaAttenuation"].GetBool();

	m_tEffectMeshInfo.m_bDiffuse = doc["Use Diffuse Texture"].GetBool();
	m_tEffectMeshInfo.m_bNoise = doc["Use Noise Texture"].GetBool();
	m_tEffectMeshInfo.m_bMask = doc["Use Mask Texture"].GetBool();
	m_tEffectMeshInfo.m_bDistortion = doc["Use Distortion Texture"].GetBool();
	m_tEffectMeshInfo.m_bDissolve = doc["Use Dissolve Texture"].GetBool();

	/* Base Value */
	m_tEffectMeshInfo.m_fDistortionWeight = doc["Distortion Weight"].GetFloat();
	m_tEffectMeshInfo.m_fDissolveWeight = doc["Dissolve Weight"].GetFloat();
	m_tEffectMeshInfo.m_fGlowWeight = doc["Glow Weight"].GetFloat();
	m_tEffectMeshInfo.m_fBloomWeight = doc["Bloom Weight"].GetFloat();
	m_tEffectMeshInfo.m_fAlphaAttenuationWeight = doc["AlphaAttenuation Weight"].GetFloat();
	m_tEffectMeshInfo.m_vLifeTime.x = doc["Life Time"][0].GetFloat();
	m_tEffectMeshInfo.m_vLifeTime.y = doc["Life Time"][1].GetFloat();
	m_tEffectMeshInfo.m_vUVMoveSpeed.x = doc["UVMove Speed"][0].GetFloat();
	m_tEffectMeshInfo.m_vUVMoveSpeed.y = doc["UVMove Speed"][1].GetFloat();

	/* InstanceEffectMesh Info */
	m_tInstanceEffectMeshInfo.m_eParticleType = (EParticleType)doc["Instance Mesh Particle Type"].GetInt();
	m_tInstanceEffectMeshInfo.m_iNumInstance = doc["Num Mesh Instance"].GetInt();

	m_tInstanceEffectMeshInfo.m_bGravity = doc["Is Gravity"].GetBool();
	m_tInstanceEffectMeshInfo.m_fGravityValue = doc["Gravity Value"].GetFloat();

	m_tInstanceEffectMeshInfo.m_vSize.x = doc["Size"][0].GetFloat();
	m_tInstanceEffectMeshInfo.m_vSize.y = doc["Size"][1].GetFloat();

	m_tInstanceEffectMeshInfo.m_vSpeed.x = doc["Speed"][0].GetFloat();
	m_tInstanceEffectMeshInfo.m_vSpeed.y = doc["Speed"][1].GetFloat();

	m_tInstanceEffectMeshInfo.m_vRange.x = doc["Range"][0].GetFloat();
	m_tInstanceEffectMeshInfo.m_vRange.y = doc["Range"][1].GetFloat();
	m_tInstanceEffectMeshInfo.m_vRange.z = doc["Range"][2].GetFloat();

	m_tInstanceEffectMeshInfo.m_vRadius.x = doc["Radius"][0].GetFloat();
	m_tInstanceEffectMeshInfo.m_vRadius.y = doc["Radius"][1].GetFloat();

	m_tInstanceEffectMeshInfo.m_vRotation.x = doc["Rotation Per Seconds"][0].GetFloat();
	m_tInstanceEffectMeshInfo.m_vRotation.y = doc["Rotation Per Seconds"][1].GetFloat();
	m_tInstanceEffectMeshInfo.m_vRotation.z = doc["Rotation Per Seconds"][2].GetFloat();

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

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.x = tEffectMeshKeyFrame["Color1"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.y = tEffectMeshKeyFrame["Color1"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.z = tEffectMeshKeyFrame["Color1"][2].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.w = tEffectMeshKeyFrame["Color1"][3].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor1Weight = tEffectMeshKeyFrame["Color1 Weight"].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eColor1EasingType = (EEasingType)tEffectMeshKeyFrame["Color1 Easing Type"].GetUint();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor1EasingExpo = tEffectMeshKeyFrame["Color1 Easing Expo"].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.x = tEffectMeshKeyFrame["Color2"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.y = tEffectMeshKeyFrame["Color2"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.z = tEffectMeshKeyFrame["Color2"][2].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.w = tEffectMeshKeyFrame["Color2"][3].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor2Weight = tEffectMeshKeyFrame["Color2 Weight"].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eColor2EasingType = (EEasingType)tEffectMeshKeyFrame["Color2 Easing Type"].GetUint();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor2EasingExpo = tEffectMeshKeyFrame["Color2 Easing Expo"].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.x = tEffectMeshKeyFrame["Color3"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.y = tEffectMeshKeyFrame["Color3"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.z = tEffectMeshKeyFrame["Color3"][2].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.w = tEffectMeshKeyFrame["Color3"][3].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor3Weight = tEffectMeshKeyFrame["Color3 Weight"].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eColor3EasingType = (EEasingType)tEffectMeshKeyFrame["Color3 Easing Type"].GetUint();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor3EasingExpo = tEffectMeshKeyFrame["Color3 Easing Expo"].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vScale.x = tEffectMeshKeyFrame["Scale"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vScale.y = tEffectMeshKeyFrame["Scale"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vScale.z = tEffectMeshKeyFrame["Scale"][2].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vRotation.x = tEffectMeshKeyFrame["Rotation"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vRotation.y = tEffectMeshKeyFrame["Rotation"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vRotation.z = tEffectMeshKeyFrame["Rotation"][2].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fRotateWeight = tEffectMeshKeyFrame["Rotate Weight"].GetFloat();

		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vTranslation.x = tEffectMeshKeyFrame["Translation"][0].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vTranslation.y = tEffectMeshKeyFrame["Translation"][1].GetFloat();
		m_tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vTranslation.z = tEffectMeshKeyFrame["Translation"][2].GetFloat();
	}
#pragma endregion

	return S_OK;
}
HRESULT Engine::CInstanceEffectMeshComponent::WriteToJsonFile(const wstring& _wstrJsonFileName, rapidjson::StringBuffer& _strBuffer, rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer)
{
	_writer.StartObject();

	/* Base Info */
	string strName = "Instance Mesh - " + CStringUtility::ToString(GetOwnerGameObject()->GetGameObjectName());
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

	/* InstanceEffectMesh Info */
	_writer.Key("Instance Mesh Particle Type");
	_writer.Int((int32)m_tInstanceEffectMeshInfo.m_eParticleType);
	_writer.Key("Num Mesh Instance");
	_writer.Int(m_tInstanceEffectMeshInfo.m_iNumInstance);

	_writer.Key("Is Gravity");
	_writer.Bool(m_tInstanceEffectMeshInfo.m_bGravity);
	_writer.Key("Gravity Value");
	_writer.Double(m_tInstanceEffectMeshInfo.m_fGravityValue);

	_writer.Key("Size");
	_writer.StartArray();
	_writer.Double(m_tInstanceEffectMeshInfo.m_vSize.x);
	_writer.Double(m_tInstanceEffectMeshInfo.m_vSize.y);
	_writer.EndArray();

	_writer.Key("Speed");
	_writer.StartArray();
	_writer.Double(m_tInstanceEffectMeshInfo.m_vSpeed.x);
	_writer.Double(m_tInstanceEffectMeshInfo.m_vSpeed.y);
	_writer.EndArray();

	_writer.Key("Range");
	_writer.StartArray();
	_writer.Double(m_tInstanceEffectMeshInfo.m_vRange.x);
	_writer.Double(m_tInstanceEffectMeshInfo.m_vRange.y);
	_writer.Double(m_tInstanceEffectMeshInfo.m_vRange.z);
	_writer.EndArray();

	_writer.Key("Radius");
	_writer.StartArray();
	_writer.Double(m_tInstanceEffectMeshInfo.m_vRadius.x);
	_writer.Double(m_tInstanceEffectMeshInfo.m_vRadius.y);
	_writer.EndArray();

	_writer.Key("Rotation Per Seconds");
	_writer.StartArray();
	_writer.Double(m_tInstanceEffectMeshInfo.m_vRotation.x);
	_writer.Double(m_tInstanceEffectMeshInfo.m_vRotation.y);
	_writer.Double(m_tInstanceEffectMeshInfo.m_vRotation.z);
	_writer.EndArray();

	/* InstanceEffectMesh KeyFrame */
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

		_writer.Key("Rotation");
		_writer.StartArray();
		_writer.Double(vecEffectMeshKeyFrames[i].m_vRotation.x);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vRotation.y);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vRotation.z);
		_writer.EndArray();

		_writer.Key("Translation");
		_writer.StartArray();
		_writer.Double(vecEffectMeshKeyFrames[i].m_vTranslation.x);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vTranslation.y);
		_writer.Double(vecEffectMeshKeyFrames[i].m_vTranslation.z);
		_writer.EndArray();

		_writer.Key("Rotate Weight");
		_writer.Double(vecEffectMeshKeyFrames[i].m_fRotateWeight);

		_writer.EndObject();
	}
	_writer.EndArray();

	_writer.EndObject();

	return S_OK;
}