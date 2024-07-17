#pragma once
#include "ContainerSystem.h"

namespace Engine
{
	class CPanel;
	class CInputManager;
	class CRectComponent;
	class CTextComponent;
	class CImageComponent;
	class CProgressBarComponent;
}

namespace Client
{
	class CItem;
	class CUIManager;

	// test
	class CWeapon;
	class CArmor;
	class CIngredient;

	/********************
		Descriptor
	********************/
	struct FInventorySystemDesc final : public FContainerSystemDesc
	{
	};

	class CInventorySystem final  : public CContainerSystem
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CInventorySystem() = default;

		virtual ~CInventorySystem() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CInventorySystem> Create(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Getter/Setter
	********************/
	public:
		void SetPlayerStatus();

		void SetEquippedWeaponItemSlots(FInventoryItemInfo _tInventoryItemInfo);
		 
		void SetEquippedAccessoryItemSlots(FInventoryItemInfo _tInventoryItemInfo);
		 
		void SetEquippedFoodItemSlots(FInventoryItemInfo _tInventoryItemInfo);

		void SetEquippedHeadItemSlot(FInventoryItemInfo _tInventoryItemInfo) { m_tHeadSlot = _tInventoryItemInfo; }
		 
		void SetEquippedBodyItemSlot(FInventoryItemInfo _tInventoryItemInfo) {  m_tBodySlot = _tInventoryItemInfo; }
		 
		void SetEquippedShieldItemSlot(FInventoryItemInfo _tInventoryItemInfo) {  m_tShieldSlot = _tInventoryItemInfo; }
		 
		void SetEquippedGliderItemSlot(FInventoryItemInfo _tInventoryItemInfo) {  m_tGliderSlot = _tInventoryItemInfo; }

	public:
		std::shared_ptr<CPanel> GetInventoryFeedWhoParentPanel() const { return m_spFeedWhoParentPanel; }

		std::map<EItemType, FInventoryItemInfo>& GetInventorySlotItemInfosRef() { return m_mapInventoryItemInfos; }

		std::array<std::array<FInventoryItemInfo, 6>, 7>& GetInventorySlotItemsRef() { return m_arrInventorySlots; }

		std::array<std::array<FInventoryItemInfo, 6>, 7> GetInventorySlotItems() const { return m_arrInventorySlots; }

		std::array<FInventoryItemInfo, 4> GetEquippedWeaponItemSlots() const { return m_arrWeaponSlots; }

		std::array<FInventoryItemInfo, 2> GetEquippedAccessoryItemSlots() const { return m_arrAccessorySlots; }

		std::array<FInventoryItemInfo, 5> GetEquippedFoodItemSlots() const { return m_arrFoodSlots; }

		FInventoryItemInfo GetEquippedHeadItemSlot() const { return m_tHeadSlot; }

		FInventoryItemInfo GetEquippedBodyItemSlot() const { return m_tBodySlot; }

		FInventoryItemInfo GetEquippedShieldItemSlot() const { return m_tShieldSlot; }

		FInventoryItemInfo GetEquippedGliderItemSlot() const { return m_tGliderSlot; }

		int32 FindInventoryItemTotalNum(EItemType _eItemType) const;

		float GetInventoryTotalWeight() const { return m_fTotalWeight; }

	public:
		void ReduceItemNum(EItemType _eItemType, int32 _iNumItem);

	/********************
		Methods
	********************/
	public:
		virtual void AddItemToContainer(const std::shared_ptr<CItem>& _spItem) override;

		virtual void AddItemToContainer(EItemType _eItemType, int32 _iNumItem) override;

		virtual void SortItemsInContainer(ESortingType _eSortingType) override;

		virtual void ActiveSlotHover() override;

		void DropItemFromInventory(std::shared_ptr<CItem>& _spItem);

		void DiscardItemFromInventory(std::pair<int32, int32> _prPreClickedItemSlotIndexs);

		void DiscardFoodItemFromInventory(std::pair<int32, int32> _prFoodItemIndexs);

	public:
		void MoveItemUI();
		
		void ActiveItemInfoWindow();

		void ActiveFeedWhoWindow();

		void ExchangeInventoryItemSlotUI(const std::wstring& _wstrPreClickedItemSlotTextureName, const std::wstring& _wstrPreClickedItemSlotTextNum, const std::wstring& _wstrCurClickedItemSlotTextureName, const std::wstring& _wstrCurClickedItemSlotTextNum);

		void ShowEquippedItemUI();

		void MoveEquippedItemSlotUIToInventoryItemSlotUI();

		void ShowPartnerPalInfoOnFeedWhoPanel();

		void RemoveInventoryItemSlotUI();

		void ShowInventoryItemInInventorySlots();

		void RemoveInventoryItemInInventorySlots();

		void OpenInventoryWidget(int32 _iKey);

		void CalculateNumWeight();

		void CaculatePlayerHp();


		/* item -> empty slot */ 
		void MoveInventorySlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToEmptyWeaponSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToEmptyAccessorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToEmptyFoodSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToEmptyHeadSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToEmptyBodySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToEmptyShieldSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToEmptyGliderSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// weapon -> empty weapon
		// weapon -> empty inventory
		void MoveWeaponSlotUIToEmptyWeaponSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveWeaponSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// accessory -> empty accessory
		// accessory -> empty inventory
		void MoveAccessorySlotUIToEmptyAccessorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveAccessorySlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// food -> empty food
		// food -> empty inventory
		void MoveFoodSlotUIToEmptyFoodSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveFoodSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// head -> empty head
		// head -> empty inventory
		//void MoveHeadSlotUIToEmptyHeadSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveHeadSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// body -> empty body
		// body -> empty inventory
		//void MoveBodySlotUIToEmptyBodySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveBodySlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// shield -> empty shield
		// shield -> empty inventory
		//void MoveShieldSlotUIToEmptyShieldSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveShieldSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// glider -> empty glider
		// glider -> empty inventory
		//void MoveGliderSlotUIToEmptyGliderSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveGliderSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		/* item -> occupied slot */
		void MoveInventorySlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToOccupiedWeaponSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToOccupiedAccessorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToOccupiedFoodSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToOccupiedHeadSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToOccupiedBodySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToOccupiedShieldSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveInventorySlotUIToOccupiedGliderSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// weapon -> occupied weapon
		// weapon -> occupied inventory
		void MoveWeaponSlotUIToOccupiedWeaponSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveWeaponSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// accessory -> occupied accessory
		// accessory -> occupied inventory
		void MoveAccessorySlotUIToOccupiedAccessorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveAccessorySlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// food -> occupied food
		// food -> occupied inventory
		void MoveFoodSlotUIToOccupiedFoodSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
		void MoveFoodSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// head -> occupied head
		// head -> occupied inventory
		void MoveHeadSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// body -> occupied body
		// body -> occupied inventory
		void MoveBodySlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// shield -> occupied shield
		// shield -> occupied inventory
		void MoveShieldSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		// glider -> occupied glider
		// glider -> occupied inventory
		void MoveGliderSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);



