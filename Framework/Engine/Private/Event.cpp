#include "pch.h"
#include "Event.h"

#include "World.h"

HRESULT Engine::CEvent::Initialize(const std::shared_ptr<FEventDesc>& _spEventDesc)
{
	m_eEventType = _spEventDesc->m_eEventType;
	m_eEventInvokeType = _spEventDesc->m_eEventInvokeType;
	return S_OK;
}

void Engine::CEvent::Release()
{
}
