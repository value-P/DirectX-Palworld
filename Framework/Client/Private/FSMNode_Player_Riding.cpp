#include "pch.h"
#include "FSMNode_Player_Riding.h"

#include "InputManager.h"
#include "MathUtility.h"
#include "Player.h"
#include "FSMComponent.h"
#include "Model.h"
#include "Bone.h"
#include "RidingObject.h"

shared_ptr<CFSMNode_Player_Riding> Client::CFSMNode_Player_Riding::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
    shared_ptr<CFSMNode_Player_Riding> spInstance = make_shared<CFSMNode_Player_Riding>();
    CHECKF(spInstance, L"CFSMNode_Player_Riding::Create : 생성에 실패했습니다");
    spInstance->Initialize(_spMeshComponent, _spPlayer);

    return spInstance;
}

void Client::CFSMNode_Player_Riding::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
    CFSMNode_Player::Initialize(_spMeshComponent, _spPlayer);
}

void Client::CFSMNode_Player_Riding::OnEnterState()
{
    CFSMNode_Player::OnEnterState();

    m_wpPlayer.lock()->ChangeControlMode(EPlayerControllerMode::NONEMOVE_MODE);

    m_tOwnerPlayerStateData.m_wpRidingObject.lock()->SetRideMode(m_wpPlayer.lock());

    m_wpPlayer.lock()->ActiveStaminaUI(false);
}

void Client::CFSMNode_Player_Riding::OnState(float _fDeltaSeconds)
{
    CFSMNode_Player::OnState(_fDeltaSeconds);

    Matrix matBone = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetRidingBone()->GetCombinedTransformationMatrix();
    Matrix matWorld = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetTransformMatrix().value();
    Vector3 vRidingOffset = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetRidingOffset();
    
    Vector3 vTranslation = matBone.Translation() + vRidingOffset;
    matBone.Translation(vTranslation);

    matWorld = matBone * matWorld;

    Vector3 vPos = matWorld.Translation() + Vector3(0.f, 0.65f, 0.f);

    m_wpPlayer.lock()->SetPositionWithController(vPos);

    Vector3 vLook = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetForwardVectorWithoutY().value();
    m_wpPlayer.lock()->LookAt(m_wpPlayer.lock()->GetPosition().value() + vLook);


    if (m_wpInputManager.lock()->IsKeyPressed(DIK_F))
    {
        m_fKeyDownTime += _fDeltaSeconds;
        if (m_fKeyDownTime > 1.5f)
        {
            m_fKeyDownTime = 0.f;

            Vector3 vRideObjectRight = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetRightVector().value();
            Vector3 vCurPos = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetPosition().value();

            m_wpPlayer.lock()->SetPositionWithController(vCurPos + Vector3(0.f,3.f,0.f) + vRideObjectRight * 2.f);

            m_wpPlayer.lock()->RideOff();
            m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::JUMP);
            return;
        }
    }
}

void Client::CFSMNode_Player_Riding::OnExitState()
{
    m_wpPlayer.lock()->ChangeControlMode(EPlayerControllerMode::DEFAULT_MODE);

    m_tOwnerPlayerStateData.m_wpRidingObject.lock()->SetRideMode(nullptr);
}

void Client::CFSMNode_Player_Riding::KeyInput()
{
}

void Client::CFSMNode_Player_Riding::ExpectNextDownAnim(FAnimData& _tDownAnimData)
{
    _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_None;
}

void Client::CFSMNode_Player_Riding::ExpectNextUpperAnim(FAnimData& _tUpperAnimData)
{
    if (m_tOwnerPlayerStateData.m_bWeaponChangeCall)
    {
        ExpectWeaponChangeAnim(_tUpperAnimData);
    }
    else
    {
        ExpectUpperRidingAnim(_tUpperAnimData);
    }
}

void Client::CFSMNode_Player_Riding::ExpectWeaponChangeAnim(FAnimData& _tUpperAnimData)
{
    switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
    {
    case EWeaponActionType::NONE:
    {
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Draw_Melee;
        _tUpperAnimData.m_bUseReversePlay = true;
    }
    break;

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
        ExpectUpperRidingAnim(_tUpperAnimData);
        return;
    }
}

void Client::CFSMNode_Player_Riding::ExpectUpperRidingAnim(FAnimData& _tUpperAnimData)
{
    if (IsMoving())
    {
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_None;
    }
    else
    {
        switch (m_tOwnerPlayerStateData.m_eWeaponActionType)
        {
        case EWeaponActionType::MELEE:
        case EWeaponActionType::SPEAR:
        {
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_Idle_Melee;
        }
        break;
        case EWeaponActionType::BOW:
        {
            m_wpPlayer.lock()->DisableBowAnimation();
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_Idle_Bow;
        }
        break;
        case EWeaponActionType::ASSAULT_RIFLE:
        {
            m_wpPlayer.lock()->ChangeAssaultRifleAnimation((int32)EASSAULTRIFLE::AS_AssaultRifle_Empty);
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_Idle_AssaultRifle;
        }
        break;
        default:
        {
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_None;
        }
        break;
        }
    }
}
