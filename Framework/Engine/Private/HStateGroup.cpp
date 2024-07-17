#include "pch.h"
#include "HStateGroup.h"

#include "HState.h"

std::shared_ptr<CHStateGroup> Engine::CHStateGroup::Create(const std::shared_ptr<FHStateGroupDesc>& _spHStateGroupDesc)
{
	const std::shared_ptr<CHStateGroup> spHStateGroup = make_shared<CHStateGroup>();
	ENSUREF(spHStateGroup->Initialize(_spHStateGroupDesc), L"Failed to initialize state group.");
	return spHStateGroup;
}

HRESULT Engine::CHStateGroup::Initialize(const std::shared_ptr<FHStateGroupDesc>& _spHStateGroupDesc)
{
	m_wstrStateGroupName = _spHStateGroupDesc->m_wstrStateGroupName;
	m_iHStateGroupIndex = _spHStateGroupDesc->m_iHStateGroupIndex;
	m_spOwnerGameObject = _spHStateGroupDesc->m_spOwnerGameObject;
	return S_OK;
}

int32 Engine::CHStateGroup::Tick(int32 _iStateIndex, float _fDeltaSeconds)
{
	return m_umapHStates[_iStateIndex]->Tick(_fDeltaSeconds);
}

void Engine::CHStateGroup::Release()
{
	m_spOwnerGameObject = nullptr;

	for (auto& pair : m_umapHStates)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapHStates.clear();
}

HRESULT Engine::CHStateGroup::AddHState(const std::shared_ptr<CHState>& _spHState)
{
	int32 iHStateIndex = _spHState->GetHStateIndex();

	if (iHStateIndex < 0) { return E_FAIL; }

	if (m_umapHStates.contains(iHStateIndex)) { return E_FAIL; }
	
	m_umapHStates.emplace(iHStateIndex, _spHState);
	return S_OK;
}

HRESULT Engine::CHStateGroup::RemoveHState(int32 _iHStateIndex)
{
	if (_iHStateIndex < 0) { return E_FAIL; }

	if (!m_umapHStates.contains(_iHStateIndex)) { return E_FAIL; }

	m_umapHStates.erase(_iHStateIndex);
	return S_OK;
}
