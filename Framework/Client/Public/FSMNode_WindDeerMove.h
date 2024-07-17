#pragma once
#include "FSMNode_Riding.h"

namespace Client
{
    class CFSMNode_WindDeerMove final : public CFSMNode_Riding
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_WindDeerMove() = default;

		virtual ~CFSMNode_WindDeerMove() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CFSMNode_WindDeerMove> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject);

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

		void ComputeFootstep(float _fDeltaSeconds);

	/********************
		Data Members
	********************/
	protected:
		bool m_bSprint = false;

		float m_fFootstepTime = 0.f;
    };
}