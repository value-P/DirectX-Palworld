#include "pch.h"
#include "InventoryCustomObject.h"

#include "ClientLoop.h"
#include "EngineModule.h"
#include "CameraManager.h"
#include "InputManager.h"

#include "MeshComponent.h"
#include "Model.h"
#include "MeshBuffer.h"
#include "Material.h"

std::shared_ptr<CInventoryCustomObject> Client::CInventoryCustomObject::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CInventoryCustomObject> spInstance = make_shared<CInventoryCustomObject>();
	CHECKF(spInstance, L"CInventoryCustomObject : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CInventoryCustomObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FInventoryCustomObjectDesc> spDesc = static_pointer_cast<FInventoryCustomObjectDesc>(_spGameObjectDesc);
	
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();

	InitMesh(spDesc);

	InitDisable();

	FCustomObjectData tCustomData = CClientLoop::s_spClientDataSheet->k_tCustomData;

	ChangeHeadEquipWithID(tCustomData.m_eInitialHeadEquip);
	ChangeOutfitWithID(tCustomData.m_eInitialOutfit);

	return S_OK;
}

HRESULT Client::CInventoryCustomObject::BeginPlay()
{
	CGameObject::BeginPlay();

	return S_OK;
}

int32 Client::CInventoryCustomObject::PreTick(float _fDeltaSeconds)
{
	CGameObject::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CInventoryCustomObject::Tick(float _fDeltaSeconds)
{
	CGameObject::Tick(_fDeltaSeconds);

	Vector3 vCamPos = m_wpCameraManager.lock()->GetCurCameraWorldPosition();
	Vector3 vCamRight = m_wpCameraManager.lock()->GetCurCameraWorldRightVector();
	Vector3 vCamLook = m_wpCameraManager.lock()->GetCurCameraWorldForwardVector();
	Vector3 vCamUp = m_wpCameraManager.lock()->GetCurCameraWorldUpVector();

	SetPosition(vCamPos + vCamRight * m_vCamOffset.x + vCamLook * m_vCamOffset.z - vCamUp * m_vCamOffset.y);

	LookAt(GetPosition().value() - vCamLook);

	return 0;
}

int32 Client::CInventoryCustomObject::PostTick(float _fDeltaSeconds)
{
	CGameObject::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CInventoryCustomObject::EndPlay()
{
	CGameObject::EndPlay();

	return S_OK;
}

void Client::CInventoryCustomObject::Release()
{	
	SharedNull(m_spAnimRefMesh); // ref bone용 메쉬
	SharedNull(m_spHeadMesh);
	SharedNull(m_spHairMesh);

	for (auto& Pair : m_umapHeadEquipMesh)
	{
		SharedNull(Pair.second);
	}
	m_umapHeadEquipMesh.clear();

	for (auto& Pair : m_umapOutfitMesh)
	{
		SharedNull(Pair.second);
	}
	m_umapOutfitMesh.clear();

	CGameObject::Release();
}

void Client::CInventoryCustomObject::SetActive(bool _bActive)
{
	CGameObject::SetActive(_bActive);

	if (_bActive)
	{
		InitDisable();

		ChangeOutfitWithID(m_eCurOutfitID);
		ChangeHeadEquipWithID(m_eCurHeadEquipID);
		ChangeWeaponWithID(m_eCurWeaponID);
	}
}

void Client::CInventoryCustomObject::InitMesh(const std::shared_ptr<FInventoryCustomObjectDesc>& _spGameObjectDesc)
{
	// 기본 몸 모델 관련 초기화
	InitDefaultBodyMesh(_spGameObjectDesc);

	InitWeaponMesh();

	// 머리장식 메쉬 등록
	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/HeadEquip/HeadEquip019_v01.dat",m_umapHeadEquipMesh,EItemType::HeadEquip019);
	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/HeadEquip/HeadEquip021.dat", m_umapHeadEquipMesh,EItemType::HeadEquip021);

	LoadMeshComponent(L"../../Resource/Models/Dat/NonAnim/HeadEquip016.dat", m_umapHeadEquipMesh,EItemType::HeadEquip016, L"Socket_HairAttach_HeadEquip_front");
	LoadMeshComponent(L"../../Resource/Models/Dat/NonAnim/HeadEquip018.dat", m_umapHeadEquipMesh,EItemType::HeadEquip018, L"Socket_HairAttach_HeadEquip_front");
	
	// 옷 메쉬 등록
	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_Cloth001.dat", m_umapOutfitMesh, EItemType::Outfit_Cloth001);
	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_Hunter001.dat", m_umapOutfitMesh, EItemType::Outfit_Hunter001);
	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_OldCloth001.dat", m_umapOutfitMesh, EItemType::Outfit_OldCloth001);
	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_Platinum001_v02.dat", m_umapOutfitMesh, EItemType::Outfit_Platinum001_v02);

	SetCustomColor();
}

void Client::CInventoryCustomObject::InitDefaultBodyMesh(const std::shared_ptr<FInventoryCustomObjectDesc>& _spGameObjectDesc)
{
	FCustomObjectData tCustomData = CClientLoop::s_spClientDataSheet->k_tCustomData;

	// 몸 메시를 생성합니다.
	{
		// component desc
		FMeshComponentDesc tBodyMeshComponentDesc;
		tBodyMeshComponentDesc.m_wstrComponentName = L"BodyMeshComponent";
		tBodyMeshComponentDesc.m_eRenderType = ERenderType::NONE;
		tBodyMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tBodyMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

		// mesh component desc
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/Player_Female.dat";
		tBodyMeshComponentDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
		tBodyMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), XMConvertToRadians(180.f)));

		m_spAnimRefMesh = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tBodyMeshComponentDesc));
		AddComponent(tBodyMeshComponentDesc.m_wstrComponentName, m_spAnimRefMesh);

		m_spAnimRefMesh->ChangeAnimation((int32)EPLAYER_FEMALE::AS_Player_Female_Idle_None, false);
		m_spAnimRefMesh->SetCulled(true);
	}

	// 머리 메시를 생성합니다.
	{
		// component desc
		FMeshComponentDesc tHeadMeshComponentDesc;
		tHeadMeshComponentDesc.m_wstrComponentName = L"HeadMeshComponent";
		tHeadMeshComponentDesc.m_eRenderType = ERenderType::RENDER_POST;
		tHeadMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tHeadMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tHeadMeshComponentDesc.m_spParentMeshComponent = m_spAnimRefMesh;

		// mesh component desc
		tHeadMeshComponentDesc.m_wstrModelFilePath = tCustomData.m_wstrHeadMeshPath;
		tHeadMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f);

		m_spHeadMesh = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tHeadMeshComponentDesc));
		AddComponent(tHeadMeshComponentDesc.m_wstrComponentName, m_spHeadMesh);		
	}

	// 헤어 메시를 생성합니다.
	{
		// component desc
		FMeshComponentDesc tHairMeshComponentDesc;
		tHairMeshComponentDesc.m_wstrComponentName = L"HairMeshComponent";
		tHairMeshComponentDesc.m_eRenderType = ERenderType::RENDER_POST;
		tHairMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tHairMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tHairMeshComponentDesc.m_spParentMeshComponent = m_spAnimRefMesh;

		// mesh component desc
		tHairMeshComponentDesc.m_wstrModelFilePath = tCustomData.m_wstrHairMeshPath;
		tHairMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f);

		m_spHairMesh = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tHairMeshComponentDesc));
		AddComponent(tHairMeshComponentDesc.m_wstrComponentName, m_spHairMesh);
	}
}

