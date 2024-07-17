#include "pch.h"
#include "HFSMComponent.h"

#include "HStateGroup.h"

std::shared_ptr<CHFSMComponent> Engine::CHFSMComponent::Create(const std::shared_ptr<FHFSMComponentDesc>& _spHFSMComponentDesc)
{
	const std::shared_ptr<CHFSMComponent> spHFSMComponent = make_shared<CHFSMComponent>();
	ENSUREF(spHFSMComponent->InitializeComponent(_spHFSMComponentDesc), L"Failed to initialize HFSMComponent");
	return spHFSMComponent;
}

HRESULT Engine::CHFSMComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	return S_OK;
}

HRESULT Engine::CHFSMComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CHFSMComponent::PreTickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CHFSMComponent::TickComponent(float _fDeltaSeconds)
{
	const vector<tuple<int32, int32>>& vecCommand = m_dqCommands.front();
	auto&[iStateGroup, iState] = vecCommand[m_iCurCommandIndex];

	if (m_umapHStateGroups[iStateGroup]->Tick(iState, _fDeltaSeconds) < 0)
	{
		++m_iCurCommandIndex;

		if (m_iCurCommandIndex >= vecCommand.size())
		{
			m_dqCommands.pop_front();
			m_iCurCommandIndex = 0;
			return 0;
		}
	}

	return 0;
}

int32 Engine::CHFSMComponent::PostTickComponent(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Engine::CHFSMComponent::EndPlay()
{
	return S_OK;
}

void Engine::CHFSMComponent::Render(int32 _iShaderPath)
{
}

void Engine::CHFSMComponent::Release()
{
	for (auto& pair : m_umapHStateGroups)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapHStateGroups.clear();
}

HRESULT Engine::CHFSMComponent::BindVariable()
{
	return S_OK;
}

HRESULT Engine::CHFSMComponent::AddHGroup(const std::shared_ptr<CHStateGroup>& _spHStateGroup)
{
	int32 iHStateGroupIndex = _spHStateGroup->GetHStateGroupIndex();

	if (iHStateGroupIndex < 0) { return E_FAIL; }

	if (m_umapHStateGroups.contains(iHStateGroupIndex)) { return E_FAIL; }

	m_umapHStateGroups.emplace(iHStateGroupIndex, _spHStateGroup);
	return S_OK;
}

HRESULT Engine::CHFSMComponent::RemoveHGroup(int32 _iHStateGroupIndex)
{
	if (_iHStateGroupIndex < 0) { return E_FAIL; }

	if (!m_umapHStateGroups.contains(_iHStateGroupIndex)) { return E_FAIL; }

	m_umapHStateGroups.erase(_iHStateGroupIndex);
	return S_OK;
}

void Engine::CHFSMComponent::ReserveCommand(const std::vector<std::tuple<int32, int32>>& _vecCommand, EHFSMReserveCommandType _eReserveCommandType)
{
	switch (_eReserveCommandType)
	{
	case Engine::EHFSMReserveCommandType::FRONT:
		m_dqCommands.push_front(_vecCommand);
		break;

	case Engine::EHFSMReserveCommandType::DEFERRED:
	{
		if (m_dqCommands.size() <= 1)
		{
			m_dqCommands.push_back(_vecCommand);
		}
		else
		{
			auto iter = m_dqCommands.begin()++;
			m_dqCommands.insert(iter, _vecCommand);
		}
	}
	break;

	case Engine::EHFSMReserveCommandType::BACK:
		m_dqCommands.push_back(_vecCommand);
		break;

	default:
		CHECKF(false, L"Reserve command type is not designated.");
		break;
	}
}
