// default
#include "pch.h"
#include "BoxComponent.h"

// manager
#include "PhysXManager.h"
#include "CameraManager.h"

#include "GameObject.h"

#include "MathUtility.h"

std::shared_ptr<CBoxComponent> Engine::CBoxComponent::Create(const std::shared_ptr<FBoxComponentDesc>& _spBoxComponentDesc)
{
	const std::shared_ptr<CBoxComponent> spBoxComponent = make_shared<CBoxComponent>();
	ENSUREF(spBoxComponent->InitializeComponent(_spBoxComponentDesc), L"Failed to initialize box component");
	return spBoxComponent;
}

HRESULT Engine::CBoxComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// collider component
	if (FAILED(CColliderComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// box component
	shared_ptr<FBoxComponentDesc> spBoxComponent = static_pointer_cast<FBoxComponentDesc>(_spComponentDesc);
	m_vExtents = spBoxComponent->m_vExtents;

	// etc
	PxShapeFlags eShapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE;
	if (m_bTrigger) { eShapeFlags |= PxShapeFlag::eTRIGGER_SHAPE; }
	else { eShapeFlags |= PxShapeFlag::eSIMULATION_SHAPE; }
	
	// 박스 모양을 생성합니다.
	m_pBoxShape = m_pPhysics->createShape
	(
		PxBoxGeometry(m_vExtents.x, m_vExtents.y, m_vExtents.z),
		*m_pMaterial,
		true,
		eShapeFlags
	);

	// 로컬 좌표계를 변경합니다.
	Matrix matLocal = Matrix::CreateRotationX(XMConvertToRadians(m_vLocalRotate.x));
	matLocal *= Matrix::CreateRotationY(XMConvertToRadians(m_vLocalRotate.y));
	matLocal *= Matrix::CreateRotationZ(XMConvertToRadians(m_vLocalRotate.z));
	matLocal *= Matrix::CreateTranslation(m_vLocalPosition);
	
	PxTransform matLocalPose = CPhysXManager::GetPxTransformFromMatrix(matLocal);
	m_pBoxShape->setLocalPose(matLocalPose);

	// 콜리전 레이어와 플래그를 설정합니다.
	if (m_eCollisionLayer != ECollisionLayer::ENUM_END && m_eCollisionFlag != ECollisionFlag::ENUM_END)
	{
		const PxFilterData tTriggerFilterData((PxU32)m_eCollisionLayer, (PxU32)m_eCollisionFlag, 0xffffffff, 0xffffffff);
		m_pBoxShape->setSimulationFilterData(tTriggerFilterData);
		m_pBoxShape->setQueryFilterData(tTriggerFilterData);
	}

	switch (m_eMobilityType)
	{
	case Engine::EMobilityType::STATIC:
	{
		// 정적인 박스를 만듭니다.
		m_pStaticRigidActor = m_pPhysics->createRigidStatic(m_oPxTransform);
		m_pStaticRigidActor->userData = this;
		m_pStaticRigidActor->attachShape(*m_pBoxShape);

		if (m_bEnableSimulation) { m_pStaticRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false); }
		else { m_pStaticRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true); }

		if (!m_bDeferredAdd)
		{
			if (!m_pScene->addActor(*m_pStaticRigidActor))
			{
				CHECK(false);
			}
		}
	}
	break;

	case Engine::EMobilityType::DYNAMIC:
	{
		// 동적인 박스를 만듭니다.
		m_pDynamicRigidActor = m_pPhysics->createRigidDynamic(m_oPxTransform);

		// 중력을 설정합니다.
		if (!m_bGravity) { m_pDynamicRigidActor->setActorFlags(PxActorFlag::eDISABLE_GRAVITY); }
		m_pDynamicRigidActor->setRigidDynamicLockFlags(static_cast<PxRigidDynamicLockFlags>(m_iAxisLockType));
		m_pDynamicRigidActor->userData = this;
		m_pDynamicRigidActor->attachShape(*m_pBoxShape);

		if (m_bEnableSimulation) { m_pDynamicRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false); }
		else { m_pDynamicRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true); }

		if (!m_bDeferredAdd)
		{
			if (!m_pScene->addActor(*m_pDynamicRigidActor))
			{
				CHECK(false);
			}
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
	m_tDxLocalOBB = BoundingOrientedBox(m_vLocalPosition, m_vExtents, vQuat);
#endif // _DEBUG

	return S_OK;
}

