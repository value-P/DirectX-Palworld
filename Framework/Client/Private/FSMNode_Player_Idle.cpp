#include "pch.h"
#include "FSMNode_Player_Idle.h"

#include "GameManager.h"
#include "ChestBoxSystem.h"
#include "CameraManager.h"
#include "PhysXManager.h"
#include "InputManager.h"
#include "BuildManager.h"
#include "UIManager.h"
#include "PalBoxSystem.h"
#include "PalInteractionSystem.h"

#include "MeshComponent.h"
#include "FSMComponent.h"
#include "ColliderComponent.h"

#include "Npc.h"
#include "Player.h"
#include "Model.h"
#include "Monster.h"
#include "Factory.h"

shared_ptr<CFSMNode_Player_Idle> Client::CFSMNode_Player_Idle::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	shared_ptr<CFSMNode_Player_Idle> spInstance = make_shared<CFSMNode_Player_Idle>();
	CHECKF(spInstance, L"CFSMNode_Player_Idle::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spPlayer);
	
	return spInstance;
}

void Client::CFSMNode_Player_Idle::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	CFSMNode_Player::Initialize(_spMeshComponent, _spPlayer);

	m_wpChestBoxSystem = CGameManager::GetInstance()->GetChestBoxSystem();
	m_wpPalboxSystem = CGameManager::GetInstance()->GetPalBoxSystem();
	m_wpPalInteractionSystem = CGameManager::GetInstance()->GetPalInteractionSystem();
}

void Client::CFSMNode_Player_Idle::OnEnterState()
{
	m_wpPlayer.lock()->SetCanJump(true);

	CFSMNode_Player::OnEnterState();
}

void Client::CFSMNode_Player_Idle::OnState(float _fDeltaSeconds)
{
	CFSMNode_Player::OnState(_fDeltaSeconds);

	m_wpPlayer.lock()->ComputeStamina(_fDeltaSeconds);
	ComputeSpineRoatation();
	ComputeCatchRateUI();

	InteractKeyInput();

	// To Jump
	if (m_tOwnerPlayerStateData.m_bJumping ||
		m_tOwnerPlayerStateData.m_eVerticalMoveType == EVerticalMovementType::DOWN)
	{
		m_wpPlayer.lock()->ZoomOut();
		m_wpPlayer.lock()->ActiveArrow(false);
		m_wpPlayer.lock()->ActivePalSphere(false);

		if (IsCurWeaponType(EWeaponActionType::PROJECTILE)) { m_wpPlayer.lock()->ChangeWeapon(m_tOwnerPlayerStateData.m_ePrevWeaponID); }

		m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::JUMP);
		return;
	}

	// To Move
	if (m_tDownAnimData.m_iAnimID == m_tDownAnimationStateData.m_iCurrentAnimID &&
		!IsDownLerping() &&
		IsMoving())
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
		return;
	}

	if (m_tDownAnimData.m_iAnimID == m_tDownAnimationStateData.m_iCurrentAnimID &&
		!IsDownLerping())
	{
		m_wpPlayer.lock()->ChangeControlMode(EPlayerControllerMode::DEFAULT_MODE);
	}
}

void Client::CFSMNode_Player_Idle::OnExitState()
{
	m_bEnableCatchRateUI = false;
	m_bEPressed = false;
	m_bQPressed = false;
	m_bVPressed = false;
}

