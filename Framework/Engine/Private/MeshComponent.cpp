#include "pch.h"
#include "MeshComponent.h"

#include "EngineModule.h"
#include "CameraManager.h"
#include "RenderManager.h"
#include "ShaderHandler.h"
#include "DeviceManager.h"

#include "Model.h"
#include "EffectShader.h"
#include "GameObject.h"
#include "Bone.h"
#include "MeshBuffer.h"
#include "Material.h"

shared_ptr<CMeshComponent> Engine::CMeshComponent::Create(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	const shared_ptr<CMeshComponent> spMeshComponent = make_shared<CMeshComponent>();
	ENSUREF(spMeshComponent->InitializeComponent(_spComponentDesc), L"Failed to initialize mesh component.");
	return spMeshComponent;
}

HRESULT Engine::CMeshComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	std::shared_ptr<FMeshComponentDesc> spMeshComponentDesc = std::static_pointer_cast<FMeshComponentDesc>(_spComponentDesc);
	m_wstrModelFilePath = spMeshComponentDesc->m_wstrModelFilePath;
	m_matPivot = spMeshComponentDesc->m_matPivot;
	m_wpPivotBone = spMeshComponentDesc->m_spPivotBone;

	if (spMeshComponentDesc->m_spParentMeshComponent)
	{
		m_wpParentModel = spMeshComponentDesc->m_spParentMeshComponent->GetModel();
	}

	m_eMeshManageType = spMeshComponentDesc->m_eMeshType;

	// 모델 생성
	m_spModel = CModel::Create(m_wstrModelFilePath, m_matPivot, m_eMeshManageType, m_wpParentModel.lock(), spMeshComponentDesc->m_wstrUpperBoneName, spMeshComponentDesc->m_wstrAnimationGroupJsonPath);
	CHECKF(m_spModel != nullptr, L"CMeshComponent : InitializeComponent Failed");

	m_wpShaderHandler = CShaderHandler::GetInstance();

	m_cpDevice = CEngineModule::GetInstance()->GetDeviceManager()->GetDevice();
	m_cpDeviceContext = CEngineModule::GetInstance()->GetDeviceManager()->GetDeviceContext();

	if (spMeshComponentDesc->m_eMeshType == EMeshManageType::UNIQUE)
	{
		auto& umapMeshes = m_spModel->GetMeshInfos();

		for (auto& Pair : umapMeshes)
		{
			switch (Pair.first)
			{
			case Engine::EShaderType::DEFAULT_ANIM:
				m_umapShaders[Pair.first] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_DefaultAnim_PNT2TBIBW");
				break;
			case Engine::EShaderType::PLAYEREYE_ANIM:
				m_umapShaders[Pair.first] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_PlayerEye_PNT2TBIBW");
				break;
			case Engine::EShaderType::MONSTERFACE_ANIM:
				m_umapShaders[Pair.first] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_MonsterFace_PNT2TBIBW");
				break;
			case Engine::EShaderType::DEFAULT_NONANIM:
				m_umapShaders[Pair.first] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_DefaultNonAnim_PNT2T");
				break;
			case Engine::EShaderType::LANDSCAPE_NONANIM:
				m_umapShaders[Pair.first] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_Landscape_PNT2T2T");
				break;
			case Engine::EShaderType::PALLIT_TOP_NONANIM:
				m_umapShaders[Pair.first] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_PalLitTopNonAnim_PNT2T");
				break;
			case Engine::EShaderType::SKY_NONANIM:
				m_umapShaders[Pair.first] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_SkyDome_PNT2T");
				break;
			case Engine::EShaderType::EXTERN_NONANIM:
				m_umapShaders[Pair.first] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_ExternNonAnim_PNT2T");
				break;
			case Engine::EShaderType::MONSTER_KITSUNEBI_FIRE:
				m_umapShaders[Pair.first] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_MonsterKitsunebiFire_PNT2TBIBW");
				break;
			case Engine::EShaderType::ARCHITECTURE:
				m_umapShaders[Pair.first] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_Architecture_PNT2T");
				break;
			default:
				continue;
			}

			m_umapVariables_MeshInfo[Pair.first] = m_umapShaders[Pair.first]->GetRawVariableByName("g_tMeshInfo");
			m_umapVariables_Textures[Pair.first] = m_umapShaders[Pair.first]->GetShaderResourceVariableByName("g_Textures");
		}
	}

	if (m_eMeshManageType == EMeshManageType::INSTANCE)
	{
		Matrix matWorld;
		if (!m_wpPivotBone.expired())
		{
			matWorld = m_wpPivotBone.lock()->GetCombinedTransformationMatrix();
		}
		if (m_wpOwnerGameObject.lock()->GetParentGameObject())
		{
			matWorld = matWorld * m_wpOwnerGameObject.lock()->GetParentGameObject()->GetTransformMatrix().value();
		}
		else
		{
			matWorld = matWorld * m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
		}
		matWorld._44 = static_cast<float>(m_wpOwnerGameObject.lock()->GetGameObjectID());
		m_spModel->RequestInstanceRender(matWorld, m_eRenderType);
	}

	return S_OK;
}