void Client::CInventoryCustomObject::InitWeaponMesh()
{
	shared_ptr<CMeshComponent> spWeaponComponent = nullptr;

	// 근거리 무기
	{
		// Axe
		{
			FMeshComponentDesc tWeaponDesc;
			tWeaponDesc.m_wstrComponentName = L"AxeComponent";
			tWeaponDesc.m_eRenderType = ERenderType::RENDER_POST;
			tWeaponDesc.m_eComponentType = EComponentType::MESH;
			tWeaponDesc.m_wpOwnerGameObject = shared_from_this();

			tWeaponDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/HarvestTool_002.dat";
			tWeaponDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
			tWeaponDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 30.f));
			tWeaponDesc.m_spPivotBone = m_spAnimRefMesh->GetBonePtr(L"Socket_Weapon_R");

			spWeaponComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tWeaponDesc));
			m_umapWeaponMesh.emplace(EItemType::STONE_AXE, spWeaponComponent);
			AddComponent(tWeaponDesc.m_wstrComponentName, spWeaponComponent);
		}

		// PickAxe
		{
			FMeshComponentDesc tWeaponDesc;
			tWeaponDesc.m_wstrComponentName = L"PickAxeComopnent";
			tWeaponDesc.m_eRenderType = ERenderType::RENDER_POST;
			tWeaponDesc.m_eComponentType = EComponentType::MESH;
			tWeaponDesc.m_wpOwnerGameObject = shared_from_this();

			tWeaponDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/HarvestTool_001.dat";
			tWeaponDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
			tWeaponDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 30.f));
			tWeaponDesc.m_spPivotBone = m_spAnimRefMesh->GetBonePtr(L"Socket_Weapon_R");

			spWeaponComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tWeaponDesc));
			m_umapWeaponMesh.emplace(EItemType::STONE_PICKAXE, spWeaponComponent);
			AddComponent(tWeaponDesc.m_wstrComponentName, spWeaponComponent);
		}

		// HandTorch
		{
			FMeshComponentDesc tWeaponDesc;
			tWeaponDesc.m_wstrComponentName = L"HandTorchComponent";
			tWeaponDesc.m_eRenderType = ERenderType::RENDER_POST;
			tWeaponDesc.m_eComponentType = EComponentType::MESH;
			tWeaponDesc.m_wpOwnerGameObject = shared_from_this();

			tWeaponDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/Weapon_Melee_004.dat";
			tWeaponDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
			tWeaponDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 0.f));
			tWeaponDesc.m_spPivotBone = m_spAnimRefMesh->GetBonePtr(L"Socket_Weapon_R");

			spWeaponComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tWeaponDesc));
			m_umapWeaponMesh.emplace(EItemType::HAND_HELD_TORCH, spWeaponComponent);
			AddComponent(tWeaponDesc.m_wstrComponentName, spWeaponComponent);
		}
	}

	// 창
	{
		FMeshComponentDesc tWeaponDesc;
		tWeaponDesc.m_wstrComponentName = L"StoneSpearComopnent";
		tWeaponDesc.m_eRenderType = ERenderType::RENDER_POST;
		tWeaponDesc.m_eComponentType = EComponentType::MESH;
		tWeaponDesc.m_wpOwnerGameObject = shared_from_this();

		tWeaponDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/Weapon_Melee_005.dat";
		tWeaponDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tWeaponDesc.m_spPivotBone = m_spAnimRefMesh->GetBonePtr(L"Socket_Weapon_R");

		spWeaponComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tWeaponDesc));
		m_umapWeaponMesh.emplace(EItemType::STONE_SPEAR, spWeaponComponent);
		AddComponent(tWeaponDesc.m_wstrComponentName, spWeaponComponent);
	}

	// 활
	{
		FMeshComponentDesc tWeaponDesc;
		tWeaponDesc.m_wstrComponentName = L"OldBowComopnent";
		tWeaponDesc.m_eRenderType = ERenderType::RENDER_POST;
		tWeaponDesc.m_eComponentType = EComponentType::MESH;
		tWeaponDesc.m_wpOwnerGameObject = shared_from_this();
		
		tWeaponDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/Weapon_Bow_003.dat";
		tWeaponDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tWeaponDesc.m_spPivotBone = m_spAnimRefMesh->GetBonePtr(L"Socket_Weapon_L");

		spWeaponComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tWeaponDesc));
		spWeaponComponent->SetUseAnimation(false, true);
		m_umapWeaponMesh.emplace(EItemType::OLD_BOW, spWeaponComponent);
		AddComponent(tWeaponDesc.m_wstrComponentName, spWeaponComponent);
	}

	// 총
	{
		FMeshComponentDesc tWeaponDesc;
		tWeaponDesc.m_wstrComponentName = L"AssaultRifleComopnent";
		tWeaponDesc.m_eRenderType = ERenderType::RENDER_POST;
		tWeaponDesc.m_eComponentType = EComponentType::MESH;
		tWeaponDesc.m_wpOwnerGameObject = shared_from_this();

		tWeaponDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/Weapon_AssaultRifle_001.dat";
		tWeaponDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tWeaponDesc.m_spPivotBone = m_spAnimRefMesh->GetBonePtr(L"Socket_Weapon_R");

		spWeaponComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tWeaponDesc));
		spWeaponComponent->SetUseAnimation(false, false);
		m_umapWeaponMesh.emplace(EItemType::ASSAULT_RIFLE, spWeaponComponent);
		AddComponent(tWeaponDesc.m_wstrComponentName, spWeaponComponent);
	}
}

