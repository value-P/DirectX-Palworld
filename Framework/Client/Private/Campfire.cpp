#include "pch.h"
#include "Campfire.h"

#include "ClientModule.h"
#include "EffectManager.h"

#include "MeshComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CCampfire> Client::CCampfire::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	std::shared_ptr<CCampfire> spCameFire = make_shared<CCampfire>();
	ENSUREF(spCameFire->Initialize(_spGameObjectDesc), L"Failed to initialize camepfire.");
	return spCameFire;
}

HRESULT Client::CCampfire::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// factory desc
	if (FAILED(CFactory::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_eFactoryType = EFactoryType::CAMPFIRE;

	// campfire desc
	shared_ptr<FCampfireDesc> spCampfireDesc = static_pointer_cast<FCampfireDesc>(_spGameObjectDesc);

	ENSUREF(InitializeMeshComponent(), L"Failed to initialize mesh component.");
	ENSUREF(InitializeColliderComponent(), L"Failed to initialize collider component.");
	return S_OK;
}

HRESULT Client::CCampfire::BeginPlay()
{
	if (FAILED(CFactory::BeginPlay())) { return E_FAIL; }

	CClientModule::GetInstance()->GetEffectManager()->ActivateEffect(L"Group_BuildDestroy", GetPosition().value());
	return S_OK;
}

int32 Client::CCampfire::PreTick(float _fDeltaSeconds)
{
	switch (CFactory::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CCampfire::Tick(float _fDeltaSeconds)
{
	switch (CFactory::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CCampfire::PostTick(float _fDeltaSeconds)
{
	switch (CFactory::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CCampfire::EndPlay()
{
	if (FAILED(CFactory::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CCampfire::Release()
{
	// todo; 이곳에서 패널을 널포인터르 초기화합니다.

	CFactory::Release();
}

HRESULT Client::CCampfire::InitializeMeshComponent()
{
	// component desc
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName		= L"CampfireMeshComponent";
	tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();

	// mesh desc
	tMeshComponentDesc.m_wstrModelFilePath		= m_wstrModelFilePath;
	tMeshComponentDesc.m_wstrShaderName			= m_wstrShaderName;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

	shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	m_umapMeshComponents.emplace(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
	return AddComponent(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
}

HRESULT Client::CCampfire::InitializeColliderComponent()
{
	// body box component
	{
		// component desc
		FBoxComponentDesc tBoxComponentDesc;
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
		tBoxComponentDesc.m_vLocalPosition = Vector3(0.0f, 1.0f, 0.0f);

		// box component desc
		tBoxComponentDesc.m_vExtents = Vector3(2.0f, 2.0f, 2.0f);

		shared_ptr<CBoxComponent> spBodyBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, spBodyBoxComponent));
	}

	return S_OK;
}

void Client::CCampfire::AddItemToFactory(EItemType _eItemType, int32 _iNumItems)
{
	CFactory::AddItemToFactory(_eItemType, _iNumItems);

	m_iCurCommandID = RegisterCommandToSystem(EToDoType::PRODUCE_ITEM, EMonsterTaskType::KINDLE);
}

void Client::CCampfire::RemoveItemFromFactory()
{
	CFactory::RemoveItemFromFactory();

	ENSURE(UnRegisterCommandFromSystem(EToDoType::PRODUCE_ITEM, m_iCurCommandID));
}
