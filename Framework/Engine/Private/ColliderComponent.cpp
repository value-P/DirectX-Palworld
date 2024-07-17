// default
#include "pch.h"
#include "ColliderComponent.h"

// manager
#include "EngineModule.h"
#include "RenderManager.h"
#include "PhysXManager.h"
#include "CameraManager.h"
#include "DeviceManager.h"

// handler
#include "ShaderHandler.h"
#include "MathUtility.h"

#include "GameObject.h"
#include "Level.h"

HRESULT Engine::CColliderComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// collider component
	std::shared_ptr<FColliderComponentDesc> spColliderComponentDesc = static_pointer_cast<FColliderComponentDesc>(_spComponentDesc);
	m_eMobilityType		= spColliderComponentDesc->m_eMobilityType;
	m_eColliderType		= spColliderComponentDesc->m_eColliderType;
	m_eCollisionLayer	= spColliderComponentDesc->m_eCollisionLayer;
	m_eCollisionFlag	= spColliderComponentDesc->m_eCollisionFlag;
	m_iAxisLockType		= spColliderComponentDesc->m_iAxisLockType;
	m_fDensity			= spColliderComponentDesc->m_fDensity;
	m_fMass				= spColliderComponentDesc->m_fMass;
	m_bTrigger			= spColliderComponentDesc->m_bTrigger;
	m_bDeferredAdd		= spColliderComponentDesc->m_bDeferredAdd;
	m_bEnableSimulation = spColliderComponentDesc->m_bEnableSimulation;
	m_bGravity			= spColliderComponentDesc->m_bGravity;
	m_vLocalPosition	= spColliderComponentDesc->m_vLocalPosition;
	m_vLocalRotate		= spColliderComponentDesc->m_vLocalRotate;
	m_vMaterial			= spColliderComponentDesc->m_vMaterial;

	// etc
	shared_ptr<CPhysXManager> spPhysXManager = CEngineModule::GetInstance()->GetPhysXManager();
	m_pPhysics = spPhysXManager->GetPhysics();
	m_pScene = spPhysXManager->GetScene();
	m_pMaterial = m_pPhysics->createMaterial(m_vMaterial.x, m_vMaterial.y, m_vMaterial.z);
	
	Matrix matTransform = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();
	m_oPxTransform = CPhysXManager::GetPxTransformFromMatrix(matTransform);
	
	// 디버그 드로우
	m_cpDeviceContext = CEngineModule::GetInstance()->GetDeviceManager()->GetDeviceContext();
	m_wpRenderManager = CEngineModule::GetInstance()->GetRenderManager();
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();

	shared_ptr<CShaderHandler> spShaderHandler = CShaderHandler::GetInstance();
	m_cpInputLayout		= spShaderHandler->GetDebugDrawInputLayout();
	m_spPrimitiveBatch	= spShaderHandler->GetPrimitiveBatch();
	m_spBasicEffect		= spShaderHandler->GetBasicEffect();
	return S_OK;
}

HRESULT Engine::CColliderComponent::BeginPlay()
{
	DeferredAddActorToScene();
	return S_OK;
}

int32 Engine::CColliderComponent::PreTickComponent(float _fDeltaSeconds)
{
	m_fDeltaSeconds = _fDeltaSeconds;

	return 0;
}

int32 Engine::CColliderComponent::TickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CColliderComponent::PostTickComponent(float _fDeltaSeconds)
{
	m_wpRenderManager.lock()->AddComponent(shared_from_this(), ERenderType::RENDER_DEBUG);
	return 0;
}

HRESULT Engine::CColliderComponent::EndPlay()
{
	return S_OK;
}

HRESULT Engine::CColliderComponent::BindVariable()
{
	return S_OK;
}

void Engine::CColliderComponent::Render(int32 _iShaderPath)
{
}

