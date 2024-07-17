#include "pch.h"
#include "StonePit.h"

#include "ClientModule.h"
#include "EffectManager.h"
#include "UIManager.h"

#include "MeshComponent.h"
#include "BoxComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"
#include "Panel.h"
#include "Model.h"
#include "MeshBuffer.h"
#include "Material.h"

std::shared_ptr<CStonePit> Client::CStonePit::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	std::shared_ptr<CStonePit> spStonePit = make_shared<CStonePit>();
	ENSUREF(spStonePit->Initialize(_spGameObjectDesc), L"Failed to initialize StonePit.");
	return spStonePit;
}

HRESULT Client::CStonePit::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// factory desc
	if (FAILED(CFactory::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// StonePit desc
	shared_ptr<FStonePitDesc> spStonePitDesc = static_pointer_cast<FStonePitDesc>(_spGameObjectDesc);

	m_eFactoryType = EFactoryType::STONE_PIT;

	ENSUREF(InitializeMeshComponent(), L"Failed to initialize mesh component.");
	ENSUREF(InitializeColliderComponent(), L"Failed to initialize collider component.");
	return S_OK;
}

HRESULT Client::CStonePit::BeginPlay()
{
	if (FAILED(CFactory::BeginPlay())) { return E_FAIL; }

	//for (auto& MeshComponentPair : m_umapMeshComponents)
	//{
	//	auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

	//	for (auto& ShaderTypePair : umapMeshInfos)
	//	{
	//		for (auto& tMeshInfo : ShaderTypePair.second)
	//		{
	//			tMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"State", (int32)EArchitextureState::COMPLETE);
	//			tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Alpha", 1.f);
	//			tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", 1.f);

	//			/*상태*/				//m_umapInts.insert({ L"State",	0 });					// EArchitextureState 참고
	//			/*투명도*/			//m_umapFloats.insert({ L"Alpha",	0.5f });			// EArchitextureState::BLUEPRINT 일 때의 투명도	( 0 ~ 1 )		
	//			/*현재 작업 진행률*/	//m_umapFloats.insert({ L"Progress",	0.f });			// EArchitextureState::BUILDING 일 때의 진행도	( 0 ~ 1 )
	//		}
	//	}
	//}

	return S_OK;
}

int32 Client::CStonePit::PreTick(float _fDeltaSeconds)
{
	switch (CFactory::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CStonePit::Tick(float _fDeltaSeconds)
{
	switch (CFactory::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CStonePit::PostTick(float _fDeltaSeconds)
{
	switch (CFactory::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CStonePit::EndPlay()
{
	if (FAILED(CFactory::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CStonePit::Release()
{
	// todo; 이곳에서 패널을 널포인터르 초기화합니다.
	m_spSelectPanel = nullptr;

	CFactory::Release();
}

HRESULT Client::CStonePit::InitializeMeshComponent()
{
#pragma region StonePit_SM_StonePit_Acce
	FMeshComponentDesc tMeshComponentDesc;
	/*tMeshComponentDesc.m_wstrComponentName = L"StonePit_SM_StonePit_Acce";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f)));

	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	shared_ptr<CMeshComponent> spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	CHECKF(spMeshCom, L"Failed to create CMeshComponent : CStonePitPrefab");

	m_umapMeshComponents.emplace(L"StonePit_SM_StonePit_Acce", spMeshCom);
	AddComponent(L"StonePit_SM_StonePit_Acce", spMeshCom);*/
#pragma endregion

#pragma region StonePit_SM_StonePit_Fence
	tMeshComponentDesc.m_wstrComponentName = L"StonePit_SM_StonePit_Fence";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Architecture/StonePit_SM_StonePit_Fence.dat";
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f)));

	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	shared_ptr<CMeshComponent> spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	CHECKF(spMeshCom, L"Failed to create CMeshComponent : CStonePitPrefab");

	m_umapMeshComponents.emplace(L"StonePit_SM_StonePit_Fence", spMeshCom);
	AddComponent(L"StonePit_SM_StonePit_Fence", spMeshCom);
#pragma endregion

#pragma region StonePit_SM_StonePit_RockA
	tMeshComponentDesc.m_wstrComponentName = L"StonePit_SM_StonePit_RockA";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Rock/pal_b00_rock_MineableRocks.dat";
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio * 50.f) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
		Matrix::CreateTranslation(Vector3(-1.5f, 0.f, -1.f));

	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	CHECKF(spMeshCom, L"Failed to create CMeshComponent : CStonePitPrefab");

	m_umapMeshComponents.emplace(L"StonePit_SM_StonePit_RockA", spMeshCom);
	spMeshCom->SetCulled(true);
	AddComponent(L"StonePit_SM_StonePit_RockA", spMeshCom);
#pragma endregion

#pragma region StonePit_SM_StonePit_RockB
	tMeshComponentDesc.m_wstrComponentName = L"StonePit_SM_StonePit_RockB";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Rock/pal_b00_rock_MineableRocks.dat";
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio * 65.f) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
		Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.5f));

	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	CHECKF(spMeshCom, L"Failed to create CMeshComponent : CStonePitPrefab");

	m_umapMeshComponents.emplace(L"StonePit_SM_StonePit_RockB", spMeshCom);
	spMeshCom->SetCulled(true);
	AddComponent(L"StonePit_SM_StonePit_RockB", spMeshCom);
#pragma endregion

#pragma region StonePit_SM_StonePit_RockC
	tMeshComponentDesc.m_wstrComponentName = L"StonePit_SM_StonePit_RockC";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Rock/pal_b00_rock_MineableRocks.dat";
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio * 40.f) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
		Matrix::CreateTranslation(Vector3(1.5f, 0.f, -0.5f));

	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshComponentDesc.m_wstrShaderName = m_wstrShaderName;
	tMeshComponentDesc.m_eMeshType = EMeshManageType::UNIQUE;

	spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	CHECKF(spMeshCom, L"Failed to create CMeshComponent : CStonePitPrefab");

	m_umapMeshComponents.emplace(L"StonePit_SM_StonePit_RockC", spMeshCom);
	spMeshCom->SetCulled(true);
	return AddComponent(L"StonePit_SM_StonePit_RockC", spMeshCom);
#pragma endregion
}

HRESULT Client::CStonePit::InitializeColliderComponent()
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

	shared_ptr<CBoxComponent> spBodyBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(tBoxComponentDesc.m_wstrComponentName, spBodyBoxComponent);
}

void Client::CStonePit::AddItemToFactory(EItemType _eItemType, int32 _iNumItems)
{
	CFactory::AddItemToFactory(_eItemType, _iNumItems);

	m_iCurCommandID = RegisterCommandToSystem(EToDoType::MINE_STONE_PIT, EMonsterTaskType::MINE);
}

void Client::CStonePit::RemoveItemFromFactory()
{
	CFactory::RemoveItemFromFactory();

	ENSURE(UnRegisterCommandFromSystem(EToDoType::MINE_STONE_PIT, m_iCurCommandID));
}

void Client::CStonePit::AddPermanentCommand()
{
	AddItemToFactory(EItemType::STONE, 9999);

	SetWorkPanel(m_spUIManager->GetUsingWorkUI());
	if (m_spWorkPanel)
	{
		Vector3 vPosition = Vector3(GetTransform(ETransformType::POSITION).value().x, GetTransform(ETransformType::POSITION).value().y + m_vColliderExtents.y + 0.7f, GetTransform(ETransformType::POSITION).value().z);
		m_spWorkPanel->SetPivotWorldPoint(vPosition);
		//m_spWorkPanel->SetPivotWorldPoint(Vector3(GetPosition().value().x, GetPosition().value().y + 0.7f, GetPosition().value().z));
		// 아이템 이미지
		m_spWorkPanel->FindComponent<CImageComponent>(L"ItemImage")->SetTextureFileName(m_spUIManager->FindItemUITextureName(EItemType::STONE));
		// 아이템 텍스트
		m_spWorkPanel->FindComponent<CTextComponent>(L"TitleText")->SetText(m_spUIManager->FindItemName(EItemType::STONE));
		// 만들 아이템 개수
		m_spWorkPanel->FindComponent<CTextComponent>(L"MaxCountText")->SetText(L"9999");
	}
}

bool Client::CStonePit::BuildArchitecture(float _fDeltaSeconds)
{
	// 건축이 완료된 경우
	if (1.0f <= m_tBuildStatus.m_iCurPercentage) { return true; }

	CArchitecture::BuildArchitecture(_fDeltaSeconds);

	if (1.0f <= m_tBuildStatus.m_iCurPercentage)
	{
		m_umapMeshComponents.find(L"StonePit_SM_StonePit_RockA")->second->SetCulled(false);
		m_umapMeshComponents.find(L"StonePit_SM_StonePit_RockB")->second->SetCulled(false);
		m_umapMeshComponents.find(L"StonePit_SM_StonePit_RockC")->second->SetCulled(false);
	}

	return false;
}