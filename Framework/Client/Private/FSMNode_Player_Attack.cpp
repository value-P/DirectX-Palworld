#include "pch.h"
#include "FSMNode_Player_Attack.h"

#include "SoundManager.h"
#include "EngineModule.h"
#include "InputManager.h"

#include "MeshComponent.h"
#include "FSMComponent.h"

#include "Player.h"
#include "Model.h"

shared_ptr<CFSMNode_Player_Attack> Client::CFSMNode_Player_Attack::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	shared_ptr<CFSMNode_Player_Attack> spInstance = make_shared<CFSMNode_Player_Attack>();
	CHECKF(spInstance, L"CFSMNode_Player_Attack::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spPlayer);

	return spInstance;
}

void Client::CFSMNode_Player_Attack::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	CFSMNode_Player::Initialize(_spMeshComponent, _spPlayer);
}

void Client::CFSMNode_Player_Attack::OnEnterState()
{
	CFSMNode_Player::OnEnterState();

	m_wpPlayer.lock()->SetUseSpineRotation(true);

	switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
	{
	case EWeaponActionType::NONE:
	case EWeaponActionType::MELEE:
	case EWeaponActionType::SPEAR:
		NodePlaySound("Player_Female_Pickaxe_Marina_EN_04_210177256");
		m_wpPlayer.lock()->ZoomOut();
		break;

	default:
		break;
	}
}

void Client::CFSMNode_Player_Attack::OnState(float _fDeltaSeconds)
{
	CFSMNode_Player::OnState(_fDeltaSeconds);

	if (m_tOwnerPlayerStateData.m_bJumping)
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::JUMP);
	}

	if (IsUpperAnimMatching() && IsUpperAnimLoopEnd() && !IsCurWeaponType(EWeaponActionType::ASSAULT_RIFLE) && !m_bCharging )
	{

		if (IsStaminaEmptied())
		{
			if (IsMoving())
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
			else
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);

			return;
		}

		if (!m_bAttackCall)
		{
			if(IsMoving())
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
			else
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
		}
		else
		{
			m_bAttackCall = false;
		}
	}

	if (m_tUpperAnimData.m_iAnimID == (int32)EPLAYER_FEMALE::AS_Player_Female_Throw)
	{
		if (IsUpperAnimLoopEnd())
		{
			m_wpPlayer.lock()->ChangeWeapon(m_tOwnerPlayerStateData.m_ePrevWeaponID);
		}
		else if (m_tUpperAnimationStateData.m_fCurrentKeyFrameRate > 0.1f && m_wpPlayer.lock()->IsPalSphereActivate())
		{
			// 팰을 소환하기 위해 팰 스피어를 던집니다.
			if (m_tOwnerPlayerStateData.m_eInteractType == CPlayer::EInteractType::SPAWN_PAL)
			{
				m_wpPlayer.lock()->ThrowPartnerPalSphere(15.f);
				m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::ENUM_END);
			}
			// 라이딩을 소환하기 위해
			else if (m_tOwnerPlayerStateData.m_eInteractType == CPlayer::EInteractType::SPAWN_RIDING)
			{
				m_wpPlayer.lock()->ThrowRidingPalSphere(15.f);
				m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::ENUM_END);
			}
			else
			{
				m_wpPlayer.lock()->ThrowPalSphere(25.f);
			}

			// 손에 있는 팰 스피어를 끕니다.
			m_wpPlayer.lock()->ActivePalSphere(false);
		}
	}

	if (m_bReloading)
	{
		if (m_tUpperAnimationStateData.m_iCurrentAnimID == (int32)EPLAYER_FEMALE::AS_Player_Female_Reload_AssaultRifle &&
			IsUpperAnimLoopEnd())
		{
			m_bReloading = false;
			m_wpPlayer.lock()->ReloadBullet();
		}
	}
	else if (m_bFireAssaultRifle)
	{
		m_fShotDelayChecker += _fDeltaSeconds;
		if (m_fShotDelayChecker >= m_fShotDelay)
		{
			m_bCanFireAssaultRifle = true;
			m_fShotDelayChecker = 0.f;
		}
	}

	if (m_tOwnerPlayerStateData.m_eWeaponActionType == EWeaponActionType::ASSAULT_RIFLE && m_wpInputManager.lock()->IsMouseReleased(EMouseActionType::RIGHT_BUTTON))
	{
		m_wpPlayer.lock()->ZoomOut();
	}
}

