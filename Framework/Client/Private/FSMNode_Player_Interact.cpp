#include "pch.h"
#include "FSMNode_Player_Interact.h"

#include "UIManager.h"
#include "SoundManager.h"
#include "GameManager.h"
#include "MathUtility.h"
#include "InputManager.h"
#include "FSMComponent.h"
#include "Model.h"
#include "Bone.h"
#include "Date.h"

#include "Player.h"
#include "Architecture.h"
#include "Factory.h"
#include "Npc.h"
#include "RidingObject.h"
#include "Monster.h"
#include "Panel.h"

shared_ptr<CFSMNode_Player_Interact> Client::CFSMNode_Player_Interact::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	shared_ptr<CFSMNode_Player_Interact> spInstance = make_shared<CFSMNode_Player_Interact>();
	CHECKF(spInstance, L"CFSMNode_Player_Interact::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spPlayer);

	return spInstance;
}

void Client::CFSMNode_Player_Interact::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	CFSMNode_Player::Initialize(_spMeshComponent, _spPlayer);
}

void Client::CFSMNode_Player_Interact::OnEnterState()
{
	CFSMNode_Player::OnEnterState();

    switch (m_tOwnerPlayerStateData.m_eInteractType)
    {
    case Client::CPlayer::EInteractType::RIDING:
        NodePlaySound("Player_Female_Ride_Marina_EN_01_448211932");
        break;
    case Client::CPlayer::EInteractType::WORK:
        m_eChannel = m_wpSoundManager.lock()->PlaySoundEx("Crafting_Wooden_item_2_102992487", m_wpPlayer.lock()->GetPosition().value(), 1.f, true);
        break;
    default:
        break;
    }

	m_wpPlayer.lock()->ChangeControlMode(EPlayerControllerMode::NONEMOVE_MODE);

	m_wpPlayer.lock()->ActiveCurrentWeapon(false);

	m_vStartPlayerPos = m_wpPlayer.lock()->GetPosition().value();
	m_vStartLook = m_wpPlayer.lock()->GetForwardVector().value();
	m_vStartLook.y = 0.f;
	m_vStartLook.Normalize();

    m_bInSleep = true;
}

