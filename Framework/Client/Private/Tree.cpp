#include "pch.h"
#include "Tree.h"

// Module, Manager
#include "EngineModule.h"
#include "ClientLoop.h"
#include "GameManager.h"
#include "ClientModule.h"
#include "EffectManager.h"
#include "SoundManager.h"

// System
#include "InventorySystem.h"

// MathUtility
#include "MathUtility.h"

// GameObject
#include "Mineral.h"
#include "Creature.h"

// Component
#include "Panel.h"
#include "Component.h"
#include "BoxComponent.h"
#include "MeshComponent.h"
#include "ImageComponent.h"
#include "ColliderComponent.h"
#include "AnimationImageComponent.h"

// system
#include "ToDoSystem.h"

std::shared_ptr<CTree> Client::CTree::Create (const std::shared_ptr<FTreeDesc>& _spTreeDesc)
{
	const std::shared_ptr<CTree> spTree = make_shared<CTree>();
	ENSUREF(spTree->Initialize(_spTreeDesc), L"Failed to initialize tree.");
	return spTree;
}

HRESULT Client::CTree::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// nature desc
	if (FAILED(CNature::Initialize(_spGameObjectDesc))) { return E_FAIL; };

	m_eNatureType = ENatureType::TREE;

	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	// tree desc
	shared_ptr<FTreeDesc> spTreeDesc = static_pointer_cast<FTreeDesc>(_spGameObjectDesc);
	m_vExtents = spTreeDesc->m_vExtents;
	m_vCenter = spTreeDesc->m_vCenter;
	m_fCurHp = spTreeDesc->m_fCurHp;

	{
		// mesh component
		FMeshComponentDesc tMeshComponentDesc;
		tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.01f);
		tMeshComponentDesc.m_spParentMeshComponent = nullptr;
		tMeshComponentDesc.m_wstrModelFilePath = spTreeDesc->m_wstrModelFilePath;
		tMeshComponentDesc.m_wstrShaderName = L"";
		tMeshComponentDesc.m_eMeshType = EMeshManageType::INSTANCE;

		// component desc
		tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";

		m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
		ENSURE(AddComponent(L"MeshComponent", m_spMeshComponent));
	}

	{
		// box component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_vExtents			= m_vExtents;

		// collider component desc
		tBoxComponentDesc.m_eMobilityType		= EMobilityType::STATIC;
		tBoxComponentDesc.m_eColliderType		= EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionLayer		= ECollisionLayer::NATURE;
		tBoxComponentDesc.m_eCollisionFlag		= ECollisionFlag::NATURE;
		tBoxComponentDesc.m_bGravity			= false;
		tBoxComponentDesc.m_bTrigger			= false;
		tBoxComponentDesc.m_vLocalPosition		= m_vCenter;

		// component desc
		tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
		tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tBoxComponentDesc.m_wstrComponentName	= L"BoxComponent";

		m_spBodyBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(L"BoxComponent", m_spBodyBoxComponent));
	}

	return S_OK;
}

HRESULT Client::CTree::BeginPlay()
{
	if (FAILED(CNature::BeginPlay())) { return E_FAIL; };

	return S_OK;
}

