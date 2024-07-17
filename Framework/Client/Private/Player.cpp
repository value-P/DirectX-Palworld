// default
#include "pch.h"
#include "Player.h"
#include "MathUtility.h"

// manager
#include "EngineModule.h"
#include "ClientModule.h"
#include "CameraManager.h"
#include "InputManager.h"
#include "GameManager.h"
#include "BuildManager.h"
#include "EffectManager.h"
#include "PhysXManager.h"
#include "UIManager.h"
#include "BuildManager.h"
#include "ToDoSystem.h"
#include "QuestSystem.h"
#include "ClientLoop.h"
#include "InventorySystem.h"
#include "PalInteractionSystem.h"
#include "SoundManager.h"

// component
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "CameraComponent.h"
#include "FSMComponent.h"
#include "ColliderComponent.h"
#include "SphereComponent.h"
#include "BoxComponent.h"
#include "PlayerControllerComponent.h"
#include "Panel.h"
#include "TextComponent.h"
#include "AnimationImageComponent.h"
#include "ProgressBarComponent.h"
#include "ImageComponent.h"
#include "Material.h"
#include "MeshBuffer.h"

// utility
#include "StringUtility.h"
#include "Model.h"

// node
#pragma region NODE INCLUDES
#include "FSMNode_Player_Idle.h"
#include "FSMNode_Player_Move.h"
#include "FSMNode_Player_Attack.h"
#include "FSMNode_Player_Roll.h"
#include "FSMNode_Player_Emote.h"
#include "FSMNode_Player_Interact.h"
#include "FSMNode_Player_Jump.h"
#include "FSMNode_Player_Climb.h"
#include "FSMNode_Player_Riding.h"
#pragma endregion

// level
#include "World.h"
#include "Level.h"

// gameobject
#include "Weapon.h"
#include "Monster.h"
#include "Nature.h"
#include "Tree.h"
#include "Rock.h"
#include "Projectile.h"
#include "ProjectilePool.h"
#include "Monster.h"
#include "SheepBall.h"
#include "Architecture.h"
#include "Factory.h"
#include "PartnerPalsphere.h"
#include "Mineral.h"
#include "Ingredient.h"
#include "Npc.h"
#include "WindDeer.h"
#include "JetDragon.h"
#include "Torch.h"
#include "InventoryCustomObject.h"

// system
#include "PalBoxSystem.h"

#pragma region EVENTS

std::shared_ptr<CPlayer> Client::CPlayer::Create(const std::shared_ptr<FPlayerDesc>& _spPlayerDesc)
{
	shared_ptr<CPlayer> spPlayer = make_shared<CPlayer>();
	ENSUREF(spPlayer->Initialize(_spPlayerDesc), L"Failed to initialize player.");
	return spPlayer;
}

HRESULT Client::CPlayer::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// creature desc
	if (FAILED(CCreature::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_wpInvenCustomObject = static_pointer_cast<FPlayerDesc>(_spGameObjectDesc)->m_spInvenCustomObject;

	// player desc
	m_wpEffectManager = CClientModule::GetInstance()->GetEffectManager();
	m_wpInputManager = CEngineModule::GetInstance()->GetInputManager();
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();
	m_wpPhysXManager = CEngineModule::GetInstance()->GetPhysXManager();
	m_wpBuildManager = CClientModule::GetInstance()->GetBuildManager();
	m_wpToDoSystem = CGameManager::GetInstance()->GetToDoSystem();
	m_wpInventorySystem = CGameManager::GetInstance()->GetInventorySystem();
	m_wpPalInteractionSystem = CGameManager::GetInstance()->GetPalInteractionSystem();
	m_wpUIManager = CUIManager::GetInstance();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	m_eCreatureType = ECreatureType::PLAYER;
	InitCameraComponent();
	InitMeshComponent();
	InitFSMComponent();
	InitColliderComponent();
	InitNotifyFunction();
	
	// 임시 UI 연동용 palboxpalInfo prefab입니다.
	{
		FPalBoxPalInfo tPalSpherePalInfo = {};
		tPalSpherePalInfo.m_iMonsterLevel = 1;
		tPalSpherePalInfo.m_fCurExp = 0.0f;
		tPalSpherePalInfo.m_fCurHungerGauge = 100.0f;
		tPalSpherePalInfo.m_fAttackPower = 10.0f;
		tPalSpherePalInfo.m_fDefensePower = 10.0f;
		tPalSpherePalInfo.m_wstrPartnerSkillName = L"파트너 스킬 없음";
		tPalSpherePalInfo.m_iFoodAmount = 5;
		tPalSpherePalInfo.m_eMonsterAIType = EMonsterAIType::PARTNER_PAL;
		tPalSpherePalInfo.m_iPartnerSkillLevel = 1;
		tPalSpherePalInfo.m_wstrActiveSkillNameA = L"스킬 없음";
		tPalSpherePalInfo.m_wstrActiveSkillNameB = L"스킬 없음";
		tPalSpherePalInfo.m_wstrActiveSkillNameC = L"스킬 없음";
		tPalSpherePalInfo.m_eActiveSkillElementA = EMonsterElementType::ENUM_END;
		tPalSpherePalInfo.m_eActiveSkillElementB = EMonsterElementType::ENUM_END;
		tPalSpherePalInfo.m_eActiveSkillElementC = EMonsterElementType::ENUM_END;
		tPalSpherePalInfo.m_iActiveSkillLevelA = 1;
		tPalSpherePalInfo.m_iActiveSkillLevelB = 1;
		tPalSpherePalInfo.m_iActiveSkillLevelC = 1;
		tPalSpherePalInfo.m_wstrPassiveSkillNameA = L"스킬 없음";
		tPalSpherePalInfo.m_wstrPassiveSkillNameB = L"스킬 없음";


		//tPalSpherePalInfo.m_wstrMonsterName = L"도로롱";
		//tPalSpherePalInfo.m_wstrTextureFileName = m_wpUIManager.lock()->FindPalIconTextureName(EMonsterType::SHEEP_BALL);
		//tPalSpherePalInfo.m_eMonsterType = EMonsterType::SHEEP_BALL;
		//tPalSpherePalInfo.m_eMonsterElementType = EMonsterElementType::GROUND;
		//m_arrPartnerPals[0].first = tPalSpherePalInfo;
		//// 파트너 팰 정보를 도로롱으로 세팅합니다.
		////m_tCurPalInfo = tPalSpherePalInfo;

		//tPalSpherePalInfo.m_wstrMonsterName = L"찌릿도치";
		//tPalSpherePalInfo.m_wstrTextureFileName = m_wpUIManager.lock()->FindPalIconTextureName(EMonsterType::HEDGE_HOG);
		//tPalSpherePalInfo.m_eMonsterType = EMonsterType::HEDGE_HOG;
		//tPalSpherePalInfo.m_eMonsterElementType = EMonsterElementType::ELECTRIC;
		//m_arrPartnerPals[1].first = tPalSpherePalInfo;

		//tPalSpherePalInfo.m_wstrMonsterName = L"꼬꼬닭";
		//tPalSpherePalInfo.m_wstrTextureFileName = m_wpUIManager.lock()->FindPalIconTextureName(EMonsterType::CHICKEN_PAL);
		//tPalSpherePalInfo.m_eMonsterType = EMonsterType::CHICKEN_PAL;
		//tPalSpherePalInfo.m_eMonsterElementType = EMonsterElementType::NEUTRAL;
		//m_arrPartnerPals[2].first = tPalSpherePalInfo;

		//tPalSpherePalInfo.m_wstrMonsterName = L"까부냥";
		//tPalSpherePalInfo.m_wstrTextureFileName = m_wpUIManager.lock()->FindPalIconTextureName(EMonsterType::PINK_CAT);
		//tPalSpherePalInfo.m_eMonsterType = EMonsterType::PINK_CAT;
		//tPalSpherePalInfo.m_eMonsterElementType = EMonsterElementType::NEUTRAL;
		//m_arrPartnerPals[3].first = tPalSpherePalInfo;

		/*tPalSpherePalInfo.m_wstrMonsterName = L"큐를리스";
		tPalSpherePalInfo.m_wstrTextureFileName = m_wpUIManager.lock()->FindPalIconTextureName(EMonsterType::SHEEP_BALL);
		tPalSpherePalInfo.m_eMonsterType = EMonsterType::CARBUNCLO;
		tPalSpherePalInfo.m_eMonsterElementType = EMonsterElementType::GRASS;
		m_arrPartnerPals[4].first = tPalSpherePalInfo;*/

		//m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent0->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(EMonsterType::CARBUNCLO));
		//m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent1->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(EMonsterType::SHEEP_BALL));
		//m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent2->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(EMonsterType::HEDGE_HOG));
	}

	return S_OK;
}

HRESULT Client::CPlayer::BeginPlay()
{
	if (FAILED(CCreature::BeginPlay())) { return E_FAIL; }

	InitPlayerControllerComponent();
	InitProjectilePool();
	InitUIPtr();
	InitRidingObject();

	// 임의로 체력 등을 설정합니다.
	{
		m_fMaxHp = 500.0f;
		m_fCurHp = m_fMaxHp;
		m_fPreHp = m_fCurHp;

		m_fAttackPower = 74.0f;
		m_fDefensePower = 50.0f;

		m_fMaxHungerGauge = 100.0f;
		m_fCurHungerGauge = m_fMaxHungerGauge;

		m_wpUIManager.lock()->SetHUDHPInfoPanel(m_fMaxHp, m_fCurHp);

		// shared_ptr<CTextComponent> spTextComponent = m_wpUIManager.lock()->GetHUDHPInfo().spMaxHPTextComponent;
		// spTextComponent->SetText(L"/ " + to_wstring(static_cast<int>(m_fMaxHp)));
		// 
		// m_wpUIManager.lock()->GetHUDHPInfo().spCurHPTextComponent->SetText(to_wstring(static_cast<int>(m_fCurHp)));
	}

	return S_OK;
}

int32 Client::CPlayer::PreTick(float _fDeltaSeconds)
{
	switch (CCharacter::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	KeyInput();

	ComputeCamera();

	ChangePalSphereUI();

	TestInput();

	if (m_fPreStaminaGauge > m_fCurStaminaGauge)
	{
		if (!m_tPlayerData.m_bAiming)
		{
			m_wpStaminaUI.lock()->SetActive(true);
		}

		if (m_wpUIManager.lock()->MinusStamina(m_wpStaminaProgress.lock(), m_fPreStaminaGauge, m_fCurStaminaGauge, m_fMaxStaminaGauge, &m_fStaminaLerpTime, _fDeltaSeconds))
		{
			m_fPreStaminaGauge = m_fCurStaminaGauge;
		}
	}
	else if (m_fPreStaminaGauge < m_fCurStaminaGauge)
	{
		if (m_wpUIManager.lock()->PlusStamina(m_wpStaminaProgress.lock(), m_fPreStaminaGauge, m_fCurStaminaGauge, m_fMaxStaminaGauge, &m_fStaminaLerpTime, _fDeltaSeconds))
		{
			m_fPreStaminaGauge = m_fCurStaminaGauge;
		}
	}

	return 0;
}

int32 Client::CPlayer::Tick(float _fDeltaSeconds)
{
	if (m_tPlayerData.m_bAiming || (m_spFSMComponent->GetState() == (int32)EPlayerState::ATTACK))
	{
		SetUpRotationWithCameraLook();
	}
	else if((m_spFSMComponent->GetState() != (int32)EPlayerState::INTERACT))
	{
		SetUpRotationFreeWithCamera();
	}

	AdjustPlayerRotation();

	ComputeSpineRotationWithCameraLook();

	UpdateHUDUI(_fDeltaSeconds);

	ComputeMouseScrollLock();

	ComputeShield(_fDeltaSeconds);

	ComputeHp(_fDeltaSeconds);

	ComputeHunger(_fDeltaSeconds);

	CCharacter::Tick(_fDeltaSeconds);

	if (m_wpInputManager.lock()->IsKeyDown(DIK_X))
	{
		std::cerr << "Position : ";
		std::cerr << to_string(GetPosition().value().x) + ",";
		std::cerr << to_string(GetPosition().value().y) + ",";
		std::cerr << to_string(GetPosition().value().z) + ",";
		std::cerr << "\n";
		std::cerr << "Look : ";
		std::cerr << to_string(GetForwardVector().value().x) + ",";
		std::cerr << to_string(GetForwardVector().value().y) + ",";
		std::cerr << to_string(GetForwardVector().value().z) + ",";
	}

	return 0;
}

int32 Client::CPlayer::PostTick(float _fDeltaSeconds)
{
	CCharacter::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CPlayer::EndPlay()
{
	if (FAILED(CCharacter::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CPlayer::Release()
{
	// component
	SharedNull(m_spHeadMeshComponent);
	SharedNull(m_spHairMeshComponent);
	SharedNull(m_spArrowComponent);
	SharedNull(m_spFSMComponent);
	SharedNull(m_spJumpBoxComponent);
	SharedNull(m_spPartnerPal);

	// riding
	for (auto& spObj : m_arrRidingObjects)
	{
		SharedNull(spObj);
	}

	// cloth
	for (auto& Pair : m_umapOutiftMeshs)
	{
		SharedNull(Pair.second);
	}
	m_umapOutiftMeshs.clear();

	// headEquip
	for (auto& Pair : m_umapHeadEquipMeshs)
	{
		SharedNull(Pair.second);
	}
	m_umapHeadEquipMeshs.clear();

	// weapon
	for (auto& Pair : m_umapWeapon)
	{
		SharedNull(Pair.second);
	}
	m_umapWeapon.clear();
		
	if (m_spArrowPool) 
	{
		m_spArrowPool->Release();
		m_spArrowPool = nullptr;
	}

	if (m_spPalSpherePool)
	{
		m_spPalSpherePool->Release();
		m_spPalSpherePool = nullptr;
	}

	if (m_spPartnerPalSpherePool)
	{
		m_spPartnerPalSpherePool->Release();
		m_spPartnerPalSpherePool = nullptr;
	}

	if (m_spRidingPalSpherePool)
	{
		m_spRidingPalSpherePool->Release();
		m_spRidingPalSpherePool = nullptr;
	}

	CCreature::Release();
}

void Client::CPlayer::SetActive(bool _bActive)
{
	CGameObject::SetActive(_bActive);

	if (_bActive)
	{
		for (auto& Pair : m_umapWeapon)
		{
			if (Pair.second)
				Pair.second->SetActive(false);
		}

		for (auto& Pair : m_umapHeadEquipMeshs)
		{
			if (Pair.second)
				Pair.second->SetActive(false);
		}

		for (auto& Pair : m_umapOutiftMeshs)
		{
			if (Pair.second)
				Pair.second->SetActive(false);
		}

		if (!m_wpWeapon.expired())
			m_wpWeapon.lock()->SetActive(true);

		ComputeCurrentCloth();
		ComputeCurrentHeadEquip();
	}
}

void Client::CPlayer::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 컨트롤러 컴포넌트
	if (_pSrcComponent->GetComponentType() == EComponentType::CONTROLLER) { return; }

	CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	// 플레이어가 뜁니다.
	if (m_spPlayerControllerComponent->IsOnAir()
		&& pSrcColliderComponent == m_spJumpBoxComponent.get()
		&& (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT ||
			pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT2 ||
			pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BUILDING))
	{
		m_spPlayerControllerComponent->SetOnAir(false);
		m_spPlayerControllerComponent->SetJumping(false);
	}

	// 플레이어가 나무, 돌을 캡니다.
	if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::PLAYER_WEAPON &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::NATURE)
	{
		shared_ptr<CNature> spNature = static_pointer_cast<CNature>(_pDstComponent->GetOwnerGameObject());
		spNature->TakeDamage(shared_from_this(), m_fAttackPower);
	}

	// 플레이어가 몬스터를 공격합니다.
	if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::PLAYER_WEAPON &&
		(pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::MONSTER || pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BOSS))
	{
		// 종합 공격력 = 본인 공격력 + 무기 공격력
		float fWeaponAttackPower = m_wpInventorySystem.lock()->GetEquippedWeaponItemSlots()[m_iCurrentWeaponSlotIndex].m_fAttackPower;
		float fTotalAttackPower = m_fAttackPower + fWeaponAttackPower;

		shared_ptr<CMonster> spDstMonster = static_pointer_cast<CMonster>(_pDstComponent->GetOwnerGameObject());
		spDstMonster->TakeDamage(shared_from_this(), fTotalAttackPower);

		Vector3 vHitPos = (GetPosition().value() + spDstMonster->GetPosition().value()) * 0.5f + Vector3(0.f, 0.5f, 0.f);

		// 타격 이펙트 출력
		m_wpEffectManager.lock()->ActivateEffect(L"Group_Hit2", vHitPos);

		// 타격 사운드 출력
		m_wpSoundManager.lock()->PlaySoundEx("Punch", 0.5f, false);
	}

	// 플레이어가 아이템을 줍습니다.
	if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::INTERACTION &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ITEM)
	{
		shared_ptr<CItem> spItem = static_pointer_cast<CItem>(_pDstComponent->GetOwnerGameObject());
		spItem->CollectItem();
	}

	// 건물과의 상호작용
	if (m_tPlayerData.m_eInteractType == EInteractType::ENUM_END &&
		pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::INTERACTION &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BUILDING &&
		m_tPlayerData.m_wpInteractArchitecture.expired())
	{
		if (!static_pointer_cast<CArchitecture>(pDstColliderComponent->GetOwnerGameObject())->IsHousing())
		{
			CheckInteractType(static_pointer_cast<CArchitecture>(pDstColliderComponent->GetOwnerGameObject()));
		}
	}	
	
	// NPC상호작용
	if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::INTERACTION &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::NPC)
	{
		m_tPlayerData.m_wpInteractNpc = static_pointer_cast<CNpc>(pDstColliderComponent->GetOwnerGameObject());
		m_tPlayerData.m_eInteractType = EInteractType::TALKING;
	}

	// 라이딩 상호작용
	if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::INTERACTION &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::RIDING &&
		m_tPlayerData.m_wpRidingObject.expired())
	{
		m_tPlayerData.m_wpRidingObject = static_pointer_cast<CRidingObject>(pDstColliderComponent->GetOwnerGameObject());
		m_tPlayerData.m_eInteractType = EInteractType::RIDING;
	}

	// 거점 팰 상호작용
	if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::INTERACTION &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ALLY &&
		m_tPlayerData.m_wpInteractAllyPal.expired())
	{
		m_tPlayerData.m_wpInteractAllyPal = static_pointer_cast<CMonster>(pDstColliderComponent->GetOwnerGameObject());
	}
}

