// default
#include "pch.h"
#include "Architecture.h"

// engine manager
#include "EngineModule.h"
#include "InputManager.h"
#include "SoundManager.h"

// client manager
#include "ClientModule.h"
#include "GameManager.h"
#include "BuildManager.h"
#include "EffectManager.h"
#include "UIManager.h"

// level
#include "Level.h"

// system
#include "ToDoSystem.h"
#include "ProductionSystem.h"
#include "InventorySystem.h"
#include "ChestBoxSystem.h"

// component
#include "ColliderComponent.h"
#include "Panel.h"
#include "TextComponent.h"
#include "ImageComponent.h"
#include "ProgressBarComponent.h"
#include "MeshComponent.h"
#include "Model.h"

// gameobject
#include "Monster.h"
#include "BerryFarm.h"
#include "LumberYard.h"
#include "StonePit.h"

// etc
#include "Model.h"
#include "MeshBuffer.h"
#include "Material.h"

HRESULT Client::CArchitecture::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// habitat desc
	if (FAILED(CHabitat::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();
	m_wpUIManager = CUIManager::GetInstance();

	// architecture desc
	shared_ptr<FArchitectureDesc> spArchitectureDesc = static_pointer_cast<FArchitectureDesc>(_spGameObjectDesc);
	m_eArchitectureType         = spArchitectureDesc->m_eArchitectureType;
	m_iArchitectureUnlockLevel  = spArchitectureDesc->m_iArchitectureUnlockLevel;
	m_wstrShaderName            = spArchitectureDesc->m_wstrShaderName;
	m_wstrModelFilePath         = spArchitectureDesc->m_wstrModelFilePath;

	m_vColliderExtents          = spArchitectureDesc->m_vColliderExtents;
	m_vColliderCenter           = spArchitectureDesc->m_vColliderCenter;
	m_vColliderRotate           = spArchitectureDesc->m_vColliderRotate;
	
	m_tBuildStatus				= spArchitectureDesc->m_tBuildStatus;
	m_fPivotScaleRatio          = spArchitectureDesc->m_fPivotScaleRatio;
	m_vecNeededMinerals         = spArchitectureDesc->m_vecNeededMinerals;
	m_vecNeededMineralCounts    = spArchitectureDesc->m_vecNeededMineralCounts;

	// etc
	m_spBuildManager = CClientModule::GetInstance()->GetBuildManager();
	m_wpInventorySystem = CGameManager::GetInstance()->GetInventorySystem();
	m_wpChestBoxSystem = CGameManager::GetInstance()->GetChestBoxSystem();

	// ui
	if (!m_tBuildStatus.m_bBuildCompleted)
	{
		// ���� ���� ui�� �ҷ��ɴϴ�.
		m_spProductionPanel = CUIManager::GetInstance()->GetUsingProductionArchitectureUI();

		// ������Ʈ���� �������ݴϴ�.
		{
			// �θ� �г�
			//m_tPanelInfo.spHourTextComponent = m_spProductionPanel->FindComponent<CTextComponent>(L"HourText");
			//m_tPanelInfo.spMinTextComponent = m_spProductionPanel->FindComponent<CTextComponent>(L"MinText");
			m_tPanelInfo.spSecTextComponent = m_spProductionPanel->FindComponent<CTextComponent>(L"SecText");
			m_tPanelInfo.spProgressComponent = m_spProductionPanel->FindComponent<CProgressBarComponent>(L"HorizontalProgressBar");

			// �ڽ� �г�
			m_tPanelInfo.spChildPanel = static_pointer_cast<CPanel>(m_spProductionPanel->FindChildGameObject(L"ChildPalUI"));
			m_tPanelInfo.spChildPanel->SetActive(false);
			m_tPanelInfo.spPalProgressComponent = m_tPanelInfo.spChildPanel->FindComponent<CProgressBarComponent>(L"PalProgressBar");
			m_tPanelInfo.spPalImageComponent = m_tPanelInfo.spChildPanel->FindComponent<CImageComponent>(L"PalIconImage");
		}

		Vector3 vPosition(0.0f);
		// ��ġ���� �������ݴϴ�.
		if (EArchitectureType::BERRY_FARM == m_eArchitectureType || EArchitectureType::QUARRY == m_eArchitectureType || EArchitectureType::LUMBERYARD == m_eArchitectureType)
		{
			vPosition = Vector3(GetTransform(ETransformType::POSITION).value().x, GetTransform(ETransformType::POSITION).value().y + m_vColliderExtents.y + 0.7f, GetTransform(ETransformType::POSITION).value().z);
		}
		else
		{
			vPosition = Vector3(GetTransform(ETransformType::POSITION).value().x, GetTransform(ETransformType::POSITION).value().y + m_vColliderExtents.y + 0.3f, GetTransform(ETransformType::POSITION).value().z);
		}
		m_spProductionPanel->SetPivotWorldPoint(vPosition);

		m_spProductionPanel->SetActive(false);
	}

	return S_OK;
}

HRESULT Client::CArchitecture::BeginPlay()
{
	if (FAILED(CHabitat::BeginPlay())) { return E_FAIL; }

	// ���� �ӿ��� �Ǽ� ����� �����ϴ�.
	// �Ͽ�¡�� ���� ������ m_tBuildStatus.m_bBuildCompleted�� true�� �����Ͽ�, TodoSystem�� ��ϵ��� �ʵ��� ���ܷ� ó���մϴ�.
	if (!m_tBuildStatus.m_bBuildCompleted)
	{
		m_iCurCommandID = RegisterCommandToSystem(EToDoType::BUILD_ARCHITECTURE, EMonsterTaskType::HANDIWORK);
	}

	if (m_bCompleted)
	{
		for (auto& MeshComponentPair : m_umapMeshComponents)
		{
			if (!MeshComponentPair.second) { continue; }

			auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

			for (auto& ShaderTypePair : umapMeshInfos)
			{
				for (auto& tMeshInfo : ShaderTypePair.second)
				{
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"State", (int32)EArchitextureState::COMPLETE);
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Alpha", 1.f);
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", 1.f);
				}
			}
		}
	}
	else
	{
		for (auto& MeshComponentPair : m_umapMeshComponents)
		{
			if (!MeshComponentPair.second) { continue; }

			auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

			for (auto& ShaderTypePair : umapMeshInfos)
			{
				for (auto& tMeshInfo : ShaderTypePair.second)
				{
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"State", (int32)EArchitextureState::BUILDING);
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Alpha", 0.5f);
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", 0.f);
				}
			}
		}
	}	

	// ���� �Ҹ��� ����մϴ�.
	m_wpSoundManager.lock()->PlaySoundEx("Build_Hover_191978919", 1.f, false);

	return S_OK; 
}