int32 Client::CTree::PreTick(float _fDeltaSeconds)
{
	CNature::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CTree::Tick(float _fDeltaSeconds)
{
	CNature::Tick(_fDeltaSeconds);

	if (m_bStartShaking)
	{
		ShakeNature(_fDeltaSeconds);
	}

	return 0;
}

int32 Client::CTree::PostTick(float _fDeltaSeconds)
{
	CNature::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CTree::EndPlay()
{
	if (FAILED(CNature::EndPlay())) { return E_FAIL; };

	return S_OK;
}

void Client::CTree::Release()
{
	m_spBodyBoxComponent = nullptr;

	CNature::Release();
}

HRESULT Client::CTree::AddHabitatToScene()
{
	return m_spBodyBoxComponent->AddRigidActorToScene();
}

HRESULT Client::CTree::RemoveHabitatFromScene()
{
	return m_spBodyBoxComponent->RemoveRigidActorFromScene();
}

void Client::CTree::TakeDamage(const std::shared_ptr<CGameObject>& _spGameObject, float _fDamage)
{
	m_fCurHp -= _fDamage;
	m_bShaked = true;

	// 나무 베기 이펙트를 출력합니다.
	CEffectManager::GetInstance()->ActivateEffect(L"Group_Hit3", GetPosition().value() + Vector3(0.f, 1.2f, 0.f));
	CEffectManager::GetInstance()->ActivateEffect(L"Group_Build_Particle", GetPosition().value() + Vector3(0.f, 1.2f, 0.f));
	CClientModule::GetInstance()->GetEffectManager()->ActivateEffect(L"Group_WoodCutting", GetPosition().value() + Vector3(0.f, 1.5f, 0.f));

	weak_ptr<CCreature> wpCreature = dynamic_pointer_cast<CCreature>(_spGameObject);
	if (wpCreature.expired()) { return; }

	if (wpCreature.lock()->GetCreatureType() == ECreatureType::PLAYER)
	{
		// 나무 베기 사운드를 출력합니다.
		m_wpSoundManager.lock()->PlaySoundEx("Punch", 0.5f, false);

		// 일정 확률로 목재를 얻습니다.
		if (IsDropItem())
		{
			// 랜덤으로 숫자를 얻습니다.
			int32 iRandomNum0 = CMathUtility::GetRandomInt(5, 15);
			int32 iRandomNum1 = CMathUtility::GetRandomInt(0, 3);

			if (iRandomNum1 == 1 &&
				!m_wpGetItemAnimationImageComponentSlot1.lock()->IsActive())
			{
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpGameManager.lock()->GetInventorySystem()->AddItemToContainer(EItemType::RED_BERRY, iRandomNum1);

				m_wpGetItemParentPanel.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot1.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot1.lock()->SetTextureFileName(L"T_get_berry");
				m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(true);
			}
			if (!m_wpGetItemAnimationImageComponentSlot0.lock()->IsActive())
			{
				m_wpGameManager.lock()->GetInventorySystem()->AddItemToContainer(EItemType::WOOD, iRandomNum0);
				m_wpSoundManager.lock()->PlaySoundEx("GetItem");
				m_wpGetItemAnimationImageComponentSlot0.lock()->ResetFrame();
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(true);
				m_wpGetItemAnimationImageComponentSlot0.lock()->SetTextureFileName(L"T_get_wood");
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
			m_wpGameManager.lock()->GetInventorySystem()->AddItemToContainer(EItemType::RED_BERRY, iRandomNum1);
			m_wpGameManager.lock()->GetInventorySystem()->AddItemToContainer(EItemType::WOOD, iRandomNum0);
		}
	}

	/*if (m_wpGetItemAnimationImageComponentSlot0.lock()->IsFinish())
	{
	   m_wpGetItemAnimationImageComponentSlot0.lock()->SetActive(false);
	}
	if (m_wpGetItemAnimationImageComponentSlot1.lock()->IsFinish())
	{
	   m_wpGetItemAnimationImageComponentSlot1.lock()->SetActive(false);
	}*/

	if (m_fCurHp <= 0.0f)
	{
		m_fCurHp = 0.0f;
		SetCulled(true);
		CClientModule::GetInstance()->GetEffectManager()->ActivateEffect(L"Group_WoodDestroy", GetPosition().value());
	}
}

void Client::CTree::RespawnNature(float _fDeltaSeconds)
{
	if (0.0f < m_fCurHp) { return; }

	m_fSumRespawnDeltaSeconds += _fDeltaSeconds;
	if (CClientLoop::s_spClientDataSheet->k_fMaxNatureRespawnDeltaSeconds <= m_fSumRespawnDeltaSeconds)
	{
		m_fSumRespawnDeltaSeconds = 0.0f;

		m_fCurHp = CClientLoop::s_spClientDataSheet->k_fMaxTreeHp;
		SetCulled(false);

		// 작업 시스템에 명령을 등록합니다.
		m_iCurCommandID = RegisterCommandToSystem(EToDoType::CUT_DOWN_RAW_TREE, EMonsterTaskType::LUMBER);
	}
}

void Client::CTree::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 플레이어 컨트롤러 컴포넌트
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER) { return; }

	// 콜라이더 컴포넌트
	CColliderComponent* pColliderComponent = static_cast<CColliderComponent*>(_pDstComponent);

	// 나무를 작업 시스템에 등록합니다.
	if (pColliderComponent->GetCollisionLayer() == ECollisionLayer::PLAYER_WEAPON)
	{
		m_bStartShaking = true;
		m_wpSoundManager.lock()->PlaySoundEx("FellingTree");
	}

	if (pColliderComponent->GetCollisionLayer() == ECollisionLayer::BASE_CAMP)
	{
		m_iCurCommandID = RegisterCommandToSystem(EToDoType::CUT_DOWN_RAW_TREE, EMonsterTaskType::LUMBER);
	}
}

bool Client::CTree::DestroyNature(float _fDeltaSeconds)
{
	m_fSumCutDownDeltaSeconds += _fDeltaSeconds;
	if (CClientLoop::s_spClientDataSheet->k_fMaxCutDownDeltaSeconds <= m_fSumCutDownDeltaSeconds)
	{
		m_fSumCutDownDeltaSeconds = 0.0f;
		TakeDamage(nullptr, CClientLoop::s_spClientDataSheet->k_fCutDownDamage);
	}

	return m_fCurHp <= 0.0f ? true : false;
}

float Client::CTree::GetAcceptableRadius()
{
	Vector3 vExtents = m_spBodyBoxComponent->GetExtents();

	return max(vExtents.x, vExtents.z) + 0.2f;;
}
