// default
#include "pch.h"
#include "TaskDead.h"

// manager
#include "EngineModule.h"
#include "EffectManager.h"
#include "SoundManager.h"

// manager
#include "GameManager.h"
#include "UIManager.h"

// component
#include "MeshComponent.h"
#include "BehaviorTreeComponent.h"
#include "ColliderComponent.h"
#include "AnimationImageComponent.h"

// system
#include "InventorySystem.h"
#include "PalBoxSystem.h"

// gameobject
#include "Monster.h"
#include "Ingredient.h"
#include "Mineral.h"
#include "Panel.h"

// engine
#include "Model.h"

// client
#include "ClientLoop.h"

// utility
#include "MathUtility.h"

std::shared_ptr<CTaskDead> Client::CTaskDead::Create(const std::shared_ptr<FTaskDeadDesc>& _spTaskDeadDesc)
{
	const shared_ptr<CTaskDead> spTaskNode = make_shared<CTaskDead>();
	ENSUREF(spTaskNode->Initialize(_spTaskDeadDesc), L"Failed to initialize task dead.");
	return spTaskNode;
}

HRESULT Client::CTaskDead::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();
	m_wpPalBoxSystem = CGameManager::GetInstance()->GetPalBoxSystem();
	m_wpInventorySystem = CGameManager::GetInstance()->GetInventorySystem();

	// dead task desc
	shared_ptr<FTaskDeadDesc> spTaskDeadDesc = static_pointer_cast<FTaskDeadDesc>(_spNodeDesc);
	m_iAnimationID		= spTaskDeadDesc->m_iAnimationID;

	// etc
	m_wpMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_wpMonster.lock()->GetMeshComponent();

	// get item ui
	m_wpGetItemParentPanel = CUIManager::GetInstance()->FindParentsPanel(L"GetItemPanel");
	m_wpGetItemParentPanel.lock()->SetActive(true);
	m_wpGetItemAnimationImageComponentSlot0 = m_wpGetItemParentPanel.lock()->FindComponent<CAnimationImageComponent>(L"get_item_slot_0");
	m_wpGetItemAnimationImageComponentSlot1 = m_wpGetItemParentPanel.lock()->FindComponent<CAnimationImageComponent>(L"get_item_slot_1");
	m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(false);
	m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(false);

	return S_OK;
}

void Client::CTaskDead::OnNodeInit(float _fDeltaSecond)
{
	m_eMonsterType = m_wpMonster.lock()->GetMonsterType();
}

