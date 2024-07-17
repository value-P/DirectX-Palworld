#include "pch.h"
#include "HSElecPandaRoar.h"

#include "Monster.h"

std::shared_ptr<CHSElecPandaRoar> Client::CHSElecPandaRoar::Create(const std::shared_ptr<FHSElecPandaRoarDesc> _spHSElecPandaRoarDesc)
{
	const std::shared_ptr<CHSElecPandaRoar> spHSRoar = make_shared<CHSElecPandaRoar>();
	ENSUREF(spHSRoar->Initialize(_spHSElecPandaRoarDesc), L"Failed to initialize hs elecpanda roar.");
	return spHSRoar;
}

HRESULT Client::CHSElecPandaRoar::Initialize(const std::shared_ptr<FHStateDesc>& _spHStateDesc)
{
	// hstate desc
	if (FAILED(CHState::Initialize(_spHStateDesc))) { return E_FAIL; }

	// move hstate desc
	shared_ptr<FHSElecPandaRoarDesc> spHSRoarDesc = static_pointer_cast<FHSElecPandaRoarDesc>(_spHStateDesc);
	
	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_spOwnerGameObject);
	return S_OK;
}

int32 Client::CHSElecPandaRoar::Tick(float _fDeltaSeconds)
{
	return int32();
}

void Client::CHSElecPandaRoar::Release()
{
}

HRESULT Client::CHSElecPandaRoar::ResetHState()
{
	return E_NOTIMPL;
}

HRESULT Client::CHSElecPandaRoar::ReserveCommand(const std::vector<std::tuple<int32, int32>>& _vecCommand, EHFSMReserveCommandType _eHFSMReserveCommandType)
{
	return E_NOTIMPL;
}

HRESULT Client::CHSElecPandaRoar::EvaluateHState()
{
	return E_NOTIMPL;
}

HRESULT Client::CHSElecPandaRoar::ReconstructHState()
{
	return E_NOTIMPL;
}
