#include "pch.h"
#include "FSMNode_Riding.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "SoundManager.h"

#include "MeshComponent.h"
#include "Model.h"

void Client::CFSMNode_Riding::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	CFSMNode::Initialize(_spMeshComponent);

	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	m_wpRidingObject = _spRidingObject;
}

void Client::CFSMNode_Riding::OnEnterState()
{
	UpdateRidingObjectData();

	FAnimData tAnimData;

	ExpectNextAnim(tAnimData);

	m_wpCombinedMeshComponent.lock()->ChangeAnimation(tAnimData.m_iAnimID, false);

	m_tAnimData = tAnimData;
}

void Client::CFSMNode_Riding::OnState(float _fDeltaSeconds)
{
	UpdateRidingObjectData();

	FAnimData tAnimData;

	ExpectNextAnim(tAnimData);

	m_wpCombinedMeshComponent.lock()->ChangeDifferentAnimation(tAnimData.m_iAnimID);

	m_tAnimData = tAnimData;
}

void Client::CFSMNode_Riding::UpdateRidingObjectData()
{
	m_tOwnerStateData = m_wpRidingObject.lock()->GetObjectData();

	m_tCurrentAnimStateData = m_wpModel.lock()->GetModelAnimationData().m_tBaseAnimData;
}

EChannelType Client::CFSMNode_Riding::NodePlaySound(const string& _strSoundID)
{
	return m_wpSoundManager.lock()->PlaySoundEx(_strSoundID, m_wpRidingObject.lock()->GetPosition().value());
}
