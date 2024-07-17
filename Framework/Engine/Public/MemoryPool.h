#pragma once

namespace Engine
{
	template<typename T, size_t pool_size>
	class ENGINE_DLL CMemoryPool
	{
	public:
		CMemoryPool() { m_pObjects = new T[pool_size]; }

		~CMemoryPool() { delete[] m_pObjects; }

	public:
		T* Allocate()
		{
			if (m_lstAvailablePtr.size() == 0)
				return nullptr;

			T* pReturn = m_lstAvailablePtr.front();
			m_lstAvailablePtr.pop_front();

			return pReturn;
		}

		void DeAllocate(T* _pObject)
		{
			CHECKF(m_lstAvailablePtr != pool_size, L"Memory Pool is Full, DeAllocate Failed");

			m_lstAvailablePtr.push_back(_pObject);
		}

	private:
		T* m_pObjects = nullptr;

		std::list<T*> m_lstAvailablePtr;
	};
}
