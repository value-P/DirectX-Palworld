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
	// ���ڷ����͸� �˻��մϴ�.
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
		// ��尡 ������� ���� ���
		case EBTNodeResult::DECO_FAILED:
			break;

		// ��尡 ����� ���
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

	// ��� ��尡 FAILED�� ���, FAILED�� ��ȯ�մϴ�.
	m_eCurBTNodeResult = EBTNodeResult::FAILED;
	return m_eCurBTNodeResult;
}

EBTNodeResult Engine::CCompositeNode::ExecuteSequence(float _fDeltaSecond)
{
	// SEQUENCE
	while (true)
	{
		// ���ܸ� ó���մϴ�.
		if (m_iCurSequenceNodeIndex >= m_vecBTNodes.size())
		{
			m_iCurSequenceNodeIndex = 0;
			break;
		}

		// ���ڷ����Ϳ��� ��ȯ���� �� �ְ�, �׽�ũ���� ��ȯ���� �� �ֽ��ϴ�.
		EBTNodeResult eNodeResult = m_vecBTNodes[m_iCurSequenceNodeIndex]->ExecuteNode(_fDeltaSecond);

		switch (eNodeResult)
		{
		// ��尡 ������� ���� ���
		case EBTNodeResult::DECO_FAILED:
			m_eCurBTNodeResult = eNodeResult;
			m_iCurSequenceNodeIndex = 0;
			return eNodeResult;

		// ��尡 ����� ���
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

	// ��� ��尡 SUCCEEDED�� ���, SUCCEEDED�� ��ȯ�մϴ�.
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
