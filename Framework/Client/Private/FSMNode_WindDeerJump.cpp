#include "pch.h"
#include "FSMNode_WindDeerJump.h"

#include "FSMComponent.h"
#include "InputManager.h"
#include "EffectManager.h"

std::shared_ptr<CFSMNode_WindDeerJump> Client::CFSMNode_WindDeerJump::Create(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	shared_ptr<CFSMNode_WindDeerJump> spInstance = make_shared<CFSMNode_WindDeerJump>();
	CHECKF(spInstance, L"CFSMNode_WindDeerJump::Create : 생성에 실패했습니다");
	spInstance->Initialize(_spMeshComponent, _spRidingObject);

	return spInstance;
}

void Client::CFSMNode_WindDeerJump::Initialize(shared_ptr<CMeshComponent> _spMeshComponent, shared_ptr<CRidingObject> _spRidingObject)
{
	CFSMNode_Riding::Initialize(_spMeshComponent, _spRidingObject);
}

void Client::CFSMNode_WindDeerJump::OnEnterState()
{
	NodePlaySound("FengyunDeeper_CloudTempest_Start_01_75069210");

	CFSMNode_Riding::OnEnterState();

	m_wpRidingObject.lock()->OnRidingEffect();
}

void Client::CFSMNode_WindDeerJump::OnState(float _fDeltaSeconds)
{
	CFSMNode_Riding::OnState(_fDeltaSeconds);

	if (m_tOwnerStateData.m_bOnGround) { m_bJumpEnd = true; }

	if (m_bJumpEnd)
	{
		if (IsMoving())
		{
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)ERidingObjectState::MOVE);
			return;
		}
		else if (m_tAnimData.m_iAnimID == (int32)EFengyunDeeper::AS_FengyunDeeper_JumpEnd)
		{
			m_wpOwnerFSMComponent.lock()->ChangeState((int32)ERidingObjectState::IDLE);
			return;
		}
	}

	if (m_tAnimData.m_iAnimID == (int32)EFengyunDeeper::AS_FengyunDeeper_JumpStart)
	{
		if (IsAnimLoopEnd())
		{
			m_bJumpStartEnd = true;
		}
	}
}

void Client::CFSMNode_WindDeerJump::OnExitState()
{
	m_wpRidingObject.lock()->OffRidingEffect();

	m_bJumpStartEnd = false;
	m_bJumpEnd = false;
	m_bSecondJump = false;
}

void Client::CFSMNode_WindDeerJump::KeyInput()
{
	if (!m_bSecondJump && m_wpInputManager.lock()->IsKeyDown(DIK_SPACE))
	{
		NodePlaySound("FengyunDeeper_CloudTempest_Start_01_75069210");

		m_wpRidingObject.lock()->Jump();
		m_bSecondJump = true;
		m_bJumpStartEnd = false;

		CEffectManager::GetInstance()->ActivateEffect(L"Group_WindDeer_Jump", m_wpRidingObject.lock()->GetPosition().value());
	}
}

void Client::CFSMNode_WindDeerJump::ExpectNextAnim(FAnimData& _tAnimData)
{
	float fVelocityY = m_tOwnerStateData.m_vLinearVelocity.y;

	if (m_bJumpEnd)
	{
		_tAnimData.m_iAnimID = (int32)EFengyunDeeper::AS_FengyunDeeper_JumpEnd;
	}
	else
	{
		if (!m_bJumpStartEnd)
		{
			_tAnimData.m_iAnimID = (int32)EFengyunDeeper::AS_FengyunDeeper_JumpStart;
		}
		else
		{
			_tAnimData.m_iAnimID = (int32)EFengyunDeeper::AS_FengyunDeeper_JumpLoop;
		}
	}
}