void Client::CPlayer::OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 컨트롤러 컴포넌트
	if (_pSrcComponent->GetComponentType() == EComponentType::CONTROLLER) { return; }

	CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	// 콜라이더 컴포넌트
	if (pSrcColliderComponent == m_spJumpBoxComponent.get() &&
		(pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT ||
			pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT2 ||
			pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BUILDING))
	{
		PxRaycastHit tRaycastHit;
		Vector3 vPos = GetPosition().value();
		int32 iCollisionFlag = (int32)ECollisionLayer::BUILDING | (int32)ECollisionLayer::ENVIRONMENT | (int32)ECollisionLayer::ENVIRONMENT2;

		if (!m_wpPhysXManager.lock()->SingleRayCast(vPos, Vector3(0.f, -1.f, 0.f), 0.15f, iCollisionFlag, &tRaycastHit))
		{
			m_spPlayerControllerComponent->SetOnAir(true);
		}
	}

	if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::INTERACTION &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::BUILDING)
	{
		if (!static_pointer_cast<CArchitecture>(pDstColliderComponent->GetOwnerGameObject())->IsHousing())
		{
			ResetInteractingArchitectureData();
		}
	}

	if (!m_tPlayerData.m_wpInteractNpc.expired() &&
		pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::INTERACTION &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::NPC)
	{
		m_tPlayerData.m_wpInteractNpc.reset();
		m_tPlayerData.m_eInteractType = EInteractType::ENUM_END;
	}

	if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::INTERACTION &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::RIDING)
	{
		if ((EPlayerState)m_spFSMComponent->GetState() != EPlayerState::RIDING &&
			(EPlayerState)m_spFSMComponent->GetState() != EPlayerState::INTERACT)
		{
			m_tPlayerData.m_wpRidingObject.reset();
			m_tPlayerData.m_eInteractType = EInteractType::ENUM_END;
		}
	}
}

void Client::CPlayer::SetColorHair(const Vector3& _vColor)
{
	auto umapMeshInfos = m_spHairMeshComponent->GetModel()->GetMeshInfos();
	auto vecMeshInfos = umapMeshInfos[EShaderType::DEFAULT_ANIM];

	for (auto& spMeshBuffer : vecMeshInfos)
	{
		spMeshBuffer->m_spMeshBuffer->GetMaterial()->SetInt(L"Use Alternative Color", true);
		spMeshBuffer->m_spMeshBuffer->GetMaterial()->SetVector3(L"Alternative Color", _vColor);
	}
}

void Client::CPlayer::SetHeadMaterial(const FCustomObjectData& _tCustomData)
{
	auto umapMeshInfos = m_spHeadMeshComponent->GetModel()->GetMeshInfos();
	auto vecDefaultHeadInfo = umapMeshInfos[EShaderType::DEFAULT_ANIM];

	for (auto& spDefaultHeadBuffer : vecDefaultHeadInfo)
	{
		if (spDefaultHeadBuffer->m_spMeshBuffer->GetMaterial()->GetName().find(L"Brow") != wstring::npos ||
			spDefaultHeadBuffer->m_spMeshBuffer->GetMaterial()->GetName().find(L"Beard") != wstring::npos)
		{
			spDefaultHeadBuffer->m_spMeshBuffer->GetMaterial()->SetInt(L"Use Alternative Color", true);
			spDefaultHeadBuffer->m_spMeshBuffer->GetMaterial()->SetVector3(L"Alternative Color", _tCustomData.m_vColor);
		}
	}

	auto vecEyeInfo = umapMeshInfos[EShaderType::PLAYEREYE_ANIM];

	for (auto& spEyeInfo : vecEyeInfo)
	{
		spEyeInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Cornea Index", _tCustomData.m_iCorneaIndex);
		spEyeInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Highlight Index", _tCustomData.m_iHighlightIndex);
		spEyeInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Cornea Use Alternative Color", true);
		spEyeInfo->m_spMeshBuffer->GetMaterial()->SetVector3(L"Cornea Alternative Color", _tCustomData.m_vColor);
	}
}

void Client::CPlayer::SetColorHeadEquip(const Vector3& _vColor)
{
	for (auto& Pair : m_umapHeadEquipMeshs)
	{
		auto umapMeshInfos = Pair.second->GetModel()->GetMeshInfos();
		auto vecMeshInfos = umapMeshInfos[EShaderType::DEFAULT_ANIM];

		for (auto& spMeshInfo : vecMeshInfos)
		{
			if (spMeshInfo->m_spMeshBuffer->GetMaterial()->GetName().find(L"Hair") != wstring::npos)
			{
				spMeshInfo->m_spMeshBuffer->GetMaterial()->SetInt(L"Use Alternative Color", true);
				spMeshInfo->m_spMeshBuffer->GetMaterial()->SetVector3(L"Alternative Color", _vColor);
			}
		}
	}
}

void Client::CPlayer::TakeDamage(const std::shared_ptr<CGameObject>& _spInstigator, float _fDamage)
{
	if (m_bDead) { return; }

	m_wpSoundManager.lock()->PlaySoundEx("Hit_CMN_Creature_01_703359006", GetPosition().value());

	// 무적모드면 데미지 무시
	if (m_bInvincibility) { return; }

	if (IsArmorEquipped() && m_iShieldCount > 0)
	{
		m_iShieldCount--;
	}
	else
	{
		// 장비 방어력
		float fHeadDefense = m_wpInventorySystem.lock()->GetEquippedHeadItemSlot().m_fDefensePower;
		float fBodyDefense = m_wpInventorySystem.lock()->GetEquippedBodyItemSlot().m_fDefensePower;
		float fShieldDefense = m_wpInventorySystem.lock()->GetEquippedShieldItemSlot().m_fDefensePower;

		// 총 방어력
		float fTotalDefense = m_fDefensePower + fHeadDefense + fBodyDefense + fShieldDefense;

		float fBaseDamage = _fDamage - fTotalDefense;
		if (fBaseDamage < 0.f) { fBaseDamage = 0.f; }

		float fTotalDamage = fBaseDamage + CMathUtility::GetRandomFloat(0.f, 3.f);

		m_fCurHp -= fTotalDamage;

		if (m_fCurHp < 0.0f)
		{
			m_fCurHp = 0.0f;
			m_bDead = true;
		}
	}
}

#pragma endregion

#pragma region INITIALIZE

bool Client::CPlayer::IsHandTorchHeld()
{
	return m_umapWeapon[EItemType::HAND_HELD_TORCH]->IsActive();
}

bool Client::CPlayer::CanAttack()
{
	switch (m_tPlayerData.m_eWeaponActionType)
	{
	case EWeaponActionType::BOW:
	{
		int32 iArrowCount = m_wpInventorySystem.lock()->FindInventoryItemTotalNum(EItemType::ARROW);
		if (iArrowCount <= 0) { return false; }
	}
		break;

	case EWeaponActionType::ASSAULT_RIFLE:
	{
		int32 iBulletCount = m_wpInventorySystem.lock()->FindInventoryItemTotalNum(EItemType::RIFLE_BULLET);
		if (iBulletCount <= 0) { return false; }
	}
		break;

	default:
		break;
	}

	return true;
}

bool Client::CPlayer::CanThrowPalSphere()
{
	return m_wpInventorySystem.lock()->FindInventoryItemTotalNum(EItemType::BLUE_SPHERE) > 0;
}

