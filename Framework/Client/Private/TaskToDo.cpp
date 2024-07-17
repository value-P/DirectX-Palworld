#include "pch.h"
#include "TaskToDo.h"

// manager
#include "EffectManager.h"

// system
#include "ToDoSystem.h"

// component
#include "MeshComponent.h"
#include "ColliderComponent.h"

// gameobject
#include "Monster.h"
#include "Chest.h"
#include "Item.h"
#include "Mineral.h"
#include "Ingredient.h"
#include "EffectGroup.h"

// engine
#include "Model.h"
#include "Bone.h"
#include "ClientLoop.h"

std::shared_ptr<CTaskToDo> Client::CTaskToDo::Create(const std::shared_ptr<FTaskToDoDesc>& _spTaskToDoDesc)
{
	shared_ptr<CTaskToDo> spTaskNode = make_shared<CTaskToDo>();
	ENSUREF(spTaskNode->Initialize(_spTaskToDoDesc), L"Failed to initialize todo task node.");
	return spTaskNode;
}

HRESULT Client::CTaskToDo::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task node desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// todo task desc
	shared_ptr<FTaskToDoDesc> spTaskToDoDesc = static_pointer_cast<FTaskToDoDesc>(_spNodeDesc);
	m_iProduceAnimationID = spTaskToDoDesc->m_iProduceAnimationID;
	m_iBuildAnimationID = spTaskToDoDesc->m_iBuildAnimationID;
	m_iMineAnimationID = spTaskToDoDesc->m_iMineAnimationID;
	m_iCutDownAnimationID = spTaskToDoDesc->m_iCutDownAnimationID;
	m_iTransportAnimationID = spTaskToDoDesc->m_iTransportAnimationID;
	m_iPlantAnimationID = spTaskToDoDesc->m_iPlantAnimationID;
	m_iCookStartAnimationID = spTaskToDoDesc->m_iCookStartAnimationID;
	m_iCookLoopAnimationID = spTaskToDoDesc->m_iCookLoopAnimationID;

	// etc
	m_spMonster			= dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	m_spMeshComponent	= m_spMonster->GetMeshComponent();

	m_wpMouthBone = m_spMeshComponent->GetModel()->GetBonePtr(L"Socket_Mouth");

	return S_OK;
}

void Client::CTaskToDo::OnNodeInit(float _fDeltaSecond)
{
	m_vPrePosition = nullopt;
	if (m_spMonster->GetToDoCommand() == nullptr)
	{
		m_eToDoType = EToDoType::ENUM_END;
		return;
	}

	Matrix matCombined = m_wpMouthBone.lock()->GetCombinedTransformationMatrix();
	Matrix matObject = m_spMonster->GetTransformMatrix().value();
	Matrix matFinal = matCombined * matObject;

	switch (m_eToDoType = m_spMonster->GetToDoCommand()->m_eToDoType)
	{
	case EToDoType::PRODUCE_ITEM:
	{
		switch (m_spMonster->GetMonsterType())
		{
		case EMonsterType::KITSUNEBI:
			m_spMeshComponent->ChangeAnimation(m_iCookLoopAnimationID, false);

			// 불 뿜기 이펙트를 재생합니다.
			m_wpFireBreath = m_wpEffectManager.lock()->ActivateEffect(L"Group_Fiho_FireBreath", m_spMonster, matFinal.Translation());
			break;

		default:
			m_spMeshComponent->ChangeAnimation(m_iProduceAnimationID, false);
			break;
		}
	}
	break;

	case EToDoType::BUILD_ARCHITECTURE:
		m_spMeshComponent->ChangeAnimation(m_iBuildAnimationID, false);
		break;

	case EToDoType::CUT_DOWN_RAW_TREE:
	case EToDoType::CUT_DOWN_LUMBERYARD:
		m_spMeshComponent->ChangeAnimation(m_iCutDownAnimationID, false);
		break;

	case EToDoType::MINE_RAW_ROCK:
	case EToDoType::MINE_STONE_PIT:
		m_spMeshComponent->ChangeAnimation(m_iMineAnimationID, false);
		break;

	case EToDoType::TRANSPORT_MINERAL:
	case EToDoType::TRANSPORT_INGREDIENT:
	{
		shared_ptr<CItem> spItem = m_spMonster->GetToDoCommand()->m_spItem;

		switch (spItem->GetItemKindType())
		{
		case EItemKindType::MINERAL:
			static_pointer_cast<CMineral>(spItem)->SetGravity(false);
			break;

		case EItemKindType::INGREDIENT:
			static_pointer_cast<CIngredient>(spItem)->SetGravity(false);
			break;

		default:
			CHECKF(false, L"Item kind type is not designated.");
			break;
		}

		m_spMeshComponent->ChangeAnimation(m_iTransportAnimationID, false);
	}
	break;

	case EToDoType::FARM_BERRY:
		m_spMeshComponent->ChangeAnimation(m_iPlantAnimationID, false);
		break;
		
	case EToDoType::COOK_FOOD:
		m_spMeshComponent->ChangeAnimation(m_iCookLoopAnimationID, false);

		// 불 뿜기 이펙트를 재생합니다.
		m_wpFireBreath = m_wpEffectManager.lock()->ActivateEffect(L"Group_Fiho_FireBreath", m_spMonster, matFinal.Translation());
		break;

	default:
		CHECKF(false, L"To do type is not designated.");
		break;
	}

	switch (m_eToDoType)
	{
	case Client::EToDoType::BUILD_ARCHITECTURE:
		m_spMonster->GrabTool(EItemType::CONSTRUCT_HAMMER, true);
		break;

	default:
		break;
	}
}

