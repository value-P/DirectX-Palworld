// Default
#include "pch.h"
#include "ArchitecturePrefab.h"

// Module, Manager
#include "EngineModule.h"
#include "ClientModule.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "PhysXManager.h"
#include "GameManager.h"
#include "BuildManager.h"
#include "Model.h"	
#include "MeshBuffer.h"
#include "Material.h"

// GameObject
#include "Player.h"

// Component
#include "BoxComponent.h"
#include "MeshComponent.h"

std::shared_ptr<CArchitecturePrefab> Client::CArchitecturePrefab::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CArchitecturePrefab> spArchitecturePrefab = make_shared<CArchitecturePrefab>();
	CHECKF(spArchitecturePrefab, L"Failed to create CArchitecturePrefab : CArchitecturePrefab");
	spArchitecturePrefab->Initialize(_spGameObjectDesc);

	return spArchitecturePrefab;
}

HRESULT Client::CArchitecturePrefab::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
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

HRESULT Client::CArchitecturePrefab::BeginPlay()
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
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", -0.5f);
			}
		}
	}

	return S_OK;
}

int32 Client::CArchitecturePrefab::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	if (!m_bHousingPrefab)
	{
		m_fRotationCount += _fDeltaSeconds;

		if (0.05f <= m_fRotationCount)
		{
			m_fRotationCount = 0.f;

			if (CEngineModule::GetInstance()->GetInputManager()->IsKeyPressed(DIK_Q))
			{
				SetRotateAccumulated(Vector3(0.f, 1.f, 0.f), -15.f);
			}
			if (CEngineModule::GetInstance()->GetInputManager()->IsKeyPressed(DIK_E))
			{
				SetRotateAccumulated(Vector3(0.f, 1.f, 0.f), 15.f);
			}
		}
	}

	return 0;
}

