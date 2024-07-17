#pragma once
#include "FSMNode_Player.h"

namespace Client
{
    class CFSMNode_Player_Move final : public CFSMNode_Player
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_Player_Move() = default;

		virtual ~CFSMNode_Player_Move() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CFSMNode_Player_Move> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		virtual void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer) override;

		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;

		virtual void OnExitState() override;

		virtual void KeyInput() override;

	protected:
		virtual void ExpectNextDownAnim(FAnimData& _tDownAnimData) override;

		virtual void ExpectNextUpperAnim(FAnimData& _tUpperAnimData) override;

		void ComputeSpineRoatation();		
		
		void ComputeFootstep(float _fDeltaSeconds);

	private:
		// Upper Expect
		void ExpectWeaponChangeAnim(FAnimData& _tUpperAnimData);
		void ExpectUpperMoveAnim(FAnimData& _tUpperAnimData);

		// Coumpute
		void ComputeMoveSpeed();

		// Q누르는 시점에서 포획확률 UI 활성화
		void ComputeCatchRateUI();

	/********************
		Data Members
	********************/
	protected:
		bool m_bSprint = false;
		bool m_bEnableCatchRateUI = false;

		float m_fStaminaCheck = 0.f;
		
		bool m_bQPressed = false;
		bool m_bEPressed = false;
		bool m_bVPressed = false;

		float m_fFootStepTime = 0.f;
	};
}
