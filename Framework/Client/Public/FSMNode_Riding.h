#pragma once
#include "FSMNode.h"
#include "RidingObject.h"

namespace Engine
{
	class CInputManager;
	class CCameraManager;
	class CSoundManager;
}

namespace Client
{
    class CFSMNode_Riding abstract : public CFSMNode
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode_Riding() = default;

		virtual ~CFSMNode_Riding() = default;

	/********************
		Events
	********************/
	public:
		virtual void Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject);
		
	/********************
		Getter/Setter
	********************/
		bool IsMoving() { return m_tOwnerStateData.m_vMoveDirection.Length() > 0.1f; }
		bool IsAnimLoopEnd() { return m_tCurrentAnimStateData.m_bLoopEnd; }
		bool IsAnimMatching() { return m_tCurrentAnimStateData.m_iCurrentAnimID == m_tAnimData.m_iAnimID; }
		bool IsLerping() { return m_tCurrentAnimStateData.m_bIsLerping; }

	/********************
		Methods
	********************/
	public:
		virtual void OnEnterState() override;

		virtual void OnState(float _fDeltaSeconds) override;
		
	protected:
		virtual void ExpectNextAnim(FAnimData& _tDownAnimData, FAnimData& _tUpperAnimData) override {};
		
		virtual void ExpectNextAnim(FAnimData& _tAnimData) = 0;

		void UpdateRidingObjectData();

		EChannelType NodePlaySound(const string& _strSoundID);

	/********************
		Data Members
	********************/
	protected:
		weak_ptr<CRidingObject> m_wpRidingObject;
		weak_ptr<CInputManager> m_wpInputManager;
		weak_ptr<CCameraManager> m_wpCameraManager;
		weak_ptr<CSoundManager> m_wpSoundManager;

		CRidingObject::FRindingStateData m_tOwnerStateData;

		FAnimationData m_tCurrentAnimStateData;
		FAnimData m_tAnimData;
    };
}