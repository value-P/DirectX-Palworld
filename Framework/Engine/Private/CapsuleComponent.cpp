#include "pch.h"
#include "CapsuleComponent.h"

// manager
#include "CameraManager.h"
#include "PhysXManager.h"

#include "GameObject.h"

std::shared_ptr<CCapsuleComponent> Engine::CCapsuleComponent::Create(const std::shared_ptr<FCapsuleComponentDesc>& _spCapsuleComponentDesc)
{
	const std::shared_ptr<CCapsuleComponent> spCapsuleComponent = make_shared<CCapsuleComponent>();
	ENSUREF(spCapsuleComponent->InitializeComponent(_spCapsuleComponentDesc), L"Failed to initialize capsule component.");
	return spCapsuleComponent;
}

HRESULT Engine::CCapsuleComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// collider component
	if (FAILED(CColliderComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// capsule component
	shared_ptr<FCapsuleComponentDesc> spCapsuleComponentDesc = static_pointer_cast<FCapsuleComponentDesc>(_spComponentDesc);
	m_fRadius = spCapsuleComponentDesc->m_fRadius;
	m_fHalfHeight = spCapsuleComponentDesc->m_fHalfHeight;

	// etc
	PxShapeFlags eShapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE;
	if (m_bTrigger) { eShapeFlags |= PxShapeFlag::eTRIGGER_SHAPE; }
	else { eShapeFlags |= PxShapeFlag::eSIMULATION_SHAPE; }

	// 캡슐 모양을 생성합니다.
	m_pCapsuleShape = m_pPhysics->createShape
	(
		PxCapsuleGeometry(m_fRadius, m_fHalfHeight),
		*m_pMaterial,
		true,
		eShapeFlags
	);

	// 로컬 좌표계를 변경합니다.
	Matrix matLocal = Matrix::CreateRotationX(XMConvertToRadians(m_vLocalRotate.x));
	matLocal *= Matrix::CreateRotationY(XMConvertToRadians(m_vLocalRotate.y));
	matLocal *= Matrix::CreateRotationZ(XMConvertToRadians(m_vLocalRotate.z));
	matLocal *= Matrix::CreateTranslation(m_vLocalPosition);
	
	PxTransform matRelativePose(PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));

	PxTransform matLocalPose = CPhysXManager::GetPxTransformFromMatrix(matLocal);
	m_pCapsuleShape->setLocalPose(matRelativePose * matLocalPose);

	// 콜리전 레이어와 플래그를 설정합니다.
	if (m_eCollisionLayer != ECollisionLayer::ENUM_END && m_eCollisionFlag != ECollisionFlag::ENUM_END)
	{
		const PxFilterData tTriggerFilterData((PxU32)m_eCollisionLayer, (PxU32)m_eCollisionFlag, 0xffffffff, 0xffffffff);
		m_pCapsuleShape->setSimulationFilterData(tTriggerFilterData);
		m_pCapsuleShape->setQueryFilterData(tTriggerFilterData);
	}

	switch (m_eMobilityType)
	{
	case Engine::EMobilityType::STATIC:
	{
		// 정적인 캡슐을 만듭니다.
		m_pStaticRigidActor = m_pPhysics->createRigidStatic(m_oPxTransform);
		m_pStaticRigidActor->userData = this;
		m_pStaticRigidActor->attachShape(*m_pCapsuleShape);

		if (m_bEnableSimulation) { m_pStaticRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false); }
		else { m_pStaticRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true); }

		if (!m_bDeferredAdd)
		{
			m_pScene->addActor(*m_pStaticRigidActor);
		}
	}
	break;

	case Engine::EMobilityType::DYNAMIC:
	{
		// 동적인 캡슐을 만듭니다.
		m_pDynamicRigidActor = m_pPhysics->createRigidDynamic(m_oPxTransform);

		// 중력을 설정합니다.
		if (!m_bGravity) { m_pDynamicRigidActor->setActorFlags(PxActorFlag::eDISABLE_GRAVITY); }
		m_pDynamicRigidActor->setRigidDynamicLockFlags(static_cast<PxRigidDynamicLockFlags>(m_iAxisLockType));
		m_pDynamicRigidActor->userData = this;
		m_pDynamicRigidActor->attachShape(*m_pCapsuleShape);

		if (m_bEnableSimulation) { m_pDynamicRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false); }
		else { m_pDynamicRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true); }


		if (!m_bDeferredAdd)
		{
			m_pScene->addActor(*m_pDynamicRigidActor);
		}
	}
	break;

	default:
		CHECKF(false, L"Mobility type is not designated.");
		break;
	}

#ifdef _DEBUG

	// 디버그 드로우
	Vector4 vQuat = XMQuaternionRotationRollPitchYaw
	(
		XMConvertToRadians(m_vLocalRotate.x),
		XMConvertToRadians(m_vLocalRotate.y),
		XMConvertToRadians(m_vLocalRotate.z)
	);

	Vector3 vExtents = Vector3(m_fRadius, m_fHalfHeight, m_fRadius);
	m_tDxLocalOBB = BoundingOrientedBox(m_vLocalPosition, vExtents, vQuat);

	Vector3 vCenter = m_vLocalPosition;
	vCenter.y += m_fHalfHeight;
	m_tDxLocalSphere[0] = BoundingSphere(vCenter, m_fRadius);

	vCenter.y -= 2 * m_fHalfHeight;
	m_tDxLocalSphere[1] = BoundingSphere(vCenter, m_fRadius);

#endif // _DEBUG

	return S_OK;
}

HRESULT Engine::CCapsuleComponent::BeginPlay()
{
	if (FAILED(CColliderComponent::BeginPlay())) { return E_FAIL; }
	
	return S_OK;
}

