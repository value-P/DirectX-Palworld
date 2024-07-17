// Default
#include "pch.h"
#include "EffectGroup.h"
#include "StringUtility.h"
#include "MathUtility.h"

// Manager
#include "EngineModule.h"
#include "SoundManager.h"

// GameObject
#include "EffectBase.h"
#include "ParticleObject.h"
#include "EffectMeshObject.h"
#include "InstanceEffectMeshObject.h"

#include "Player.h"
#include "Monster.h"

// Component
#include "ParticleComponent.h"
#include "EffectMeshComponent.h"
#include "InstanceEffectMeshComponent.h"
#include "SphereComponent.h"
#include "BoxComponent.h"

// etc
#include "Bone.h"

shared_ptr<CEffectGroup> Client::CEffectGroup::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CEffectGroup> spInstance = make_shared<CEffectGroup>();
	CHECKF(spInstance, L"Failed to Create : CEffectGroup");

	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}
shared_ptr<CEffectGroup> Client::CEffectGroup::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CEffectGroup> spInstance = make_shared<CEffectGroup>();
	CHECKF(spInstance, L"Failed to CreateFromJson : CEffectGroup");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT Client::CEffectGroup::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	/* manager */
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	/* gameobject */
	shared_ptr<FEffectGroupDesc> spEffectGroupDesc = static_pointer_cast<FEffectGroupDesc>(_spGameObjectDesc);

	m_wstrEffectGroupName = spEffectGroupDesc->m_wstrEffectGroupName;
	m_fDuration = spEffectGroupDesc->m_fDuration;

	return S_OK;
}
HRESULT Client::CEffectGroup::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	/* manager */
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	/* gameobject */
	shared_ptr<FEffectGroupDesc> spEffectGroupDesc = static_pointer_cast<FEffectGroupDesc>(_spGameObjectDesc);

	m_wstrEffectGroupName = spEffectGroupDesc->m_wstrEffectGroupName;
	m_fDuration = spEffectGroupDesc->m_fDuration;

	LoadFromJsonFile(_wstrJsonFileName);

	return S_OK;
}
HRESULT Client::CEffectGroup::BeginPlay()
{
	/* BeginPlay - EffectGroup's components */
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	/* BeginPlay - EffectGroup's effects */
	for (const auto& pair : m_mapEffects)
	{
		if (FAILED(pair.second->BeginPlay())) { return E_FAIL; }
	}

	return S_OK;
}
int32 Client::CEffectGroup::PreTick(float _fDeltaSeconds)
{
	/* PreTick - EffectGroup's components */
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	/* PreTick - EffectGroup's effects */
	for (const auto& pair : m_mapEffects)
	{
		if (!pair.second->IsActive()) { continue; }

		switch (pair.second->PreTick(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	return 0;
}
int32 Client::CEffectGroup::Tick(float _fDeltaSeconds)
{
	if (m_bPause || m_mapEffects.empty())
		return 0;

	/* Tick - EffectGroup's components */
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	AttachToBone();

	/* Tick - EffectGroup's effects */
	m_fTrackPosition += _fDeltaSeconds;

	if (m_fTrackPosition > m_fDuration)
	{
		if (m_bLoop)
			Loop();
		else
			EndLifeTime();
	}
	else
	{
		for (const auto& pair : m_mapEffects)
		{
			if (!pair.second->IsActive()) { continue; }

			switch (pair.second->Tick(_fDeltaSeconds))
			{
			case -1:	return -1;
			default:	break;
			}
		}
	}

	return 0;
}
int32 Client::CEffectGroup::PostTick(float _fDeltaSeconds)
{
	/* PostTick - EffectGroup's components */
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	/* PostTick - EffectGroup's effects */
	for (const auto& pair : m_mapEffects)
	{
		if (!pair.second->IsActive()) { continue; }

		switch (pair.second->PostTick(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	return 0;
}
HRESULT Client::CEffectGroup::EndPlay()
{
	/* EndPlay - EffectGroup's components */
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	/* EndPlay - EffectGroup's effects */
	for (const auto& pair : m_mapEffects)
	{
		if (FAILED(pair.second->EndPlay())) { return E_FAIL; }
	}

	return S_OK;
}
void Client::CEffectGroup::Release()
{
	for (auto& pair : m_mapEffects)
	{
		if (pair.second)
		{
			pair.second->Release();
			pair.second.reset();
		}
	}
	m_mapEffects.clear();

	m_spCombinedBone = nullptr;
	m_spCombinedGameObject = nullptr;

	m_spSphereComponent = nullptr;
	m_spBoxComponent = nullptr;

	CGameObject::Release();
}

HRESULT Client::CEffectGroup::InitializeCollider(FColliderInfo _tColliderInfo)
{
	m_tColliderInfo.m_bCollider = _tColliderInfo.m_bCollider;

	switch (_tColliderInfo.m_eColliderType)
	{
	case EColliderType::SPHERE:
	{
		if (FindComponent<CSphereComponent>(L"SphereComponent") ||
			FindComponent<CBoxComponent>(L"BoxComponent"))
		{
			return E_FAIL;
		}

		FSphereComponentDesc tSphereDesc;

		// sphere component desc
		tSphereDesc.m_fRadius = _tColliderInfo.m_fRadius;

		// collider desc
		tSphereDesc.m_bGravity = false;
		tSphereDesc.m_bTrigger = true;
		tSphereDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		tSphereDesc.m_eComponentType = EComponentType::SPHERE;
		tSphereDesc.m_eCollisionFlag = _tColliderInfo.m_eCollisionFlag;
		tSphereDesc.m_eCollisionLayer = _tColliderInfo.m_eCollisionLayer;
		tSphereDesc.m_fMass = 0.f;
		tSphereDesc.m_vMaterial = Vector3(1.f, 1.f, 0.f);
		tSphereDesc.m_bDeferredAdd = true;
		tSphereDesc.m_bEnableSimulation = false;

		// component desc
		tSphereDesc.m_wpOwnerGameObject = shared_from_this();
		tSphereDesc.m_eColliderType = EColliderType::SPHERE;
		tSphereDesc.m_wstrComponentName = L"SphereCollider";
		tSphereDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		m_spSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereDesc));
		AddComponent(L"SphereCollider", m_spSphereComponent);

		break;
	}
	case EColliderType::BOX:
	{
		if (FindComponent<CBoxComponent>(L"BoxComponent") ||
			FindComponent<CSphereComponent>(L"SphereComponent"))
		{
			return E_FAIL;
		}

		FBoxComponentDesc tBoxComponentDesc;

		// box component desc
		tBoxComponentDesc.m_vExtents = _tColliderInfo.m_vExtents;
		tBoxComponentDesc.m_vLocalPosition = _tColliderInfo.m_vLocalPosition;
		tBoxComponentDesc.m_vLocalRotate = _tColliderInfo.m_vLocalRotate;

		// collider component desc
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = true;
		tBoxComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = _tColliderInfo.m_eCollisionFlag;
		tBoxComponentDesc.m_eCollisionLayer = _tColliderInfo.m_eCollisionLayer;
		tBoxComponentDesc.m_fDensity = 1.0f;
		tBoxComponentDesc.m_fMass = 0.0f;
		tBoxComponentDesc.m_iAxisLockType = 0;
		tBoxComponentDesc.m_vMaterial = Vector3(0.5f, 0.5f, 0.5f);
		tBoxComponentDesc.m_bDeferredAdd = true;
		tBoxComponentDesc.m_bEnableSimulation = false;

		// component desc
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_wstrComponentName = L"BoxComponent";
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		AddComponent(L"BoxComponent", m_spBoxComponent);

		break;
	}
	}

	return S_OK;
}

void Client::CEffectGroup::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CColliderComponent* pDstCol = dynamic_cast<CColliderComponent*>(_pDstComponent);
	CColliderComponent* pSrcCol = dynamic_cast<CColliderComponent*>(_pSrcComponent);

	// 플레이어의 공격 -> 야생 팰
	if (pDstCol &&  pDstCol->GetCollisionLayer() == ECollisionLayer::MONSTER &&
		(pSrcCol->GetCollisionLayer() == ECollisionLayer::PLAYER_PROJECTILE ||
			pSrcCol->GetCollisionLayer() == ECollisionLayer::PLAYER_WEAPON))
	{
		// 임시
		float fTestDamage = 0.f;

		static_pointer_cast<CMonster>(_pDstComponent->GetOwnerGameObject())->TakeDamage(shared_from_this(), fTestDamage);
	}
	// 파트너 팰의 공격 -> 야생 팰
	if (pDstCol && pDstCol->GetCollisionLayer() == ECollisionLayer::MONSTER &&
		(pSrcCol->GetCollisionLayer() == ECollisionLayer::ALLY_PROJECTILE ||
			pSrcCol->GetCollisionLayer() == ECollisionLayer::ALLY_WEAPON))
	{
		// 임시
		float fTestDamage = 0.f;

		static_pointer_cast<CMonster>(_pDstComponent->GetOwnerGameObject())->TakeDamage(shared_from_this(), fTestDamage);
	}
	// 야생 팰의 공격 -> 플레이어
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER &&
		(pSrcCol->GetCollisionLayer() == ECollisionLayer::MONSTER_PROJECTILE ||
			pSrcCol->GetCollisionLayer() == ECollisionLayer::MONSTER_WEAPON))
	{
		// 임시
		float fTestDamage = 0.f;

		static_pointer_cast<CPlayer>(_pDstComponent->GetOwnerGameObject())->TakeDamage(shared_from_this(), fTestDamage);
	}
	// 야생 팰의 공격 -> 파트너 팰
	if (pDstCol && pDstCol->GetCollisionLayer() == ECollisionLayer::ALLY &&
		(pSrcCol->GetCollisionLayer() == ECollisionLayer::MONSTER_PROJECTILE ||
			pSrcCol->GetCollisionLayer() == ECollisionLayer::MONSTER_WEAPON))
	{
		// 임시
		float fTestDamage = 0.f;

		static_pointer_cast<CPlayer>(_pDstComponent->GetOwnerGameObject())->TakeDamage(shared_from_this(), fTestDamage);
	}

	// if(m_spSphereComponent) { m_spSphereComponent->SetEnableSimulation(false); }
	// if(m_spBoxComponent){ m_spBoxComponent->SetEnableSimulation(false); }

	// 이펙트 비활성화
	// SetActive(false);
	// EndLifeTime();
}

void Client::CEffectGroup::OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{

}

HRESULT Client::CEffectGroup::LoadFromJsonFile(const wstring& _wstrJsonFileName)
{
	// 1. JSON문서를 읽기 모드로 연다.
	ifstream isJsonpath(L"../../Resource/Effects/EffectGroup/" + _wstrJsonFileName + L".json");
	if (!isJsonpath.is_open())
	{
		throw runtime_error("failed to open json file : CEffectGroup");
	}

	// 2. 문서 내용을 문자열로 변환한다.
	string strJsonfile((istreambuf_iterator<char>(isJsonpath)), (istreambuf_iterator<char>()));

	// 3. JSON문서의 문자열을 파싱해온다.
	rapidjson::Document doc;
	doc.Parse(strJsonfile.c_str());

	m_fDuration = doc["Duration"].GetFloat();
	m_bLoop = doc["Loop"].GetBool();

	m_tColliderInfo.m_bCollider = doc["Is Collider"].GetBool();
	m_tColliderInfo.m_eColliderType = (EColliderType)doc["Collider Type"].GetUint();
	m_tColliderInfo.m_eCollisionFlag = (ECollisionFlag)doc["Collision Flag"].GetUint();
	m_tColliderInfo.m_eCollisionLayer = (ECollisionLayer)doc["Collision Layer"].GetUint();

	m_tColliderInfo.m_fRadius = doc["Sphere Collider Radius"].GetFloat();

	// 사운드
	m_wstrSoundName = CStringUtility::ToWideString(doc["Sound Name"].GetString());
	m_bSoundLoop = doc["Sound Loop"].GetBool();
	m_fSoundValue = doc["Sound Value"].GetFloat();

	m_tColliderInfo.m_vExtents.x = doc["Box Collider Extents"][0].GetFloat();
	m_tColliderInfo.m_vExtents.y = doc["Box Collider Extents"][1].GetFloat();
	m_tColliderInfo.m_vExtents.z = doc["Box Collider Extents"][2].GetFloat();

	m_tColliderInfo.m_vLocalPosition.x = doc["Box Collider LocalPosition"][0].GetFloat();
	m_tColliderInfo.m_vLocalPosition.y = doc["Box Collider LocalPosition"][1].GetFloat();
	m_tColliderInfo.m_vLocalPosition.z = doc["Box Collider LocalPosition"][2].GetFloat();

	m_tColliderInfo.m_vLocalRotate.x = doc["Box Collider LocalRotate"][0].GetFloat();
	m_tColliderInfo.m_vLocalRotate.y = doc["Box Collider LocalRotate"][1].GetFloat();
	m_tColliderInfo.m_vLocalRotate.z = doc["Box Collider LocalRotate"][2].GetFloat();

	// 4. JSON문서로부터 데이터를 읽는다.
	if (doc["Effects"].IsArray())
	{
		auto iter = doc["Effects"].Begin();

		for (; iter != doc["Effects"].End(); ++iter)
		{
			switch ((EEffectType)(*iter)["EffectType"].GetUint())
			{
			case EEffectType::PARTICLE:
			{
				// 구조체 준비
				FParticleObjectDesc tParticleObjectDesc = {};
				FParticleInfo tParticleInfo = MakeParticleInfoFromJson(*iter);

				tParticleObjectDesc.m_wstrGameObjectName = CStringUtility::ToWideString((*iter)["Effect Name"].GetString());
				tParticleObjectDesc.m_eEffectType = EEffectType::PARTICLE;

				// 파티클 객체 생성
				shared_ptr<CParticleObject> spParticleObject = CParticleObject::Create(make_shared<FParticleObjectDesc>(tParticleObjectDesc), tParticleInfo);
				CHECKF(spParticleObject, L"Failed to Create ParticleObject : CEffectGroup");

				AddEffect(tParticleObjectDesc.m_wstrGameObjectName, spParticleObject);
				// m_mapEffects.emplace(tParticleObjectDesc.m_wstrGameObjectName, spParticleObject);
				spParticleObject->SetOwnerEffectGroup(static_pointer_cast<CEffectGroup>(shared_from_this()));

				break;
			}
			case EEffectType::EFFECTMESH:
			{
				// 구조체 준비
				FEffectMeshObjectDesc tEffectMeshObjectDesc = {};
				FEffectMeshInfo tEffectMeshInfo = MakeEffectMeshInfoFromJson(*iter);

				tEffectMeshObjectDesc.m_wstrGameObjectName = CStringUtility::ToWideString((*iter)["Effect Name"].GetString());
				tEffectMeshObjectDesc.m_eEffectType = EEffectType::EFFECTMESH;

				tEffectMeshObjectDesc.m_wstrJsonFileName = L"";
				tEffectMeshObjectDesc.m_wstrModelFilePath = tEffectMeshInfo.m_wstrModelFilePath;

				tEffectMeshObjectDesc.m_vInitScale = tEffectMeshInfo.m_vScale;
				tEffectMeshObjectDesc.m_vInitRotate = tEffectMeshInfo.m_vRotation;
				tEffectMeshObjectDesc.m_vInitPosition = tEffectMeshInfo.m_vTranslation;

				tEffectMeshObjectDesc.m_matPivot = Matrix::CreateScale(0.01f) *
					Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
					Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

				// 이펙트 메시 객체 생성
				shared_ptr<CEffectMeshObject> spEffectObject = CEffectMeshObject::Create(make_shared<FEffectMeshObjectDesc>(tEffectMeshObjectDesc), tEffectMeshInfo);
				CHECKF(spEffectObject, L"Failed to Create EffectMeshObject : CEffectGroup");

				AddEffect(tEffectMeshObjectDesc.m_wstrGameObjectName, spEffectObject);
				// m_mapEffects.emplace(tEffectMeshObjectDesc.m_wstrGameObjectName, spEffectObject);
				spEffectObject->SetOwnerEffectGroup(static_pointer_cast<CEffectGroup>(shared_from_this()));

				break;
			}
			case EEffectType::InstanceEffectMesh:
			{
				// 구조체 준비
				FInstanceEffectMeshObjectDesc tInstanceEffectMeshObjectDesc = {};
				FEffectMeshInfo	tInstanceEffectMeshEffectMeshInfo = MakeEffectMeshInfoFromJson(*iter);
				FInstanceEffectMeshInfo tInstanceEffectMeshInfo = MakeInstanceEffectMeshInfoFromJson(*iter);

				tInstanceEffectMeshObjectDesc.m_wstrGameObjectName = CStringUtility::ToWideString((*iter)["Effect Name"].GetString());
				tInstanceEffectMeshObjectDesc.m_eEffectType = EEffectType::InstanceEffectMesh;

				tInstanceEffectMeshObjectDesc.m_wstrJsonFileName = L"";
				tInstanceEffectMeshObjectDesc.m_wstrModelFilePath = tInstanceEffectMeshEffectMeshInfo.m_wstrModelFilePath;

				tInstanceEffectMeshObjectDesc.m_vInitScale = tInstanceEffectMeshEffectMeshInfo.m_vScale;
				tInstanceEffectMeshObjectDesc.m_vInitRotate = tInstanceEffectMeshEffectMeshInfo.m_vRotation;
				tInstanceEffectMeshObjectDesc.m_vInitPosition = tInstanceEffectMeshEffectMeshInfo.m_vTranslation;

				tInstanceEffectMeshObjectDesc.m_matPivot = Matrix::CreateScale(0.01f) *
					Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.f, 1.f, 0.f), XMConvertToRadians(180.f))) *
					Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

				// 이펙트 메시 객체 생성
				shared_ptr<CInstanceEffectMeshObject> spInstanceEffectMeshObject = CInstanceEffectMeshObject::Create(make_shared<FInstanceEffectMeshObjectDesc>(tInstanceEffectMeshObjectDesc), tInstanceEffectMeshEffectMeshInfo, tInstanceEffectMeshInfo);
				CHECKF(spInstanceEffectMeshObject, L"Failed to Create InstanceEffectMeshObject : CEffectGroup");

				AddEffect(tInstanceEffectMeshObjectDesc.m_wstrGameObjectName, spInstanceEffectMeshObject);
				// m_mapEffects.emplace(tInstanceEffectMeshObjectDesc.m_wstrGameObjectName, spInstanceEffectMeshObject);
				spInstanceEffectMeshObject->SetOwnerEffectGroup(static_pointer_cast<CEffectGroup>(shared_from_this()));

				break;
			}
			}
		}
	}

	if (m_tColliderInfo.m_bCollider)
	{
		InitializeCollider(m_tColliderInfo);
	}

	return S_OK;
}
FParticleInfo Client::CEffectGroup::MakeParticleInfoFromJson(rapidjson::Value& _Value)
{
	FParticleInfo tParticleInfo;

	/* Type */
	tParticleInfo.m_eRenderType = (ERenderType)_Value["RenderType"].GetUint();
	tParticleInfo.m_eEffectType = (EEffectType)_Value["EffectType"].GetUint();
	tParticleInfo.m_eParticleType = (EParticleType)_Value["ParticleType"].GetUint();
	tParticleInfo.m_wstrEffectShaderName = CStringUtility::ToWideString(_Value["Effect Shader Name"].GetString());

	/* Option */
	tParticleInfo.m_bDiffuse = _Value["Use Diffuse Texture"].GetBool();
	tParticleInfo.m_wstrDiffuseTextureName = CStringUtility::ToWideString(_Value["Diffuse Texture Name"].GetString());
	tParticleInfo.m_bMask = _Value["Use Mask Texture"].GetBool();
	tParticleInfo.m_wstrMaskTextureName = CStringUtility::ToWideString(_Value["Mask Texture Name"].GetString());

	tParticleInfo.m_bSprite = _Value["Is Sprite"].GetBool();
	tParticleInfo.m_iSpriteRow = _Value["Sprite Row"].GetInt();
	tParticleInfo.m_iSpriteCol = _Value["Sprite Col"].GetInt();
	tParticleInfo.m_fSpriteSpeed = _Value["Sprite Speed"].GetFloat();

	tParticleInfo.m_bLoop = _Value["Is Loop"].GetBool();
	tParticleInfo.m_bBillboard = _Value["Is Billboard"].GetBool();
	tParticleInfo.m_bDirectional = _Value["Is Directional"].GetBool();
	tParticleInfo.m_bGravity = _Value["Is Gravity"].GetBool();
	tParticleInfo.m_bGlow = _Value["Is Glow"].GetBool();
	tParticleInfo.m_bBloom = _Value["Is Bloom"].GetBool();
	tParticleInfo.m_bAlphaAttenuation = _Value["Is AlphaAttenuation"].GetBool();
	tParticleInfo.m_fGravityValue = _Value["Gravity Value"].GetFloat();
	tParticleInfo.m_fGlowWeight = _Value["Glow Weight"].GetFloat();
	tParticleInfo.m_fBloomWeight = _Value["Bloom Weight"].GetFloat();
	tParticleInfo.m_fAlphaAttenuationWeight = _Value["AlphaAttenuation Weight"].GetFloat();

	/* Base Value */
	tParticleInfo.m_bRandomLifeTime = _Value["Random Life Time"].GetBool();

	tParticleInfo.m_vLifeTime.x = _Value["Life Time"][0].GetFloat();
	tParticleInfo.m_vLifeTime.y = _Value["Life Time"][1].GetFloat();

	tParticleInfo.m_iNumInstance = _Value["Num Instance"].GetInt();

	tParticleInfo.m_vSize.x = _Value["Size"][0].GetFloat();
	tParticleInfo.m_vSize.y = _Value["Size"][1].GetFloat();

	tParticleInfo.m_vColor1.x = _Value["Color1"][0].GetFloat();
	tParticleInfo.m_vColor1.y = _Value["Color1"][1].GetFloat();
	tParticleInfo.m_vColor1.z = _Value["Color1"][2].GetFloat();
	tParticleInfo.m_vColor1.w = _Value["Color1"][3].GetFloat();

	tParticleInfo.m_vColor2.x = _Value["Color2"][0].GetFloat();
	tParticleInfo.m_vColor2.y = _Value["Color2"][1].GetFloat();
	tParticleInfo.m_vColor2.z = _Value["Color2"][2].GetFloat();
	tParticleInfo.m_vColor2.w = _Value["Color2"][3].GetFloat();

	tParticleInfo.m_vColor3.x = _Value["Color3"][0].GetFloat();
	tParticleInfo.m_vColor3.y = _Value["Color3"][1].GetFloat();
	tParticleInfo.m_vColor3.z = _Value["Color3"][2].GetFloat();
	tParticleInfo.m_vColor3.w = _Value["Color3"][3].GetFloat();

	tParticleInfo.m_vSpeed.x = _Value["Speed"][0].GetFloat();
	tParticleInfo.m_vSpeed.y = _Value["Speed"][1].GetFloat();

	tParticleInfo.m_vOffset.x = _Value["Offset"][0].GetFloat();
	tParticleInfo.m_vOffset.y = _Value["Offset"][1].GetFloat();
	tParticleInfo.m_vOffset.z = _Value["Offset"][2].GetFloat();

	tParticleInfo.m_vPivot.x = _Value["Pivot"][0].GetFloat();
	tParticleInfo.m_vPivot.y = _Value["Pivot"][1].GetFloat();
	tParticleInfo.m_vPivot.z = _Value["Pivot"][2].GetFloat();

	tParticleInfo.m_vRange.x = _Value["Range"][0].GetFloat();
	tParticleInfo.m_vRange.y = _Value["Range"][1].GetFloat();
	tParticleInfo.m_vRange.z = _Value["Range"][2].GetFloat();

	tParticleInfo.m_vRotation.x = _Value["Rotation Per Second"][0].GetFloat();
	tParticleInfo.m_vRotation.y = _Value["Rotation Per Second"][1].GetFloat();
	tParticleInfo.m_vRotation.z = _Value["Rotation Per Second"][2].GetFloat();

	tParticleInfo.m_vRadius.x = _Value["Radius"][0].GetFloat();
	tParticleInfo.m_vRadius.y = _Value["Radius"][1].GetFloat();

	/* KeyFrame */
	tParticleInfo.m_iNumKeyFrames = _Value["Num KeyFrames"].GetUint();

	tParticleInfo.m_vecKeyFrames.clear();
	tParticleInfo.m_vecKeyFrames.reserve(tParticleInfo.m_iNumKeyFrames);
	tParticleInfo.m_vecKeyFrames.resize(tParticleInfo.m_iNumKeyFrames);

	for (uint32 innerIndex = 0; innerIndex < tParticleInfo.m_iNumKeyFrames; ++innerIndex)
	{
		string strValue = "KeyFrame" + to_string(innerIndex);

		rapidjson::Value& KeyFrame = _Value["KeyFrames"][innerIndex];

		tParticleInfo.m_vecKeyFrames[innerIndex].m_iPassIndex = KeyFrame["Effect Shader Pass Index"].GetInt();

		tParticleInfo.m_vecKeyFrames[innerIndex].m_fTime = KeyFrame["Time"].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_bRenderCulled = KeyFrame["Render Culled"].GetBool();

		tParticleInfo.m_vecKeyFrames[innerIndex].m_fSizeRatio = KeyFrame["Size Ratio"].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_eSizeEasingType = (EEasingType)KeyFrame["Size Easing Type"].GetInt();

		tParticleInfo.m_vecKeyFrames[innerIndex].m_fSpeedRatio = KeyFrame["Speed Ratio"].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_eSpeedEasingType = (EEasingType)KeyFrame["Speed Easing Type"].GetInt();

		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor1.x = KeyFrame["Color1"][0].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor1.y = KeyFrame["Color1"][1].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor1.z = KeyFrame["Color1"][2].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor1.w = KeyFrame["Color1"][3].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor1Weight = KeyFrame["Color1 Weight"].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_eColor1EasingType = (EEasingType)KeyFrame["Color1 Easing Type"].GetUint();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor1EasingExpo = KeyFrame["Color1 Easing Expo"].GetFloat();

		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor2.x = KeyFrame["Color2"][0].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor2.y = KeyFrame["Color2"][1].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor2.z = KeyFrame["Color2"][2].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor2.w = KeyFrame["Color2"][3].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor2Weight = KeyFrame["Color2 Weight"].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_eColor2EasingType = (EEasingType)KeyFrame["Color2 Easing Type"].GetUint();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor2EasingExpo = KeyFrame["Color2 Easing Expo"].GetFloat();

		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor3.x = KeyFrame["Color3"][0].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor3.y = KeyFrame["Color3"][1].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor3.z = KeyFrame["Color3"][2].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_vColor3.w = KeyFrame["Color3"][3].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor3Weight = KeyFrame["Color3 Weight"].GetFloat();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_eColor3EasingType = (EEasingType)KeyFrame["Color3 Easing Type"].GetUint();
		tParticleInfo.m_vecKeyFrames[innerIndex].m_fColor3EasingExpo = KeyFrame["Color3 Easing Expo"].GetFloat();
	}

	return tParticleInfo;
}
FEffectMeshInfo Client::CEffectGroup::MakeEffectMeshInfoFromJson(rapidjson::Value& _Value)
{
	FEffectMeshInfo tEffectMeshInfo;

	/* Type */
	tEffectMeshInfo.m_eRenderType = (ERenderType)_Value["RenderType"].GetUint();
	tEffectMeshInfo.m_eEffectType = (EEffectType)_Value["EffectType"].GetUint();
	tEffectMeshInfo.m_wstrModelFilePath = CStringUtility::ToWideString(_Value["Model File Path"].GetString());
	// 리소스 이름 바뀐거 보정
	if (tEffectMeshInfo.m_wstrModelFilePath.find(L"/SM_") != wstring::npos)
	{
		size_t iIndex = tEffectMeshInfo.m_wstrModelFilePath.find(L"/SM_");
		wstring wstrHead = tEffectMeshInfo.m_wstrModelFilePath.substr(0, iIndex + 1);
		wstring wstrTail = tEffectMeshInfo.m_wstrModelFilePath.substr(iIndex + lstrlen(L"/SM_"));
		tEffectMeshInfo.m_wstrModelFilePath = wstrHead + wstrTail;
	}
	tEffectMeshInfo.m_wstrEffectShaderName = CStringUtility::ToWideString(_Value["Effect Shader Name"].GetString());

	
	if (_Value.HasMember("Is Trail"))
	{
		tEffectMeshInfo.m_bTrail = _Value["Is Trail"].GetBool();
	}
	if (_Value.HasMember("Num Trails"))
	{
		tEffectMeshInfo.m_iNumTrails = _Value["Num Trails"].GetInt();
	}
	if (_Value.HasMember("Trail Term"))
	{
		tEffectMeshInfo.m_fTrailTerm = _Value["Trail Term"].GetFloat();
	}

	/* Texture */
	tEffectMeshInfo.m_vDiffuseColor1.x = _Value["Diffuse Color1"][0].GetFloat();
	tEffectMeshInfo.m_vDiffuseColor1.y = _Value["Diffuse Color1"][1].GetFloat();
	tEffectMeshInfo.m_vDiffuseColor1.z = _Value["Diffuse Color1"][2].GetFloat();
	tEffectMeshInfo.m_vDiffuseColor1.w = _Value["Diffuse Color1"][3].GetFloat();

	tEffectMeshInfo.m_vDiffuseColor2.x = _Value["Diffuse Color2"][0].GetFloat();
	tEffectMeshInfo.m_vDiffuseColor2.y = _Value["Diffuse Color2"][1].GetFloat();
	tEffectMeshInfo.m_vDiffuseColor2.z = _Value["Diffuse Color2"][2].GetFloat();
	tEffectMeshInfo.m_vDiffuseColor2.w = _Value["Diffuse Color2"][3].GetFloat();

	tEffectMeshInfo.m_vDiffuseColor3.x = _Value["Diffuse Color3"][0].GetFloat();
	tEffectMeshInfo.m_vDiffuseColor3.y = _Value["Diffuse Color3"][1].GetFloat();
	tEffectMeshInfo.m_vDiffuseColor3.z = _Value["Diffuse Color3"][2].GetFloat();
	tEffectMeshInfo.m_vDiffuseColor3.w = _Value["Diffuse Color3"][3].GetFloat();

	tEffectMeshInfo.m_wstrDiffuseTextureName = CStringUtility::ToWideString(_Value["Diffuse Texture Name"].GetString());
	tEffectMeshInfo.m_wstrNoiseTextureName = CStringUtility::ToWideString(_Value["Noise Texture Name"].GetString());
	tEffectMeshInfo.m_wstrMaskTextureName = CStringUtility::ToWideString(_Value["Mask Texture Name"].GetString());
	tEffectMeshInfo.m_wstrDistortionTextureName = CStringUtility::ToWideString(_Value["Distortion Texture Name"].GetString());
	tEffectMeshInfo.m_wstrDissolveTextureName = CStringUtility::ToWideString(_Value["Dissolve Texture Name"].GetString());

	/* Option */
	tEffectMeshInfo.m_bSprite = _Value["Is Sprite"].GetBool();
	tEffectMeshInfo.m_iSpriteRow = _Value["Sprite Row"].GetInt();
	tEffectMeshInfo.m_iSpriteCol = _Value["Sprite Col"].GetInt();
	tEffectMeshInfo.m_fSpriteSpeed = _Value["Sprite Speed"].GetFloat();

	tEffectMeshInfo.m_vScale.x = _Value["Scale"][0].GetFloat();
	tEffectMeshInfo.m_vScale.y = _Value["Scale"][1].GetFloat();
	tEffectMeshInfo.m_vScale.z = _Value["Scale"][2].GetFloat();
	tEffectMeshInfo.m_vRotation.x = _Value["Rotation"][0].GetFloat();
	tEffectMeshInfo.m_vRotation.y = _Value["Rotation"][1].GetFloat();
	tEffectMeshInfo.m_vRotation.z = _Value["Rotation"][2].GetFloat();
	tEffectMeshInfo.m_vTranslation.x = _Value["Translation"][0].GetFloat();
	tEffectMeshInfo.m_vTranslation.y = _Value["Translation"][1].GetFloat();
	tEffectMeshInfo.m_vTranslation.z = _Value["Translation"][2].GetFloat();

	tEffectMeshInfo.m_bAnimation = _Value["Is Anim"].GetBool();
	tEffectMeshInfo.m_bLoop = _Value["Is Loop"].GetBool();
	tEffectMeshInfo.m_bUVMoveLoop = _Value["Is UVMove Loop"].GetBool();
	tEffectMeshInfo.m_bGlow = _Value["Is Glow"].GetBool();
	tEffectMeshInfo.m_bBloom = _Value["Is Bloom"].GetBool();
	tEffectMeshInfo.m_bAlphaAttenuation = _Value["Is AlphaAttenuation"].GetBool();

	tEffectMeshInfo.m_bDiffuse = _Value["Use Diffuse Texture"].GetBool();
	tEffectMeshInfo.m_bNoise = _Value["Use Noise Texture"].GetBool();
	tEffectMeshInfo.m_bMask = _Value["Use Mask Texture"].GetBool();
	tEffectMeshInfo.m_bDistortion = _Value["Use Distortion Texture"].GetBool();
	tEffectMeshInfo.m_bDissolve = _Value["Use Dissolve Texture"].GetBool();

	/* Base Value */
	tEffectMeshInfo.m_fDistortionWeight = _Value["Distortion Weight"].GetFloat();
	tEffectMeshInfo.m_fDissolveWeight = _Value["Dissolve Weight"].GetFloat();
	tEffectMeshInfo.m_fGlowWeight = _Value["Glow Weight"].GetFloat();
	tEffectMeshInfo.m_fBloomWeight = _Value["Bloom Weight"].GetFloat();
	tEffectMeshInfo.m_fAlphaAttenuationWeight = _Value["AlphaAttenuation Weight"].GetFloat();
	tEffectMeshInfo.m_vLifeTime.x = _Value["Life Time"][0].GetFloat();
	tEffectMeshInfo.m_vLifeTime.y = _Value["Life Time"][1].GetFloat();
	tEffectMeshInfo.m_vUVMoveSpeed.x = _Value["UVMove Speed"][0].GetFloat();
	tEffectMeshInfo.m_vUVMoveSpeed.y = _Value["UVMove Speed"][1].GetFloat();

	/* KeyFrame */
	tEffectMeshInfo.m_iNumKeyFrames = _Value["Num KeyFrames"].GetUint();
	tEffectMeshInfo.m_vecKeyFrames.clear();
	tEffectMeshInfo.m_vecKeyFrames.reserve(tEffectMeshInfo.m_iNumKeyFrames);
	tEffectMeshInfo.m_vecKeyFrames.resize(tEffectMeshInfo.m_iNumKeyFrames);

	for (uint32 innerIndex = 0; innerIndex < tEffectMeshInfo.m_iNumKeyFrames; ++innerIndex)
	{
		string strValue = "KeyFrame" + to_string(innerIndex);

		rapidjson::Value& KeyFrame = _Value["KeyFrames"][innerIndex];

		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fTime = KeyFrame["Time"].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_bRenderCulled = KeyFrame["Render Culled"].GetBool();

		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.x = KeyFrame["Color1"][0].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.y = KeyFrame["Color1"][1].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.z = KeyFrame["Color1"][2].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor1.w = KeyFrame["Color1"][3].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor1Weight = KeyFrame["Color1 Weight"].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eColor1EasingType = (EEasingType)KeyFrame["Color1 Easing Type"].GetUint();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor1EasingExpo = KeyFrame["Color1 Easing Expo"].GetFloat();

		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.x = KeyFrame["Color2"][0].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.y = KeyFrame["Color2"][1].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.z = KeyFrame["Color2"][2].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor2.w = KeyFrame["Color2"][3].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor2Weight = KeyFrame["Color2 Weight"].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eColor2EasingType = (EEasingType)KeyFrame["Color2 Easing Type"].GetUint();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor2EasingExpo = KeyFrame["Color2 Easing Expo"].GetFloat();

		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.x = KeyFrame["Color3"][0].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.y = KeyFrame["Color3"][1].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.z = KeyFrame["Color3"][2].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vColor3.w = KeyFrame["Color3"][3].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor3Weight = KeyFrame["Color3 Weight"].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eColor3EasingType = (EEasingType)KeyFrame["Color3 Easing Type"].GetUint();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fColor3EasingExpo = KeyFrame["Color3 Easing Expo"].GetFloat();

		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vScale.x = KeyFrame["Scale"][0].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vScale.y = KeyFrame["Scale"][1].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vScale.z = KeyFrame["Scale"][2].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eScaleEasingType = (EEasingType)KeyFrame["Scale Easing Type"].GetUint();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fScaleEasingExpo = KeyFrame["Scale Easing Expo"].GetFloat();

		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vRotation.x = KeyFrame["Rotation"][0].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vRotation.y = KeyFrame["Rotation"][1].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vRotation.z = KeyFrame["Rotation"][2].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eRotationEasingType = (EEasingType)KeyFrame["Rotation Easing Type"].GetUint();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fRotationEasingExpo = KeyFrame["Rotation Easing Expo"].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fRotateWeight = KeyFrame["Rotate Weight"].GetFloat();

		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vTranslation.x = KeyFrame["Translation"][0].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vTranslation.y = KeyFrame["Translation"][1].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_vTranslation.z = KeyFrame["Translation"][2].GetFloat();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_eTranslationEasingType = (EEasingType)KeyFrame["Translation Easing Type"].GetUint();
		tEffectMeshInfo.m_vecKeyFrames[innerIndex].m_fTranslationEasingExpo = KeyFrame["Translation Easing Expo"].GetFloat();
	}

	return tEffectMeshInfo;
}
FInstanceEffectMeshInfo Client::CEffectGroup::MakeInstanceEffectMeshInfoFromJson(rapidjson::Value& _Value)
{
	FInstanceEffectMeshInfo tInstanceEffectMeshInfo;

	/* Type */
	tInstanceEffectMeshInfo.m_eParticleType = (EParticleType)_Value["Instance Mesh Particle Type"].GetInt();
	tInstanceEffectMeshInfo.m_iNumInstance = _Value["Num Mesh Instance"].GetInt();

	tInstanceEffectMeshInfo.m_bGravity = _Value["Is Gravity"].GetBool();
	tInstanceEffectMeshInfo.m_fGravityValue = _Value["Gravity Value"].GetFloat();

	tInstanceEffectMeshInfo.m_vSize.x = _Value["Size"][0].GetFloat();
	tInstanceEffectMeshInfo.m_vSize.y = _Value["Size"][1].GetFloat();

	tInstanceEffectMeshInfo.m_vSpeed.x = _Value["Speed"][0].GetFloat();
	tInstanceEffectMeshInfo.m_vSpeed.y = _Value["Speed"][1].GetFloat();

	tInstanceEffectMeshInfo.m_vRange.x = _Value["Range"][0].GetFloat();
	tInstanceEffectMeshInfo.m_vRange.y = _Value["Range"][1].GetFloat();
	tInstanceEffectMeshInfo.m_vRange.z = _Value["Range"][2].GetFloat();

	tInstanceEffectMeshInfo.m_vRadius.x = _Value["Radius"][0].GetFloat();
	tInstanceEffectMeshInfo.m_vRadius.y = _Value["Radius"][1].GetFloat();

	tInstanceEffectMeshInfo.m_vRotation.x = _Value["Rotation Per Seconds"][0].GetFloat();
	tInstanceEffectMeshInfo.m_vRotation.y = _Value["Rotation Per Seconds"][1].GetFloat();
	tInstanceEffectMeshInfo.m_vRotation.z = _Value["Rotation Per Seconds"][2].GetFloat();

	return tInstanceEffectMeshInfo;
}

