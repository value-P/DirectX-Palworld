#include "pch.h"
#include "CustomizeObject.h"

#include "MeshComponent.h"
#include "Model.h"
#include "MeshBuffer.h"
#include "Material.h"
#include "Camera.h"
#include "ImageComponent.h"

#include "EngineModule.h"
#include "InputManager.h"

#include "Panel.h"
#include "ButtonComponent.h"
#include "TextComponent.h"

std::shared_ptr<CCustomizeObject> Client::CCustomizeObject::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CCustomizeObject> spInstance = make_shared<CCustomizeObject>();
	CHECKF(spInstance, L"CCustomizeObject : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CCustomizeObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FCustomizeObjectDesc> spCustomObjDesc = static_pointer_cast<FCustomizeObjectDesc>(_spGameObjectDesc);

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

		m_spAnimRefMesh->ChangeAnimation((int32)EPLAYER_FEMALE::AS_Player_Female_Idle_None, 0.f, false);
		m_spAnimRefMesh->SetCulled(true);
	}

	InitMesh();

	m_wpCustomizeBackgroundUI = spCustomObjDesc->m_spCustomizeBackgroundUI;
	InitUI();

	m_wpCustomizeLevelCam = spCustomObjDesc->m_spCamera;

	m_vecHeadMesh[m_iCurHeadIndex].first->SetActive(true);
	m_vecHairMesh[m_iCurHairIndex].first->SetActive(true);
	m_vecOutfitMesh[m_iCurOutfitIndex].first->SetActive(true);

	return S_OK;
}

HRESULT Client::CCustomizeObject::BeginPlay()
{
	CGameObject::BeginPlay();

	return S_OK;
}

