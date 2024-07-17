#pragma once

namespace Engine
{
	class CEngineModule;
}

namespace Client
{
	class CAsyncManager;
	class CBuildManager;
	class CEffectManager;
	class CGameManager;
	class CNotifyManager;
	class CTreeManager;
	class CUIManager;
	class CPoolManager;

	class CClientModule final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CClientModule)

	/********************
		Events
	********************/
	public:
		HRESULT Initialize(int32 _iLevelType);

		HRESULT BeginPlay(int32 _iLevelType);

		HRESULT Initialize();

		HRESULT BeginPlay();

		int32 PreTick(float _fDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);

		int32 PostTick(float _fDeltaSeconds);

		void Render();

		void Release();
		
	/********************
		Getter/Setter
	********************/
	public:
		std::shared_ptr<CGameManager> GetGameManager() const { return m_spGameManager; }

		std::shared_ptr<CUIManager> GetUIManager() const { return m_spUIManager; }

		std::shared_ptr<CTreeManager> GetTreeManager() const { return m_spTreeManager; }

		std::shared_ptr<CBuildManager> GetBuildManager() const { return m_spBuildManager; }

		std::shared_ptr<CEffectManager> GetEffectManager() const { return m_spEffectManager; }

		std::shared_ptr<CNotifyManager> GetNotifyManager() const { return m_spNotifyManager; }

		// std::shared_ptr<CPoolManager> GetPoolManager() const { return m_spPoolManager; }

	public:
		void SetCurLevelType(ELevelType _eLevelType) { m_eCurLevelType.store(_eLevelType); }

		ELevelType GetCurLevelType() const { return m_eCurLevelType; }

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CAsyncManager> m_spAsyncManager = nullptr;
		
		std::shared_ptr<CTreeManager> m_spTreeManager = nullptr;
	
		std::shared_ptr<CGameManager> m_spGameManager = nullptr;

		std::shared_ptr<CUIManager> m_spUIManager = nullptr;

		std::shared_ptr<CBuildManager> m_spBuildManager = nullptr;

		std::shared_ptr<CEffectManager> m_spEffectManager = nullptr;

		std::shared_ptr<CNotifyManager> m_spNotifyManager = nullptr;

		// std::shared_ptr<CPoolManager> m_spPoolManager = nullptr;

	private:
		atomic<ELevelType> m_eCurLevelType = ELevelType::PRE_LOAD;
	};
}