void Client::CTaskToDo::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_spMonster->ChangeEye(EPalEyeType::DEFAULT);
	m_spMonster->ChangeMouth(EPalMouthType::DEFAULT);
}

EBTNodeResult Client::CTaskToDo::ExecuteTaskNode(float _fDeltaSecond)
{
	if (!m_vPrePosition.has_value())
	{
		m_vPrePosition = m_spMonster->GetPosition().value();
	}
	m_spMonster->SetPosition(m_vPrePosition.value());

	Matrix matCombined = m_wpMouthBone.lock()->GetCombinedTransformationMatrix();
	Matrix matObject = m_spMonster->GetTransformMatrix().value();
	Matrix matFinal = matCombined * matObject;

	switch (m_eToDoType)
	{
	case Client::EToDoType::BUILD_ARCHITECTURE:
	{
		bool bCompleted = m_spMonster->ExecuteToDoCommand();
		if (bCompleted)
		{
			m_spMonster->GrabTool(EItemType::CONSTRUCT_HAMMER, false);
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_FINISH, true);
			m_vPrePosition = nullopt;

			m_wpEffectManager.lock()->ActivateEffect(L"Group_Slave_TodoComplete", m_spMonster->GetPosition().value() + Vector3(0.f, 1.f, 0.f));
			return EBTNodeResult::SUCCEEDED;
		}

		return EBTNodeResult::IN_PROGRESS;
	}
	break;

	case Client::EToDoType::TRANSPORT_MINERAL:
	case Client::EToDoType::TRANSPORT_INGREDIENT:
	case Client::EToDoType::PRODUCE_ITEM:
	case Client::EToDoType::CUT_DOWN_RAW_TREE:
	case Client::EToDoType::CUT_DOWN_LUMBERYARD:
	case Client::EToDoType::MINE_RAW_ROCK:
	case Client::EToDoType::MINE_STONE_PIT:
	case Client::EToDoType::FARM_BERRY:
	case Client::EToDoType::COOK_FOOD:
	{
		if (!m_wpFireBreath.expired())
		{
			m_wpFireBreath.lock()->SetPosition(matFinal.Translation());
		}

		bool bCompleted = m_spMonster->ExecuteToDoCommand();
		if (bCompleted)
		{
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_FINISH, true);
			m_vPrePosition = nullopt;
			m_wpEffectManager.lock()->ActivateEffect(L"Group_Slave_TodoComplete", m_spMonster->GetPosition().value() + Vector3(0.f, 1.f, 0.f));
			// 불 뿜기 이펙트를 회수합니다.
			if (!m_wpFireBreath.expired())
			{
				m_wpFireBreath.lock()->EndLifeTime();
				m_wpFireBreath.reset();
			}
			return EBTNodeResult::SUCCEEDED;
		}

		return EBTNodeResult::IN_PROGRESS;
	}
	break;

	case Client::EToDoType::ENUM_END:
	{
		if (m_spMonster->ExecuteToDoCommand())
		{
			m_spMonster->ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_FINISH, true);
			m_vPrePosition = nullopt;

			// 불 뿜기 이펙트를 회수합니다.
			if (!m_wpFireBreath.expired())
			{
				m_wpFireBreath.lock()->EndLifeTime();
				m_wpFireBreath.reset();
			}
			return EBTNodeResult::SUCCEEDED;
		}
		else
		{
			CHECKF(false, L"완료된 작업을 완료되지 않았다고 판단했습니다.");
		}
	}
	break;

	default:
		CHECKF(false, L"To do type is not designated.");
		break;
	}
	return EBTNodeResult::FAILED;
}

void Client::CTaskToDo::Release()
{
	m_spMonster = nullptr;
	m_spMeshComponent = nullptr;

	CTaskNode::Release();
}