int32 Client::CCustomizeObject::PreTick(float _fDeltaSeconds)
{
	CGameObject::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CCustomizeObject::Tick(float _fDeltaSeconds)
{
	CGameObject::Tick(_fDeltaSeconds);

	UpdateCustomizeUIText();
	UpdateButtonEvent();
	UpdateColor();
	ComputeCameraPos();

	return 0;
}

int32 Client::CCustomizeObject::PostTick(float _fDeltaSeconds)
{
	CGameObject::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CCustomizeObject::EndPlay()
{
	CGameObject::EndPlay();

	return S_OK;
}

void Client::CCustomizeObject::Release()
{
	SharedNull(m_spAnimRefMesh);

	for (auto& spMeshComponent : m_vecHeadMesh)
	{
		SharedNull(spMeshComponent.first);
	}
	m_vecHeadMesh.clear();

	for (auto& spMeshComponent : m_vecHairMesh)
	{
		SharedNull(spMeshComponent.first);
	}
	m_vecHairMesh.clear();

	for (auto& spMeshComponent : m_vecHeadEquipMesh)
	{
		SharedNull(spMeshComponent.first);
	}
	m_vecHeadEquipMesh.clear();

	for (auto& spMeshComponent : m_vecOutfitMesh)
	{
		SharedNull(spMeshComponent.first);
	}
	m_vecOutfitMesh.clear();

	CGameObject::Release();
}

void Client::CCustomizeObject::InitMesh()
{
	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Hair", m_vecHairMesh);
	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Head", m_vecHeadMesh);

	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/HeadEquip/HeadEquip019_v01.dat", m_vecHeadEquipMesh, EItemType::HeadEquip019);
	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/HeadEquip/HeadEquip021.dat", m_vecHeadEquipMesh, EItemType::HeadEquip021);

	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_OldCloth001.dat", m_vecOutfitMesh, EItemType::Outfit_OldCloth001);
	LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_Cloth001.dat", m_vecOutfitMesh, EItemType::Outfit_Cloth001);
	//LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_Hunter001.dat", m_vecOutfitMesh, EItemType::Outfit_Hunter001);
	//LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_Platinum001_v02.dat", m_vecOutfitMesh, EItemType::Outfit_Platinum001_v02);
}

void Client::CCustomizeObject::InitUI()
{
	{
		// 머리카락 ui
		m_wpHairPrevSelectButton = m_wpCustomizeBackgroundUI.lock()->FindComponent<CButtonComponent>(L"LeftArrowButton0");
		m_wpHairNextSelectButton = m_wpCustomizeBackgroundUI.lock()->FindComponent<CButtonComponent>(L"RightArrowButton0");
		m_wpHairNameText = m_wpCustomizeBackgroundUI.lock()->FindComponent<CTextComponent>(L"HairText");
	}

	{
		// 얼굴 ui
		m_wpHeadPrevSelectButton = m_wpCustomizeBackgroundUI.lock()->FindComponent<CButtonComponent>(L"LeftArrowButton1");
		m_wpHeadNextSelectButton = m_wpCustomizeBackgroundUI.lock()->FindComponent<CButtonComponent>(L"RightArrowButton1");
		m_wpHeadNameText = m_wpCustomizeBackgroundUI.lock()->FindComponent<CTextComponent>(L"FaceText");
	}

	{
		// 옷 ui
		m_wpOutfitPrevSelectButton = m_wpCustomizeBackgroundUI.lock()->FindComponent<CButtonComponent>(L"LeftArrowButton2");
		m_wpOutfitNextSelectButton = m_wpCustomizeBackgroundUI.lock()->FindComponent<CButtonComponent>(L"RightArrowButton2");
		m_wpOutfitNameText = m_wpCustomizeBackgroundUI.lock()->FindComponent<CTextComponent>(L"BodyText");
	}

	{
		// 머리장식 ui
		m_wpHeadEquipPrevSelectButton = m_wpCustomizeBackgroundUI.lock()->FindComponent<CButtonComponent>(L"LeftArrowButton3");
		m_wpHeadEquipNextSelectButton = m_wpCustomizeBackgroundUI.lock()->FindComponent<CButtonComponent>(L"RightArrowButton3");
		m_wpHeadEquipNameText = m_wpCustomizeBackgroundUI.lock()->FindComponent<CTextComponent>(L"EquipmentText");
	}

	{
		// RGB 슬라이더
		m_wpColorR = m_wpCustomizeBackgroundUI.lock()->FindComponent<CImageComponent>(L"ColorR");
		m_wpColorG = m_wpCustomizeBackgroundUI.lock()->FindComponent<CImageComponent>(L"ColorG");
		m_wpColorB = m_wpCustomizeBackgroundUI.lock()->FindComponent<CImageComponent>(L"ColorB");
	}
}

void Client::CCustomizeObject::LoadMeshComponent(const wstring& _wstrFilePath, vector<pair<shared_ptr<CMeshComponent>, wstring>>& _vecMeshContainer)
{
	filesystem::path filePath(_wstrFilePath);
	filesystem::directory_iterator dirIter(filePath);

	while (dirIter != filesystem::end(dirIter))
	{
		if (dirIter->is_regular_file())
		{
			FMeshComponentDesc tMeshDesc;
			tMeshDesc.m_eRenderType = ERenderType::RENDER_POST;
			tMeshDesc.m_eComponentType = EComponentType::MESH;
			tMeshDesc.m_wpOwnerGameObject = shared_from_this();
			tMeshDesc.m_spParentMeshComponent = m_spAnimRefMesh;

			// mesh component desc
			tMeshDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
			tMeshDesc.m_matPivot = Matrix::CreateScale(0.0001f);

			wstring componentName = dirIter->path().filename().wstring();
			size_t iPoint = componentName.find('.');
			if (iPoint != wstring::npos) { componentName = componentName.substr(0, iPoint); }
			componentName += L"Component";

			tMeshDesc.m_wstrComponentName = componentName;
			tMeshDesc.m_wstrModelFilePath = dirIter->path().wstring();

			shared_ptr<CMeshComponent> spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshDesc));
			spMeshComponent->SetActive(false);
			_vecMeshContainer.push_back({ spMeshComponent, dirIter->path().wstring() });
			AddComponent(tMeshDesc.m_wstrComponentName, spMeshComponent);
		}

		++dirIter;
	}
}