HRESULT Client::CEffectGroup::AddEffect(const wstring& _wstrEffectName, shared_ptr<CEffectBase> _spEffect)
{
	if (!_spEffect) { return E_FAIL; }
	if (m_mapEffects.contains(_wstrEffectName)) { return E_FAIL; }

	m_mapEffects.emplace(_wstrEffectName, _spEffect);
	_spEffect->SetParentGameObject(shared_from_this());

	return S_OK;
}
HRESULT Client::CEffectGroup::RemoveEffect(const wstring& _wstrEffectName)
{
	if (!m_mapEffects.contains(_wstrEffectName)) { return E_FAIL; }

	m_mapEffects[_wstrEffectName]->Release();
	m_mapEffects[_wstrEffectName].reset();

	m_mapEffects.erase(_wstrEffectName);

	return S_OK;
}
shared_ptr<CEffectBase> Client::CEffectGroup::FindEffect(const wstring& _wstrEffectName)
{
	if (m_mapEffects.contains(_wstrEffectName)) { return m_mapEffects[_wstrEffectName]; }

	return nullptr;
}

void Client::CEffectGroup::AttachToBone()
{
	if (!m_spCombinedBone && !m_spCombinedGameObject) { return; }

	Vector4 vFinalPosition = Vector4(0.f, 0.f, 0.f, 1.f);
	Matrix matCombined = XMMatrixIdentity();

	// 부착된 본은 없고, 부착된 오브젝트만 있는 경우
	if (!m_spCombinedBone && m_spCombinedGameObject)
	{
		Matrix matCombinedOffset = Matrix::CreateTranslation(m_vCombinedOffset);
		Matrix matCombinedObject = Matrix::CreateTranslation(m_spCombinedGameObject->GetPosition().value());

		matCombined = matCombinedOffset * matCombinedObject;
	}
	else
	{
		Matrix matCombinedOffset = Matrix::CreateTranslation(m_vCombinedOffset);
		Matrix matCombinedBone = m_spCombinedBone->GetCombinedTransformationMatrix();
		Matrix matCombinedObject = m_spCombinedGameObject->GetTransformMatrix().value();

		matCombined = matCombinedOffset * matCombinedBone * matCombinedObject;
	}

	// matCombined = CMathUtility::GetScaledKilledMatrix(matCombined);

	// SetTransformMatrix(matCombined);

	vFinalPosition = Vector4(matCombined.Translation());
	vFinalPosition.w = 1.f;

	SetTransform(ETransformType::POSITION, vFinalPosition);

	Vector3 vLook = m_spCombinedGameObject->GetForwardVector().value();
	vLook.Normalize();

	LookAt(GetPosition().value() + vLook);

	// 비둘기 목 코드
	if (m_wstrEffectGroupName == L"Group_Fiho_FireBreath")
	{
		matCombined = CMathUtility::GetScaledKilledMatrix(matCombined);
		SetTransformMatrix(matCombined);
	}
}

