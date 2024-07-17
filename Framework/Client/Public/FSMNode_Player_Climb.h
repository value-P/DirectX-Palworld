#pragma once
#include "FSMNode_Player.h"

namespace Engine
{
	class CPhysXManager;
}

namespace Client
{
    class CFSMNode_Player_Climb final : public CFSMNode_Player
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_Player_Climb() = default;

		virtual ~CFSMNode_Player_Climb() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CFSMNode_Player_Climb> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		virtual void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer) override;

		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;

		virtual void OnExitState() override;

	private:
		virtual void KeyInput() override;

		void ComputePlayerWithMapNoraml();

		void ComputeSound(float _fDeltaSeconds);

	private:
		virtual void ExpectNextAnim(FAnimData& _tDownAnimData, FAnimData& _tUpperAnimData) override;

		virtual void ExpectNextDownAnim(FAnimData& _tDownAnimData) {}

		virtual void ExpectNextUpperAnim(FAnimData& _tUpperAnimData) {}

	/********************
		Data Members
	********************/
	protected:
		weak_ptr<CPhysXManager> m_wpPhysXManager;

		bool m_bOnLedge = false;
		float m_fDeclineStaminaChecker = 0.f;
		float m_fSoundTimeChecker = 0.f;
    };
}