void Client::CPlayer::InitMeshComponent()
{
	FCustomObjectData tCustomData = CClientLoop::s_spClientDataSheet->k_tCustomData;

	// 몸 메시를 생성합니다.
	{
		// component desc
		FMeshComponentDesc tBodyMeshComponentDesc;
		tBodyMeshComponentDesc.m_wstrComponentName = L"BodyMeshComponent";
		tBodyMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tBodyMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tBodyMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

		// mesh component desc
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/Player_Female.dat";
		tBodyMeshComponentDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
		tBodyMeshComponentDesc.m_wstrUpperBoneName = L"spine_01";
		tBodyMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(180.f));
		tBodyMeshComponentDesc.m_wstrAnimationGroupJsonPath = L"../../Resource/Models/AnimGroupData/Player_Animation_Group.json";

		m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tBodyMeshComponentDesc));
		AddComponent(tBodyMeshComponentDesc.m_wstrComponentName, m_spMeshComponent);

		m_spMeshComponent->ChangeUpperAnimation((int32)EPLAYER_FEMALE::AS_Player_Female_Idle_None, false);
		m_spMeshComponent->ChangeDownAnimation((int32)EPLAYER_FEMALE::AS_Player_Female_Walk_None, false);
		m_spMeshComponent->SetCulled(true);
	}

	// 머리 메시를 생성하고 레퍼런스 본을 연결합니다.
	{
		// component desc
		FMeshComponentDesc tHeadMeshComponentDesc;
		tHeadMeshComponentDesc.m_wstrComponentName = L"HeadMeshComponent";
		tHeadMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tHeadMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tHeadMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

		// mesh component desc
		//tHeadMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/Head/Player_Female_Head001.dat";
		tHeadMeshComponentDesc.m_wstrModelFilePath = tCustomData.m_wstrHeadMeshPath;
		tHeadMeshComponentDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
		tHeadMeshComponentDesc.m_spParentMeshComponent = m_spMeshComponent;
		tHeadMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(180.f));

		m_spHeadMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tHeadMeshComponentDesc));
		AddComponent(tHeadMeshComponentDesc.m_wstrComponentName, m_spHeadMeshComponent);

		SetHeadMaterial(tCustomData);
	}

	// 머리카락 메시를 생성하고 레퍼런스 본을 연결합니다.
	{
		// component desc
		FMeshComponentDesc tHairMeshComponentDesc;
		tHairMeshComponentDesc.m_wstrComponentName = L"HairMeshComponent";
		tHairMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tHairMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tHairMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

		// mesh component desc
		//tHairMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/Hair/Player_Hair001.dat";
		tHairMeshComponentDesc.m_wstrModelFilePath = tCustomData.m_wstrHairMeshPath;
		tHairMeshComponentDesc.m_spParentMeshComponent = m_spMeshComponent;
		tHairMeshComponentDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
		tHairMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(180.f));

		m_spHairMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tHairMeshComponentDesc));
		AddComponent(tHairMeshComponentDesc.m_wstrComponentName, m_spHairMeshComponent);

		SetColorHair(tCustomData.m_vColor);
	}

	// 옷 메시를 생성하고 레퍼런스 본을 연결합니다.
	{
		LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_OldCloth001.dat", EItemType::Outfit_OldCloth001, m_umapOutiftMeshs);
		LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_Cloth001.dat", EItemType::Outfit_Cloth001, m_umapOutiftMeshs);
		LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_Hunter001.dat", EItemType::Outfit_Hunter001, m_umapOutiftMeshs);
		LoadMeshComponent(L"../../Resource/Models/Dat/Anim/Outfit/Player_Female_Outfit_Platinum001_v02.dat", EItemType::Outfit_Platinum001_v02, m_umapOutiftMeshs);


		if (tCustomData.m_eInitialOutfit == EItemType::NONE) { ChangeOutfit(EItemType::Outfit_OldCloth001); }
		else
		{
			ChangeOutfit(tCustomData.m_eInitialOutfit);

			FInventoryItemInfo tBodyItemInfo = {};
			tBodyItemInfo.m_eItemType = tCustomData.m_eInitialOutfit;
			tBodyItemInfo.m_eItemKindType = EItemKindType::ARMOR;
			tBodyItemInfo.m_iNumItem = 1;
			tBodyItemInfo.m_eArmorType = (EArmorType)tCustomData.m_eInitialOutfit;
			tBodyItemInfo.m_wstrTextureFileName = m_wpUIManager.lock()->FindItemUITextureName(tBodyItemInfo.m_eItemType);
			tBodyItemInfo.m_fDefensePower = CClientLoop::s_spClientDataSheet->k_mapArmorDefaultDefensePowers.at(tBodyItemInfo.m_eItemType);
			tBodyItemInfo.m_fDurability = 5;
			tBodyItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(tBodyItemInfo.m_eItemType);

			m_wpInventorySystem.lock()->SetEquippedBodyItemSlot(tBodyItemInfo);
		}
	}

	// 모자 메시를 생성하고 레퍼런스 본을 연결합니다.
	{
		LoadMeshComponent(L"../../Resource/Models/Dat/Anim/HeadEquip/HeadEquip019_v01.dat", EItemType::HeadEquip019, m_umapHeadEquipMeshs);
		LoadMeshComponent(L"../../Resource/Models/Dat/Anim/HeadEquip/HeadEquip021.dat", EItemType::HeadEquip021, m_umapHeadEquipMeshs);

		LoadMeshComponent(L"../../Resource/Models/Dat/NonAnim/HeadEquip016.dat", EItemType::HeadEquip016, m_umapHeadEquipMeshs, L"Socket_HairAttach_HeadEquip_front");
		LoadMeshComponent(L"../../Resource/Models/Dat/NonAnim/HeadEquip018.dat", EItemType::HeadEquip018, m_umapHeadEquipMeshs, L"Socket_HairAttach_HeadEquip_front");

		SetColorHeadEquip(tCustomData.m_vColor);

		if (tCustomData.m_eInitialHeadEquip != EItemType::NONE)
		{
			ChangeHeadEquip(tCustomData.m_eInitialHeadEquip);

			FInventoryItemInfo tHeadItemInfo = {};
			tHeadItemInfo.m_eItemType = tCustomData.m_eInitialHeadEquip;
			tHeadItemInfo.m_eItemKindType = EItemKindType::ARMOR;
			tHeadItemInfo.m_iNumItem = 1;
			tHeadItemInfo.m_eArmorType = (EArmorType)tCustomData.m_eInitialHeadEquip;
			tHeadItemInfo.m_wstrTextureFileName = m_wpUIManager.lock()->FindItemUITextureName(tHeadItemInfo.m_eItemType);
			tHeadItemInfo.m_fDefensePower = CClientLoop::s_spClientDataSheet->k_mapArmorDefaultDefensePowers.at(tHeadItemInfo.m_eItemType);
			tHeadItemInfo.m_fDurability = 5;
			tHeadItemInfo.m_fWeight = CClientLoop::s_spClientDataSheet->k_mapItemDefaultWeight.at(tHeadItemInfo.m_eItemType);

			m_wpInventorySystem.lock()->SetEquippedHeadItemSlot(tHeadItemInfo);
		}
	}

	// 창
	{
		// component desc
		FWeaponDesc tSpearDesc;
		tSpearDesc.m_wstrGameObjectName = L"PlayerWeapon_Spear";
		tSpearDesc.m_bIsAnimed = false;
		tSpearDesc.m_eWeaponActionType = EWeaponActionType::SPEAR;
		tSpearDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tSpearDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tSpearDesc.m_eWeaponType = EWeaponType::STONE_SPEAR;
		tSpearDesc.m_eCurItemType = EItemType::STONE_SPEAR;

		// mesh component desc
		tSpearDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/Weapon_Melee_005.dat";
		tSpearDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tSpearDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 0.f));
		tSpearDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		shared_ptr<CWeapon> spSpear = CWeapon::Create(make_shared<FWeaponDesc>(tSpearDesc));
		m_umapWeapon.emplace(EItemType::STONE_SPEAR, spSpear);
		AddChildGameObject(spSpear);
		spSpear->SetActive(false);
	}

	// 로켓 런처
	{
		// component desc
		FWeaponDesc tRocketLauncherDesc;
		tRocketLauncherDesc.m_wstrGameObjectName = L"PlayerWeapon_RocketLauncher";
		tRocketLauncherDesc.m_bIsAnimed = false;
		tRocketLauncherDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tRocketLauncherDesc.m_eWeaponActionType = EWeaponActionType::ROCKET_LAUNCHER;
		tRocketLauncherDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tRocketLauncherDesc.m_eWeaponType = EWeaponType::ROCKET_LAUNCHER;
		tRocketLauncherDesc.m_eCurItemType = EItemType::ROCKET_LAUNCHER;

		// mesh component desc
		tRocketLauncherDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/Weapon_RocketLauncher_001.dat";
		tRocketLauncherDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tRocketLauncherDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 0.f));
		tRocketLauncherDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		shared_ptr<CWeapon> spRocketLauncher = CWeapon::Create(make_shared<FWeaponDesc>(tRocketLauncherDesc));
		m_umapWeapon.emplace(EItemType::ROCKET_LAUNCHER, spRocketLauncher);
		AddChildGameObject(spRocketLauncher);
		spRocketLauncher->SetActive(false);
	}

	// 화살
	{
		// component desc
		FMeshComponentDesc tArrowDesc;
		tArrowDesc.m_wstrComponentName = L"PlayerWeapon_Arrow";
		tArrowDesc.m_eComponentType = EComponentType::MESH;
		tArrowDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tArrowDesc.m_wpOwnerGameObject = shared_from_this();

		// mesh component desc
		tArrowDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/Weapon_Arrow_001.dat";
		tArrowDesc.m_wstrShaderName = L"FX_NonAnimMesh_PNT2T";
		tArrowDesc.m_matPivot = Matrix::CreateScale(0.01f);// *Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tArrowDesc.m_spPivotBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		m_spArrowComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tArrowDesc));
		AddComponent(tArrowDesc.m_wstrComponentName, m_spArrowComponent);
		m_spArrowComponent->SetActive(false);
	}

	// 활
	{
		// component desc
		FWeaponDesc tBowDesc;
		tBowDesc.m_wstrGameObjectName = L"PlayerWeapon_Bow";
		tBowDesc.m_bIsAnimed = true;
		tBowDesc.m_eWeaponActionType = EWeaponActionType::BOW;
		tBowDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tBowDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tBowDesc.m_eWeaponType = EWeaponType::OLD_BOW;
		tBowDesc.m_eCurItemType = EItemType::OLD_BOW;

		// mesh component desc
		tBowDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/Weapon_Bow_003.dat";
		tBowDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tBowDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 0.f));
		tBowDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_L");

		shared_ptr<CWeapon> spBow = CWeapon::Create(make_shared<FWeaponDesc>(tBowDesc));
		m_umapWeapon.emplace(EItemType::OLD_BOW, spBow);
		AddChildGameObject(spBow);
		spBow->SetUseAnimation(false, true);
		spBow->SetActive(false);
	}

	// 총
	{
		// component desc
		FWeaponDesc tAssaultRifleDesc;
		tAssaultRifleDesc.m_wstrGameObjectName = L"PlayerWeapon_AssaultRifle";
		tAssaultRifleDesc.m_bIsAnimed = true;
		tAssaultRifleDesc.m_eWeaponActionType = EWeaponActionType::ASSAULT_RIFLE;
		tAssaultRifleDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tAssaultRifleDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tAssaultRifleDesc.m_eWeaponType = EWeaponType::ASSAULT_RIFLE;
		tAssaultRifleDesc.m_eCurItemType = EItemType::ASSAULT_RIFLE;

		// mesh component desc
		tAssaultRifleDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/Weapon_AssaultRifle_001.dat";
		tAssaultRifleDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tAssaultRifleDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 0.f));
		tAssaultRifleDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		shared_ptr<CWeapon> spAssaultRifle = CWeapon::Create(make_shared<FWeaponDesc>(tAssaultRifleDesc));
		m_umapWeapon.emplace(EItemType::ASSAULT_RIFLE, spAssaultRifle);
		AddChildGameObject(spAssaultRifle);
		spAssaultRifle->ChangeAnimation((int32)EASSAULTRIFLE::AS_AssaultRifle_Empty);
		spAssaultRifle->SetActive(false);
	}

	// 글라이더
	{
		// component desc
		FWeaponDesc tGliderDesc;
		tGliderDesc.m_wstrGameObjectName = L"PlayerWeapon_Glider";
		tGliderDesc.m_bIsAnimed = false;
		tGliderDesc.m_eWeaponActionType = EWeaponActionType::NONE;
		tGliderDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tGliderDesc.m_eRenderType = ERenderType::RENDER_NBLEND_NONCULL;
		tGliderDesc.m_eWeaponType = EWeaponType::NONE;
		tGliderDesc.m_eCurItemType = EItemType::GLIDER;

		// mesh component desc
		tGliderDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/Weapon_Glider_001.dat";
		tGliderDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tGliderDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 0.f));
		tGliderDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_L");

		shared_ptr<CWeapon> spGlider = CWeapon::Create(make_shared<FWeaponDesc>(tGliderDesc));
		m_umapWeapon.emplace(EItemType::GLIDER, spGlider);
		AddChildGameObject(spGlider);
		spGlider->SetActive(false);
	}

	// 보여주기 용 팰스피어
	{
		// component desc
		FWeaponDesc tPalSphereDesc;
		tPalSphereDesc.m_wstrGameObjectName = L"PlayerWeapon_PalSphere";
		tPalSphereDesc.m_bIsAnimed = true;
		tPalSphereDesc.m_eWeaponActionType = EWeaponActionType::PROJECTILE;
		tPalSphereDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tPalSphereDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tPalSphereDesc.m_eWeaponType = EWeaponType::NONE;
		tPalSphereDesc.m_eCurItemType = EItemType::BLUE_SPHERE;

		// mesh component desc
		tPalSphereDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/Weapon_PalSphere_001.dat";
		tPalSphereDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tPalSphereDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 0.f));
		tPalSphereDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		shared_ptr<CWeapon> spPalSphere = CWeapon::Create(make_shared<FWeaponDesc>(tPalSphereDesc));
		m_umapWeapon.emplace(EItemType::BLUE_SPHERE, spPalSphere);
		AddChildGameObject(spPalSphere);
		spPalSphere->SetUseAnimation(false);
		spPalSphere->SetActive(false);
	}

	// 곡괭이
	{
		// component desc
		FWeaponDesc tMeleeDesc;
		tMeleeDesc.m_wstrGameObjectName = L"PlayerWeapon_PickAxe";
		tMeleeDesc.m_bIsAnimed = false;
		tMeleeDesc.m_eWeaponActionType = EWeaponActionType::MELEE;
		tMeleeDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tMeleeDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeleeDesc.m_eWeaponType = EWeaponType::STONE_PICKAXE;
		tMeleeDesc.m_eCurItemType = EItemType::STONE_PICKAXE;

		// mesh component desc
		tMeleeDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/HarvestTool_001.dat";
		tMeleeDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tMeleeDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 0.f));
		tMeleeDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		shared_ptr<CWeapon> spMeleeWeapon = CWeapon::Create(make_shared<FWeaponDesc>(tMeleeDesc));
		m_umapWeapon.emplace(EItemType::STONE_PICKAXE, spMeleeWeapon);
		AddChildGameObject(spMeleeWeapon);
		spMeleeWeapon->SetActive(false);
	}

	// 도끼
	{
		// component desc
		FWeaponDesc tMeleeDesc;
		tMeleeDesc.m_wstrGameObjectName = L"PlayerWeapon_Axe";
		tMeleeDesc.m_bIsAnimed = false;
		tMeleeDesc.m_eWeaponActionType = EWeaponActionType::MELEE;
		tMeleeDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tMeleeDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeleeDesc.m_eWeaponType = EWeaponType::STONE_AXE;
		tMeleeDesc.m_eCurItemType = EItemType::STONE_AXE;

		// mesh component desc
		tMeleeDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/HarvestTool_002.dat";
		tMeleeDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tMeleeDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		shared_ptr<CWeapon> spMeleeWeapon = CWeapon::Create(make_shared<FWeaponDesc>(tMeleeDesc));
		m_umapWeapon.emplace(EItemType::STONE_AXE, spMeleeWeapon);
		AddChildGameObject(spMeleeWeapon);
		spMeleeWeapon->SetActive(false);
	}

	// 제작 망치
	{
		// component desc
		FWeaponDesc tMeleeDesc;
		tMeleeDesc.m_wstrGameObjectName = L"PlayerWeapon_ConstructionHammer";
		tMeleeDesc.m_bIsAnimed = false;
		tMeleeDesc.m_eWeaponActionType = EWeaponActionType::MELEE;
		tMeleeDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tMeleeDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeleeDesc.m_eWeaponType = EWeaponType::ENUM_END;
		tMeleeDesc.m_eCurItemType = EItemType::CONSTRUCT_HAMMER;

		// mesh component desc
		tMeleeDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/Weapon_ConstructionTool_001.dat";
		tMeleeDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tMeleeDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");
		//tMeleeDesc.m_eMeshManageType = EMeshManageType::INSTANCE;

		shared_ptr<CWeapon> spMeleeWeapon = CWeapon::Create(make_shared<FWeaponDesc>(tMeleeDesc));
		m_umapWeapon.emplace(EItemType::CONSTRUCT_HAMMER, spMeleeWeapon);
		AddChildGameObject(spMeleeWeapon);
		spMeleeWeapon->SetActive(false);
	}

	// 후라이팬
	{
		// component desc
		FWeaponDesc tMeleeDesc;
		tMeleeDesc.m_wstrGameObjectName = L"PlayerWeapon_FryingPan";
		tMeleeDesc.m_bIsAnimed = false;
		tMeleeDesc.m_eWeaponActionType = EWeaponActionType::MELEE;
		tMeleeDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tMeleeDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeleeDesc.m_eWeaponType = EWeaponType::ENUM_END;
		tMeleeDesc.m_eCurItemType = EItemType::FRYING_PAN;

		// mesh component desc
		tMeleeDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/Weapon_ConstructionTool_002.dat";
		tMeleeDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tMeleeDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		shared_ptr<CWeapon> spMeleeWeapon = CWeapon::Create(make_shared<FWeaponDesc>(tMeleeDesc));
		m_umapWeapon.emplace(EItemType::FRYING_PAN, spMeleeWeapon);
		AddChildGameObject(spMeleeWeapon);
		spMeleeWeapon->SetActive(false);
	}

	// 횃불
	{
		// component desc
		FWeaponDesc tMeleeDesc;
		tMeleeDesc.m_wstrGameObjectName = L"PlayerWeapon_TorchLight";
		tMeleeDesc.m_bIsAnimed = false;
		tMeleeDesc.m_eWeaponActionType = EWeaponActionType::MELEE;
		tMeleeDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tMeleeDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeleeDesc.m_eWeaponType = EWeaponType::HAND_HELD_TORCH;
		tMeleeDesc.m_eCurItemType = EItemType::HAND_HELD_TORCH;

		// mesh component desc
		tMeleeDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/Weapon_Melee_004.dat";
		tMeleeDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tMeleeDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 0.f));
		tMeleeDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		shared_ptr<CTorch> spMeleeWeapon = CTorch::Create(make_shared<FWeaponDesc>(tMeleeDesc));
		m_umapWeapon.emplace(EItemType::HAND_HELD_TORCH, spMeleeWeapon);
		AddChildGameObject(spMeleeWeapon);
		spMeleeWeapon->SetActive(false);
	}

	// 도축칼
	{
		// component desc
		FWeaponDesc tMeleeDesc;
		tMeleeDesc.m_wstrGameObjectName = L"PlayerWeapon_MeatCleaver";
		tMeleeDesc.m_bIsAnimed = false;
		tMeleeDesc.m_eWeaponActionType = EWeaponActionType::MELEE;
		tMeleeDesc.m_eCurItemKindType = EItemKindType::WEAPON;
		tMeleeDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeleeDesc.m_eWeaponType = EWeaponType::HAND_HELD_TORCH;
		tMeleeDesc.m_eCurItemType = EItemType::HAND_HELD_TORCH;

		// mesh component desc
		tMeleeDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/ChopperKnife.dat";
		tMeleeDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tMeleeDesc.m_matPivot.Translation(Vector3(0.f, 0.f, 0.f));
		tMeleeDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		shared_ptr<CWeapon> spMeleeWeapon = CWeapon::Create(make_shared<FWeaponDesc>(tMeleeDesc));
		m_umapWeapon.emplace(EItemType::MEAT_CLEAVER, spMeleeWeapon);
		AddChildGameObject(spMeleeWeapon);
		spMeleeWeapon->SetActive(false);
	}
}

void Client::CPlayer::InitCameraComponent()
{
	// component desc
	FCameraComponentDesc tCameraComponentDesc;
	tCameraComponentDesc.m_wstrComponentName = L"PlayerCameraComponent";
	tCameraComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tCameraComponentDesc.m_eRenderType = ERenderType::NONE;
	tCameraComponentDesc.m_eComponentType = EComponentType::CAMERA;

	// camera component desc
	tCameraComponentDesc.m_bKeyboardControl = false;
	tCameraComponentDesc.m_eCameraType = ECameraType::SPHERICAL;
	tCameraComponentDesc.m_fMinElevationDegree = -45.0f;
	tCameraComponentDesc.m_fMaxElevationDegree = 45.0f;
	tCameraComponentDesc.m_vSphericalCameraAxisOffset = m_vStandCameraOffset;
	tCameraComponentDesc.m_vSphericalCameraEyeOffset = Vector3(0.5f, 0.0f, 0.0f);
	tCameraComponentDesc.m_fCurSphericalCameraRadius = 3.0f;
	tCameraComponentDesc.m_fMouseSensitivity = 2.0f;

	tCameraComponentDesc.m_fCullFrustumNear = 0.1f;
	tCameraComponentDesc.m_fCullFrustumFar = 120.0f;
	tCameraComponentDesc.m_fCullFrustumSlops = Vector4(1.5f, -1.5f, -1.5f, 1.5f);

	tCameraComponentDesc.m_fCullSphereRadius = 50.0f;

	m_spCameraComponent = CCameraComponent::Create(make_shared<FCameraComponentDesc>(tCameraComponentDesc));
	AddComponent(tCameraComponentDesc.m_wstrComponentName, m_spCameraComponent);

	m_spCameraComponent->SetOnAir();
}

