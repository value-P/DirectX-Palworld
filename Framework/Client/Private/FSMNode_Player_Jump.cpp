#include "pch.h"
#include "FSMNode_Player_Jump.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "PhysXManager.h"

#include "Model.h"
#include "FSMComponent.h"
#include "ColliderComponent.h"

#include "Player.h"

shared_ptr<CFSMNode_Player_Jump> Client::CFSMNode_Player_Jump::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	shared_ptr<CFSMNode_Player_Jump> spInstance = make_shared<CFSMNode_Player_Jump>();
	CHECKF(spInstance, L"CFSMNode_Player_Jump::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spPlayer);

	return spInstance;
}

void Client::CFSMNode_Player_Jump::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	CFSMNode_Player::Initialize(_spMeshComponent, _spPlayer);
}

void Client::CFSMNode_Player_Jump::OnEnterState()
{
	NodePlaySound("AKE_Player_Jump_Sand_2_191008601");

	CFSMNode_Player::OnEnterState();
}

void Client::CFSMNode_Player_Jump::OnState(float _fDeltaSeconds)
{
	CFSMNode_Player::OnState(_fDeltaSeconds);

	if (m_bGliding)
	{
		if (IsStaminaEmptied())
		{
			m_bGliding = false;
			m_wpPlayer.lock()->ActiveGlider(false);
			m_wpPlayer.lock()->SetTerminalVelocityAsDefault();
		}
		else
		{
			m_fGlidingTimeChecker += _fDeltaSeconds;
			m_fDeclineStaminaChecker += _fDeltaSeconds;
			if (m_fDeclineStaminaChecker > 1.f)
			{
				m_fDeclineStaminaChecker = 0.f;
				m_wpPlayer.lock()->DeclineStamina();
			}
		}
	}
	else
	{
		m_fDeclineStaminaChecker = 0.f;
		m_fNoneGlidingTimeChecker += _fDeltaSeconds;

		m_wpPlayer.lock()->ComputeStamina(_fDeltaSeconds);
	}

	if (m_bJumpEnd)
	{
		if (IsMoving())
		{ 
			m_wpPlayer.lock()->SetStandType(CPlayer::EStandType::STAND);
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
		}
		else if (IsDownAnimLoopEnd())
		{
			m_wpPlayer.lock()->SetStandType(CPlayer::EStandType::STAND);
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
		}
	}

	ClimbingCheck();
}

void Client::CFSMNode_Player_Jump::OnExitState()
{
	m_bJumpStartEnd = false;
	m_bJumpEnd = false;
	m_wpPlayer.lock()->SetCanJump(true);
	m_wpPlayer.lock()->SetTerminalVelocityAsDefault();
	m_wpPlayer.lock()->SetSpeed(3.f);

	m_fGlidingTimeChecker = 0.f;
	m_fNoneGlidingTimeChecker = 0.8f;
	m_fDeclineStaminaChecker = 0.f;

	NodePlaySound("AKE_Player_Landing_Sand_2_92656123");
}

void Client::CFSMNode_Player_Jump::KeyInput()
{
	if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_SPACE))
	{
		if (m_bGliding)
		{
			if (m_fGlidingTimeChecker < 1.5f) { return; }

			m_wpPlayer.lock()->SetSpeed(3.f);
			m_wpPlayer.lock()->ActiveGlider(false);
			m_wpPlayer.lock()->SetTerminalVelocityAsDefault();

			m_bGliding = false;
			m_fGlidingTimeChecker = 0.f;
		}
		else if(!IsStaminaEmptied() && m_wpPlayer.lock()->IsGliderEquipped())
		{
			if (m_fNoneGlidingTimeChecker < 0.8f) { return; }

			m_wpPlayer.lock()->SetSpeed(5.f);
			m_wpPlayer.lock()->SetTerminalVelocity(3.f);

			m_bGliding = true;
			m_fNoneGlidingTimeChecker = 0.f;
		}
	}
}

void Client::CFSMNode_Player_Jump::ClimbingCheck()
{
	if (IsStaminaEmptied()) { return; }

	Vector3 vPos = m_wpPlayer.lock()->GetPosition().value();
	Vector3 vLook = m_wpPlayer.lock()->GetForwardVector().value();

	PxRaycastHit tRayHit;

	int32 iCollisionFlag = (int32)ECollisionLayer::ENVIRONMENT |(int32)ECollisionLayer::ENVIRONMENT2 | (int32)ECollisionLayer::BUILDING;

	if (IsMoving() &&
		CEngineModule::GetInstance()->GetPhysXManager()->SingleRayCast(vPos + Vector3(0.f, 0.5f, 0.f), vLook, 0.8f, iCollisionFlag, &tRayHit))
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::CLIMB);
		
		if (m_bGliding)
		{
			m_wpPlayer.lock()->ActiveGlider(false);
			m_bGliding = false;
		}
	}
}

