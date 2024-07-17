#include "pch.h"
#include "BTNode.h"
#include "DecoratorNode.h"

#include "BehaviorTreeComponent.h"
#include "GameObject.h"

HRESULT Engine::CBTNode::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	m_eBTNodeType				= _spNodeDesc->m_eBTNodeType;
	m_wstrBTNodeName			= _spNodeDesc->m_wstrBTNodeName;
	m_wpOwnerGameObject			= _spNodeDesc->m_spOwnerGameObject;
	m_bRoot						= _spNodeDesc->m_bRoot;
	m_wpBehaviorTreeComponent	= _spNodeDesc->m_spOwnerGameObject->FindComponent<CBehaviorTreeComponent>(L"BehaviorTreeComponent");
	return S_OK;
}

EBTNodeResult Engine::CBTNode::ExecuteNode(float _fDeltaSecond)
{
	for (auto& spDecoratorNode : m_vecDecoratorNodes)
	{
		if (spDecoratorNode->EvaluateDecoNode() == false)
		{
			return EBTNodeResult::DECO_FAILED;
		}	
	}

	return EBTNodeResult::DECO_SUCCEEDED;
}

void Engine::CBTNode::Release()
{
	for (auto& spDecoNode : m_vecDecoratorNodes)
	{
		spDecoNode->Release();
		spDecoNode = nullptr;
	}
	m_vecDecoratorNodes.clear();

	for (auto& spBTNode : m_vecBTNodes)
	{
		spBTNode->Release();
		spBTNode = nullptr;
	}
	m_vecBTNodes.clear();

	m_spRootBTNode = nullptr;
	m_spParentBTNode = nullptr;
}

HRESULT Engine::CBTNode::AddBTNode(const std::shared_ptr<CBTNode>& _spBTNode)
{
	if ((GetBTNodeType() == EBTNodeType::TASK) && (_spBTNode->GetBTNodeType() != EBTNodeType::DECORATOR))
	{
		CHECKF(false, L"Task node can't have child nodes.");
		return E_FAIL;
	}

	if (GetBTNodeType() == EBTNodeType::DECORATOR)
	{
		CHECKF(false, L"Decorator node can't have child nodes.");
		return E_FAIL;
	}

	_spBTNode->SetOwnerGameObject(m_wpOwnerGameObject.lock());
	_spBTNode->SetParentBTNode(shared_from_this());

	// 루트 노드인 경우
	if (m_bRoot)
	{
		m_wpBehaviorTreeComponent.lock()->AddRootBTNode(_spBTNode);
	}

	m_wpBehaviorTreeComponent.lock()->AddBTNode(_spBTNode);

	switch (_spBTNode->GetBTNodeType())
	{
	case EBTNodeType::COMPOSITE:
	case EBTNodeType::TASK:
		m_vecBTNodes.push_back(_spBTNode);
		break;

	case EBTNodeType::DECORATOR:
		m_vecDecoratorNodes.push_back(dynamic_pointer_cast<CDecoratorNode>(_spBTNode));
		break;

	default:
		CHECKF(false, L"BT node type is not desginated.");
		break;
	}

	return E_FAIL;
}

void Engine::CBTNode::ResetBTNodeResult()
{
	// 노드 결과
	m_eCurBTNodeResult = EBTNodeResult::ENUM_END;

	// 데코레이터 노드들
	for (auto& spDecoNode : m_vecDecoratorNodes)
	{
		spDecoNode->ResetBTNodeResult();
	}

	// 비헤이비어 노드들
	for (auto& spBTNode : m_vecBTNodes)
	{
		spBTNode->ResetBTNodeResult();
	}
}