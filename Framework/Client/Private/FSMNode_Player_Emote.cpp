#include "pch.h"
#include "FSMNode_Player_Emote.h"

#include "Player.h"
#include "FSMComponent.h"
#include "Model.h"

shared_ptr<CFSMNode_Player_Emote> Client::CFSMNode_Player_Emote::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
    shared_ptr<CFSMNode_Player_Emote> spInstance = make_shared<CFSMNode_Player_Emote>();
    CHECKF(spInstance, L"CFSMNode_Player_Emote::Create : 생성에 실패했습니다");
    spInstance->Initialize(_spMeshComponent, _spPlayer);

    return spInstance;
}

void Client::CFSMNode_Player_Emote::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
    CFSMNode_Player::Initialize(_spMeshComponent, _spPlayer);
}

void Client::CFSMNode_Player_Emote::OnEnterState()
{
    CFSMNode_Player::OnEnterState();

    m_wpPlayer.lock()->ChangeControlMode(EPlayerControllerMode::NONEMOVE_MODE);
}

void Client::CFSMNode_Player_Emote::OnState(float _fDeltaSeconds)
{
    UpdatePlayerData();

    switch (m_tOwnerPlayerStateData.m_eEmoteType)
    {
    case EEmoteType::CHEER:
    {
        if (IsDownAnimMatching() && IsDownAnimLoopEnd())
        {
            m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
        }
    }
        break;
    case EEmoteType::DANCE:
    case EEmoteType::WAVE:
    {
        if (IsMoving())
        {
            m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
        }
        else if (m_tDownAnimationStateData.m_iLoopCount > 10)
        {
            m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
        }
    }
        break;
    default:
        break;
    }
}

void Client::CFSMNode_Player_Emote::OnExitState()
{
    m_wpPlayer.lock()->ChangeControlMode(EPlayerControllerMode::DEFAULT_MODE);
}

void Client::CFSMNode_Player_Emote::KeyInput()
{
}

void Client::CFSMNode_Player_Emote::ExpectNextAnim(FAnimData& _tDownAnimData, FAnimData& _tUpperAnimData)
{
    switch (m_tOwnerPlayerStateData.m_eEmoteType)
    {
    case EEmoteType::CHEER:
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Emote_Cheer;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Emote_Cheer;
        break;
    case EEmoteType::DANCE:
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Emote_Dance;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Emote_Dance;
        break;
    case EEmoteType::WAVE:
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Emote_Wave;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Emote_Wave;
        break;
    default:
        break;
    }
}