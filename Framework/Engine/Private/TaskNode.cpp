#include "pch.h"
#include "TaskNode.h"

HRESULT Engine::CTaskNode::Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc)
{
	if (FAILED(CBTNode::Initialize(_spNodeDesc))) { return E_FAIL; };

	return S_OK;
}

void Engine::CTaskNode::OnNodeInit(float _fDeltaSecond)
{
	return;
}

void Engine::CTaskNode::OnNodeEnter(float _fDeltaSecond)
{
	return;
}

EBTNodeResult Engine::CTaskNode::ExecuteNode(float _fDeltaSecond)
{
	// ���ڷ����� ��带 �����մϴ�.
	EBTNodeResult eDecoNodeResult = CBTNode::ExecuteNode(_fDeltaSecond);
	switch (eDecoNodeResult)
	{
	case Engine::EBTNodeResult::DECO_FAILED:
		m_eCurBTNodeResult = eDecoNodeResult;
		return eDecoNodeResult;

	case Engine::EBTNodeResult::DECO_SUCCEEDED:
		break;

	default:
		CHECKF(false, L"BT node result is not designated.");
		break;
	}

	// �½�ũ�� �ʱ�ȭ�մϴ�.

	if (m_eBTNodeType == EBTNodeType::TASK)
	{
		switch (m_eCurBTNodeResult)
		{
		case Engine::EBTNodeResult::SUCCEEDED:
		case Engine::EBTNodeResult::FAILED:
		case Engine::EBTNodeResult::ENUM_END:
			OnNodeInit(_fDeltaSecond);
			break;

		default:
			break;
		}
	}

	// �׽�ũ ��带 �����մϴ�.
	EBTNodeResult eTaskNodeResult = ExecuteTaskNode(_fDeltaSecond);
	switch (eTaskNodeResult)
	{
	case Engine::EBTNodeResult::SUCCEEDED:
	{
		if (m_eCurBTNodeResult != EBTNodeResult::IN_PROGRESS)
		{
			OnNodeEnter(_fDeltaSecond);
		}

		// TODO; ���⼭ �ʱ�ȭ�ؾ� �ұ�?
		m_eCurBTNodeResult = eTaskNodeResult;
		return eTaskNodeResult;
	}
	break;

	case Engine::EBTNodeResult::IN_PROGRESS:
	{
		if (m_eCurBTNodeResult != EBTNodeResult::IN_PROGRESS)
		{
			OnNodeEnter(_fDeltaSecond);
		}
		m_eCurBTNodeResult = eTaskNodeResult;
		return eTaskNodeResult;
	}

	case Engine::EBTNodeResult::FAILED:
	case Engine::EBTNodeResult::ABORT:
		m_eCurBTNodeResult = eTaskNodeResult;
		return eTaskNodeResult;

	default:
		CHECKF(false, L"BT node result is not designated.");
		break;
	}

	CHECKF(false, L"BT node result is not designated.");
	return EBTNodeResult::ENUM_END;
}

void Engine::CTaskNode::Release()
{
	CBTNode::Release();
}

EBTNodeResult Engine::CTaskNode::ExecuteTaskNode(float _fDeltaSecond)
{
	return EBTNodeResult::ENUM_END;
}
