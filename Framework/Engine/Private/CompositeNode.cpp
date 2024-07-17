#include "pch.h"
#include "CompositeNode.h"

shared_ptr<CCompositeNode> Engine::CCompositeNode::Create(const shared_ptr<FCompositeNodeDesc>& _spCompositeNodeDesc)
{
	shared_ptr<CCompositeNode> spICompositeNode = make_shared<CCompositeNode>();
	ENSUREF(spICompositeNode->Initialize(_spCompositeNodeDesc), L"Failed to initialize composite node.");
	return spICompositeNode;
}

HRESULT Engine::CCompositeNode::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	// bt node desc
	if (FAILED(CBTNode::Initialize(_spNodeDesc))) { return E_FAIL; };

	// composite node desc
	shared_ptr<FCompositeNodeDesc> spCompositeNodeDesc = static_pointer_cast<FCompositeNodeDesc>(_spNodeDesc);
	m_eCompositeType = spCompositeNodeDesc->m_eCompositeType;
	return S_OK;
}

void Engine::CCompositeNode::OnNodeEnter(float _fDeltaSecond)
{
	return;
}

EBTNodeResult Engine::CCompositeNode::ExecuteNode(float _fDeltaSecond)
{
	// 데코레이터를 검사합니다.
	if (CBTNode::ExecuteNode(_fDeltaSecond) == EBTNodeResult::DECO_FAILED)
	{
		m_eCurBTNodeResult = EBTNodeResult::DECO_FAILED;
		return m_eCurBTNodeResult;
	}

	switch (m_eCompositeType)
	{
	case Engine::ECompositeType::SELECTOR:
		m_eCurBTNodeResult = ExecuteSelector(_fDeltaSecond);
		break;

	case Engine::ECompositeType::SEQUENCE:
		m_eCurBTNodeResult = ExecuteSequence(_fDeltaSecond);
		break;

	case Engine::ECompositeType::PARALLEL:
		m_eCurBTNodeResult = ExecuteParallel(_fDeltaSecond);
		break;
	}

	return m_eCurBTNodeResult;
}

void Engine::CCompositeNode::OnNodeInit(float _fDeltaSecond)
{
	return;
}

void Engine::CCompositeNode::Release()
{
	CBTNode::Release();
}

EBTNodeResult Engine::CCompositeNode::ExecuteSelector(float _fDeltaSecond)
{

	int32 iSize = (int32)m_vecBTNodes.size();

	// SELECTOR
	for (int32 iIndex = 0; iIndex < iSize; ++iIndex)
	{
		if (m_bRoot) { m_iCurRootChildNodeIndex = iIndex; }

		EBTNodeResult eNodeResult = m_vecBTNodes[iIndex]->ExecuteNode(_fDeltaSecond);
		switch (eNodeResult)
		{
		// 노드가 실행되지 않은 경우
		case EBTNodeResult::DECO_FAILED:
			break;

		// 노드가 실행된 경우
		case EBTNodeResult::SUCCEEDED:
			m_eCurBTNodeResult = eNodeResult;
			return eNodeResult;

		case EBTNodeResult::IN_PROGRESS:
			m_eCurBTNodeResult = eNodeResult;
			return eNodeResult;

		case EBTNodeResult::FAILED:
			break;
		}
	}

	// 모든 노드가 FAILED인 경우, FAILED를 반환합니다.
	m_eCurBTNodeResult = EBTNodeResult::FAILED;
	return m_eCurBTNodeResult;
}

EBTNodeResult Engine::CCompositeNode::ExecuteSequence(float _fDeltaSecond)
{
	// SEQUENCE
	while (true)
	{
		// 예외를 처리합니다.
		if (m_iCurSequenceNodeIndex >= m_vecBTNodes.size())
		{
			m_iCurSequenceNodeIndex = 0;
			break;
		}

		// 데코레이터에서 반환됐을 수 있고, 테스크에서 반환됐을 수 있습니다.
		EBTNodeResult eNodeResult = m_vecBTNodes[m_iCurSequenceNodeIndex]->ExecuteNode(_fDeltaSecond);

		switch (eNodeResult)
		{
		// 노드가 실행되지 않은 경우
		case EBTNodeResult::DECO_FAILED:
			m_eCurBTNodeResult = eNodeResult;
			m_iCurSequenceNodeIndex = 0;
			return eNodeResult;

		// 노드가 실행된 경우
		case EBTNodeResult::SUCCEEDED:
			++m_iCurSequenceNodeIndex;
			break;

		case EBTNodeResult::IN_PROGRESS:
			m_eCurBTNodeResult = eNodeResult;
			return eNodeResult;

		case EBTNodeResult::FAILED:
			m_eCurBTNodeResult = eNodeResult;
			m_iCurSequenceNodeIndex = 0;
			return eNodeResult;
		}
	}

	// 모든 노드가 SUCCEEDED인 경우, SUCCEEDED를 반환합니다.
	m_eCurBTNodeResult = EBTNodeResult::SUCCEEDED;
	return m_eCurBTNodeResult;
}

EBTNodeResult Engine::CCompositeNode::ExecuteParallel(float _fDeltaSecond)
{
	return EBTNodeResult();
}

void Engine::CCompositeNode::ResetBTNodeResult()
{
	CBTNode::ResetBTNodeResult();

	m_iCurSequenceNodeIndex = 0;
}