void Client::CPlayer::InitRidingObject()
{
	// riding wind deer
	{
		FWindDeerDesc tWindDeerDesc;
		tWindDeerDesc.m_vInitPosition = Vector3(-3530.0f, 74.0f, -2714.0f);
		tWindDeerDesc.m_fJumpPower = 500.f;
		tWindDeerDesc.m_fMoveSpeed = 8.f;
		tWindDeerDesc.m_vRidingOffset = Vector3(0.f, 0.1f , -0.3f);

		m_arrRidingObjects[(size_t)ERidingObjectType::WIND_DEER] = CWindDeer::Create(make_shared<FWindDeerDesc>(tWindDeerDesc));
		m_wpOwnerLevel.lock()->AddGameObject(m_arrRidingObjects[(size_t)ERidingObjectType::WIND_DEER]);
		m_arrRidingObjects[(size_t)ERidingObjectType::WIND_DEER]->SetActive(false);
	}
	
	// riding jet dragon
	{
		FJetDragonDesc tJetDragonDesc;
		tJetDragonDesc.m_vInitPosition = Vector3(-3530.0f, 74.0f, -2718.0f);
		tJetDragonDesc.m_fMoveSpeed = 10.f;
		tJetDragonDesc.m_vRidingOffset = Vector3(0.f, 0.1f , -0.1f);

		m_arrRidingObjects[(size_t)ERidingObjectType::JET_DRAGON] = CJetDragon::Create(make_shared<FJetDragonDesc>(tJetDragonDesc));
		m_wpOwnerLevel.lock()->AddGameObject(m_arrRidingObjects[(size_t)ERidingObjectType::JET_DRAGON]);
		m_arrRidingObjects[(size_t)ERidingObjectType::JET_DRAGON]->SetActive(false);
	}	
}

void Client::CPlayer::InitFSMComponent()
{
	// component desc
	FFSMComponentDesc tFSMComponentDesc;
	tFSMComponentDesc.m_wstrComponentName = L"FSMComponent";
	tFSMComponentDesc.m_eRenderType = ERenderType::NONE;
	tFSMComponentDesc.m_eComponentType = EComponentType::FSM;
	tFSMComponentDesc.m_wpOwnerGameObject = shared_from_this();

	// FSM component desc
	tFSMComponentDesc.m_iNumState = (int32)EPlayerState::ENUM_END;

	m_spFSMComponent = CFSMComponent::Create(make_shared<FFSMComponentDesc>(tFSMComponentDesc));
	AddComponent(tFSMComponentDesc.m_wstrComponentName, m_spFSMComponent);

	// FSM Component Node 채워넣기
	m_spFSMComponent->AddState((int32)EPlayerState::IDLE, CFSMNode_Player_Idle::Create(m_spMeshComponent, static_pointer_cast<CPlayer>(shared_from_this())));
	m_spFSMComponent->AddState((int32)EPlayerState::MOVE, CFSMNode_Player_Move::Create(m_spMeshComponent, static_pointer_cast<CPlayer>(shared_from_this())));
	m_spFSMComponent->AddState((int32)EPlayerState::ATTACK, CFSMNode_Player_Attack::Create(m_spMeshComponent, static_pointer_cast<CPlayer>(shared_from_this())));
	m_spFSMComponent->AddState((int32)EPlayerState::ROLL, CFSMNode_Player_Roll::Create(m_spMeshComponent, static_pointer_cast<CPlayer>(shared_from_this())));
	m_spFSMComponent->AddState((int32)EPlayerState::EMOTE, CFSMNode_Player_Emote::Create(m_spMeshComponent, static_pointer_cast<CPlayer>(shared_from_this())));
	m_spFSMComponent->AddState((int32)EPlayerState::INTERACT, CFSMNode_Player_Interact::Create(m_spMeshComponent, static_pointer_cast<CPlayer>(shared_from_this())));
	m_spFSMComponent->AddState((int32)EPlayerState::JUMP, CFSMNode_Player_Jump::Create(m_spMeshComponent, static_pointer_cast<CPlayer>(shared_from_this())));
	m_spFSMComponent->AddState((int32)EPlayerState::CLIMB, CFSMNode_Player_Climb::Create(m_spMeshComponent, static_pointer_cast<CPlayer>(shared_from_this())));
	m_spFSMComponent->AddState((int32)EPlayerState::RIDING, CFSMNode_Player_Riding::Create(m_spMeshComponent, static_pointer_cast<CPlayer>(shared_from_this())));
}

void Client::CPlayer::InitPlayerControllerComponent()
{
	// component desc
	FPlayerControllerComponentDesc tPlayerControllerComponentDesc;
	tPlayerControllerComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tPlayerControllerComponentDesc.m_eComponentType = EComponentType::CONTROLLER;
	tPlayerControllerComponentDesc.m_wstrComponentName = L"ControllerComponent";
	tPlayerControllerComponentDesc.m_eRenderType = ERenderType::NONE;
	tPlayerControllerComponentDesc.m_fStepOffset = 0.2f;

	// controller desc
	tPlayerControllerComponentDesc.m_eControllerShapeType = EControllerShapeType::CAPSULE;
	tPlayerControllerComponentDesc.m_vMaterials = Vector3(0.5f, 0.5f, 0.5f);
	tPlayerControllerComponentDesc.m_eControllerLayer = ECollisionLayer::PLAYER;
	tPlayerControllerComponentDesc.m_eControllerFlag = ECollisionFlag::PLAYER;

	tPlayerControllerComponentDesc.m_fCapsuleHalfHeight = 0.65f;
	tPlayerControllerComponentDesc.m_fCapsuleRadius = 0.2f;
	//tPlayerControllerComponentDesc.m_vLocalPosition = Vector3(0.0f, 0.75f, 0.0f);

	// player controller desc
	tPlayerControllerComponentDesc.m_spCameraComponent = m_spCameraComponent;

	m_spPlayerControllerComponent = CPlayerControllerComponent::Create(make_shared<FPlayerControllerComponentDesc>(tPlayerControllerComponentDesc));
	AddComponent(tPlayerControllerComponentDesc.m_wstrComponentName, m_spPlayerControllerComponent);
}

void Client::CPlayer::InitColliderComponent()
{
	// jump trigger component
	{
		// component
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_wstrComponentName = L"JumpBoxComponent";

		// collider component desc
		tBoxComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::PLAYER;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::PLAYER;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = true;

		// box component desc
		tBoxComponentDesc.m_vExtents = Vector3(0.2f, 0.1f, 0.2f);

		m_spJumpBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spJumpBoxComponent);
	}

	// interaction trigger component
	{
		// component
		FSphereComponentDesc tSphereComponentDesc;
		tSphereComponentDesc.m_eComponentType = EComponentType::SPHERE;
		tSphereComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
		tSphereComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tSphereComponentDesc.m_wstrComponentName = L"Interaction SphereComponent";

		// collider component desc
		tSphereComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		tSphereComponentDesc.m_eColliderType = EColliderType::SPHERE;
		tSphereComponentDesc.m_eCollisionFlag = ECollisionFlag::INTERACTION;
		tSphereComponentDesc.m_eCollisionLayer = ECollisionLayer::INTERACTION;
		tSphereComponentDesc.m_bGravity = false;
		tSphereComponentDesc.m_bTrigger = true;

		// sphere component desc
		// tSphereComponentDesc.m_fDensity = 2.5f;// 밀도를 왜 설정했을까?
		tSphereComponentDesc.m_fRadius = CClientLoop::s_spClientDataSheet->k_fPlayerInteractRadius;
		tSphereComponentDesc.m_vLocalPosition = Vector3(0.0f, 1.0f, 0.0f);

		m_spInteractionSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spInteractionSphereComponent);
	}
}

void Client::CPlayer::InitProjectilePool()
{
	shared_ptr<CLevel> spPersistentLevel = CWorld::GetInstance()->GetPersistentLevel();

	// 팰스피어 풀
	{
		FProjectileDesc tProjDesc;
		tProjDesc.m_bIsAnimed			= true;
		tProjDesc.m_bActive				= true;
		tProjDesc.m_eWeaponActionType	= EWeaponActionType::PROJECTILE;
		tProjDesc.m_eRenderType			= ERenderType::RENDER_NBLEND_NONCULL; 
		tProjDesc.m_fColliderRadius		= 1.f;
		tProjDesc.m_matPivot			= Matrix::CreateScale(0.0005f);
		tProjDesc.m_spOwnPlayer			= static_pointer_cast<CPlayer>(shared_from_this());
		tProjDesc.m_wstrGameObjectName	= L"Thorw";
		tProjDesc.m_wstrEffectName		= L"Group_PalSphere_Tracing";
		tProjDesc.m_wstrModelFilePath	= L"../../Resource/Models/Dat/Anim/Weapon_PalSphere_001.dat";
		tProjDesc.m_bDeferredAdd		= false;

		m_spPalSpherePool = CProjectilePool::Create(spPersistentLevel, EProjectileType::PALSPHERE, make_shared<FProjectileDesc>(tProjDesc), 5);
	}

	// 파트너 팰스피어 풀
	{
		FProjectileDesc tProjDesc;

		tProjDesc.m_bIsAnimed			= true;
		tProjDesc.m_bActive				= true;
		tProjDesc.m_eWeaponActionType	= EWeaponActionType::PROJECTILE;
		tProjDesc.m_eRenderType			= ERenderType::RENDER_NBLEND_NONCULL; 
		tProjDesc.m_fColliderRadius		= 0.2f;
		tProjDesc.m_matPivot			= Matrix::CreateScale(0.0005f);
		tProjDesc.m_spOwnPlayer			= static_pointer_cast<CPlayer>(shared_from_this());
		tProjDesc.m_wstrGameObjectName	= L"SummonPal";
		tProjDesc.m_wstrEffectName		= L"Group_PalSphere_Tracing";
		tProjDesc.m_wstrModelFilePath	= L"../../Resource/Models/Dat/Anim/Weapon_PalSphere_001.dat";
		tProjDesc.m_bDeferredAdd		= false;

		m_spPartnerPalSpherePool = CProjectilePool::Create(spPersistentLevel, EProjectileType::PARTNER_PALSPHERE, make_shared<FProjectileDesc>(tProjDesc), 1);
	}
	
	// 라이딩 팰스피어 풀
	{
		FProjectileDesc tProjDesc;

		tProjDesc.m_bIsAnimed			= true;
		tProjDesc.m_bActive				= true;
		tProjDesc.m_eWeaponActionType	= EWeaponActionType::PROJECTILE;
		tProjDesc.m_eRenderType			= ERenderType::RENDER_NBLEND_NONCULL; 
		tProjDesc.m_fColliderRadius		= 0.2f;
		tProjDesc.m_matPivot			= Matrix::CreateScale(0.0005f);
		tProjDesc.m_spOwnPlayer			= static_pointer_cast<CPlayer>(shared_from_this());
		tProjDesc.m_wstrGameObjectName	= L"SummonRiding";
		tProjDesc.m_wstrEffectName		= L"Group_PalSphere_Tracing";
		tProjDesc.m_wstrModelFilePath	= L"../../Resource/Models/Dat/Anim/Weapon_PalSphere_001.dat";
		tProjDesc.m_bDeferredAdd		= false;

		m_spRidingPalSpherePool = CProjectilePool::Create(spPersistentLevel, EProjectileType::RIDING_PALSPHERE, make_shared<FProjectileDesc>(tProjDesc), 1);
	}

	// 화살 풀
	{
		FProjectileDesc tArrowPool;
		tArrowPool.m_bIsAnimed			= true;
		tArrowPool.m_bActive			= true;
		tArrowPool.m_eWeaponActionType	= EWeaponActionType::PROJECTILE;
		tArrowPool.m_eRenderType		= ERenderType::RENDER_NBLEND;
		tArrowPool.m_fColliderRadius	= 0.2f;
		tArrowPool.m_matPivot			= Matrix::CreateScale(0.002f);
		tArrowPool.m_spOwnPlayer		= static_pointer_cast<CPlayer>(shared_from_this());
		tArrowPool.m_wstrGameObjectName = L"Arrow";
		tArrowPool.m_wstrEffectName		= L"Group_Arrow";
		tArrowPool.m_wstrModelFilePath	= L"../../Resource/Models/Dat/NonAnim/Weapon/EffectMeshs/Beam_inner.dat";
		tArrowPool.m_bDeferredAdd		= false;

		tArrowPool.m_fAttackPower = 20.f;

		m_spArrowPool = CProjectilePool::Create(spPersistentLevel, EProjectileType::ARROW, make_shared<FProjectileDesc>(tArrowPool), 5);
	}
}

void Client::CPlayer::InitUIPtr()
{
	// 포획 UI
	{
		m_wpCatchRateUI = m_wpUIManager.lock()->FindParentsPanel(L"CatchBeforeUI");
		m_wpCatchProgressBar = m_wpCatchRateUI.lock()->FindComponent<CProgressBarComponent>(L"CircleProgressBar");
		m_wpCatchRateText = m_wpCatchRateUI.lock()->FindChildGameObject(L"ChildProbabilityUI")->FindComponent<CTextComponent>(L"ProbabilityText");
		m_wpCatchArrowImage = m_wpCatchRateUI.lock()->FindChildGameObject(L"ChildProbabilityUI")->FindComponent<CImageComponent>(L"UpArrowImage");
		m_wpCatchAnimation0 = m_wpCatchRateUI.lock()->FindChildGameObject(L"ChildFirstCircleUI")->FindComponent<CAnimationImageComponent>(L"FirstCircleAnimation0");
		m_wpCatchAnimation1 = m_wpCatchRateUI.lock()->FindChildGameObject(L"ChildFirstCircleUI")->FindComponent<CAnimationImageComponent>(L"FirstCircleAnimation1");
	}

	// 스테미너 UI
	{
		m_wpStaminaUI = m_wpUIManager.lock()->FindParentsPanel(L"StaminaUI");
		m_wpStaminaProgress = m_wpStaminaUI.lock()->FindComponent<CProgressBarComponent>(L"CircleProgressBar");
	}

	// 라이딩 UI
	{
		m_wpRidingUI = m_wpUIManager.lock()->FindParentsPanel(L"RidingUI");
		m_wpRidingUI.lock()->SetActive(true);

		m_wpRidingObjectImage = m_wpRidingUI.lock()->FindComponent<CImageComponent>(L"PalImage");
		m_wpLockImage = m_wpRidingUI.lock()->FindComponent<CImageComponent>(L"LockImage");

		ChangeRidingPalUI();
	}
}

void Client::CPlayer::InitNotifyFunction()
{
	m_umapFunctions.emplace(L"DeclineStamina", [this]() { DeclineStamina(); });
}

void Client::CPlayer::SetOnAir()
{
	m_spCameraComponent->SetOnAir();
}

#pragma endregion

#pragma region GETTER/SETTER

Client::CPlayer::FPlayerStateData Client::CPlayer::GetPlayerData()
{
	m_tPlayerData.m_vMoveDirection = m_spPlayerControllerComponent->GetMoveDirection();
	m_tPlayerData.m_eVerticalMoveType = m_spPlayerControllerComponent->GetVerticalMovementType();
	m_tPlayerData.m_bJumping = m_spPlayerControllerComponent->IsJumping();
	m_tPlayerData.m_bOnAir = m_spPlayerControllerComponent->IsOnAir();
	m_tPlayerData.m_eControlMode = m_spPlayerControllerComponent->GetCurrentControlMode();

	return m_tPlayerData;
}

void Client::CPlayer::SetUseCameraMouseMove(bool _bUseMouseMove)
{
	m_spCameraComponent->SetMouseControl(_bUseMouseMove);
}

void Client::CPlayer::SetPositionWithController(const Vector3& _vPos)
{
	m_spPlayerControllerComponent->SetPosition(_vPos);
}