int32 Client::CArchitecture::PreTick(float _fDeltaSeconds)
{
	if (!m_tBuildStatus.m_bBuildCompleted) { return 0; }

	switch(CHabitat::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CArchitecture::Tick(float _fDeltaSeconds)
{
	if (!m_tBuildStatus.m_bBuildCompleted)
	{
		if (m_spProductionPanel)
		{
			UpdateUI();
		}
	}

	if (!m_bPrefab && (m_eArchitectureType == EArchitectureType::LUMBERYARD || m_eArchitectureType == EArchitectureType::QUARRY || m_eArchitectureType == EArchitectureType::BERRY_FARM))
	{
		for (auto& MeshComponentPair : m_umapMeshComponents)
		{
			if (!MeshComponentPair.second) { continue; }

			auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

			for (auto& ShaderTypePair : umapMeshInfos)
			{
				for (auto& tMeshInfo : ShaderTypePair.second)
				{
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", m_tBuildStatus.m_iCurPercentage - 0.5f);
				}
			}
		}
	}
	else if (!m_bPrefab)
	{
		for (auto& MeshComponentPair : m_umapMeshComponents)
		{
			if (!MeshComponentPair.second) { continue; }

			auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

			for (auto& ShaderTypePair : umapMeshInfos)
			{
				for (auto& tMeshInfo : ShaderTypePair.second)
				{
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", m_tBuildStatus.m_iCurPercentage - 0.01f);
				}
			}
		}
	}
	else
	{
		for (auto& MeshComponentPair : m_umapMeshComponents)
		{
			if (!MeshComponentPair.second) { continue; }

			auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

			for (auto& ShaderTypePair : umapMeshInfos)
			{
				for (auto& tMeshInfo : ShaderTypePair.second)
				{
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", -5.f);
				}
			}
		}
	}

	if (!m_tBuildStatus.m_bBuildCompleted) { return 0; }

	switch(CHabitat::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CArchitecture::PostTick(float _fDeltaSeconds)
{
	// if (!m_tBuildStatus.m_bBuildCompleted) { return 0; }

	switch(CHabitat::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CArchitecture::EndPlay()
{
	if (FAILED(CHabitat::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CArchitecture::Release()
{
	m_spBuildManager = nullptr;
	m_spBoxComponent = nullptr;

	for (auto& spRequiredMineral : m_vecNeededMinerals)
	{
		spRequiredMineral = nullptr;
	}
	m_vecNeededMinerals.clear();

	if (m_spProductionPanel)
	{
		m_wpUIManager.lock()->ReturnProductionArchitectureUI(m_spProductionPanel);
		m_spProductionPanel = nullptr;
	}

	if (nullptr != m_wpUIManager.lock())
	{
		if (true == m_wpUIManager.lock()->GetInteractionPanel()->IsActive())
		{
			m_wpUIManager.lock()->SetActiveInteractionPanel(false);
		}
	}

	{
		m_tPanelInfo.spSecTextComponent = nullptr;
		m_tPanelInfo.spProgressComponent = nullptr;

		m_tPanelInfo.spPalProgressComponent = nullptr;
		m_tPanelInfo.spPalImageComponent = nullptr;
		m_tPanelInfo.spChildPanel = nullptr;
	}

	CHabitat::Release();
}

void Client::CArchitecture::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		return;
	}

	// �浹 ���̾ ��ȣ�ۿ��� ���, �÷��׸� �մϴ�.
	CColliderComponent* pColliderComponent = static_cast<CColliderComponent*>(_pDstComponent);
	if (ECollisionLayer::INTERACTION == pColliderComponent->GetCollisionLayer())
	{
		m_bInteractionEnable = true;
		if (m_spProductionPanel)
		{
			m_spProductionPanel->SetActive(true);
		}

		if (EArchitectureType::BERRY_FARM == m_eArchitectureType || EArchitectureType::BLAST_FURNACE == m_eArchitectureType || EArchitectureType::HUMAN_BED == m_eArchitectureType
			|| EArchitectureType::IRON_BOX == m_eArchitectureType || EArchitectureType::LUMBERYARD == m_eArchitectureType || EArchitectureType::PAL_BOX == m_eArchitectureType
			|| EArchitectureType::POT == m_eArchitectureType || EArchitectureType::WEAPON_WORKBENCH == m_eArchitectureType || EArchitectureType::WORKBENCH == m_eArchitectureType
			|| EArchitectureType::WOODEN_CHEST == m_eArchitectureType || EArchitectureType::QUARRY == m_eArchitectureType)
		{
			m_wpUIManager.lock()->SetActiveInteractionPanel(true);
			m_wpUIManager.lock()->SetInteractionPivotPoint(GetPosition().value());
		}
	}
}

void Client::CArchitecture::OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		return;
	}

	// �浹 ���̾ ��ȣ�ۿ��� ���, �÷��׸� �մϴ�.
	CColliderComponent* pDstColliderComponent = static_cast<CColliderComponent*>(_pDstComponent);
	if (ECollisionLayer::INTERACTION == pDstColliderComponent->GetCollisionLayer())
	{
		m_bInteractionEnable = false;
		if (m_spProductionPanel) { m_spProductionPanel->SetActive(false); }
		if (true == m_wpUIManager.lock()->GetInteractionPanel()->IsActive())
		{
			m_wpUIManager.lock()->SetActiveInteractionPanel(false);
		}
	}
}

bool Client::CArchitecture::BuildArchitecture(float _fDeltaSeconds)
{
	// ������ �Ϸ�� ���
	if (1.0f <= m_tBuildStatus.m_iCurPercentage) { return true; }

	// ���� ������� ���մϴ�.
	m_tBuildStatus.m_fSumDeltaSeconds += _fDeltaSeconds;
	m_tBuildStatus.m_iCurPercentage = m_tBuildStatus.m_fSumDeltaSeconds / m_tBuildStatus.m_fMaxDeltaSeconds;

	shared_ptr<CMonster> spMonster = nullptr;
	for (auto& pair : m_umapWorkingSlavePals)
	{
		if (!pair.second.expired())
		{
			spMonster = pair.second.lock();
		}
	}

	if (nullptr != spMonster)
	{
		m_tPanelInfo.spChildPanel->SetActive(true);
		m_tPanelInfo.spPalImageComponent->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(spMonster->GetMonsterType()));
	}
	else
	{
		m_tPanelInfo.spChildPanel->SetActive(false);
	}

	// ������ �Ϸ�� ���
	if (1.0f <= m_tBuildStatus.m_iCurPercentage)
	{
		m_tBuildStatus.m_bBuildCompleted = true;
		CUIManager::GetInstance()->ReturnProductionArchitectureUI(m_spProductionPanel);
		m_spProductionPanel = nullptr;

		m_wpToDoSystem.lock()->SetCommandState(EToDoType::BUILD_ARCHITECTURE, m_iCurCommandID, EToDoCommandState::FINISHED);

		// ���ϴ� ���� �����մϴ�.
		for (auto& pair : m_umapWorkingSlavePals) { pair.second.reset(); }
		m_umapWorkingSlavePals.clear();

		switch (m_eArchitectureType)
		{
		case Client::EArchitectureType::WOODEN_CHEST:
			m_wpChestBoxSystem.lock()->MakeChestBox(EArchitectureType::WOODEN_CHEST, m_iGameObjectID);
			break;

		case Client::EArchitectureType::IRON_BOX:
			m_wpChestBoxSystem.lock()->MakeChestBox(EArchitectureType::IRON_BOX, m_iGameObjectID);
			break;

		case Client::EArchitectureType::BERRY_FARM:
		{
			shared_ptr<CBerryFarm> spFactory = dynamic_pointer_cast<CBerryFarm>(shared_from_this());
			spFactory->AddPermanentCommand();
		}
		break;

		case Client::EArchitectureType::LUMBERYARD:
		{
			shared_ptr<CLumberYard> spFactory = dynamic_pointer_cast<CLumberYard>(shared_from_this());
			spFactory->AddPermanentCommand();
		}
		break;

		case Client::EArchitectureType::QUARRY:
		{
			shared_ptr<CStonePit> spFactory = dynamic_pointer_cast<CStonePit>(shared_from_this());
			spFactory->AddPermanentCommand();
		}
		break;

		default:
			break;
		}

		for (auto& MeshComponentPair : m_umapMeshComponents)
		{
			auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

			for (auto& ShaderTypePair : umapMeshInfos)
			{
				for (auto& tMeshInfo : ShaderTypePair.second)
				{
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"State", (int32)EArchitextureState::COMPLETE);
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Alpha", 1.0f);
					tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", 1.f);
				}
			}
		}

		return true;
	}

	return false;
}

void Client::CArchitecture::DestroyArchitecture()
{
	// ���� �Ŵ������� ���๰�� �����մϴ�.
	m_spBuildManager->RemoveArchitecture(m_eArchitectureType, static_pointer_cast<CArchitecture>(shared_from_this()));

	// �������� ������Ʈ�� �����մϴ�.
	m_wpOwnerLevel.lock()->RemoveGameObject(shared_from_this());

	// �ı� ����Ʈ�� ����մϴ�.
	CEffectManager::GetInstance()->ActivateEffect(L"Group_BuildDestroy", GetPosition().value());

	// UI�� �ݳ��մϴ�.
	if (m_spProductionPanel)
	{
		CUIManager::GetInstance()->ReturnProductionArchitectureUI(m_spProductionPanel);
	}

	// (����) �ӹڽ��� ���, ���� �Ŵ������� ������ nullptr�� �ٲߴϴ�.
	if (EArchitectureType::PAL_BOX == m_eArchitectureType)
	{
		m_spBuildManager->SetBaseCampNull();
	}

	ClearEffect();

	// Release�� ȣ���մϴ�.
	// Release();
}

void Client::CArchitecture::UpdateUI()
{
	float fTime = m_tBuildStatus.m_fMaxDeltaSeconds - m_tBuildStatus.m_fSumDeltaSeconds;
	int iTime = 0;

	std::wstring wstrTimeText;
	if (fTime >= 10.0f)
	{
		wstrTimeText = to_wstring(static_cast<int>(fTime));
	} 
	else
	{
		wstrTimeText = L"0" + to_wstring(static_cast<int>(fTime));
	}

	m_tPanelInfo.spSecTextComponent->SetText(wstrTimeText);
	m_tPanelInfo.spProgressComponent->SetRatio(m_tBuildStatus.m_iCurPercentage);
}

void Client::CArchitecture::AddItemToInventory(EItemType _eItemType, int32 _iNumItems)
{
	// �κ��丮�� �������� �ֽ��ϴ�.
	m_wpInventorySystem.lock()->AddItemToContainer(_eItemType, _iNumItems);
}

void Client::CArchitecture::AddItemToFactory(EItemType _eItemType, int32 _iNumItems)
{
	CHECKF(m_bIsFactory, L"Only factory can produce items.");
}

void Client::CArchitecture::OnBluePrint()
{
	for (auto& MeshComponentPair : m_umapMeshComponents)
	{
		if (!MeshComponentPair.second) { continue; }

		auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

		for (auto& ShaderTypePair : umapMeshInfos)
		{
			for (auto& tMeshInfo : ShaderTypePair.second)
			{
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"State", (int32)EArchitextureState::BUILDING);
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Alpha", 0.5f);
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", 0.f);
			}
		}
	}
}

void Client::CArchitecture::OffBluePrint()
{
	for (auto& MeshComponentPair : m_umapMeshComponents)
	{
		if (!MeshComponentPair.second) { continue; }

		auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

		for (auto& ShaderTypePair : umapMeshInfos)
		{
			for (auto& tMeshInfo : ShaderTypePair.second)
			{
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"State", (int32)EArchitextureState::COMPLETE);
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Alpha", 1.f);
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", 1.f);
			}
		}
	}
}

void Client::CArchitecture::PlayBuildCompleteSound(EArchitectureType _eArchitectureType)
{
	switch (_eArchitectureType)
	{
	case EArchitectureType::WORKBENCH:
	
		break;

	case EArchitectureType::PAL_BOX:

		break;

	// case EArchitectureType::WOODEN
	}
}