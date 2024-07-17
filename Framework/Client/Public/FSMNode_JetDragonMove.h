
#pragma once
#include "FSMNode_Riding.h"

namespace Client
{
	class CFSMNode_JetDragonMove final : public CFSMNode_Riding
	{
    /********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_JetDragonMove() = default;

		virtual ~CFSMNode_JetDragonMove() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CFSMNode_JetDragonMove> Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject);

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
		bool m_bSprint = false;

		EChannelType m_eChannel = EChannelType::ENUM_END;
	};
}