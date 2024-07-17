#include "pch.h"
#include "DecoratorDay.h"

#include "EngineModule.h"
#include "TimerManager.h"

#include "BehaviorTreeComponent.h"
#include "Date.h"

std::shared_ptr<CDecoratorDay> Client::CDecoratorDay::Create(const std::shared_ptr<FDecoratorDayDesc>& _spDecoratorDayDesc)
{
	const std::shared_ptr<CDecoratorDay> spDecoratorDay = make_shared<CDecoratorDay>();
	ENSUREF(spDecoratorDay->Initialize(_spDecoratorDayDesc), L"Failed to initialize day decorator node.");
	return spDecoratorDay;
}

HRESULT Client::CDecoratorDay::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// deco desc
	if (FAILED(CDecoratorNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// day deco desc
	shared_ptr<FDecoratorDayDesc> spDecoDayDesc = static_pointer_cast<FDecoratorDayDesc>(_spNodeDesc);

	// etc
	m_spDate = CEngineModule::GetInstance()->GetTimerManager()->FindDate(L"Date120");
	return S_OK;
}

bool Client::CDecoratorDay::EvaluateDecoNode()
{
	if (m_spDate->IsDay())
	{
		m_wpBehaviorTreeComponent.lock()->ResetBTNodeResult(L"IdleWalkRestSequence");
		return true;
	}

	return false;
}

void Client::CDecoratorDay::Release()
{
	m_spDate = nullptr;

	CDecoratorNode::Release();
}
