// Default
#include "pch.h"
#include "BlastFurnace.h"

// Module, Manager
#include "ClientModule.h"
#include "EffectManager.h"

// GameObject
#include "EffectGroup.h"

// Component
#include "MeshComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CBlastFurnace> Client::CBlastFurnace::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CBlastFurnace> spBlastFurnace = make_shared<CBlastFurnace>();
	ENSUREF(spBlastFurnace->Initialize(_spGameObjectDesc), L"Failed to initialize blast furnace.");
	return spBlastFurnace;
}

HRESULT Client::CBlastFurnace::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// factory desc
	if (FAILED(CFactory::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_eFactoryType = EFactoryType::BLAST_FURNACE;

	// blast furnace desc
	shared_ptr <FBlastFurnaceDesc> spBlastFurnaceDesc = static_pointer_cast<FBlastFurnaceDesc>(_spGameObjectDesc);
	ENSURE(InitializeMeshComponent());
	ENSURE(InitializeColliderComponent());

	return S_OK;
}

HRESULT Client::CBlastFurnace::BeginPlay()
{
	if (FAILED(CFactory::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CBlastFurnace::PreTick(float _fDeltaSeconds)
{
	switch (CFactory::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CBlastFurnace::Tick(float _fDeltaSeconds)
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

int32 Client::CBlastFurnace::PostTick(float _fDeltaSeconds)
{
	switch (CFactory::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CBlastFurnace::EndPlay()
{
	if (FAILED(CFactory::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CBlastFurnace::Release()
{
	m_spSelectPanel = nullptr;

	CFactory::Release();
}

void Client::CBlastFurnace::ClearEffect()
{
	if (!m_wpFireEffect.expired()) { m_wpFireEffect.lock()->EndLifeTime(); }
}

HRESULT Client::CBlastFurnace::InitializeMeshComponent()
{
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName		= L"MeshComponent";
	tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath		= m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot				= Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
	tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();
	tMeshComponentDesc.m_wstrShaderName			= m_wstrShaderName;

	shared_ptr<CMeshComponent> spMeshComoponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	m_umapMeshComponents.emplace(L"MeshComponent", spMeshComoponent);
	return AddComponent(L"MeshComponent", spMeshComoponent);
}

HRESULT Client::CBlastFurnace::InitializeColliderComponent()
{
	// box component desc
	FBoxComponentDesc tBoxComponentDesc;

	tBoxComponentDesc.m_vExtents			= m_vColliderExtents;
	tBoxComponentDesc.m_vLocalPosition		= m_vColliderCenter;
	tBoxComponentDesc.m_vLocalRotate		= m_vColliderRotate;

	// collider component desc
	tBoxComponentDesc.m_bTrigger			= false;
	tBoxComponentDesc.m_bGravity			= true;
	tBoxComponentDesc.m_eMobilityType		= EMobilityType::STATIC;
	tBoxComponentDesc.m_eColliderType		= EColliderType::BOX;
	tBoxComponentDesc.m_eCollisionFlag		= ECollisionFlag::BUILDING;
	tBoxComponentDesc.m_eCollisionLayer		= ECollisionLayer::BUILDING;

	// component desc
	tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tBoxComponentDesc.m_wstrComponentName	= L"BoxComponent";
	tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(L"BoxComponent", m_spBoxComponent);
}

void Client::CBlastFurnace::AddItemToFactory(EItemType _eItemType, int32 _iNumItems)
{
	CFactory::AddItemToFactory(_eItemType, _iNumItems);

	m_iCurCommandID = RegisterCommandToSystem(EToDoType::PRODUCE_ITEM, EMonsterTaskType::KINDLE);
}

void Client::CBlastFurnace::RemoveItemFromFactory()
{
	CFactory::RemoveItemFromFactory();

	ENSURE(UnRegisterCommandFromSystem(EToDoType::PRODUCE_ITEM, m_iCurCommandID));
}
