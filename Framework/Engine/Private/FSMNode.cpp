#include "pch.h"
#include "FSMNode.h"
#include "MeshComponent.h"
#include "Model.h"

void Engine::CFSMNode::SetOwnerFSMComponent(const std::shared_ptr<CFSMComponent>& _spOwnerComponent)
{
	CHECKF(_spOwnerComponent, L"CFSMComponent::SetOwnerFSMComponent : ����� FSM ������Ʈ�� nullptr �Դϴ�");

	m_wpOwnerFSMComponent = _spOwnerComponent;
}

void Engine::CFSMNode::Initialize(const std::shared_ptr<CMeshComponent>& _spMeshComponent)
{
	// �޽� ������Ʈ ����
	CHECKF(_spMeshComponent, L"CFSMComponent::InitializeComponent : ����� �޽� ������Ʈ�� nullptr �Դϴ�");
	CHECKF(_spMeshComponent->GetModelType() == EModelType::ANIM, L"CFSMComponent::InitializeComponent : ����� �޽� ������Ʈ�� �ݵ�� �ִԸ޽����� �մϴ�");

	m_wpCombinedMeshComponent = _spMeshComponent;
	m_wpModel = _spMeshComponent->GetModel();
}