#include "pch.h"
#include "TaskCaptured.h"

#include "EffectManager.h"
#include "EffectGroup.h"

#include "Monster.h"
#include "BehaviorTreeComponent.h"
#include "MeshComponent.h"

std::shared_ptr<CTaskCaptured> Client::CTaskCaptured::Create(const std::shared_ptr<FTaskCapturedDesc>& _spTaskCapturedDesc)
{
	shared_ptr<CTaskCaptured> spTaskNode = make_shared<CTaskCaptured>();
	ENSUREF(spTaskNode->Initialize(_spTaskCapturedDesc), L"Failed to initialize captured task node.");
	return spTaskNode;
}

HRESULT Client::CTaskCaptured::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// task desc
	if (FAILED(CTaskNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	m_wpEffectManager = CEffectManager::GetInstance();

	// captured task desc
	shared_ptr<FTaskCapturedDesc> spTaskCapturedDesc = static_pointer_cast<FTaskCapturedDesc>(_spNodeDesc);

	// etc
	m_wpMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	return S_OK;
}

void Client::CTaskCaptured::OnNodeInit(float _fDeltaSecond)
{
	m_wpMonster.lock()->SetCulled(true);
	m_wpMonster.lock()->GetMeshComponent()->SetUseAnimation(false);
	m_wpMonster.lock()->GetBoolValue(EBlackboardBoolValue::BOOL_CAPTURE_RESULT) ? m_bCaptured = true : m_bCaptured = false;
	
	EMonsterType eMonsterType = m_wpMonster.lock()->GetMonsterType();
	switch (eMonsterType)
	{
	case Client::EMonsterType::BERRY_GOAT:
		break;

	case Client::EMonsterType::BOAR:
	{
		shared_ptr<CGameObject> spGameObject = m_wpMonster.lock()->GetGameObjectValue(EBlackboardGameObjectValue::EFFECT_GROUP_00);
		if (spGameObject)
		{
			shared_ptr<CEffectGroup> spEffectGroup = static_pointer_cast<CEffectGroup>(spGameObject);
			spEffectGroup->EndLifeTime();
		}
		m_wpMonster.lock()->StopSoundAll();
	}
	break;

	case Client::EMonsterType::CARBUNCLO:
		break;

	case Client::EMonsterType::CHICKEN_PAL:
		break;

	case Client::EMonsterType::DREAM_DEMON:
		break;

	case Client::EMonsterType::ELEC_PANDA:
		break;

	case Client::EMonsterType::GRASS_MAMMOTH:
		break;

	case Client::EMonsterType::HEDGE_HOG:
		break;

	case Client::EMonsterType::HEDGE_HOG_ICE:
		break;

	case Client::EMonsterType::KITSUNEBI:
		break;

	case Client::EMonsterType::PENGUIN:
		break;

	case Client::EMonsterType::PINK_CAT:
		break;

	case Client::EMonsterType::SHEEP_BALL:
	{
		shared_ptr<CGameObject> spGameObject = m_wpMonster.lock()->GetGameObjectValue(EBlackboardGameObjectValue::EFFECT_GROUP_00);
		if (spGameObject)
		{
			shared_ptr<CEffectGroup> spEffectGroup = static_pointer_cast<CEffectGroup>(spGameObject);
			spEffectGroup->EndLifeTime();
		}
		m_wpMonster.lock()->StopSoundAll();
	}
	break;

	case Client::EMonsterType::WOOL_FOX:
		break;

	default:
		break;
	}

	// 전투 시스템에서 제외합니다.
	m_wpMonster.lock()->ParticipateOutCombat();
}

void Client::CTaskCaptured::OnNodeEnter(float _fDeltaSecond)
{
	// 표정을 변경합니다.
	m_wpMonster.lock()->ChangeEye(EPalEyeType::DAMAGED);
	m_wpMonster.lock()->ChangeMouth(EPalMouthType::DAMAGED);
}

EBTNodeResult Client::CTaskCaptured::ExecuteTaskNode(float _fDeltaSecond)
{
	// 팰 스피어에 포획된 경우
	if (m_bCaptured)
	{
		m_wpMonster.lock()->StopSoundAll();
		m_wpMonster.lock()->ModifyBoolValue(EBlackboardBoolValue::BOOL_CAPTURED, false);
		m_wpMonster.lock()->ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, false);
		m_wpBehaviorTreeComponent.lock()->ResetBehindBTNodeResult();
		m_wpMonster.lock()->GetMeshComponent()->SetUseAnimation(true);

		return EBTNodeResult::SUCCEEDED;
	}

	// 팰 스피어에 포획되지 않은 경우
	if (m_wpMonster.lock()->GetBoolValue(EBlackboardBoolValue::BOOL_ESCAPE))
	{
		m_wpMonster.lock()->PariticipateInCombat() ;
		m_wpMonster.lock()->ModifyBoolValue(EBlackboardBoolValue::BOOL_CAPTURED, false);
		m_wpMonster.lock()->ModifyBoolValue(EBlackboardBoolValue::BOOL_JUMP, true);
		m_wpMonster.lock()->ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, true);
		m_wpMonster.lock()->ModifyBoolValue(EBlackboardBoolValue::BOOL_ESCAPE, false);
		m_wpMonster.lock()->GetMeshComponent()->SetUseAnimation(true);

		m_wpBehaviorTreeComponent.lock()->ResetBehindBTNodeResult();
		return EBTNodeResult::SUCCEEDED;
	}

	return EBTNodeResult::IN_PROGRESS;
}

void Client::CTaskCaptured::Release()
{
	m_wpMonster.reset();

	CTaskNode::Release();
}