void Client::CEffectGroup::UpdateColliderInfo(FColliderInfo _tColliderInfo)
{
	m_tColliderInfo = _tColliderInfo;

	if (m_spSphereComponent || m_spBoxComponent)
	{
		m_tColliderInfo.m_bCollider = true;
	}

	m_tColliderInfo.m_eCollisionFlag = _tColliderInfo.m_eCollisionFlag;
	m_tColliderInfo.m_eCollisionLayer = _tColliderInfo.m_eCollisionLayer;

	switch (_tColliderInfo.m_eColliderType)
	{
	case EColliderType::SPHERE:
	{
		if (!m_spSphereComponent) { return; }

		m_spSphereComponent->SetRadius(_tColliderInfo.m_fRadius);

		break;
	}
	case EColliderType::BOX:
	{
		if (!m_spBoxComponent) { return; }

		m_spBoxComponent->SetExtents(_tColliderInfo.m_vExtents);
		m_spBoxComponent->SetLocalPosition(_tColliderInfo.m_vLocalPosition);
		// m_spBoxComponent->SetLocalRotate(_tColliderInfo.m_vLocalRotate);
		break;
	}
	}
}

void Client::CEffectGroup::SetCull(bool _bCulled)
{
	SetPosition(Vector3(0.f, -1000.f, 0.f));
	SetCulled(_bCulled);

	for (auto& pair : m_mapEffects)
	{
		pair.second->SetCulled(_bCulled);

		switch (pair.second->GetEffectType())
		{
		case EEffectType::PARTICLE:
		{
			static_pointer_cast<CParticleObject>(pair.second)->GetParticleCom()->EndLifeTime();
			break;
		}
		case EEffectType::EFFECTMESH:
		{
			static_pointer_cast<CEffectMeshObject>(pair.second)->GetEffectMeshCom()->EndLifeTime();
			break;
		}
		case EEffectType::InstanceEffectMesh:
		{
			static_pointer_cast<CInstanceEffectMeshObject>(pair.second)->GetInstanceEffectMeshCom()->EndLifeTime();
			break;
		}
		}
	}

	// m_fTrackPosition = 0.f;
}
void Client::CEffectGroup::NoneActivate()
{
	SetActive(false);
	SetCulled(true);

	m_spCombinedGameObject = nullptr;
	m_spCombinedBone = nullptr;

	m_fTrackPosition = 0.f;
}
void Client::CEffectGroup::EndLifeTime()
{
	SetPosition(Vector3(0.f, -1000.f, 0.f));
	SetActive(false);
	SetCulled(true);

	m_spCombinedGameObject = nullptr;
	m_spCombinedBone = nullptr;

	m_fTrackPosition = 0.f;

	if (EChannelType::ENUM_END != m_eChannel)
	{
		m_wpSoundManager.lock()->StopChannelEx(m_eChannel);
	}

	for (auto& pair : m_mapEffects)
	{
		switch (pair.second->GetEffectType())
		{
		case EEffectType::PARTICLE:
		{
			static_pointer_cast<CParticleObject>(pair.second)->GetParticleCom()->EndLifeTime();
			break;
		}
		case EEffectType::EFFECTMESH:
		{
			static_pointer_cast<CEffectMeshObject>(pair.second)->GetEffectMeshCom()->EndLifeTime();
			break;
		}
		case EEffectType::InstanceEffectMesh:
		{
			static_pointer_cast<CInstanceEffectMeshObject>(pair.second)->GetInstanceEffectMeshCom()->EndLifeTime();
			break;
		}
		}
	}
}
void Client::CEffectGroup::Loop()
{
	SetActive(true);
	SetCulled(false);
	m_fTrackPosition = 0.f;

	for (auto& pair : m_mapEffects)
	{
		switch (pair.second->GetEffectType())
		{
		case EEffectType::PARTICLE:
		{
			static_pointer_cast<CParticleObject>(pair.second)->GetParticleCom()->RecycleFromPool();
			break;
		}
		case EEffectType::EFFECTMESH:
		{
			static_pointer_cast<CEffectMeshObject>(pair.second)->GetEffectMeshCom()->RecycleFromPool();
			break;
		}
		case EEffectType::InstanceEffectMesh:
		{
			static_pointer_cast<CInstanceEffectMeshObject>(pair.second)->GetInstanceEffectMeshCom()->RecycleFromPool();
			break;
		}
		}
	}
}
void Client::CEffectGroup::SetCollisionOption(ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag)
{
	if (!m_spBoxComponent && !m_spSphereComponent) { return; }

	if (m_spBoxComponent && !m_spSphereComponent)
	{
		m_spBoxComponent->SetCollisionLayer(_eCollisionLayer, _eCollisionFlag);
	}
	else if (!m_spBoxComponent && m_spSphereComponent)
	{
		m_spSphereComponent->SetCollisionLayer(_eCollisionLayer, _eCollisionFlag);
	}
}
void Client::CEffectGroup::Reset()
{
	SetActive(true);
	SetCulled(false);
	m_fTrackPosition = 0.f;

	if (!m_wstrSoundName.empty())
	{
		string strSoundName = CStringUtility::ToString(m_wstrSoundName);
		m_eChannel = m_wpSoundManager.lock()->PlaySoundEx(strSoundName, GetPosition().value(), m_fSoundValue, m_bSoundLoop);
	}

	for (auto& pair : m_mapEffects)
	{
		switch (pair.second->GetEffectType())
		{
		case EEffectType::PARTICLE:
		{
			static_pointer_cast<CParticleObject>(pair.second)->GetParticleCom()->RecycleFromPool();
			break;
		}
		case EEffectType::EFFECTMESH:
		{
			static_pointer_cast<CEffectMeshObject>(pair.second)->GetEffectMeshCom()->RecycleFromPool();
			break;
		}
		case EEffectType::InstanceEffectMesh:
		{
			static_pointer_cast<CInstanceEffectMeshObject>(pair.second)->GetInstanceEffectMeshCom()->RecycleFromPool();
			break;
		}
		}
	}
}