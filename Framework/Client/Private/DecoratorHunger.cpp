#include "pch.h"
#include "DecoratorHunger.h"

#include "Monster.h"
#include "Architecture.h"

std::shared_ptr<CDecoratorHunger> Client::CDecoratorHunger::Create(const std::shared_ptr<FDecoratorHungerDesc>& _spDecoratorHungerDesc)
{
	const std::shared_ptr<CDecoratorHunger> spDecoratorHunger = make_shared<CDecoratorHunger>();
	ENSUREF(spDecoratorHunger->Initialize(_spDecoratorHungerDesc), L"Failed to initialize hunger decorator node.");
	return spDecoratorHunger;
}

HRESULT Client::CDecoratorHunger::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// deco desc
	if (FAILED(CDecoratorNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// hunger deco desc
	shared_ptr<FDecoratorHungerDesc> spDecoHungerDesc = static_pointer_cast<FDecoratorHungerDesc>(_spNodeDesc);

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	return S_OK;
}

bool Client::CDecoratorHunger::EvaluateDecoNode()
{
	// shared_ptr<CArchitecture> spFeedingBox = m_spMonster->FindArchitecture(EArchitectureType::FEEDING_BOX);

	// 배가 고프고 먹이 상자가 있는 경우
	if (m_spMonster->GetBoolValue(EBlackboardBoolValue::BOOL_HUNGER)/* && spFeedingBox && spFeedingBox->IsBuildComplete()*/)
	{
		return true;
	}

	return false;
}

void Client::CDecoratorHunger::Release()
{
	m_spMonster = nullptr;

	CDecoratorNode::Release();
}
