#include "pch.h"
#include "HState.h"

#include "HFSMComponent.h"

HRESULT Engine::CHState::Initialize(const std::shared_ptr<FHStateDesc>& _spHStateDesc)
{
	m_wstrStateName = _spHStateDesc->m_wstrStateName;
	m_iHStateIndex = _spHStateDesc->m_iHStateIndex;
	m_spOwnerGameObject = _spHStateDesc->m_spOwnerGameObject;
	return S_OK;
}

int32 Engine::CHState::Tick(float _fDeltaSeconds)
{
	return 0;
}

void Engine::CHState::Release()
{
	m_spOwnerGameObject = nullptr;
}

HRESULT Engine::CHState::ReserveCommand(const std::vector<std::tuple<int32, int32>>& _vecCommand, EHFSMReserveCommandType _eHFSMReserveCommandType)
{
	if (FAILED(ResetHState())) { return E_FAIL; }

	m_wpHFSMComponent.lock()->ReserveCommand(_vecCommand, _eHFSMReserveCommandType);
	return S_OK;
}
