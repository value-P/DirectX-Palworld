#pragma once
#include "Level.h"

namespace Engine
{
	class CCamera;
	class CPanel;
	class CButtonComponent;
	class CInputManager;
}

namespace Client
{
	class CUIManager;
	class CCustomizeObject;

    class CCustomizeLevel final : public CLevel
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
		
	private:
		void StartGameLevel();

		void RotatePlayer();

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CUIManager> m_wpUIManager;
		std::weak_ptr<CInputManager> m_wpInputManager;

	private:
		std::shared_ptr<CCamera> m_spCamera = nullptr;
		
		std::shared_ptr<CCustomizeObject> m_spCustomObject = nullptr;

		std::shared_ptr<CPanel> m_spCustomizeBackgroundUI = nullptr;
		std::weak_ptr<CButtonComponent> m_wpGameStartButton;
    };
}


