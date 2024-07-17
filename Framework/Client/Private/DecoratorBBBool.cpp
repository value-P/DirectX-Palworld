#include "pch.h"
#include "DecoratorBBBool.h"

#include "Monster.h"

std::shared_ptr<CDecoratorBBBool> Client::CDecoratorBBBool::Create(const std::shared_ptr<FDecoratorBBBoolDesc>& _spDecoratorBBBoolDesc)
{
	const std::shared_ptr<CDecoratorBBBool> spDecoratorBool = make_shared<CDecoratorBBBool>();
	ENSUREF(spDecoratorBool->Initialize(_spDecoratorBBBoolDesc), L"Failed to initialize bool decorator node.");
	return spDecoratorBool;
}

HRESULT Client::CDecoratorBBBool::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// deco desc
	if (FAILED(CDecoratorNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// bool deco desc
	shared_ptr<FDecoratorBBBoolDesc> spDecoBoolDesc = static_pointer_cast<FDecoratorBBBoolDesc>(_spNodeDesc);
	m_eBoolValue = spDecoBoolDesc->m_eBoolValue;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	return S_OK;
}

bool Client::CDecoratorBBBool::EvaluateDecoNode()
{
	CHECKF(m_eBoolValue != EBlackboardBoolValue::ENUM_END, L"Blackboard value is enum end.");

	return m_spMonster->GetBoolValue(m_eBoolValue);
}

void Client::CDecoratorBBBool::Release()
{
	m_spMonster = nullptr;

	CDecoratorNode::Release();
}