void Client::CCustomizeObject::LoadMeshComponent(const wstring& _wstrFilePath, vector<pair<shared_ptr<CMeshComponent>, pair<EItemType, wstring>>>& _vecMeshContainer, EItemType _eItemType, const wstring& _wstrAttachBoneName)
{
	filesystem::path filePath(_wstrFilePath);

	FMeshComponentDesc tMeshDesc;
	tMeshDesc.m_eRenderType = ERenderType::RENDER_POST;
	tMeshDesc.m_eComponentType = EComponentType::MESH;
	tMeshDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshDesc.m_spParentMeshComponent = m_spAnimRefMesh;
	tMeshDesc.m_spPivotBone = m_spAnimRefMesh->GetBonePtr(_wstrAttachBoneName);

	// mesh component desc
	tMeshDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
	tMeshDesc.m_matPivot = Matrix::CreateScale(0.0001f);
	shared_ptr<CMeshComponent> spMeshComponent = nullptr;

	wstring componentName = filePath.filename().wstring();
	size_t iPoint = componentName.find('.');
	if (iPoint != wstring::npos) { componentName = componentName.substr(0, iPoint); }
	componentName += L"Component";

	tMeshDesc.m_wstrComponentName = componentName;
	tMeshDesc.m_wstrModelFilePath = filePath.wstring();

	spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshDesc));
	spMeshComponent->SetActive(false);
	_vecMeshContainer.push_back({ spMeshComponent, {_eItemType, _wstrFilePath} });
	AddComponent(tMeshDesc.m_wstrComponentName, spMeshComponent);
}

void Client::CCustomizeObject::ComputeCameraPos()
{
	Vector3 vCurCamPos = m_wpCustomizeLevelCam.lock()->GetPosition().value();

	vCurCamPos = Vector3::Lerp(vCurCamPos, m_vDstCamPos, 0.15f);

	m_wpCustomizeLevelCam.lock()->SetPosition(vCurCamPos);
}

void Client::CCustomizeObject::ChangeHeadToNext()
{
	m_vecHeadMesh[m_iCurHeadIndex].first->SetActive(false);

	++m_iCurHeadIndex;
	if (m_iCurHeadIndex >= m_vecHeadMesh.size()) m_iCurHeadIndex = 0;

	m_vecHeadMesh[m_iCurHeadIndex].first->SetActive(true);
}

void Client::CCustomizeObject::ChangeHairToNext()
{
	m_vecHairMesh[m_iCurHairIndex].first->SetActive(false);

	++m_iCurHairIndex;
	if (m_iCurHairIndex >= m_vecHairMesh.size()) m_iCurHairIndex = 0;

	if (m_iCurHeadEquipIndex == -1)
		m_vecHairMesh[m_iCurHairIndex].first->SetActive(true);
}

void Client::CCustomizeObject::ChangeHeadEquipToNext()
{
	if (m_iCurHeadEquipIndex == -1) {}
	else { m_vecHeadEquipMesh[m_iCurHeadEquipIndex].first->SetActive(false); }

	++m_iCurHeadEquipIndex;

	if (m_iCurHeadEquipIndex >= m_vecHeadEquipMesh.size()) m_iCurHeadEquipIndex = -1;

	if (m_iCurHeadEquipIndex == -1) { m_vecHairMesh[m_iCurHairIndex].first->SetActive(true); }
	else
	{
		m_vecHeadEquipMesh[m_iCurHeadEquipIndex].first->SetActive(true);
		m_vecHairMesh[m_iCurHairIndex].first->SetActive(false);
	}
}

