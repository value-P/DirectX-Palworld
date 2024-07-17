#pragma once
#include "FSMNode_Riding.h"

namespace Client
{
    class CFSMNode_WindDeerJump final : public CFSMNode_Riding
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_WindDeerJump() = default;

		virtual ~CFSMNode_WindDeerJump() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CFSMNode_WindDeerJump> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject);

		virtual void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject);
		
	/********************
		Methods
	********************/
	public:
		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;

		virtual void OnExitState() override;

		virtual void KeyInput() override;

	protected:
		virtual void ExpectNextAnim(FAnimData& _tAnimData) override;

	/********************
		Data Members
	********************/
	protected:
		bool m_bJumpStartEnd = false;
		bool m_bJumpEnd = false;
		bool m_bSecondJump = false;
    };
}