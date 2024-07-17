#include "pch.h"
#include "PhysXManager.h"

// component
#include "ColliderComponent.h"
#include "SphereComponent.h"
#include "BoxComponent.h"
#include "CapsuleComponent.h"

// actor
#include "GameObject.h"
#include "Character.h"

HRESULT Engine::CPhysXManager::Initialize()
{
	ENSUREF(InitializePhysics(), L"Failed to initialize Physics");
	ENSUREF(InitializeScene(), L"Failed to initialize Physics");
	return S_OK;
}

int32 Engine::CPhysXManager::Tick(float _fDeltaSeconds)
{
	m_pScene->simulate(_fDeltaSeconds);
	m_pScene->fetchResults(true);
	return 0;
}

void Engine::CPhysXManager::Release()
{
	PX_RELEASE(m_pControllerManager);
	PX_RELEASE(m_pScene);
	PX_RELEASE(m_pCudaContextManager);
	PX_RELEASE(m_pPhysics);
	PX_RELEASE(m_pDispatcher);

	if (m_pPvd)
	{
		PxPvdTransport* pTransport = m_pPvd->getTransport();
		m_pPvd->release();
		m_pPvd = nullptr;
		PX_RELEASE(pTransport);
	}

	PX_RELEASE(m_pFoundation);
}

HRESULT Engine::CPhysXManager::InitializePhysics()
{
	// �Ŀ�̼��� �����մϴ�.
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_oAllocator, m_oErrorCallback);
	if (!m_pFoundation) { return E_FAIL; }

	// ���־� ����Ÿ� �����մϴ�.
	m_pPvd = PxCreatePvd(*m_pFoundation);
	if (!m_pPvd) { return E_FAIL; }

	PxPvdTransport* pTransport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	m_pPvd->connect(*pTransport, PxPvdInstrumentationFlag::eALL);

	// ���������� ����� �� �ִ� ���� ������ �����մϴ�.
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, nullptr);
	if (!m_pPhysics) { return E_FAIL; }

	// CUDA �Ŵ����� �����մϴ�.
	PxCudaContextManagerDesc oCudaContextManagerDesc;
	m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, oCudaContextManagerDesc, PxGetProfilerCallback());
	if (m_pCudaContextManager)
	{
		if (!m_pCudaContextManager->contextIsValid())
		{
			PX_RELEASE(m_pCudaContextManager);
		}
	}

	return S_OK;
}

HRESULT Engine::CPhysXManager::InitializeScene()
{
	{
		// ���� �����մϴ�.
		PxSceneDesc oSceneDesc(m_pPhysics->getTolerancesScale());

		// �̺�Ʈ �ݹ� ����� �����մϴ�.
		oSceneDesc.simulationEventCallback = &m_oSimulationEventCallBack;

		// �߷��� �����մϴ�.
		oSceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

		// ������ ���� �����մϴ�.
		m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
		oSceneDesc.cpuDispatcher = m_pDispatcher;

		// ����, �� ���� ������ �浹 ���θ� �����մϴ�.
		oSceneDesc.filterShader = CSimulationEventCallback::PxTriggerSimulationFilterShader;

		// CUDA�� Ȱ��ȭ�մϴ�.
		oSceneDesc.cudaContextManager = m_pCudaContextManager;
		oSceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
		oSceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
		oSceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
		oSceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
		oSceneDesc.gpuMaxNumPartitions = 4;

		m_pScene = m_pPhysics->createScene(oSceneDesc);

#ifdef _DEBUG
		// ���ɿ� ū ���ϰ� ���� ������ ������� ���� �մϴ�.
		/*
		m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
		m_pScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 10.0f);		// ���� ��
		m_pScene->setVisualizationParameter(PxVisualizationParameter::eWORLD_AXES, 10.0f);		// ���� ��
		*/
#endif // _DEBUG
	}

	{
		// ��Ʈ�ѷ� �Ŵ����� �����մϴ�.
		m_pControllerManager = PxCreateControllerManager(*m_pScene);

		// ������ ���� ����� Ȱ��ȭ�մϴ�.
		m_pControllerManager->setOverlapRecoveryModule(true);
	}

	{
		// ���־� ����Ÿ� �����մϴ�.
		/*
		PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient();
		if (pPvdClient)
		{
			pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
		*/
	}

	return S_OK;
}