void Client::CFSMNode_Player_Idle::KeyInput()
{
	// ToAttack
	if (m_wpInputManager.lock()->IsMouseDown(EMouseActionType::LEFT_BUTTON) &&
		!IsStaminaEmptied() && m_wpPlayer.lock()->CanAttack() &&
		!m_wpBuildManager.lock()->IsBuildMode() &&
		!m_wpUIManager.lock()->IsOnUIPanel() &&
		!IsCurWeaponType(EWeaponActionType::PROJECTILE))
	{
		m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::ATTACK);
		return;
	}

	// PartnerSpwan or Return
	if (!m_wpPlayer.lock()->IsRidingObjectSpawned() && !m_bQPressed && !m_bVPressed)
	{
		if (m_wpInputManager.lock()->IsKeyUp(DIK_E))
		{
			m_bEPressed = false;
			if (IsCurWeaponType(EWeaponActionType::PROJECTILE))
			{
				m_wpPlayer.lock()->ZoomOut();
				m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::SPAWN_PAL);
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::ATTACK);
				return;
			}
		}
		else if (m_wpInputManager.lock()->IsKeyDown(DIK_E) && !m_wpBuildManager.lock()->IsBuildMode())
		{
			m_bEPressed = true;

			// 팰을 회수합니다.
			if (m_wpPlayer.lock()->IsPartnerPalSpawned())
			{
				m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::PALRETURN);
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::INTERACT);
				return;
			}
			else if (!m_wpPlayer.lock()->IsCurrentPartnerPalEmpty())
			{
				m_wpPlayer.lock()->ZoomIn();
				m_wpPlayer.lock()->ChangeWeapon(EItemType::BLUE_SPHERE);
			}
		}
	}

	// ToRidingPalSpeher or Return
	if (!m_wpPlayer.lock()->IsPartnerPalSpawned() && m_wpPlayer.lock()->CanSpawnCurrentRiding() && !m_bQPressed && !m_bEPressed)
	{
		if (m_wpInputManager.lock()->IsKeyUp(DIK_V))
		{
			m_bVPressed = false;
			if (IsCurWeaponType(EWeaponActionType::PROJECTILE))
			{
				m_wpPlayer.lock()->ZoomOut();
				m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::SPAWN_RIDING);
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::ATTACK);
				return;
			}
		}
		else if (m_wpInputManager.lock()->IsKeyDown(DIK_V) && !m_wpBuildManager.lock()->IsBuildMode())
		{
			m_bVPressed = true;

			// 라이딩을 회수합니다.
			if (m_wpPlayer.lock()->IsRidingObjectSpawned())
			{
				m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::RIDING_RETURN);
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::INTERACT);
				return;
			}
			else
			{
				m_wpPlayer.lock()->ZoomIn();
				m_wpPlayer.lock()->ChangeWeapon(EItemType::BLUE_SPHERE);
			}
		}
	}

	// ToThrowPalSphere
	if (m_wpInputManager.lock()->IsKeyUp(DIK_Q) && !m_bVPressed && !m_bEPressed)
	{
		m_bQPressed = false;
		if (IsCurWeaponType(EWeaponActionType::PROJECTILE))
		{
			m_wpPlayer.lock()->EnActiveCatchRateUI();
			m_wpPlayer.lock()->ZoomOut();
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::ATTACK);
			return;
		}
	}
	else if (m_wpInputManager.lock()->IsKeyDown(DIK_Q))
	{
		m_bQPressed = true;
		if (!IsCurWeaponType(EWeaponActionType::PROJECTILE) && !m_wpBuildManager.lock()->IsBuildMode() && m_wpPlayer.lock()->CanThrowPalSphere())
		{
			m_wpPlayer.lock()->ZoomIn();
			m_wpPlayer.lock()->ChangeWeapon(EItemType::BLUE_SPHERE);
		}
	}
	else if (m_wpInputManager.lock()->IsKeyPressed(DIK_Q) && IsCurWeaponType(EWeaponActionType::PROJECTILE))
	{
		m_bEnableCatchRateUI = true;
	}

	// Zoom In/Out
	if (m_wpInputManager.lock()->IsMouseDown(EMouseActionType::RIGHT_BUTTON))
	{
		m_wpPlayer.lock()->ZoomIn();
	}
	else if (m_wpInputManager.lock()->IsMouseUp(EMouseActionType::RIGHT_BUTTON))
	{
		if(IsCurWeaponType(EWeaponActionType::PROJECTILE))
			m_wpPlayer.lock()->ChangeWeapon(m_tOwnerPlayerStateData.m_ePrevWeaponID);

		m_wpPlayer.lock()->ActiveArrow(false);

		m_bEnableCatchRateUI = false;
		m_wpPlayer.lock()->EnActiveCatchRateUI();

		m_wpPlayer.lock()->ZoomOut();

		m_bEPressed = false;
		m_bQPressed = false;
		m_bVPressed = false;
	}

	// ToRoll
	if (m_tOwnerPlayerStateData.m_eStandType == CPlayer::EStandType::STAND || m_tOwnerPlayerStateData.m_eStandType == CPlayer::EStandType::CROUCH)
	{
		if (IsDownAnimMatching() && !IsDownLerping() &&
			m_tDownAnimationStateData.m_fTrackPosition > 14.f &&
			m_wpInputManager.lock()->IsKeyDown(DIK_LCONTROL))
		{
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::ROLL);
			return;
		}
	}
}

