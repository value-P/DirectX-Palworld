#include "pch.h"
#include "PlayerControllerComponent.h"

#include "EngineModule.h"
#include "PhysXManager.h"
#include "InputManager.h"
#include "UIManager.h"

#include "Character.h"
#include "CameraComponent.h"
#include "ColliderComponent.h"

std::shared_ptr<CPlayerControllerComponent> Client::CPlayerControllerComponent::Create(const std::shared_ptr<FPlayerControllerComponentDesc>& _spPlayerControllerComponentDesc)
{
	shared_ptr<CPlayerControllerComponent> spPlayerControllerComponent = make_shared<CPlayerControllerComponent>();
	ENSUREF(spPlayerControllerComponent->InitializeComponent(_spPlayerControllerComponentDesc), L"Failed to initialize pal controller.");
	return spPlayerControllerComponent;
}

HRESULT Client::CPlayerControllerComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// controller component desc
	if (FAILED(CControllerComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// player controller desc
	shared_ptr<FPlayerControllerComponentDesc> spPlayerControllerComponentDesc = static_pointer_cast<FPlayerControllerComponentDesc>(_spComponentDesc);
	m_wpPlayerCameraComponent = spPlayerControllerComponentDesc->m_spCameraComponent;	
	m_wpUIManager = CUIManager::GetInstance();

	return S_OK;
}

HRESULT Client::CPlayerControllerComponent::BeginPlay()
{
	CControllerComponent::BeginPlay();

	return S_OK;
}

int32 Client::CPlayerControllerComponent::PreTickComponent(float _fDeltaSeconds)
{
	CControllerComponent::PreTickComponent(_fDeltaSeconds);

#ifdef DEBUG
	if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_GRAVE)) 
	{
		m_bGodMode = !m_bGodMode;
	}
#endif // DEBUG

	return 0;
}

int32 Client::CPlayerControllerComponent::TickComponent(float _fDeltaSeconds)
{
	CControllerComponent::TickComponent(_fDeltaSeconds);

	UpdateControllerComponentFromScene(_fDeltaSeconds);
	return 0;
}

