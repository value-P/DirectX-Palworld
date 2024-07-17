// Default
#include "pch.h"
#include "Ranch.h"

// Module, Manager
#include "ClientModule.h"
#include "EffectManager.h"
#include "GameManager.h"
#include "QuestSystem.h"

// Component
#include "MeshComponent.h"
#include "BoxComponent.h"

std::shared_ptr<CRanch> Client::CRanch::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CRanch> spRanch = make_shared<CRanch>();
	CHECKF(spRanch, L"Failed to create CRanch : CRanch");
	spRanch->Initialize(_spGameObjectDesc);

	return spRanch;
}

HRESULT Client::CRanch::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CArchitecture::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_eArchitectureType = EArchitectureType::RANCH;

	shared_ptr<FRanchDesc> spRanchDesc = static_pointer_cast<FRanchDesc>(_spGameObjectDesc);
	InitializeMeshComponent();
	InitializeColliderComponent();
	return S_OK;
}

HRESULT Client::CRanch::BeginPlay()
{
	if (FAILED(CArchitecture::BeginPlay())) { return E_FAIL; }

	// 찌릿도치 목장 예외
	if (m_wstrGameObjectName == L"Dochi_Ranch")
	{
		OffBluePrint();
	}

	return S_OK;
}

int32 Client::CRanch::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CRanch::Tick(float _fDeltaSeconds)
{
	switch (CArchitecture::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CRanch::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CRanch::EndPlay()
{
	if (FAILED(CArchitecture::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CRanch::Release()
{
	CArchitecture::Release();
}

HRESULT Client::CRanch::InitializeMeshComponent()
{
	// 건축물 메쉬 생성
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName		= L"MeshComponent";
	tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();

	tMeshComponentDesc.m_wstrModelFilePath		= m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), XMConvertToRadians(-90.f)));
	tMeshComponentDesc.m_wstrShaderName			= m_wstrShaderName;

	shared_ptr<CMeshComponent> spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));

	m_umapMeshComponents.emplace(L"MeshComponent", spMeshCom);
	return AddComponent(L"MeshComponent", spMeshCom);
}

HRESULT Client::CRanch::InitializeColliderComponent()
{
	// box component desc
	FBoxComponentDesc tBoxComponentDesc;

	tBoxComponentDesc.m_vExtents			= Vector3(2.0f, 1.0f, 2.0f);
	tBoxComponentDesc.m_vLocalPosition		= m_vColliderCenter;
	tBoxComponentDesc.m_vLocalRotate		= m_vColliderRotate;

	// collider component desc
	tBoxComponentDesc.m_eMobilityType		= EMobilityType::STATIC;
	tBoxComponentDesc.m_eColliderType		= EColliderType::BOX;
	tBoxComponentDesc.m_eCollisionFlag		= ECollisionFlag::BUILDING;
	tBoxComponentDesc.m_eCollisionLayer		= ECollisionLayer::BUILDING;
	tBoxComponentDesc.m_bGravity			= false;
	tBoxComponentDesc.m_bTrigger			= true;
	tBoxComponentDesc.m_iAxisLockType		= 0;

	// component desc
	tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tBoxComponentDesc.m_wstrComponentName	= L"BoxComponent";
	tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(L"BoxComponent", m_spBoxComponent);
}

void Client::CRanch::CompleteQuest()
{
	CGameManager::GetInstance()->GetQuestSystem()->SetCurrentQuestTrue(0);
}
