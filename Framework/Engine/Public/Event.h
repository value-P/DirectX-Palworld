#pragma once

namespace Engine
{
	/********************
		Description
	********************/
	struct FEventDesc : public enable_shared_from_this<FEventDesc>
	{
	public:
		EEventType m_eEventType = EEventType::ENUM_END;

		EEventInvokeType m_eEventInvokeType = EEventInvokeType::ENUM_END;
	};

	class ENGINE_DLL CEvent abstract : public enable_shared_from_this<CEvent>
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CEvent() = default;

		virtual ~CEvent() = default;
		
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FEventDesc>& _spEventDesc);

		virtual void Release();
		
	/********************
		Methods
	********************/
	public:
		virtual void InvokeEvent() = 0;
		
	/********************
		Getter/Setter
	********************/
	public:
		EEventType GetEventType() const { return m_eEventType; }

		EEventInvokeType GetEventInvokeType() const { return m_eEventInvokeType; }

	/********************
		Data Members
	********************/
	private:
		EEventType m_eEventType = EEventType::ENUM_END;

		EEventInvokeType m_eEventInvokeType = EEventInvokeType::ENUM_END;
	};
}