void Client::CPlayer::SetCustomInvenCustomObject(bool _bActivate)
{
	m_wpInvenCustomObject.lock()->SetActive(_bActivate);
}
void Client::CPlayer::ResetInteractingArchitectureData()
{
	m_tPlayerData.m_eInteractType = EInteractType::ENUM_END;
	
	if(!m_tPlayerData.m_wpInteractArchitecture.expired()) { m_tPlayerData.m_wpInteractArchitecture.reset(); }
	if(!m_tPlayerData.m_wpInteractFactory.expired()) { m_tPlayerData.m_wpInteractFactory.reset(); }
}

bool Client::CPlayer::IsArmorEquipped()
{
	return m_wpInventorySystem.lock()->GetEquippedShieldItemSlot().m_eItemType != EItemType::ENUM_END;
}

bool Client::CPlayer::IsGliderEquipped()
{
	return m_wpInventorySystem.lock()->GetEquippedGliderItemSlot().m_eItemType != EItemType::ENUM_END;
}

void Client::CPlayer::ChangeWeapon(EItemType _eWeaponID)
{
	if (_eWeaponID != EItemType::ENUM_END && _eWeaponID != EItemType::NONE && !m_umapWeapon.contains(_eWeaponID)) { return; }

	if (_eWeaponID == EItemType::ENUM_END) { _eWeaponID = EItemType::NONE; }

	if (m_tPlayerData.m_eWeaponID == _eWeaponID) { return; }

	if (!m_tPlayerData.m_bWeaponChangeCall)
	{
		if (!m_wpWeapon.expired())
		{
			m_wpWeapon.lock()->SetActive(false);
			m_tPlayerData.m_ePrevWeaponActionType = m_wpWeapon.lock()->GetWeaponActionType();
			m_tPlayerData.m_ePrevWeaponID = m_wpWeapon.lock()->GetItemType();

			if (m_tPlayerData.m_eWeaponID == EItemType::OLD_BOW) { ActiveArrow(false); }
		}
		else
		{
			m_tPlayerData.m_ePrevWeaponActionType = EWeaponActionType::NONE;
			m_tPlayerData.m_ePrevWeaponID = EItemType::NONE;
		}
				
		m_tPlayerData.m_bWeaponChangeCall = true;
		
		if (_eWeaponID == EItemType::NONE)
		{
			m_wpWeapon.reset();
			m_tPlayerData.m_eWeaponActionType = EWeaponActionType::NONE;
			m_tPlayerData.m_eWeaponID = EItemType::NONE;
		}
		else
		{
			m_wpWeapon = m_umapWeapon[_eWeaponID];
			m_tPlayerData.m_eWeaponActionType = m_wpWeapon.lock()->GetWeaponActionType();
			m_tPlayerData.m_eWeaponID = _eWeaponID;
			m_wpWeapon.lock()->SetActive(true);
		}
	}

	// weapon ui를 업데이트해줍니다.
	ChangeWeaponUI(_eWeaponID);
}

void Client::CPlayer::ChangeOutfit(EItemType _eOutfitType)
{
	if (_eOutfitType != EItemType::ENUM_END && _eOutfitType != EItemType::NONE && !m_umapOutiftMeshs.contains(_eOutfitType)) { return; }

	if (_eOutfitType == EItemType::ENUM_END) { _eOutfitType = EItemType::NONE; }

	if(m_umapOutiftMeshs[m_tPlayerData.m_eOutfitID])
		m_umapOutiftMeshs[m_tPlayerData.m_eOutfitID]->SetActive(false);

	if (_eOutfitType == EItemType::NONE)
	{
		m_umapOutiftMeshs[EItemType::Outfit_OldCloth001]->SetActive(true);
		m_tPlayerData.m_eOutfitID = EItemType::Outfit_OldCloth001;
	}
	else
	{
		m_umapOutiftMeshs[_eOutfitType]->SetActive(true);
		m_tPlayerData.m_eOutfitID = _eOutfitType;
	}
}

void Client::CPlayer::ChangeHeadEquip(EItemType _eHeadEquipType)
{
	if (_eHeadEquipType != EItemType::ENUM_END && _eHeadEquipType != EItemType::NONE && !m_umapHeadEquipMeshs.contains(_eHeadEquipType)) { return; }

	if (_eHeadEquipType == EItemType::ENUM_END) { _eHeadEquipType = EItemType::NONE; }

	if(m_umapHeadEquipMeshs[m_tPlayerData.m_eHeadEquipID])
		m_umapHeadEquipMeshs[m_tPlayerData.m_eHeadEquipID]->SetActive(false);

	if (_eHeadEquipType == EItemType::NONE)
	{
		m_spHairMeshComponent->SetActive(true);
	}
	else if ((_eHeadEquipType != EItemType::HeadEquip019 && _eHeadEquipType != EItemType::HeadEquip021))
	{
		m_spHairMeshComponent->SetActive(true);
		m_umapHeadEquipMeshs[_eHeadEquipType]->SetActive(true);
	}
	else
	{
		m_spHairMeshComponent->SetActive(false);
		m_umapHeadEquipMeshs[_eHeadEquipType]->SetActive(true);
	}
		
	m_tPlayerData.m_eHeadEquipID = _eHeadEquipType;
}

void Client::CPlayer::ZoomIn()
{
	if(!m_wpUIManager.lock()->IsOnUIPanel() && !m_wpBuildManager.lock()->IsBuildMode())
		m_tPlayerData.m_bAiming = true;
}

void Client::CPlayer::ChangeBowAnimation(int32 _iAnimID)
{
	m_umapWeapon[EItemType::OLD_BOW]->ChangeAnimation(_iAnimID);
}

void Client::CPlayer::ActiveCurrentWeapon(bool _bActivate)
{
	if (!m_wpWeapon.expired())
	{
		m_wpWeapon.lock()->SetActive(_bActivate);
	}
}

void Client::CPlayer::ActiveArrow(bool _bActivate)
{
	m_spArrowComponent->SetActive(_bActivate);
}

void Client::CPlayer::ActiveGlider(bool _bActivate)
{
	if (_bActivate)
	{
		if (!m_wpWeapon.expired())
			m_wpWeapon.lock()->SetActive(false);
	}
	else
	{
		if (!m_wpWeapon.expired())
			m_wpWeapon.lock()->SetActive(true);
	}

	if (!m_umapWeapon[EItemType::GLIDER]->IsActive())
	{
		m_wpSoundManager.lock()->PlaySoundEx("AKE_Player_Glider_Start_447303952", GetPosition().value());
		m_wpSoundManager.lock()->PlaySoundEx("Player_Female_Glider_Marina_EN_02_1047543790", GetPosition().value());
	}

	m_umapWeapon[EItemType::GLIDER]->SetActive(_bActivate);
}

void Client::CPlayer::ActivePalSphere(bool _bActivate)
{
	if (_bActivate)
	{
		if (!m_wpWeapon.expired())
			m_wpWeapon.lock()->SetActive(false);
	}
	else
	{
		if (!m_wpWeapon.expired() && !m_wpWeapon.lock()->IsActive())
			m_wpWeapon.lock()->SetActive(true);
	}

	m_umapWeapon[EItemType::BLUE_SPHERE]->SetActive(_bActivate);
}

void Client::CPlayer::ActiveConstructHammer(bool _bActivate)
{
	m_umapWeapon[EItemType::CONSTRUCT_HAMMER]->SetActive(_bActivate);
}

void Client::CPlayer::ActiveFryingPan(bool _bActivate)
{
	m_umapWeapon[EItemType::FRYING_PAN]->SetActive(_bActivate);
}

void Client::CPlayer::ActiveStoneAxe(bool _bActivate)
{
	m_umapWeapon[EItemType::STONE_AXE]->SetActive(_bActivate);
}

void Client::CPlayer::ActiveStonePickAxe(bool _bActivate)
{
	m_umapWeapon[EItemType::STONE_PICKAXE]->SetActive(_bActivate);
}

void Client::CPlayer::ActiveMeatCleaver(bool _bActivate)
{
	m_umapWeapon[EItemType::MEAT_CLEAVER]->SetActive(_bActivate);
}

void Client::CPlayer::ActiveStaminaUI(bool _bActivate)
{
	m_wpStaminaUI.lock()->SetActive(_bActivate);
}

void Client::CPlayer::DisableBowAnimation()
{
	m_umapWeapon[EItemType::OLD_BOW]->SetUseAnimation(false, true);
}

void Client::CPlayer::ChangeAssaultRifleAnimation(int32 _iAnimID)
{
	m_umapWeapon[EItemType::ASSAULT_RIFLE]->ChangeAnimation(_iAnimID);
}

bool Client::CPlayer::IsPalSphereActivate()
{
	return m_umapWeapon[EItemType::BLUE_SPHERE]->IsActive();
}

void Client::CPlayer::ThrowPalSphere(float _fSpeed)
{
	if (!CanThrowPalSphere()) { return; }

	m_wpInventorySystem.lock()->ReduceItemNum(EItemType::BLUE_SPHERE, 1);

	Vector3 vPosition = m_umapWeapon[EItemType::BLUE_SPHERE]->GetAttachedWorldPosition();
	Vector3 vCamLook = m_wpCameraManager.lock()->GetCurCameraWorldForwardVector();

	m_spPalSpherePool->Throw(vPosition, vCamLook, _fSpeed, 0.f);
}

