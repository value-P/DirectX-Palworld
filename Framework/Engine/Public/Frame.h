#pragma once

namespace Engine
{
	class ENGINE_DLL CFrame final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CFrame() = default;

		virtual ~CFrame() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CFrame> Create(EFrameLoopType _eFrameLoopType);

		HRESULT Initialize(EFrameLoopType _eFrameLoopType);

		float Tick();

		void Release();

	/**********************
		Getter/Setter
	**********************/
	public:
		float GetDeltaSeconds() const { return m_fDeltaSeconds; }

	/********************
		Data Members
	********************/
	private:
		LARGE_INTEGER m_llCurTime{};

		LARGE_INTEGER m_llFixTime{};

		LARGE_INTEGER m_llLastTime{};

		LARGE_INTEGER m_llTickPerSecond{};

	private:
		// ������ �̸�
		EFrameLoopType	m_eFrameLoopType = EFrameLoopType::ENUM_END;

		// ���� ��Ÿ ������
		float m_fDeltaSeconds = 0.0f;
	};
}
