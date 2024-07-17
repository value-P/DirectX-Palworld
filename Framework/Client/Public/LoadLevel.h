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
		// ���
		std::weak_ptr<CClientModule> m_wpClientModule;

		std::weak_ptr<CEngineModule> m_wpEngineModule;

		// �Ŵ���
		std::weak_ptr<CThreadManager> m_wpThreadManager;

		std::weak_ptr<CInputManager> m_wpInputManager;

		std::weak_ptr<CEventManager> m_wpEventManager;

	private:
		// ���ҽ� ǻ��
		std::vector<std::future<HRESULT>> m_vecLoadResourceFutures;

		std::vector<bool> m_vecLoadResourceCompleted;

		// �ʱ�ȭ ǻ��
		std::vector<std::future<HRESULT>> m_vecInitLevelFutures;

		std::vector<bool> m_vecInitLevelCompleted;

		// �ε��� ���� �ε���
		int32 m_iPostLevelType = -1;

		// �ε��� ����
		std::shared_ptr<CLevel> m_spPostFrontLevel = nullptr;
	};
}
