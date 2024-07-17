// Default
#include "pch.h"
#include "LumberYardPrefab.h"

// GameObject
#include "Tree.h"

// Module, Manager
#include "ClientLoop.h"
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

std::shared_ptr<CLumberYardPrefab> Client::CLumberYardPrefab::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CLumberYardPrefab> spLumberYardPrefab = make_shared<CLumberYardPrefab>();
	CHECKF(spLumberYardPrefab, L"Failed to create CLumberYardPrefab : CLumberYardPrefab");
	spLumberYardPrefab->Initialize(_spGameObjectDesc);

	return spLumberYardPrefab;
}

HRESULT Client::CLumberYardPrefab::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
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

HRESULT Client::CLumberYardPrefab::BeginPlay()
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

int32 Client::CLumberYardPrefab::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecturePrefab::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CLumberYardPrefab::Tick(float _fDeltaSeconds)
{
	switch (CArchitecturePrefab::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CLumberYardPrefab::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecturePrefab::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CLumberYardPrefab::EndPlay()
{
	if (FAILED(CArchitecturePrefab::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CLumberYardPrefab::Release()
{
	CArchitecturePrefab::Release();
}

HRESULT Client::CLumberYardPrefab::InitializeMeshComponent()
{
#pragma region Loggingcamp_Acce
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName = L"Loggingcamp_Acce";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) * Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f)));

	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	shared_ptr<CMeshComponent> spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	CHECKF(spMeshCom, L"Failed to create CMeshComponent : CLumberYardPrefab");

	m_umapMeshComponents.emplace(L"Loggingcamp_Acce", spMeshCom);
	AddComponent(L"Loggingcamp_Acce", spMeshCom);
#pragma endregion

#pragma region Loggingcamp_Fence
	tMeshComponentDesc.m_wstrComponentName = L"Loggingcamp_Fence";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Architecture/Loggingcamp_Fence.dat";
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) * Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f)));

	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	CHECKF(spMeshCom, L"Failed to create CMeshComponent : CLumberYardPrefab");

	m_umapMeshComponents.emplace(L"Loggingcamp_Fence", spMeshCom);
	AddComponent(L"Loggingcamp_Fence", spMeshCom);
#pragma endregion

	return S_OK;
}

HRESULT Client::CLumberYardPrefab::InitializeColliderComponent()
{
	// 박스 컴포넌트 생성 구문을 추가해야합니다.

	return S_OK;
}