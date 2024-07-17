#include "pch.h"
#include "DecoratorSleep.h"

#include "Monster.h"

std::shared_ptr<CDecoratorSleep> Client::CDecoratorSleep::Create(const std::shared_ptr<FDecoratorSleepDesc>& _spDecoratorSleepDesc)
{
	const std::shared_ptr<CDecoratorSleep> spDecoratorSleep = make_shared<CDecoratorSleep>();
	ENSUREF(spDecoratorSleep->Initialize(_spDecoratorSleepDesc), L"Failed to initialize sleep decorator node.");
	return spDecoratorSleep;
}

HRESULT Client::CDecoratorSleep::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// deco desc
	if (FAILED(CDecoratorNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// dead deco desc
	shared_ptr<FDecoratorSleepDesc> spDecoSleepDesc = static_pointer_cast<FDecoratorSleepDesc>(_spNodeDesc);

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	return S_OK;
}

bool Client::CDecoratorSleep::EvaluateDecoNode()
{
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_SLEEP))
	{
		// 작업 중에 밤이 되는 경우, 작업을 일시 정지하고 반환합니다.
		m_spMonster->PauseToDoCommand();
		return true;
	}

	return false;
}

void Client::CDecoratorSleep::Release()
{
	m_spMonster = nullptr;

	CDecoratorNode::Release();
}