HRESULT Engine::CMeshComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CMeshComponent::PreTickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CMeshComponent::TickComponent(float _fDeltaSeconds)
{
	if ((m_spModel->GetModelType() == EModelType::ANIM) && m_bPlayingAnimation)
	{
		m_spModel->PlayAnimation(_fDeltaSeconds);
	}

	list<FAnimNotifyData> lstDownAnimNotifies = m_spModel->GetCurrentDownAnimNotifyData();
	if (lstDownAnimNotifies.size() > 0)
	{
		m_wpOwnerGameObject.lock()->AnimNotifyCall(lstDownAnimNotifies);
	}

	list<FAnimNotifyData> lstUpperAnimNotifies = m_spModel->GetCurrentUpperAnimNotifyData();
	if (lstUpperAnimNotifies.size() > 0)
	{
		m_wpOwnerGameObject.lock()->AnimNotifyCall(lstUpperAnimNotifies);
	}

	return 0;
}

int32 Engine::CMeshComponent::PostTickComponent(float _fDeltaSeconds)
{
	// if (m_bRenderActivate)
	// 활성화되어 있거나 컬링되어있지 않은 경우

	/*if (m_eMeshManageType == EMeshManageType::INSTANCE)
	{
		Matrix matWorld;
		if (!m_wpPivotBone.expired())
		{
			matWorld = m_wpPivotBone.lock()->GetCombinedTransformationMatrix();
		}
		if (m_wpOwnerGameObject.lock()->GetParentGameObject())
		{
			matWorld = matWorld * m_wpOwnerGameObject.lock()->GetParentGameObject()->GetTransformMatrix().value();
		}
		else
		{
			matWorld = matWorld * m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
		}
		matWorld._44 = static_cast<float>(m_wpOwnerGameObject.lock()->GetGameObjectID());
		m_spModel->RequestInstanceRender(matWorld, m_eRenderType);
	}
	else if (m_eMeshManageType == EMeshManageType::UNIQUE)
	{
		CEngineModule::GetInstance()->GetRenderManager()->AddComponent(shared_from_this(), m_eRenderType);
	}*/

	if (m_eMeshManageType == EMeshManageType::UNIQUE)
	{
		CEngineModule::GetInstance()->GetRenderManager()->AddComponent(shared_from_this(), m_eRenderType);
	}

	/*
	if (m_bCulled.load() == false)
	{
		CEngineModule::GetInstance()->GetRenderManager()->AddComponent(shared_from_this(), m_eRenderType);
		return 0;
	}

	if (m_bActive.load() == true)
	{
		CEngineModule::GetInstance()->GetRenderManager()->AddComponent(shared_from_this(), m_eRenderType);
		return 0;
	}
	*/
	return 0;
}

HRESULT Engine::CMeshComponent::EndPlay()
{
	return S_OK;
}

HRESULT Engine::CMeshComponent::BindVariable()
{
	return S_OK;
}

void Engine::CMeshComponent::Render(int32 _iShaderPath)
{
	// Binding
	Matrix matWorld;
	if (!m_wpPivotBone.expired())
	{
		matWorld = m_wpPivotBone.lock()->GetCombinedTransformationMatrix();
	}
	if (m_wpOwnerGameObject.lock()->GetParentGameObject())
	{
		matWorld = matWorld * m_wpOwnerGameObject.lock()->GetParentGameObject()->GetTransformMatrix().value();
	}
	else
	{
		matWorld = matWorld * m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
	}
	matWorld._44 = static_cast<float>(m_wpOwnerGameObject.lock()->GetGameObjectID());

	EModelType eModelType = m_spModel->GetModelType();
	EMeshManageType eMeshType = m_spModel->GetMeshType();
	auto& umapMeshes = m_spModel->GetMeshInfos();

	for (auto& Pair : umapMeshes)
	{
		if (Pair.first == EShaderType::ENUM_END)
		{
			continue;
		}

		if (eMeshType == EMeshManageType::UNIQUE)
		{
			for (auto& tMeshInfo : Pair.second)
			{
				if (eModelType == EModelType::ANIM)
				{
					vector<Matrix> vecMatrix(256, XMMatrixIdentity());
					tMeshInfo->m_spMeshBuffer->SetUpBoneMatrices(vecMatrix, m_spModel->GetBones());
					tMeshInfo->m_spMeshBuffer->SetMatrices(matWorld, vecMatrix.data());
					tMeshInfo->m_spMeshBuffer->BindVariable(m_umapVariables_MeshInfo[Pair.first], m_umapVariables_Textures[Pair.first]);
					m_umapShaders[Pair.first]->BeginPass(_iShaderPath);
					tMeshInfo->m_spMeshBuffer->Render();
				}
				else
				{
					tMeshInfo->m_spMeshBuffer->SetMatrices(tMeshInfo->m_matTransform, matWorld);
					tMeshInfo->m_spMeshBuffer->BindVariable(m_umapVariables_MeshInfo[Pair.first], m_umapVariables_Textures[Pair.first]);
					m_umapShaders[Pair.first]->BeginPass(_iShaderPath);
					tMeshInfo->m_spMeshBuffer->Render();
				}
			}
		}
	}
}

