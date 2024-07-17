#include "pch.h"
#include "OpenLevelEvent.h"

#include "World.h"
#include "ClientModule.h"

std::shared_ptr<COpenLevelEvent> Client::COpenLevelEvent::Create(const std::shared_ptr<FOpenLevelEventDesc>& _spOpenLevelDesc)
{
	std::shared_ptr<COpenLevelEvent> spOpenLevelEvent = make_shared<COpenLevelEvent>();
	ENSUREF(spOpenLevelEvent->Initialize(_spOpenLevelDesc), L"Failed to initialize open level event.");
	return spOpenLevelEvent;
}

HRESULT Client::COpenLevelEvent::Initialize(const std::shared_ptr<FEventDesc>& _spEventDesc)
{
	if (FAILED(CEvent::Initialize(_spEventDesc))) { return E_FAIL; }

	shared_ptr<FOpenLevelEventDesc> spOpenLevelEventDesc = static_pointer_cast<FOpenLevelEventDesc>(_spEventDesc);
	m_ePostLevelType = spOpenLevelEventDesc->m_ePostLevelType;
	m_spPostFrontLevel = spOpenLevelEventDesc->m_spPostFrontLevel;
	m_spPostBackLevel = spOpenLevelEventDesc->m_spPostBackLevel;
	return S_OK;
}

void Client::COpenLevelEvent::Release()
{
	m_spPostFrontLevel = nullptr;
	m_spPostBackLevel = nullptr;

	CEvent::Release();
}

void Client::COpenLevelEvent::InvokeEvent()
{
	CWorld::GetInstance()->OpenLevel(m_spPostFrontLevel, m_spPostBackLevel);

	CClientModule::GetInstance()->SetCurLevelType(m_ePostLevelType);
}
