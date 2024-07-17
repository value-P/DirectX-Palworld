#include "pch.h"
#include "FSMNode.h"
#include "MeshComponent.h"
#include "Model.h"

void Engine::CFSMNode::SetOwnerFSMComponent(const std::shared_ptr<CFSMComponent>& _spOwnerComponent)
{
	CHECKF(_spOwnerComponent, L"CFSMComponent::SetOwnerFSMComponent : 연결될 FSM 컴포넌트가 nullptr 입니다");

	m_wpOwnerFSMComponent = _spOwnerComponent;
}

void Engine::CFSMNode::Initialize(const std::shared_ptr<CMeshComponent>& _spMeshComponent)
{
	// 메쉬 컴포넌트 연결
	CHECKF(_spMeshComponent, L"CFSMComponent::InitializeComponent : 연결될 메쉬 컴포넌트가 nullptr 입니다");
	CHECKF(_spMeshComponent->GetModelType() == EModelType::ANIM, L"CFSMComponent::InitializeComponent : 연결된 메쉬 컴포넌트는 반드시 애님메쉬여야 합니다");

	m_wpCombinedMeshComponent = _spMeshComponent;
	m_wpModel = _spMeshComponent->GetModel();
}