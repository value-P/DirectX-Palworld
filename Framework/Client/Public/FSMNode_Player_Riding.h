#pragma once
#include "FSMNode_Player.h"

namespace Client
{
    class CFSMNode_Player_Riding final : public CFSMNode_Player
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_Player_Riding() = default;
		virtual ~CFSMNode_Player_Riding() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CFSMNode_Player_Riding> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;

		virtual void OnExitState() override;

		virtual void KeyInput() override;

	protected:
		virtual void ExpectNextDownAnim(FAnimData& _tDownAnimData) override;

		virtual void ExpectNextUpperAnim(FAnimData& _tUpperAnimData) override;

		// Upper Expect
		void ExpectWeaponChangeAnim(FAnimData& _tUpperAnimData);

		void ExpectUpperRidingAnim(FAnimData& _tUpperAnimData);

	/********************
		Data Members
	********************/
	protected:
		float m_fKeyDownTime = 0.f;
    };
}