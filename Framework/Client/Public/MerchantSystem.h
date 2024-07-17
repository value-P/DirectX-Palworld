#pragma once
#include "ContainerSystem.h"

namespace Engine
{
	class CPanel;
	class CRectComponent; // 추후 ButtonComponent로 바뀔 수도 있습니다.
}

namespace Client
{
	class CItem;

	/********************
		Descriptor
	********************/
	struct FMerchantSystemDesc final : public FContainerSystemDesc
	{
	};

	class CMerchantSystem final : public CContainerSystem
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CMerchantSystem() = default;

		virtual ~CMerchantSystem() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CMerchantSystem> Create(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc);

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
		virtual void AddItemToContainer(const std::shared_ptr<CItem>& _spItem) override;

		//virtual void ChangeItemIndexInContainer(std::pair<int32, int32> _prCurClickedItemSlotIndexs, FInventoryItemInfo _tPreClickedInventoryItemInfo) override;

		virtual void SortItemsInContainer(ESortingType _eSortingType) override;

		virtual void ActiveSlotHover() override;

		/********************
			Data Members
		********************/
	protected:
		std::vector<std::vector<std::shared_ptr<CItem>>> m_vecMerchantSlots;

		std::map<EItemType, FContainerInfo> m_mapMerchantItemInfos;


		// UI 자식 Panel들을 가지고 있는 부모Panel입니다.
		std::map<std::wstring, std::shared_ptr<CPanel>> m_mapParentMerchantPanels;

		// 개별 UI를 가지고 있는 map입니다. // 보류
		std::map<std::wstring, std::shared_ptr<CRectComponent>> m_mapMerchantItemUIs;
	};
}
