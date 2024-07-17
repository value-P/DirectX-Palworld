#include "pch.h"
#include "FSMNode_Player_Climb.h"

#include "EngineModule.h"
#include "PhysXManager.h"

#include "Player.h"
#include "FSMComponent.h"

shared_ptr<CFSMNode_Player_Climb> Client::CFSMNode_Player_Climb::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	shared_ptr<CFSMNode_Player_Climb> spInstance = make_shared<CFSMNode_Player_Climb>();
	CHECKF(spInstance, L"CFSMNode_Player_Climb::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spPlayer);

	return spInstance;
}

void Client::CFSMNode_Player_Climb::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	CFSMNode_Player::Initialize(_spMeshComponent, _spPlayer);

	m_wpPhysXManager = CEngineModule::GetInstance()->GetPhysXManager();
}

void Client::CFSMNode_Player_Climb::OnEnterState()
{
	CFSMNode_Player::OnEnterState();

	m_wpPlayer.lock()->ChangeControlMode(EPlayerControllerMode::CLIMB_MODE);

	m_wpPlayer.lock()->ActiveCurrentWeapon(false);
}

void Client::CFSMNode_Player_Climb::OnState(float _fDeltaSeconds)
{
	CFSMNode_Player::OnState(_fDeltaSeconds);

	ComputePlayerWithMapNoraml();
	ComputeSound(_fDeltaSeconds);

	if (IsStaminaEmptied())
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::JUMP);
		return;
	}
	else if (IsMoving())
	{
		m_fDeclineStaminaChecker += _fDeltaSeconds;
		if (m_fDeclineStaminaChecker > 1.f)
		{
			m_fDeclineStaminaChecker = 0.f;
			m_wpPlayer.lock()->DeclineStamina();
		}
	}

	if (m_bOnLedge)
	{
		if (m_tDownAnimationStateData.m_iCurrentAnimID == (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_ClimpAtTop)
		{
			if (IsDownAnimLoopEnd())
			{
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);

				m_bOnLedge = false;
			}
			else
			{
				Vector3 vPos = m_wpPlayer.lock()->GetPosition().value();
				Vector3 vLook = m_wpPlayer.lock()->GetForwardVector().value();
				Vector3 vDstLook = vLook;

				vDstLook.y = 0.f;
				vDstLook.Normalize();

				vDstLook = Vector3::Lerp(vLook, vDstLook, m_tDownAnimationStateData.m_fCurrentKeyFrameRate);

				m_wpPlayer.lock()->LookAt(vPos + vDstLook);
			}
		}

		return;
	}

	if (m_tOwnerPlayerStateData.m_bJumping)
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::JUMP);
	}
}

void Client::CFSMNode_Player_Climb::OnExitState()
{
	m_wpPlayer.lock()->ChangeControlMode(EPlayerControllerMode::DEFAULT_MODE);

	Vector3 vPos = m_wpPlayer.lock()->GetPosition().value();
	Vector3 vLook = m_wpPlayer.lock()->GetForwardVector().value();

	vLook.y = 0.f;
	vLook.Normalize();

	m_bOnLedge = false;

	m_wpPlayer.lock()->LookAt(vPos + vLook);

	m_wpPlayer.lock()->ActiveCurrentWeapon(true);

	m_fDeclineStaminaChecker = 0.f;
}

void Client::CFSMNode_Player_Climb::KeyInput()
{
}

void Client::CFSMNode_Player_Climb::ComputePlayerWithMapNoraml()
{
	PxRaycastHit tRaycastHit;
	Vector3 vPos = m_wpPlayer.lock()->GetPosition().value();
	Vector3 vLook = m_wpPlayer.lock()->GetForwardVector().value();
	Vector3 vUp = m_wpPlayer.lock()->GetUpVector().value();
	int32 iCollisionFlag = (int32)ECollisionLayer::BUILDING | (int32)ECollisionLayer::ENVIRONMENT | (int32)ECollisionLayer::ENVIRONMENT2;

	if (m_wpPhysXManager.lock()->SingleRayCast(vPos, Vector3(0.f,-1.f,0.f), 0.15f, iCollisionFlag, &tRaycastHit))
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
		return;
	}

	Vector3 vStraightLook = vLook;
	vStraightLook.y = 0.f;
	vStraightLook.Normalize();

	if (!m_wpPhysXManager.lock()->SingleRayCast(vPos + vUp * 1.2f, vStraightLook, 1.3f, iCollisionFlag, &tRaycastHit))
	{
		m_bOnLedge = true;
		m_wpPlayer.lock()->ActiveLedgeMode();
		return;
	}

	if (m_wpPhysXManager.lock()->SingleRayCast(vPos + vUp * 0.35f, vLook, 0.8f, iCollisionFlag, &tRaycastHit))
	{
		Vector3 vNormal = CPhysXManager::GetVector3FromPxVec3(tRaycastHit.normal);

		Vector3 vDstLook = Vector3::Lerp(vLook, -vNormal, 0.2f);
		vDstLook.Normalize();

		m_wpPlayer.lock()->LookAt(vPos + vDstLook);
	}
}

void Client::CFSMNode_Player_Climb::ComputeSound(float _fDeltaSeconds)
{
	m_fSoundTimeChecker += _fDeltaSeconds;

	if (m_fSoundTimeChecker > 0.7f)
	{
		m_fSoundTimeChecker = 0.f;
		NodePlaySound("Climb_01_Cloth_A_218189544");
	}
}

void Client::CFSMNode_Player_Climb::ExpectNextAnim(FAnimData& _tDownAnimData, FAnimData& _tUpperAnimData)
{
	Vector3 vMoveDirection = m_tOwnerPlayerStateData.m_vMoveDirection;

	if (m_bOnLedge)
	{
		_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_ClimpAtTop;
		_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_ClimpAtTop;
	}
	else
	{
		if (vMoveDirection.x > 0.1f)
		{
			_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_Right;
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_Right;
		}
		else if (vMoveDirection.x < -0.1f)
		{
			_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_Left;
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_Left;
		}
		else if (vMoveDirection.z > 0.1f)
		{
			_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_Up;
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_Up;
		}
		else if (vMoveDirection.z < -0.1f)
		{
			_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_Down;
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_Down;
		}
		else
		{
			_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_Idle;
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Climb_Idle;
		}
	}
}
