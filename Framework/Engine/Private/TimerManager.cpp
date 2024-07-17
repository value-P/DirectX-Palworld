#include "pch.h"
#include "TimerManager.h"

#include "Timer.h"
#include "Date.h"

HRESULT Engine::CTimerManager::Initialize()
{
	FDateDesc tDataDesc;
	tDataDesc.m_fDeltaSecondsPerDay = 86'400.0f;

	std::shared_ptr<CDate> spDate = CDate::Create(make_shared<FDateDesc>(tDataDesc));
	ENSUREF(AddDate(L"StandardDate", spDate), L"Failed to add standared date.");
	return S_OK;
}

int32 Engine::CTimerManager::Tick(float _fDeltaSeconds)
{
	m_fWorldDeltaSeconds += _fDeltaSeconds;

	for (const auto& pair : m_umapTimers)
	{
		pair.second->Tick(_fDeltaSeconds);
	}

	for (const auto& pair : m_umapDates)
	{
		pair.second->Tick(_fDeltaSeconds);
	}

	return 0;
}

void Engine::CTimerManager::Release()
{
	for (auto& pair : m_umapTimers)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapTimers.clear();

	for (auto& pair : m_umapDates)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapDates.clear();

	CManager::Release();
}

HRESULT Engine::CTimerManager::AddDate(const std::wstring& _wstrDateName, const std::shared_ptr<CDate>& _spDate)
{
	if (m_umapDates.contains(_wstrDateName)) { return E_FAIL; }

	m_umapDates.emplace(_wstrDateName, _spDate);
	return S_OK;
}

HRESULT Engine::CTimerManager::RemoveDate(const std::wstring& _wstrDateName)
{
	if (!m_umapDates.contains(_wstrDateName)) { return E_FAIL; }

	m_umapDates[_wstrDateName]->Release();
	m_umapDates[_wstrDateName] = nullptr;

	m_umapDates.erase(_wstrDateName);
	return S_OK;
}

const shared_ptr<CDate> Engine::CTimerManager::FindDate(const std::wstring& _wstrDateName)
{
	if (!m_umapDates.contains(_wstrDateName)) { return nullptr; }

	return m_umapDates[_wstrDateName];
}

HRESULT Engine::CTimerManager::AddTimer(const std::wstring& _wstrTimerName, const std::shared_ptr<CTimer>& _spTimer)
{
	if (m_umapTimers.contains(_wstrTimerName)) { return E_FAIL; }

	m_umapTimers.emplace(_wstrTimerName, _spTimer);
	return S_OK;
}

HRESULT Engine::CTimerManager::RemoveTimer(const std::wstring& _wstrTimerName)
{
	if (!m_umapTimers.contains(_wstrTimerName)) { return E_FAIL; }

	m_umapTimers[_wstrTimerName]->Release();
	m_umapTimers[_wstrTimerName] = nullptr;

	m_umapTimers.erase(_wstrTimerName);
	return S_OK;
}

const shared_ptr<CTimer> Engine::CTimerManager::FindTimer(const std::wstring& _wstrTimerName)
{
	if (!m_umapTimers.contains(_wstrTimerName)) { return nullptr; }

	return m_umapTimers[_wstrTimerName];
}

HRESULT Engine::CTimerManager::StartTimer(const std::wstring& _wstrTimerName)
{
	if (!m_umapTimers.contains(_wstrTimerName)) { return E_FAIL; }
	
	m_umapTimers[_wstrTimerName]->StartTimer();
	return S_OK;
}

HRESULT Engine::CTimerManager::StopTimer(const std::wstring& _wstrTimerName)
{
	if (!m_umapTimers.contains(_wstrTimerName)) { return E_FAIL; }
	
	m_umapTimers[_wstrTimerName]->StopTimer();
	return S_OK;
}

HRESULT Engine::CTimerManager::ResetTimer(const std::wstring& _wstrTimerName)
{
	if (!m_umapTimers.contains(_wstrTimerName)) { return E_FAIL; }

	m_umapTimers[_wstrTimerName]->ResetTimer();
	return S_OK;
}
