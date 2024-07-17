#pragma once
#include "Manager.h"

namespace Engine
{
	class CEvent;

	class ENGINE_DLL CEventManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CEventManager() = default;

		virtual ~CEventManager() = default;

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	public:
		HRESULT AddEvent(const std::shared_ptr<CEvent>& _spEvent);

		HRESULT RemoveEvent(EEventType _eEventType);

		void InvockEvent(EEventInvokeType _eEventInvokeType);

		void ClearEvent();

	/********************
		Data Members
	********************/
	private:
		std::vector<std::shared_ptr<CEvent>> m_vecForwardEvents;

		std::vector<std::shared_ptr<CEvent>> m_vecDeferredEvents;
	};
}
