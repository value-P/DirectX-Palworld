#pragma once
#include "WindowBase.h"

namespace ToolAnimation
{
	class CNotifyTab : public CWindowBase
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CNotifyTab() = default;
		virtual ~CNotifyTab() = default;

	/********************
		Events
	********************/
	public:
		virtual void Initialize() override;
		virtual void Tick(float _fDeltaSeconds) override;
		virtual void Release() override;

	/********************
		Methods
	********************/
	public:

	private:
		void NotifyTab();
			void NotifyList();
			void NotifyDelete();
		void NotifyDetail();
		
	private:
		void PlayNotify();

	/********************
		Getter/Setter
	********************/
	public:

	/********************
		Data Members
	********************/
	private:
		FToolNotifyData m_tNotifyData;
		int32 m_iSelectedNotifyIndex = -1;

		list<int32> m_lstPlayedNotifyID;
	};
}

