// Default
#include "pch.h"
#include "Housing.h"
#include "StringUtility.h"

// Module, Manager
#include "ClientModule.h"
#include "UIManager.h"
#include "BuildManager.h"
#include "EffectManager.h"

// Level
#include "Level.h"

// GameObject
#include "HousingLump.h"

// Component
#include "MeshComponent.h"
#include "BoxComponent.h"

// etc
#include "Model.h"
#include "MeshBuffer.h"
#include "Material.h"

std::shared_ptr<CHousing> Client::CHousing::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CHousing> spInstance = make_shared<CHousing>();
	CHECKF(spInstance, L"CHousing : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CHousing::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CArchitecture::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	/* gameobject */
	shared_ptr<FHousingDesc> spHousingDesc = static_pointer_cast<FHousingDesc>(_spGameObjectDesc);

	m_eBuildType            = spHousingDesc->m_eBuildType;
	m_eBuildDir				= spHousingDesc->m_eBuildDir;
	m_umapNeededBuildTypes  = spHousingDesc->m_umapNeededBuildTypes;
	m_wpHousingLump         = spHousingDesc->m_spHousingLump;
	m_tpBuildIndex          = spHousingDesc->m_tpBuildIndex;
	m_iFloor				= spHousingDesc->m_iFloor;

	m_vecCombinedHousings.reserve((int32)EBuildDir::ENUM_END);
	m_vecCombinedHousings.resize((int32)EBuildDir::ENUM_END);

	/* component */
	InitializeMeshComponent();
	InitializeColliderComponent();

	return S_OK;
}

HRESULT Client::CHousing::BeginPlay()
{
	if (FAILED(CArchitecture::BeginPlay())) { return E_FAIL; }

	for (auto& MeshComponentPair : m_umapMeshComponents)
	{
		auto& umapMeshInfos = MeshComponentPair.second->GetModel()->GetMeshInfos();

		for (auto& ShaderTypePair : umapMeshInfos)
		{
			for (auto& tMeshInfo : ShaderTypePair.second)
			{
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"State", (int32)EArchitextureState::COMPLETE);
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Alpha", 1.f);
				tMeshInfo->m_spMeshBuffer->GetMaterial()->SetFloat(L"Progress", 1.f);

				/*상태*/				//m_umapInts.insert({ L"State",	0 });					// EArchitextureState 참고
				/*투명도*/			//m_umapFloats.insert({ L"Alpha",	0.5f });			// EArchitextureState::BLUEPRINT 일 때의 투명도	( 0 ~ 1 )		
				/*현재 작업 진행률*/	//m_umapFloats.insert({ L"Progress",	0.f });			// EArchitextureState::BUILDING 일 때의 진행도	( 0 ~ 1 )
			}
		}
	}

	return S_OK;
}