void Client::CFSMNode_Player_Interact::OnState(float _fDeltaSeconds)
{
    CFSMNode_Player::OnState(_fDeltaSeconds);

	switch (m_tOwnerPlayerStateData.m_eInteractType)
	{
	case CPlayer::EInteractType::MINING:
	case CPlayer::EInteractType::DEFOREST:
	case CPlayer::EInteractType::WORK:
	case CPlayer::EInteractType::WATERING:
	case CPlayer::EInteractType::COOKING:
	case CPlayer::EInteractType::PLANTING:
	{
		// 생산 중인 아이템이 없는 경우
		if (m_tOwnerPlayerStateData.m_wpInteractFactory.expired() ||
			!m_tOwnerPlayerStateData.m_wpInteractFactory.lock()->GetCanInteract())
		{
			if (IsMoving())
			{
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
			}    
			else
			{
				m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
			}

			return;
		}

		// 생산 중인 아이템이 있는 경우
		else
		{
			EProduceItemState eProduceItemState = m_tOwnerPlayerStateData.m_wpInteractFactory.lock()->ProduceItem(_fDeltaSeconds);

			// 아이템을 생산 완료한 경우
			if (eProduceItemState == EProduceItemState::COMPLETED)
			{
				m_tOwnerPlayerStateData.m_wpInteractFactory.lock()->ChangeCommandState(EToDoType::PRODUCE_ITEM, EToDoCommandState::FINISHED);

				if (IsMoving())
				{
					m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
				}
				else
				{
					m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
				}
				return;
			}
		}
	}
	    break;

	case CPlayer::EInteractType::ARCHITECT:
	{
		if (m_tOwnerPlayerStateData.m_wpInteractArchitecture.expired() ||
			m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->IsBuildComplete())
		{
			if (m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->IsFactory())
			{
				EFactoryType eFactory = m_tOwnerPlayerStateData.m_wpInteractFactory.lock()->GetFactoryType();

				switch (eFactory)
				{
				case Client::EFactoryType::WORK_BENCH:
				case Client::EFactoryType::ADVANCED_WORKBENCH:
				case Client::EFactoryType::REPAIR_BENCH:
				case Client::EFactoryType::SPHERE_TABLE:
				case Client::EFactoryType::WEAPON_WORKBENCH:
					m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::WORK);
					break;

				case Client::EFactoryType::CAMPFIRE:
				case Client::EFactoryType::POT:
					m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::COOKING);
					break;

				case Client::EFactoryType::BERRY_FARM:
				case Client::EFactoryType::WHEAT_FARM:
					m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::PLANTING);
					break;

				case Client::EFactoryType::LUMBERYARD:
					m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::DEFOREST);
					break;

				case Client::EFactoryType::STONE_PIT:
					m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::MINING);
					break;

                default:
                    break;
                }
            }
            else
            {
                switch (m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->GetArchitectureType())
                {
                case EArchitectureType::WOODEN_CHEST:
                case EArchitectureType::IRON_BOX:
                case EArchitectureType::PAL_BOX:
                    m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::OPEN);
                    break;

                case EArchitectureType::HUMAN_BED:
                    m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::SLEEP);
                    break;

                default:
                    m_wpPlayer.lock()->SetInteractType(CPlayer::EInteractType::ENUM_END);
                    break;
                }
            }

            if (IsMoving())
                m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
            else
                m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
        }
        else
        {
            if (m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->BuildArchitecture(_fDeltaSeconds))
            {
                m_tOwnerPlayerStateData.m_wpInteractArchitecture.lock()->UnRegisterCommandFromSystem(EToDoType::BUILD_ARCHITECTURE);
            }
        }
    }
        break;

    case CPlayer::EInteractType::PALRETURN:
    {
        if (IsDownAnimMatching())
        {
            if (m_tDownAnimationStateData.m_fTrackPosition >= 30.f && !m_bRetrivePalCalled)
            {
                m_bRetrivePalCalled = true;
                m_wpPlayer.lock()->RetrievePartnerPal();
            }
            else if (IsDownAnimLoopEnd())
            {
                m_wpPlayer.lock()->ActivePalSphere(false);

                if (IsMoving())
                    m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
                else
                    m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);

                m_wpPlayer.lock()->SetInteractType(m_tOwnerPlayerStateData.m_ePrevInteractType);
            }
        }
    }
        break;

    case CPlayer::EInteractType::RIDING_RETURN:
    {
        if (IsDownAnimMatching())
        {
            if (m_tDownAnimationStateData.m_fTrackPosition >= 30.f && !m_bRetrivePalCalled)
            {
                m_bRetrivePalCalled = true;
                m_wpPlayer.lock()->RetrieveRidingObject();
            }
            else if (IsDownAnimLoopEnd())
            {
                m_wpPlayer.lock()->ActivePalSphere(false);

                if (IsMoving())
                    m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
                else
                    m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);

                m_wpPlayer.lock()->SetInteractType(m_tOwnerPlayerStateData.m_ePrevInteractType);
            }
        }
    }
        break;

    case CPlayer::EInteractType::TALKING:
    {
        if (m_tOwnerPlayerStateData.m_wpInteractFactory.expired() ||
            !m_tOwnerPlayerStateData.m_wpInteractFactory.lock()->GetCanInteract())
        {
            if (IsMoving())
                m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
            else
                m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
        }
    }
        break;

    case CPlayer::EInteractType::RIDING:
    {
        if (IsDownAnimMatching())
        {
            if (IsDownAnimLoopEnd())
            {
                m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::RIDING);
            }
            else
            {
                // 라이딩 위치까지 포지션 러프
                Matrix matBone = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetRidingBone()->GetCombinedTransformationMatrix();
                Matrix matWorld = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetTransformMatrix().value();
                Vector3 vRidingOffset = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetRidingOffset();

                Vector3 vTranslation = matBone.Translation() + vRidingOffset;
                matBone.Translation(vTranslation);

                matWorld = matBone * matWorld;

                Vector3 vDstPos = matWorld.Translation();
                Vector3 vPos = CMathUtility::EaseInOutBack(m_vStartPlayerPos, vDstPos, m_tDownAnimationStateData.m_fCurrentKeyFrameRate + 0.05f) + Vector3(0.f,0.85f,0.f);

                m_wpPlayer.lock()->SetPositionWithController(vPos);

                // 라이딩의 look으로 서서히 lookAt
                Vector3 vDstLook = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetForwardVectorWithoutY().value();
                vDstLook.y = 0.f;
                vDstLook.Normalize();
                Vector3 vLook = Vector3::Lerp(m_vStartLook, vDstLook, m_tDownAnimationStateData.m_fCurrentKeyFrameRate);

                m_wpPlayer.lock()->LookAt(m_wpPlayer.lock()->GetPosition().value() + vLook);
            }

		}
	}
	    break;

    case CPlayer::EInteractType::PETTING:
    case CPlayer::EInteractType::FEEDING:
    {
        if (m_tOwnerPlayerStateData.m_wpInteractAllyPal.lock()->CanInteract() && !m_bPalInteractStarted) { m_bPalInteractStarted = true; }


        if (m_bPalInteractStarted && m_tDownAnimationStateData.m_iLoopCount > 2 &&
            IsDownAnimLoopEnd())
        {
            m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
            return;
        }
    }        
        break;

    case CPlayer::EInteractType::SLAUGHTER:
    {
        if (m_tOwnerPlayerStateData.m_wpInteractAllyPal.lock()->CanInteract() && !m_bPalInteractStarted) 
        {
            std::shared_ptr<CPanel> spPanel = m_wpUIManager.lock()->GetSlaughterPanel();
            spPanel->SetActive(true);
            spPanel->SetPivotWorldPoint(m_tOwnerPlayerStateData.m_wpInteractAllyPal.lock()->GetPosition().value());
            m_bPalInteractStarted = true; 
        }


        if (m_bPalInteractStarted && IsDownAnimLoopEnd())
        {
            m_wpUIManager.lock()->GetSlaughterPanel()->SetActive(false);
            m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
            return;
        }
    }
        break;

    case CPlayer::EInteractType::SLEEP:
    {
        m_wpPlayer.lock()->SetUseCameraMouseMove(false);

        if (CGameManager::GetInstance()->GetDate()->IsDay())
        {
            CGameManager::GetInstance()->GetDate()->SetDateTimeScale(1.f);
        }

        if (m_bInSleep && IsDownAnimMatching() && IsDownAnimLoopEnd()) 
        {
            m_bInSleep = false;
            m_bSleepLoop = true;

            CGameManager::GetInstance()->GetDate()->SetDateTimeScale(120.f);
        }
        else if (m_bSleepLoop && IsDownAnimMatching() && IsDownAnimLoopEnd() && CGameManager::GetInstance()->GetDate()->IsDay()) 
        {
            m_bSleepLoop = false; 
            m_bSleepEnd = true; 
        }
        else if (m_bSleepEnd && IsDownAnimMatching() && IsDownAnimLoopEnd())
        {
            m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
        }
    }
        break;

	case CPlayer::EInteractType::ENUM_END:
	{
		if (IsMoving())
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
		else
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
	}
		break;

	default:
		break;
	}
}