void Client::CFSMNode_Player_Jump::ExpectNextDownAnim(FAnimData& _tDownAnimData)
{
	if (m_bGliding)
	{
		if (m_tOwnerPlayerStateData.m_eVerticalMoveType == EVerticalMovementType::ON_GROUND)
		{
			_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_None;
			m_bGliding = false;
			m_wpPlayer.lock()->ActiveGlider(false);
		}
		else if (m_tDownAnimData.m_iAnimID == (int32)EPLAYER_FEMALE::AS_Player_Female_TwoHandGlide_Start)
		{
			if (IsDownAnimLoopEnd())
			{
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_TwoHandGlide_Idle;
			}
			else if (m_tDownAnimationStateData.m_fCurrentKeyFrameRate > 0.35f)
			{
				m_wpPlayer.lock()->ActiveGlider(true);
			}
		}
		else if(m_tDownAnimData.m_iAnimID == (int32)EPLAYER_FEMALE::AS_Player_Female_TwoHandGlide_Idle)
		{
			_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_TwoHandGlide_Idle;
		}
		else
		{
			_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_TwoHandGlide_Start;
		}
	}
	else
	{
		switch (m_tOwnerPlayerStateData.m_eVerticalMoveType)
		{
		case Engine::EVerticalMovementType::ON_GROUND:
		{
			m_bJumpEnd = true;

			switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
			{
			case Client::EWeaponActionType::NONE:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_None;
				break;
			case Client::EWeaponActionType::BOW:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_Bow;
				break;
			case Client::EWeaponActionType::ROCKET_LAUNCHER:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_RocketLauncher;
				break;
			case Client::EWeaponActionType::PROJECTILE:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_Throw;
				break;
			case Client::EWeaponActionType::ASSAULT_RIFLE:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_AssaultRifle;
				break;
			default:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_Melee;
				break;
			}
		}
		break;
		case Engine::EVerticalMovementType::DOWN:
		{
			_tDownAnimData.m_fLerpTime = 0.5f;

			switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
			{
			case Client::EWeaponActionType::NONE:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_None;
				break;
			case Client::EWeaponActionType::BOW:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_Bow;
				break;
			case Client::EWeaponActionType::ROCKET_LAUNCHER:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_RocketLauncher;
				break;
			case Client::EWeaponActionType::PROJECTILE:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_Throw;
				break;
			case Client::EWeaponActionType::ASSAULT_RIFLE:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_AssaultRifle;
				break;
			default:
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_Melee;
				break;
			}
		}
		break;
		default:
		{
			if (!m_bJumpStartEnd && m_tDownAnimData.m_iAnimID == m_tDownAnimationStateData.m_iCurrentAnimID &&
				m_tUpperAnimationStateData.m_fCurrentKeyFrameRate > 0.25f)
			{
				m_bJumpStartEnd = true;
			}

			if (m_bJumpStartEnd)
			{
				switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
				{
				case Client::EWeaponActionType::NONE:
					_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_None;
					break;
				case Client::EWeaponActionType::BOW:
					_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_Bow;
					break;
				case Client::EWeaponActionType::ROCKET_LAUNCHER:
					_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_RocketLauncher;
					break;
				case Client::EWeaponActionType::PROJECTILE:
					_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_Throw;
					break;
				case Client::EWeaponActionType::ASSAULT_RIFLE:
					_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_AssaultRifle;
					break;
				default:
					_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_Melee;
					break;
				}
			}
			else
			{
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpStart_None;
			}
		}
		break;
		}
	}
}

void Client::CFSMNode_Player_Jump::ExpectNextUpperAnim(FAnimData& _tUpperAnimData)
{
	if (m_bGliding)
	{
		_tUpperAnimData.m_iAnimID = m_tDownAnimData.m_iAnimID;
	}
	else
	{	
		switch (m_tOwnerPlayerStateData.m_eVerticalMoveType)
		{
		case Engine::EVerticalMovementType::ON_GROUND:
		{
			switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
			{
			case Client::EWeaponActionType::NONE:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_None;
				break;
			case Client::EWeaponActionType::BOW:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_Bow;
				break;
			case Client::EWeaponActionType::ROCKET_LAUNCHER:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_RocketLauncher;
				break;
			case Client::EWeaponActionType::PROJECTILE:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_Throw;
				break;
			case Client::EWeaponActionType::ASSAULT_RIFLE:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_AssaultRifle;
				break;
			default:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpEnd_Melee;
				break;
			}
		}
		break;
		case Engine::EVerticalMovementType::DOWN:
		{
			_tUpperAnimData.m_fLerpTime = 0.5f;

			switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
			{
			case Client::EWeaponActionType::NONE:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_None;
				break;
			case Client::EWeaponActionType::BOW:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_Bow;
				break;
			case Client::EWeaponActionType::ROCKET_LAUNCHER:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_RocketLauncher;
				break;
			case Client::EWeaponActionType::PROJECTILE:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_Throw;
				break;
			case Client::EWeaponActionType::ASSAULT_RIFLE:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_AssaultRifle;
				break;
			default:
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpDownLoop_Melee;
				break;
			}
		}
		break;
		default:
		{
			if (m_bJumpStartEnd)
			{
				switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
				{
				case Client::EWeaponActionType::NONE:
					_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_None;
					break;
				case Client::EWeaponActionType::BOW:
					_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_Bow;
					break;
				case Client::EWeaponActionType::ROCKET_LAUNCHER:
					_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_RocketLauncher;
					break;
				case Client::EWeaponActionType::PROJECTILE:
					_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_Throw;
					break;
				case Client::EWeaponActionType::ASSAULT_RIFLE:
					_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_AssaultRifle;
					break;
				default:
					_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpUpLoop_Melee;
					break;
				}

			}
			else
			{
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JumpStart_None;
			}
		}
		break;
		}	
	}
}