#include "pch.h"
#include "EnvCollision.h"

#include "TriangleMeshComponent.h"

std::shared_ptr<CEnvCollision> Client::CEnvCollision::Create(const std::shared_ptr<FEnvCollisionDesc>& _spEnvCollisionDesc)
{
	const std::shared_ptr<CEnvCollision> spEnvCollision = make_shared<CEnvCollision>();
	ENSUREF(spEnvCollision->Initialize(_spEnvCollisionDesc), L"Faild to initialize env collision object.");
	return spEnvCollision;
}

HRESULT Client::CEnvCollision::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// game object desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// env collision desc
	std::shared_ptr<FEnvCollisionDesc> spEnvCollisionDesc = static_pointer_cast<FEnvCollisionDesc>(_spGameObjectDesc);

	// component desc
	FTriangleMeshComponentDesc tTriangleMeshComponentDesc;
	tTriangleMeshComponentDesc.m_wpOwnerGameObject			= shared_from_this();
	tTriangleMeshComponentDesc.m_eComponentType				= EComponentType::TRIANGLE_MESH;
	tTriangleMeshComponentDesc.m_wstrComponentName			= L"TriangleMeshComponent";
	tTriangleMeshComponentDesc.m_eRenderType				= ERenderType::NONE;

	// collider component desc
	tTriangleMeshComponentDesc.m_bTrigger					= false;
	tTriangleMeshComponentDesc.m_eColliderType				= EColliderType::TRIANGLE_MESH;
	tTriangleMeshComponentDesc.m_eMobilityType				= EMobilityType::STATIC;
	tTriangleMeshComponentDesc.m_eCollisionLayer			= ECollisionLayer::ENVIRONMENT2;
	tTriangleMeshComponentDesc.m_eCollisionFlag				= ECollisionFlag::ENVIRONMENT2;

	// triangle mesh component desc
	tTriangleMeshComponentDesc.m_bInsertDirectly = true;// spEnvCollisionDesc->m_bInsertDirectly;
	tTriangleMeshComponentDesc.m_wstrTriangleMeshFilePath	= spEnvCollisionDesc->m_wstrEnvCollisionFilePath;

	m_spTriangleMeshComponent = CTriangleMeshComponent::Create(make_shared<FTriangleMeshComponentDesc>(tTriangleMeshComponentDesc));
	return AddComponent(L"TriangleMeshComponent", m_spTriangleMeshComponent);
}

HRESULT Client::CEnvCollision::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CEnvCollision::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CEnvCollision::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CEnvCollision::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

HRESULT Client::CEnvCollision::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CEnvCollision::Release()
{
	m_spTriangleMeshComponent = nullptr;

	CGameObject::Release();
}

HRESULT Client::CEnvCollision::ActivateEnvCollision()
{
	m_spTriangleMeshComponent->SetSimulation(true);
	return S_OK;
}

HRESULT Client::CEnvCollision::DeactivateEnvCollision()
{
	m_spTriangleMeshComponent->SetSimulation(false);
	return S_OK;
}