void Client::CFSMNode_Player_Interact::OnExitState()
{
    if (m_wpOwnerFSMComponent.lock()->GetNextState() != (int32)CPlayer::EPlayerState::RIDING)
    {
        m_wpPlayer.lock()->ChangeControlMode(EPlayerControllerMode::DEFAULT_MODE);
    }
     
    m_wpPlayer.lock()->ActiveCurrentWeapon(true);

	m_wpPlayer.lock()->ActiveConstructHammer(false);
	m_wpPlayer.lock()->ActiveFryingPan(false);
	m_wpPlayer.lock()->ActiveStoneAxe(false);
	m_wpPlayer.lock()->ActiveStonePickAxe(false);
    m_wpPlayer.lock()->ActiveMeatCleaver(false);

	m_bRetrivePalCalled = false;
    m_bPalInteractStarted = false;
	RideAnimtypeSelected = false;
    m_bInSleep = true;
    m_bSleepLoop = false;
    m_bSleepEnd = false;

    m_wpPlayer.lock()->SetUseCameraMouseMove(true);

    if (m_eChannel != EChannelType::ENUM_END)
    {
        m_wpSoundManager.lock()->StopChannelEx(m_eChannel);
    }
}

void Client::CFSMNode_Player_Interact::KeyInput()
{
	if(m_tOwnerPlayerStateData.m_eInteractType != CPlayer::EInteractType::PETTING &&
		m_tOwnerPlayerStateData.m_eInteractType != CPlayer::EInteractType::PALRETURN &&
		m_tOwnerPlayerStateData.m_eInteractType != CPlayer::EInteractType::RIDING_RETURN &&
		m_tOwnerPlayerStateData.m_eInteractType != CPlayer::EInteractType::TALKING &&
		m_tOwnerPlayerStateData.m_eInteractType != CPlayer::EInteractType::RIDING &&
		m_tOwnerPlayerStateData.m_eInteractType != CPlayer::EInteractType::SLEEP &&
		m_wpInputManager.lock()->IsKeyUp(DIK_F)) 
	{
		if (IsMoving())
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
		else
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
	}

	if (m_tOwnerPlayerStateData.m_eInteractType == CPlayer::EInteractType::TALKING &&
		!m_tOwnerPlayerStateData.m_wpInteractNpc.expired())
	{
		if (m_wpInputManager.lock()->IsKeyDown(DIK_F))
		{
			m_tOwnerPlayerStateData.m_wpInteractNpc.lock()->Interact();

			if(!m_tOwnerPlayerStateData.m_wpInteractNpc.lock()->IsDialogActivate())
			{
				if (IsMoving())
					m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::MOVE);
				else
					m_wpOwnerFSMComponent.lock()->ChangeState((int32)CPlayer::EPlayerState::IDLE);
			}
		}
	}
}