int32 Client::CHousing::PreTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CHousing::Tick(float _fDeltaSeconds)
{
	switch (CArchitecture::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CHousing::PostTick(float _fDeltaSeconds)
{
	switch (CArchitecture::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CHousing::EndPlay()
{
	if (FAILED(CArchitecture::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CHousing::Release()
{
	m_spBoxComponent = nullptr;

	for (uint32 innerIndex = 0; innerIndex < m_vecCombinedHousings.size(); ++innerIndex)
	{
		m_vecCombinedHousings[innerIndex] = nullptr;
	}
	m_vecCombinedHousings.clear();

	CArchitecture::Release();
}

void Client::CHousing::RecycleFromPool()
{

}

void Client::CHousing::ReturnToPool()
{

}

HRESULT Client::CHousing::AddCombinedHousing(EBuildDir _eBuildDir, std::shared_ptr<CHousing> _spHousing)
{
	CHECKF(!m_vecCombinedHousings.empty(), L"m_vecCombinedHousings is empty! : CHousing");

	if (nullptr == m_vecCombinedHousings[(int32)_eBuildDir])
	{
		m_vecCombinedHousings[(int32)_eBuildDir] = _spHousing;
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

std::shared_ptr<CHousing> Client::CHousing::GetCombinedHousing(EBuildDir _eBuildDir)
{
	// CHECKF(!, L"m_vecCombinedHousings is empty! : CHousing");
	if (m_vecCombinedHousings.empty()) { return nullptr; }
	if (nullptr == m_vecCombinedHousings[(int32)_eBuildDir])
	{
		return nullptr;
	}

	return m_vecCombinedHousings[(int32)_eBuildDir];
}

void Client::CHousing::DestroyArchitecture()
{
	// 토대, 지붕인 경우 하우징 덩어리에서 하우징을 제거합니다.
	if (EBuildType::FOUNDATION == m_eBuildType ||
		EBuildType::ROOF == m_eBuildType ||
		EBuildType::SLOPEDROOF == m_eBuildType)
	{
		m_wpHousingLump.lock()->RemoveHousing(m_tpBuildIndex);
	}

	// 부모 하우징의 연결된 방향에서 해당 하우징을 제거합니다.
	if (m_prParentHousing.second)
	{
		vector<shared_ptr<CHousing>>& vecCombinedHousings = m_prParentHousing.second->GetCombinedHousings();
		if (!vecCombinedHousings.empty())
		{
			vecCombinedHousings[(uint32)m_prParentHousing.first] = nullptr;
		}
	}

	// 레벨에서 오브젝트를 제거합니다.
	m_wpOwnerLevel.lock()->RemoveGameObject(shared_from_this());

	// 파괴 이펙트를 재생합니다.
	CEffectManager::GetInstance()->ActivateEffect(L"Group_BuildDestroy", GetPosition().value());

	// UI를 반납합니다.
	CUIManager::GetInstance()->ReturnProductionArchitectureUI(m_spProductionPanel);

	// Release를 호출합니다.
	// Release();
}

HRESULT Client::CHousing::InitializeMeshComponent()
{
	// 건축물 메쉬 생성
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName	= L"Housing MeshComponent";
	tMeshComponentDesc.m_eRenderType		= ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType		= EComponentType::MESH;
	tMeshComponentDesc.m_wstrModelFilePath	= m_wstrModelFilePath;
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(m_fPivotScaleRatio) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
		Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
	tMeshComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tMeshComponentDesc.m_wstrShaderName		= m_wstrShaderName;

	shared_ptr<CMeshComponent> spMeshCom = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));

	m_umapMeshComponents.emplace(L"Housing MeshComponent", spMeshCom);
	return AddComponent(L"Housing MeshComponent", spMeshCom);
}

HRESULT Client::CHousing::InitializeColliderComponent()
{
	// box compzonent desc
	FBoxComponentDesc tBoxComponentDesc;

	tBoxComponentDesc.m_vExtents			= m_vColliderExtents;
	tBoxComponentDesc.m_vLocalPosition		= m_vColliderCenter;
	tBoxComponentDesc.m_vLocalRotate		= m_vColliderRotate;

	// collider component desc
	tBoxComponentDesc.m_eMobilityType		= EMobilityType::STATIC;
	tBoxComponentDesc.m_eColliderType		= EColliderType::BOX;
	tBoxComponentDesc.m_eCollisionFlag		= ECollisionFlag::BUILDING;
	tBoxComponentDesc.m_eCollisionLayer		= ECollisionLayer::BUILDING;
	tBoxComponentDesc.m_fDensity			= 1.0f;
	tBoxComponentDesc.m_fMass				= 0.0f;
	tBoxComponentDesc.m_bGravity			= false;
	tBoxComponentDesc.m_bTrigger			= false;
	tBoxComponentDesc.m_iAxisLockType		= 0;
	tBoxComponentDesc.m_vMaterial			= Vector3(0.1f, 0.1f, 0.0f);
	//tBoxComponentDesc.m_vMaterial			= Vector3(0.5f, 0.5f, 0.01f);

	// component desc
	tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
	tBoxComponentDesc.m_wstrComponentName	= L"BoxComponent";
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	return AddComponent(L"BoxComponent", m_spBoxComponent);
}

HRESULT Client::CHousing::SaveHousing(rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer)
{
	_writer.StartObject();

	_writer.Key("Architecture Type");
	_writer.Uint((uint32)GetArchitectureType());

	_writer.Key("Build Type");
	_writer.Uint((uint32)GetBuildType());

	_writer.Key("Build Dir");
	_writer.Uint((uint32)GetBuildDir());

	_writer.Key("Model File Path");
	wstring wstrModelFilePath = GetHousingModelFilePath();
	_writer.String(CStringUtility::ToString(wstrModelFilePath).c_str());

	_writer.Key("Build Index");
	_writer.StartArray();
	_writer.Int(get<0>(GetBuildIndex()));
	_writer.Int(get<1>(GetBuildIndex()));
	_writer.Int(get<2>(GetBuildIndex()));
	_writer.EndArray();

	_writer.Key("Scale");
	_writer.StartArray();
	_writer.Double(GetScale().value().x);
	_writer.Double(GetScale().value().y);
	_writer.Double(GetScale().value().z);
	_writer.EndArray();

	_writer.Key("Rotation");
	_writer.StartArray();
	_writer.Double(GetRotate().value().x);
	_writer.Double(GetRotate().value().y);
	_writer.Double(GetRotate().value().z);
	_writer.EndArray();

	_writer.Key("Position");
	_writer.StartArray();
	_writer.Double(GetPosition().value().x);
	_writer.Double(GetPosition().value().y);
	_writer.Double(GetPosition().value().z);
	_writer.EndArray();

	_writer.Key("Collider Extents");
	_writer.StartArray();
	_writer.Double(GetColliderExtents().x);
	_writer.Double(GetColliderExtents().y);
	_writer.Double(GetColliderExtents().z);
	_writer.EndArray();

	_writer.Key("Collider Center");
	_writer.StartArray();
	_writer.Double(GetColliderCenter().x);
	_writer.Double(GetColliderCenter().y);
	_writer.Double(GetColliderCenter().z);
	_writer.EndArray();

	_writer.Key("Collider Rotate");
	_writer.StartArray();
	_writer.Double(GetColliderRotate().x);
	_writer.Double(GetColliderRotate().y);
	_writer.Double(GetColliderRotate().z);
	_writer.EndArray();

	_writer.EndObject();

	for (auto& combinedhousing : m_vecCombinedHousings)
	{
		if (!combinedhousing) { continue; }

		combinedhousing->SaveHousing(_writer);
	}

	return S_OK;
}