void Client::CInventoryCustomObject::InitDisable()
{
	for (auto Pair0 : m_umapWeaponMesh)
	{
		Pair0.second->SetActive(false);
	}

	for (auto Pair1 : m_umapOutfitMesh)
	{
		Pair1.second->SetActive(false);
	}

	for (auto Pair2 : m_umapHeadEquipMesh)
	{
		Pair2.second->SetActive(false);
	}
}

void Client::CInventoryCustomObject::LoadMeshComponent(const wstring& _wstrFilePath, unordered_map<EItemType, shared_ptr<CMeshComponent>>& _umapMeshContainer, EItemType _eItemType, const wstring& _wstrAttachBoneName)
{
	filesystem::path filePath(_wstrFilePath);

	FMeshComponentDesc tMeshDesc;
	tMeshDesc.m_eRenderType = ERenderType::RENDER_POST;
	tMeshDesc.m_eComponentType = EComponentType::MESH;
	tMeshDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshDesc.m_spParentMeshComponent = m_spAnimRefMesh;
	tMeshDesc.m_spPivotBone = m_spAnimRefMesh->GetBonePtr(_wstrAttachBoneName);

	// mesh component desc
	if (_wstrAttachBoneName == L"")
		tMeshDesc.m_matPivot = Matrix::CreateScale(0.0001f);
	else
		tMeshDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));

	wstring componentName = filePath.filename().wstring();
	size_t iPoint = componentName.find('.');
	if (iPoint != wstring::npos) { componentName = componentName.substr(0, iPoint); }
	componentName += L"Component";

	tMeshDesc.m_wstrComponentName = componentName;
	tMeshDesc.m_wstrModelFilePath = filePath.wstring();

	 shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshDesc));
	_umapMeshContainer.emplace(_eItemType, spMeshComponent);
	AddComponent(tMeshDesc.m_wstrComponentName, spMeshComponent);
}