void Client::CTaskDead::OnNodeEnter(float _fDeltaSecond)
{
	m_wpMonster.lock()->StopSoundAll();

	Vector3 vPosition = m_wpMonster.lock()->GetPosition().value();

	// 사망 사운드를 출력합니다.
	switch (m_eMonsterType)
	{
	case Client::EMonsterType::BERRY_GOAT:
		m_wpSoundManager.lock()->PlaySoundEx("VO_BerryGoat_06_Death_2_566170877", vPosition);
		break;

	case Client::EMonsterType::BOAR:
		m_wpSoundManager.lock()->PlaySoundEx("VO_Boar_06_Death_827553277", vPosition);
		break;

	case Client::EMonsterType::DREAM_DEMON:
		m_wpSoundManager.lock()->PlaySoundEx("VO_DreamDemon_06_Death_2_163610239", vPosition);
		break;

	case Client::EMonsterType::HEDGE_HOG:
		m_wpSoundManager.lock()->PlaySoundEx("VO_Hedgehog_06_Death_2_824900089", vPosition);
		break;

	case Client::EMonsterType::KITSUNEBI:
		m_wpSoundManager.lock()->PlaySoundEx("VO_Kitsunebi_06_Death_02_799063528", vPosition);
		break;

	case Client::EMonsterType::PENGUIN:
		m_wpSoundManager.lock()->PlaySoundEx("VO_Penguin_06_Death_607220847", vPosition);
		break;

	case Client::EMonsterType::PINK_CAT:
		m_wpSoundManager.lock()->PlaySoundEx("VO_PinkCat_06_Death_842808624", vPosition);
		break;

	case Client::EMonsterType::SHEEP_BALL:
		m_wpSoundManager.lock()->PlaySoundEx("VO_SheepBall_06_Death_PT1_2_803069845", vPosition);
		break;

	case Client::EMonsterType::WOOL_FOX:
		m_wpSoundManager.lock()->PlaySoundEx("VO_WoolFox_06_Death_432551616", vPosition);
		break;

	case Client::EMonsterType::ELEC_PANDA:
		m_wpSoundManager.lock()->PlaySoundEx("VO_ElecPanda_06_Death_2_839150214", vPosition);
		break;

	case Client::EMonsterType::GRASS_MAMMOTH:
		m_wpSoundManager.lock()->PlaySoundEx("VO_GrassMammoth_06_Death_2_1072616691", vPosition);
		break;

	default:
		break;
	}

	// 렉돌 상태로 변경합니다
	switch (m_eMonsterType)
	{
	case Client::EMonsterType::BERRY_GOAT:
	case Client::EMonsterType::BOAR:
	case Client::EMonsterType::DREAM_DEMON:
	case Client::EMonsterType::HEDGE_HOG:
	case Client::EMonsterType::KITSUNEBI:
	case Client::EMonsterType::PENGUIN:
	case Client::EMonsterType::WOOL_FOX:
	case Client::EMonsterType::PINK_CAT:
	case Client::EMonsterType::SHEEP_BALL:
		m_wpMonster.lock()->GetBodyColliderComponent()->SetLockFlags(0);
		break;

	case Client::EMonsterType::ELEC_PANDA:
	case Client::EMonsterType::GRASS_MAMMOTH:
		break;

	default:
		break;
	}

	// 아이템을 생성합니다.
	const vector<EItemType>& vecItemTypes = CClientLoop::s_spClientDataSheet->k_mapPalPossibleDrops[m_eMonsterType];
	for (const auto& eItemType : vecItemTypes)
	{
		switch (eItemType)
		{
		// ingredient
		case Client::EItemType::EGG:
		{
			int32 iRandomNum = CMathUtility::GetRandomInt(3, 5);

			if (!m_wpGetItemAnimationImageComponentSlot1.lock()->IsActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpInventorySystem.lock()->AddItemToContainer(eItemType, iRandomNum);
				m_wpGetItemAnimationImageComponentSlot1.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot1.lock()->SetTextureFileName(L"T_get_egg");
			}
		}
		break;

		case Client::EItemType::SHEEPBALL_MEAT:
		{
			int32 iRandomNum = CMathUtility::GetRandomInt(3, 5);

			if (!m_wpGetItemAnimationImageComponentSlot1.lock()->IsActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpInventorySystem.lock()->AddItemToContainer(eItemType, iRandomNum);
				m_wpGetItemAnimationImageComponentSlot1.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot1.lock()->SetTextureFileName(L"T_get_sheepball_meat");
			}
		}
		break;

		case Client::EItemType::BOAR_MEAT:
		{
			int32 iRandomNum = CMathUtility::GetRandomInt(3, 5);

			if (!m_wpGetItemAnimationImageComponentSlot0.lock()->IsActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpInventorySystem.lock()->AddItemToContainer(eItemType, iRandomNum);
				m_wpGetItemAnimationImageComponentSlot0.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetTextureFileName(L"T_get_boar_meat");
			}
		}
		break;

		case Client::EItemType::RED_BERRY:
		{
			int32 iRandomNum = CMathUtility::GetRandomInt(3, 5);

			if (!m_wpGetItemAnimationImageComponentSlot0.lock()->IsActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpInventorySystem.lock()->AddItemToContainer(eItemType, iRandomNum);
				m_wpGetItemAnimationImageComponentSlot0.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetTextureFileName(L"T_get_berry");
			}
		}
		break;

		// mineral
		case Client::EItemType::WOOL:
		{
			int32 iRandomNum = CMathUtility::GetRandomInt(4, 6);

			if (!m_wpGetItemAnimationImageComponentSlot0.lock()->IsActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpInventorySystem.lock()->AddItemToContainer(eItemType, 10);
				m_wpGetItemAnimationImageComponentSlot0.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetTextureFileName(L"T_get_wool");
			}
		}
		break;

		case Client::EItemType::IRON:
		{
			int32 iRandomNum = CMathUtility::GetRandomInt(1, 3);
			
			if (!m_wpGetItemAnimationImageComponentSlot0.lock()->IsActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpInventorySystem.lock()->AddItemToContainer(eItemType, iRandomNum);
				m_wpGetItemAnimationImageComponentSlot0.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetTextureFileName(L"T_get_iron");
			}
		}
		break;

		case Client::EItemType::LEATHER:
		{
			int32 iRandomNum = CMathUtility::GetRandomInt(4, 6);

			if (!m_wpGetItemAnimationImageComponentSlot1.lock()->IsActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpInventorySystem.lock()->AddItemToContainer(eItemType, 6);
				m_wpGetItemAnimationImageComponentSlot1.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot1.lock()->SetTextureFileName(L"T_get_leather");
			}
		}
		break;

		case Client::EItemType::GRASSMAMMOTH_MEAT:
		{
			int32 iRandomNum = CMathUtility::GetRandomInt(1, 3);

			if (!m_wpGetItemAnimationImageComponentSlot0.lock()->IsActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpInventorySystem.lock()->AddItemToContainer(eItemType, iRandomNum);
				m_wpGetItemAnimationImageComponentSlot0.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetTextureFileName(L"T_get_greenmoth_meat");
			}
		}
		break;

		}
	}

	// 표정을 변경합니다.
	m_wpMonster.lock()->ChangeEye(EPalEyeType::FAINT);
	m_wpMonster.lock()->ChangeMouth(EPalMouthType::DAMAGED);

	// 불리언 값을 초기화합니다.
	m_wpMonster.lock()->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACK, false);
	m_wpMonster.lock()->ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, false);
	m_wpMonster.lock()->ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, false);

	m_spMeshComponent->ChangeAnimation(m_iAnimationID, false);
}

