#include "pch.h"
#include "Rock.h"

// module, manager
#include "ClientLoop.h"
#include "EngineModule.h"
#include "ClientModule.h"
#include "EffectManager.h"
#include "CameraManager.h"
#include "GameManager.h"
#include "TimerManager.h"
#include "SoundManager.h"

// system
#include "InventorySystem.h"
#include "ToDoSystem.h"

// math utility
#include "MathUtility.h"

// gameobject
#include "Creature.h"
#include "Mineral.h"
#include "Camera.h"
#include "Panel.h"

// component
#include "Component.h"
#include "SphereComponent.h"
#include "ColliderComponent.h"
#include "MeshComponent.h"
#include "CameraComponent.h"
#include "AnimationImageComponent.h"
#include "ImageComponent.h"

std::shared_ptr<CRock> Client::CRock::Create(const std::shared_ptr<FRockDesc>& _spRockDesc)
{
	const std::shared_ptr<CRock> spRock = make_shared<CRock>();
	ENSUREF(spRock->Initialize(_spRockDesc), L"Failed to initialize rock.");
	return spRock;
}

HRESULT Client::CRock::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// nature desc
	if (FAILED(CNature::Initialize(_spGameObjectDesc))) { return E_FAIL; };

	m_eNatureType = ENatureType::ROCK;

	// rock desc
	shared_ptr<FRockDesc> spRockDesc = static_pointer_cast<FRockDesc>(_spGameObjectDesc);
	m_vCenter = spRockDesc->m_vCenter;
	m_fRadius = spRockDesc->m_fRadius;
	m_fCurHp = spRockDesc->m_fCurHP;

	{
		// sphere component desc
		FSphereComponentDesc tSphereComponentDesc;
		tSphereComponentDesc.m_fRadius				= m_fRadius;

		// collider component desc
		tSphereComponentDesc.m_eMobilityType		= EMobilityType::STATIC;
		tSphereComponentDesc.m_eColliderType		= EColliderType::SPHERE;
		tSphereComponentDesc.m_eCollisionLayer		= ECollisionLayer::NATURE;
		tSphereComponentDesc.m_eCollisionFlag		= ECollisionFlag::NATURE;
		tSphereComponentDesc.m_bGravity				= false;
		tSphereComponentDesc.m_bTrigger				= false;
		tSphereComponentDesc.m_bDeferredAdd			= true;
		tSphereComponentDesc.m_iAxisLockType		= 0;
		tSphereComponentDesc.m_vLocalPosition		= m_vCenter;

		// component desc
		tSphereComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tSphereComponentDesc.m_eComponentType		= EComponentType::SPHERE;
		tSphereComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tSphereComponentDesc.m_wstrComponentName	= L"SphereComponent";

		m_spBodySphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		ENSURE(AddComponent(L"SphereComponent", m_spBodySphereComponent));
	}

	{
		// mesh component
		FMeshComponentDesc tMeshComponentDesc;
		tMeshComponentDesc.m_matPivot				= Matrix::CreateScale(1.0f);
		tMeshComponentDesc.m_spParentMeshComponent	= nullptr;
		tMeshComponentDesc.m_wstrModelFilePath		= spRockDesc->m_wstrModelFilePath;
		tMeshComponentDesc.m_wstrShaderName			= L"";
		tMeshComponentDesc.m_eMeshType				= EMeshManageType::INSTANCE;

		// component desc
		tMeshComponentDesc.m_eRenderType			= ERenderType::RENDER_NBLEND;
		tMeshComponentDesc.m_eComponentType			= EComponentType::MESH;
		tMeshComponentDesc.m_wpOwnerGameObject		= shared_from_this();
		tMeshComponentDesc.m_wstrComponentName		= L"MeshComponent";

		m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
		ENSURE(AddComponent(L"MeshComponent", m_spMeshComponent));
	}
	return S_OK;
}

HRESULT Client::CRock::BeginPlay()
{
	if (FAILED(CNature::BeginPlay())) { return E_FAIL; };

	return S_OK;
}

