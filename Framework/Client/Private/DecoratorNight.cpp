#include "pch.h"
#include "DecoratorNight.h"

#include "EngineModule.h"
#include "TimerManager.h"

#include "BehaviorTreeComponent.h"
#include "Date.h"

std::shared_ptr<CDecoratorNight> Client::CDecoratorNight::Create(const std::shared_ptr<FDecoratorNightDesc>& _spDecoratorNightdDesc)
{
	const std::shared_ptr<CDecoratorNight> spDecoratorNight = make_shared<CDecoratorNight>();
	ENSUREF(spDecoratorNight->Initialize(_spDecoratorNightdDesc), L"Failed to initialize night decorator node.");
	return spDecoratorNight;
}

HRESULT Client::CDecoratorNight::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// deco desc
	if (FAILED(CDecoratorNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// night deco desc
	shared_ptr<FDecoratorNightDesc> spDecoNightDesc = static_pointer_cast<FDecoratorNightDesc>(_spNodeDesc);

	// etc
	m_spDate = CEngineModule::GetInstance()->GetTimerManager()->FindDate(L"Date120");
	return S_OK;
}

bool Client::CDecoratorNight::EvaluateDecoNode()
{
	if (m_spDate->IsNight())
	{
		m_wpBehaviorTreeComponent.lock()->ResetBTNodeResult(L"IdleWalkRestSequence");
		return true;
	}
	
	return false;
}

void Client::CDecoratorNight::Release()
{
	m_spDate = nullptr;

	CDecoratorNode::Release();
}
