#pragma once
#include "FSMNode_Player.h"

namespace Client
{
    class CFSMNode_Player_Jump final : public CFSMNode_Player
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_Player_Jump() = default;

		virtual ~CFSMNode_Player_Jump() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CFSMNode_Player_Jump> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		virtual void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer) override;

		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;

		virtual void OnExitState() override;

	private:
		virtual void KeyInput() override;

		void ClimbingCheck();

	private:
		virtual void ExpectNextDownAnim(FAnimData& _tDownAnimData) override;

		virtual void ExpectNextUpperAnim(FAnimData& _tUpperAnimData) override;

	/********************
		Data Members
	********************/
	protected:
		bool m_bJumpStartEnd = false;
		bool m_bJumpEnd = false;

		bool m_bGliding = false;

		float m_fGlidingTimeChecker = 0.f;
		float m_fNoneGlidingTimeChecker = 0.8f;
		float m_fDeclineStaminaChecker = 0.f;
    };
}


