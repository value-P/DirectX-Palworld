#pragma once
#include "ContainerSystem.h"

namespace Engine
{
	class CPanel;
	class CRectComponent; // ���� ButtonComponent�� �ٲ� ���� �ֽ��ϴ�.
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


		// UI �ڽ� Panel���� ������ �ִ� �θ�Panel�Դϴ�.
		std::map<std::wstring, std::shared_ptr<CPanel>> m_mapParentMerchantPanels;

		// ���� UI�� ������ �ִ� map�Դϴ�. // ����
		std::map<std::wstring, std::shared_ptr<CRectComponent>> m_mapMerchantItemUIs;
	};
}
