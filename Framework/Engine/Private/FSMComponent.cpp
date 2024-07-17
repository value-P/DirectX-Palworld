#include "pch.h"
#include "FSMComponent.h"
#include "MeshComponent.h"

void Engine::CFSMComponent::AddState(int32 _iState, shared_ptr<CFSMNode> _spNode)
{
	CHECKF(_spNode, L"CFSMComponent::AddUpperState : 노드가nullptr 입니다	");
	m_vecNode[_iState] = _spNode;
	_spNode->SetOwnerFSMComponent(static_pointer_cast<CFSMComponent>(shared_from_this()));
}

shared_ptr<CFSMComponent> Engine::CFSMComponent::Create(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	shared_ptr<CFSMComponent> spFSMComponent = make_shared<CFSMComponent>();
	ENSUREF(spFSMComponent->InitializeComponent(_spComponentDesc), L"Failed to initialize FSM component.");
	return spFSMComponent;
}

HRESULT Engine::CFSMComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component desc
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; };

	// FSM component desc
	shared_ptr<FFSMComponentDesc> tFSMComponentDesc = static_pointer_cast<FFSMComponentDesc>(_spComponentDesc);
	m_iNumState = tFSMComponentDesc->m_iNumState;
	m_iCurrentState = tFSMComponentDesc->m_iInitialState;

	// 노드를 예약합니다.
	m_vecNode.resize(m_iNumState);
	return S_OK;
}

int32 Engine::CFSMComponent::PreTickComponent(float _fDeltaSeconds)
{
	if (m_eNodeState == ENodeState::OnExit)
	{
		m_vecNode[m_iCurrentState]->OnExitState();
		m_iPrevState = m_iCurrentState;
		m_iCurrentState = m_iNextState;
		m_eNodeState = ENodeState::OnEnter;
	}

	if (m_eNodeState == ENodeState::OnEnter)
	{
		m_vecNode[m_iCurrentState]->OnEnterState();

		m_eNodeState = ENodeState::Running;

		m_vecNode[m_iCurrentState]->OnState(_fDeltaSeconds);
	}

	m_vecNode[m_iCurrentState]->KeyInput();
	return 0;
}

int32 Engine::CFSMComponent::TickComponent(float _fDeltaSeconds)
{
	switch (m_eNodeState)
	{
	case Engine::CFSMComponent::ENodeState::Running:
	{
		m_vecNode[m_iCurrentState]->OnState(_fDeltaSeconds);
	}
	break;
	}

	return 0;
}

void Engine::CFSMComponent::Release()
{
	for (auto& spFSMNode : m_vecNode)
	{
		spFSMNode = nullptr;
	}

	m_vecNode.clear();
}

void Engine::CFSMComponent::ChangeState(int32 _iNextState)
{
	if (m_iNextState == _iNextState || m_iCurrentState == _iNextState)
		return;

	m_iNextState = _iNextState;
	m_eNodeState = ENodeState::OnExit;
}