void Engine::CColliderComponent::Release()
{
	if (m_pStaticRigidActor)
	{
		m_pStaticRigidActor->userData = nullptr;
		m_pScene->removeActor(*m_pStaticRigidActor);
		PX_RELEASE(m_pStaticRigidActor);
	}

	if (m_pDynamicRigidActor)
	{
		m_pDynamicRigidActor->userData = nullptr;
		m_pScene->removeActor(*m_pDynamicRigidActor);
		PX_RELEASE(m_pDynamicRigidActor);
	}

	PX_RELEASE(m_pMaterial);
	m_pPhysics = nullptr;
	m_pScene = nullptr;
}

void Engine::CColliderComponent::LookAt(const Vector3& _vPosition)
{
	if (m_bTrigger) { return; }

	PxTransform matPreTransform = m_pDynamicRigidActor->getGlobalPose();
	Matrix matWorld = CPhysXManager::GetMatrixFromPxTransform(matPreTransform);

	Vector3 vScale;
	Quaternion vQuaternion;
	Vector3 vPosition;
	matWorld.Decompose(vScale, vQuaternion, vPosition);

	Vector3 vForward = _vPosition - vPosition;
	vForward.Normalize();
	vForward.y = 0.0f;

	Vector3 vRight = XMVector3Cross(Vector4::UnitY, vForward);
	Vector3 vUp = XMVector3Cross(vForward, vRight);

	Vector4 vRightTransform = XMVector3Normalize(vRight) * vScale.x;
	::memcpy(&matWorld.m[(int32)ETransformType::RIGHT], &vRightTransform, sizeof(Vector4));

	Vector4 vUpTransform = XMVector3Normalize(vUp) * vScale.y;
	::memcpy(&matWorld.m[(int32)ETransformType::UP], &vUpTransform, sizeof(Vector4));

	Vector4 vForwardTransform = XMVector3Normalize(vForward) * vScale.z;
	::memcpy(&matWorld.m[(int32)ETransformType::FORWARD], &vForwardTransform, sizeof(Vector4));

	PxTransform matCurTransform = CPhysXManager::GetPxTransformFromMatrix(matWorld);
	m_pDynamicRigidActor->setGlobalPose(matCurTransform);

	m_wpOwnerGameObject.lock()->SetTransformMatrix(matWorld);
}

void Engine::CColliderComponent::Turn(const Vector3& _vAxis, float _fAngularSpeed)
{
	if (m_bTrigger) { return; }

	Vector4 vAxis = CMathUtility::DirVec3ToVec4(_vAxis);
	Matrix matRotate = XMMatrixRotationAxis(vAxis, _fAngularSpeed * m_fDeltaSeconds);

	PxTransform matPreTransform = m_pDynamicRigidActor->getGlobalPose();
	Matrix matWorld = CPhysXManager::GetMatrixFromPxTransform(matPreTransform);

	for (int32 iInnerIndex = (int32)ETransformType::RIGHT; iInnerIndex < (int32)ETransformType::POSITION; ++iInnerIndex)
	{
		ETransformType eTransformType = static_cast<ETransformType>(iInnerIndex);

		Vector4 vDirection = Vector4::Zero;
		::memcpy(&vDirection, &matWorld.m[(int32)eTransformType], sizeof(Vector4));

		Vector4 vTransform = XMVector4Transform(vDirection, matRotate);
		::memcpy(&matWorld.m[(int32)eTransformType], &vTransform, sizeof(Vector4));
	}

	PxTransform matCurTransform = CPhysXManager::GetPxTransformFromMatrix(matWorld);
	m_pDynamicRigidActor->setGlobalPose(matCurTransform);

	m_wpOwnerGameObject.lock()->SetTransformMatrix(matWorld);
}

void Engine::CColliderComponent::MoveColliderForward(float _fSpeed)
{
	if (m_bTrigger) { return; }

	PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();
	Matrix matWorld = CPhysXManager::GetMatrixFromPxTransform(matTransform);
	
	Vector3 vDisplacement = -matWorld.Forward() * m_fDeltaSeconds * _fSpeed;
	matTransform.p += PxVec3(vDisplacement.x, vDisplacement.y, vDisplacement.z);
	m_pDynamicRigidActor->setGlobalPose(matTransform);
}