void Client::CCustomizeObject::ChangeOutfitToNext()
{
	m_vecOutfitMesh[m_iCurOutfitIndex].first->SetActive(false);

	++m_iCurOutfitIndex;
	if (m_iCurOutfitIndex >= m_vecOutfitMesh.size()) m_iCurOutfitIndex = 0;

	m_vecOutfitMesh[m_iCurOutfitIndex].first->SetActive(true);
}

void Client::CCustomizeObject::ChangeCorneaToNext()
{
	++m_iCorneaIndex;

	if (m_iCorneaIndex > 3) 
	{
		m_iCorneaIndex = 0; 
	}
}

void Client::CCustomizeObject::ChangeHighlightToNext()
{
	++m_iHighlightIndex;

	if (m_iHighlightIndex > 5)
	{
		m_iHighlightIndex = 0;
	}
}

void Client::CCustomizeObject::ChangeHeadToPrev()
{
	m_vecHeadMesh[m_iCurHeadIndex].first->SetActive(false);

	--m_iCurHeadIndex;
	if (m_iCurHeadIndex < 0) m_iCurHeadIndex = (int32)m_vecHeadMesh.size() - 1;

	m_vecHeadMesh[m_iCurHeadIndex].first->SetActive(true);
}

void Client::CCustomizeObject::ChangeHairToPrev()
{
	m_vecHairMesh[m_iCurHairIndex].first->SetActive(false);

	--m_iCurHairIndex;
	if (m_iCurHairIndex < 0) m_iCurHairIndex = (int32)m_vecHairMesh.size() - 1;

	if (m_iCurHeadEquipIndex == -1) { m_vecHairMesh[m_iCurHairIndex].first->SetActive(true); }
}

void Client::CCustomizeObject::ChangeHeadEquipToPrev()
{
	if (m_iCurHeadEquipIndex == -1) {}
	else { m_vecHeadEquipMesh[m_iCurHeadEquipIndex].first->SetActive(false); }

	--m_iCurHeadEquipIndex;

	if (m_iCurHeadEquipIndex < -1) { m_iCurHeadEquipIndex = (int32)m_vecHeadEquipMesh.size() - 1; }

	if (m_iCurHeadEquipIndex == -1) { m_vecHairMesh[m_iCurHairIndex].first->SetActive(true); }
	else
	{
		m_vecHeadEquipMesh[m_iCurHeadEquipIndex].first->SetActive(true);
		m_vecHairMesh[m_iCurHairIndex].first->SetActive(false);
	}
}

void Client::CCustomizeObject::ChangeOutfitToPrev()
{
	m_vecOutfitMesh[m_iCurOutfitIndex].first->SetActive(false);

	--m_iCurOutfitIndex;
	if (m_iCurOutfitIndex < 0) m_iCurOutfitIndex = (int32)m_vecOutfitMesh.size() - 1;

	m_vecOutfitMesh[m_iCurOutfitIndex].first->SetActive(true);
}

void Client::CCustomizeObject::ChangeCorneaToPrev()
{
	--m_iCorneaIndex;

	if (m_iCorneaIndex < 0)
	{
		m_iCorneaIndex = 3;
	}
}

void Client::CCustomizeObject::ChangeHighlightToPrev()
{
	--m_iHighlightIndex;

	if (m_iHighlightIndex < 0)
	{
		m_iHighlightIndex = 5;
	}
}

