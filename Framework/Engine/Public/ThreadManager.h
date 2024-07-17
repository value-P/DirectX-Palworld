#pragma once
#include "Manager.h"

namespace Engine
{
	class ENGINE_DLL CThreadManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CThreadManager() = default;

		virtual ~CThreadManager() = default;

		CThreadManager(const CThreadManager& _rhs) = delete;

		CThreadManager(CThreadManager&& _rhs) = delete;

		CThreadManager& operator=(const CThreadManager& _rhs) = delete;

		CThreadManager& operator=(CThreadManager&& _rhs) = delete;

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		virtual void Release() override;
		
	/********************
		Getter/Setter
	********************/
	public:
		int32 GetNumThreads() const noexcept
		{
			return static_cast<int32>(m_vecThreads.size());
		}

		int32 GetNumWorkingThreads() const noexcept
		{
			return m_iNumWorkingThreads;
		}

		int32 GetNumQueuedTasks()
		{
			std::scoped_lock slLock(m_mtMutex);
			return static_cast<int32>(m_qTasks.size());
		}

		void ClearQueue()
		{
			std::scoped_lock slLock(m_mtMutex);
			m_qTasks = std::queue<std::function<void()>>();
		}
		
	/********************
		Methods
	********************/
	public:
		template <typename T, typename... Args>
		std::future<std::invoke_result_t<T, Args...>> AddTask(T&& _fnTask, Args &&..._args)
		{
			using ReturnType = std::invoke_result_t<T, Args...>;
			std::future<ReturnType> ftResult;

			{
				std::scoped_lock slLock(m_mtMutex);

				auto spPromise = std::make_shared<std::promise<ReturnType>>();
				ftResult = spPromise->get_future();

				m_qTasks.emplace([p = std::move(spPromise), fn = std::forward<T>(_fnTask), ... args = std::forward<Args>(_args)]()
					{
						if constexpr (std::is_same<ReturnType, void>::value)
						{
							std::invoke(fn, args...);
							p->set_value();
						}
						else
						{
							p->set_value(std::invoke(fn, args...));
						}
					}
				);
			}

			m_cvConditionVariable.notify_one();
			return ftResult;
		}


	/******************
		Data Members
	******************/
	private:
		// 풀에 있는 스레드들
		std::vector<std::jthread> m_vecThreads;

		// 큐에 있는 작업들
		std::queue<std::function<void()>> m_qTasks;

		// 작업 큐를 동기화할 뮤텍스
		std::mutex m_mtMutex;

		// 조건 변수
		std::condition_variable_any m_cvConditionVariable;

		// 작업하고 있는 스레드 수
		std::atomic<int32> m_iNumWorkingThreads;

		// 전체 스레드 수
		const int32 m_iNumThreads = 8;
	};
}