PxTransform Engine::CPhysXManager::GetPxTransformFromMatrix(Matrix& _matTransform)
{
	Vector3 vTranslate = Vector3::Zero;
	Vector3 vScale = Vector3::One;
	Quaternion vQuaternion = Quaternion::Identity;
	_matTransform.Decompose(vScale, vQuaternion, vTranslate);

	return PxTransform(PxVec3(vTranslate.x, vTranslate.y, vTranslate.z), PxQuat(vQuaternion.x, vQuaternion.y, vQuaternion.z, vQuaternion.w));
}

Matrix Engine::CPhysXManager::GetMatrixFromPxTransform(PxTransform& _pxTransform)
{
	Vector3 vTranslate = { _pxTransform.p.x, _pxTransform.p.y, _pxTransform.p.z };
	Quaternion vQuaternion = { _pxTransform.q.x, _pxTransform.q.y, _pxTransform.q.z, _pxTransform.q.w };

	Matrix matTransform = Matrix::Identity;
	matTransform *= Matrix::CreateFromQuaternion(vQuaternion);
	matTransform.Translation(vTranslate);
	return matTransform;
}

Vector3 Engine::CPhysXManager::GetVector3FromPxVec3(PxVec3& _pxVec3)
{
	return Vector3(_pxVec3.x, _pxVec3.y, _pxVec3.z);
}

PxVec3 Engine::CPhysXManager::GetPxVec3FromVector3(Vector3& _vVector3)
{
	return PxVec3(_vVector3.x, _vVector3.y, _vVector3.z);
}

PxQuat Engine::CPhysXManager::GetPxQuatFromQuaternion(Quaternion& _qtQuaternion)
{
	return PxQuat(_qtQuaternion.x, _qtQuaternion.y, _qtQuaternion.z, _qtQuaternion.w);
}

bool Engine::CPhysXManager::SingleRayCast
(
	const Vector3& _vPosition,
	const Vector3& _vDirection,
	float _fMaxDistance,
	ECollisionLayer _eCollisionLayer,
	PxRaycastHit* _pRaycastHit
)
{
	Vector3 vPosition = _vPosition;
	PxVec3 pxPosition = GetPxVec3FromVector3(vPosition);

	Vector3 vDirection = _vDirection;
	vDirection.Normalize();
	PxVec3 pxDirection = GetPxVec3FromVector3(vDirection);

	PxReal fMaxDistance = _fMaxDistance;

	// ��ġ�� ���� ���͸� ���մϴ�.
	PxHitFlags tHitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

	// �⺻������ ���� ������(closest hit) �浹�� ���͸� ���մϴ�.
	PxQueryFilterData tFilterData;
	tFilterData.data.word0 = (int32)_eCollisionLayer;

	PxRaycastBuffer tRaycastBuffer;
	if (m_pScene->raycast(pxPosition, pxDirection, fMaxDistance, tRaycastBuffer, tHitFlags, tFilterData))
	{
		*_pRaycastHit = tRaycastBuffer.block;
		return true;
	}

	return false;
}

bool Engine::CPhysXManager::SingleRayCast(const Vector3& _vPosition, const Vector3& _vDirection, float _fMaxDistance, int32 _iCollisionFlag, PxRaycastHit* _pRaycastHit)
{
	Vector3 vPosition = _vPosition;
	PxVec3 pxPosition = GetPxVec3FromVector3(vPosition);

	Vector3 vDirection = _vDirection;
	vDirection.Normalize();
	PxVec3 pxDirection = GetPxVec3FromVector3(vDirection);

	PxReal fMaxDistance = _fMaxDistance;

	// ��ġ�� ���� ���͸� ���մϴ�.
	PxHitFlags tHitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;// | PxHitFlag::eANY_HIT;

	// �⺻������ ���� ������(closest hit) �浹�� ���͸� ���մϴ�.
	PxQueryFilterData tFilterData;
	tFilterData.data.word0 = _iCollisionFlag;

	PxRaycastBuffer tRaycastBuffer;
	if (m_pScene->raycast(pxPosition, pxDirection, fMaxDistance, tRaycastBuffer, tHitFlags, tFilterData))
	{
		*_pRaycastHit = tRaycastBuffer.block;
		return true;
	}

	return false;
}