void Client::CInventoryCustomObject::SetCustomColor()
{	
	FCustomObjectData tCustomData = CClientLoop::s_spClientDataSheet->k_tCustomData;

	// 얼굴 염색
	{
		auto umapMeshInfos = m_spHeadMesh->GetModel()->GetMeshInfos();
		auto vecDefaultHeadInfo = umapMeshInfos[EShaderType::DEFAULT_ANIM];

		for (auto& spDefaultHeadBuffer : vecDefaultHeadInfo)
		{
			if (spDefaultHeadBuffer->m_spMeshBuffer->GetMaterial()->GetName().find(L"Brow") != wstring::npos ||
				spDefaultHeadBuffer->m_spMeshBuffer->GetMaterial()->GetName().find(L"Beard") != wstring::npos)
			{
				spDefaultHeadBuffer->m_spMeshBuffer->GetMaterial()->SetInt(L"Use Alternative Color", true);
				spDefaultHeadBuffer->m_spMeshBuffer->GetMaterial()->SetVector3(L"Alternative Color", tCustomData.m_vColor);
			}
		}

		auto vecEyeInfo = umapMeshInfos[EShaderType::PLAYEREYE_ANIM];

		for (auto& spEyeInfo : vecEyeInfo)
		{
			spEyeInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Cornea Index", tCustomData.m_iCorneaIndex);
			spEyeInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Highlight Index", tCustomData.m_iHighlightIndex);
			spEyeInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Cornea Use Alternative Color", true);
			spEyeInfo->m_spMeshBuffer->GetMaterial()->SetVector3(L"Cornea Alternative Color", tCustomData.m_vColor);
		}
	}

	// 머리 염색
	{
		auto umapMeshInfos = m_spHairMesh->GetModel()->GetMeshInfos();
		auto vecMeshInfos = umapMeshInfos[EShaderType::DEFAULT_ANIM];

		for (auto& spMeshBuffer : vecMeshInfos)
		{
			spMeshBuffer->m_spMeshBuffer->GetMaterial()->SetInt(L"Use Alternative Color", true);
			spMeshBuffer->m_spMeshBuffer->GetMaterial()->SetVector3(L"Alternative Color", tCustomData.m_vColor);
		}
	}

	// 모자 염색
	{
		for (auto& Pair : m_umapHeadEquipMesh)
		{
			auto umapMeshInfos = Pair.second->GetModel()->GetMeshInfos();
			auto vecMeshInfos = umapMeshInfos[EShaderType::DEFAULT_ANIM];

			for (auto& spMeshInfo : vecMeshInfos)
			{
				if (spMeshInfo->m_spMeshBuffer->GetMaterial()->GetName().find(L"Hair") != wstring::npos)
				{
					spMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Use Alternative Color", true);
					spMeshInfo->m_spMeshBuffer->GetMaterial()->SetVector3(L"Alternative Color", tCustomData.m_vColor);
				}
			}
		}
	}
}

