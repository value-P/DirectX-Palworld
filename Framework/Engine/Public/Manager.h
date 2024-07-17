#pragma once

namespace Engine
{
	class ENGINE_DLL CManager : public enable_shared_from_this<CManager>
	{
	/********************
		Events
	********************/
	public:
		template<typename T>
		static std::shared_ptr<T> Create()
		{
			const std::shared_ptr<T> spManager = make_shared<T>();
			return spManager;
		}

		virtual void Release();

	public:
		atomic<bool> m_bInitialized = false;

		atomic<bool> m_bBeginPlayed = false;
	};
}
