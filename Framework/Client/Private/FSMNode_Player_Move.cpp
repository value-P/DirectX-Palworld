#include "pch.h"
#include "FSMNode_Player_Move.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "BuildManager.h"
#include "UIManager.h"
#include "CameraManager.h"
#include "PhysXManager.h"

#include "MeshComponent.h"
#include "FSMComponent.h"
#include "ColliderComponent.h"

#include "Player.h"
#include "Model.h"
#include "Monster.h"

shared_ptr<CFSMNode_Player_Move> Client::CFSMNode_Player_Move::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	shared_ptr<CFSMNode_Player_Move> spInstance = make_shared<CFSMNode_Player_Move>();
	CHECKF(spInstance, L"CFSMNode_Player_Move::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spPlayer);

	return spInstance;
}

void Client::CFSMNode_Player_Move::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	CFSMNode_Player::Initialize(_spMeshComponent, _spPlayer);
}

void Client::CFSMNode_Player_Move::OnEnterState()
{
    m_wpPlayer.lock()->SetCanJump(true);

	CFSMNode_Player::OnEnterState();
}

void Client::CFSMNode_Player_Move::OnState(float _fDeltaSeconds)
{
	CFSMNode_Player::OnState(_fDeltaSeconds);

    if(!m_bSprint) 
    {
        m_wpPlayer.lock()->ComputeStamina(_fDeltaSeconds); 
        m_fStaminaCheck = 0.f;
    }
    else 
    {
        if (IsStaminaEmptied())
        {
            m_bSprint = false;
        }
        else
        {
            m_fStaminaCheck += _fDeltaSeconds;
            if (m_fStaminaCheck > 1.f)
            {
                m_fStaminaCheck = 0.f;
                m_wpPlayer.lock()->DeclineStamina();
            }
        }
    }

    ComputeMoveSpeed();
    ComputeSpineRoatation();
    ComputeCatchRateUI();
    ComputeFootstep(_fDeltaSeconds);

    // ToJump
    if (m_tOwnerPlayerStateData.m_bJumping ||
        m_tOwnerPlayerStateData.m_eVerticalMoveType == EVerticalMovementType::DOWN)
    {
        m_wpPlayer.lock()->ZoomOut();
        m_wpPlayer.lock()->ActiveArrow(false);
        m_wpPlayer.lock()->ActivePalSphere(false);

        if (IsCurWeaponType(EWeaponActionType::PROJECTILE)) { m_wpPlayer.lock()->ChangeWeapon(m_tOwnerPlayerStateData.m_ePrevWeaponID); }

        m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::JUMP);
    }

    // ToIdle
    if (m_tOwnerPlayerStateData.m_vMoveDirection.Length() <= 0.1f)
    {
        m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
    }

    // Check sprintMode
    if (m_bSprint && IsAiming())
    {
        m_bSprint = false;
    }
}

void Client::CFSMNode_Player_Move::OnExitState()
{
    m_bEnableCatchRateUI = false;
    m_bSprint = false;
    m_wpPlayer.lock()->SetSpeed(3.f);
    m_fStaminaCheck = 0.f;

    m_bEPressed = false;
    m_bQPressed = false;
    m_bVPressed = false;
}

void Client::CFSMNode_Player_Move::KeyInput()
{
    // ToAttack
    if (CEngineModule::GetInstance()->GetInputManager()->IsMouseDown(EMouseActionType::LEFT_BUTTON) &&
        !IsStaminaEmptied() &&
        !CBuildManager::GetInstance()->IsBuildMode() &&
        !m_wpUIManager.lock()->IsOnUIPanel() && !m_bSprint &&
        !IsCurWeaponType(EWeaponActionType::PROJECTILE))
    {
        m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::ATTACK);
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
        if (IsCurWeaponType(EWeaponActionType::PROJECTILE))
            m_wpPlayer.lock()->ChangeWeapon(m_tOwnerPlayerStateData.m_ePrevWeaponID);

        m_wpPlayer.lock()->ActiveArrow(false);

        m_bEnableCatchRateUI = false;
        m_wpPlayer.lock()->EnActiveCatchRateUI();

        m_wpPlayer.lock()->ZoomOut();
    }

    // Dash
    if (m_tOwnerPlayerStateData.m_eStandType == CPlayer::EStandType::STAND)
    {
        if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_LSHIFT) &&
            !IsStaminaEmptied() && !IsAiming())
        {
            m_bSprint = true;
        }
        else if (CEngineModule::GetInstance()->GetInputManager()->IsKeyUp(DIK_LSHIFT))
        {
            m_bSprint = false;
        }
    }

    // ToRoll
    if (m_tOwnerPlayerStateData.m_eStandType == CPlayer::EStandType::STAND ||
        m_tOwnerPlayerStateData.m_eStandType == CPlayer::EStandType::CROUCH)
    {
        if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_LCONTROL))
            m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::ROLL);
    }
}

