#include "pch.h"
#include "Timer.h"

#include "GameObject.h"

std::shared_ptr<CTimer> Engine::CTimer::Create(const std::shared_ptr<FTimerDesc>& _spTimerDesc)
{
	const std::shared_ptr<CTimer> spTimer = make_shared<CTimer>();
	ENSUREF(spTimer->Initialize(_spTimerDesc), L"Failed to initialize timer.");
	return spTimer;
}

HRESULT Engine::CTimer::Initialize(const std::shared_ptr<FTimerDesc>& _spTimerDesc)
{
	if (!_spTimerDesc->m_spListener) { return E_FAIL; }

	m_fTime = _spTimerDesc->m_fTime;
	m_bLoop = _spTimerDesc->m_bLoop;
	m_wpListener = _spTimerDesc->m_spListener;
	return S_OK;
}

int32 Engine::CTimer::Tick(float _fDeltaSeconds)
{
	if (!m_bPlay) { return 0; }

	m_fCurDeltaSeconds += _fDeltaSeconds;
	m_fSumDeltaSeconds += _fDeltaSeconds;

	if (m_fTime <= m_fCurDeltaSeconds)
	{
		++m_iNumLoops;
		if (!m_bLoop) 
		{
			m_bPlay = false;
		}
		m_fCurDeltaSeconds = 0.0f;

		if (!m_wpListener.expired())
		{
			m_wpListener.lock()->OnTimerOver(shared_from_this());
		}
	}

	return 0;
}

void Engine::CTimer::Release()
{
}

void Engine::CTimer::StartTimer()
{
	m_bPlay = true;
}

void Engine::CTimer::StopTimer()
{
	m_bPlay = false;
}

void Engine::CTimer::ResetTimer()
{
	m_fCurDeltaSeconds = 0.0f;
	m_fSumDeltaSeconds = 0.0f;
	m_bPlay = false;
}