int32 Client::CPlayerControllerComponent::PostTickComponent(float _fDeltaSeconds)
{
	CControllerComponent::PostTickComponent(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CPlayerControllerComponent::EndPlay()
{
	CControllerComponent::EndPlay();

	return S_OK;
}

void Client::CPlayerControllerComponent::Render(int32 _iShaderPath)
{
}

void Client::CPlayerControllerComponent::Release()
{
	CControllerComponent::Release();
}

void Client::CPlayerControllerComponent::ChangeControllerMode(EPlayerControllerMode _eControlMode)
{
	m_eControlMode = _eControlMode;
}

void Client::CPlayerControllerComponent::SetPosition(const Vector3& _vPos)
{
	m_pController->setPosition(PxExtendedVec3(_vPos.x, _vPos.y, _vPos.z));

	PxExtendedVec3 vExFootPosition = m_pController->getFootPosition();
	m_wpOwnerGameObject.lock()->SetPosition({ (float)vExFootPosition.x, (float)vExFootPosition.y, (float)vExFootPosition.z });
}

Vector3 Client::CPlayerControllerComponent::GetPosition() const
{
	PxExtendedVec3 vExFootPosition = m_pController->getFootPosition();

	return { (float)vExFootPosition.x, (float)vExFootPosition.y, (float)vExFootPosition.z };
}

void Client::CPlayerControllerComponent::AddForce(const Vector3& _vDirection, float _fPower)
{
	Vector3 vDir = _vDirection;
	vDir.y = 1.f;

	vDir.Normalize();
	
	m_bAddForce = true;
	m_fForce = _fPower;
	m_vForceDirection = PxVec3(vDir.x, vDir.y, vDir.z);
}

void Client::CPlayerControllerComponent::UpdateControllerComponentFromScene(float _fDeltaSeconds)
{
	// 충돌 플래그를 초기화합니다.
	m_oControllerCollisionFlags = PxControllerCollisionFlags(0);

#ifdef DEBUG
	if (m_bGodMode)
	{
		Vector3 vMove = Vector3(0.f,0.f,0.f);
		if (m_wpInputManager.lock()->IsKeyPressed(DIK_W)) { vMove.z = +1.0f; }

		if (m_wpInputManager.lock()->IsKeyPressed(DIK_S)) { vMove.z = -1.0f; }

		if (m_wpInputManager.lock()->IsKeyPressed(DIK_D)) { vMove.x = +1.0f; }

		if (m_wpInputManager.lock()->IsKeyPressed(DIK_A)) { vMove.x = -1.0f; }

		if (m_wpInputManager.lock()->IsKeyPressed(DIK_Q)) { vMove.y = +0.3f; }

		if (m_wpInputManager.lock()->IsKeyPressed(DIK_E)) { vMove.y = -0.3f; }

		Vector3 vLook = m_wpPlayerCameraComponent.lock()->GetSphericalCameraForwardVector();
		Vector3 vRight = m_wpPlayerCameraComponent.lock()->GetSphericalCameraRightVector();
		vLook.y = 0.0f;
		vRight.y = 0.0f;
		vLook.Normalize();
		vRight.Normalize();

		Vector3 vTotalMove = vLook * vMove.z + vRight * vMove.x + Vector3(0.f, 1.f, 0.f) * vMove.y;

		m_oControllerCollisionFlags |= m_pController->move(5.f * PxVec3(vTotalMove.x, vTotalMove.y, vTotalMove.z), 0.01f, 0.01f, m_oControllerFilters);

		// 컨트롤러의 이동과 캐릭터의 이동을 발 위치로 동기화합니다.
		PxExtendedVec3 vExFootPosition = m_pController->getFootPosition();
		m_wpOwnerGameObject.lock()->SetPosition({ (float)vExFootPosition.x, (float)vExFootPosition.y, (float)vExFootPosition.z });

		return;
	}
#endif // DEBUG

	if (m_wpUIManager.lock()->IsOnUIPanel()) 
	{
		m_vMoveDirection = Vector3(0.f, 0.f, 0.f);

		return;
	}

	// 입력 처리
	{
		m_vMoveDirection = { 0.0f, 0.0f, 0.0f };

		switch (m_eControlMode)
		{
			break;
		case Client::EPlayerControllerMode::STATICMOVE_MODE:
		{
			m_vMoveDirection = m_vFixedMoveDirection;
		}
		break;
		default:
		{
			if (m_wpInputManager.lock()->IsKeyPressed(DIK_W)) { m_vMoveDirection.z = +1.0f; }

			if (m_wpInputManager.lock()->IsKeyPressed(DIK_S)) { m_vMoveDirection.z = -1.0f; }

			if (m_wpInputManager.lock()->IsKeyPressed(DIK_D)) { m_vMoveDirection.x = +1.0f; }

			if (m_wpInputManager.lock()->IsKeyPressed(DIK_A)) { m_vMoveDirection.x = -1.0f; }
		}
		break;
		}

		// 방향 벡터를 정규화합니다.
		m_vMoveDirection.Normalize();

		if (m_bCanJump && m_wpInputManager.lock()->IsKeyDown(DIK_SPACE))
		{
			m_bAir = true;
			m_bJumping = true;
			m_bCanJump = false;

			m_eControlMode = EPlayerControllerMode::DEFAULT_MODE;
		}
	}

	if (m_bAddForce) { m_vMoveDirection = Vector3(0.f, 0.f, 0.f); }

	// 이동
	{
		switch (m_eControlMode)
		{
		case EPlayerControllerMode::NONEMOVE_MODE:
		{

		}
		break;

		case Client::EPlayerControllerMode::DEFAULT_MODE:
		{
			Vector3 vLook = m_wpPlayerCameraComponent.lock()->GetSphericalCameraForwardVector();
			Vector3 vRight = m_wpPlayerCameraComponent.lock()->GetSphericalCameraRightVector();
			vLook.y = 0.0f;
			vRight.y = 0.0f;
			vLook.Normalize();
			vRight.Normalize();

			Vector3 vMoveDirection = vLook * m_vMoveDirection.z + vRight * m_vMoveDirection.x;
			vMoveDirection.Normalize();

			m_oControllerCollisionFlags |= m_pController->move(m_fCurMoveSpeed * _fDeltaSeconds * PxVec3(vMoveDirection.x, vMoveDirection.y, vMoveDirection.z), 0.01f, 0.01f, m_oControllerFilters);
		}
		break;

		case Client::EPlayerControllerMode::STATICMOVE_MODE:
		{
			m_oControllerCollisionFlags |= m_pController->move(m_fCurMoveSpeed * _fDeltaSeconds * PxVec3(m_vMoveDirection.x, m_vMoveDirection.y, m_vMoveDirection.z), 0.01f, 0.01f, m_oControllerFilters);
		}
		break;

		case Client::EPlayerControllerMode::CLIMB_MODE:
		{
			Vector3 vRight = m_wpOwnerGameObject.lock()->GetRightVector().value();
			Vector3 vUp = m_wpOwnerGameObject.lock()->GetUpVector().value();
			Vector3 vLook = m_wpOwnerGameObject.lock()->GetForwardVector().value();
			Vector3 vMoveDirection;

			if (m_bLedgeMode)
			{
				m_fLedgeModeChecker += _fDeltaSeconds;

				if (m_fLedgeModeChecker < 0.4f)
				{
					vMoveDirection = Vector3(0.f,1.f,0.f);
				}
				else if (m_fLedgeModeChecker < m_fLedgeModeTime)
				{
					vMoveDirection = vLook * 1.5f;
				}
				else
				{
					m_fLedgeModeChecker = 0.f;
					m_bLedgeMode = false;
					m_eControlMode = EPlayerControllerMode::DEFAULT_MODE;
				}
			}
			else
			{
				vMoveDirection = vRight * m_vMoveDirection.x + vUp * m_vMoveDirection.z;
				vMoveDirection.Normalize();
			}

			m_oControllerCollisionFlags |= m_pController->move(m_fCurMoveSpeed * _fDeltaSeconds * PxVec3(vMoveDirection.x, vMoveDirection.y, vMoveDirection.z), 0.01f, 0.01f, m_oControllerFilters);
		}
			break;
		}
	}

	// 중력
	if (m_bJumping)
	{ 
		m_fSumJumpSeconds += _fDeltaSeconds;

		m_fCurJumpVelocity = (m_fJumpVelocity) - (0.5f * m_fGravity * pow(m_fSumJumpSeconds, 2.f));
		m_fCurJumpVelocity = max(-m_fTerminalVelocity, m_fCurJumpVelocity);

		m_oControllerCollisionFlags |= m_pController->move(m_fCurJumpVelocity * _fDeltaSeconds * PxVec3(0.0f, 1.0f, 0.0f), 0.01f, 0.01f, m_oControllerFilters);
	}
	else if (m_eControlMode == EPlayerControllerMode::CLIMB_MODE)
	{
		m_fCurJumpVelocity = 0.0f;
		m_fSumJumpSeconds = 0.0f;
	}
	else
	{
		m_fCurJumpVelocity = 0.0f;
		m_fSumJumpSeconds = 0.0f;

		m_oControllerCollisionFlags |= m_pController->move(m_fTerminalVelocity * _fDeltaSeconds * 0.3f * PxVec3(0.0f, -1.0f, 0.0f), 0.01f, 0.01f, m_oControllerFilters);
	}

	// AddForce
	if (m_bAddForce)
	{
		m_fForce = lerp(m_fForce, 0.f, 0.1f);
		m_oControllerCollisionFlags |= m_pController->move(m_vForceDirection * m_fForce * _fDeltaSeconds, 0.01f, 0.01f, m_oControllerFilters);

		if (m_fForce < 0.1f) { m_bAddForce = false; }
	}

	// 컨트롤러의 이동과 캐릭터의 이동을 발 위치로 동기화합니다.
	PxExtendedVec3 vExFootPosition = m_pController->getFootPosition();
	m_wpOwnerGameObject.lock()->SetPosition({ (float)vExFootPosition.x, (float)vExFootPosition.y, (float)vExFootPosition.z });
}
