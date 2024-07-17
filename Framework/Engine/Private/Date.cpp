#include "pch.h"
#include "Date.h"

std::shared_ptr<CDate> Engine::CDate::Create(const std::shared_ptr<FDateDesc>& _spDateDesc)
{
	const std::shared_ptr<CDate> spDate = make_shared<CDate>();
	ENSUREF(spDate->Initialize(_spDateDesc), L"Failed to initialize date.");
	return spDate;
}

HRESULT Engine::CDate::Initialize(const std::shared_ptr<FDateDesc>& _spDateDesc)
{
	m_wstrDateName = _spDateDesc->m_wstrDateName;
	m_fDeltaSecondsPerDay = _spDateDesc->m_fDeltaSecondsPerDay;
	m_fDeltaSecondsPerHour = m_fDeltaSecondsPerDay / 24.0f;
	m_fDeltaSecondsPerMinute = m_fDeltaSecondsPerHour / 60.0f;
	m_fDeltaSecondsPerSecond = m_fDeltaSecondsPerMinute / 60.0f;
	return S_OK;
}

void Engine::CDate::Tick(float _fDeltaSeconds)
{
	m_fSumDeltaSeconds += _fDeltaSeconds * m_fDateTimeScale;
}

void Engine::CDate::Release()
{
}

void Engine::CDate::GetTime(int32& _iDay, int32& _iHour, int32& _iMinute, int32& _iSecond) const
{
	// 일을 구합니다.
	float fLocalDeltaSeconds = m_fSumDeltaSeconds;
	_iDay = static_cast<int32>(fLocalDeltaSeconds / m_fDeltaSecondsPerDay);

	// 시간을 구합니다.
	fLocalDeltaSeconds -= (m_fDeltaSecondsPerDay * _iDay);
	_iHour = static_cast<int32>(fLocalDeltaSeconds / m_fDeltaSecondsPerHour);

	// 분을 구합니다.
	fLocalDeltaSeconds -= (m_fDeltaSecondsPerHour * _iHour);
	_iMinute = static_cast<int32>(fLocalDeltaSeconds / m_fDeltaSecondsPerMinute);

	// 초를 구합니다.
	fLocalDeltaSeconds -= (m_fDeltaSecondsPerMinute * _iMinute);
	_iSecond = static_cast<int32>(fLocalDeltaSeconds / m_fDeltaSecondsPerSecond);
}

int32 Engine::CDate::GetHour24() const
{
	// 일을 구합니다.
	float fLocalDeltaSeconds = m_fSumDeltaSeconds;
	int32 iDay = static_cast<int32>(fLocalDeltaSeconds / m_fDeltaSecondsPerDay);

	// 시간을 구합니다.
	fLocalDeltaSeconds -= (m_fDeltaSecondsPerDay * iDay);
	return static_cast<int32>(fLocalDeltaSeconds / m_fDeltaSecondsPerHour);
}

int32 Engine::CDate::GetMinute() const
{
	// 일을 구합니다.
	float fLocalDeltaSeconds = m_fSumDeltaSeconds;
	int32 iDay = static_cast<int32>(fLocalDeltaSeconds / m_fDeltaSecondsPerDay);

	// 시간을 구합니다.
	fLocalDeltaSeconds -= (m_fDeltaSecondsPerDay * iDay);
	int32 iHour = static_cast<int32>(fLocalDeltaSeconds / m_fDeltaSecondsPerHour);

	// 분을 구합니다.
	fLocalDeltaSeconds -= (m_fDeltaSecondsPerHour * iHour);
	return static_cast<int32>(fLocalDeltaSeconds / m_fDeltaSecondsPerMinute);
}

bool Engine::CDate::IsDay() const
{
	int32 iHour = GetHour24();

    return (0 <= iHour && iHour < 12);
}

bool Engine::CDate::IsNight() const
{
	return !IsDay();
}