int32 Client::CRock::PreTick(float _fDeltaSeconds)
{
	CNature::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CRock::Tick(float _fDeltaSeconds)
{
	CNature::Tick(_fDeltaSeconds);

	if (m_bStartShaking)
	{
		ShakeNature(_fDeltaSeconds);
	}

	return 0;
}

int32 Client::CRock::PostTick(float _fDeltaSeconds)
{
	CNature::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CRock::EndPlay()
{
	if (FAILED(CNature::EndPlay())) { return E_FAIL; };

	return S_OK;
}

void Client::CRock::Release()
{
	m_spBodySphereComponent = nullptr;

	CNature::Release();
}

HRESULT Client::CRock::AddHabitatToScene()
{
	return m_spBodySphereComponent->AddRigidActorToScene();
}

HRESULT Client::CRock::RemoveHabitatFromScene()
{
	return m_spBodySphereComponent->RemoveRigidActorFromScene();
}

void Client::CRock::TakeDamage(const std::shared_ptr<CGameObject>& _spGameObject, float _fDamage)
{
	m_fCurHp -= _fDamage;
	m_bShaked = true;

	// 바위 채굴 이펙트를 출력합니다.
	CClientModule::GetInstance()->GetEffectManager()->ActivateEffect(L"Group_Mine", GetPosition().value() + Vector3(0.f, 1.f, 0.f));

	weak_ptr<CCreature> wpCreature = dynamic_pointer_cast<CCreature>(_spGameObject);
	if (wpCreature.expired()) { return; }

	if (wpCreature.lock()->GetCreatureType() == ECreatureType::PLAYER)
	{
		// 일정 확률로 돌맹이를 얻습니다.
		if (IsDropItem())
		{
			// 랜덤으로 숫자를 얻습니다.
			int32 iRandomNum0 = CMathUtility::GetRandomInt(5, 15);
			int32 iRandomNum1 = CMathUtility::GetRandomInt(0, 3);

			if (iRandomNum1 == 1 &&
				!m_wpGetItemAnimationImageComponentSlot1.lock()->IsActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpGameManager.lock()->GetInventorySystem()->AddItemToContainer(EItemType::PALDIUM_FRAGMENT, iRandomNum1);

				m_wpGetItemParentPanel.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot1.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot1.lock()->SetTextureFileName(L"T_get_paldium");
				m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(true);
			}
			if (!m_wpGetItemAnimationImageComponentSlot0.lock()->IsActive())
			{
				m_wpGameManager.lock()->GetInventorySystem()->AddItemToContainer(EItemType::STONE, iRandomNum0);
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpGetItemAnimationImageComponentSlot0.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetTextureFileName(L"T_get_stone");
			}
		}
	}
	else if (wpCreature.lock()->GetCreatureType() == ECreatureType::PAL)
	{
		// 일정 확률로 돌맹이를 얻습니다.
		if (IsDropItem())
		{
			// 랜덤으로 숫자를 얻습니다.
			int32 iRandomNum0 = CMathUtility::GetRandomInt(5, 15);
			int32 iRandomNum1 = CMathUtility::GetRandomInt(0, 3);
			m_wpGameManager.lock()->GetInventorySystem()->AddItemToContainer(EItemType::PALDIUM_FRAGMENT, iRandomNum1);
			m_wpGameManager.lock()->GetInventorySystem()->AddItemToContainer(EItemType::STONE, iRandomNum0);
		}
	}

	// 채굴한 횟수에 따라서 바위의 크기를 감소시킵니다.
	if (m_iGetDamageCount == 4)
	{
		Vector3 vScale = GetScale().value();
		Vector3 vTargetScale = vScale * 0.75f;
		SetScale(vTargetScale);

		Vector3 vTranslate = GetPosition().value();
		Vector3 vTragetTranslate = Vector3(vTranslate.x, vTranslate.y - 0.25f, vTranslate.z);
		SetPosition(vTragetTranslate);
	}
	else if (m_iGetDamageCount == 8)
	{
		Vector3 vScale = GetScale().value();
		Vector3 vTargetScale = vScale * 0.75f;
		SetScale(vTargetScale);

		Vector3 vTranslate = GetPosition().value();
		Vector3 vTragetTranslate = Vector3(vTranslate.x, vTranslate.y - 0.25f, vTranslate.z);
		SetPosition(vTragetTranslate);
	}
	else if (m_fCurHp <= 0.0f)
	{
		// 바위 채굴 이펙트를 출력합니다.
		CClientModule::GetInstance()->GetEffectManager()->ActivateEffect(L"Group_RockDestroy_Smoke", GetPosition().value() + Vector3(0.f, 1.f, 0.f));

		m_fCurHp = 0.0f;
		m_iGetDamageCount = 0;

		SetCulled(true);
	}
}

void Client::CRock::RespawnNature(float _fDeltaSeconds)
{
	if (0.0f < m_fCurHp) { return; }

	m_fSumRespawnDeltaSeconds += _fDeltaSeconds;
	if (CClientLoop::s_spClientDataSheet->k_fMaxNatureRespawnDeltaSeconds <= m_fSumRespawnDeltaSeconds)
	{
		m_fSumRespawnDeltaSeconds = 0.0f;

		m_fCurHp = CClientLoop::s_spClientDataSheet->k_fMaxRockHp;
		SetCulled(false);
	}
}

void Client::CRock::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 플레이어 컨트롤러 컴포넌트
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		return;
	}

	// 콜라이더 컴포넌트
	CColliderComponent* pColliderComponent = static_cast<CColliderComponent*>(_pDstComponent);
	
	// 바위를 작업 시스템에 등록합니다.		
	if (pColliderComponent->GetCollisionLayer() == ECollisionLayer::PLAYER_WEAPON)
	{
		m_bStartShaking = true;
		m_wpSoundManager.lock()->PlaySoundEx("MiningStone");
	}

	if (pColliderComponent->GetCollisionLayer() == ECollisionLayer::BASE_CAMP)
	{
		m_iCurCommandID = RegisterCommandToSystem(EToDoType::MINE_RAW_ROCK, EMonsterTaskType::MINE);
	}
}

bool Client::CRock::DestroyNature(float _fDeltaSeconds)
{
	m_fSumMineDeltaSeconds += _fDeltaSeconds;
	if (CClientLoop::s_spClientDataSheet->k_fMaxMineDeltaSeconds <= m_fSumMineDeltaSeconds)
	{
		m_fSumMineDeltaSeconds = 0.0f;
		TakeDamage(nullptr, CClientLoop::s_spClientDataSheet->k_fMineDamage);
	}

	return m_fCurHp <= 0.0f ? true : false;
}

float Client::CRock::GetAcceptableRadius()
{
	return m_spBodySphereComponent->GetRadius() + 0.2f;
}
