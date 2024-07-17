#include "pch.h"
#include "HSElecPandaMove.h"

#include "HFSMComponent.h"

#include "GameObject.h"
#include "Monster.h"

#include "MathUtility.h"

std::shared_ptr<CHSElecPandaMove> Client::CHSElecPandaMove::Create(const std::shared_ptr<FHSElecPandaMoveDesc> _spHSElecPandaMoveDesc)
{
	const std::shared_ptr<CHSElecPandaMove> spHSMove = make_shared<CHSElecPandaMove>();
	ENSUREF(spHSMove->Initialize(_spHSElecPandaMoveDesc), L"Failed to initialize hs elecpanda move.");
	return spHSMove;
}

HRESULT Client::CHSElecPandaMove::Initialize(const std::shared_ptr<FHStateDesc>& _spHStateDesc)
{
	// hstate desc
	if (FAILED(CHState::Initialize(_spHStateDesc))) { return E_FAIL; }

	// move hstate desc
	shared_ptr<FHSElecPandaMoveDesc> spHSMoveDesc = static_pointer_cast<FHSElecPandaMoveDesc>(_spHStateDesc);
	m_eElecPandaMoveType = spHSMoveDesc->m_eElecPandaMoveType;
	m_fDistanceRange = spHSMoveDesc->m_fDistanceRange;

	// etc
	m_spMonster = static_pointer_cast<CMonster>(m_spOwnerGameObject);
	return S_OK;
}

int32 Client::CHSElecPandaMove::Tick(float _fDeltaSeconds)
{
	switch (m_eElecPandaMoveType)
	{
	case Client::EElecPandaMoveType::TO_POSITION:
	{
		// 목적지를 바라보도록 회전합니다.
		m_fSumLerpDeltaSeconds += _fDeltaSeconds;
		if (m_fSumLerpDeltaSeconds <= m_fMaxLerpDeltaSeconds)
		{
			Vector3 vBossForward = m_spMonster->GetForwardVector().value();
			vBossForward.Normalize();

			Vector3 vBossPosition = m_spMonster->GetPosition().value();
			Vector3 vDirection = m_vDstPosition - vBossPosition;
			vDirection.Normalize();

			float fRatio = m_fSumLerpDeltaSeconds / m_fMaxLerpDeltaSeconds;
			Vector3 vLerpDirection = CMathUtility::EaseOutCirc(vBossForward, vDirection, fRatio);
			m_spMonster->LookAt(vBossPosition + vLerpDirection);
		}
		else
		{
			// 목적지와 가까운 경우
			Vector3 vBossPosition = m_spMonster->GetPosition().value();

			float fDistance = Vector3::Distance(vBossPosition, m_vDstPosition);
			if (fDistance <= m_fDistanceRange)
			{
				m_fSumLerpDeltaSeconds = 0.0f;
				EvaluateHState();
				return 0;
			}

			// 목적지와 먼 경우, 계속 이동합니다.
			m_spMonster->MoveForward(m_spMonster->GetMoveSpeed());
		}
	}
	break;

	case Client::EElecPandaMoveType::TO_TARGET:
	{
		// 목표를 바라보도록 회전합니다.
		m_fSumLerpDeltaSeconds += _fDeltaSeconds;
		if (m_fSumLerpDeltaSeconds <= m_fMaxLerpDeltaSeconds)
		{
			Vector3 vBossForward = m_spMonster->GetForwardVector().value();
			vBossForward.Normalize();

			Vector3 vBossPosition = m_spMonster->GetPosition().value();
			Vector3 vTargetPosition = m_spTarget->GetPosition().value();

			Vector3 vDirection = vTargetPosition - vBossPosition;
			vDirection.Normalize();

			float fRatio = m_fSumLerpDeltaSeconds / m_fMaxLerpDeltaSeconds;
			Vector3 vLerpDirection = CMathUtility::EaseOutCirc(vBossForward, vDirection, fRatio);
			m_spMonster->LookAt(vBossPosition + vLerpDirection);
		}
		else
		{
			// 목표와 가까운 경우
			Vector3 vBossPosition = m_spMonster->GetPosition().value();
			Vector3 vTargetPosition = m_spTarget->GetPosition().value();

			float fDistance = Vector3::Distance(vBossPosition, vTargetPosition);
			if (fDistance <= m_fDistanceRange)
			{
				m_fSumLerpDeltaSeconds = 0.0f;
				EvaluateHState();
				return 0;
			}

			// 목적지와 먼 경우, 계속 이동합니다.
			m_spMonster->MoveForward(m_spMonster->GetMoveSpeed());
		}
	}
	break;

	case Client::EElecPandaMoveType::TO_DIRECTION:
	{
		// 방향을 바라보도록 회전합니다.

		// todo;
	}
	break;

	default:
		CHECKF(false, L"Elec panda move type is not designated.");
		break;
	}

	return 0;
}

void Client::CHSElecPandaMove::Release()
{
	m_spMonster = nullptr;
	m_spTarget = nullptr;
	CHState::Release();
}

HRESULT Client::CHSElecPandaMove::ResetHState()
{
	return S_OK;
}

HRESULT Client::CHSElecPandaMove::ReserveCommand(const std::vector<std::tuple<int32, int32>>& _vecCommand, EHFSMReserveCommandType _eHFSMReserveCommandType)
{
	if (FAILED(CHState::ReserveCommand(_vecCommand, _eHFSMReserveCommandType))) { return E_FAIL; }

	return S_OK;
}

HRESULT Client::CHSElecPandaMove::EvaluateHState()
{
	Vector3 vTargetPosition = m_spTarget->GetPosition().value();


	// 플레이어와 거리가 멀 경우

	// 플레이어와 거리가 중간인 경우

	// 플레이어와 거리가 가까운 경우
	

	return S_OK;
}

HRESULT Client::CHSElecPandaMove::ReconstructHState()
{
	return S_OK;
}