int32 Engine::CPhysXManager::MultiRayCast
(
	const Vector3& _vPosition,
	const Vector3& _vDirection,
	float _fMaxDistance,
	ECollisionLayer _eCollisionLayer,
	std::vector<PxRaycastHit>& _vecRaycastHits
)
{
	Vector3 vPosition = _vPosition;
	PxVec3 pxPosition = GetPxVec3FromVector3(vPosition);

	Vector3 vDirection = _vDirection;
	vDirection.Normalize();
	PxVec3 pxDirection = GetPxVec3FromVector3(vDirection);

	// �ִ� ���� �Ÿ��� ���մϴ�.
	PxReal fMaxDistance = _fMaxDistance;

	// ��ġ�� ���� ���͸� ���մϴ�.
	PxHitFlags tHitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

	PxQueryFilterData tFilterData;
	tFilterData.data.word0 = (int32)_eCollisionLayer;

	PxRaycastBuffer tRaycastBuffer(_vecRaycastHits.data(), (PxU32)_vecRaycastHits.size());

	if (m_pScene->raycast(pxPosition, pxDirection, fMaxDistance, tRaycastBuffer, tHitFlags, tFilterData))
	{
		return tRaycastBuffer.nbTouches;
	}

	return 0;
}

int32 Engine::CPhysXManager::MultiRayCast(const Vector3& _vPosition, const Vector3& _vDirection, float _fMaxDistance, int32 _iCollisionFlag, std::vector<PxRaycastHit>& _vecRaycastHits)
{
	Vector3 vPosition = _vPosition;
	PxVec3 pxPosition = GetPxVec3FromVector3(vPosition);

	Vector3 vDirection = _vDirection;
	vDirection.Normalize();
	PxVec3 pxDirection = GetPxVec3FromVector3(vDirection);

	// �ִ� ���� �Ÿ��� ���մϴ�.
	PxReal fMaxDistance = _fMaxDistance;

	// ��ġ�� ���� ���͸� ���մϴ�.
	PxHitFlags tHitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

	PxQueryFilterData tFilterData;
	tFilterData.data.word0 = _iCollisionFlag;

	PxRaycastBuffer tRaycastBuffer(_vecRaycastHits.data(), (PxU32)_vecRaycastHits.size());

	if (m_pScene->raycast(pxPosition, pxDirection, fMaxDistance, tRaycastBuffer, tHitFlags, tFilterData))
	{
		return tRaycastBuffer.nbTouches;
	}

	return 0;
}

int32 Engine::CPhysXManager::BoxOverlap
(
	const Vector3& _vExtents,
	const Vector3& _vPosition,
	const Vector3 _vRotate,
	ECollisionLayer _eCollisionLayer,
	std::vector<PxOverlapHit>& _vecOverlapHits
)
{
	// �ڽ��� �����մϴ�.
	PxBoxGeometry tBoxGeometry(_vExtents.x, _vExtents.y, _vExtents.z);

	// ��ġ
	Quaternion qtRotateX = Quaternion::CreateFromAxisAngle(Vector3::Right, XMConvertToRadians(_vRotate.x));
	Quaternion qtRotateY = Quaternion::CreateFromAxisAngle(Vector3::Up, XMConvertToRadians(_vRotate.y));
	Quaternion qtRotateZ = Quaternion::CreateFromAxisAngle(Vector3::Backward, XMConvertToRadians(_vRotate.z));

	Matrix matTransform = Matrix::CreateFromQuaternion(qtRotateZ * qtRotateY * qtRotateX);
	matTransform.Translation(_vPosition);

	PxTransform tPose(CPhysXManager::GetPxTransformFromMatrix(matTransform));

	// ����
	PxOverlapBuffer tOverlapBuffer(_vecOverlapHits.data(), static_cast<uint32>(_vecOverlapHits.size()));

	// ����
	PxQueryFilterData tFilterData;
	tFilterData.data.word0 = (int32)_eCollisionLayer;

	if (m_pScene->overlap(tBoxGeometry, tPose, tOverlapBuffer, tFilterData))
	{
		return tOverlapBuffer.nbTouches;
	}

	return 0;
}

int32 Engine::CPhysXManager::SphereOverlap
(
	float _fRadius,
	const Vector3& _vPosition,
	const Vector3 _vRotate,
	ECollisionLayer _eCollisionLayer,
	std::vector<PxOverlapHit>& _vecOverlapHits
)

