#pragma once

namespace Engine
{
	class CSoundManager;
	class CInputManager;
	class CPanel;
	class CImageComponent;
}

namespace Client
{
	class CUIManager;

	struct FPalCircleUIInfo final
	{
		EMonsterInteractType eMonsterInteractType = EMonsterInteractType::ENUM_END;
		float fMinAngle = 0.0f;
		float fMaxAngle = 0.0f;
	};

	struct FPlayerCircleUIInfo final
	{
		EEmoteType eEmoteType = EEmoteType::ENUM_END;
		float fMinAngle = 0.0f;
		float fMaxAngle = 0.0f;
	};

	/********************
		Descriptor
	********************/
	struct FPalInteractionSystemDesc final
	{
	};

	enum class EInteractionSystemUIType
	{
		EEmote,
		EPalInteract,
	};

	class CPalInteractionSystem final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPalInteractionSystem() = default;

		virtual ~CPalInteractionSystem() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPalInteractionSystem> Create(const std::shared_ptr<FPalInteractionSystemDesc>& _spPalInteractionSystemDesc);

		HRESULT Initialize(const std::shared_ptr<FPalInteractionSystemDesc>& _spPalInteractionSystemDesc);

		HRESULT BeginPlay();

		int32 PreTick(float _fDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);
		
		int32 PostTick(float _fDeltaSeconds);

		HRESULT EndPlay();

		void Release();

	/********************
		Getter/Setter
	********************/
	public:
		shared_ptr<CPanel> GetPalInteractionPanel() { return m_spPalInteractionUI; }

		EInteractionSystemUIType GetCurrentUIType() { return m_eCurSystemUIType; }

		bool IsInteracUIActivate();

	/********************
		Methods
	********************/
	public:
		void ToggleInteractUI();
		void ToggleEmoteUI();

		int32 InteractUIClick();

	private:
		void AddvecIndex();

		void AddPlayerIndex();

		bool IsInAngle(float _fMinAngle, float _fMaxAngle, float _fDegreeAngle);

	/********************
		Data Members
	********************/
	private:
		std::vector<pair<int32, FPalCircleUIInfo>> m_vecIndex;

		std::vector<pair<int32, FPlayerCircleUIInfo>> m_vecPlayerIndex;

		int32 m_iCurIndex = -1;
		int32 m_iPreIndex = 0;

		EInteractionSystemUIType m_eCurSystemUIType = EInteractionSystemUIType::EPalInteract;

	private:
		std::wstring m_wstrPalImageName = L"T_Pal_NameImage";
		std::wstring m_wstrPlayerImageName = L"T_Player_NameImage";

	private:
		std::shared_ptr<CPanel> m_spPalInteractionUI = nullptr;

		std::shared_ptr<CImageComponent> m_spHoverImage = nullptr;

		std::shared_ptr<CImageComponent> m_spNameImage = nullptr;

	private:
		std::weak_ptr<CUIManager> m_wpUIManager;

		std::weak_ptr<CInputManager> m_wpInputManager;

		std::weak_ptr<CSoundManager> m_wpSoundManager;
	};
}