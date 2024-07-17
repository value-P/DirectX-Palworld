#include "pch.h"
#include "ControllerComponent.h"

#include "EngineModule.h"
#include "PhysXManager.h"
#include "InputManager.h"

#include "Character.h"
#include "CameraComponent.h"
#include "ColliderComponent.h"

std::shared_ptr<CControllerComponent> Engine::CControllerComponent::Create(const std::shared_ptr<FControllerComponentDesc>& _spControllerComponentDesc)
{
	const std::shared_ptr<CControllerComponent> spControllerComponent = make_shared<CControllerComponent>();
	ENSUREF(spControllerComponent->InitializeComponent(_spControllerComponentDesc), L"Failed to initialize player controller.");
	return spControllerComponent;
}

HRESULT Engine::CControllerComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component desc
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// controller component desc
	shared_ptr<FControllerComponentDesc> spControllerComponentDesc = static_pointer_cast<FControllerComponentDesc>(_spComponentDesc);
	m_eControllerShapeType	= spControllerComponentDesc->m_eControllerShapeType;
	m_fSlopeLimit			= spControllerComponentDesc->m_fSlopeLimit;
	m_fStepOffset			= spControllerComponentDesc->m_fStepOffset;
	m_fContactOffset		= spControllerComponentDesc->m_fContactOffset;
	m_eControllerLayer		= spControllerComponentDesc->m_eControllerLayer;
	m_eControllerFlag		= spControllerComponentDesc->m_eControllerFlag;

	m_vLocalPosition		= spControllerComponentDesc->m_vLocalPosition;
	m_vLocalRotate			= spControllerComponentDesc->m_vLocalRotate;

	float fStaticFriction	= spControllerComponentDesc->m_vMaterials.x;
	float fDynamicFriction	= spControllerComponentDesc->m_vMaterials.y;
	float fRestitution		= spControllerComponentDesc->m_vMaterials.z;

	// etc
	bool bUserBehaviorCallback = false;
	if (spControllerComponentDesc->m_pControllerComponentBehaviorCallback != nullptr)
	{
		spControllerComponentDesc->m_pControllerComponentBehaviorCallback->m_spGameObject = m_wpOwnerGameObject.lock();
		spControllerComponentDesc->m_pControllerComponentBehaviorCallback->m_spControllerComponent = dynamic_pointer_cast<CControllerComponent>(shared_from_this());
		bUserBehaviorCallback = true;
	}
	else
	{
		m_oBehaviorCallback.m_spGameObject = m_wpOwnerGameObject.lock();
		m_oBehaviorCallback.m_spControllerComponent = dynamic_pointer_cast<CControllerComponent>(shared_from_this());
	}

	bool bUserHitReport = false;
	if (spControllerComponentDesc->m_pControllerComponentHitReport != nullptr)
	{
		spControllerComponentDesc->m_pControllerComponentHitReport->m_spGameObject = m_wpOwnerGameObject.lock();
		spControllerComponentDesc->m_pControllerComponentHitReport->m_spControllerComponent = dynamic_pointer_cast<CControllerComponent>(shared_from_this());
		bUserHitReport = true;
	}
	else
	{
		m_oHitReportCallback.m_spGameObject = m_wpOwnerGameObject.lock();
		m_oHitReportCallback.m_spControllerComponent = dynamic_pointer_cast<CControllerComponent>(shared_from_this());
	}
	
	m_wpInputManager		= CEngineModule::GetInstance()->GetInputManager();
	m_wpPhysXManager		= CEngineModule::GetInstance()->GetPhysXManager();

	m_pPhysics				= m_wpPhysXManager.lock()->GetPhysics();
	m_pScene				= m_wpPhysXManager.lock()->GetScene();
	m_pControllerManager	= m_wpPhysXManager.lock()->GetControllerManager();

	m_pMaterial = m_pPhysics->createMaterial(fStaticFriction, fDynamicFriction, fRestitution);

	switch (spControllerComponentDesc->m_eControllerShapeType)
	{
	case EControllerShapeType::CAPSULE:
	{
		PxCapsuleControllerDesc oCapsuleControllerDesc;
		oCapsuleControllerDesc.height			= spControllerComponentDesc->m_fCapsuleHalfHeight;
		oCapsuleControllerDesc.radius			= spControllerComponentDesc->m_fCapsuleRadius;
		oCapsuleControllerDesc.material			= m_pMaterial;

		if (bUserBehaviorCallback)
		{
			oCapsuleControllerDesc.behaviorCallback = spControllerComponentDesc->m_pControllerComponentBehaviorCallback;
			oCapsuleControllerDesc.reportCallback = spControllerComponentDesc->m_pControllerComponentHitReport;
		}
		else
		{
			oCapsuleControllerDesc.behaviorCallback = &m_oBehaviorCallback;
			oCapsuleControllerDesc.reportCallback = &m_oHitReportCallback;
		}

		oCapsuleControllerDesc.climbingMode		= PxCapsuleClimbingMode::eCONSTRAINED;
		oCapsuleControllerDesc.stepOffset		= m_fStepOffset;
		oCapsuleControllerDesc.contactOffset	= m_fContactOffset;
		oCapsuleControllerDesc.nonWalkableMode	= PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
		oCapsuleControllerDesc.slopeLimit		= cosf(XMConvertToRadians(m_fSlopeLimit));

		m_pController = m_pControllerManager->createController(oCapsuleControllerDesc);
	}
	break;

	case EControllerShapeType::BOX:
	{
		PxBoxControllerDesc oBoxControllerDesc;

		oBoxControllerDesc.halfForwardExtent	= spControllerComponentDesc->m_vExtents.z;
		oBoxControllerDesc.halfHeight			= spControllerComponentDesc->m_vExtents.y;
		oBoxControllerDesc.halfSideExtent		= spControllerComponentDesc->m_vExtents.x;
		oBoxControllerDesc.material				= m_pMaterial;

		if (bUserBehaviorCallback)
		{
			oBoxControllerDesc.behaviorCallback = spControllerComponentDesc->m_pControllerComponentBehaviorCallback;
			oBoxControllerDesc.reportCallback = spControllerComponentDesc->m_pControllerComponentHitReport;
		}
		else
		{
			oBoxControllerDesc.behaviorCallback = &m_oBehaviorCallback;
			oBoxControllerDesc.reportCallback = &m_oHitReportCallback;
		}

		oBoxControllerDesc.stepOffset			= m_fStepOffset;
		oBoxControllerDesc.contactOffset		= m_fContactOffset;
		oBoxControllerDesc.nonWalkableMode		= PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
		oBoxControllerDesc.contactOffset		= spControllerComponentDesc->m_fContactOffset;

		m_pController = m_pControllerManager->createController(oBoxControllerDesc);
	}
	break;

	default:
		CHECKF(false, L"Player controller shape type is not designated.");
		break;
	}

	// 사용자 데이터를 설정합니다.
	m_pController->getActor()->userData = this;

	// 로컬 좌표계를 변경합니다.
	Matrix matLocal = Matrix::CreateRotationX(XMConvertToRadians(m_vLocalRotate.x));
	matLocal *= Matrix::CreateRotationY(XMConvertToRadians(m_vLocalRotate.y));
	matLocal *= Matrix::CreateRotationZ(XMConvertToRadians(m_vLocalRotate.z));
	matLocal *= Matrix::CreateTranslation(m_vLocalPosition);
	m_pController->getActor()->getShapes(&m_pShape, 1, 0);

	PxTransform matLocalPose = CPhysXManager::GetPxTransformFromMatrix(matLocal);
	// m_pShape->setLocalPose(matLocalPose);

	// 콜리전 레이어와 플래그를 설정합니다.
	if (m_eControllerLayer != ECollisionLayer::ENUM_END && m_eControllerFlag != ECollisionFlag::ENUM_END)
	{
		const PxFilterData tTriggerFilterData((PxU32)m_eControllerLayer, (PxU32)m_eControllerFlag, 0xffffffff, 0xffffffff);
		m_pShape->setSimulationFilterData(tTriggerFilterData);
	}

	// 컨트롤러의 초기 위치를 설정합니다.
	Vector3 vPosition = m_wpOwnerGameObject.lock()->GetPosition().value();
	m_pController->setPosition(PxExtendedVec3(vPosition.x, vPosition.y, vPosition.z));
	return S_OK;
}