void Engine::CColliderComponent::MoveColliderBackward(float _fSpeed)
{
	if (m_bTrigger) { return; }

	PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();
	Matrix matWorld = CPhysXManager::GetMatrixFromPxTransform(matTransform);

	Vector3 vDisplacement = -matWorld.Backward() * m_fDeltaSeconds * _fSpeed;
	matTransform.p += PxVec3(vDisplacement.x, vDisplacement.y, vDisplacement.z);
	m_pDynamicRigidActor->setGlobalPose(matTransform);
}

void Engine::CColliderComponent::MoveColliderRight(float _fSpeed)
{
	if (m_bTrigger) { return; }

	PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();
	Matrix matWorld = CPhysXManager::GetMatrixFromPxTransform(matTransform);

	Vector3 vDisplacement = matWorld.Right() * m_fDeltaSeconds * _fSpeed;

	matTransform.p += PxVec3(vDisplacement.x, vDisplacement.y, vDisplacement.z);
	m_pDynamicRigidActor->setGlobalPose(matTransform);
}

void Engine::CColliderComponent::MoveColliderLeft(float _fSpeed)
{
	if (m_bTrigger) { return; }

	PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();
	Matrix matWorld = CPhysXManager::GetMatrixFromPxTransform(matTransform);

	Vector3 vDisplacement = matWorld.Left() * m_fDeltaSeconds * _fSpeed;
	matTransform.p += PxVec3(vDisplacement.x, vDisplacement.y, vDisplacement.z);
	m_pDynamicRigidActor->setGlobalPose(matTransform);
}

void Engine::CColliderComponent::MoveColliderUp(float _fSpeed)
{
	if (m_bTrigger) { return; }

	PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();
	Matrix matWorld = CPhysXManager::GetMatrixFromPxTransform(matTransform);

	Vector3 vDisplacement = matWorld.Up() * m_fDeltaSeconds * _fSpeed;
	matTransform.p += PxVec3(vDisplacement.x, vDisplacement.y, vDisplacement.z);
	m_pDynamicRigidActor->setGlobalPose(matTransform);
}

void Engine::CColliderComponent::MoveColliderDown(float _fSpeed)
{
	if (m_bTrigger) { return; }

	PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();
	Matrix matWorld = CPhysXManager::GetMatrixFromPxTransform(matTransform);

	Vector3 vDisplacement = matWorld.Down() * m_fDeltaSeconds * _fSpeed;

	matTransform.p += PxVec3(vDisplacement.x, vDisplacement.y, vDisplacement.z);
	m_pDynamicRigidActor->setGlobalPose(matTransform);
}

void Engine::CColliderComponent::SetPosition(const Vector3& _vPosition)
{
	if (m_bTrigger) { return; }

	PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();

	matTransform.p = PxVec3(_vPosition.x, _vPosition.y, _vPosition.z);
	m_pDynamicRigidActor->setGlobalPose(matTransform);
}

Vector3 Engine::CColliderComponent::GetPosition() const
{
	PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();

	return Vector3(matTransform.p.x, matTransform.p.y, matTransform.p.z);
}

Quaternion Engine::CColliderComponent::GetQuaternion() const
{
	Vector3 vScale = Vector3::One;
	Quaternion vQuaternion = Quaternion::Identity;
	Vector3 vPosition = Vector3::Zero;

	PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();
	Matrix m_matTransform = CPhysXManager::GetMatrixFromPxTransform(matTransform);
	m_matTransform.Decompose(vScale, vQuaternion, vPosition);

	return vQuaternion;
}