{
	// ���� �����մϴ�.
	PxSphereGeometry tSphereGeometry(_fRadius);

	// ��ġ
	Quaternion qtRotateX = Quaternion::CreateFromAxisAngle(Vector3::Right, XMConvertToRadians(_vRotate.x));
	Quaternion qtRotateY = Quaternion::CreateFromAxisAngle(Vector3::Up, XMConvertToRadians(_vRotate.y));
	Quaternion qtRotateZ = Quaternion::CreateFromAxisAngle(Vector3::Backward, XMConvertToRadians(_vRotate.z));

	Matrix matTransform = Matrix::CreateFromQuaternion(qtRotateZ * qtRotateY * qtRotateX);
	matTransform.Translation(_vPosition);

	PxTransform tPose(CPhysXManager::GetPxTransformFromMatrix(matTransform));

	// ����
	PxOverlapBuffer tOverlapBuffer(_vecOverlapHits.data(), static_cast<uint32>(_vecOverlapHits.size()));

	// ����
	PxQueryFilterData tFilterData;
	tFilterData.data.word0 = (int32)_eCollisionLayer;

	if (m_pScene->overlap(tSphereGeometry, tPose, tOverlapBuffer, tFilterData))
	{
		return tOverlapBuffer.nbTouches;
	}

	return 0;
}

PxFilterFlags Engine::CSimulationEventCallback::PxTriggerSimulationFilterShader
(
	PxFilterObjectAttributes attributes0,
	PxFilterData filterData0,
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize
)
{
	// �� �浹ü �߿� �ϳ��� Ʈ������ ���, 
   	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		if (IsFlagOverlapped(filterData0, filterData1) == false)
		{
			return PxFilterFlag::eKILL;
		}

		pairFlags |= PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	else
	{
		if (IsFlagOverlapped(filterData0, filterData1) == false)
		{
			return PxFilterFlag::eKILL;
		}

		pairFlags |= PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	return PxFilterFlag::eDEFAULT;
}

bool Engine::CSimulationEventCallback::IsFlagOverlapped(const PxFilterData& data0, const PxFilterData& data1)
{
	if ((data0.word0 & data1.word1) && (data0.word1 & data1.word0)) { return true; }

	return false;
}

void Engine::CSimulationEventCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void Engine::CSimulationEventCallback::onWake(PxActor** actors, PxU32 count)
{
}

void Engine::CSimulationEventCallback::onSleep(PxActor** actors, PxU32 count)
{
}

void Engine::CSimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	while (nbPairs--)
	{
		// ����� �����͸� �������� ���� ���
		if (!pairHeader.actors[0]->userData) { continue; }
		if (!pairHeader.actors[1]->userData) { continue; }

		CColliderComponent* pSrcCollider = static_cast<CColliderComponent*>(pairHeader.actors[0]->userData);
		CColliderComponent* pDstCollider = static_cast<CColliderComponent*>(pairHeader.actors[1]->userData);

		shared_ptr<CGameObject> spSrcGameObject = pSrcCollider->GetOwnerGameObject();
		shared_ptr<CGameObject> spDstGameObject = pDstCollider->GetOwnerGameObject();

		const PxContactPair& tCurrent = *pairs++;

		if (tCurrent.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND))
		{
			spSrcGameObject->OnCollisionEnter(pSrcCollider, pDstCollider);
			spDstGameObject->OnCollisionEnter(pDstCollider, pSrcCollider);
		}
		else if (tCurrent.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			spSrcGameObject->OnCollisionExit(pSrcCollider, pDstCollider);
			spDstGameObject->OnCollisionExit(pDstCollider, pSrcCollider);
		}
	}
}

void Engine::CSimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	while (count--)
	{
		const PxTriggerPair& tCurrent = *pairs++;

		// ����� �����͸� �������� ���� ���
		if (tCurrent.otherActor->userData == nullptr) { continue; }
		if (tCurrent.triggerActor->userData == nullptr) { continue; }

		// ���Ͱ� �ݶ��̴��� ���
		CComponent* pSrcComponent = static_cast<CComponent*>(tCurrent.triggerActor->userData);
		CComponent* pDstComponent = static_cast<CComponent*>(tCurrent.otherActor->userData);

		shared_ptr<CGameObject> spSrcGameObject = pSrcComponent->GetOwnerGameObject();
		shared_ptr<CGameObject> spDstGameObject = pDstComponent->GetOwnerGameObject();

		if (spSrcGameObject == nullptr) { continue; }
		if (spDstGameObject == nullptr) { continue; }

		if (tCurrent.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			spSrcGameObject->OnTriggerEnter(pSrcComponent, pDstComponent);
			spDstGameObject->OnTriggerEnter(pDstComponent, pSrcComponent);
		}
		else if (tCurrent.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			spSrcGameObject->OnTriggerExit(pSrcComponent, pDstComponent);
			spDstGameObject->OnTriggerExit(pDstComponent, pSrcComponent);
		}
	}
}

void Engine::CSimulationEventCallback::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}
