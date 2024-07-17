// default
#include "pch.h"
#include "PalBox.h"

// manager
#include "EffectManager.h"
#include "UIManager.h"

// level, layer
#include "Level.h"

// gameObject
#include "EffectGroup.h"
#include "Panel.h"

// component
#include "BoxComponent.h"
#include "MeshComponent.h"
#include "EffectMeshComponent.h"
#include "SphereComponent.h"

// system
#include "ToDoSystem.h"

#include "EngineModule.h"
#include "InputManager.h"

std::shared_ptr<CPalBox> Client::CPalBox::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	const shared_ptr<CPalBox> spPalBox = make_shared<CPalBox>();
	ENSUREF(spPalBox->Initialize(_spGameObjectDesc), L"Failed to initialize pal box.");
	return spPalBox;
}

HRESULT Client::CPalBox::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CArchitecture::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FPalBoxDesc> spPalBoxDesc = static_pointer_cast<FPalBoxDesc>(_spGameObjectDesc);

	InitializeMeshComponent();
	InitializeColliderComponent();

	m_wpRangeEffect = CEffectManager::GetInstance()->ActivateEffect(L"Group_PalBox_Range", GetPosition().value());

	return S_OK;
}

HRESULT Client::CPalBox::BeginPlay()
{
	if (FAILED(CArchitecture::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CPalBox::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPalBox::Tick(float _fDeltaSeconds)
{
	switch (CArchitecture::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPalBox::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CPalBox::EndPlay()
{
	if (FAILED(CArchitecture::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CPalBox::Release()
{
	m_spBaseCampSphereComponent = nullptr;

	CArchitecture::Release();
}

void Client::CPalBox::ClearEffect()
{
	if (!m_wpRangeEffect.expired()) { m_wpRangeEffect.lock()->EndLifeTime(); }
}

HRESULT Client::CPalBox::InitializeMeshComponent()
{
	// 건축물 메쉬 생성
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName = L"PalBox MeshComponent";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_LBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	shared_ptr<CMeshComponent> spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));

	m_umapMeshComponents.emplace(L"PalBox MeshComponent", spMeshCom);
	return AddComponent(L"PalBox MeshComponent", spMeshCom);
}

HRESULT Client::CPalBox::InitializeColliderComponent()
{
	// body collider component
	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents = m_vColliderExtents;
		tBoxComponentDesc.m_vLocalPosition = m_vColliderCenter;
		tBoxComponentDesc.m_vLocalRotate = m_vColliderRotate;

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::BUILDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::BUILDING;
		tBoxComponentDesc.m_fDensity = 1.0f;
		tBoxComponentDesc.m_fMass = 0.0f;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = false;
		tBoxComponentDesc.m_iAxisLockType = 0;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wstrComponentName = L"BoxComponent";

		m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSUREF(AddComponent(L"BoxComponent", m_spBoxComponent), L"Failed to add the box component.");
	}

	{
		// component desc
		FSphereComponentDesc tSphereComponentDesc;
		tSphereComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tSphereComponentDesc.m_eComponentType = EComponentType::SPHERE;
		tSphereComponentDesc.m_wstrComponentName = L"BaseCampSphereComponent";
		tSphereComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		// collider component desc
		tSphereComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;// 다이나믹일 때만 다른 콜라이더 컴포넌트와 감지됩니다.
		tSphereComponentDesc.m_eColliderType = EColliderType::SPHERE;
		tSphereComponentDesc.m_eCollisionLayer = ECollisionLayer::BASE_CAMP;
		tSphereComponentDesc.m_eCollisionFlag = ECollisionFlag::BASE_CAMP;
		tSphereComponentDesc.m_bGravity = false;
		tSphereComponentDesc.m_bTrigger = true;

		// sphere component desc
		tSphereComponentDesc.m_fRadius = m_fRangeRadius;

		m_spBaseCampSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		ENSUREF(AddComponent(L"BaseCampSphereComponent", m_spBaseCampSphereComponent), L"Failed to add the sphere component.");
	}

	return S_OK;
}