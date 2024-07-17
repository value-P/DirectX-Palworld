#pragma once

namespace Engine
{
	struct FDateDesc
	{
		std::wstring m_wstrDateName = L"";

		float m_fDeltaSecondsPerDay = 86'400.0f;
	};

	class ENGINE_DLL CDate final : public enable_shared_from_this<CDate>
	{
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDate> Create(const std::shared_ptr<FDateDesc>& _spDateDesc);

		HRESULT Initialize(const std::shared_ptr<FDateDesc>& _spDateDesc);

		void Tick(float _fDeltaSeconds);

		void Release();
		
	/********************
		Methods
	********************/
	public:
		void GetTime(int32& _iDay, int32& _iHour, int32& _iMinute, int32& _iSecond) const;

		int32 GetHour24() const;

		int32 GetMinute() const;

		void SetDateTimeScale(float _fTimeScale) { m_fDateTimeScale = _fTimeScale; }

		bool IsDay() const;

		bool IsNight() const;

	/********************
		Data Members
	********************/
	private:
		std::wstring m_wstrDateName = L"";

		// 06:00:00 ~ 17:59:59∞° ≥∑, 18:00:00 ~ 05:59:59∞° π„¿‘¥œ¥Ÿ.
		bool m_bDay = true;

		float m_fSumDeltaSeconds = 0.0f;

		float m_fDeltaSecondsPerDay = 1.0f;

		float m_fDeltaSecondsPerHour = 1.0f / 24.0f;

		float m_fDeltaSecondsPerMinute = 1.0f / 24.0f / 60.0f;

		float m_fDeltaSecondsPerSecond = 1.0f / 24.0f / 60.0f / 60.0f;

		float m_fDateTimeScale = 1.f;
	};
}
