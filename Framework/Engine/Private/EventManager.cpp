#include "pch.h"
#include "EventManager.h"

#include "Event.h"

HRESULT Engine::CEventManager::Initialize()
{
	return S_OK;
}

void Engine::CEventManager::Release()
{
	for (auto& spEvent : m_vecForwardEvents)
	{
		spEvent->Release();
		spEvent = nullptr;
	}
	m_vecForwardEvents.clear();

	for (auto& spEvent : m_vecDeferredEvents)
	{
		spEvent->Release();
		spEvent = nullptr;
	}
	m_vecDeferredEvents.clear();
}

HRESULT Engine::CEventManager::AddEvent(const std::shared_ptr<CEvent>& _spEvent)
{
	if (!_spEvent) { return E_FAIL; }
	
	switch (_spEvent->GetEventInvokeType())
	{
	case EEventInvokeType::FORWARD:
		m_vecForwardEvents.emplace_back(_spEvent);
		break;

	case EEventInvokeType::DEFERRED:
		m_vecDeferredEvents.emplace_back(_spEvent);
		break;

	default:
		CHECKF(false, L"Event invoke type is not designated.");
		break;
	}

	return S_OK;
}

HRESULT Engine::CEventManager::RemoveEvent(EEventType _eEventType)
{
	return S_OK;
}

void Engine::CEventManager::InvockEvent(EEventInvokeType _eEventInvokeType)
{
	switch (_eEventInvokeType)
	{
	case Engine::EEventInvokeType::FORWARD:
	{
		if (m_vecForwardEvents.empty()) { return; }

		for (const auto& spEvent : m_vecForwardEvents)
		{
			spEvent->InvokeEvent();
		}
	}
	break;

	case Engine::EEventInvokeType::DEFERRED:
	{
		if (m_vecDeferredEvents.empty()) { return; }

		for (const auto& spEvent : m_vecDeferredEvents)
		{
			spEvent->InvokeEvent();
		}
	}
	break;

	default:
		CHECKF(false, L"Event invoke type is not designated.");
		break;
	}
}

void Engine::CEventManager::ClearEvent()
{
	for (auto& spEvent : m_vecForwardEvents)
	{
		spEvent->Release();
		spEvent = nullptr;
	}
	m_vecForwardEvents.clear();

	for (auto& spEvent : m_vecDeferredEvents)
	{
		spEvent->Release();
		spEvent = nullptr;
	}
	m_vecDeferredEvents.clear();
}
