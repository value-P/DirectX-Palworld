
#include "pch.h"
#include "EffectMeshObject.h"

#include "EffectMeshComponent.h"

shared_ptr<CEffectMeshObject> Client::CEffectMeshObject::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, FEffectMeshInfo _tEffectMeshInfo)
{
	shared_ptr<CEffectMeshObject> spInstance = make_shared<CEffectMeshObject>();
	CHECKF(spInstance, L"CEffectObject : Create Failed");

	spInstance->SetObjectEffectMeshInfo(_tEffectMeshInfo);
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

shared_ptr<CEffectMeshObject> Client::CEffectMeshObject::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CEffectMeshObject> spInstance = make_shared<CEffectMeshObject>();
	CHECKF(spInstance, L"CEffectObject : Create Failed");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT Client::CEffectMeshObject::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CEffectBase::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FEffectMeshObjectDesc> spEffectObjectDesc = static_pointer_cast<FEffectMeshObjectDesc>(_spGameObjectDesc);

	/* gameobject */
	m_matOrigin = Matrix::CreateScale(m_tEffectMeshInfo.m_vScale) *
				  Matrix::CreateRotationX(m_tEffectMeshInfo.m_vRotation.x) *
				  Matrix::CreateRotationY(m_tEffectMeshInfo.m_vRotation.y) *
				  Matrix::CreateRotationZ(m_tEffectMeshInfo.m_vRotation.z) *
				  Matrix::CreateTranslation(m_tEffectMeshInfo.m_vTranslation);

	/* component */
	FEffectMeshComponentDesc tEffectMeshDesc;

	tEffectMeshDesc.m_wpOwnerGameObject = shared_from_this();
	tEffectMeshDesc.m_eComponentType = EComponentType::MESH;
	tEffectMeshDesc.m_wstrComponentName = L"Effect Mesh Component";
	tEffectMeshDesc.m_eRenderType = m_tEffectMeshInfo.m_eRenderType;

	tEffectMeshDesc.m_eEffectType = EEffectType::EFFECTMESH;

	tEffectMeshDesc.m_wstrJsonFileName = L"";
	tEffectMeshDesc.m_wstrModelFilePath = spEffectObjectDesc->m_wstrModelFilePath;
	tEffectMeshDesc.m_matPivot = spEffectObjectDesc->m_matPivot;

	m_spEffectMesh = CEffectMeshComponent::Create(make_shared<FEffectMeshComponentDesc>(tEffectMeshDesc), m_tEffectMeshInfo);
	AddComponent(L"EffectMesh Component", m_spEffectMesh);

	return S_OK;
}

HRESULT Client::CEffectMeshObject::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectBase::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FEffectMeshObjectDesc> spEffectObjectDesc = static_pointer_cast<FEffectMeshObjectDesc>(_spGameObjectDesc);

	/* gameobject */
	m_matOrigin = Matrix::CreateScale(m_tEffectMeshInfo.m_vScale) *
				  Matrix::CreateRotationX(m_tEffectMeshInfo.m_vRotation.x) *
				  Matrix::CreateRotationY(m_tEffectMeshInfo.m_vRotation.y) *
				  Matrix::CreateRotationZ(m_tEffectMeshInfo.m_vRotation.z) *
				  Matrix::CreateTranslation(m_tEffectMeshInfo.m_vTranslation);

	/* component */
	FEffectMeshComponentDesc tEffectMeshDesc;

	tEffectMeshDesc.m_wpOwnerGameObject = shared_from_this();
	tEffectMeshDesc.m_eComponentType = EComponentType::MESH;
	tEffectMeshDesc.m_wstrComponentName = L"Effect Mesh Component";
	tEffectMeshDesc.m_eRenderType = m_tEffectMeshInfo.m_eRenderType;

	tEffectMeshDesc.m_eEffectType = EEffectType::EFFECTMESH;

	tEffectMeshDesc.m_wstrJsonFileName = spEffectObjectDesc->m_wstrJsonFileName;
	tEffectMeshDesc.m_wstrModelFilePath = L"";
	tEffectMeshDesc.m_matPivot = spEffectObjectDesc->m_matPivot;

	m_spEffectMesh = CEffectMeshComponent::CreateFromJson(make_shared<FEffectMeshComponentDesc>(tEffectMeshDesc), _wstrJsonFileName);
	AddComponent(L"Effect Mesh Component", m_spEffectMesh);

	return S_OK;
}

HRESULT Client::CEffectMeshObject::BeginPlay()
{
	if (FAILED(CEffectBase::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CEffectMeshObject::PreTick(float _fDeltaSeconds)
{
	switch (CEffectBase::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 Client::CEffectMeshObject::Tick(float _fDeltaSeconds)
{
	switch (CEffectBase::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 Client::CEffectMeshObject::PostTick(float _fDeltaSeconds)
{
	switch (CEffectBase::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

HRESULT Client::CEffectMeshObject::EndPlay()
{
	if (FAILED(CEffectBase::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CEffectMeshObject::Release()
{
	CEffectBase::Release();
}

void Client::CEffectMeshObject::RecycleFromPool()
{
}

void Client::CEffectMeshObject::NoneActivate()
{
}

void Client::CEffectMeshObject::EndLifeTime()
{
}

void Client::CEffectMeshObject::Loop()
{
}

uint32 Client::CEffectMeshObject::GetCurKeyFrameIndex()
{
	return m_spEffectMesh->GetCurKeyFrameIndex();
}

std::vector<FEffectMeshKeyFrame>& Client::CEffectMeshObject::GetKeyFrames()
{
	return m_spEffectMesh->GetKeyFrames();
}