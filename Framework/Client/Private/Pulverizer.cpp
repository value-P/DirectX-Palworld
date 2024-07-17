#include "pch.h"
#include "Pulverizer.h"

#include "ClientModule.h"
#include "EffectManager.h"

#include "MeshComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CPulverizer> Client::CPulverizer::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	std::shared_ptr<CPulverizer> spPulverizer = make_shared<CPulverizer>();
	ENSUREF(spPulverizer->Initialize(_spGameObjectDesc), L"Failed to initialize pulverizer.");
	return spPulverizer;
}

HRESULT Client::CPulverizer::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// factory desc
	if (FAILED(CFactory::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// pulverizer desc
	shared_ptr<FPulverizerDesc> spPulverizerDesc = static_pointer_cast<FPulverizerDesc>(_spGameObjectDesc);

	ENSUREF(InitializeMeshComponent(), L"Failed to initialize mesh component.");
	ENSUREF(InitializeColliderComponent(), L"Failed to initialize collider component.");
	return S_OK;
}

HRESULT Client::CPulverizer::BeginPlay()
{
	if (FAILED(CFactory::BeginPlay())) { return E_FAIL; }

	CClientModule::GetInstance()->GetEffectManager()->ActivateEffect(L"Group_BuildDestroy", GetPosition().value());
	return S_OK;
}

int32 Client::CPulverizer::PreTick(float _fDeltaSeconds)
{
	switch (CFactory::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPulverizer::Tick(float _fDeltaSeconds)
{
	switch (CFactory::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPulverizer::PostTick(float _fDeltaSeconds)
{
	switch (CFactory::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CPulverizer::EndPlay()
{
	if (FAILED(CFactory::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CPulverizer::Release()
{
	// todo; 이곳에서 패널을 널포인터르 초기화합니다.
	m_spSelectPanel = nullptr;

	CFactory::Release();
}

HRESULT Client::CPulverizer::InitializeMeshComponent()
{
	// component desc
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName		= L"PulverizerMeshComponent";
	tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();

	// mesh desc
	tMeshComponentDesc.m_wstrModelFilePath		= m_wstrModelFilePath;
	tMeshComponentDesc.m_wstrShaderName			= m_wstrShaderName;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), XMConvertToRadians(180.f)));

	shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	m_umapMeshComponents.emplace(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
	return AddComponent(tMeshComponentDesc.m_wstrComponentName, spMeshComponent);
}

HRESULT Client::CPulverizer::InitializeColliderComponent()
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

void Client::CPulverizer::AddItemToFactory(EItemType _eItemType, int32 _iNumItems)
{
	CFactory::AddItemToFactory(_eItemType, _iNumItems);

	m_iCurCommandID = RegisterCommandToSystem(EToDoType::PRODUCE_ITEM, EMonsterTaskType::HANDIWORK);
}

void Client::CPulverizer::RemoveItemFromFactory()
{
	CFactory::RemoveItemFromFactory();

	ENSURE(UnRegisterCommandFromSystem(EToDoType::PRODUCE_ITEM, m_iCurCommandID));
}
