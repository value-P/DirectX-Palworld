#include "pch.h"
#include "Frame.h"

shared_ptr<CFrame> Engine::CFrame::Create(EFrameLoopType _eFrameLoopType)
{
	const shared_ptr<CFrame> spTimer = make_shared<CFrame>();
	ENSUREF(spTimer->Initialize(_eFrameLoopType), L"Failed to initialize frame.");
	return spTimer;
}

HRESULT Engine::CFrame::Initialize(EFrameLoopType _eFrameLoopType)
{
	m_eFrameLoopType = _eFrameLoopType;
	QueryPerformanceCounter(&m_llCurTime);
	QueryPerformanceCounter(&m_llFixTime);
	QueryPerformanceCounter(&m_llLastTime);

	// 1초당 틱 수를 구합니다.
	QueryPerformanceFrequency(&m_llTickPerSecond);
	return S_OK;
}

float Engine::CFrame::Tick()
{
	// 현재 틱 수를 구합니다.
	QueryPerformanceCounter(&m_llCurTime);

	if (m_llCurTime.QuadPart - m_llFixTime.QuadPart >= m_llTickPerSecond.QuadPart)
	{
		// 1초당 틱 수를 다시 구합니다.
		QueryPerformanceFrequency(&m_llTickPerSecond);
		m_llFixTime = m_llCurTime;
	}

	// 현재 틱 수에서 이전 틱 수를 뺀 틱을 구하고, 이 틱만큼의 초(second)를 구합니다.
	m_fDeltaSeconds = static_cast<float>(m_llCurTime.QuadPart - m_llLastTime.QuadPart) / m_llTickPerSecond.QuadPart;

	// 현재 틱 수를 이전 틱 수에 저장합니다.
	m_llLastTime = m_llCurTime;

	return m_fDeltaSeconds;
}

void Engine::CFrame::Release()
{
}