void Client::CFSMNode_Player_Attack::OnExitState()
{
	m_bShot = false;

	m_wpPlayer.lock()->SetUseSpineRotation(false);
	m_bAttackCall = false;
	m_bReloading = false;
	m_bFireAssaultRifle = false;
	m_fShotDelayChecker = 0.f;
	
	if (m_eARFireSoundChannel != EChannelType::ENUM_END)
	{
		m_wpSoundManager.lock()->StopChannelEx(m_eARFireSoundChannel);
		NodePlaySound("AKE_Weapon_AssultRifle_Fire_FullAuto_Tail_788515356");
		m_eARFireSoundChannel = EChannelType::ENUM_END;
	}
}

void Client::CFSMNode_Player_Attack::KeyInput()
{
	switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
	{
	case EWeaponActionType::BOW:
	{
		if (m_bCharging &&
			m_wpInputManager.lock()->IsMouseReleased(EMouseActionType::LEFT_BUTTON))
		{
			NodePlaySound("AKE_Weapon_Bow_Fire_202708598");
			m_bShot = true;
			m_bAttackCall = false;
			m_wpPlayer.lock()->FireArrow();
		}
	}
	break;

	case EWeaponActionType::ASSAULT_RIFLE:
	{
		if (!m_wpPlayer.lock()->CanAttack())
		{
			if (IsMoving())
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
			else
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
		}

		if (m_wpInputManager.lock()->IsMousePressed(EMouseActionType::LEFT_BUTTON))
		{
			m_bFireAssaultRifle = true;

			if (m_bCanFireAssaultRifle && !m_bReloading)
			{
				m_bReloading = !m_wpPlayer.lock()->FireAssaultRifle();

				// 소리 정지
				if (m_bReloading && m_eARFireSoundChannel != EChannelType::ENUM_END)
				{
					m_wpSoundManager.lock()->StopChannelEx(m_eARFireSoundChannel);
					NodePlaySound("AKE_Weapon_AssultRifle_Fire_FullAuto_Tail_788515356");
					m_eARFireSoundChannel = EChannelType::ENUM_END;
				}
				else if (m_eARFireSoundChannel == EChannelType::ENUM_END && m_tOwnerPlayerStateData.m_iCurrentBullet > 0)
				{					
					m_eARFireSoundChannel = m_wpSoundManager.lock()->PlaySoundEx("AKE_Weapon_AssultRifle_Fire_FullAuto_319879592", 1.f, true);
				}
				m_bCanFireAssaultRifle = false;
			}
		}
		else if (m_wpInputManager.lock()->IsMouseReleased(EMouseActionType::LEFT_BUTTON) && !m_bReloading)
		{
			if (IsMoving())
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
			else
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
		}
	}
	break;

	default:
	{
		if (m_wpInputManager.lock()->IsMousePressed(EMouseActionType::LEFT_BUTTON))
		{
			m_bAttackCall = true;
		}
		else if (m_wpInputManager.lock()->IsMouseUp(EMouseActionType::LEFT_BUTTON))
		{
			m_bAttackCall = false;
		}
	}
	break;
	}

	if (m_tOwnerPlayerStateData.m_eStandType == CPlayer::EStandType::STAND || m_tOwnerPlayerStateData.m_eStandType == CPlayer::EStandType::CROUCH)
	{
		if (m_wpInputManager.lock()->IsKeyDown(DIK_LCONTROL))
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::ROLL);
	}
}

void Client::CFSMNode_Player_Attack::ExpectNextDownAnim(FAnimData& _tDownAnimData)
{
	Vector3 vMoveDirection = m_tOwnerPlayerStateData.m_vMoveDirection;

	if (vMoveDirection.Length() > 0.1f)
	{
		switch (m_tOwnerPlayerStateData.m_eStandType)
		{
		case CPlayer::EStandType::CROUCH:
		{
			if (vMoveDirection.x > 0.1f)
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_WalkRight_Aim_Melee;
			else if (vMoveDirection.x < -0.1f)
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_WalkLeft_Aim_Melee;
			else if (vMoveDirection.z > 0.1f)
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_WalkFwd_Aim_Melee;
			else if (vMoveDirection.z < -0.1f)
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Walkbwd_Aim_Melee;
		}
		break;
		default:
		{
			if (vMoveDirection.x > 0.1f)
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JogRight_Aim_Melee;
			else if (vMoveDirection.x < -0.1f)
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JogLeft_Aim_Melee;
			else if (vMoveDirection.z > 0.1f)
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_None;
			else if (vMoveDirection.z < -0.1f)
			{
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_None;
				_tDownAnimData.m_bUseReversePlay = true;
			}
		}
		break;
		}
	}
	else
	{
		switch (m_tOwnerPlayerStateData.m_eStandType)
		{
		case CPlayer::EStandType::STAND:
		{
			switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
			{
			case EWeaponActionType::MELEE:
			{
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Attack_Melee;
				_tDownAnimData.m_fLoopRatio = 0.2f;
			}
			break;
			case EWeaponActionType::BOW:
			{
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_Bow;
			}
			break;
			case EWeaponActionType::ASSAULT_RIFLE:
			{
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_AssaultRifle;
			}
			break;
			case EWeaponActionType::SPEAR:
			{
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Attack_Spear;
			}
			break;
			case EWeaponActionType::PROJECTILE:
			{
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Throw;
			}
			break;
			case EWeaponActionType::ROCKET_LAUNCHER:
			{
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_RocketLauncher;
			}
			break;
			default:
			{
				_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Attack_None;
				_tDownAnimData.m_fLoopRatio = 0.1679f;
			}
			break;
			}
		}
			break;
		case CPlayer::EStandType::CROUCH:
		{
			_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Idle_None;
		}
			break;
		}
	}
}

