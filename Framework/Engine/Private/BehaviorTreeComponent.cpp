#include "pch.h"
#include "BehaviorTreeComponent.h"

#include "BTNode.h"
#include "CompositeNode.h"
#include "TaskNode.h"

shared_ptr<CBehaviorTreeComponent> Engine::CBehaviorTreeComponent::Create(const std::shared_ptr<FBTComponentDesc>& _spBTComponentDesc)
{
	shared_ptr<CBehaviorTreeComponent> spBehaviorTreeComponent = make_shared<CBehaviorTreeComponent>();
	ENSUREF(spBehaviorTreeComponent->InitializeComponent(_spBTComponentDesc), L"Failed to initialize behaviortree component.");
	return spBehaviorTreeComponent;
}

HRESULT Engine::CBehaviorTreeComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component desc
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// bahaviortree component desc
	shared_ptr<FBTComponentDesc> spBTComponentDesc = static_pointer_cast<FBTComponentDesc>(_spComponentDesc);
	m_spRootNode = spBTComponentDesc->m_spRootNode;
	m_bBehaveTree = spBTComponentDesc->m_bBehaveTree;

	// etc
	if (m_spRootNode)
	{
		m_spRootNode->SetOwnerGameObject(m_wpOwnerGameObject.lock());
	}
	return S_OK;
}

HRESULT Engine::CBehaviorTreeComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CBehaviorTreeComponent::PreTickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CBehaviorTreeComponent::TickComponent(float _fDeltaSeconds)
{
	if (m_bBehaveTree && m_spRootNode)
	{
		m_spRootNode->ExecuteNode(_fDeltaSeconds);
	}
	return 0;
}

int32 Engine::CBehaviorTreeComponent::PostTickComponent(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Engine::CBehaviorTreeComponent::EndPlay()
{
	return S_OK;
}

void Engine::CBehaviorTreeComponent::Render(int32 _iShaderPath)
{
}

void Engine::CBehaviorTreeComponent::Release()
{
	for (auto& pair : m_umapBTNodes)
	{
		pair.second = nullptr;
	}
	m_umapBTNodes.clear();

	for (auto& spBTNode : m_vecRootBTNodes)
	{
		spBTNode = nullptr;
	}
	m_vecRootBTNodes.clear();

	m_spRootNode->Release();
	m_spRootNode = nullptr;
}

HRESULT Engine::CBehaviorTreeComponent::BindVariable()
{
	return S_OK;
}

void Engine::CBehaviorTreeComponent::ResetBTNodeResult()
{
	for (auto& spBTNode : m_vecRootBTNodes)
	{
		spBTNode->ResetBTNodeResult();
	}
}

HRESULT Engine::CBehaviorTreeComponent::ResetBTNodeResult(const std::wstring& _wstrBTNodeName)
{
	if (!m_umapBTNodes.contains(_wstrBTNodeName)) { return E_FAIL; }

	m_umapBTNodes[_wstrBTNodeName]->ResetBTNodeResult();
	return S_OK;
}

HRESULT Engine::CBehaviorTreeComponent::ResetBTNodeResult(const std::shared_ptr<CBTNode>& _spBTRootNode)
{
	_spBTRootNode->ResetBTNodeResult();

	return S_OK;
}

HRESULT Engine::CBehaviorTreeComponent::ResetBehindBTNodeResult()
{
	int32 iChildNodeIndex = m_spRootNode->GetCurRootBasedChildNodeIndex();
	if (iChildNodeIndex < 0)
	{
		CHECKF(false, L"Child node index is -1.");
		return E_FAIL;
	}

	int32 iSize = (int32)m_vecRootBTNodes.size();
	if (iSize <= iChildNodeIndex)
	{
		CHECKF(false, L"Child node index is overflowed.");
		return E_FAIL;
	}

	for (int32 iIndex = iChildNodeIndex + 1; iIndex < iSize; ++iIndex)
	{
		m_vecRootBTNodes[iIndex]->ResetBTNodeResult();
	}
	
	return S_OK;
}

HRESULT Engine::CBehaviorTreeComponent::AddBTNode(const std::shared_ptr<CBTNode>& _spBTNode)
{
	const wstring& wstrBTNodeName = _spBTNode->GetBTNodeName();

	if (m_umapBTNodes.contains(wstrBTNodeName)) { return E_FAIL; }

	m_umapBTNodes.emplace(wstrBTNodeName, _spBTNode);
	return S_OK;
}

HRESULT Engine::CBehaviorTreeComponent::AddRootBTNode(const std::shared_ptr<CBTNode>& _spBTNode)
{
	m_vecRootBTNodes.emplace_back(_spBTNode);
	return S_OK;
}
