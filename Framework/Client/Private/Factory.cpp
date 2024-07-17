#include "pch.h"
#include "Factory.h"

// client manager
#include "UIManager.h"
#include "GameManager.h"
#include "EffectManager.h"

// engine manager
#include "EngineModule.h"
#include "InputManager.h"

// engine
#include "Panel.h"

// component
#include "ProgressBarComponent.h"
#include "TextComponent.h"
#include "ImageComponent.h"

// system
#include "InventorySystem.h"
#include "ProductionSystem.h"
#include "ToDoSystem.h"

// gameobject
#include "Monster.h"
#include "EffectGroup.h"

#include "ClientLoop.h"

HRESULT Client::CFactory::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// architecture desc
	if (FAILED(CArchitecture::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// factory desc
	shared_ptr<FFactoryDesc> spFactoryDesc = static_pointer_cast<FFactoryDesc>(_spGameObjectDesc);
	m_bIsFactory = true;

	// 인벤토리 시스템을 불러옵니다.
	m_wpInventorySystem = CGameManager::GetInstance()->GetInventorySystem();
	m_spUIManager = CUIManager::GetInstance();

	return S_OK;
}

HRESULT Client::CFactory::BeginPlay()
{
	if (FAILED(CArchitecture::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CFactory::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	// 건축이 완료되고 콜라이더에 닿았을 때 조건을 통과합니다.
	if (m_tBuildStatus.m_bBuildCompleted && m_bInteractionEnable && nullptr == m_spCurProduceStatus && nullptr == m_spWorkPanel)
	{
		if (CEngineModule::GetInstance()->GetInputManager()->IsKeyDown(DIK_F))
		{
			shared_ptr<CProductionSystem> spProductionSystem = CGameManager::GetInstance()->GetProductionSystem();

			switch (m_eArchitectureType)
			{
			case Client::EArchitectureType::WORKBENCH:
				spProductionSystem->OnWorkPanel(m_eArchitectureType, static_pointer_cast<CArchitecture>(shared_from_this()));
				break;

			case Client::EArchitectureType::BLAST_FURNACE:
				spProductionSystem->OnWorkPanel(m_eArchitectureType, static_pointer_cast<CArchitecture>(shared_from_this()));
				break;

			case Client::EArchitectureType::POT:
				spProductionSystem->OnWorkPanel(m_eArchitectureType, static_pointer_cast<CArchitecture>(shared_from_this()));
				break;

			case Client::EArchitectureType::WEAPON_WORKBENCH:
				spProductionSystem->OnWorkPanel(m_eArchitectureType, static_pointer_cast<CArchitecture>(shared_from_this()));
				break;
			}
		}
	}

	if (m_bInteractionEnable)
	{
		if (nullptr != m_spWorkPanel) { m_spWorkPanel->SetActive(true); }
	}
	else
	{
		if (nullptr != m_spWorkPanel) { m_spWorkPanel->SetActive(false); }
	}

	return 0;
}

int32 Client::CFactory::Tick(float _fDeltaSeconds)
{
	switch (CArchitecture::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CFactory::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

HRESULT Client::CFactory::EndPlay()
{
	if (FAILED(CArchitecture::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CFactory::Release()
{
	m_spCurProduceStatus = nullptr;
	m_spPalImage = nullptr;
	m_spChildPalPanel = nullptr;
	m_spCurNumText = nullptr;
	m_spCircleProgressBar = nullptr;
	if (m_spWorkPanel)
	{
		m_spUIManager->ReturnWorkUI(m_spWorkPanel);
	}
	m_spWorkPanel = nullptr;
	m_spUIManager = nullptr;
	CArchitecture::Release();
}

bool Client::CFactory::IsProduceCompleted() const
{
	if (!m_spCurProduceStatus) { return false; }

	return m_spCurProduceStatus->m_eProduceCompleted == EProduceItemState::COMPLETED;
}

bool Client::CFactory::IsProducePartialyCompleted() const
{
	if (!m_spCurProduceStatus) { return false; }

	return m_spCurProduceStatus->m_eProduceCompleted == EProduceItemState::PARTIALY_COMPLETED;
}

void Client::CFactory::AddItemToFactory(EItemType _eItemType, int32 _iNumItems)
{
	CArchitecture::AddItemToFactory(_eItemType, _iNumItems);

	FProduceStatus tProduceStatus;
	tProduceStatus.m_eItemType = _eItemType;
	tProduceStatus.m_iNumItems = _iNumItems;
	tProduceStatus.m_fMaxDeltaSeconds = CClientLoop::s_spClientDataSheet->k_mapItemProduceStatuses[_eItemType].first;
	tProduceStatus.m_iNumItemAtOnce = CClientLoop::s_spClientDataSheet->k_mapItemProduceStatuses[_eItemType].second;

	m_spCurProduceStatus = make_shared<FProduceStatus>(tProduceStatus);
}

void Client::CFactory::RemoveItemFromFactory()
{
	m_spCurProduceStatus = nullptr;
}

void Client::CFactory::TakeItemFromFactory()
{
	// 아이템이 없거나 생산되지 않은 경우
	if (!m_spCurProduceStatus) { return; }

	if (m_spCurProduceStatus->m_eProduceCompleted == EProduceItemState::NOT_COMPLETED) { return; }

	// 부분적으로 생산 완료된 경우
	if (m_spCurProduceStatus->m_eProduceCompleted == EProduceItemState::PARTIALY_COMPLETED)
	{
		// 아이템을 인벤토리에 넣습니다.
		switch (m_eFactoryType)
		{
		case Client::EFactoryType::BERRY_FARM:
		case Client::EFactoryType::LUMBERYARD:
		case Client::EFactoryType::STONE_PIT:
			AddItemToInventory(m_spCurProduceStatus->m_eItemType, m_spCurProduceStatus->m_iNumProducedItem);
			m_spCurProduceStatus->m_iNumProducedItem = 0;

			m_spCurNumText->SetText(to_wstring(m_spCurProduceStatus->m_iNumProducedItem));
			m_spCurProduceStatus->m_eProduceCompleted = EProduceItemState::NOT_COMPLETED;
			break;
		}
		return;
	}

	// 이곳에서 UI를 끕니다
	if (m_spWorkPanel)
	{
 		m_spUIManager->ReturnWorkUI(m_spWorkPanel);
		m_spWorkPanel = nullptr;
	}

	// 아이템을 인벤토리에 넣습니다.
	AddItemToInventory(m_spCurProduceStatus->m_eItemType, m_spCurProduceStatus->m_iNumItems);
	
	// 명령을 제거합니다.
	RemoveItemFromFactory();
}

EProduceItemState Client::CFactory::ProduceItem(float _fDeltaSeconds)
{
	if (m_spCurProduceStatus->m_eProduceCompleted == EProduceItemState::COMPLETED)
	{
		return EProduceItemState::COMPLETED;
	}

	// 생산 진행률을 구합니다.
	m_spCurProduceStatus->m_fSumDeltaSeconds += _fDeltaSeconds;
	m_spCurProduceStatus->m_iCurPercentage = m_spCurProduceStatus->m_fSumDeltaSeconds / m_spCurProduceStatus->m_fMaxDeltaSeconds;

	// 생산 진행률을 프로그래스바와 연동해줍니다.
	m_spCircleProgressBar->SetRatio(m_spCurProduceStatus->m_iCurPercentage);

	shared_ptr<CMonster> spMonster = nullptr;
	for (auto& pair : m_umapWorkingSlavePals)
	{
		if (pair.second.expired()) { continue; }
		
		spMonster = pair.second.lock();
	}
	
	// 생산하고 있는 팰이 있다면 자식 패널에 연동해줍니다.
	if (nullptr != spMonster)
	{
		m_spChildPalPanel->SetActive(true);
		m_spPalImage->SetTextureFileName(m_spUIManager->FindPalIconTextureName(spMonster->GetMonsterType()));
	}
	else
	{
		m_spChildPalPanel->SetActive(false);
	}

	if (m_spCurProduceStatus->m_iNumProducedItem == 0)
	{
		m_spCurNumText->SetText(to_wstring(m_spCurProduceStatus->m_iNumProducedItem));
		m_spCurProduceStatus->m_eProduceCompleted = EProduceItemState::NOT_COMPLETED;
	}

	// 한 아이템이 생산되었습니다.
	if (1.0f <= m_spCurProduceStatus->m_iCurPercentage)
	{
		// 한 번에 생산할 수 있는 아이템 수를 생산된 아이템에 추가합니다.
		m_spCurProduceStatus->m_iNumProducedItem += m_spCurProduceStatus->m_iNumItemAtOnce;

		// 아이템 하나가 추가되었으므로 프로그래스바를 0으로 초기화해줍니다.
		m_spCircleProgressBar->SetRatio(0.0f);

		// 생산된 아이템 개수를 연동합니다.
		m_spCurNumText->SetText(to_wstring(m_spCurProduceStatus->m_iNumProducedItem));

		// 모든 아이템이 생산 완료되었습니다.
		if (m_spCurProduceStatus->m_iNumItems <= m_spCurProduceStatus->m_iNumProducedItem)
		{
			m_spCurProduceStatus->m_iNumProducedItem = m_spCurProduceStatus->m_iNumItems;
			m_spCurProduceStatus->m_eProduceCompleted = EProduceItemState::COMPLETED;
			m_spCircleProgressBar->SetRatio(1.0f);

			// 일하는 팰을 해제합니다.
			for (auto& pair : m_umapWorkingSlavePals) { pair.second.reset(); }
			m_umapWorkingSlavePals.clear();

			m_spChildPalPanel->SetActive(false);
			return EProduceItemState::COMPLETED;
		}
		// 남은 아이템을 더 생산해야 합니다.
		else
		{
			m_spCurProduceStatus->m_eProduceCompleted = EProduceItemState::PARTIALY_COMPLETED;
		}

		m_spCurProduceStatus->m_fSumDeltaSeconds = 0.0f;
		m_spCurProduceStatus->m_iCurPercentage = 0.0f;
	}

	// 제작 이펙트를 출력합니다.
	PlayWorkingEffect(m_eFactoryType, _fDeltaSeconds);

	return EProduceItemState::PARTIALY_COMPLETED;
}

void Client::CFactory::SetWorkPanel(std::shared_ptr<CPanel> _spWorkPanel)
{
	m_spWorkPanel = _spWorkPanel;
	m_spCircleProgressBar = m_spWorkPanel->FindComponent<CProgressBarComponent>(L"CircleProgressBar");
	m_spCurNumText = m_spWorkPanel->FindComponent<CTextComponent>(L"CurCountText");

	m_spChildPalPanel = static_pointer_cast<CPanel>(m_spWorkPanel->FindChildGameObject(L"ChildWorkPalUI"));
	m_spChildPalPanel->SetActive(false);
	m_spPalImage = m_spChildPalPanel->FindComponent<CImageComponent>(L"PalIconImage");

	// 위치를 동기화시켜줍니다.
	Vector3 vPosition = Vector3(GetTransform(ETransformType::POSITION).value().x, GetTransform(ETransformType::POSITION).value().y + m_vColliderExtents.y + 0.3f, GetTransform(ETransformType::POSITION).value().z);
	m_spWorkPanel->SetPivotWorldPoint(vPosition);
}

void Client::CFactory::PlayWorkingEffect(EFactoryType _eFactoryType, float _fDeltaSeconds)
{
	m_fEffectTimeAcc += _fDeltaSeconds;

	if (1.f < m_fEffectTimeAcc)
	{
		m_fEffectTimeAcc = 0.f;

		// 1초에 한번 제작 이펙트를 활성화 합니다.
		switch (_eFactoryType)
		{
		case EFactoryType::WORK_BENCH:
		case EFactoryType::WEAPON_WORKBENCH:
			m_wpEffectManager.lock()->ActivateEffect(L"Group_Crafting", GetPosition().value() + Vector3(0.f, 1.f, 0.f));
			break;

		}
	}
}