void Client::CFSMNode_Player_Interact::ExpectNextAnim(FAnimData& _tDownAnimData, FAnimData& _tUpperAnimData)
{
    switch (m_tOwnerPlayerStateData.m_eInteractType)
    {
    case Client::CPlayer::EInteractType::WORK:
    {
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_CommonWork;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_CommonWork;
    }
        break;
    case Client::CPlayer::EInteractType::MINING:
    {
        m_wpPlayer.lock()->ActiveStonePickAxe(true);
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Mining;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Mining;
    }
        break;
    case Client::CPlayer::EInteractType::DEFOREST:
    {
        m_wpPlayer.lock()->ActiveStoneAxe(true);
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Deforest_None;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Deforest_None;
    }
        break;
    case Client::CPlayer::EInteractType::WATERING:
    {
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Watering_None;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Watering_None;
    }
        break;
    case Client::CPlayer::EInteractType::COOKING:
    {
        m_wpPlayer.lock()->ActiveFryingPan(true);
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Cooking;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Cooking;
    }
        break;
    case Client::CPlayer::EInteractType::ARCHITECT: 
    {
        m_wpPlayer.lock()->ActiveConstructHammer(true);
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Architecture;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Architecture;
    }
        break;
    case Client::CPlayer::EInteractType::PLANTING:
    {
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Planting_None;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Planting_None;
    }
        break;
    case Client::CPlayer::EInteractType::PETTING:
    {
        if (m_bPalInteractStarted)
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Petting_Small;
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Petting_Small;
        }
        else
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Idle_None;
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Idle_None;
        }
    }
        break;
    case Client::CPlayer::EInteractType::FEEDING:
    {        
        if (m_bPalInteractStarted)
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Feeding;
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Feeding;
        }
        else
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Idle_None;
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Idle_None;
        }
    }
        break;

    case Client::CPlayer::EInteractType::SLAUGHTER:
    {   
        if (m_bPalInteractStarted)
        {
            m_wpPlayer.lock()->ActiveMeatCleaver(true);
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Dismembering_Melee;
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Dismembering_Melee;
        }
        else
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Idle_None;
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_Crouch_Idle_None;
        }
    }
        break;
    case Client::CPlayer::EInteractType::PALRETURN:
    case Client::CPlayer::EInteractType::RIDING_RETURN:
    {
        m_wpPlayer.lock()->ActivePalSphere(true);
        _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_PalReturn;
        _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_PalReturn;
    }
        break;
    case Client::CPlayer::EInteractType::RIDING:
    {
        if (!RideAnimtypeSelected)
        {
            Vector3 vRideObjectLook = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetForwardVectorWithoutY().value();

            Vector3 vOwnerPos = m_wpPlayer.lock()->GetPosition().value();
            Vector3 vRidePos = m_tOwnerPlayerStateData.m_wpRidingObject.lock()->GetPosition().value();

            Vector3 vDir = vOwnerPos - vRidePos;
            vDir.y = 0.f;
            vDir.Normalize();

            float fAngle = XMConvertToDegrees(acosf(vRideObjectLook.Dot(vDir)));

            // 전후좌우 라이딩 모션 측정
            if (fAngle < 45.f)
            {
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_JumpIn_Front;
                _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_JumpIn_Front;
            }
            else if (fAngle >= 45.f && fAngle < 135.f)
            {
                if (vDir.x > 0.f)
                {
                    _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_JumpIn_Right;
                    _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_JumpIn_Right;
                }
                else
                {
                    _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_JumpIn_Left;
                    _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_JumpIn_Left;
                }
            }
            else
            {
                _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_JumpIn_Rear;
                _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_HorseRide_JumpIn_Rear;
            }

            RideAnimtypeSelected = true;

        }
    }
        break;

    case Client::CPlayer::EInteractType::SLEEP:
    {
        if (m_bInSleep)
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_SleepIn;
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_SleepIn;
        }
        else if (m_bSleepLoop)
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_SleepLoop;
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_SleepLoop;
        }
        else if (m_bSleepEnd)
        {
            _tDownAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_SleepEnd;
            _tUpperAnimData.m_iAnimID = (int32)EPLAYER_FEMALE::AS_Player_Female_SleepEnd;
        }
    }
        break;

    default:
        break;
    }
}