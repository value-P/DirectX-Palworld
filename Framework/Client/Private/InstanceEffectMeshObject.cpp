#include "pch.h"
#include "InstanceEffectMeshObject.h"

#include "InstanceEffectMeshComponent.h"

shared_ptr<CInstanceEffectMeshObject> Client::CInstanceEffectMeshObject::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, FEffectMeshInfo _tEffectMeshInfo, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo)
{
	shared_ptr<CInstanceEffectMeshObject> spInstance = make_shared<CInstanceEffectMeshObject>();
	CHECKF(spInstance, L"CInstanceEffectMeshObject : Create Failed");

	spInstance->SetObjectEffectMeshInfo(_tEffectMeshInfo);
	spInstance->SetObjectInstanceEffectMeshInfo(_tInstanceEffectMeshInfo);
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

shared_ptr<CInstanceEffectMeshObject> Client::CInstanceEffectMeshObject::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CInstanceEffectMeshObject> spInstance = make_shared<CInstanceEffectMeshObject>();
	CHECKF(spInstance, L"CInstanceEffectMeshObject : Create Failed");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT Client::CInstanceEffectMeshObject::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CEffectBase::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FInstanceEffectMeshObjectDesc> spInstanceEffectMeshObjectDesc = static_pointer_cast<FInstanceEffectMeshObjectDesc>(_spGameObjectDesc);

	/* gameobject */
	m_matOrigin = Matrix::CreateScale(m_tEffectMeshInfo.m_vScale) *
				  Matrix::CreateRotationX(m_tEffectMeshInfo.m_vRotation.x) *
				  Matrix::CreateRotationY(m_tEffectMeshInfo.m_vRotation.y) *
				  Matrix::CreateRotationZ(m_tEffectMeshInfo.m_vRotation.z) *
				  Matrix::CreateTranslation(m_tEffectMeshInfo.m_vTranslation);

	/* component */
	FInstanceEffectMeshComponentDesc tInstanceEffectMeshDesc;

	tInstanceEffectMeshDesc.m_wpOwnerGameObject = shared_from_this();
	tInstanceEffectMeshDesc.m_eComponentType = EComponentType::MESH;
	tInstanceEffectMeshDesc.m_wstrComponentName = L"Effect Mesh Component";
	tInstanceEffectMeshDesc.m_eRenderType = m_tEffectMeshInfo.m_eRenderType;

	tInstanceEffectMeshDesc.m_eEffectType = EEffectType::InstanceEffectMesh;

	tInstanceEffectMeshDesc.m_wstrJsonFileName = L"";
	tInstanceEffectMeshDesc.m_wstrModelFilePath = spInstanceEffectMeshObjectDesc->m_wstrModelFilePath;
	tInstanceEffectMeshDesc.m_matPivot = spInstanceEffectMeshObjectDesc->m_matPivot;

	m_spInstanceEffectMesh = CInstanceEffectMeshComponent::Create(make_shared<FInstanceEffectMeshComponentDesc>(tInstanceEffectMeshDesc), m_tEffectMeshInfo, m_tInstanceEffectMeshInfo);
	AddComponent(L"Particle Component", m_spInstanceEffectMesh);

	return S_OK;
}

HRESULT Client::CInstanceEffectMeshObject::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectBase::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FInstanceEffectMeshObjectDesc> spInstanceEffectMeshObjectDesc = static_pointer_cast<FInstanceEffectMeshObjectDesc>(_spGameObjectDesc);

	/* gameobject */
	m_matOrigin = Matrix::CreateScale(m_tEffectMeshInfo.m_vScale) *
				  Matrix::CreateRotationX(m_tEffectMeshInfo.m_vRotation.x) *
				  Matrix::CreateRotationY(m_tEffectMeshInfo.m_vRotation.y) *
				  Matrix::CreateRotationZ(m_tEffectMeshInfo.m_vRotation.z) *
				  Matrix::CreateTranslation(m_tEffectMeshInfo.m_vTranslation);

	/* component */
	FInstanceEffectMeshComponentDesc tInstanceEffectMeshDesc;

	tInstanceEffectMeshDesc.m_wpOwnerGameObject = shared_from_this();
	tInstanceEffectMeshDesc.m_eComponentType = EComponentType::MESH;
	tInstanceEffectMeshDesc.m_wstrComponentName = L"Instance Mesh Component";
	tInstanceEffectMeshDesc.m_eRenderType = m_tEffectMeshInfo.m_eRenderType;

	tInstanceEffectMeshDesc.m_eEffectType = EEffectType::InstanceEffectMesh;

	tInstanceEffectMeshDesc.m_wstrJsonFileName = spInstanceEffectMeshObjectDesc->m_wstrJsonFileName;
	tInstanceEffectMeshDesc.m_wstrModelFilePath = L"";
	tInstanceEffectMeshDesc.m_matPivot = spInstanceEffectMeshObjectDesc->m_matPivot;

	m_spInstanceEffectMesh = CInstanceEffectMeshComponent::CreateFromJson(make_shared<FInstanceEffectMeshComponentDesc>(tInstanceEffectMeshDesc), _wstrJsonFileName);
	AddComponent(L"Instance Mesh Component", m_spInstanceEffectMesh);

	return S_OK;
}

HRESULT Client::CInstanceEffectMeshObject::BeginPlay()
{
	if (FAILED(CEffectBase::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CInstanceEffectMeshObject::PreTick(float _fDeltaSeconds)
{
	switch (CEffectBase::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 Client::CInstanceEffectMeshObject::Tick(float _fDeltaSeconds)
{
	switch (CEffectBase::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 Client::CInstanceEffectMeshObject::PostTick(float _fDeltaSeconds)
{
	switch (CEffectBase::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

HRESULT Client::CInstanceEffectMeshObject::EndPlay()
{
	if (FAILED(CEffectBase::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CInstanceEffectMeshObject::Release()
{
	CEffectBase::Release();
}

uint32 Client::CInstanceEffectMeshObject::GetCurKeyFrameIndex()
{
	return m_spInstanceEffectMesh->GetCurKeyFrameIndex();
}

std::vector<FEffectMeshKeyFrame>& Client::CInstanceEffectMeshObject::GetKeyFrames()
{
	return m_spInstanceEffectMesh->GetKeyFrames();
}