HRESULT Engine::CColliderComponent::DeferredAddActorToScene()
{
	if (!m_bDeferredAdd) { return S_OK; }

	if (m_pStaticRigidActor)
	{
		if (!m_pScene->addActor(*m_pStaticRigidActor))
		{
			CHECK(false);
			return E_FAIL;
		}
		else
		{
			return S_OK;
		}
	}

	if (m_pDynamicRigidActor)
	{
		if (!m_pScene->addActor(*m_pDynamicRigidActor))
		{
			CHECK(false);
			return E_FAIL;
		}
		else
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT Engine::CColliderComponent::AddRigidActorToScene()
{
	if (m_pStaticRigidActor)
	{
		if (!m_pScene->addActor(*m_pStaticRigidActor))
		{
			CHECK(false);
			return E_FAIL;
		}
		else
		{
			return S_OK;
		}
	}

	if (m_pDynamicRigidActor)
	{
		if (!m_pScene->addActor(*m_pDynamicRigidActor))
		{
			CHECK(false);
			return E_FAIL;
		}
		else
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT Engine::CColliderComponent::RemoveRigidActorFromScene()
{
	if (m_pStaticRigidActor)
	{
		m_pScene->removeActor(*m_pStaticRigidActor);
		PX_RELEASE(m_pStaticRigidActor);
	}

	if (m_pDynamicRigidActor)
	{
		m_pScene->removeActor(*m_pDynamicRigidActor);
		PX_RELEASE(m_pDynamicRigidActor);
	}

	return S_OK;
}

void Engine::CColliderComponent::AddForce(const Vector3& _vDirection, float _fPower)
{
	if (m_pDynamicRigidActor)
	{
		PxVec3 vForce = PxVec3(_vDirection.x * _fPower, _vDirection.y * _fPower, _vDirection.z * _fPower);
		m_pDynamicRigidActor->addForce(vForce);
	}
}

void Engine::CColliderComponent::SetLinearVelocity(const PxVec3& _vLinearVelocity)
{
	if (m_pDynamicRigidActor)
	{
		m_pDynamicRigidActor->setLinearVelocity(_vLinearVelocity);
	}
}

void Engine::CColliderComponent::SetAngularVelocity(const PxVec3& _vAngularVelocity)
{
	if (m_pDynamicRigidActor)
	{
		m_pDynamicRigidActor->setAngularVelocity(_vAngularVelocity);
	}
}

Vector3 Engine::CColliderComponent::GetLinearVelocity() const
{
	if (m_pDynamicRigidActor)
	{
		PxVec3 pxvLinearVelocity = m_pDynamicRigidActor->getAngularVelocity();

		return Vector3(pxvLinearVelocity.x, pxvLinearVelocity.y, pxvLinearVelocity.z);
	}

	return Vector3(0.f, 0.f, 0.f);
}

Vector3 Engine::CColliderComponent::GetAngularVelocity() const
{
	if (m_pDynamicRigidActor)
	{
		PxVec3 pxvAngularVelocity = m_pDynamicRigidActor->getAngularVelocity();

		return Vector3(pxvAngularVelocity.x, pxvAngularVelocity.y, pxvAngularVelocity.z);
	}

	return Vector3(0.0f, 0.0f, 0.0f);
}

Vector3 Engine::CColliderComponent::GetLocalPosition()
{
	return Vector3(0.0f, 0.0f, 0.0f);
}

void Engine::CColliderComponent::RequestSimulation(bool _bSimulation)
{
	m_bRenderDebug = _bSimulation;
	
	// 시뮬레이션이 변경되었습니다.
	m_bRequestSimulationFlag.store(true);

	m_bRequestedSimulation.store(_bSimulation);

	shared_ptr<CLevel> spLevel = m_wpOwnerGameObject.lock()->GetOwnerLevel();
	spLevel->RequestSimulationColliderComponent(static_pointer_cast<CColliderComponent>(shared_from_this()));
}

void Engine::CColliderComponent::ApplySimulation()
{
	return;
}

void Engine::CColliderComponent::ApplySimulationDirectly(bool _bSimulation)
{
	return;
}

void Engine::CColliderComponent::RequestTransform(std::optional<Vector3> _vTranslate, std::optional<Vector3> _vRotate)
{
	// 위치가 변경되었습니다.
	m_bRequestTransformFlag.store(true);

	if (_vTranslate.has_value())
	{
		m_bRequestedTranslate = _vTranslate.value();
	}
	else
	{
		m_bRequestedTranslate = nullopt;
	}

	if (_vRotate.has_value())
	{
		m_bRequestedRotate = _vRotate.value();
	}
	else
	{
		m_bRequestedRotate = nullopt;
	}

	m_wpOwnerGameObject.lock()->GetOwnerLevel()->RequestTransformColliderComponent(static_pointer_cast<CColliderComponent>(shared_from_this()));
}

void Engine::CColliderComponent::ApplyTransform()
{
	if (m_bRequestTransformFlag.load() == true)
	{
		if (m_pDynamicRigidActor)
		{
			PxTransform matTransform = m_pDynamicRigidActor->getGlobalPose();

			if (m_bRequestedTranslate.has_value())
			{
				Vector3 vRequestedTranslate = m_bRequestedTranslate.value();
				matTransform.p = CPhysXManager::GetPxVec3FromVector3(vRequestedTranslate);
			}

			if (m_bRequestedRotate.has_value())
			{
				Vector3 vRequestedRotate = m_bRequestedRotate.value();
				vRequestedRotate.x = XMConvertToRadians(vRequestedRotate.x);
				vRequestedRotate.y = XMConvertToRadians(vRequestedRotate.y);
				vRequestedRotate.z = XMConvertToRadians(vRequestedRotate.z);

				Quaternion qtX = Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), vRequestedRotate.x);
				Quaternion qtY = Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), vRequestedRotate.y);
				Quaternion qtZ = Quaternion::CreateFromAxisAngle(Vector3(0.0f, 0.0f, 1.0f), vRequestedRotate.z);

				Quaternion qtRequestedRotate = qtZ * qtY * qtX;
				matTransform.q = CPhysXManager::GetPxQuatFromQuaternion(qtRequestedRotate);
			}

			m_pDynamicRigidActor->setGlobalPose(matTransform);
		}

		m_bRequestTransformFlag.store(false);
	}
}