void Client::CPlayer::ThrowPartnerPalSphere(float _fSpeed)
{
	Vector3 vPosition = m_wpCameraManager.lock()->GetCurCameraWorldPosition();
	Vector3 vDirection = m_wpCameraManager.lock()->GetCurCameraWorldForwardVector();

	PxRaycastHit tRaycastHit;
	int32 iLayers = (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::NATURE;

	Vector3 vPos;
	if (m_wpPhysXManager.lock()->SingleRayCast(vPosition, vDirection, 100.0f, iLayers, &tRaycastHit))
	{
		vPos = CPhysXManager::GetVector3FromPxVec3(tRaycastHit.position);
	}
	else
	{
		vPos = vPosition + vDirection * 1000.f;
	}

	Vector3 vStartPos = m_umapWeapon[EItemType::BLUE_SPHERE]->GetAttachedWorldPosition();
	Vector3 vDir = vPos - vStartPos;
	vDir.Normalize();

	m_spPartnerPalSpherePool->Throw(vStartPos, vDir, _fSpeed, 0.f);
}

void Client::CPlayer::ThrowRidingPalSphere(float _fSpeed)
{
	Vector3 vPosition = m_umapWeapon[EItemType::BLUE_SPHERE]->GetAttachedWorldPosition();
	Vector3 vCamLook = m_wpCameraManager.lock()->GetCurCameraWorldForwardVector();

	m_spRidingPalSpherePool->Throw(vPosition, vCamLook, _fSpeed, 0.f);
}

void Client::CPlayer::SpawnRidingObject(const Vector3& _vSpawnPos)
{
	// 파트너 팰이 소환중인 경우 | 이미 라이딩이 소환중인 경우 | 현재 인덱스의 라이딩이 사용 불가일 경우
	if (IsPartnerPalSpawned() || IsRidingObjectSpawned() || !m_arrCanUseRiding[m_iCurRidingObjectIndex]) { return; }

	m_wpSpawnedRidingObject = m_arrRidingObjects[m_iCurRidingObjectIndex];

	m_wpSpawnedRidingObject.lock()->SetActive(true);
	m_wpSpawnedRidingObject.lock()->GetBodyColliderComponent()->RequestTransform(_vSpawnPos);

	m_wpSoundManager.lock()->PlaySoundEx("PAL_Sphere_02_Start_574309610", GetPosition().value());

	if(m_iCurRidingObjectIndex == (int32)ERidingObjectType::JET_DRAGON) { m_wpSoundManager.lock()->PlaySoundEx("VO_JetDragon_01_Normal_474464904", GetPosition().value()); }
	else if(m_iCurRidingObjectIndex == (int32)ERidingObjectType::WIND_DEER) { m_wpSoundManager.lock()->PlaySoundEx("VO_FengyunDeeper_01_Normal_PT1_2_758156059", GetPosition().value()); }
}

void Client::CPlayer::SpawnPartnerPal(const Vector3& _vSpawnPosition)
{
	// 이미 파트너 팰이 있는 경우
	if (m_spPartnerPal != nullptr) { return; }

	FPalBoxPalInfo tPalInfo = m_arrPartnerPals[m_iCurPalInfoIndex].first;
	shared_ptr<CMonster> spPartnerPal = m_arrPartnerPals[m_iCurPalInfoIndex].second;

	m_spPartnerPal = spPartnerPal;

	spPartnerPal->SetActive(true);
	spPartnerPal->SetCulled(false);

	// 전투를 요청합니다.
	spPartnerPal->RequestCombatCommand();

	// 팰 스피어 위치로 이동합니다.
	Vector3 vSpawnPosition = _vSpawnPosition;
	if (spPartnerPal->GetMonsterType() == EMonsterType::DREAM_DEMON)
	{
		vSpawnPosition.y += 1.0f;
	}
	spPartnerPal->GetBodyColliderComponent()->RequestTransform(vSpawnPosition);

	// 파트너 팰의 체력 UI를 출력합니다.
	spPartnerPal->SetPalHPPanel();

	m_wpSoundManager.lock()->PlaySoundEx("PAL_Sphere_02_Start_574309610", GetPosition().value());

	switch (spPartnerPal->GetMonsterType())
	{
	case Client::EMonsterType::BERRY_GOAT:
		m_wpSoundManager.lock()->PlaySoundEx("VO_BerryGoat_01_Normal_3_59043235", m_spPartnerPal->GetPosition().value());
		break;
	case Client::EMonsterType::BOAR:
		m_wpSoundManager.lock()->PlaySoundEx("VO_Boar_01_Normal_164867547", m_spPartnerPal->GetPosition().value());
		break;
	case Client::EMonsterType::CHICKEN_PAL:
		m_wpSoundManager.lock()->PlaySoundEx("VO_ChickenPal_01_Normal_02_673501484", m_spPartnerPal->GetPosition().value());
		break;
	case Client::EMonsterType::DREAM_DEMON:
		m_wpSoundManager.lock()->PlaySoundEx("VO_DreamDemon_01_Normal_2_559150555", m_spPartnerPal->GetPosition().value());
		break;
	case Client::EMonsterType::HEDGE_HOG:
		m_wpSoundManager.lock()->PlaySoundEx("VO_Hedgehog_01_Normal_2_246580790", m_spPartnerPal->GetPosition().value());
		break;
	case Client::EMonsterType::KITSUNEBI:
		m_wpSoundManager.lock()->PlaySoundEx("VO_Kitsunebi_01_Normal_541210977", m_spPartnerPal->GetPosition().value());
		break;
	case Client::EMonsterType::PENGUIN:
		m_wpSoundManager.lock()->PlaySoundEx("VO_Penguin_01_Normal_291409068", m_spPartnerPal->GetPosition().value());
		break;
	case Client::EMonsterType::PINK_CAT:
		m_wpSoundManager.lock()->PlaySoundEx("VO_PinkCat_01_Normal_02_717847191", m_spPartnerPal->GetPosition().value());
		break;
	case Client::EMonsterType::SHEEP_BALL:
		m_wpSoundManager.lock()->PlaySoundEx("VO_SheepBall_01_Normal_PT1_2_744521170", m_spPartnerPal->GetPosition().value());
		break;
	case Client::EMonsterType::WOOL_FOX:
		m_wpSoundManager.lock()->PlaySoundEx("VO_WoolFox_01_Normal_427540710", m_spPartnerPal->GetPosition().value());
		break;
	default:
		break;
	}
}

void Client::CPlayer::RetrieveRidingObject()
{
	// 아무것도 없으면 소환 안함
	if (!IsRidingObjectSpawned()) { return; }

	// 회수 트레일을 출력합니다. 
	Vector3 vStartPos = m_wpSpawnedRidingObject.lock()->GetPosition().value() + Vector3(0.f, 1.f, 0.f);
	Vector3 vEndPos = GetPosition().value() + Vector3(0.f, 1.f, 0.f);
	m_wpEffectManager.lock()->ActivePalRetrieveTrail(vStartPos, vEndPos, 0.1f);

	// 팰 회수 이펙트를 출력합니다.
	m_wpEffectManager.lock()->ActivePalSummonParticle(m_wpSpawnedRidingObject.lock(), m_wpSpawnedRidingObject.lock()->GetMeshComponent()->GetModel());

	m_wpSpawnedRidingObject.lock()->SetActive(false);
	m_wpSpawnedRidingObject.reset();

	m_wpSoundManager.lock()->PlaySoundEx("PAL_Sphere_02_Suck_659059517", GetPosition().value());
}

void Client::CPlayer::RetrievePartnerPal()
{
	if (m_spPartnerPal == nullptr) { return; }

	// 회수 트레일을 출력합니다.
	Vector3 vStartPos = m_spPartnerPal->GetPosition().value() + Vector3(0.f, 1.f, 0.f);
	Vector3 vEndPos = GetPosition().value() + Vector3(0.f, 1.f, 0.f);
	m_wpEffectManager.lock()->ActivePalRetrieveTrail(vStartPos, vEndPos, 0.1f);

	m_spPartnerPal->RetrieveToPalSphere();
	m_spPartnerPal = nullptr;

	m_wpSoundManager.lock()->PlaySoundEx("PAL_Sphere_02_Suck_659059517", GetPosition().value());
}

bool Client::CPlayer::FireAssaultRifle()
{
	if (m_tPlayerData.m_iCurrentBullet <= 0) { return false; }

	m_spMeshComponent->ShakeUpperBody(1.f, 0.1f);
	//m_spCameraComponent->ShakeCamera(EShakeType::SHAKE_ROTX, 0.25f, 0.1f);
	m_spCameraComponent->ShakeCamera(EShakeType::SHAKE_ROTZ, 0.f, 0.55f, 0.15f);
	m_spCameraComponent->ShakeCamera(EShakeType::SHAKE_FOV, 0.f, 0.75f, 0.15f);

	m_tPlayerData.m_iCurrentBullet -= 1;
	m_wpInventorySystem.lock()->ReduceItemNum(EItemType::RIFLE_BULLET, 1);
	ChangeWeaponUI(EItemType::ASSAULT_RIFLE);

	Vector3 vRifleWorldPos = m_umapWeapon[EItemType::ASSAULT_RIFLE]->GetBoneWorldMatrix(L"Muzzle").Translation();
	m_wpEffectManager.lock()->ActivateEffect(L"Group_GunFire", vRifleWorldPos);

	Vector3 vPosition = m_wpCameraManager.lock()->GetCurCameraWorldPosition();
	Vector3 vDirection = m_wpCameraManager.lock()->GetCurCameraWorldForwardVector();

	int32 iCollisionFlag = (int32)ECollisionLayer::MONSTER | (int32)ECollisionLayer::ENVIRONMENT | (int32)ECollisionLayer::BUILDING;

	PxRaycastHit tRaycastHit;
	if (m_wpPhysXManager.lock()->SingleRayCast(vPosition, vDirection, 100.0f, iCollisionFlag, &tRaycastHit))
	{
		// 콜라이더 컴포넌트를 상속받은 경우, userData에 콜라이더 컴포넌트 생포인터가 저장되어 있습니다.
		if (tRaycastHit.actor->userData)
		{
			CColliderComponent* pSrcCollider = static_cast<CColliderComponent*>(tRaycastHit.actor->userData);

			if (pSrcCollider->GetCollisionLayer() == ECollisionLayer::MONSTER)
			{
				// 종합 공격력 = 본인 공격력 + 무기 공격력
				float fWeaponAttackPower = m_wpInventorySystem.lock()->GetEquippedWeaponItemSlots()[m_iCurrentWeaponSlotIndex].m_fAttackPower;
				float fTotalAttackPower = m_fAttackPower + fWeaponAttackPower;

				static_pointer_cast<CMonster>(pSrcCollider->GetOwnerGameObject())->TakeDamage(shared_from_this(), fTotalAttackPower);
			}

			Vector3 vPos = CPhysXManager::GetVector3FromPxVec3(tRaycastHit.position);
			m_wpEffectManager.lock()->ActivateEffect(L"Group_Hit2", vPos);
		}
	}

	return true;
}

void Client::CPlayer::ReloadBullet()
{
	int32 iLeftBulletCount = m_wpInventorySystem.lock()->FindInventoryItemTotalNum(EItemType::RIFLE_BULLET);

	if(iLeftBulletCount >= 40)
		m_tPlayerData.m_iCurrentBullet = 40;
	else
		m_tPlayerData.m_iCurrentBullet = iLeftBulletCount;

	ChangeWeaponUI(EItemType::ASSAULT_RIFLE);
}

void Client::CPlayer::FireArrow()
{
	int32 iArrowCount = m_wpInventorySystem.lock()->FindInventoryItemTotalNum(EItemType::ARROW);
	if (iArrowCount <= 0) { return; }

	m_wpInventorySystem.lock()->ReduceItemNum(EItemType::ARROW, 1);
	ChangeWeaponUI(EItemType::OLD_BOW);

	Vector3 vPosition = m_wpCameraManager.lock()->GetCurCameraWorldPosition();
	Vector3 vDirection = m_wpCameraManager.lock()->GetCurCameraWorldForwardVector();

	PxRaycastHit tRaycastHit;
	int32 iLayers = (int)ECollisionLayer::BOSS
		| (int)ECollisionLayer::MONSTER
		| (int)ECollisionLayer::ENVIRONMENT
		| (int)ECollisionLayer::BUILDING
		| (int)ECollisionLayer::NATURE;

	Vector3 vPos;
	if (m_wpPhysXManager.lock()->SingleRayCast(vPosition, vDirection, 100.0f, iLayers, &tRaycastHit))
	{
		vPos = CPhysXManager::GetVector3FromPxVec3(tRaycastHit.position);
	}
	else
	{
		vPos = vPosition + vDirection * 1000.f;
	}

	Matrix vBowMatrix = m_umapWeapon[EItemType::OLD_BOW]->GetAttachedWorldMatrix();
	Vector3 vStartPos = vBowMatrix.Translation() + vBowMatrix.Backward() * 0.5f + vBowMatrix.Right() * 0.5f;

	Vector3 vDir = vPos - vStartPos;
	vDir.Normalize();

	m_spArrowPool->Throw(vStartPos, vDir, 30.f, m_fAttackPower);
}

void Client::CPlayer::SetSpeed(float _fSpeed)
{
	m_spPlayerControllerComponent->SetCurMoveSpeed(_fSpeed);
}

void Client::CPlayer::ChangeControlMode(Client::EPlayerControllerMode _eControlMode)
{
	m_spPlayerControllerComponent->ChangeControllerMode(_eControlMode);
}

Client::EPlayerControllerMode Client::CPlayer::GetCurrentControlMode()
{
	return m_spPlayerControllerComponent->GetCurrentControlMode();
}

void Client::CPlayer::SetCanJump(bool _bCanJump)
{
	m_spPlayerControllerComponent->SetCanJump(_bCanJump);
}

void Client::CPlayer::SetTerminalVelocity(float _fTerminalVelocity)
{
	m_spPlayerControllerComponent->SetTerminalVelocity(_fTerminalVelocity);
}

void Client::CPlayer::SetTerminalVelocityAsDefault()
{
	m_spPlayerControllerComponent->SetTerminalVelocityAsDefault();
}

void Client::CPlayer::SetFixedMoveDirection(Vector3 _vFixedMoveDir)
{
	m_spPlayerControllerComponent->SetFixedMoveDirection(_vFixedMoveDir);
}

void Client::CPlayer::ActiveLedgeMode()
{
	m_spPlayerControllerComponent->ActiveLedgeMode();
}

void Client::CPlayer::SetInteractType(EInteractType _eInteractType)
{
	m_tPlayerData.m_ePrevInteractType = m_tPlayerData.m_eInteractType;
	m_tPlayerData.m_eInteractType = _eInteractType;
}

void Client::CPlayer::SetCatchRateUI(const std::shared_ptr<CMonster>& _spTargetMonster, const Vector3& _vRayHitPos)
{
	if (_spTargetMonster)
	{
		if (false == m_wpCatchRateUI.lock()->IsActive())
		{
			m_wpCatchAnimation0.lock()->SetLoop(false);
			m_wpCatchAnimation0.lock()->SetStartEndIndex(0, 2);
			m_wpCatchAnimation1.lock()->SetLoop(false);
			m_wpCatchAnimation1.lock()->SetStartEndIndex(0, 2);
			m_wpCatchRateUI.lock()->SetActive(true);
			m_wpCatchRateText.lock()->SetText(to_wstring(static_cast<int32>(_spTargetMonster->GetCapturePercentage())));
			float fRatio = _spTargetMonster->GetCapturePercentage() * 0.01f;
			m_wpCatchProgressBar.lock()->SetRatio(fRatio);
			Vector4 vColor = m_wpUIManager.lock()->ChangeCirCleProgressBarColor(fRatio);
			m_wpCatchProgressBar.lock()->SetColor(vColor);
			m_wpCatchArrowImage.lock()->SetColor(vColor);
			Vector3 vPos = _spTargetMonster->GetPosition().value();
			m_wpCatchRateUI.lock()->SetPivotWorldPoint(Vector3(vPos.x, _vRayHitPos.y, vPos.z));
			//m_wpCatchRateUI.lock()->SetPivotWorldPoint(Vector3(vPos.x, vPos.y, vPos.z));
		}

		if (m_wpCatchAnimation0.lock()->IsFinish())
		{
			if (false == m_wpCatchAnimation0.lock()->GetLoop())
			{
				m_wpCatchAnimation0.lock()->SetLoop(true);
				m_wpCatchAnimation0.lock()->SetStartEndIndex(2, 3);
				m_wpCatchAnimation1.lock()->SetLoop(true);
				m_wpCatchAnimation1.lock()->SetStartEndIndex(2, 3);
			}
		}

		int i = 0;
	}
	else
	{
		m_wpCatchRateUI.lock()->SetActive(false);
	}
}

void Client::CPlayer::EnActiveCatchRateUI()
{
	m_wpCatchRateUI.lock()->SetActive(false);
}

#pragma endregion

#pragma region Input

void Client::CPlayer::UnlockRiding(ERidingObjectType _eRidingObjectType)
{
	if (_eRidingObjectType == ERidingObjectType::ENUM_END) { return; }

	m_arrCanUseRiding[(int32)_eRidingObjectType] = true;
	ChangeRidingPalUI();
}

void Client::CPlayer::KeyInput()
{
	WeaponChangeInput();
	StandChangeInput();
	PartnerPalInput();
	UIInput(); 
	CheatInput();

#ifdef _DEBUG
	TestInput();
#endif // _DEBUG
}

void Client::CPlayer::ResetInteractAllyPal()
{
	if (!m_tPlayerData.m_wpInteractAllyPal.expired())
	{
		m_tPlayerData.m_wpInteractAllyPal.reset();
	}
}

void Client::CPlayer::WeaponChangeInput()
{
	EPlayerState eCurrentState = (EPlayerState)m_spFSMComponent->GetState();

	if (eCurrentState != EPlayerState::IDLE &&
		eCurrentState != EPlayerState::MOVE &&
		eCurrentState != EPlayerState::JUMP)
		return;

	if (!m_wpBuildManager.lock()->IsBuildMode())
	{
		if (m_wpInputManager.lock()->IsScrollDown())
		{
			++m_iCurrentWeaponSlotIndex;

			if (m_iCurrentWeaponSlotIndex > 3) { m_iCurrentWeaponSlotIndex = 0; }

			CoumputeCurrentWeapon();
		}
		else if (m_wpInputManager.lock()->IsScrollUp())
		{
			--m_iCurrentWeaponSlotIndex;

			if (m_iCurrentWeaponSlotIndex < 0) { m_iCurrentWeaponSlotIndex = 3; }

			CoumputeCurrentWeapon();
		}
	}
}

void Client::CPlayer::StandChangeInput()
{
	if (m_wpInputManager.lock()->IsKeyDown(DIK_C))
	{
		if (m_tPlayerData.m_eStandType == EStandType::CROUCH) { m_tPlayerData.m_eStandType = EStandType::STAND; }

		else if (m_tPlayerData.m_eStandType == EStandType::STAND) { m_tPlayerData.m_eStandType = EStandType::CROUCH; }
	}
}

void Client::CPlayer::AddExp(float _fExp)
{
	m_fCurExp += _fExp;

	while (m_fCurExp >= m_fMaxExp)
	{
		m_fCurExp -= _fExp;
		m_iCurCreatureLevel++;

		m_wpEffectManager.lock()->ActivateEffect(L"Group_LevelUp", GetPosition().value());
	}
}

void Client::CPlayer::AimChangeInput()
{
	if (m_wpInputManager.lock()->IsMouseDown(EMouseActionType::RIGHT_BUTTON) && !m_wpUIManager.lock()->IsOnUIPanel() && !m_wpBuildManager.lock()->IsBuildMode())
	{
		m_tPlayerData.m_bAiming = true;
	}

	else if (m_wpInputManager.lock()->IsMouseReleased(EMouseActionType::RIGHT_BUTTON) && m_tPlayerData.m_bAiming)
	{
		m_tPlayerData.m_bAiming = false;
	}
}

void Client::CPlayer::PartnerPalInput()
{
	// 파트너 스킬을 사용합니다. -> 파트너 스킬은 폐기되었습니다.
	/*
	if (m_wpInputManager.lock()->IsKeyDown(DIK_Z))
	{
		// 파트너 팰이 없는 경우
		if (!m_spPartnerPal) { return; }

		bool bPartnerSkill = m_spPartnerPal->GetBoolValue(EBlackboardBoolValue::BOOL_PARTNER_SKILL);
		m_spPartnerPal->ModifyBoolValue(EBlackboardBoolValue::BOOL_PARTNER_SKILL, !bPartnerSkill);
	}
	*/
}

void Client::CPlayer::CheatInput()
{
	if (m_wpInputManager.lock()->IsKeyDown(DIK_F1))
	{
		m_bInvincibility = !m_bInvincibility;
	}
}

void Client::CPlayer::TestInput()
{
	if (m_wpInputManager.lock()->IsKeyDown(DIK_0))
	{
		UnlockRiding(ERidingObjectType::WIND_DEER);
		UnlockRiding(ERidingObjectType::JET_DRAGON);
	}
	
	if (m_wpInputManager.lock()->IsKeyDown(DIK_9))
	{
		if (CEngineModule::GetInstance()->GetInputManager()->GetCursorLocked())
		{
			CEngineModule::GetInstance()->GetInputManager()->LockCursor(false);
		}
		else
		{
			CEngineModule::GetInstance()->GetInputManager()->LockCursor(true);
		}
	}

	if (m_wpInputManager.lock()->IsKeyDown(DIK_U))
	{
		AddExp(110.f);
		//m_bDead = false;
		//m_fCurHp = 100.f;
	}
}

void Client::CPlayer::UIInput()
{
	if (!m_wpUIManager.lock()->IsOnUIPanel())
	{
		if (m_wpInputManager.lock()->IsKeyDown(DIK_1))
		{
			m_wpSoundManager.lock()->PlaySoundEx("InvenClickSound");
			NextPalIndex(0);
		}
		else if (m_wpInputManager.lock()->IsKeyDown(DIK_3))
		{
			m_wpSoundManager.lock()->PlaySoundEx("InvenClickSound");
			NextPalIndex(1);
		}

		if (m_wpInputManager.lock()->IsKeyDown(DIK_6))
		{
			m_iCurRidingObjectIndex++;

			if (m_iCurRidingObjectIndex >= (int32)ERidingObjectType::ENUM_END) { m_iCurRidingObjectIndex = 0; }

			ChangeRidingPalUI();
		}
	}
}

void Client::CPlayer::ChangeRidingPalUI()
{
	ERidingObjectType eRidingObjType = (ERidingObjectType)m_iCurRidingObjectIndex;

	switch (eRidingObjType)
	{
	case Client::ERidingObjectType::WIND_DEER:
	{
		m_wpRidingObjectImage.lock()->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(EMonsterType::FENGYUNDEEPER));
		if (m_arrCanUseRiding[(int32)eRidingObjType]) { m_wpLockImage.lock()->SetActive(false); }
		else { m_wpLockImage.lock()->SetActive(true); }
	}
		break;
	case Client::ERidingObjectType::JET_DRAGON:
	{
		m_wpRidingObjectImage.lock()->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(EMonsterType::JETDRAGON));
		if (m_arrCanUseRiding[(int32)eRidingObjType]) { m_wpLockImage.lock()->SetActive(false); }
		else { m_wpLockImage.lock()->SetActive(true); }
	}
		break;
	default:
		break;
	}
}