void Client::CCustomizeObject::CustomizeColor(const Vector3& _vRGB)
{
	m_vCustomColor = _vRGB;

	// 머리 염색
	{
		auto umapMeshInfos = m_vecHairMesh[m_iCurHairIndex].first->GetModel()->GetMeshInfos();
		auto vecMeshInfos = umapMeshInfos[EShaderType::DEFAULT_ANIM];

		for (auto& spMeshBuffer : vecMeshInfos)
		{
			spMeshBuffer->m_spMeshBuffer->GetMaterial()->SetInt(L"Use Alternative Color", true);
			spMeshBuffer->m_spMeshBuffer->GetMaterial()->SetVector3(L"Alternative Color", m_vCustomColor);
		}
	}

	// 얼굴 염색
	{
		auto umapMeshInfos = m_vecHeadMesh[m_iCurHeadIndex].first->GetModel()->GetMeshInfos();
		auto vecMeshInfos = umapMeshInfos[EShaderType::DEFAULT_ANIM];
		
		for (auto& spMeshInfo : vecMeshInfos)
		{
			if (spMeshInfo->m_spMeshBuffer->GetMaterial()->GetName().find(L"Brow") != wstring::npos ||
				spMeshInfo->m_spMeshBuffer->GetMaterial()->GetName().find(L"Beard") != wstring::npos)
			{
				spMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Use Alternative Color", true);
				spMeshInfo->m_spMeshBuffer->GetMaterial()->SetVector3(L"Alternative Color", m_vCustomColor);
			}
		}
	}

	// 모자 머리카락 염색
	if (m_iCurHeadEquipIndex != -1)
	{		
		auto umapMeshInfos = m_vecHeadEquipMesh[m_iCurHeadEquipIndex].first->GetModel()->GetMeshInfos();
		auto vecMeshInfos = umapMeshInfos[EShaderType::DEFAULT_ANIM];

		for (auto& spMeshInfo : vecMeshInfos)
		{
			if (spMeshInfo->m_spMeshBuffer->GetMaterial()->GetName().find(L"Hair") != wstring::npos )
			{
				spMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Use Alternative Color", true);
				spMeshInfo->m_spMeshBuffer->GetMaterial()->SetVector3(L"Alternative Color", m_vCustomColor);
			}
		}
	}

	// 눈깔 염색
	{
		auto umapMeshInfos = m_vecHeadMesh[m_iCurHeadIndex].first->GetModel()->GetMeshInfos();
		auto vecMeshInfos = umapMeshInfos[EShaderType::PLAYEREYE_ANIM];

		for (auto& spMeshInfo : vecMeshInfos)
		{
			spMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Cornea Index", m_iCorneaIndex);
			spMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Highlight Index", m_iHighlightIndex);
			spMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Cornea Use Alternative Color", true);
			spMeshInfo->m_spMeshBuffer->GetMaterial()->SetVector3(L"Cornea Alternative Color", m_vCustomColor);
		}

		/*홍채 종류*/					//m_umapInts.insert({ L"Cornea Index"						, 0 });
		/*광채 종류*/					//m_umapInts.insert({ L"Highlight Index"					, 0 });
		/*동공 종류*/					//m_umapInts.insert({ L"Pupil Index"						, 0 });
		/*홍채 대체 색상 사용 여부*/	//m_umapInts.insert({ L"Cornea Use Alternative Color"      , false });
		/*광채 대체 색상 사용 여부*/	//m_umapInts.insert({ L"Highlight Use Alternative Color"       , false });
		/*동공 대체 색상 사용 여부*/	//m_umapInts.insert({ L"Pupil Use Alternative Color"	, false });
		/*홍채 대체 색상*/				//m_umapVector3s.insert({ L"Cornea Alternative Color"			, Vector3(0,0.7f,0.9f) });
		/*광채 대체 색상*/				//m_umapVector3s.insert({ L"Highlight Alternative Color"		, Vector3(1,1,1) });
		/*동공 대체 색상*/				//m_umapVector3s.insert({ L"Pupil Alternative Color"			, Vector3(1,1,1) });
		/*홍채 발광 강도*/				//m_umapFloats.insert({ L"Cornea Emision"					, 0.75f });
		/*광채 발광 강도*/				//m_umapFloats.insert({ L"Highlight Emission"				, 0.f });
		/*동공 발광 강도*/				//m_umapFloats.insert({ L"Pupli Emission"					, 0.f });
	}
}

