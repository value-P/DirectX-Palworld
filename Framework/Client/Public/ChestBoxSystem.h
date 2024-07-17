#pragma once
#include "ContainerSystem.h"

namespace Engine
{
	class CPanel;
	class CImageComponent;
	class CProgressBarComponent;
}

namespace Client
{
	class CItem;

	/********************
		Descriptor
	********************/
	struct FChestBoxSystemDesc final : public FContainerSystemDesc
	{
	};

	class CChestBoxSystem : public CContainerSystem
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CChestBoxSystem() = default;

		virtual ~CChestBoxSystem() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CChestBoxSystem> Create(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc);

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

	/********************
		Methods
	********************/
	public:
		void AddItemToContainer(EArchitectureType _eArchitectureType, int32 _iChestBoxID, const std::shared_ptr<CItem>& _spItem);

		void InsertItemToChestBox(EArchitectureType _eArchitectureType, int32 _iChestBoxID, EItemType _eItemType, int32 _iNumItem);

		void MakeChestBox(EArchitectureType _eArchitectureType, int32 _iChestBoxID);

		virtual void SortItemsInContainer(ESortingType _eSortingType) override;

		virtual void ActiveSlotHover() override;

		void SortItemsInChestBoxs(ESortingType _eSortingType);

		void ShowInventoryItemInInventorySlots();

		void RemoveInventoryItemInInventorySlots();

		void ShowChestBoxItemInChestBoxSlots(EArchitectureType _eArchitectureType, int32 _iChestBoxID);

		void RemoveChestBoxItemInChestBoxSlots(EArchitectureType _eArchitectureType, int32 _iChestBoxID);

		void OpenSpecificChestBox(EArchitectureType _eArchitectureType, int32 _iChestBoxID, int32 _key);

	public:
		void ActiveItemInfoWindow();

		void MoveItemUI();

		void MoveInventorySlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		void MoveInventorySlotUIToEmptyChestBoxSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		void MoveChestBoxSlotUIToEmptyChestBoxSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
	
		void MoveChestBoxSlotUIToEmptyInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);


		void MoveInventorySlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);
								  
		void MoveInventorySlotUIToOccupiedChestBoxSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		void MoveChestBoxSlotUIToOccupiedChestBoxSlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		void MoveChestBoxSlotUIToOccupiedInventorySlotUI(std::pair<int32, int32> _prPreClickedItemSlotIndexs, std::pair<int32, int32> _prCurClickedItemSlotIndexs);

		void ExchangeInventoryItemSlotUI(std::shared_ptr<CPanel> _spChildPanel, const std::wstring& _wstrPreClickedItemSlotTextureName, const std::wstring& _wstrPreClickedItemSlotTextNum, const std::wstring& _wstrCurClickedItemSlotTextureName, const std::wstring& _wstrCurClickedItemSlotTextNum);

		void RemoveInventoryItemSlotUI();

		void UpdateItemInfoWindowPos();

		void UpdateItemInfoFoodWindowPos();

		void CalculateNumWeight();

	/********************
		Data Members
	********************/
	private:
		int32 m_iChestBoxID = -1;
		
		EArchitectureType m_eArchitectureType = EArchitectureType::ENUM_END;

		std::wstring m_wstrInvisibleTextureFileName = L"T_WoodenChest_item_slot_none";
		
	private:
		std::shared_ptr<CPanel> m_spItemInfoParentPanel = nullptr;

		std::shared_ptr<CPanel> m_spItemInfoFoodParentPanel = nullptr;

		std::shared_ptr<CPanel> m_spInventorySlotChildPanel = nullptr;

		std::shared_ptr<CPanel> m_spWoodenChestSlotChildPanel = nullptr;

		std::shared_ptr<CPanel> m_spMetalChestSlotChildPanel = nullptr;

		//std::shared_ptr<CPanel> m_spChestBoxSlotChildPanel = nullptr;

		std::weak_ptr<CImageComponent> m_wpItemSlotHoverImage;

		std::weak_ptr<CImageComponent> m_wpSortHoverImage;

	private:
		//std::unordered_map<int32, std::unordered_map<EItemType, FInventoryItemInfo>> m_umapWoodenChests;

		//std::unordered_map<int32, std::unordered_map<EItemType, FInventoryItemInfo>> m_umapMetalChests;

		//std::unordered_map<int32, std::array<std::array<FInventoryItemInfo, 6>, 2>> m_umapWoodenChestsSlots;

		//std::unordered_map<int32, std::array<std::array<FInventoryItemInfo, 6>, 4>> m_umapMetalChestsSlots;

		std::map<EArchitectureType, std::map<int32, std::map<EItemType, FInventoryItemInfo>>> m_mapChestBoxs;

		std::map<EArchitectureType, std::map<int32, std::vector<std::vector<FInventoryItemInfo>>>> m_mapChestBoxsSlots;
	};
}