HRESULT Engine::CControllerComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CControllerComponent::PreTickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CControllerComponent::TickComponent(float _fDeltaSeconds)
{
	/*
	PxExtendedVec3 v3 = m_pController->getFootPosition();
	cerr << v3.x << " " << v3.y << " " << v3.z << endl;
	*/
	return 0;
}

int32 Engine::CControllerComponent::PostTickComponent(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Engine::CControllerComponent::EndPlay()
{
	return S_OK;
}

void Engine::CControllerComponent::Render(int32 _iShaderPath)
{
}

void Engine::CControllerComponent::Release()
{
	if (m_oBehaviorCallback.m_spGameObject) { m_oBehaviorCallback.m_spGameObject = nullptr; }

	if (m_oBehaviorCallback.m_spControllerComponent) { m_oBehaviorCallback.m_spControllerComponent = nullptr; }

	if (m_oHitReportCallback.m_spGameObject) { m_oHitReportCallback.m_spGameObject = nullptr; }

	if (m_oHitReportCallback.m_spControllerComponent) { m_oHitReportCallback.m_spControllerComponent = nullptr; }

	if (m_pControllerManager) { m_pControllerManager = nullptr; }
	
	if (m_pPhysics) { m_pPhysics = nullptr; }
	
	if (m_pShape) { m_pShape = nullptr; }

	m_pController->getActor()->userData = nullptr;
	PX_RELEASE(m_pController);

	PX_RELEASE(m_pMaterial);
}

HRESULT Engine::CControllerComponent::BindVariable()
{
	return S_OK;
}

void Engine::CControllerComponent::UpdateControllerComponentFromScene(float _fDeltaSeconds)
{
	/*
	// 충돌 플래그를 초기화합니다.
	m_oControllerCollisionFlags = PxControllerCollisionFlags(0);

	m_vDirection = { 0.0f, 0.0f, 0.0f };

	if (m_wpInputManager.lock()->IsKeyPressed(DIK_W)) { m_vDirection.z = +1.0f; }

	if (m_wpInputManager.lock()->IsKeyPressed(DIK_S)) { m_vDirection.z = -1.0f; }

	if (m_wpInputManager.lock()->IsKeyPressed(DIK_D)) { m_vDirection.x = +1.0f; }

	if (m_wpInputManager.lock()->IsKeyPressed(DIK_A)) { m_vDirection.x = -1.0f; }

	// 방향 벡터를 정규화합니다.
	m_vDirection.normalize();

	// 속력을 적용합니다.
	m_oControllerCollisionFlags |= m_pController->move(m_fCurMoveSpeed * _fDeltaSeconds * m_vDirection, 0.01f, 0.01f, m_oControllerFilters);

	if (m_bJumping)
	{
		m_fSumJumpSeconds += _fDeltaSeconds;

		float fVerticleVelocity = -0.5f * m_fGravity * m_fJumpVelocity * pow(m_fSumJumpSeconds, 2.f);
		fVerticleVelocity = max(m_fTerminalVelocity, fVerticleVelocity);

		m_oControllerCollisionFlags |= m_pController->move(fVerticleVelocity * _fDeltaSeconds * PxVec3(0.0f, 1.0f, 0.0f), 0.01f, 0.01f, m_oControllerFilters);
	}
	else
	{
		m_fSumJumpSeconds = 0.f;

		m_oControllerCollisionFlags |= m_pController->move(m_fTerminalVelocity * _fDeltaSeconds * PxVec3(0.0f, -1.0f, 0.0f), 0.01f, 0.01f, m_oControllerFilters);
	}

	// 컨트롤러의 이동과 캐릭터의 이동을 동기화합니다.
	PxExtendedVec3 vExPosition = m_pController->getPosition();
	m_wpCharacter.lock()->SetPosition({ (float)vExPosition.x, (float)vExPosition.y, (float)vExPosition.z });
	*/
}

EVerticalMovementType Engine::CControllerComponent::GetVerticalMovementType()
{
	if (m_bAir && (0.0f < m_fCurJumpVelocity))
	{
		return EVerticalMovementType::UP;
	}
	else if (m_bAir)
	{
		return EVerticalMovementType::DOWN;
	}

	return EVerticalMovementType::ON_GROUND;
}

PxControllerBehaviorFlags Engine::CControllerComponentBehaviorCallback::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
	// 비헤이비어 콜백은 충돌하는 중일 때도 계속해서 호출됩니다.
	/*
	PxActorFlags tFlags = actor.getActorFlags();
	const char* szName = actor.getName();

	PxActorType::Enum eActorType = actor.getType();
	void* pUserData = actor.userData;
	*/
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags Engine::CControllerComponentBehaviorCallback::getBehaviorFlags(const PxController& controller)
{
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags Engine::CControllerComponentBehaviorCallback::getBehaviorFlags(const PxObstacle & obstacle)
{
	return PxControllerBehaviorFlags();
}

void Engine::CControllerComponentHitReport::onShapeHit(const PxControllerShapeHit& hit)
{
	// 히트 콜백은 move 함수를 호출해서 캐릭터가 능동적으로 다른 물체에 부딪혔을 때만 호출됩니다.
	// 다른 물체가 캐릭터한테 스스로 부딪힐 때는 호출되지 않습니다.
	// RigidDynamic 액터와 부딪혔을 때 이 콜백이 호출됩니다.
	/*
	CGameObject* pSrcGameObject = m_spGameObject.get();
	CColliderComponent* pDstCollider = static_cast<CColliderComponent*>(hit.actor->userData);
	
	pSrcGameObject->OnCollisionStay(nullptr, pDstCollider);
	*/
}

void Engine::CControllerComponentHitReport::onControllerHit(const PxControllersHit& hit)
{
}

void Engine::CControllerComponentHitReport::onObstacleHit(const PxControllerObstacleHit& hit)
{
}