void Client::CFSMNode_Player_Idle::InteractKeyInput()
{
	if (m_wpInputManager.lock()->IsKeyDown(DIK_F))
	{
		switch (m_tOwnerPlayerStateData.m_eInteractType)
		{
		case Client::CPlayer::EInteractType::WORK:
		case Client::CPlayer::EInteractType::MINING:
		case Client::CPlayer::EInteractType::DEFOREST:
		case Client::CPlayer::EInteractType::WATERING:
		case Client::CPlayer::EInteractType::COOKING:
		case Client::CPlayer::EInteractType::PLANTING:
		{
			if (nullptr == m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()) { break; }
			if (m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->IsFactory())
			{
				shared_ptr<CFactory> spFactory = m_tOwnerPlayerStateData.m_wpInteractFactory.lock();

				switch (spFactory->GetFactoryType())
				{
				case Client::EFactoryType::WORK_BENCH:
				case Client::EFactoryType::REPAIR_BENCH:
				case Client::EFactoryType::CAMPFIRE:
				case Client::EFactoryType::CRUSHER:
				case Client::EFactoryType::ADVANCED_WORKBENCH:
				case Client::EFactoryType::BLAST_FURNACE:
				case Client::EFactoryType::SPHERE_TABLE:
				case Client::EFactoryType::WHEAT_FARM:
				case Client::EFactoryType::MILL:
				case Client::EFactoryType::POT:
				case Client::EFactoryType::WEAPON_WORKBENCH:
				{
					if (spFactory->IsProduceCompleted())
					{
						spFactory->TakeItemFromFactory();
						return;
					}
				}
				break;

				case Client::EFactoryType::BERRY_FARM:
				case Client::EFactoryType::LUMBERYARD:
				case Client::EFactoryType::STONE_PIT:
				{
					if (spFactory->IsProduceCompleted() ||
						spFactory->IsProducePartialyCompleted())
					{
						spFactory->TakeItemFromFactory();
						return;
					}
				}
				break;

				default:
					break;
				}

				if (m_tOwnerPlayerStateData.m_wpInteractFactory.lock()->GetCanInteract())
				{
					m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::INTERACT);
				}
			}
		}
		break;

		case Client::CPlayer::EInteractType::COLLECT:
		{
			if (m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->IsFactory())
			{
				if (m_tOwnerPlayerStateData.m_wpInteractFactory.lock()->IsProduceCompleted())
				{
					m_tOwnerPlayerStateData.m_wpInteractFactory.lock()->TakeItemFromFactory();
				}
			}
		}
		break;
		case Client::CPlayer::EInteractType::ARCHITECT:
		case Client::CPlayer::EInteractType::PETTING:
		case Client::CPlayer::EInteractType::SPAWN_PAL:
		case Client::CPlayer::EInteractType::PALRETURN:
		case Client::CPlayer::EInteractType::RIDING:
		{
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::INTERACT);
		}
		break;

		case Client::CPlayer::EInteractType::SLEEP:
		{
			Vector3 vPos = m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->GetPosition().value();
			Vector3 vRight = m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->GetRightVectorWithoutY().value();
			m_wpPlayer.lock()->SetPositionWithController(vPos + Vector3(0.f, 1.3f, 0.f) + vRight * 0.35f);
			m_wpPlayer.lock()->LookAt(m_wpPlayer.lock()->GetPosition().value() + vRight);
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::INTERACT);
		}
		break;

		case Client::CPlayer::EInteractType::TALKING:
		{
			m_tOwnerPlayerStateData.m_wpInteractNpc.lock()->Interact();
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::INTERACT);
		}
		break;

		case Client::CPlayer::EInteractType::OPEN:
		{
			if (nullptr == m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()) { break; }
			switch (m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->GetArchitectureType())
			{
			case EArchitectureType::WOODEN_CHEST:
			case EArchitectureType::IRON_BOX:
				m_wpChestBoxSystem.lock()->OpenSpecificChestBox(m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->GetArchitectureType(), m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->GetGameObjectID(), DIK_F);
				break;

			case EArchitectureType::PAL_BOX:
				m_wpPalboxSystem.lock()->OpenContainerWidget(DIK_F);
				break;

			default:
				break;
			}
		}
		break;

		case Client::CPlayer::EInteractType::ENUM_END:
		default:
			break;
		}
	}

	if (m_wpInputManager.lock()->IsKeyDown(DIK_4) && !m_tOwnerPlayerStateData.m_wpInteractAllyPal.expired())
	{
		m_wpPalInteractionSystem.lock()->ToggleInteractUI();

		// 꺼졌으면 상호작용 팰 해제
		if(!m_wpPalInteractionSystem.lock()->IsInteracUIActivate())
		{
			m_wpPlayer.lock()->ResetInteractAllyPal();
		}
	}

	if (m_wpInputManager.lock()->IsKeyDown(DIK_G))
	{
		m_wpPalInteractionSystem.lock()->ToggleEmoteUI();

		// 꺼졌으면 상호작용 팰 해제
		if (!m_wpPalInteractionSystem.lock()->IsInteracUIActivate())
		{
			m_wpPlayer.lock()->ResetInteractAllyPal();
		}
	}

	// 상호작용 UI 켜져있을때 좌클릭하면 해당인덱스에 맞춰서 상태 변환
	if (m_wpInputManager.lock()->IsMouseDown(EMouseActionType::LEFT_BUTTON) && m_wpPalInteractionSystem.lock()->IsInteracUIActivate())
	{
		int32 iSelectedInteractType = m_wpPalInteractionSystem.lock()->InteractUIClick();

		EInteractionSystemUIType eType = m_wpPalInteractionSystem.lock()->GetCurrentUIType();

		if (eType == EInteractionSystemUIType::EPalInteract)
		{
			if (iSelectedInteractType != -1)
			{
				EMonsterInteractType eInteractType = static_cast<EMonsterInteractType>(iSelectedInteractType);
				m_tOwnerPlayerStateData.m_wpInteractAllyPal.lock()->InteractWithSlavePal(eInteractType);

				switch (eInteractType)
				{
				case Client::EMonsterInteractType::EAT:
					m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::FEEDING);
					m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::INTERACT);
					return;

				case Client::EMonsterInteractType::PET:
					m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::PETTING);
					m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::INTERACT);
					return;

				case Client::EMonsterInteractType::SLAUGHTER:
					m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::SLAUGHTER);
					m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::INTERACT);
					return;

				default:
					break;
				}
			}
		}
		else if (eType == EInteractionSystemUIType::EEmote)
		{
			if (iSelectedInteractType != -1)
			{
				EEmoteType eEmoteType = static_cast<EEmoteType>(iSelectedInteractType);

				m_wpPlayer.lock()->SetEmoteType(eEmoteType);
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::EMOTE);
			}
		}
	}
}

