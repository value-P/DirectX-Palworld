#pragma once
#include "Level.h"

namespace Engine
{
	class CEngineModule;
	class CThreadManager;
	class CInputManager;
	class CEventManager;
}

namespace Client
{
	class CClientModule;

	class CLoadLevel final : public CLevel
	{
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize() override;

		virtual HRESULT BeginPlay() override;
		
		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual void Release() override;

		virtual void LoadResource(int32 _iLevelType) override;

	/********************
		Methods
	********************/
	private:
		void CheckLoadResourceFuture();

		void CheckInitLevelFuture();

	private:
		HRESULT LoadSampleLevelResource();

		HRESULT LoadIslandLevelResource();

		HRESULT LoadTowerLevelResource();

		HRESULT LoadLogoLevelResource();

		HRESULT LoadCustomizeLevelResource();

	/********************
		Data Members
	********************/
	private:
		// 모듈
		std::weak_ptr<CClientModule> m_wpClientModule;

		std::weak_ptr<CEngineModule> m_wpEngineModule;

		// 매니저
		std::weak_ptr<CThreadManager> m_wpThreadManager;

		std::weak_ptr<CInputManager> m_wpInputManager;

		std::weak_ptr<CEventManager> m_wpEventManager;

	private:
		// 리소스 퓨쳐
		std::vector<std::future<HRESULT>> m_vecLoadResourceFutures;

		std::vector<bool> m_vecLoadResourceCompleted;

		// 초기화 퓨쳐
		std::vector<std::future<HRESULT>> m_vecInitLevelFutures;

		std::vector<bool> m_vecInitLevelCompleted;

		// 로드할 레벨 인덱스
		int32 m_iPostLevelType = -1;

		// 로드할 레벨
		std::shared_ptr<CLevel> m_spPostFrontLevel = nullptr;
	};
}
