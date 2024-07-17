#include "pch.h"
#include "HSElecPandaSkill.h"

std::shared_ptr<CHSElecPandaSkill> Client::CHSElecPandaSkill::Create(const std::shared_ptr<FHSElecPandaSkillDesc> _spHSElecPandaSkillDesc)
{
	const std::shared_ptr<CHSElecPandaSkill> spHSElecPandaSkill = make_shared<CHSElecPandaSkill>();
	ENSUREF(spHSElecPandaSkill->Initialize(_spHSElecPandaSkillDesc), L"Failed to initialize hs skill.");
	return spHSElecPandaSkill;
}

HRESULT Client::CHSElecPandaSkill::Initialize(const std::shared_ptr<FHStateDesc>& _spHStateDesc)
{
	// hstate desc
	if (FAILED(CHState::Initialize(_spHStateDesc))) { return E_FAIL; }

	// skill hstate desc
	shared_ptr<FHSElecPandaSkillDesc> spHSkillDesc = static_pointer_cast<FHSElecPandaSkillDesc>(_spHStateDesc);
	m_eElecPandaSkillType = spHSkillDesc->m_eElecPandaSkillType;
	return S_OK;
}

int32 Client::CHSElecPandaSkill::Tick(float _fDeltaSeconds)
{
	switch (m_eElecPandaSkillType)
	{
	case Client::EElecPandaSkillType::SHORT_RANGE_TYPE_A:
		break;

	case Client::EElecPandaSkillType::SHORT_RANGE_TYPE_B:
		break;

	case Client::EElecPandaSkillType::SHORT_RANGE_TYPE_C:
		break;

	case Client::EElecPandaSkillType::MID_RANGE_TYPE_A:
		break;

	case Client::EElecPandaSkillType::MID_RANGE_TYPE_B:
		break;

	case Client::EElecPandaSkillType::MID_RANGE_TYPE_C:
		break;

	case Client::EElecPandaSkillType::LONG_RANGE_TYPE_A:
		break;

	case Client::EElecPandaSkillType::LONG_RANGE_TYPE_B:
		break;

	case Client::EElecPandaSkillType::LONG_RANGE_TYPE_C:
		break;

	default:
		CHECKF(false, L"Elec panda skill type is not designated.");
		break;
	}
	return 0;
}

void Client::CHSElecPandaSkill::Release()
{
	CHState::Release();
}

HRESULT Client::CHSElecPandaSkill::ResetHState()
{
	return S_OK;
}

HRESULT Client::CHSElecPandaSkill::ReserveCommand(const std::vector<std::tuple<int32, int32>>& _vecCommand, EHFSMReserveCommandType _eHFSMReserveCommandType)
{
	if (FAILED(CHState::ReserveCommand(_vecCommand, _eHFSMReserveCommandType))) { return E_FAIL; }

	return S_OK;
}

HRESULT Client::CHSElecPandaSkill::EvaluateHState()
{
	// 플레이어와 거리가 멀 경우

	// 플레이어와 거리가 중간인 경우

	// 플레이어와 거리가 가까운 경우

	//ReserveCommand({ { , (int32)EElecPandaMoveGroupType::FORWARD }, {}, {} });

	return S_OK;
}

HRESULT Client::CHSElecPandaSkill::ReconstructHState()
{
	return S_OK;
}
