#include "pch.h"
#include "RidingObject.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "ClientModule.h"
#include "CameraManager.h"
#include "EffectManager.h"
#include "PhysXManager.h"

#include "PhysXManager.h"
#include "MathUtility.h"
#include "FSMComponent.h"
#include "BoxComponent.h"
#include "SphereComponent.h"
#include "MeshComponent.h"
#include "Player.h"
#include "Model.h"

HRESULT Client::CRidingObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CGameObject::Initialize(_spGameObjectDesc);

	shared_ptr<FRidingObjectDesc> spDesc = static_pointer_cast<FRidingObjectDesc>(_spGameObjectDesc);

	m_tRidingObjectData.m_fJumpPower = spDesc->m_fJumpPower;
	m_tRidingObjectData.m_fMoveSpeed = spDesc->m_fMoveSpeed;

	m_vRidingOffset = spDesc->m_vRidingOffset;

	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();
	m_wpEffectManager = CEffectManager::GetInstance();
	m_wpPhysXManager = CEngineModule::GetInstance()->GetPhysXManager();

	InitMeshComponent();
	InitColliderComponent();
	InitFSMComponent();
	InitRidingBone();

	return S_OK;
}

HRESULT Client::CRidingObject::BeginPlay()
{
	CGameObject::BeginPlay();

	return S_OK;
}

int32 Client::CRidingObject::PreTick(float _fDeltaSeconds)
{	
	CGameObject::PreTick(_fDeltaSeconds);

	KeyInput();

	return 0;
}

int32 Client::CRidingObject::Tick(float _fDeltaSeconds)
{
	CGameObject::Tick(_fDeltaSeconds);

	ComputeRotation(_fDeltaSeconds);

	Move(_fDeltaSeconds);

	return 0;
}

int32 Client::CRidingObject::PostTick(float _fDeltaSeconds)
{
	CGameObject::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CRidingObject::EndPlay()
{
	CGameObject::EndPlay();

	return S_OK;
}

void Client::CRidingObject::Release()
{
	SharedNull(m_spFSMComponent);
	SharedNull(m_spSphereCollider);
	SharedNull(m_spMeshComponent);
	SharedNull(m_spBoxTrigger_ForEnv);
	SharedNull(m_spBoxTrigger_ForEnv2);

	CGameObject::Release();
}

void Client::CRidingObject::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 컨트롤러 컴포넌트
	if (_pSrcComponent->GetComponentType() == EComponentType::CONTROLLER) { return; }

	CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	// 공중상태 판별
	if (pSrcColliderComponent == m_spBoxTrigger_ForEnv.get())
	{
		if (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT ||
			pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BUILDING)
		{
			m_bEnv1Entered = true;
			m_tRidingObjectData.m_bOnGround = true;
		}
	}
	else if (pSrcColliderComponent == m_spBoxTrigger_ForEnv2.get())
	{
		if (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT2 ||
			pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BUILDING)
		{
 			m_bEnv2Entered = true;
			m_tRidingObjectData.m_bOnGround = true;
		}
	}
}

void Client::CRidingObject::OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 컨트롤러 컴포넌트
	if (_pSrcComponent->GetComponentType() == EComponentType::CONTROLLER) { return; }

	CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	// 공중상태 판별
	if (pSrcColliderComponent == m_spBoxTrigger_ForEnv.get())
	{
		if (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT ||
			pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BUILDING)
		{
			m_bEnv1Entered = false;

			if(!m_bEnv2Entered) { m_tRidingObjectData.m_bOnGround = false; }
		}
	}
	else if (pSrcColliderComponent == m_spBoxTrigger_ForEnv2.get())
	{
		if (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT2 ||
			pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BUILDING)
		{
			m_bEnv2Entered = false;
			if (!m_bEnv1Entered) { m_tRidingObjectData.m_bOnGround = false; }
		}
	}
}

shared_ptr<CColliderComponent> Client::CRidingObject::GetBodyColliderComponent()
{
	return m_spSphereCollider;
}

void Client::CRidingObject::SetState(ERidingObjectState _eState)
{
	m_spFSMComponent->ChangeState((int32)_eState);
}

void Client::CRidingObject::SetPosition(const Vector3& _vPos)
{
	if (m_spSphereCollider)
	{
		m_spSphereCollider->SetPosition(_vPos);
	}
}

const std::optional<Vector3> Client::CRidingObject::GetPosition() const
{
	if (m_spSphereCollider)
	{
		return m_spSphereCollider->GetPosition();
	}

	return nullopt;
}

