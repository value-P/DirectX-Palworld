#pragma once
#include "FSMNode_Player.h"

namespace Client
{
    class CFSMNode_Player_Attack final : public CFSMNode_Player
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_Player_Attack() = default;

		virtual ~CFSMNode_Player_Attack() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CFSMNode_Player_Attack> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		virtual void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer) override;

		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;

		virtual void OnExitState() override;

	private:
		virtual void KeyInput() override;

	private:
		virtual void ExpectNextDownAnim(FAnimData& _tDownAnimData) override;

		virtual void ExpectNextUpperAnim(FAnimData& _tUpperAnimData) override;

	/********************
		Data Members
	********************/
	protected:
		bool m_bAttackCall = false;

		bool m_bCharging = false;

		bool m_bShot = false;
		
		bool m_bFireAssaultRifle = false;
		bool m_bCanFireAssaultRifle = false;
		bool m_bReloading = false;

		float m_fShotDelayChecker = 0.f;
		float m_fShotDelay = 0.065f;

		EChannelType m_eARFireSoundChannel = EChannelType::ENUM_END;
	};
}
