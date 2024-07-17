#include "pch.h"
#include "TriangleMeshComponent.h"

#include "Model.h"
#include "MeshBuffer.h"

#include "EngineModule.h"
#include "InputManager.h"

std::shared_ptr<CTriangleMeshComponent> Engine::CTriangleMeshComponent::Create(const std::shared_ptr<FTriangleMeshComponentDesc>& _spTriangleMeshComponentDesc)
{
	const std::shared_ptr<CTriangleMeshComponent> spTriangleMeshComponent = make_shared<CTriangleMeshComponent>();
	ENSUREF(spTriangleMeshComponent->InitializeComponent(_spTriangleMeshComponentDesc), L"Failed to initialize triangle mesh component.");
	return spTriangleMeshComponent;
}

HRESULT Engine::CTriangleMeshComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// collider component desc
	if (FAILED(CColliderComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// triangle mesh component desc
	shared_ptr<FTriangleMeshComponentDesc> spTriangleMeshComponentDesc = static_pointer_cast<FTriangleMeshComponentDesc>(_spComponentDesc);
	m_wstrTriangleMeshFilePath = spTriangleMeshComponentDesc->m_wstrTriangleMeshFilePath;
	m_bInsertDirectly = spTriangleMeshComponentDesc->m_bInsertDirectly;

	// etc
	CreateTriangleMeshFromFile();
	return S_OK;
}

HRESULT Engine::CTriangleMeshComponent::BeginPlay()
{
	// if (FAILED(CColliderComponent::BeginPlay())) { return E_FAIL; }

	for (const auto& spRigidStatic : m_vecRigidStatics)
	{
		if (!m_pScene->addActor(*spRigidStatic))
		{
			CHECK(false);
		}
	}

	return S_OK;
}

int32 Engine::CTriangleMeshComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1: break;
	default: break;
	}

	return 0;
}

int32 Engine::CTriangleMeshComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::TickComponent(_fDeltaSeconds))
	{
	case -1: break;
	default: break;
	}

	return 0;
}

int32 Engine::CTriangleMeshComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1: break;
	default: break;
	}

	return 0;
}

