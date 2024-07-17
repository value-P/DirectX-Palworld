#include "pch.h"
#include "FSMNode_Player.h"

#include "EngineModule.h"
#include "InputManager.h"
#include "PhysXManager.h"
#include "ClientModule.h"
#include "UIManager.h"
#include "BuildManager.h"
#include "CameraManager.h"
#include "SoundManager.h"

#include "MeshComponent.h"
#include "Model.h"

void Client::CFSMNode_Player::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CPlayer> _spPlayer)
{
	CFSMNode::Initialize(_spMeshComponent);

	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();
	m_wpPhysXManager = CEngineModule::GetInstance()->GetPhysXManager();
	m_wpBuildManager = CClientModule::GetInstance()->GetBuildManager();
	m_wpUIManager	 = CClientModule::GetInstance()->GetUIManager();
	m_wpCameraManager= CEngineModule::GetInstance()->GetCameraManager();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	CHECKF(_spPlayer, L"CFSMNode_Player::Initialize : �÷��̾��� �����Ͱ� nullptr�Դϴ�");
	m_wpPlayer = _spPlayer;
}

void Client::CFSMNode_Player::OnEnterState()
{
	UpdatePlayerData();
	
	FAnimData tNextDownAnimData;
	FAnimData tNextUpperAnimData;

	ExpectNextAnim(tNextDownAnimData, tNextUpperAnimData);

	if (tNextDownAnimData.m_iAnimID != -1)
	{
		if(tNextDownAnimData.m_fLerpTime > 0.f)
			m_wpCombinedMeshComponent.lock()->ChangeDownAnimation(tNextDownAnimData.m_iAnimID, tNextDownAnimData.m_fLerpTime, tNextDownAnimData.m_bUseReversePlay, tNextDownAnimData.m_fLoopRatio);
		else
			m_wpCombinedMeshComponent.lock()->ChangeDownAnimation(tNextDownAnimData.m_iAnimID, tNextDownAnimData.m_bUseReversePlay, tNextDownAnimData.m_fLoopRatio);

		m_tDownAnimData = tNextDownAnimData;
	}

	if (tNextUpperAnimData.m_iAnimID != -1)
	{
		if (tNextUpperAnimData.m_fLerpTime > 0.f)
			m_wpCombinedMeshComponent.lock()->ChangeUpperAnimation(tNextUpperAnimData.m_iAnimID, tNextUpperAnimData.m_bUseReversePlay, tNextUpperAnimData.m_fLoopRatio);
		else
			m_wpCombinedMeshComponent.lock()->ChangeUpperAnimation(tNextUpperAnimData.m_iAnimID, tNextUpperAnimData.m_fLerpTime, tNextUpperAnimData.m_bUseReversePlay, tNextUpperAnimData.m_fLoopRatio);

		m_tUpperAnimData = tNextUpperAnimData;
	}
}

void Client::CFSMNode_Player::OnState(float _fDeltaSeconds)
{
	UpdatePlayerData();

	FAnimData tNextDownAnimData;
	FAnimData tNextUpperAnimData;

	// �ִϸ��̼� ����
	ExpectNextAnim(tNextDownAnimData, tNextUpperAnimData);

	tNextDownAnimData != m_tDownAnimData;

	// �ִϸ��̼� ���� Ȯ���� �ִϸ��̼� ����
	if (tNextDownAnimData.m_iAnimID != -1 &&
		tNextDownAnimData != m_tDownAnimData)
	{
		if (tNextDownAnimData.m_fLerpTime == 0.2f)
			m_wpCombinedMeshComponent.lock()->ChangeDownAnimation(tNextDownAnimData.m_iAnimID, tNextDownAnimData.m_bUseReversePlay, tNextDownAnimData.m_fLoopRatio);
		else
			m_wpCombinedMeshComponent.lock()->ChangeDownAnimation(tNextDownAnimData.m_iAnimID, tNextDownAnimData.m_fLerpTime, tNextDownAnimData.m_bUseReversePlay, tNextDownAnimData.m_fLoopRatio);

		m_tDownAnimData = tNextDownAnimData;
	}

	if (tNextUpperAnimData.m_iAnimID != -1 &&
		tNextUpperAnimData != m_tUpperAnimData)
	{
		if (tNextUpperAnimData.m_fLerpTime == 0.2f)
			m_wpCombinedMeshComponent.lock()->ChangeUpperAnimation(tNextUpperAnimData.m_iAnimID, tNextUpperAnimData.m_bUseReversePlay, tNextUpperAnimData.m_fLoopRatio);
		else
			m_wpCombinedMeshComponent.lock()->ChangeUpperAnimation(tNextUpperAnimData.m_iAnimID, tNextUpperAnimData.m_fLerpTime, tNextUpperAnimData.m_bUseReversePlay, tNextUpperAnimData.m_fLoopRatio);

		m_tUpperAnimData = tNextUpperAnimData;
	}
}

void Client::CFSMNode_Player::ExpectNextAnim(FAnimData& _tDownAnimData, FAnimData& _tUpperAnimData)
{
	ExpectNextDownAnim(_tDownAnimData);
	ExpectNextUpperAnim(_tUpperAnimData);
}

void Client::CFSMNode_Player::UpdatePlayerData()
{
	// �÷��̾� ����������
	m_tOwnerPlayerStateData = m_wpPlayer.lock()->GetPlayerData();
	
	// ���� �ִϸ��̼��� ���� ������
	m_tDownAnimationStateData = m_wpModel.lock()->GetModelAnimationData().m_tBaseAnimData;
	m_tUpperAnimationStateData = m_wpModel.lock()->GetModelAnimationData().m_tBlendAnimData;
}

EChannelType Client::CFSMNode_Player::NodePlaySound(const string& _strSoundID)
{
	return m_wpSoundManager.lock()->PlaySoundEx(_strSoundID, m_wpPlayer.lock()->GetPosition().value());
}
