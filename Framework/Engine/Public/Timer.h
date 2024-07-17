#pragma once

namespace Engine
{
	class CGameObject;

	struct FTimerDesc
	{
		// Ÿ�̸� �ð�
		float m_fTime = 0.0f;

		// �ݺ� ����
		bool m_bLoop = false;

		// �˶��� ��� ���� ���� ������Ʈ
		std::shared_ptr<CGameObject> m_spListener = nullptr;
	};

	class ENGINE_DLL CTimer final : public enable_shared_from_this<CTimer>
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTimer() = default;

		virtual ~CTimer() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTimer> Create(const std::shared_ptr<FTimerDesc>& _spTimerDesc);

		HRESULT Initialize(const std::shared_ptr<FTimerDesc>& _spTimerDesc);

		int32 Tick(float _fDeltaSeconds);

		void Release();
		
	/********************
		Methods
	********************/
	public:
		void StartTimer();

		void StopTimer();

		void ResetTimer();

	/********************
		Data Members
	********************/
	private:
		std::wstring m_wstrTimerName = L"";

		float m_fTime = 0.0f;

		float m_fSumDeltaSeconds = 0.0f;

		float m_fCurDeltaSeconds = 0.0f;

		bool m_bLoop = false;

		bool m_bPlay = false;

		int32 m_iNumLoops = 0;

		std::weak_ptr<CGameObject> m_wpListener;
	};
}
