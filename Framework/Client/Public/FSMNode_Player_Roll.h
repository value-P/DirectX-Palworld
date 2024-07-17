#pragma once
#include "FSMNode_Player.h"

namespace Client
{
	class CFSMNode_Player_Roll final : public CFSMNode_Player
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_Player_Roll() = default;
		virtual ~CFSMNode_Player_Roll() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CFSMNode_Player_Roll> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer);

		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;

		virtual void OnExitState() override;

		virtual void KeyInput() override;

	protected:
		virtual void ExpectNextDownAnim(FAnimData& _tDownAnimData) override;

		virtual void ExpectNextUpperAnim(FAnimData& _tUpperAnimData) override;

	/********************
		Data Members
	********************/
	protected:

	};
}