int32 Client::CArchitecturePrefab::Tick(float _fDeltaSeconds)
{
	switch (CArchitecture::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	FollowCameraLook();

	return 0;
}

int32 Client::CArchitecturePrefab::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CArchitecturePrefab::EndPlay()
{
	if (FAILED(CArchitecture::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CArchitecturePrefab::Release()
{
	CArchitecture::Release();
}

void Client::CArchitecturePrefab::RecycleFromPool()
{
	SetActive(true);

	// m_bActive = true;
	// m_bCulled = false;
}

void Client::CArchitecturePrefab::ReturnToPool()
{
	SetActive(false);

	// m_bActive = false;
	// m_bCulled = true;
}

HRESULT Client::CArchitecturePrefab::InitializeMeshComponent()
{
	// ���๰ �޽� ����
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName = L"ArchitecturePrefab MeshComponent";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_LBLEND_NONCULL;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
	if (EArchitectureType::MILL == m_eArchitectureType ||
		EArchitectureType::POT == m_eArchitectureType ||
		EArchitectureType::SPHERE_TABLE == m_eArchitectureType ||
		EArchitectureType::WEAPON_WORKBENCH == m_eArchitectureType ||
		EArchitectureType::IRON_BOX == m_eArchitectureType)
	{
		tMeshComponentDesc.m_matPivot *= Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), XMConvertToRadians(90.f)));
	}

	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	// tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	shared_ptr<CMeshComponent> spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));

	m_umapMeshComponents.emplace(L"ArchitecturePrefab MeshComponent", spMeshCom);
	return AddComponent(L"ArchitecturePrefab MeshComponent", spMeshCom);
}

HRESULT Client::CArchitecturePrefab::InitializeColliderComponent()
{
	// �ڽ� ������Ʈ ���� ������ �߰��ؾ��մϴ�.

	return S_OK;
}

void Client::CArchitecturePrefab::FollowCameraLook()
{
	if (m_wpPlayer.expired()) { return; }

	Vector3 vPlayerPos = m_wpPlayer.lock()->GetPosition().value();
	Vector3 vCameraLook = CEngineModule::GetInstance()->GetCameraManager()->GetCurCameraWorldForwardVector();

	SetPosition(vPlayerPos + vCameraLook * m_fBuildDistance);

	// ��ũ�ѷ� �Ÿ� ����
	if (CEngineModule::GetInstance()->GetInputManager()->GetMouseDelta(EMouseAxisType::Z_AXIS))
	{
		float fMouseZDelta = (float)CEngineModule::GetInstance()->GetInputManager()->GetMouseDelta(EMouseAxisType::Z_AXIS);
		fMouseZDelta *= 0.005f;

		m_fBuildDistance += fMouseZDelta;

		if (2.f > m_fBuildDistance)
		{
			m_fBuildDistance = 2.f;
		}
		else if (10.f < m_fBuildDistance)
		{
			m_fBuildDistance = 10.f;
		}
	}
	// Y��ǥ ����
	if (CEngineModule::GetInstance()->GetInputManager()->IsMouseMoved(EMouseAxisType::Y_AXIS))
	{
		float fMouseYDelta = (float)CEngineModule::GetInstance()->GetInputManager()->GetMouseDelta(EMouseAxisType::Y_AXIS);
		fMouseYDelta *= 0.01f;

		SetPosition(GetPosition().value() + Vector3(0.f, fMouseYDelta, 0.f));
	}
	// �ӽ�
	if (EArchitectureType::WOODEN_FOUNDATION == m_eArchitectureType ||
		EArchitectureType::WOODEN_WALL == m_eArchitectureType ||
		EArchitectureType::WOODEN_ROOF == m_eArchitectureType ||
		EArchitectureType::WOODEN_STAIRS == m_eArchitectureType ||
		EArchitectureType::WOODEN_DOOR == m_eArchitectureType ||
		EArchitectureType::WOODEN_WINDOW_WALL == m_eArchitectureType ||
		EArchitectureType::WOODEN_TRYANGLE_WALL == m_eArchitectureType ||
		EArchitectureType::WOODEN_SLOPE_ROOF == m_eArchitectureType)
	{
		return; 
	}

	// �������� �հ� ���� ���ϰ� �մϴ�.
	PxRaycastHit tRaycastHit;
	int32 iCollisionFlag = (int32)ECollisionLayer::ENVIRONMENT | (int32)ECollisionLayer::BUILDING;
	if (m_wpPhysXManager.lock()->SingleRayCast(GetPosition().value() + Vector3(0.f, 5.f, 0.f), Vector3(0.f, -1.f, 0.f), 100.f, iCollisionFlag, &tRaycastHit))
	{
		Vector3 vOriginPosition = GetPosition().value();
		vOriginPosition.y = tRaycastHit.position.y;

		if (EArchitectureType::WOODEN_FOUNDATION == m_eArchitectureType ||
			EArchitectureType::IRON_BOX == m_eArchitectureType)
		{
			// vOriginPosition.y += 0.15f;
		}

		SetPosition(vOriginPosition);
	}
}

bool Client::CArchitecturePrefab::IsBuildable()
{
	// �浹 ������Ʈ umap�� ũ�Ⱑ 1�̻��̸� false
	if (!m_umapCollidedGameObjects.empty()) { return false; }

	// ���� : �۾���� ���� ��ġ ����
	if (m_eArchitectureType == EArchitectureType::WORKBENCH) { return true; }

	// ���� : ������ �Ѱ��� ��ġ ����
	if (m_eArchitectureType == EArchitectureType::PAL_BOX)
	{
		if (!m_spBuildManager->IsExistBaseCamp()) { return true; }
		else { return false; }
	}

	// ���� �ȿ� �ִ°�?
	if (!m_spBuildManager->IsInBaseCamp(GetPosition().value())) { return false; }

	return true;
}

void Client::CArchitecturePrefab::OnCollisionEnter(CColliderComponent* _pSrcColliderComponent, CColliderComponent* _pDstColliderComponent)
{
	CHECKF(_pSrcColliderComponent->GetOwnerGameObject(), L"SrcCollider's Owner GameObject is nullptr : CHousingFreefab");
	CHECKF(_pDstColliderComponent->GetOwnerGameObject(), L"DstCollider's Owner GameObject is nullptr : CHousingFreefab");

	// ��� ������Ʈ�� ���� ID�� �����ɴϴ�.
	uint32 iGameObjectID = _pSrcColliderComponent->GetOwnerGameObject()->GetGameObjectID();

	// �̹� �浹 umap�� ����ִ� ������Ʈ�� return�մϴ�.
	if (m_umapCollidedGameObjects.contains(iGameObjectID)) { return; }

	// ��� ������Ʈ�� ID�� Ű ������ �浹 umap�� �߰��մϴ�.
	m_umapCollidedGameObjects.emplace(iGameObjectID, _pSrcColliderComponent->GetOwnerGameObject());

	// ���� ���������� �����մϴ�.
	// SetColor(Color(1.f, 0.f, 0.f, 0.8f));
}
void Client::CArchitecturePrefab::OnCollisionStay(CColliderComponent* _pSrcColliderComponent, CColliderComponent* _pDstColliderComponent)
{
}
void Client::CArchitecturePrefab::OnCollisionExit(CColliderComponent* _pSrcColliderComponent, CColliderComponent* _pDstColliderComponent)
{
	CHECKF(_pSrcColliderComponent->GetOwnerGameObject(), L"SrcCollider's Owner GameObject is nullptr : CHousingFreefab");
	CHECKF(_pDstColliderComponent->GetOwnerGameObject(), L"DstCollider's Owner GameObject is nullptr : CHousingFreefab");

	// ��� ������Ʈ�� ���� ID�� �����ɴϴ�.
	uint32 iGameObjectID = _pSrcColliderComponent->GetOwnerGameObject()->GetGameObjectID();

	// �̹� �浹 umap���� ������ ������Ʈ�� return�մϴ�.
	if (!m_umapCollidedGameObjects.contains(iGameObjectID)) { return; }

	// ��� ������Ʈ�� ID�� Ű ������ �浹 umap���� ã�Ƽ� erase�մϴ�.
	m_umapCollidedGameObjects.erase(iGameObjectID);

	// ���� �ʷϻ����� �����մϴ�.
	// SetColor(Color(0.f, 1.f, 0.f, 0.8f));
}