void Client::CFSMNode_Player_Move::ExpectNextDownAnim(FAnimData& _tDownAnimData)
{
    Vector3 vMoveDir = m_tOwnerPlayerStateData.m_vMoveDirection;

    switch (m_tOwnerPlayerStateData.m_eStandType)
    {
    case CPlayer::EStandType::CROUCH:
    {
        if (IsAiming())
        {
            if (vMoveDir.x > 0.1f)
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_WalkRight_Aim_Melee;
            else if (vMoveDir.x < -0.1f)
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_WalkLeft_Aim_Melee;
            else if (vMoveDir.z > 0.1f)
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_WalkFwd_Aim_Melee;
            else if (vMoveDir.z < -0.1f)
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Walkbwd_Aim_Melee;
        }
        else
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Walk;

    }
    break;
    default:
    {
        if (m_bSprint)
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Sprint;
        }
        else if (IsAiming())
        {
            if (vMoveDir.x > 0.1f)
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JogRight_Aim_Melee;
            else if (vMoveDir.x < -0.1f)
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_JogLeft_Aim_Melee;
            else if (vMoveDir.z > 0.1f)
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_None;
            else if (vMoveDir.z < -0.1f)
            {
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_None;
                _tDownAnimData.m_bUseReversePlay = true;
            }
        }
        else
        {
            if(m_tOwnerPlayerStateData.m_eWeaponActionType == EWeaponActionType::BOW)
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_Bow;
            else
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_None;
        }
    }
    break;
    }
}

void Client::CFSMNode_Player_Move::ExpectNextUpperAnim(FAnimData& _tUpperAnimData)
{
    if (IsAiming())
        m_wpCombinedMeshComponent.lock()->SetRotationExceptBone(L"spine_01");

	if (m_tOwnerPlayerStateData.m_bWeaponChangeCall)
	{
        ExpectWeaponChangeAnim(_tUpperAnimData);
	}
	else
	{
        ExpectUpperMoveAnim(_tUpperAnimData);
    }
}

void Client::CFSMNode_Player_Move::ComputeSpineRoatation()
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

void Client::CFSMNode_Player_Move::ComputeFootstep(float _fDeltaSeconds)
{
    m_fFootStepTime += _fDeltaSeconds;

    if (m_bSprint)
    {
        if (m_fFootStepTime > 0.3f)
        {
            m_fFootStepTime = 0.f;
            NodePlaySound("AKE_Player_Footsteps_Run_Sand_2_194671898");
        }
    }
    else
    {
        if (m_fFootStepTime > 0.5f)
        {
            m_fFootStepTime = 0.f;
            NodePlaySound("AKE_Player_Footsteps_Walk_Sand_2_844273401");
        }
    }
}

void Client::CFSMNode_Player_Move::ExpectWeaponChangeAnim(FAnimData& _tUpperAnimData)
{
    switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
    {
    case EWeaponActionType::NONE:
        m_wpPlayer.lock()->WeaponChangeEnd();
        ExpectUpperMoveAnim(_tUpperAnimData);
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
        ExpectUpperMoveAnim(_tUpperAnimData);
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

        ExpectUpperMoveAnim(_tUpperAnimData);
        return;
    }
}

void Client::CFSMNode_Player_Move::ExpectUpperMoveAnim(FAnimData& _tUpperAnimData)
{
    switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
    {
    case EWeaponActionType::MELEE:
    {
        if (m_bSprint)
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Sprint_Melee;
        else if (IsAiming())
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Idle_Melee;
        else
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_Melee;
    }
    break;
    case EWeaponActionType::BOW:
    {
        if (m_bSprint)
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Sprint_Bow;
        else if (IsAiming())
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_Bow;
        else
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_Bow;
    }
    break;
    case EWeaponActionType::ASSAULT_RIFLE:
    {
        if (m_bSprint)
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Sprint_AssaultRifle;
        else if (IsAiming())
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_AssaultRifle;
        else
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_AssaultRifle;
    }
    break;
    case EWeaponActionType::SPEAR:
    {
        if (m_bSprint)
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Sprint_Melee;
        else if (IsAiming())
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Idle_Spear;
        else
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_Spear;
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
        if (m_bSprint)
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Sprint_RocketLauncher;
        else if (IsAiming())
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_RocketLauncher;
        else
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_RocketLauncher;
    }
    break;
    default:
    {
        if (m_bSprint)
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Sprint;
        else if (IsAiming())
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Aim_None;
        else
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Jog_None;
    }
    break;
    }
}

void Client::CFSMNode_Player_Move::ComputeMoveSpeed()
{
    if (m_tOwnerPlayerStateData.m_eStandType == CPlayer::EStandType::CROUCH)
    {
        m_wpPlayer.lock()->SetSpeed(1.5f);
    }
    else if (m_bSprint)
    {
        m_wpPlayer.lock()->SetSpeed(6.f);
    }
    else
    {
        m_wpPlayer.lock()->SetSpeed(3.f);
    }
}

void Client::CFSMNode_Player_Move::ComputeCatchRateUI()
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
            m_wpPlayer.lock()->SetCatchRateUI(nullptr, Vector3(0.f, 0.f, 0.f));
        }
    }
    else
    {
        m_wpPlayer.lock()->EnActiveCatchRateUI();
    }
}
