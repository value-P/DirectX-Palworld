#pragma once
#include "FSMNode_Player.h"

namespace Client
{
	class CFSMNode_Player_Interact final : public CFSMNode_Player
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_Player_Interact() = default;
		virtual ~CFSMNode_Player_Interact() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CFSMNode_Player_Interact> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;

		virtual void OnExitState() override;

		virtual void KeyInput() override;

	protected:
		virtual void ExpectNextAnim(FAnimData& _tDownAnimData, FAnimData& _tUpperAnimData) override;

		virtual void ExpectNextDownAnim(FAnimData& _tDownAnimData) override {};

		virtual void ExpectNextUpperAnim(FAnimData& _tUpperAnimData) override {};

	/********************
		Data Members
	********************/
	protected:
		bool m_bRetrivePalCalled = false;

		EChannelType m_eChannel = EChannelType::ENUM_END;

		Vector3 m_vStartPlayerPos = Vector3(0.f);
		Vector3 m_vStartLook = Vector3(0.f);

		bool RideAnimtypeSelected = false;
		bool m_bPalInteractStarted = false;

		bool m_bInSleep = true;
		bool m_bSleepLoop = false;
		bool m_bSleepEnd = false;
	};
}