#pragma endregion

#pragma region Compute

void Client::CPlayer::AdjustPlayerRotation()
{
	if (m_spFSMComponent->GetState() == (int32)EPlayerState::ROLL ||
		m_spPlayerControllerComponent->GetCurrentControlMode() == EPlayerControllerMode::CLIMB_MODE || 
		m_spFSMComponent->GetState() == (int32)EPlayerState::RIDING)
		return;

	Vector3 vCurrentLook = GetForwardVector().value();
	Vector3 vDestLook = Vector3::Lerp(vCurrentLook, m_vDestLook, 0.35f);
	Vector3 vCurrentPos = GetPosition().value();

	m_wpTransformComponent.lock()->LookAt(vCurrentPos + vDestLook);
}

void Client::CPlayer::ComputeCamera()
{
	// radius 조정
	if (m_bRidingSprint)
	{
		m_fCurrentRadius = lerp(m_fCurrentRadius, m_fRidingFarRadius, 0.1f);
	}
	else if ((EPlayerState)m_spFSMComponent->GetState() == EPlayerState::RIDING)
	{
		m_fCurrentRadius = lerp(m_fCurrentRadius, m_fRidingNearRadius, 0.1f);
	}
	else if ((EPlayerState)m_spFSMComponent->GetState() == EPlayerState::INTERACT && m_tPlayerData.m_eInteractType == EInteractType::SLEEP)
	{
		m_fCurrentRadius = lerp(m_fCurrentRadius, m_fSleepRaius, 0.1f);
	}
	else if (m_tPlayerData.m_bAiming)
	{
		m_wpStaminaUI.lock()->SetActive(false);
		m_fCurrentRadius = lerp(m_fCurrentRadius, m_fZoomInRadius, 0.1f);
		m_fCurrentFOV = lerp(m_fCurrentFOV, m_fZoomFOV, 0.1f);
	}
	else
	{
		m_fCurrentRadius = lerp(m_fCurrentRadius, m_fDefaultRadius, 0.1f);
		m_fCurrentFOV = lerp(m_fCurrentFOV, m_fDefaultFOV, 0.1f);
	}

	m_spCameraComponent->SetSphericalCameraRadius(m_fCurrentRadius);
	m_spCameraComponent->SetFieldOfView(m_fCurrentFOV);

	// 카메라 축 오프셋 조정
	Vector3 vCurrentAxisOffset = m_spCameraComponent->GetSphericalCameraAxisOffset();

	if (m_tPlayerData.m_eStandType == EStandType::CROUCH)
	{
		vCurrentAxisOffset = Vector3::Lerp(vCurrentAxisOffset, m_vCrouchCameraOffset, 0.1f);
	}
	else
	{
		if (m_tPlayerData.m_bAiming)
		{
			vCurrentAxisOffset = Vector3::Lerp(vCurrentAxisOffset, m_vZoomInCameraOffset, 0.1f);
		}
		else
		{
			vCurrentAxisOffset = Vector3::Lerp(vCurrentAxisOffset, m_vStandCameraOffset, 0.1f);
		}
	}

	m_spCameraComponent->SetSphericalCameraAxisOffset(vCurrentAxisOffset);
}

void Client::CPlayer::ComputeShield(float _fDeltaSeconds)
{
	if (IsArmorEquipped() && m_iShieldCount < 5)
	{
		m_fShieldTimeChecker += _fDeltaSeconds;

		if (m_fShieldTimeChecker > 10.f)
		{
			m_fShieldTimeChecker = 0.f;
			m_iShieldCount++;
		}
	}
}

void Client::CPlayer::ComputeStamina(float _fDeltaSeconds)
{
	if(m_tPlayerData.m_bStaminaEmptied) { m_wpStaminaProgress.lock()->SetColor(Vector4(1.f, 0.f, 0.f, 1.f)); }

	if (m_fCurStaminaGauge < 100)
	{
		m_wpStaminaUI.lock()->SetActive(true);
		m_fCurStaminaGauge += _fDeltaSeconds * 7.f;
	}
	else
	{
		if (m_tPlayerData.m_bStaminaEmptied) 
		{
			m_tPlayerData.m_bStaminaEmptied = false; 
			m_wpStaminaProgress.lock()->SetColor(Vector4(1.f, 1.f, 1.f, 1.f));
		}
		m_wpStaminaUI.lock()->SetActive(false);
	}
}

void Client::CPlayer::SetUpRotationFreeWithCamera()
{
	Vector3 vMoveDirection = m_spPlayerControllerComponent->GetMoveDirection();
	Vector3 vCamLook = m_spCameraComponent->GetSphericalCameraForwardVector();
	Vector3 vCamRight = m_spCameraComponent->GetSphericalCameraRightVector();

	m_vDestLook = vCamRight * vMoveDirection.x + vCamLook * vMoveDirection.z;
	m_vDestLook.y = 0.0f;
	m_vDestLook.Normalize();
}

void Client::CPlayer::AddForce(const Vector3& _vForceDirection, float _fPower)
{
	m_spPlayerControllerComponent->AddForce(_vForceDirection, _fPower);
}

void Client::CPlayer::SetUpRotationWithCameraLook()
{
	m_vDestLook = m_spCameraComponent->GetSphericalCameraForwardVector();
	m_vDestLook.y = 0.0f;
	m_vDestLook.Normalize();
}

void Client::CPlayer::ComputeSpineRotationWithCameraLook()
{
	float fDstRotationX = 0.0f;
	float fDstRotationY = 0.0f;

	if (!m_tPlayerData.m_bUseSpineRot)
	{
		m_fPrevSpineRotX = lerp(m_fPrevSpineRotX, fDstRotationX, 0.1f);
		m_fPrevSpineRotY = lerp(m_fPrevSpineRotY, fDstRotationY, 0.1f);
	}
	else
	{
		if (m_tPlayerData.m_eWeaponActionType == EWeaponActionType::BOW)
		{
			fDstRotationX = m_spCameraComponent->GetCurElevationRadian() - XMConvertToRadians(20.f);
			fDstRotationY = XMConvertToRadians(15.f);
		}
		else if (m_tPlayerData.m_eWeaponActionType == EWeaponActionType::ASSAULT_RIFLE)
		{
			fDstRotationX = m_spCameraComponent->GetCurElevationRadian() -XMConvertToRadians(10.f);
			fDstRotationY = XMConvertToRadians(15.f);
		}
		else
		{
			fDstRotationX = m_spCameraComponent->GetCurElevationRadian();
			fDstRotationY = XMConvertToRadians(15.f);
		}

		m_fPrevSpineRotX = lerp(m_fPrevSpineRotX, fDstRotationX, 0.1f);
		m_fPrevSpineRotY = lerp(m_fPrevSpineRotY, fDstRotationY, 0.1f);
	}

	m_spMeshComponent->SetRotationAtBone(L"spine_01", Vector3(m_fPrevSpineRotX, m_fPrevSpineRotY, 0.f));
}

void Client::CPlayer::CheckInteractType(const std::shared_ptr<CArchitecture> _spArchitecture)
{
	m_tPlayerData.m_wpInteractArchitecture = _spArchitecture;

	if (_spArchitecture->IsFactory()) { m_tPlayerData.m_wpInteractFactory = static_pointer_cast<CFactory>(_spArchitecture); }

	if (_spArchitecture->IsBuildComplete())
	{
		if (_spArchitecture->IsFactory())
		{
			m_tPlayerData.m_wpInteractFactory = static_pointer_cast<CFactory>(_spArchitecture);
			if (m_tPlayerData.m_wpInteractFactory.lock()->GetCanInteract())
			{
				EFactoryType eFactory = m_tPlayerData.m_wpInteractFactory.lock()->GetFactoryType();

				switch (eFactory)
				{
				case Client::EFactoryType::WORK_BENCH:
				case Client::EFactoryType::ADVANCED_WORKBENCH:
				case Client::EFactoryType::REPAIR_BENCH:
				case Client::EFactoryType::SPHERE_TABLE:
				case Client::EFactoryType::WEAPON_WORKBENCH:
					m_tPlayerData.m_eInteractType = EInteractType::WORK;
					break;

				case Client::EFactoryType::CAMPFIRE:
				case Client::EFactoryType::POT:
					m_tPlayerData.m_eInteractType = EInteractType::COOKING;
					break;

				case Client::EFactoryType::BERRY_FARM:
				case Client::EFactoryType::WHEAT_FARM:
					m_tPlayerData.m_eInteractType = EInteractType::PLANTING;
					break;


				case Client::EFactoryType::LUMBERYARD:
					m_tPlayerData.m_eInteractType = EInteractType::DEFOREST;
					break;

				case Client::EFactoryType::STONE_PIT:
					m_tPlayerData.m_eInteractType = EInteractType::MINING;
					break;

				case Client::EFactoryType::BLAST_FURNACE:
					m_tPlayerData.m_eInteractType = EInteractType::COLLECT;
					break;

				default:
					break;
				}
			}
		}
		else
		{
			switch (_spArchitecture->GetArchitectureType())
			{
			case EArchitectureType::IRON_BOX:
			case EArchitectureType::WOODEN_CHEST:
			case EArchitectureType::PAL_BOX:
			{
				m_tPlayerData.m_wpInteractArchitecture = _spArchitecture;
				m_tPlayerData.m_eInteractType = EInteractType::OPEN;
			}
				break;
			case EArchitectureType::HUMAN_BED:
			{
				m_tPlayerData.m_wpInteractArchitecture = _spArchitecture;
				m_tPlayerData.m_eInteractType = EInteractType::SLEEP;
			}
				break;
			default:
			{
				if (!m_tPlayerData.m_wpInteractArchitecture.expired())
					m_tPlayerData.m_wpInteractArchitecture.reset();
			}
				break;
			}

			return;
		}
	}
	else
	{
		m_tPlayerData.m_eInteractType = EInteractType::ARCHITECT;
	}
}

void Client::CPlayer::ComputeMouseScrollLock()
{
	if (m_wpUIManager.lock()->IsOnUIPanel())
	{
		m_spCameraComponent->SetMouseControl(false);
	}
	else
	{
		m_spCameraComponent->SetMouseControl(true);
	}
}

void Client::CPlayer::ComputeHp(float _fDeltaSeconds)
{
	if (m_fCurHp < m_fMaxHp)
	{
		m_fCurHp += _fDeltaSeconds * 2.f;
		m_fCurHp = min(m_fCurHp, m_fMaxHp);
	}
}

void Client::CPlayer::ComputeHunger(float _fDeltaSeconds)
{
	if (m_fCurHungerGauge > 0)
	{
		m_fCurHungerGauge -= _fDeltaSeconds * 0.05f;
		m_fCurHungerGauge = max(0.f, m_fCurHungerGauge);
	}	
}

void Client::CPlayer::CoumputeCurrentWeapon()
{
	FInventoryItemInfo tItemInfo = m_wpInventorySystem.lock()->GetEquippedWeaponItemSlots()[m_iCurrentWeaponSlotIndex];

	ChangeWeapon(tItemInfo.m_eItemType);

	m_wpInvenCustomObject.lock()->ChangeWeaponWithID(tItemInfo.m_eItemType);
}

void Client::CPlayer::ComputeCurrentHeadEquip()
{
	FInventoryItemInfo tItemInfo = m_wpInventorySystem.lock()->GetEquippedHeadItemSlot();

	ChangeHeadEquip(tItemInfo.m_eItemType);

	m_wpInvenCustomObject.lock()->ChangeHeadEquipWithID(tItemInfo.m_eItemType);
}

void Client::CPlayer::ComputeCurrentCloth()
{
	FInventoryItemInfo tItemInfo = m_wpInventorySystem.lock()->GetEquippedBodyItemSlot();

	ChangeOutfit(tItemInfo.m_eItemType);

	m_wpInvenCustomObject.lock()->ChangeOutfitWithID(tItemInfo.m_eItemType);
}

void Client::CPlayer::RideOff()
{
	if (!m_tPlayerData.m_wpRidingObject.expired())
	{
		m_tPlayerData.m_wpRidingObject.reset();
	}

	m_tPlayerData.m_eInteractType = EInteractType::ENUM_END;
}

void Client::CPlayer::UpdateHUDUI(float _fDeltaSeconds)
{
	// 체력이 이전 체력보다 적을 경우 체력을 깎아줍니다.
	if (m_fPreHp > m_fCurHp)
	{
		m_wpUIManager.lock()->GetHUDHPInfo().spCurHPTextComponent->SetText(to_wstring(static_cast<int>(m_fCurHp)));
		if (m_wpUIManager.lock()->MinusHP(m_wpUIManager.lock()->GetHUDHPInfo().spHPProgressComponent, m_fPreHp, m_fCurHp, m_fMaxHp, &m_fHPLerpTime, _fDeltaSeconds))
		{
			m_fPreHp = m_fCurHp;
		}
	}
	else if (m_fPreHp < m_fCurHp)
	{
		//m_wpUIManager.lock()->GetHUDHPInfo().spCurHPTextComponent->SetText(to_wstring(static_cast<int>(m_fCurHp)));
		if (m_wpUIManager.lock()->PlusHP(m_wpUIManager.lock()->GetHUDHPInfo().spHPProgressComponent, m_wpUIManager.lock()->GetHUDHPInfo().spCurHPTextComponent, m_fPreHp, m_fCurHp, m_fMaxHp, &m_fHPLerpTime, _fDeltaSeconds))
		{
			m_fPreHp = m_fCurHp;
		}
	}

	// 공복 ui를 관리합니다.
	m_wpUIManager.lock()->GetHungerPanel();

	if (m_fCurHungerGauge <= 50.f)
	{
		if (false == m_wpUIManager.lock()->GetHungerPanel()->IsActive())
		{
			m_wpUIManager.lock()->GetHungerPanel()->SetActive(true);
		}
	}
	else
	{
		if (true == m_wpUIManager.lock()->GetHungerPanel()->IsActive())
		{
			m_wpUIManager.lock()->GetHungerPanel()->SetActive(false);
		}
	}

	// 공복 상태를 업데이트해줍니다.
	float fHungerRatio =  m_fCurHungerGauge / m_fMaxHungerGauge;
	m_wpUIManager.lock()->GetHUDHPInfo().spHungerProgressComponent->SetRatio(fHungerRatio);

	if (IsArmorEquipped())
	{
		if (false == m_wpUIManager.lock()->GetHUDHPInfo().spShieldProgressComponent->IsActive())
		{
			m_wpUIManager.lock()->SetActiveShield(true);
		}

		// 실드 상태를 업데이트해줍니다.
		if (m_iPreShieldCount != m_iShieldCount)
		{
			if (m_wpUIManager.lock()->MinusShield(m_wpUIManager.lock()->GetHUDHPInfo().spShieldProgressComponent, m_iPreShieldCount, m_iShieldCount, &m_fShieldLerpTime, _fDeltaSeconds))
			{
				m_iPreShieldCount = m_iShieldCount;
			}
		}
	}
	else
	{
		m_wpUIManager.lock()->SetActiveShield(false);
	}
}