HRESULT Engine::CTriangleMeshComponent::EndPlay()
{
	if (FAILED(CColliderComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

HRESULT Engine::CTriangleMeshComponent::BindVariable()
{
	if (FAILED(CColliderComponent::BindVariable())) { return E_FAIL; }

	return S_OK;
}

void Engine::CTriangleMeshComponent::Render(int32 _iShaderPath)
{
	CColliderComponent::Render(_iShaderPath);
}

void Engine::CTriangleMeshComponent::Release()
{
	for (auto& spOutBuffer : m_vecOutBuffers)
	{
		spOutBuffer = nullptr;
	}
	m_vecOutBuffers.clear();

	for (auto& pTriangleMesh : m_vecTriangleMeshes)
	{
		if (pTriangleMesh)
		{
			PX_RELEASE(pTriangleMesh)
		}
	}
	m_vecTriangleMeshes.clear();

	for (auto& pRigidbodyStatic : m_vecRigidStatics)
	{
		if (pRigidbodyStatic)
		{
			m_pScene->removeActor(*pRigidbodyStatic);
			PX_RELEASE(pRigidbodyStatic);
		}
	}
	m_vecRigidStatics.clear();

	CColliderComponent::Release();
}

HRESULT Engine::CTriangleMeshComponent::RemoveTriangleMeshFromScene()
{
	// 트라이앵글 메시를 없앱니다.
	for (auto& pTriangleMesh : m_vecTriangleMeshes)
	{
		if (pTriangleMesh)
		{
			PX_RELEASE(pTriangleMesh)
		}
	}
	m_vecTriangleMeshes.clear();

	// 스태틱 액터를 없앱니다.
	for (auto& pRigidbodyStatic : m_vecRigidStatics)
	{
		if (pRigidbodyStatic)
		{
			m_pScene->removeActor(*pRigidbodyStatic);
			PX_RELEASE(pRigidbodyStatic);
		}
	}
	m_vecRigidStatics.clear();

	return S_OK;
}

void Engine::CTriangleMeshComponent::SetSimulation(bool _bSimulation)
{
	for (const auto& spRigidStatic : m_vecRigidStatics)
	{
		spRigidStatic->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !_bSimulation);
	}
}

void Engine::CTriangleMeshComponent::CreateTriangleMeshFromFile()
{
	m_spModel = CModel::Create(m_wstrTriangleMeshFilePath, Matrix::CreateScale(0.01f), EMeshManageType::UNIQUE, nullptr);

	auto& umapMeshInfos = m_spModel->GetMeshInfos();
	for (auto& pair : umapMeshInfos)
	{
		m_iNumTriangleMeshes = static_cast<int32>(pair.second.size());

		for (auto& tMeshInfo : pair.second)
		{
			// 트라이앵글 메시의 좌표를 DX로 변경합니다.
			vector<Vector3>& vecVertexPositions = tMeshInfo->m_spMeshBuffer->GetVertexPositions();
			for (auto& vPosition : vecVertexPositions)
			{
				vPosition = Vector3::Transform(vPosition, tMeshInfo->m_matTransform);
			}

			vector<UINT32>& vecVertexIndices = tMeshInfo->m_spMeshBuffer->GetVertexIndices();

			PxTriangleMeshDesc oTriangleMeshDesc;
			oTriangleMeshDesc.points.count = (PxU32)vecVertexPositions.size();
			oTriangleMeshDesc.points.stride = sizeof(PxVec3);
			oTriangleMeshDesc.points.data = vecVertexPositions.data();

			oTriangleMeshDesc.triangles.count = (PxU32)vecVertexIndices.size() / 3;
			oTriangleMeshDesc.triangles.stride = 3 * sizeof(PxU32);
			oTriangleMeshDesc.triangles.data = vecVertexIndices.data();

			PxTolerancesScale oTolerancesScale;
			PxCookingParams tCookingParams(oTolerancesScale);

			// 삼각형 메시를 굽는데 사용할 수 있는 옵션입니다.
			tCookingParams.buildGPUData = true;
			tCookingParams.midphaseDesc = PxMeshMidPhase::eBVH33;
			tCookingParams.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
			tCookingParams.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
			tCookingParams.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
			tCookingParams.midphaseDesc.mBVH33Desc.meshSizePerformanceTradeOff = 0.55f;

			// 삼각형 메시를 굽는 즉시 신에 추가합니다.
			if (m_bInsertDirectly)
			{
				PxTriangleMesh* pTriangleMesh = PxCreateTriangleMesh(tCookingParams, oTriangleMeshDesc, m_pPhysics->getPhysicsInsertionCallback());

				PxMeshScale oMeshScale;
				PxMeshGeometryFlags tMeshGeometryFlags;

				PxRigidStatic* pRigidStatic = PxCreateStatic
				(
					*m_pPhysics,
					m_oPxTransform,
					PxTriangleMeshGeometry(pTriangleMesh, oMeshScale, tMeshGeometryFlags),
					*m_pMaterial
				);

				// 콜리전 레이어와 플래그를 설정합니다.
				if (m_eCollisionLayer != ECollisionLayer::ENUM_END && m_eCollisionFlag != ECollisionFlag::ENUM_END)
				{
					PxShape* pShape = nullptr;
					pRigidStatic->getShapes(&pShape, 1, 0);

					const PxFilterData tTriggerFilterData((PxU32)m_eCollisionLayer, (PxU32)m_eCollisionFlag, 0xffffffff, 0xffffffff);
					pShape->setSimulationFilterData(tTriggerFilterData);
					pShape->setQueryFilterData(tTriggerFilterData);
				}

				pRigidStatic->userData = this;
				// m_pScene->addActor(*pRigidStatic);

				m_vecRigidStatics.push_back(pRigidStatic);
				m_vecTriangleMeshes.push_back(pTriangleMesh);
			}
			// 삼각형 메시를 스트림에 저장한 후, 나중에 신에 추가합니다.
			else
			{
				shared_ptr<PxDefaultMemoryOutputStream> spOutBuffer = make_shared<PxDefaultMemoryOutputStream>();
				PxCookTriangleMesh(tCookingParams, oTriangleMeshDesc, *spOutBuffer);
				m_vecOutBuffers.push_back(spOutBuffer);
			}
		}
	}

	m_vecOutBuffers.shrink_to_fit();
	m_iOutBufferSize = (int32)m_vecOutBuffers.size();
}

HRESULT Engine::CTriangleMeshComponent::AddTriangleMeshToScene()
{
	for (int32 iIndex = 0; iIndex < m_iOutBufferSize; ++iIndex)
	{
		PxDefaultMemoryInputData oInputData(m_vecOutBuffers[iIndex]->getData(), m_vecOutBuffers[iIndex]->getSize());
		PxTriangleMesh* pTriangleMesh = m_pPhysics->createTriangleMesh(oInputData);
		m_iTriangleMeshSize = m_vecOutBuffers[iIndex]->getSize();

		PxMeshScale oMeshScale;
		PxMeshGeometryFlags tMeshGeometryFlags;

		PxRigidStatic* pRigidStatic = PxCreateStatic
		(
			*m_pPhysics,
			m_oPxTransform,
			PxTriangleMeshGeometry(pTriangleMesh, oMeshScale, tMeshGeometryFlags),
			*m_pMaterial
		);

		// 콜리전 레이어와 플래그를 설정합니다.
		if (m_eCollisionLayer != ECollisionLayer::ENUM_END && m_eCollisionFlag != ECollisionFlag::ENUM_END)
		{
			PxShape* pShape = nullptr;
			pRigidStatic->getShapes(&pShape, 1, 0);

			const PxFilterData tTriggerFilterData((PxU32)m_eCollisionLayer, (PxU32)m_eCollisionFlag, 0xffffffff, 0xffffffff);
			pShape->setSimulationFilterData(tTriggerFilterData);
		}

		pRigidStatic->userData = this;
		m_pScene->addActor(*pRigidStatic);

		m_vecRigidStatics.emplace_back(pRigidStatic);
		m_vecTriangleMeshes.emplace_back(pTriangleMesh);
	}

	m_vecRigidStatics.shrink_to_fit();
	m_vecTriangleMeshes.shrink_to_fit();
	return S_OK;
}