void Client::CInventoryCustomObject::ChangeHeadEquipWithID(EItemType _eItemType)
{	
	if (m_umapHeadEquipMesh.contains(m_eCurHeadEquipID))
		m_umapHeadEquipMesh[m_eCurHeadEquipID]->SetActive(false);

	m_eCurHeadEquipID = _eItemType;

	if (m_eCurHeadEquipID == EItemType::NONE || (m_eCurHeadEquipID != EItemType::HeadEquip019 && m_eCurHeadEquipID != EItemType::HeadEquip021))
	{
		m_spHairMesh->SetActive(true);
	}
	else
	{
		m_spHairMesh->SetActive(false);
	}

	if(m_umapHeadEquipMesh.contains(m_eCurHeadEquipID))
		m_umapHeadEquipMesh[m_eCurHeadEquipID]->SetActive(true);
}

void Client::CInventoryCustomObject::ChangeOutfitWithID(EItemType _eItemType)
{
	if (m_umapOutfitMesh.contains(m_eCurOutfitID)) { m_umapOutfitMesh[m_eCurOutfitID]->SetActive(false); }

	if (_eItemType == EItemType::NONE || _eItemType == EItemType::ENUM_END) { m_eCurOutfitID = EItemType::Outfit_OldCloth001; }
	else {   m_eCurOutfitID = _eItemType; }

	if (m_umapOutfitMesh.contains(m_eCurOutfitID)) { m_umapOutfitMesh[m_eCurOutfitID]->SetActive(true); }
}

void Client::CInventoryCustomObject::ChangeWeaponWithID(EItemType _eItemType)
{
	if(m_umapWeaponMesh.contains(m_eCurWeaponID))
		m_umapWeaponMesh[m_eCurWeaponID]->SetActive(false);

	m_eCurWeaponID = _eItemType;

	if(m_umapWeaponMesh.contains(m_eCurWeaponID))
		m_umapWeaponMesh[m_eCurWeaponID]->SetActive(true);

	if (m_eCurWeaponID == EItemType::OLD_BOW)
	{
		m_spAnimRefMesh->ChangeDifferentAnimation((int32)EPLAYER_FEMALE::AS_Player_Female_Idle_Bow);
	}
	else if (m_eCurWeaponID == EItemType::ASSAULT_RIFLE)
	{
		m_spAnimRefMesh->ChangeDifferentAnimation((int32)EPLAYER_FEMALE::AS_Player_Female_Idle_AssaultRifle);
	}
	else if (m_eCurWeaponID == EItemType::STONE_SPEAR)
	{
		m_spAnimRefMesh->ChangeDifferentAnimation((int32)EPLAYER_FEMALE::AS_Player_Female_Idle_Spear);
	}
	else if(m_eCurWeaponID == EItemType::ENUM_END || m_eCurWeaponID == EItemType::NONE)
	{
		m_spAnimRefMesh->ChangeDifferentAnimation((int32)EPLAYER_FEMALE::AS_Player_Female_Idle_None);
	}
	else
	{
		m_spAnimRefMesh->ChangeDifferentAnimation((int32)EPLAYER_FEMALE::AS_Player_Female_Idle_Melee);
	}
}