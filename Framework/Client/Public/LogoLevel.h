#pragma once
#include "Level.h"

namespace Engine
{
	class CInputManager;
	class CEventManager;
	class CButtonComponent;
}

namespace Client
{
	class CUIManager;

	class CLogoLevel final : public CLevel
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CLogoLevel() = default;

		virtual ~CLogoLevel() = default;

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
		
	/********************
		Methods
	********************/
	public:
		void ClickStartButtonEvent();

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CInputManager> m_wpInputManager;

		std::weak_ptr<CEventManager> m_wpEventManager;

		std::weak_ptr<CUIManager> m_wpUIManager;

		std::weak_ptr<CButtonComponent> m_wpStartButton;
	};
}