void Engine::CColliderComponent::SetLocalPosition(const Vector3& _vLocalPosition)
{
	return;
}

void Engine::CColliderComponent::SetWorldPosition(const Vector3& _vWorldPosition)
{
	if (m_pDynamicRigidActor)
	{
		PxTransform pxTransform = m_pDynamicRigidActor->getGlobalPose();
		pxTransform.p = PxVec3(_vWorldPosition.x, _vWorldPosition.y, _vWorldPosition.z);

		m_pDynamicRigidActor->setGlobalPose(pxTransform);

		Matrix matWorld = m_wpOwnerGameObject.lock()->GetTransformMatrix().value();

		matWorld.Translation(_vWorldPosition);

		m_wpOwnerGameObject.lock()->SetTransformMatrix(matWorld);
	}
}

void Engine::CColliderComponent::SetUseGravity(bool _bUseGravity)
{
	if (m_pDynamicRigidActor)
	{
		m_pDynamicRigidActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !_bUseGravity);
	}
}

void Engine::CColliderComponent::SetLockFlags(int32 _iAxisLockType)
{
	if (m_pDynamicRigidActor)
	{
		m_pDynamicRigidActor->setRigidDynamicLockFlags(static_cast<PxRigidDynamicLockFlags>(_iAxisLockType));
	}
}

void Engine::CColliderComponent::SetMassLocalPose(Vector3& _vPosition)
{
	if (m_pDynamicRigidActor)
	{
		PxVec3 pxPosition = CPhysXManager::GetPxVec3FromVector3(_vPosition);
		m_pDynamicRigidActor->setCMassLocalPose(PxTransform(pxPosition));
	}
}

void Engine::CColliderComponent::SetPhysxTransformMatrix(const Matrix& _matTransform)
{
	Matrix matTransform = _matTransform;
	PxTransform pxTransform = CPhysXManager::GetPxTransformFromMatrix(matTransform);

	if (m_pDynamicRigidActor)
	{
		m_pDynamicRigidActor->setGlobalPose(pxTransform);
	}
}

void Engine::CColliderComponent::SetCulled(bool _bCulled)
{ 
	CComponent::SetCulled(_bCulled);

	RequestSimulation(!_bCulled);
}

void Engine::CColliderComponent::SetActive(bool _bActive)
{
	CComponent::SetActive(_bActive);

	RequestSimulation(_bActive);
}