void Client::CRidingObject::InitFSMComponent()
{
	// component desc
	FFSMComponentDesc tFSMComponentDesc;
	tFSMComponentDesc.m_wstrComponentName = L"FSMComponent";
	tFSMComponentDesc.m_eComponentType = EComponentType::FSM;
	tFSMComponentDesc.m_wpOwnerGameObject = shared_from_this();

	// FSM component desc
	tFSMComponentDesc.m_iNumState = (int32)ERidingObjectState::ENUM_END;

	m_spFSMComponent = CFSMComponent::Create(make_shared<FFSMComponentDesc>(tFSMComponentDesc));
	AddComponent(tFSMComponentDesc.m_wstrComponentName, m_spFSMComponent);
}

void Client::CRidingObject::KeyInput()
{
	if (m_wpRider.expired()) { return; }

	if (m_wpInputManager.lock()->IsKeyPressed(DIK_W))
	{
		m_tRidingObjectData.m_vMoveDirection.z = 1.f;
	}
	else if (m_wpInputManager.lock()->IsKeyPressed(DIK_S))
	{
		m_tRidingObjectData.m_vMoveDirection.z = -1.f;
	}

	if (m_wpInputManager.lock()->IsKeyPressed(DIK_D))
	{
		m_tRidingObjectData.m_vMoveDirection.x = 1.f;
	}
	else if (m_wpInputManager.lock()->IsKeyPressed(DIK_A))
	{
		m_tRidingObjectData.m_vMoveDirection.x = -1.f;
	}

	if (m_wpInputManager.lock()->IsKeyUp(DIK_W) || m_wpInputManager.lock()->IsKeyUp(DIK_S))
	{
		m_tRidingObjectData.m_vMoveDirection.z = 0.f;
	}
	if (m_wpInputManager.lock()->IsKeyUp(DIK_A) || m_wpInputManager.lock()->IsKeyUp(DIK_D))
	{
		m_tRidingObjectData.m_vMoveDirection.x = 0.f;
	}

	m_tRidingObjectData.m_vMoveDirection.Normalize();
}

Client::CRidingObject::FRindingStateData Client::CRidingObject::GetObjectData()
{
	m_tRidingObjectData.m_vLinearVelocity = m_spSphereCollider->GetLinearVelocity();

	return m_tRidingObjectData;
}

void Client::CRidingObject::SetRideMode(shared_ptr<CPlayer> _spPlayer)
{
	if (!_spPlayer)
	{
		if (!m_wpRider.expired()) { m_wpRider.reset(); }

		m_tRidingObjectData.m_vMoveDirection = Vector3(0.f);
	}
	else
	{
		m_wpRider = _spPlayer;
	}
}

void Client::CRidingObject::SetRiderCamera(bool _bSprint)
{
	if (!m_wpRider.expired())
	{
		m_wpRider.lock()->RidingSprint(_bSprint);
	}
}

void Client::CRidingObject::ComputeRotation(float _fDeltaSeconds)
{
	if (m_wpRider.expired()) { return; }

	if (m_tRidingObjectData.m_vMoveDirection.Length() > 0.1f)
	{
		Vector3 vMoveDirection = m_tRidingObjectData.m_vMoveDirection;
		Vector3 vCamLook = m_wpCameraManager.lock()->GetCurCameraWorldForwardVector();
		Vector3 vCamRight = m_wpCameraManager.lock()->GetCurCameraWorldRightVector();

		m_vDestLook = vCamRight * vMoveDirection.x + vCamLook * vMoveDirection.z;
		m_vDestLook.y = 0.0f;
		m_vDestLook.Normalize();

		Vector3 vCurrentLook = GetForwardVectorWithoutY().value();
		Vector3 vDestLook = Vector3::Lerp(vCurrentLook, m_vDestLook, 0.35f);
		Vector3 vCurrentPos = GetPosition().value();

		LookAt(vCurrentPos + vDestLook);
	}
}

void Client::CRidingObject::Move(float _fDeltaSeconds)
{
	if (m_wpRider.expired()) { return; }

	if (m_tRidingObjectData.m_vMoveDirection.Length() > 0.1f)
	{	
		Vector3 vMoveHorizontal = m_tRidingObjectData.m_vMoveDirection;
		vMoveHorizontal.y = 0.f;

		if (vMoveHorizontal.Length() > 0.1f)
		{
			m_spSphereCollider->MoveColliderForward(m_tRidingObjectData.m_fMoveSpeed);
		}

		if (m_tRidingObjectData.m_vMoveDirection.y > 0.f)
		{
			m_spSphereCollider->MoveColliderUp(15.f);
		}
		else if (m_tRidingObjectData.m_vMoveDirection.y < 0.f)
		{
			m_spSphereCollider->MoveColliderUp(-15.f);
		}
	}
}

void Client::CRidingObject::LookAt(const Vector3& _vPosition)
{
	if (!m_spSphereCollider) { return; }

	m_spSphereCollider->LookAt(_vPosition);
}
