#include "pch.h"
#include "KitchenMedieval.h"

#include "ClientModule.h"
#include "EffectManager.h"

#include "EffectGroup.h"

#include "MeshComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CKitchenMedieval> Client::CKitchenMedieval::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	std::shared_ptr<CKitchenMedieval> spKitchenMedieval = make_shared<CKitchenMedieval>();
	ENSUREF(spKitchenMedieval->Initialize(_spGameObjectDesc), L"Failed to initialize kitchen medieval.");
	return spKitchenMedieval;
}

HRESULT Client::CKitchenMedieval::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// factory desc
	if (FAILED(CFactory::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// kitchen medieval desc
	shared_ptr<FKitchenMedievalDesc> spKitchenMedievalDesc = static_pointer_cast<FKitchenMedievalDesc>(_spGameObjectDesc);

	m_eFactoryType = EFactoryType::POT;

	ENSUREF(InitializeMeshComponent(), L"Failed to initialize mesh component.");
	ENSUREF(InitializeColliderComponent(), L"Failed to initialize collider component.");
	return S_OK;
}

HRESULT Client::CKitchenMedieval::BeginPlay()
{
	if (FAILED(CFactory::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CKitchenMedieval::PreTick(float _fDeltaSeconds)
{
	switch (CFactory::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CKitchenMedieval::Tick(float _fDeltaSeconds)
{
	switch (CFactory::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	if (!m_bFireEffectOn && m_tBuildStatus.m_bBuildCompleted)
	{
		m_wpFireEffect = CEffectManager::GetInstance()->ActivateEffect(L"Group_Torch", GetPosition().value() + Vector3(0.f, 0.2f, 0.f));
		m_bFireEffectOn = true;
	}

	return 0;
}

int32 Client::CKitchenMedieval::PostTick(float _fDeltaSeconds)
{
	switch (CFactory::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CKitchenMedieval::EndPlay()
{
	if (FAILED(CFactory::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CKitchenMedieval::Release()
{
	CFactory::Release();
}

void Client::CKitchenMedieval::ClearEffect()
{
	if (!m_wpFireEffect.expired()) { m_wpFireEffect.lock()->EndLifeTime(); }
}

HRESULT Client::CKitchenMedieval::InitializeMeshComponent()
{
	// component desc
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName		= L"KitchenMedievalMeshComponent";
	tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();

	// mesh desc
	tMeshComponentDesc.m_wstrModelFilePath		= m_wstrModelFilePath;
	tMeshComponentDesc.m_wstrShaderName			= m_wstrShaderName;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f)));

	shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	m_umapMeshComponents.emplace(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
	return AddComponent(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
}

HRESULT Client::CKitchenMedieval::InitializeColliderComponent()
{
	// box compzonent desc
	FBoxComponentDesc tBoxComponentDesc;

	tBoxComponentDesc.m_vExtents = m_vColliderExtents;
	tBoxComponentDesc.m_vLocalPosition = m_vColliderCenter;
	tBoxComponentDesc.m_vLocalRotate = m_vColliderRotate;

	// component desc
	tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
	tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tBoxComponentDesc.m_wstrComponentName = L"BodySphereComponent";
	tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

	// collider component desc
	tBoxComponentDesc.m_bTrigger = false;
	tBoxComponentDesc.m_bGravity = true;
	tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
	tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
	tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
	tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxComponent);
}

void Client::CKitchenMedieval::AddItemToFactory(EItemType _eItemType, int32 _iNumItems)
{
	CFactory::AddItemToFactory(_eItemType, _iNumItems);

	m_iCurCommandID = RegisterCommandToSystem(EToDoType::COOK_FOOD, EMonsterTaskType::KINDLE);
}

void Client::CKitchenMedieval::RemoveItemFromFactory()
{
	CFactory::RemoveItemFromFactory();

	ENSURE(UnRegisterCommandFromSystem(EToDoType::COOK_FOOD, m_iCurCommandID));
}
