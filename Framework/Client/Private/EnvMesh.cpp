#include "pch.h"
#include "EnvMesh.h"

#include "MeshComponent.h"

std::shared_ptr<CEnvMesh> Client::CEnvMesh::Create(const std::shared_ptr<FEnvMeshDesc>& _spEnvMeshDesc)
{
	const std::shared_ptr<CEnvMesh> spEnvMesh = make_shared<CEnvMesh>();
	ENSUREF(spEnvMesh->Initialize(_spEnvMeshDesc), L"Faild to initialize env mesh object.");
	return spEnvMesh;
}

HRESULT Client::CEnvMesh::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// game object desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// env mesh desc
	shared_ptr<FEnvMeshDesc> spEnvMeshDesc = static_pointer_cast<FEnvMeshDesc>(_spGameObjectDesc);
	
	// component desc
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;

	// mesh component desc
	tMeshComponentDesc.m_wstrModelFilePath		= spEnvMeshDesc->m_wstrEnvMeshFilePath;
	tMeshComponentDesc.m_matPivot				= Matrix::CreateScale(0.01f);
	tMeshComponentDesc.m_spParentMeshComponent	= nullptr;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::INSTANCE;

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	return AddComponent(L"MeshComponent", m_spMeshComponent);
}

HRESULT Client::CEnvMesh::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CEnvMesh::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CEnvMesh::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CEnvMesh::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

HRESULT Client::CEnvMesh::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CEnvMesh::Release()
{
	m_spMeshComponent = nullptr;
	CGameObject::Release();
}