	public:
		void UpdateItemInfoWindowPos();

		void UpdateItemInfoFoodWindowPos();

		void PartnerPalInfoOnFeedWhoWindow(FPalBoxPalInfo _tPalBoxPalInfo);
	
	/********************
		Data Members
	********************/
	private:
		float m_fTotalWeight = 0.0f;
		
		std::wstring m_wstrInvisibleTextureFileName = L"T_Inventory_item_slot_none"; // 투명 텍스처 이름입니다.
	
	private: // 구조체
		FInventoryItemInfo m_tHeadSlot = {};

		FInventoryItemInfo m_tBodySlot = {};

		FInventoryItemInfo m_tShieldSlot = {};

		FInventoryItemInfo m_tGliderSlot = {};

	private: // 컨테이너
		std::map<EItemType, FInventoryItemInfo> m_mapInventoryItemInfos; // 빠른 탐색을 위한 맵 컨테이너입니다.

		std::unordered_map<EItemType, int32> m_umapInventoryNumItem;

		std::vector<std::weak_ptr<CRectComponent>> m_vecFeedWhoDetailInfoPanel;

		std::array<std::array<FInventoryItemInfo, 6>, 7> m_arrInventorySlots = {};

		std::array<FInventoryItemInfo, 4> m_arrWeaponSlots = {};

		std::array<FInventoryItemInfo, 2> m_arrAccessorySlots = {};

		std::array<FInventoryItemInfo, 5> m_arrFoodSlots = {};

	private: // 스마트 포인터
		std::shared_ptr<CPanel> m_spItemInfoParentPanel = nullptr;

		std::shared_ptr<CPanel> m_spItemInfoFoodParentPanel = nullptr;

		std::shared_ptr<CPanel> m_spFeedWhoParentPanel = nullptr;

		std::shared_ptr<CPanel> m_spFeedWhoChildPanel = nullptr;

		std::shared_ptr<CPanel> m_spFeedWhoDetailInfoPanel = nullptr;

		std::weak_ptr<CRectComponent> m_wpSortHoverImage;

		std::weak_ptr<CRectComponent> m_wpDropDiscardHoverImage;

		std::weak_ptr<CRectComponent> m_wpItemSlotHoverImage;

		std::weak_ptr<CRectComponent> m_wpFeedWhoHoverImage;

		std::weak_ptr<CProgressBarComponent> m_wpPlayerHpBarProgressBarxComponent;

		std::weak_ptr<CTextComponent> m_wpPlayerHpBarNumTextComponent;

		std::weak_ptr<CTextComponent> m_wpPlayerHpNumTextComponent;

		std::weak_ptr<CProgressBarComponent> m_wpInventoryWeightBarProgressBarxComponent;

		std::weak_ptr<CTextComponent> m_wpInventoryWeightBarNumTextComponent;

		std::weak_ptr<CTextComponent> m_wpInventoryWeightNumTextComponent;

	};
}


