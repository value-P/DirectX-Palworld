#include "pch.h"
#include "MerchantSystem.h"

// GameObject
#include "Item.h"

std::shared_ptr<CMerchantSystem> Client::CMerchantSystem::Create(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc)
{
	shared_ptr<CMerchantSystem> spInstance = make_shared<CMerchantSystem>();
	CHECKF(spInstance, L"CMerchantSystem : Create Failed");
	spInstance->Initialize(_spContainerSystemDesc);

	return spInstance;
}

HRESULT Client::CMerchantSystem::Initialize(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc)
{
	if (FAILED(CContainerSystem::Initialize(_spContainerSystemDesc))) { return E_FAIL; }

	// 6 x 7 칸 짜리의 2차원 벡터를 초기화 합니다.
	m_vecMerchantSlots.resize(6, std::vector<shared_ptr<CItem>>(7, nullptr));

	return S_OK;
}

HRESULT Client::CMerchantSystem::BeginPlay()
{
	if (FAILED(CContainerSystem::BeginPlay())) { return E_FAIL; }
	return S_OK;
}

int32 Client::CMerchantSystem::PreTick(float _fDeltaSeconds)
{
	switch (CContainerSystem::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CMerchantSystem::Tick(float _fDeltaSeconds)
{
	switch (CContainerSystem::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CMerchantSystem::PostTick(float _fDeltaSeconds)
{
	switch (CContainerSystem::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CMerchantSystem::EndPlay()
{
	if (FAILED(CContainerSystem::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CMerchantSystem::Release()
{
	CContainerSystem::Release();
}

void Client::CMerchantSystem::AddItemToContainer(const std::shared_ptr<CItem>& _spItem)
{
	//if (nullptr == _spItem) { return; }
	//EItemType eItemType = _spItem->GetItemType();

	//// 인벤토리에 해당 아이템이 없을 경우 
	//auto iter = m_mapMerchantItemInfos.find(eItemType);
	//if (iter == m_mapMerchantItemInfos.end())
	//{
	//	for (int32 iOuterIndex = 0; iOuterIndex < 7; iOuterIndex++)
	//	{
	//		for (int32 iInnerIndex = 0; iInnerIndex < 6; iInnerIndex++)
	//		{
	//			if (nullptr == m_vecMerchantSlots[iOuterIndex][iInnerIndex])
	//			{
	//				FContainerInfo tMerchantInfo{};
	//				tMerchantInfo.iIndexX = iOuterIndex;
	//				tMerchantInfo.iIndexY = iInnerIndex;
	//				tMerchantInfo.iCount = 1;

	//				m_mapMerchantItemInfos.emplace(eItemType, tMerchantInfo);
	//				m_vecMerchantSlots[iOuterIndex][iInnerIndex] = _spItem;
	//				_spItem->SetItemCount(tMerchantInfo.iCount);
	//				break;
	//			}
	//		}
	//	}
	//}
	//// 인벤토리에 해당 아이템이 있을 경우
	//else
	//{
	//	weak_ptr<CItem> wpItem = m_vecMerchantSlots[iter->second.iIndexX][iter->second.iIndexY];
	//	iter->second.iCount += wpItem.lock()->GetItemCount(); // map의 개수를 변경해줍니다.
	//	wpItem.lock()->SetItemCount(iter->second.iCount); // 객체가 소유하고 있는 개수를 변경해줍니다.

	//	// 이미 Merchant에 있어서, 매개변수로 받아온 객체를 해제해줍니다.
	//	//_spItem->Release(); 
	//}
}
//
//void Client::CMerchantSystem::ChangeItemIndexInContainer(std::pair<int32, int32> _prCurClickedItemSlotIndexs, FInventoryItemInfo _tPreClickedInventoryItemInfo)
//{
//	//if (nullptr == _spItem) { return; }
//	//EItemType eItemType = _spItem->GetItemType();
//
//	//auto pair = m_mapMerchantItemInfos.find(eItemType);
//	//if (pair == m_mapMerchantItemInfos.end()) { return; }
//	//else
//	//{
//	//	// 인벤토리 슬롯위치에 이미 다른 객체가 있을 경우
//	//	if (nullptr != m_vecMerchantSlots[_tContainerInfo.iIndexX][_tContainerInfo.iIndexY])
//	//	{
//	//		// 기존 슬롯에 있던 객체를 잠시 복사를 해둡니다.
//	//		shared_ptr<CItem> spOriginItem = m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY];
//	//		m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY].reset();
//	//		m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY] = nullptr;
//
//	//		// 이동할 슬롯에 있던 객체를 잠시 복사를 해둡니다.
//	//		shared_ptr<CItem> spNextItem = m_vecMerchantSlots[_tContainerInfo.iIndexX][_tContainerInfo.iIndexY];
//	//		m_vecMerchantSlots[_tContainerInfo.iIndexX][_tContainerInfo.iIndexY].reset();
//	//		m_vecMerchantSlots[_tContainerInfo.iIndexX][_tContainerInfo.iIndexY] = nullptr;
//
//	//		// Origin(map, vec) -> Next(map, vec)
//	//		FContainerInfo tOriginItemInfo = pair->second;
//	//		m_mapMerchantItemInfos.find(spNextItem->GetItemType())->second = tOriginItemInfo;
//	//		m_vecMerchantSlots[_tContainerInfo.iIndexX][_tContainerInfo.iIndexY] = spOriginItem;
//	//		spOriginItem.reset(); spOriginItem = nullptr;
//
//	//		// Next(map, vec) -> Origin(map, vec) 
//	//		FContainerInfo tNextItemInfo = m_mapMerchantItemInfos.find(spNextItem->GetItemType())->second;
//	//		pair->second = tNextItemInfo;
//	//		m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY] = spNextItem;
//	//		spNextItem.reset(); spNextItem = nullptr;
//	//	}
//	//	// 인벤토리 슬롯위치에 이미 다른 객체가 없을 경우
//	//	else
//	//	{
//	//		m_vecMerchantSlots[_tContainerInfo.iIndexX][_tContainerInfo.iIndexY] = m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY];
//	//		m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY].reset();
//	//		m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY] = nullptr;
//
//	//		pair->second.iIndexX = _tContainerInfo.iIndexX;
//	//		pair->second.iIndexY = _tContainerInfo.iIndexY;
//	//		pair->second.iCount; // 명시적으로 표시해둠
//	//	}
//	//}
//}

void Client::CMerchantSystem::SortItemsInContainer(ESortingType _eSortingType)
{
	//int32 iIndex = 0;
	//std::array<std::shared_ptr<CItem>, 42> arrItems{ nullptr };

	//for (size_t i = 0; i < m_vecMerchantSlots.size(); i++)
	//{
	//	for (size_t j = 0; j < m_vecMerchantSlots[i].size(); j++)
	//	{
	//		arrItems[iIndex++] = m_vecMerchantSlots[i][j];
	//	}
	//}

	//// 정렬
	//std::sort(arrItems.begin(), arrItems.end(), [&](const std::shared_ptr<CItem>& _spItemA, const std::shared_ptr<CItem>& _spItemB) -> bool
	//	{
	//		switch (_eSortingType)
	//		{
	//		case ESortingType::RANDOM: { return _spItemA != nullptr && _spItemB == nullptr; }
	//		case ESortingType::ITEM_TYPE: { return _spItemA->GetItemType() < _spItemB->GetItemType(); }
	//		case ESortingType::ITEM_COUNT: { return _spItemA->GetItemCount() < _spItemB->GetItemCount(); }
	//		}
	//		return 0;
	//	});

	//iIndex = 0;
	//for (size_t i = 0; i < m_vecMerchantSlots.size(); i++)
	//{
	//	for (size_t j = 0; j < m_vecMerchantSlots[i].size(); j++)
	//	{
	//		m_vecMerchantSlots[i][j] = arrItems[iIndex++];

	//		if (nullptr == m_vecMerchantSlots[i][j]) { continue; }

	//		EItemType eItemType = m_vecMerchantSlots[i][j]->GetItemType();
	//		m_mapMerchantItemInfos.find(eItemType)->second.iIndexX = static_cast<int32>(i);
	//		m_mapMerchantItemInfos.find(eItemType)->second.iIndexY = static_cast<int32>(j);
	//	}
	//}
}

void Client::CMerchantSystem::ActiveSlotHover()
{
}