HRESULT Engine::CBoxComponent::BeginPlay()
{
	if (FAILED(CColliderComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CBoxComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	if (m_eMobilityType == EMobilityType::DYNAMIC)
	{
		// TODO; 콜라이더와 트랜스폼 간의 상호 업데이트가 올바르게 이루어지는지 확인해야 합니다.
		// 콜라이더 이동이 게임 오브젝트 이동에 영향을 줍니다.
		if (!m_bTrigger)
		{
			PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();
			m_wpOwnerGameObject.lock()->SetTransformMatrix(CPhysXManager::GetMatrixFromPxTransform(matTransform));
		}
		// 게임 오브젝트 이동이 트리거에 영향을 줍니다.
		else
		{
			Matrix matWorld = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
			m_pDynamicRigidActor->setGlobalPose(CPhysXManager::GetPxTransformFromMatrix(matWorld));
		}
	}
	return 0;
}

int32 Engine::CBoxComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CBoxComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	if (m_eMobilityType == EMobilityType::DYNAMIC)
	{
		// TODO; 콜라이더인 경우, 콜라이더와 트랜스폼 간의 상호 업데이트가 올바르게 이루어지는지 확인해야 합니다.

		// 콜라이더인 경우, 다이나믹 액터가 게임 오브젝트에 영향을 줍니다.
		if (!m_bTrigger)
		{
			PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();
			Matrix matWorld = CPhysXManager::GetMatrixFromPxTransform(matTransform);
			m_wpOwnerGameObject.lock()->SetTransformMatrix(matWorld);
		}

		// 트리거인 경우, 게임 오브젝트가 다이나믹 액터에 영향을 줍니다.
		if (m_bTrigger)
		{
			Matrix matWorld = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
			PxTransform matTransform = CPhysXManager::GetPxTransformFromMatrix(matWorld);
			m_pDynamicRigidActor->setGlobalPose(matTransform);
		}
	}

#ifdef _DEBUG
	Matrix matGameObject = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
	Matrix matScaleKilled = CMathUtility::GetScaledKilledMatrix(matGameObject);
	m_tDxLocalOBB.Transform(m_tDxWorldOBB, matGameObject);
#endif // _DEBUG

	return 0;
}

HRESULT Engine::CBoxComponent::EndPlay()
{
	if (FAILED(CColliderComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CBoxComponent::Render(int32 _iShaderPath)
{
	if (!m_bRenderDebug) { return; }

#ifdef _DEBUG
	
	m_cpDeviceContext->GSSetShader(nullptr, nullptr, 0);

	m_spBasicEffect->SetWorld(XMMatrixIdentity());

	m_spBasicEffect->SetView(m_wpCameraManager.lock()->GetCurViewMatrix());

	m_spBasicEffect->SetProjection(m_wpCameraManager.lock()->GetCurPerspectiveProjectionMatrix());

	m_spBasicEffect->Apply(m_cpDeviceContext.Get());

	m_cpDeviceContext->IASetInputLayout(m_cpInputLayout.Get());

	m_spPrimitiveBatch->Begin();

	DX::Draw(m_spPrimitiveBatch.get(), m_tDxWorldOBB, m_bCollided ? Colors::Red : Colors::Black);

	m_spPrimitiveBatch->End();
	
#endif // _DEBUG

	CColliderComponent::Render(_iShaderPath);
}

void Engine::CBoxComponent::Release()
{
	PX_RELEASE(m_pBoxShape);

	CColliderComponent::Release();
}

HRESULT Engine::CBoxComponent::BindVariable()
{
	return S_OK;
}

HRESULT Engine::CBoxComponent::RemoveRigidActorFromScene()
{
	PX_RELEASE(m_pBoxShape);

	return CColliderComponent::RemoveRigidActorFromScene();
}

void Engine::CBoxComponent::ApplySimulation()
{
	if (m_bRequestSimulationFlag.load() == true)
	{
		bool bRequested = m_bRequestedSimulation.load();

		if (!m_bTrigger)
		{
			m_pBoxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, bRequested);
		}
		else
		{
			m_pBoxShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, bRequested);
		}

		m_pBoxShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, bRequested);

		if (m_pStaticRigidActor)
		{
			m_pStaticRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !bRequested);
		}

		if (m_pDynamicRigidActor)
		{
			m_pDynamicRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !bRequested);
		}

		m_bRequestSimulationFlag.store(false);
	}
}

void Engine::CBoxComponent::ApplySimulationDirectly(bool _bSimulation)
{
	if (!m_bTrigger)
	{
		m_pBoxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, _bSimulation);
	}
	else
	{
		m_pBoxShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, _bSimulation);
	}

	m_pBoxShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, _bSimulation);

	if (m_pStaticRigidActor)
	{
		m_pStaticRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !_bSimulation);
	}

	if (m_pDynamicRigidActor)
	{
		m_pDynamicRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !_bSimulation);
	}
}

void Engine::CBoxComponent::SetExtents(const Vector3& _vExtents)
{
	if (m_eMobilityType == EMobilityType::DYNAMIC)
	{
		m_pBoxShape->setGeometry(PxBoxGeometry(_vExtents.x, _vExtents.y, _vExtents.z));
		m_vExtents = _vExtents;
		m_tDxLocalOBB.Extents = _vExtents;
	}
}

Vector3 Engine::CBoxComponent::GetLocalPosition()
{
	if (!m_pBoxShape) { return Vector3(0.f, 0.f, 0.f); }

	PxVec3 vLocal = m_pBoxShape->getLocalPose().p;

	return Vector3(vLocal.x, vLocal.y, vLocal.z);
}

void Engine::CBoxComponent::SetLocalPosition(const Vector3& _vLocalPosition)
{
	if (!m_pBoxShape) { return; }

	Matrix matLocal = Matrix::CreateTranslation(_vLocalPosition);
	PxTransform matLocalPose = CPhysXManager::GetPxTransformFromMatrix(matLocal);
	m_pBoxShape->setLocalPose(matLocalPose);
	
	// 디버그 드로우 박스의 중심을 업데이트합니다.
	m_vLocalPosition = _vLocalPosition;
	m_tDxLocalOBB.Center = _vLocalPosition;
}

void Engine::CBoxComponent::SetCollisionLayer(ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag)
{
	// 콜리전 레이어와 플래그를 설정합니다.
	if (_eCollisionLayer != ECollisionLayer::ENUM_END && _eCollisionFlag != ECollisionFlag::ENUM_END)
	{
		const PxFilterData tTriggerFilterData((PxU32)_eCollisionLayer, (PxU32)_eCollisionFlag, 0xffffffff, 0xffffffff);
		m_pBoxShape->setSimulationFilterData(tTriggerFilterData);
		m_pBoxShape->setQueryFilterData(tTriggerFilterData);
	}

	m_eCollisionLayer = _eCollisionLayer;
	m_eCollisionFlag = _eCollisionFlag;
}
