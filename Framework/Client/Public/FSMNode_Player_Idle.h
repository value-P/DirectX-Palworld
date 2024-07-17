#pragma once
#include "FSMNode_Player.h"

namespace Client
{
	class CChestBoxSystem;
	class CPalBoxSystem;
	class CPalInteractionSystem;

    class CFSMNode_Player_Idle final : public CFSMNode_Player
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_Player_Idle() = default;

		virtual ~CFSMNode_Player_Idle() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CFSMNode_Player_Idle> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		virtual void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer) override;

		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;

		virtual void OnExitState() override;

		virtual void KeyInput() override;

	private:
		void InteractKeyInput();

	protected:
		void ExpectNextDownAnim(FAnimData& _tDownAnimData) override;

		void ExpectNextUpperAnim(FAnimData& _tUpperAnimData) override;

	private:
		// Upper Expect
		void ExpectWeaponChangeAnim(FAnimData& _tUpperAnimData);
		void ExpectUpperIdleAnim(FAnimData& _tUpperAnimData);

		// Compute
		void ComputeSpineRoatation();

		// Q누르는 시점에서 포획확률 UI 활성화
		void ComputeCatchRateUI();

	/********************
		Data Members
	********************/
	protected:
		weak_ptr<CChestBoxSystem> m_wpChestBoxSystem;
		weak_ptr<CPalBoxSystem> m_wpPalboxSystem;
		weak_ptr<CPalInteractionSystem> m_wpPalInteractionSystem;

		bool m_bEnableCatchRateUI = false;

		bool m_bQPressed = false;
		bool m_bEPressed = false;
		bool m_bVPressed = false;
	};
}