EBTNodeResult Client::CTaskDead::ExecuteTaskNode(float _fDeltaSecond)
{
	if (m_wpMonster.lock()->GetBoolValue(EBlackboardBoolValue::BOOL_DEAD) == false) { return EBTNodeResult::FAILED; }

	switch (m_eMonsterType)
	{
	case Client::EMonsterType::ELEC_PANDA:
	case Client::EMonsterType::GRASS_MAMMOTH:
	{
		const FModelAnimData& tAnimationData = m_spMeshComponent->GetModel()->GetModelAnimationData();
		if (tAnimationData.m_tBaseAnimData.m_bLoopEnd && tAnimationData.m_tBaseAnimData.m_iCurrentAnimID == m_iAnimationID)
		{
			m_wpMonster.lock()->SetActive(false);

			// 사망 이펙트를 출력합니다.
			m_wpEffectManager.lock()->ActivePalDeadParticle(m_wpMonster.lock(), m_wpMonster.lock()->GetMeshComponent()->GetModel());

			return EBTNodeResult::SUCCEEDED;
		}
	}
	break;

	default:
	{
		m_fSumDeadDeltaSeconds += _fDeltaSecond;
		if (m_fMaxDeadDeltaSeconds <= m_fSumDeadDeltaSeconds)
		{
			m_wpMonster.lock()->SetActive(false);

			// 사망 이펙트를 출력합니다.
			m_wpEffectManager.lock()->ActivePalDeadParticle(m_wpMonster.lock(), m_wpMonster.lock()->GetMeshComponent()->GetModel());

			// 팰박스 거점팰이 도축 당할 경우에 대한 처리입니다.
			if (m_wpMonster.lock()->GetMonsterAIType() == EMonsterAIType::SLAVE_PAL)
			{
				m_wpPalBoxSystem.lock()->ExecuteSlavePal(m_wpMonster.lock());

				int32 iRandomNum = CMathUtility::GetRandomInt(3, 5);

				if (!m_wpGetItemAnimationImageComponentSlot1.lock()->IsActive())
				{
					m_wpSoundManager.lock()->PlaySoundEx("GetItem");
					m_wpInventorySystem.lock()->AddItemToContainer(EItemType::SHEEPBALL_MEAT, iRandomNum);
					m_wpGetItemAnimationImageComponentSlot1.lock()->ResetFrame();
					m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(true);
					m_wpGetItemAnimationImageComponentSlot1.lock()->SetTextureFileName(L"T_get_sheepball_meat");
				}
				if (!m_wpGetItemAnimationImageComponentSlot0.lock()->IsActive())
				{
					m_wpSoundManager.lock()->PlaySoundEx("GetItem");
					m_wpInventorySystem.lock()->AddItemToContainer(EItemType::WOOL, iRandomNum);
					m_wpGetItemAnimationImageComponentSlot0.lock()->ResetFrame();
					m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(true);
					m_wpGetItemAnimationImageComponentSlot0.lock()->SetTextureFileName(L"T_get_wool");
				}
			}

			return EBTNodeResult::SUCCEEDED;
		}
	}
	break;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskDead::Release()
{
	m_spMeshComponent = nullptr;
	m_wpMonster.lock() = nullptr;

	CTaskNode::Release();
}
