#pragma once

namespace Engine
{
	// manager
	class CInputManager;
	class CSoundManager;

	// gameobject
	class CPanel;

	// component
	class CImageComponent;
}

namespace Client
{
	class CGameManager;
	class CUIManager;
	class CPlayer;
	class CItem;

	/********************
		Descriptor
	********************/
	struct FContainerSystemDesc
	{
	};

	class CContainerSystem
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CContainerSystem() = default;

		virtual ~CContainerSystem() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc);

		virtual HRESULT BeginPlay();

		virtual int32 PreTick(float _fDeltaSeconds);

		virtual int32 Tick(float _fDeltaSeconds);

		virtual int32 PostTick(float _fDeltaSeconds);

		virtual HRESULT EndPlay();

		virtual void Release();

	/********************
		Getter/Setter
	********************/
	public:
		std::shared_ptr<CPanel> GetContainerParentPanel() const { return m_spContainerParentPanel; }

		std::shared_ptr<CPanel> GetContainerChildPanel() const { return m_spContainerChildPanel; }

		void SetContainerParentPanel(std::shared_ptr<CPanel>& _spContainerParentPanel) { m_spContainerParentPanel = _spContainerParentPanel; }

		void SetContainerChildPanel(std::shared_ptr<CPanel>& _spContainerChildPanel) { m_spContainerChildPanel = _spContainerChildPanel; }

		void SetMouseChildPanel(std::shared_ptr<CPanel>& _spMouseChildPanel) { m_spMouseChildPanel = _spMouseChildPanel; }

		void SetItemInfoChildPanel(std::shared_ptr<CPanel>& _spItemInfoChildPanel) { m_spItemInfoChildPanel = _spItemInfoChildPanel; }

		void SetFeedWhoChildPanel(std::shared_ptr<CPanel>& _spFeedWhoChildPanel) { m_spFeedWhoChildPanel = _spFeedWhoChildPanel; }

	/********************
		Methods
	********************/
	public:
		virtual void AddItemToContainer(const std::shared_ptr<CItem>& _spItem);

		virtual void AddItemToContainer(EItemType _eItemType, int32 _iNumItem);

		virtual void SortItemsInContainer(ESortingType _eSortingType);

		virtual void ActiveSlotHover();

	public:
		void OpenContainerWidget(int32 _iKey);

		void UpdateCurMouseSlotPos();

	public:
		// Function
		std::pair<int32, int32> ExtractLastChar(const std::wstring& _wstrComponentName, int32 _iNumberWantToExtract);

		std::wstring ExceptLastChar(const std::wstring& _wstrComponentName, int32 _iNumberWantToExtract);
	

	/********************
		Data Members
	********************/
	protected:
		bool m_bOpenContainerWidget = false;

		wstring m_wstrPreClickedComponentName = L""; // 이전 선택한 슬롯의 컴포넌트 이름을 저장합니다.

		wstring m_wstrPreClickedTextureName = L""; // 이전 선택한 슬롯의 텍스처 이름을 저장합니다.


	protected:
		// Manager
		std::shared_ptr<CGameManager> m_spGameManager = nullptr;

		std::shared_ptr<CInputManager> m_spInputManager = nullptr;

		std::shared_ptr<CUIManager> m_spUIManager = nullptr;

		std::weak_ptr<CSoundManager> m_wpSoundManager;

	protected:
		std::shared_ptr<CPanel> m_spContainerParentPanel = nullptr;

		std::shared_ptr<CPanel> m_spContainerChildPanel = nullptr;

		std::shared_ptr<CPanel> m_spMouseChildPanel = nullptr;

		std::shared_ptr<CPanel> m_spItemInfoChildPanel = nullptr;

		std::shared_ptr<CPanel> m_spFeedWhoChildPanel = nullptr;


		std::weak_ptr<CPlayer> m_wpPlayer;

		std::weak_ptr<CImageComponent> m_wpCurMouseSlotComponent;

		std::weak_ptr<CImageComponent> m_wpCurHoverSlotComponent;

		std::weak_ptr<CImageComponent> m_wpCurLeftClickedSlotComponent;

		std::weak_ptr<CImageComponent> m_wpPreLeftClickedSlotComponent;

		std::weak_ptr<CImageComponent> m_wpPrePreLeftClickedSlotComponent;

		std::weak_ptr<CImageComponent> m_wpCurRightClickedSlotComponent;		
	};
}