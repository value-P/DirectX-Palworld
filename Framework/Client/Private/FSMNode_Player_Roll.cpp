#include "pch.h"
#include "FSMNode_Player_Roll.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "SoundManager.h"

#include "MeshComponent.h"
#include "FSMComponent.h"

#include "Player.h"
#include "Model.h"

shared_ptr<CFSMNode_Player_Roll> Client::CFSMNode_Player_Roll::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
    shared_ptr<CFSMNode_Player_Roll> spInstance = make_shared<CFSMNode_Player_Roll>();
    CHECKF(spInstance, L"CFSMNode_Player_Roll::Create : 생성에 실패했습니다");
    spInstance->Initialize(_spMeshComponent, _spPlayer);

    return spInstance;
}

void Client::CFSMNode_Player_Roll::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
    CFSMNode_Player::Initialize(_spMeshComponent, _spPlayer);
}

void Client::CFSMNode_Player_Roll::OnEnterState()
{
    NodePlaySound("AKE_Player_Rolling_Front_Impact_1_914056513");
    NodePlaySound("Player_Female_Rolling_Marina_EN_04_752367860");

    CFSMNode_Player::OnEnterState();
    
    m_wpPlayer.lock()->ChangeControlMode(EPlayerControllerMode::STATICMOVE_MODE);

    m_wpPlayer.lock()->ActiveCurrentWeapon(false);
}

void Client::CFSMNode_Player_Roll::OnState(float _fDeltaSeconds)
{
    UpdatePlayerData();

    if (m_tUpperAnimData.m_iAnimID == m_tUpperAnimationStateData.m_iCurrentAnimID &&
        m_tUpperAnimationStateData.m_fCurrentKeyFrameRate > 0.55f)
    {
        m_wpPlayer.lock()->SetSpeed(3.f);
        m_wpPlayer.lock()->SetFixedMoveDirection(Vector3(0.f, 0.f, 0.f));
    }

    if (m_tUpperAnimData.m_iAnimID == m_tUpperAnimationStateData.m_iCurrentAnimID &&
        IsUpperAnimLoopEnd())
    {
        m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
    }
}

void Client::CFSMNode_Player_Roll::OnExitState()
{
    m_wpPlayer.lock()->ActiveCurrentWeapon(true);
}

void Client::CFSMNode_Player_Roll::KeyInput()
{
}

void Client::CFSMNode_Player_Roll::ExpectNextDownAnim(FAnimData& _tDownAnimData)
{
    Vector3 vMoveDir = m_tOwnerPlayerStateData.m_vMoveDirection;
    Vector3 vRightDir = m_wpPlayer.lock()->GetRightVector().value();
    Vector3 vLookDir = m_wpPlayer.lock()->GetForwardVector().value();

    m_wpPlayer.lock()->SetSpeed(6.f);

    if (m_wpOwnerFSMComponent.lock()->GetPrevState() == (int32)CPlayer::EPlayerState::ATTACK ||
        IsAiming())
    {
        if (vMoveDir.x > 0.1f)
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_RollRight;
            m_wpPlayer.lock()->SetFixedMoveDirection(vRightDir);
        }
        else if (vMoveDir.x < -0.1f)
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_RollLeft;
            m_wpPlayer.lock()->SetFixedMoveDirection(-vRightDir);
        }
        else if (vMoveDir.z > 0.1f)
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_RollFwd;
            m_wpPlayer.lock()->SetFixedMoveDirection(vLookDir);
        }
        else
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_FlipBwd;
            m_wpPlayer.lock()->SetFixedMoveDirection(-vLookDir);
        }
    }
    else
    {
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_RollFwd;

        m_wpPlayer.lock()->SetFixedMoveDirection(vLookDir);
    }
}

void Client::CFSMNode_Player_Roll::ExpectNextUpperAnim(FAnimData& _tUpperAnimData)
{
    Vector3 vMoveDir = m_tOwnerPlayerStateData.m_vMoveDirection;

    if (m_wpOwnerFSMComponent.lock()->GetPrevState() == (int32)CPlayer::EPlayerState::ATTACK ||
        IsAiming())
    {
        if (vMoveDir.x > 0.1f)
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_RollRight;
        else if (vMoveDir.x < -0.1f)
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_RollLeft;
        else if (vMoveDir.z > 0.1f)
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_RollFwd;
        else if (vMoveDir.z < -0.1f)
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_FlipBwd;
        else
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_FlipBwd;
    }
    else
    {
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_RollFwd;
    }
}