void Engine::CMeshComponent::Release()
{
	for (auto& Pair : m_umapShaders) { Pair.second = nullptr; }
	m_umapShaders.clear();

	for (auto& Pair : m_umapVariables_MeshInfo) { Pair.second = nullptr; }
	m_umapVariables_MeshInfo.clear();

	for (auto& Pair : m_umapVariables_Textures) { Pair.second = nullptr; }
	m_umapVariables_Textures.clear();

	if (m_spModel)
	{
		m_spModel->Release();
		m_spModel = nullptr;
	}
}

int32 Engine::CMeshComponent::GetMeshCount(EShaderType _eShaderType) const
{
	return m_spModel->GetNumMeshes(_eShaderType);
}

void Engine::CMeshComponent::SetRotationExceptBone(const wstring& _wstrBoneName)
{
	m_spModel->SetRotationExceptBone(_wstrBoneName);
}

shared_ptr<CBone> Engine::CMeshComponent::GetBonePtr(const wstring& _wstrBoneName)
{
	return m_spModel->GetBonePtr(_wstrBoneName);
}

shared_ptr<CBone> Engine::CMeshComponent::GetBonePtr(int32 _iBoneIndex)
{
	return m_spModel->GetBonePtr(_iBoneIndex);
}

void Engine::CMeshComponent::SetUseAnimation(bool _bUseAnimation, bool _bUseIdentityMatrices)
{
	m_spModel->SetUseAnimation(_bUseAnimation, _bUseIdentityMatrices);
}

FModelAnimData Engine::CMeshComponent::GetModelAnimationData()
{
	return m_spModel->GetModelAnimationData();
}

void Engine::CMeshComponent::SetTrackPosition(float _fTrackPosition)
{
	m_spModel->SetTrackPosition(_fTrackPosition);
}

void Engine::CMeshComponent::ShakeUpperBody(float _fShakePower, float _fShakeTime)
{
	m_spModel->ShakeUpperBody(_fShakePower, _fShakeTime);
}

int32 Engine::CMeshComponent::GetUpperCurrentLoop()
{
	return m_spModel->GetUpperCurrentLoop();
}

int32 Engine::CMeshComponent::GetDownCurrentLoop()
{
	return m_spModel->GetDownCurrentLoop();
}

void Engine::CMeshComponent::ChangeAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate)
{
	m_spModel->ChangeAnimation(_iNextAnim, _bPlayReverse, _fLoopStartRate);
}

void Engine::CMeshComponent::ChangeAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate)
{
	m_spModel->ChangeAnimation(_iNextAnim, _fLerpTime, _bPlayReverse, _fLoopStartRate);
}

void Engine::CMeshComponent::ChangeAnimation(const wstring& _wstrAnimName)
{
	m_spModel->ChangeAnimation(_wstrAnimName);
}

void Engine::CMeshComponent::ChangeDifferentAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate)
{
	m_spModel->ChangeDifferentAnimation(_iNextAnim, _bPlayReverse, _fLoopStartRate);
}

void Engine::CMeshComponent::ChangeUpperAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate)
{
	m_spModel->ChangeUpperAnimation(_iNextAnim, _bPlayReverse, _fLoopStartRate);
}

void Engine::CMeshComponent::ChangeUpperAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate)
{
	m_spModel->ChangeUpperAnimation(_iNextAnim, _fLerpTime, _bPlayReverse, _fLoopStartRate);
}

void Engine::CMeshComponent::ChangeDownAnimation(int32 _iNextAnim, bool _bPlayReverse, float _fLoopStartRate)
{
	m_spModel->ChangeDownAnimation(_iNextAnim, _bPlayReverse, _fLoopStartRate);
}

void Engine::CMeshComponent::ChangeDownAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse, float _fLoopStartRate)
{
	m_spModel->ChangeDownAnimation(_iNextAnim, _fLerpTime, _bPlayReverse, _fLoopStartRate);
}

void Engine::CMeshComponent::SetRotationAtBone(const wstring& _wstrBoneName, Vector3 _vRotation)
{
	m_spModel->SetBoneRotation(_wstrBoneName, _vRotation);
}

Engine::EModelType Engine::CMeshComponent::GetModelType() const
{
	return m_spModel->GetModelType();
}