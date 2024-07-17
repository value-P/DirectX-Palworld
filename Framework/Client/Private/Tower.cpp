#include "pch.h"
#include "Tower.h"

#include "MeshComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CTower> Client::CTower::Create(const std::shared_ptr<FGameObjectDesc>& _spBossRoomDesc)
{
	const std::shared_ptr<CTower> spInstnace = make_shared<CTower>();
	ENSUREF(spInstnace->Initialize(_spBossRoomDesc), L"Faild to initialize CTower.");
	return spInstnace;
}

HRESULT Client::CTower::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// gameobject desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }
	
	LookAt(GetPosition().value() + Vector3(0.73f, 0.f, -0.68f));

	// mesh component
	{
		// component desc
		FMeshComponentDesc tMeshComponentDesc;
		tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";

		// mesh component desc
		tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Tower.dat";
		tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(-90.0f));
		tMeshComponentDesc.m_spParentMeshComponent = nullptr;
		tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

		shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
		ENSURE(AddComponent(L"MeshComponent", spMeshComponent));
	}

	// box component floor 00
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = Vector3(5.0f, 15.0f, 5.0f);
		tBoxComponentDesc.m_vLocalPosition = Vector3(0.0f);
		tBoxComponentDesc.m_vLocalRotate = Vector3(0.0f);

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::ENVIRONMENT;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::ENVIRONMENT;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName = L"Floor00BoxComponent";

		shared_ptr<CBoxComponent> spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBoxComponent));
	}

	return S_OK;
}

HRESULT Client::CTower::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CTower::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CTower::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CTower::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

HRESULT Client::CTower::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CTower::Release()
{
	CGameObject::Release();
}
