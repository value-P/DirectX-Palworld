#include "pch.h"
#include "DecoratorSkillType.h"

#include "Monster.h"

std::shared_ptr<CDecoratorSkillType> Client::CDecoratorSkillType::Create(const std::shared_ptr<FDecoratorSkillTypeDesc>& _spDecoratorHungerDesc)
{
	const std::shared_ptr<CDecoratorSkillType> spDecorator = make_shared<CDecoratorSkillType>();
	ENSUREF(spDecorator->Initialize(_spDecoratorHungerDesc), L"Failed to initialize skill type decorator node.");
	return spDecorator;
}

HRESULT Client::CDecoratorSkillType::Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// deco desc
	if (FAILED(CDecoratorNode::Initialize(_spNodeDesc))) { return E_FAIL; }

	// skill type deco desc
	shared_ptr<FDecoratorSkillTypeDesc> spDecoSkillTypeDesc = static_pointer_cast<FDecoratorSkillTypeDesc>(_spNodeDesc);
	m_iSkillType = spDecoSkillTypeDesc->m_iSkillType;

	// etc
	m_spMonster = dynamic_pointer_cast<CMonster>(m_wpOwnerGameObject.lock());
	return S_OK;
}

bool Client::CDecoratorSkillType::EvaluateDecoNode()
{
	if (m_spMonster->GetIntValue(EBlackboardIntValue::INT_SKILL_TYPE) == -1)
	{
		m_spMonster->ModifyIntValue(EBlackboardIntValue::INT_SKILL_TYPE, 0);
		return true;
	}

	if (m_spMonster->GetIntValue(EBlackboardIntValue::INT_SKILL_TYPE) == m_iSkillType)
	{
		return true;
	}

	return false;
}

void Client::CDecoratorSkillType::Release()
{
	m_spMonster = nullptr;

	CDecoratorNode::Release();
}
