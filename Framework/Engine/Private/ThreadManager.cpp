#include "pch.h"
#include "ThreadManager.h"

#include "EngineModule.h"
#include "PipelineManager.h"

HRESULT Engine::CThreadManager::Initialize()
{
	auto fnThread = [this](std::stop_token _stToken)
		{
			while (true)
			{
				std::unique_lock<std::mutex> ulLock(m_mtMutex);

				/* Wait for either a task to be added, or a stop to be requested */
				m_cvConditionVariable.wait(ulLock, _stToken, [this]() { return !m_qTasks.empty(); });

				if (_stToken.stop_requested() && m_qTasks.empty()) { return; }

				std::function<void()> fnTask = m_qTasks.front();
				m_qTasks.pop();
				ulLock.unlock();

				m_iNumWorkingThreads++;
				fnTask();
				m_iNumWorkingThreads--;
			}
		};

	for (int32 iIndex = 0; iIndex < m_iNumThreads; ++iIndex)
	{
		m_vecThreads.emplace_back(std::jthread(fnThread));
	}
	return S_OK;
}

void Engine::CThreadManager::Release()
{
	m_vecThreads.clear();
}