void Client::CFSMNode_Player_Idle::ExpectNextDownAnim(FAnimData& _tDownAnimData)
{
	switch (m_tOwnerPlayerStateData.m_eStandType)
	{
	case CPlayer::EStandType::CROUCH:
		_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Idle_None;
		break;
	default:
		_tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Idle_None;
		break;
	}
}

void Client::CFSMNode_Player_Idle::ExpectNextUpperAnim(FAnimData& _tUpperAnimData)
{
	if (IsAiming())
		m_wpCombinedMeshComponent.lock()->SetRotationExceptBone(L"spine_01");

	if (m_tOwnerPlayerStateData.m_bWeaponChangeCall)
	{
		ExpectWeaponChangeAnim(_tUpperAnimData);
	}
	else
	{
		ExpectUpperIdleAnim(_tUpperAnimData);
	}
}

void Client::CFSMNode_Player_Idle::ExpectWeaponChangeAnim(FAnimData& _tUpperAnimData)
{
	switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
	{
	case EWeaponActionType::NONE:	
		m_wpPlayer.lock()->WeaponChangeEnd();
		ExpectUpperIdleAnim(_tUpperAnimData);
		return;

	case EWeaponActionType::BOW:
	{
		_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Draw_Bow;
		m_wpPlayer.lock()->DisableBowAnimation();
	}
		break;

	case EWeaponActionType::ASSAULT_RIFLE:
	{
		m_wpPlayer.lock()->ChangeAssaultRifleAnimation((int32)EASSAULTRIFLE::AS_AssaultRifle_Empty);
		_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Draw_AssaultRifle;
	}
		break;
		
	case EWeaponActionType::ROCKET_LAUNCHER:
	{
		_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Draw_RocketLauncher;
	}
		break;

	case EWeaponActionType::PROJECTILE:
	{
		m_wpPlayer.lock()->WeaponChangeEnd();
		ExpectUpperIdleAnim(_tUpperAnimData);
	}
		return;

	default:
	{
		_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Draw_Melee;
	}
		break;
	}

	if (_tUpperAnimData.m_iAnimID == m_tUpperAnimationStateData.m_iCurrentAnimID &&
		IsUpperAnimLoopEnd())
	{
		m_wpPlayer.lock()->WeaponChangeEnd();
		ExpectUpperIdleAnim(_tUpperAnimData);
		return;
	}
}

