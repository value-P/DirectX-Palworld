#pragma once
#include "FSMNode.h"
#include "Player.h"

namespace Engine
{
	class CPhysXManager;
	class CInputManager;
	class CCameraManager;
	class CSoundManager;
}

namespace Client
{
	class CBuildManager;
	class CUIManager;

    class CFSMNode_Player abstract : public CFSMNode
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_Player() = default;

		virtual ~CFSMNode_Player() = default;

	/********************
		Events
	********************/
	public:
		virtual void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);
		
	/********************
		Getter/Setter
	********************/
		bool IsMoving() { return m_tOwnerPlayerStateData.m_vMoveDirection.Length() > 0.1f; }
		bool IsCurWeaponType(EWeaponActionType _EWeaponActionType) { return m_tOwnerPlayerStateData.m_eWeaponActionType == _EWeaponActionType; }
		bool IsAiming() { return m_tOwnerPlayerStateData.m_bAiming; }
		bool IsDownAnimLoopEnd() { return !m_tDownAnimationStateData.m_bIsLerping && m_tDownAnimationStateData.m_bLoopEnd; }
		bool IsUpperAnimLoopEnd() { return !m_tUpperAnimationStateData.m_bIsLerping && m_tUpperAnimationStateData.m_bLoopEnd; }
		bool IsDownAnimMatching() { return m_tDownAnimationStateData.m_iCurrentAnimID == m_tDownAnimData.m_iAnimID; }
		bool IsUpperAnimMatching() { return m_tUpperAnimationStateData.m_iCurrentAnimID == m_tUpperAnimData.m_iAnimID; }
		bool IsDownLerping() { return m_tDownAnimationStateData.m_bIsLerping; }
		bool IsUpperLerping() { return m_tUpperAnimationStateData.m_bIsLerping; }
		bool IsStaminaEmptied() { return m_tOwnerPlayerStateData.m_bStaminaEmptied; }

	/********************
		Methods
	********************/
	public:
		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;
				
	protected:
		virtual void ExpectNextAnim(FAnimData& _tDownAnimData, FAnimData& _tUpperAnimData) override;

		virtual void ExpectNextDownAnim(FAnimData& _tDownAnimData) = 0;

		virtual void ExpectNextUpperAnim(FAnimData& _tUpperAnimData) = 0;

		void UpdatePlayerData();

		EChannelType NodePlaySound(const string& _strSoundID);

	/********************
		Data Members
	********************/
	protected:
		weak_ptr<CPlayer> m_wpPlayer;
		weak_ptr<CInputManager> m_wpInputManager;
		weak_ptr<CPhysXManager> m_wpPhysXManager;
		weak_ptr<CBuildManager> m_wpBuildManager;
		weak_ptr<CUIManager> m_wpUIManager;
		weak_ptr<CCameraManager> m_wpCameraManager;
		weak_ptr<CSoundManager> m_wpSoundManager;

		bool m_bUpperEvent = false;

		bool m_bDownEvent = false;

		CPlayer::FPlayerStateData m_tOwnerPlayerStateData;

		FAnimationData m_tDownAnimationStateData;
		FAnimationData m_tUpperAnimationStateData;
    };
}