FCustomObjectData Client::CCustomizeObject::GetCustomObjectData()
{
	FCustomObjectData tCustomData;

	tCustomData.m_wstrHairMeshPath = m_vecHairMesh[m_iCurHairIndex].second;
	tCustomData.m_wstrHeadMeshPath = m_vecHeadMesh[m_iCurHeadIndex].second;

	if (m_iCurHeadEquipIndex != -1) { tCustomData.m_eInitialHeadEquip = m_vecHeadEquipMesh[m_iCurHeadEquipIndex].second.first; }
	if (m_iCurOutfitIndex != 0) { tCustomData.m_eInitialOutfit = m_vecOutfitMesh[m_iCurOutfitIndex].second.first; }

	tCustomData.m_iCorneaIndex = m_iCorneaIndex;
	tCustomData.m_iHighlightIndex = m_iHighlightIndex;
	tCustomData.m_vColor = m_vCustomColor;

	return tCustomData;
}

void Client::CCustomizeObject::UpdateButtonEvent()
{
	{
		// 머리카락 ui 이벤트
		m_wpHairPrevSelectButton.lock()->AddButtonClickEvent([&]() { ChangeHairToPrev(); m_vDstCamPos = m_vFaceCamPos; });
		m_wpHairNextSelectButton.lock()->AddButtonClickEvent([&]() { ChangeHairToNext(); m_vDstCamPos = m_vFaceCamPos; });
	}

	{
		// 얼굴 ui 이벤트
		m_wpHeadPrevSelectButton.lock()->AddButtonClickEvent([&]() { ChangeHeadToPrev(); m_vDstCamPos = m_vFaceCamPos; });
		m_wpHeadNextSelectButton.lock()->AddButtonClickEvent([&]() { ChangeHeadToNext(); m_vDstCamPos = m_vFaceCamPos; });
	}

	{
		// 옷 ui 이벤트
		m_wpOutfitPrevSelectButton.lock()->AddButtonClickEvent([&]() { ChangeOutfitToPrev(); m_vDstCamPos = m_vBodyCamPos; });
		m_wpOutfitNextSelectButton.lock()->AddButtonClickEvent([&]() { ChangeOutfitToNext(); m_vDstCamPos = m_vBodyCamPos; });
	}

	{
		// 머리장식 ui 이벤트
		m_wpHeadEquipPrevSelectButton.lock()->AddButtonClickEvent([&]() { ChangeHeadEquipToPrev(); m_vDstCamPos = m_vFaceCamPos; });
		m_wpHeadEquipNextSelectButton.lock()->AddButtonClickEvent([&]() { ChangeHeadEquipToNext(); m_vDstCamPos = m_vFaceCamPos; });
	}
}

void Client::CCustomizeObject::UpdateCustomizeUIText()
{
	wstring wstrText = L"";

	// 헤어 텍스트
	{
		wstrText = L"머리" + to_wstring(m_iCurHairIndex + 1);
		m_wpHairNameText.lock()->SetText(wstrText);
	}

	// 얼굴 텍스트
	{
		wstrText = L"얼굴" + to_wstring(m_iCurHeadIndex + 1);
		m_wpHeadNameText.lock()->SetText(wstrText);
	}

	// 옷 텍스트
	{
		if (m_iCurOutfitIndex == 0)
			wstrText = L"허름한 옷";
		else
			wstrText = L"천 옷" + to_wstring(m_iCurOutfitIndex);

		m_wpOutfitNameText.lock()->SetText(wstrText);
	}

	// 머리장식 텍스트
	{
		if (m_iCurHeadEquipIndex == -1)
			wstrText = L"없음";
		else
			wstrText = L"모자" + to_wstring(m_iCurHeadEquipIndex + 1);
		m_wpHeadEquipNameText.lock()->SetText(wstrText);
	}
}

void Client::CCustomizeObject::UpdateColor()
{
	Vector3 vColor;
	vColor.x = m_wpColorR.lock()->GetRatio();
	vColor.y = m_wpColorG.lock()->GetRatio();
	vColor.z = m_wpColorB.lock()->GetRatio();

	CustomizeColor(vColor);
}
