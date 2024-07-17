#pragma once
#include "Event.h"

namespace Engine
{
	class CLevel;
}

namespace Client
{
	/********************
		Description
	********************/
	struct FOpenLevelEventDesc : public FEventDesc
	{
		ELevelType m_ePostLevelType = ELevelType::ENUM_END;

		std::shared_ptr<CLevel> m_spPostFrontLevel = nullptr;

		std::shared_ptr<CLevel> m_spPostBackLevel = nullptr;
	};

	class COpenLevelEvent : public CEvent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit COpenLevelEvent() = default;

		virtual ~COpenLevelEvent() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<COpenLevelEvent> Create(const std::shared_ptr<FOpenLevelEventDesc>& _spOpenLevelDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FEventDesc>& _spEventDesc);

		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		virtual void InvokeEvent() override;

	/********************
		Data Members
	********************/
	private:
		ELevelType m_ePostLevelType = ELevelType::ENUM_END;

		std::shared_ptr<CLevel> m_spPostFrontLevel = nullptr;

		std::shared_ptr<CLevel> m_spPostBackLevel = nullptr;
	};
}