void Client::CFSMNode_Player_Idle::ExpectUpperIdleAnim(FAnimData& _tUpperAnimData)
{
	switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
	{
	case EWeaponActionType::MELEE:
	{
		_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Idle_Melee;
	}
	break;
	case EWeaponActionType::BOW:
	{
		if (IsAiming())
		{
			m_wpPlayer.lock()->ActiveArrow(true);
			m_wpPlayer.lock()->ChangeBowAnimation((int32)EBOW::AS_Bow_003_Idle);
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_Bow;
		}
		else
		{
			m_wpPlayer.lock()->ActiveArrow(false);
			m_wpPlayer.lock()->DisableBowAnimation();
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Idle_Bow;
		}
	}
	break;
	case EWeaponActionType::ASSAULT_RIFLE:
	{
		m_wpPlayer.lock()->ChangeAssaultRifleAnimation((int32)EASSAULTRIFLE::AS_AssaultRifle_Empty);

		if (IsAiming())
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_AssaultRifle;
		else
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Idle_AssaultRifle;
	}
	break;
	case EWeaponActionType::SPEAR:
	{
		_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Idle_Spear;
	}
	break;
	case EWeaponActionType::PROJECTILE:
	{
		_tUpperAnimData.m_fLerpTime = 0.05f;
		_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_Throw;
	}
	break;
	case EWeaponActionType::ROCKET_LAUNCHER:
	{
		if (IsAiming())
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_RocketLauncher;
		else
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Idle_RocketLauncher;
	}
	break;
	default:
	{
		if (IsAiming())
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_None;
		else
			_tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Idle_None;
	}
	break;
	}
}

void Client::CFSMNode_Player_Idle::ComputeSpineRoatation()
{
	if (IsAiming())
	{
		m_wpPlayer.lock()->SetUseSpineRotation(true);
	}
	else
	{
		m_wpPlayer.lock()->SetUseSpineRotation(false);
	}
}

void Client::CFSMNode_Player_Idle::ComputeCatchRateUI()
{
	if (m_bEnableCatchRateUI)
	{
		PxRaycastHit tRayHit;

		Vector3 vCamPos = m_wpCameraManager.lock()->GetCurCameraWorldPosition();
		Vector3 vCamLook = m_wpCameraManager.lock()->GetCurCameraWorldForwardVector();

		if (m_wpPhysXManager.lock()->SingleRayCast(vCamPos, vCamLook, 100.f, ECollisionLayer::MONSTER, &tRayHit))
		{
			CColliderComponent* pColliderComponent = static_cast<CColliderComponent*>(tRayHit.actor->userData);

			shared_ptr<CMonster> spMonster = static_pointer_cast<CMonster>(pColliderComponent->GetOwnerGameObject());
			
			m_wpPlayer.lock()->SetCatchRateUI(spMonster, CPhysXManager::GetVector3FromPxVec3(tRayHit.position));
		}
		else
		{
			m_wpPlayer.lock()->SetCatchRateUI(nullptr, Vector3(0.f,0.f,0.f));
		}
	}
	else
	{
		m_wpPlayer.lock()->EnActiveCatchRateUI();
	}
}
