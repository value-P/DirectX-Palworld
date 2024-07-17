#include "pch.h"
#include "Landscape.h"

#include "HeightFieldComponent.h"
#include "MeshComponent.h"

std::shared_ptr<CLandscape> Client::CLandscape::Create(const std::shared_ptr<FLandscapeDesc>& _spLandscapeDesc)
{
	const std::shared_ptr<CLandscape> spLandscape = make_shared<CLandscape>();
	ENSUREF(spLandscape->Initialize(_spLandscapeDesc), L"Failed to initialize landscape.");
	return spLandscape;
}

HRESULT Client::CLandscape::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// game object desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// landscape desc
	std::shared_ptr<FLandscapeDesc> spLandscapeDesc = static_pointer_cast<FLandscapeDesc>(_spGameObjectDesc);

	{
		// component desc
		FHeightFieldComponentDesc tHeightFieldComponentDesc;
		tHeightFieldComponentDesc.m_wpOwnerGameObject = shared_from_this();;
		tHeightFieldComponentDesc.m_eComponentType = EComponentType::HEIGHT_FIELD;
		tHeightFieldComponentDesc.m_wstrComponentName = L"HeightFieldComponent";
		tHeightFieldComponentDesc.m_eRenderType = ERenderType::NONE;

		// collider component desc
		tHeightFieldComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tHeightFieldComponentDesc.m_eColliderType = EColliderType::HEIGHT_FIELD;
		tHeightFieldComponentDesc.m_eCollisionLayer = ECollisionLayer::ENVIRONMENT;
		tHeightFieldComponentDesc.m_eCollisionFlag = ECollisionFlag::ENVIRONMENT;

		tHeightFieldComponentDesc.m_fDensity = 1.0f;
		tHeightFieldComponentDesc.m_fMass = 100.0f;
		tHeightFieldComponentDesc.m_bTrigger = false;
		tHeightFieldComponentDesc.m_vMaterial = Vector3(1.0f, 1.0f, 0.05f);
		tHeightFieldComponentDesc.m_bDeferredAdd = true;

		// height field component desc
		tHeightFieldComponentDesc.m_wstrHeightFieldFilePath = spLandscapeDesc->m_wstrLandscapeFilePath;
		tHeightFieldComponentDesc.m_vHeightFieldScale = spLandscapeDesc->m_vLandscapeScale;
		tHeightFieldComponentDesc.m_vHeightFieldRotate = spLandscapeDesc->m_vLandscapeRotate;
		tHeightFieldComponentDesc.m_vHeightFieldTranslate = spLandscapeDesc->m_vLandscapeTranslate;
		tHeightFieldComponentDesc.m_iWidth = spLandscapeDesc->m_iWidth;
		tHeightFieldComponentDesc.m_iHeight = spLandscapeDesc->m_iHeight;

		m_spHeightFieldComponent = CHeightFieldComponent::Create(make_shared<FHeightFieldComponentDesc>(tHeightFieldComponentDesc));
		ENSUREF(AddComponent(L"HeightFieldComponent", m_spHeightFieldComponent), L"Failed to add height field component.");
	}
	
	{
		// component desc
		FMeshComponentDesc tMeshComponentDesc;
		tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";
		tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;

		// mesh component desc
		tMeshComponentDesc.m_wstrModelFilePath = spLandscapeDesc->m_wstrLandscapeFilePath;
		tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateTranslation({ 0.0f, -0.15f, 0.0f });

		tMeshComponentDesc.m_spParentMeshComponent = nullptr;

		m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
		ENSUREF(AddComponent(L"MeshComponent", m_spMeshComponent), L"Failed to add mesh component.");
	}

	return S_OK;
}

HRESULT Client::CLandscape::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CLandscape::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CLandscape::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CLandscape::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

HRESULT Client::CLandscape::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CLandscape::Release()
{
	m_spHeightFieldComponent = nullptr;
	CGameObject::Release();
}