void Client::CPlayer::NextPalIndex(uint32 _iDirection)
{
	std::shared_ptr<FHUDPalInfo> spPalInfo = m_wpUIManager.lock()->GetHUDPalInfoPointer();

	// 1번 키를 눌러 왼쪽으로 넘겼을 때입니다.
	if (0 == _iDirection)
	{
		if (m_iCurPalInfoIndex < 4)
		{ 
			m_iCurPalInfoIndex++;
		}
		else
		{
			m_iCurPalInfoIndex = 0;
		}

		uint32 iPrePalInfoIndex = m_iCurPalInfoIndex - 1;
		uint32 iNextPalInfoIndex = m_iCurPalInfoIndex + 1;
		if (0 == m_iCurPalInfoIndex)
		{
			iPrePalInfoIndex = 4;
		}
		if (4 == m_iCurPalInfoIndex)
		{
			iNextPalInfoIndex = 0;
		}

		if (0 == spPalInfo->e_LeftSlot)
		{
			spPalInfo->e_LeftSlot = 1;
			spPalInfo->e_CenterSlot = 2;
			spPalInfo->e_RightSlot = 0;

			spPalInfo->spPalSlotComponent0->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iNextPalInfoIndex].first.m_eMonsterType), true);

			// 각각 자리에 맞는 애니메이션을 재생합니다.
			spPalInfo->spPalSlotComponent0->SetStartEndIndex(7, 8);
			//spPalInfo->spPalSlotComponent0->SetStartEndIndex(8, 9);
			spPalInfo->spPalSlotComponent1->SetStartEndIndex(3, 4);
			spPalInfo->spPalSlotComponent2->SetStartEndIndex(1, 2);
		}
		else if (1 == spPalInfo->e_LeftSlot)
		{
			spPalInfo->e_LeftSlot = 2;
			spPalInfo->e_CenterSlot = 0;
			spPalInfo->e_RightSlot = 1;

			spPalInfo->spPalSlotComponent1->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iNextPalInfoIndex].first.m_eMonsterType), true);

			spPalInfo->spPalSlotComponent0->SetStartEndIndex(1, 2);
			spPalInfo->spPalSlotComponent1->SetStartEndIndex(7, 8);
			//spPalInfo->spPalSlotComponent1->SetStartEndIndex(8, 9);
			spPalInfo->spPalSlotComponent2->SetStartEndIndex(3, 4);
		}
		else if (2 == spPalInfo->e_LeftSlot)
		{
			spPalInfo->e_LeftSlot = 0;
			spPalInfo->e_CenterSlot = 1;
			spPalInfo->e_RightSlot = 2;

			spPalInfo->spPalSlotComponent2->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iNextPalInfoIndex].first.m_eMonsterType), true);

			spPalInfo->spPalSlotComponent0->SetStartEndIndex(3, 4);
			spPalInfo->spPalSlotComponent1->SetStartEndIndex(1, 2);
			spPalInfo->spPalSlotComponent2->SetStartEndIndex(7, 8);
			//spPalInfo->spPalSlotComponent2->SetStartEndIndex(8, 9);
		}
	}
	// 3번 키를 눌러 오른쯕으로 넘겼을 때입니다.
	else if (1 == _iDirection)
	{
		if (m_iCurPalInfoIndex > 0)
		{
			m_iCurPalInfoIndex--;
		}
		else
		{
			m_iCurPalInfoIndex = 4;
		}

		uint32 iPrePalInfoIndex = m_iCurPalInfoIndex - 1;
		uint32 iNextPalInfoIndex = m_iCurPalInfoIndex + 1;
		if (0 == m_iCurPalInfoIndex)
		{
			iPrePalInfoIndex = 4;
		}
		if (4 == m_iCurPalInfoIndex)
		{
			iNextPalInfoIndex = 0;
		}

		if (0 == spPalInfo->e_RightSlot)
		{
			spPalInfo->e_LeftSlot = 0;
			spPalInfo->e_CenterSlot = 1;
			spPalInfo->e_RightSlot = 2;

			// 각각 자리에 맞는 애니메이션을 재생합니다.
			spPalInfo->spPalSlotComponent0->SetStartEndIndex(14, 15);
			spPalInfo->spPalSlotComponent1->SetStartEndIndex(10, 11);
			spPalInfo->spPalSlotComponent2->SetStartEndIndex(12, 13);

			spPalInfo->spPalSlotComponent0->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iPrePalInfoIndex].first.m_eMonsterType), true);
		}
		else if (1 == spPalInfo->e_RightSlot)
		{
			spPalInfo->e_LeftSlot = 1;
			spPalInfo->e_CenterSlot = 2;
			spPalInfo->e_RightSlot = 0;

			// 각각 자리에 맞는 애니메이션을 재생합니다.
			spPalInfo->spPalSlotComponent0->SetStartEndIndex(12, 13);
			spPalInfo->spPalSlotComponent1->SetStartEndIndex(14, 15);
			spPalInfo->spPalSlotComponent2->SetStartEndIndex(10, 11);

			spPalInfo->spPalSlotComponent1->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iPrePalInfoIndex].first.m_eMonsterType), true);
		}
		else if (2 == spPalInfo->e_RightSlot)
		{
			spPalInfo->e_LeftSlot = 2;
			spPalInfo->e_CenterSlot = 0;
			spPalInfo->e_RightSlot = 1;

			// 각각 자리에 맞는 애니메이션을 재생합니다.
			spPalInfo->spPalSlotComponent0->SetStartEndIndex(10, 11);
			spPalInfo->spPalSlotComponent1->SetStartEndIndex(12, 13);
			spPalInfo->spPalSlotComponent2->SetStartEndIndex(14, 15);

			spPalInfo->spPalSlotComponent2->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iPrePalInfoIndex].first.m_eMonsterType), true);
		}
	}
}

void Client::CPlayer::UpdatePalIndexUI()
{
	std::shared_ptr<FHUDPalInfo> spPalInfo = m_wpUIManager.lock()->GetHUDPalInfoPointer();

	uint32 iPrePalInfoIndex = m_iCurPalInfoIndex - 1;
	uint32 iNextPalInfoIndex = m_iCurPalInfoIndex + 1;
	if (0 == m_iCurPalInfoIndex)
	{
		iPrePalInfoIndex = 4;
	}
	if (4 == m_iCurPalInfoIndex)
	{
		iNextPalInfoIndex = 0;
	}

	if (0 == spPalInfo->e_LeftSlot)
	{
		m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent0->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iPrePalInfoIndex].first.m_eMonsterType));
		m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent1->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[m_iCurPalInfoIndex].first.m_eMonsterType));
		m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent2->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iNextPalInfoIndex].first.m_eMonsterType));
	}
	else if(1 == spPalInfo->e_LeftSlot)
	{
		m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent0->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iNextPalInfoIndex].first.m_eMonsterType));
		m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent1->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iPrePalInfoIndex].first.m_eMonsterType));
		m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent2->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[m_iCurPalInfoIndex].first.m_eMonsterType));
	}
	else if (2 == spPalInfo->e_LeftSlot)
	{
		m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent0->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[m_iCurPalInfoIndex].first.m_eMonsterType));
		m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent1->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iNextPalInfoIndex].first.m_eMonsterType));
		m_wpUIManager.lock()->GetHUDPalInfo().spPalSlotComponent2->SetTextureFileName(m_wpUIManager.lock()->FindPalIconTextureName(m_arrPartnerPals[iPrePalInfoIndex].first.m_eMonsterType));
	}
}

void Client::CPlayer::ChangePalSphereUI()
{
	std::shared_ptr<FHUDWeaponInfo> spWeaponInfo = m_wpUIManager.lock()->GetHUDWeaponInfoPointer();

	int32 iSphereNum = m_wpInventorySystem.lock()->FindInventoryItemTotalNum(EItemType::BLUE_SPHERE);
	std::wstring wstrSphereNum;
	if (10 > iSphereNum)
	{
		wstrSphereNum = L"00" + to_wstring(iSphereNum);
	}
	else if (100 > iSphereNum)
	{
		wstrSphereNum = L"0" + to_wstring(iSphereNum);
	}
	else
	{
		wstrSphereNum = to_wstring(iSphereNum);
	}

	spWeaponInfo->spPalSphereNumComponent->SetText(wstrSphereNum);
}

void Client::CPlayer::ChangeWeaponUI(EItemType _eItemType)
{
	std::shared_ptr<FHUDWeaponInfo> spWeaponInfo = m_wpUIManager.lock()->GetHUDWeaponInfoPointer();

	if (EItemType::BLUE_SPHERE == _eItemType || EItemType::GREEN_SPHERE == _eItemType)
	{
		return;
	}

	spWeaponInfo->spWeaponImageComponent->SetTextureFileName(m_wpUIManager.lock()->FindHUDWeaponTextureName(_eItemType));
	spWeaponInfo->spWeaponNameComponent->SetText(m_wpUIManager.lock()->FindItemName(_eItemType));

	if (EItemType::OLD_BOW == _eItemType)
	{
		spWeaponInfo->spWeaponCurNumComponent->SetActive(true);
		spWeaponInfo->spWeaponOnceNumComponent->SetActive(true);

		int32 iArrowNum = m_wpInventorySystem.lock()->FindInventoryItemTotalNum(EItemType::ARROW);
		std::wstring wstrArrowNum;
		if (10 > iArrowNum)
		{
			wstrArrowNum = L"0" + to_wstring(iArrowNum);
		}
		else
		{
			wstrArrowNum = to_wstring(iArrowNum);
		}

		spWeaponInfo->spWeaponCurNumComponent->SetText(wstrArrowNum);
		spWeaponInfo->spWeaponOnceNumComponent->SetText(L"1");
	}
	else if (EItemType::ASSAULT_RIFLE == _eItemType)
	{
		spWeaponInfo->spWeaponCurNumComponent->SetActive(true);
		spWeaponInfo->spWeaponOnceNumComponent->SetActive(true);

		std::wstring wstrBulletNum;
		if (10 > m_tPlayerData.m_iCurrentBullet)
		{
			wstrBulletNum = L"0" + to_wstring(m_tPlayerData.m_iCurrentBullet);
		}
		else
		{
			wstrBulletNum = to_wstring(m_tPlayerData.m_iCurrentBullet);
		}

		spWeaponInfo->spWeaponCurNumComponent->SetText(wstrBulletNum);
		spWeaponInfo->spWeaponOnceNumComponent->SetText(L"1");
	}
	else
	{
		spWeaponInfo->spWeaponCurNumComponent->SetActive(false);
		spWeaponInfo->spWeaponOnceNumComponent->SetActive(false);
	}
}

void Client::CPlayer::LoadMeshComponent(const wstring& _wstrFilePath, EItemType _eItemType, unordered_map<EItemType, shared_ptr<CMeshComponent>>& _umapMeshContainer, const wstring& _wstrAttachBoneName)
{
	filesystem::path filePath(_wstrFilePath);

	FMeshComponentDesc tMeshDesc;
	tMeshDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshDesc.m_eComponentType = EComponentType::MESH;
	tMeshDesc.m_wpOwnerGameObject = shared_from_this();
	tMeshDesc.m_spParentMeshComponent = m_spMeshComponent;

	if(_wstrAttachBoneName != L"") { tMeshDesc.m_spPivotBone = m_spMeshComponent->GetBonePtr(_wstrAttachBoneName); }

	// mesh component desc
	if (_wstrAttachBoneName == L"")
		tMeshDesc.m_matPivot = Matrix::CreateScale(0.0001f);
	else
		tMeshDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));

	shared_ptr<CMeshComponent> spMeshComponent = nullptr;

	wstring componentName = filePath.filename().wstring();
	size_t iPoint = componentName.find('.');
	if (iPoint != wstring::npos) { componentName = componentName.substr(0, iPoint); }
	componentName += L"Component";

	tMeshDesc.m_wstrComponentName = componentName;
	tMeshDesc.m_wstrModelFilePath = filePath.wstring();

	spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshDesc));
	spMeshComponent->SetActive(false);
	_umapMeshContainer.emplace(_eItemType, spMeshComponent);
	AddComponent(tMeshDesc.m_wstrComponentName, spMeshComponent);
}

void Client::CPlayer::DeclineStamina()
{
	m_fCurStaminaGauge -= 5.f; 
	m_fCurStaminaGauge = max(0.f, m_fCurStaminaGauge);

	if (m_fCurStaminaGauge == 0.f) 
	{
		m_tPlayerData.m_bStaminaEmptied = true; 
	}
}

#pragma endregion

bool Client::CPlayer::TryToAddCapturedPal(const std::shared_ptr<CMonster>& _spMonster)
{
	int32 iPartnerPalSize = static_cast<int32>(m_arrPartnerPals.size());
	for (int32 iIndex = 0; iIndex < iPartnerPalSize; ++iIndex)
	{
		if (m_arrPartnerPals[iIndex].second == nullptr)
		{
			EMonsterType eMonsterType = _spMonster->GetMonsterType();

			FPalBoxPalInfo tPalBoxPalInfo;
			tPalBoxPalInfo.m_iIndexX					= iIndex;
			tPalBoxPalInfo.m_iIndexY					= 0;
			tPalBoxPalInfo.m_eMonsterAIType				= EMonsterAIType::PARTNER_PAL;
			tPalBoxPalInfo.m_eToDoType					= EToDoType::ENUM_END;
			tPalBoxPalInfo.m_wstrTextureFileName		= CClientLoop::s_spClientDataSheet->k_mapPalIconTextureNames.at(eMonsterType);
			tPalBoxPalInfo.m_eMonsterElementType		= CClientLoop::s_spClientDataSheet->k_mapPalElementTypes.at(eMonsterType);
			tPalBoxPalInfo.m_wstrMonsterName			= CClientLoop::s_spClientDataSheet->k_mapPalName.at(eMonsterType);
			tPalBoxPalInfo.m_eMonsterType				= _spMonster->GetMonsterType();
			tPalBoxPalInfo.m_eMonsterPersonalityType	= _spMonster->GetMonsterPersonalityType();
			tPalBoxPalInfo.m_iMonsterLevel				= _spMonster->GetCurCreatureLevel();
			tPalBoxPalInfo.m_fCurHp						= _spMonster->GetCurHp();
			tPalBoxPalInfo.m_fCurHungerGauge			= _spMonster->GetCurHungerGauge();
			tPalBoxPalInfo.m_fCurExp					= _spMonster->GetCurExp();
			tPalBoxPalInfo.m_fAttackPower				= _spMonster->GetAttackPower();
			tPalBoxPalInfo.m_fDefensePower				= _spMonster->GetDefensePower();
			tPalBoxPalInfo.m_fWorkSpeed					= CClientLoop::s_spClientDataSheet->k_mapPalDefaultToDoRatios.at(eMonsterType);
			tPalBoxPalInfo.m_iFoodAmount				= CClientLoop::s_spClientDataSheet->k_mapPalFoodAmounts.at(eMonsterType);
			tPalBoxPalInfo.m_iPartnerSkillLevel			= _spMonster->GetPartnerSkillLevel();
			tPalBoxPalInfo.m_iActiveSkillLevelA			= _spMonster->GetActiveSkillLevelA();
			tPalBoxPalInfo.m_iActiveSkillLevelB			= _spMonster->GetActiveSkillLevelB();
			tPalBoxPalInfo.m_iActiveSkillLevelC			= _spMonster->GetActiveSkillLevelC();
			tPalBoxPalInfo.m_wstrPartnerSkillName		= CClientLoop::s_spClientDataSheet->k_mapPalPartnerSkillName.at(eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameA		= CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameA.at(eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameB		= CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameB.at(eMonsterType);
			tPalBoxPalInfo.m_wstrActiveSkillNameC		= CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameC.at(eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementA		= CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeA.at(eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementB		= CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeB.at(eMonsterType);
			tPalBoxPalInfo.m_eActiveSkillElementC		= CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeC.at(eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameA		= CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameA.at(eMonsterType);
			tPalBoxPalInfo.m_wstrPassiveSkillNameB		= CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameB.at(eMonsterType);
			
			m_arrPartnerPals[iIndex].first = tPalBoxPalInfo;
			m_arrPartnerPals[iIndex].second = _spMonster;

			CGameManager::GetInstance()->GetPalBoxSystem()->AddPalboxPalCount(eMonsterType);
			
			// hud pal ui 연동입니다.
			UpdatePalIndexUI();

			return true;
		}
	}
	return false;
}

#pragma endregion