void Client::CFSMNode_Player_Attack::ExpectNextUpperAnim(FAnimData& _tUpperAnimData)
{
	if (m_tOwnerPlayerStateData.m_bWeaponChangeCall)
	{
		if (m_tUpperAnimData.m_iAnimID == m_tUpperAnimationStateData.m_iCurrentAnimID &&
			IsUpperAnimLoopEnd())
		{
			m_wpPlayer.lock()->WeaponChangeEnd();
		}

		switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
		{
		case EWeaponActionType::BOW:
		{
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Draw_Bow;
			m_wpPlayer.lock()->ActiveArrow(true);
		}
		break;
		case EWeaponActionType::ASSAULT_RIFLE:
		{
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Draw_AssaultRifle;
		}
		break;
		case EWeaponActionType::ROCKET_LAUNCHER:
		{
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Draw_RocketLauncher;
		}
		break;
		default:
		{
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Draw_Melee;
		}
		break;
		}
	}
	else
	{
		switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
		{
		case EWeaponActionType::MELEE:
		{
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Attack_Melee;
			_tUpperAnimData.m_fLoopRatio = 0.22f;
		}
		break;
		case EWeaponActionType::BOW:
		{
			m_wpCombinedMeshComponent.lock()->SetRotationExceptBone(L"spine_01");
			m_wpPlayer.lock()->ActiveArrow(true);

			if (m_bShot)
			{
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Fire_Bow;
				m_wpPlayer.lock()->ChangeBowAnimation((int32)EBOW::AS_Bow_003_Fire);
				m_bCharging = false;
				m_wpPlayer.lock()->ActiveArrow(false);
			}
			else if (m_bCharging)
			{
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_FireChargeLoop_Bow;
			}
			else
			{
				if (m_tUpperAnimData.m_iAnimID == (int32)EPLAYER_FEMALE::AS_Player_Female_FireCharge_Bow && IsUpperAnimLoopEnd())
				{
					m_bCharging = true;
					NodePlaySound("AKE_Weapon_Bow_Charge_990427998");
				}
				else if (m_tUpperAnimData.m_iAnimID == (int32)EPLAYER_FEMALE::AS_Player_Female_FireCharge_Bow && m_tUpperAnimationStateData.m_fCurrentKeyFrameRate > 0.5f)
				{
					m_wpPlayer.lock()->ChangeBowAnimation((int32)EBOW::AS_Bow_003_FireChargeLoop);
				}
				else
				{
					_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_FireCharge_Bow;
					m_wpPlayer.lock()->ChangeBowAnimation((int32)EBOW::AS_Bow_003_FireCharge);
				}
			}
		}
		break;
		case EWeaponActionType::ASSAULT_RIFLE:
		{
			m_wpCombinedMeshComponent.lock()->SetRotationExceptBone(L"spine_01");

			if (m_bReloading)
			{
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Reload_AssaultRifle;
				m_wpPlayer.lock()->ChangeAssaultRifleAnimation((int32)EASSAULTRIFLE::AS_AssaultRifle_Empty);
			}
			else
			{
				_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_AssaultRifle;
				m_wpPlayer.lock()->ChangeAssaultRifleAnimation((int32)EASSAULTRIFLE::AS_AssaultRifle_Fire);
			}
		}
		break;
		case EWeaponActionType::SPEAR:
		{
			m_wpCombinedMeshComponent.lock()->SetRotationExceptBone(L"spine_01");

			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Attack_Spear;
		}
		break;
		case EWeaponActionType::PROJECTILE:
		{
			m_wpCombinedMeshComponent.lock()->SetRotationExceptBone(L"spine_01");

			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Throw;
		}
		break;
		case EWeaponActionType::ROCKET_LAUNCHER:
		{
			m_wpCombinedMeshComponent.lock()->SetRotationExceptBone(L"spine_01");

			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_RocketLauncher;
		}
		break;
		default:
		{
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Attack_None;
			_tUpperAnimData.m_fLoopRatio = 0.22f;
		}
		break;
		}
	}
}