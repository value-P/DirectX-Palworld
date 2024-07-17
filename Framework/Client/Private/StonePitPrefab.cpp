// Default
#include "pch.h"
#include "StonePitPrefab.h"

// Module, Manager
#include "EngineModule.h"
#include "ClientModule.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "PhysXManager.h"
#include "GameManager.h"
#include "BuildManager.h"

// Component
#include "BoxComponent.h"
#include "MeshComponent.h"

#include "Model.h"	
#include "MeshBuffer.h"
#include "Material.h"

std::shared_ptr<CStonePitPrefab> Client::CStonePitPrefab::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CStonePitPrefab> spStonePitPrefab = make_shared<CStonePitPrefab>();
	CHECKF(spStonePitPrefab, L"Failed to create CStonePitPrefab : CStonePitPrefab");
	spStonePitPrefab->Initialize(_spGameObjectDesc);

	return spStonePitPrefab;
}

HRESULT Client::CStonePitPrefab::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CArchitecture::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_spBuildManager = CClientModule::GetInstance()->GetBuildManager();
	m_wpPhysXManager = CEngineModule::GetInstance()->GetPhysXManager();

	/* gameobject */
	shared_ptr<FArchitecturePrefabDesc> spArchitecturePrefabDesc = static_pointer_cast<FArchitecturePrefabDesc>(_spGameObjectDesc);

	m_vColliderExtents = spArchitecturePrefabDesc->m_vColliderExtents;
	m_vColliderCenter = spArchitecturePrefabDesc->m_vColliderCenter;
	m_vColliderRotate = spArchitecturePrefabDesc->m_vColliderRotate;

	m_wpPlayer = spArchitecturePrefabDesc->m_spPlayer;
	m_wstrShaderName = spArchitecturePrefabDesc->m_wstrShaderName;
	m_wstrModelFilePath = spArchitecturePrefabDesc->m_wstrModelFilePath;

	m_tBuildStatus.m_bBuildCompleted = true;
	m_bPrefab = true;

	InitializeMeshComponent();
	InitializeColliderComponent();

	return S_OK;
}

HRESULT Client::CStonePitPrefab::BeginPlay()
{
	if (FAILED(CArchitecture::BeginPlay())) { return E_FAIL; }

	m_wpPlayer = CClientModule::GetInstance()->GetGameManager()->GetPlayer();

	for (auto& MeshComponentPair : m_umapMeshComponents)
	{
		auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

		for (auto& ShaderTypePair : umapMeshInfos)
		{
			for (auto& tMeshInfo : ShaderTypePair.second)
			{
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"State", (int32)EArchitextureState::BUILDING);
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Alpha", 0.5f);
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", -0.4f);
			}
		}
	}

	return S_OK;
}

int32 Client::CStonePitPrefab::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecturePrefab::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CStonePitPrefab::Tick(float _fDeltaSeconds)
{
	switch (CArchitecturePrefab::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CStonePitPrefab::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecturePrefab::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CStonePitPrefab::EndPlay()
{
	if (FAILED(CArchitecturePrefab::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CStonePitPrefab::Release()
{
	CArchitecturePrefab::Release();
}

HRESULT Client::CStonePitPrefab::InitializeMeshComponent()
{
#pragma region StonePit_SM_StonePit_Acce
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName = L"StonePit_SM_StonePit_Acce";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_LBLEND_NONCULL;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f)));

	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	shared_ptr<CMeshComponent> spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	CHECKF(spMeshCom, L"Failed to create CMeshComponent : CStonePitPrefab");

	m_umapMeshComponents.emplace(L"StonePit_SM_StonePit_Acce", spMeshCom);
	AddComponent(L"StonePit_SM_StonePit_Acce", spMeshCom);
#pragma endregion
#pragma region StonePit_SM_StonePit_Fence
	tMeshComponentDesc.m_wstrComponentName = L"StonePit_SM_StonePit_Fence";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_LBLEND_NONCULL;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Architecture/StonePit_SM_StonePit_Fence.dat";
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f)));

	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	CHECKF(spMeshCom, L"Failed to create CMeshComponent : CStonePitPrefab");

	m_umapMeshComponents.emplace(L"StonePit_SM_StonePit_Fence", spMeshCom);
	AddComponent(L"StonePit_SM_StonePit_Fence", spMeshCom);
#pragma endregion

	return S_OK;
}

HRESULT Client::CStonePitPrefab::InitializeColliderComponent()
{
	// 박스 컴포넌트 생성 구문을 추가해야합니다.

	return S_OK;
}