int32 Engine::CCapsuleComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	if (m_eMobilityType == EMobilityType::DYNAMIC)
	{
		Matrix matWorldPosition = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
		PxTransform matTransform = CPhysXManager::GetPxTransformFromMatrix(matWorldPosition);
		m_pDynamicRigidActor->setGlobalPose(matTransform);

		// TODO; 콜라이더와 트랜스폼 간의 상호 업데이트가 올바르게 이루어지는지 확인해야 합니다.
		if (!m_bTrigger)
		{
			PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();
			Matrix matWorldPosition = CPhysXManager::GetMatrixFromPxTransform(matTransform);
			m_wpOwnerGameObject.lock()->SetTransformMatrix(matWorldPosition);
		}
	}

	return 0;
}

int32 Engine::CCapsuleComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CCapsuleComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

#ifdef _DEBUG
	Matrix matGameObject = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
	m_tDxLocalOBB.Transform(m_tDxWorldOBB, matGameObject);
	m_tDxLocalSphere[0].Transform(m_tDxWorldSphere[0], matGameObject);
	m_tDxLocalSphere[1].Transform(m_tDxWorldSphere[1], matGameObject);
#endif // _DEBUG

	return 0;
}

HRESULT Engine::CCapsuleComponent::EndPlay()
{
	if (FAILED(CColliderComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CCapsuleComponent::Render(int32 _iShaderPath)
{
	if (!m_bRenderDebug) { return; }

#ifdef _DEBUG
	/*
	m_cpDeviceContext->GSSetShader(nullptr, nullptr, 0);

	m_spBasicEffect->SetWorld(XMMatrixIdentity());

	m_spBasicEffect->SetView(m_wpCameraManager.lock()->GetCurViewMatrix());

	m_spBasicEffect->SetProjection(m_wpCameraManager.lock()->GetCurPerspectiveProjectionMatrix());

	m_spBasicEffect->Apply(m_cpDeviceContext.Get());

	m_cpDeviceContext->IASetInputLayout(m_cpInputLayout.Get());

	m_spPrimitiveBatch->Begin();

	DX::Draw(m_spPrimitiveBatch.get(), m_tDxWorldOBB, m_bCollided ? Colors::Red : Colors::Black);
	DX::Draw(m_spPrimitiveBatch.get(), m_tDxWorldSphere[0], m_bCollided ? Colors::Red : Colors::Black);
	DX::Draw(m_spPrimitiveBatch.get(), m_tDxWorldSphere[1], m_bCollided ? Colors::Red : Colors::Black);

	m_spPrimitiveBatch->End();
	*/
#endif // _DEBUG

	CColliderComponent::Render(_iShaderPath);
}

void Engine::CCapsuleComponent::Release()
{
	PX_RELEASE(m_pCapsuleShape);
	
	CColliderComponent::Release();
}

HRESULT Engine::CCapsuleComponent::BindVariable()
{
	return S_OK;
}

HRESULT Engine::CCapsuleComponent::RemoveRigidActorFromScene()
{
	PX_RELEASE(m_pCapsuleShape);

	return CColliderComponent::RemoveRigidActorFromScene();
}

void Engine::CCapsuleComponent::ApplySimulation()
{
	if (m_bRequestSimulationFlag.load() == true)
	{
		if (!m_bTrigger)
		{
			m_pCapsuleShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, m_bRequestedSimulation.load());
		}
		else
		{
			m_pCapsuleShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_bRequestedSimulation.load());
		}

		m_pCapsuleShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, m_bRequestedSimulation.load());

		if (m_pStaticRigidActor)
		{
			m_pStaticRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !m_bRequestedSimulation.load());
		}

		if (m_pDynamicRigidActor)
		{
			m_pDynamicRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !m_bRequestedSimulation.load());
		}

		m_bRequestSimulationFlag.store(false);
	}
}

void Engine::CCapsuleComponent::ApplySimulationDirectly(bool _bSimulation)
{
	if (!m_bTrigger)
	{
		m_pCapsuleShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, _bSimulation);
	}
	else
	{
		m_pCapsuleShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, _bSimulation);
	}

	m_pCapsuleShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, _bSimulation);

	if (m_pStaticRigidActor)
	{
		m_pStaticRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !_bSimulation);
	}

	if (m_pDynamicRigidActor)
	{
		m_pDynamicRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !_bSimulation);
	}
}

void Engine::CCapsuleComponent::SetRadius(float _fRadius)
{
	if (m_eMobilityType == EMobilityType::DYNAMIC)
	{
		m_pCapsuleShape->setGeometry(PxCapsuleGeometry(_fRadius, m_fHalfHeight));
		m_fRadius = _fRadius;

		// TODO; 디버그 드로우 업데이트
	}
}

void Engine::CCapsuleComponent::SetHalfHeight(float _fHalfHeight)
{
	if (m_eMobilityType == EMobilityType::DYNAMIC)
	{
		m_pCapsuleShape->setGeometry(PxCapsuleGeometry(m_fRadius, _fHalfHeight));
		m_fHalfHeight = _fHalfHeight;

		// TODO; 디버그 드로우 업데이트
	}
}

void Engine::CCapsuleComponent::SetLocalPosition(const Vector3& _vLocalPosition)
{
	if (!m_pCapsuleShape) { return; }

	Matrix matLocal = Matrix::CreateTranslation(_vLocalPosition);
	PxTransform matLocalPose = CPhysXManager::GetPxTransformFromMatrix(matLocal);
	m_pCapsuleShape->setLocalPose(matLocalPose);

	/*
`	// TODO; 디버그 드로우 업데이트
	m_vCenter = _vLocalPosition;
	m_tDxLocalSphere.Center = m_vCenter;
	*/
}
