// Default
#include "pch.h"
#include "BerryFarm.h"

// Module, Manager
#include "ClientModule.h"
#include "EffectManager.h"
#include "UIManager.h"

// Component
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"
#include "Panel.h"

std::shared_ptr<CBerryFarm> Client::CBerryFarm::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CBerryFarm> spBerryFarm = make_shared<CBerryFarm>();
	ENSUREF(spBerryFarm->Initialize(_spGameObjectDesc), L"Failed to initialize berry farm.");
	return spBerryFarm;
}

HRESULT Client::CBerryFarm::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CFactory::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FBerryFarmDesc> spBerryFarmDesc = static_pointer_cast<FBerryFarmDesc>(_spGameObjectDesc);

	m_eFactoryType = EFactoryType::BERRY_FARM;

	ENSUREF(InitializeMeshComponent(), L"Failed to initialize mesh component.");
	ENSUREF(InitializeColliderComponent(), L"Failed to initialize collider component.");
	return S_OK;
}

HRESULT Client::CBerryFarm::BeginPlay()
{
	if (FAILED(CFactory::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CBerryFarm::PreTick(float _fDeltaSeconds)
{
	switch (CFactory::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CBerryFarm::Tick(float _fDeltaSeconds)
{
	switch (CFactory::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CBerryFarm::PostTick(float _fDeltaSeconds)
{
	switch (CFactory::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CBerryFarm::EndPlay()
{
	if (FAILED(CFactory::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CBerryFarm::Release()
{
	CFactory::Release();
}

HRESULT Client::CBerryFarm::InitializeMeshComponent()
{
	// 건축물 메쉬 생성
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName		= L"MeshComponent";
	tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath		= m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), XMConvertToRadians(-90.f)));
	tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();
	tMeshComponentDesc.m_wstrShaderName			= m_wstrShaderName;

	shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));

	m_umapMeshComponents.emplace(L"Main MeshComponent", spMeshComponent);
	AddComponent(L"MeshComponent", spMeshComponent);

	for (uint16 i = 0; i < 2; ++i)
	{
		for (uint16 j = 0; j < 3; ++j)
		{
			// 1
			tMeshComponentDesc.m_wstrComponentName = L"BerryBush" + to_wstring(i) + L"_" + to_wstring(j);
			tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND_NONCULL;
			tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
			tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Architecture/BerryBush.dat";
			tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio * 0.015f) *
				Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
				Matrix::CreateTranslation(Vector3(-2.f + 2.f * j, 0.f, 1.f + (-2.f * i)));

			tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
			tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
			tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

			spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));

			m_umapMeshComponents.emplace(L"BerryBush" + to_wstring(i) + L"_" + to_wstring(j) + L"_1", spMeshComponent);
			AddComponent(L"BerryBush" + to_wstring(i) + L"_" + to_wstring(j) + L"_1", spMeshComponent);
		}
	}

	return S_OK;
}

HRESULT Client::CBerryFarm::InitializeColliderComponent()
{
	// box component desc
	FBoxComponentDesc tBoxComponentDesc;

	tBoxComponentDesc.m_vExtents			= m_vColliderExtents;
	tBoxComponentDesc.m_vLocalPosition		= m_vColliderCenter;
	tBoxComponentDesc.m_vLocalRotate		= m_vColliderRotate;

	// collider component desc
	tBoxComponentDesc.m_eMobilityType		= EMobilityType::STATIC;
	tBoxComponentDesc.m_eColliderType		= EColliderType::BOX;
	tBoxComponentDesc.m_eCollisionFlag		= ECollisionFlag::BUILDING;
	tBoxComponentDesc.m_eCollisionLayer		= ECollisionLayer::BUILDING;
	tBoxComponentDesc.m_bGravity			= false;
	tBoxComponentDesc.m_bTrigger			= false;

	// component desc
	tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
	tBoxComponentDesc.m_wstrComponentName	= L"BoxComponent";

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(L"BoxComponent", m_spBoxComponent);
}

void Client::CBerryFarm::AddItemToFactory(EItemType _eItemType, int32 _iNumItems)
{
	CFactory::AddItemToFactory(_eItemType, _iNumItems);

	m_iCurCommandID = RegisterCommandToSystem(EToDoType::FARM_BERRY, EMonsterTaskType::PLANT);
}

void Client::CBerryFarm::RemoveItemFromFactory()
{
	CFactory::RemoveItemFromFactory();

	ENSURE(UnRegisterCommandFromSystem(EToDoType::FARM_BERRY, m_iCurCommandID));
}

void Client::CBerryFarm::AddPermanentCommand()
{
	AddItemToFactory(EItemType::RED_BERRY, 9999);

	SetWorkPanel(m_spUIManager->GetUsingWorkUI());
	if (m_spWorkPanel)
	{
		// 위치를 동기화시켜줍니다.
		Vector3 vPosition = Vector3(GetTransform(ETransformType::POSITION).value().x, GetTransform(ETransformType::POSITION).value().y + m_vColliderExtents.y + 0.7f, GetTransform(ETransformType::POSITION).value().z);
		m_spWorkPanel->SetPivotWorldPoint(vPosition);
		//m_spWorkPanel->SetPivotWorldPoint(Vector3(GetPosition().value().x, GetPosition().value().y + 0.7f, GetPosition().value().z));
		// 아이템 이미지
		m_spWorkPanel->FindComponent<CImageComponent>(L"ItemImage")->SetTextureFileName(m_spUIManager->FindItemUITextureName(EItemType::RED_BERRY));
		// 아이템 텍스트
		m_spWorkPanel->FindComponent<CTextComponent>(L"TitleText")->SetText(m_spUIManager->FindItemName(EItemType::RED_BERRY));
		// 만들 아이템 개수
		m_spWorkPanel->FindComponent<CTextComponent>(L"MaxCountText")->SetText(L"9999");
	}
}
