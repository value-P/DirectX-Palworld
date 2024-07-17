#pragma once
#include "Manager.h"

namespace Engine
{
	class CTimer;
	class CDate;

	class ENGINE_DLL CTimerManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTimerManager() = default;

		virtual ~CTimerManager() = default;

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		int32 Tick(float _fDeltaSeconds);

		virtual void Release() override;
	
	/********************
		Methods
	********************/
	public:
		float GetWorldDeltaSeconds() const { return m_fWorldDeltaSeconds; }
		
		HRESULT AddDate(const std::wstring& _wstrDateName, const std::shared_ptr<CDate>& _spDate);

		HRESULT RemoveDate(const std::wstring& _wstrDateName);

		const shared_ptr<CDate> FindDate(const std::wstring& _wstrDateName);

		HRESULT AddTimer(const std::wstring& _wstrTimerName, const std::shared_ptr<CTimer>& _spTimer);

		HRESULT RemoveTimer(const std::wstring& _wstrTimerName);

		const shared_ptr<CTimer> FindTimer(const std::wstring& _wstrTimerName);

	public:
		HRESULT StartTimer(const std::wstring& _wstrTimerName);

		HRESULT StopTimer(const std::wstring& _wstrTimerName);

		HRESULT ResetTimer(const std::wstring& _wstrTimerName);

	/********************
		Data Members
	********************/
	private:
		std::unordered_map<std::wstring, std::shared_ptr<CTimer>> m_umapTimers;

		std::unordered_map<std::wstring, std::shared_ptr<CDate>> m_umapDates;

		float m_fWorldDeltaSeconds = 0.0f;
	};
}
