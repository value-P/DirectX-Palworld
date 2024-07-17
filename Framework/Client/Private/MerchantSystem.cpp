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

	// 6 x 7 ĭ ¥���� 2���� ���͸� �ʱ�ȭ �մϴ�.
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

	//// �κ��丮�� �ش� �������� ���� ��� 
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
	//// �κ��丮�� �ش� �������� ���� ���
	//else
	//{
	//	weak_ptr<CItem> wpItem = m_vecMerchantSlots[iter->second.iIndexX][iter->second.iIndexY];
	//	iter->second.iCount += wpItem.lock()->GetItemCount(); // map�� ������ �������ݴϴ�.
	//	wpItem.lock()->SetItemCount(iter->second.iCount); // ��ü�� �����ϰ� �ִ� ������ �������ݴϴ�.

	//	// �̹� Merchant�� �־, �Ű������� �޾ƿ� ��ü�� �������ݴϴ�.
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
//	//	// �κ��丮 ������ġ�� �̹� �ٸ� ��ü�� ���� ���
//	//	if (nullptr != m_vecMerchantSlots[_tContainerInfo.iIndexX][_tContainerInfo.iIndexY])
//	//	{
//	//		// ���� ���Կ� �ִ� ��ü�� ��� ���縦 �صӴϴ�.
//	//		shared_ptr<CItem> spOriginItem = m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY];
//	//		m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY].reset();
//	//		m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY] = nullptr;
//
//	//		// �̵��� ���Կ� �ִ� ��ü�� ��� ���縦 �صӴϴ�.
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
//	//	// �κ��丮 ������ġ�� �̹� �ٸ� ��ü�� ���� ���
//	//	else
//	//	{
//	//		m_vecMerchantSlots[_tContainerInfo.iIndexX][_tContainerInfo.iIndexY] = m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY];
//	//		m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY].reset();
//	//		m_vecMerchantSlots[pair->second.iIndexX][pair->second.iIndexY] = nullptr;
//
//	//		pair->second.iIndexX = _tContainerInfo.iIndexX;
//	//		pair->second.iIndexY = _tContainerInfo.iIndexY;
//	//		pair->second.iCount; // ��������� ǥ���ص�
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

	//// ����
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
