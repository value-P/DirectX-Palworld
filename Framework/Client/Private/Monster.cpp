// default
#include "pch.h"
#include "Monster.h"

// engine manager
#include "ClientLoop.h"
#include "EngineModule.h"
#include "TimerManager.h"
#include "SoundManager.h"

// client manager
#include "GameManager.h"
#include "UIManager.h"
#include "EffectManager.h"

// system
#include "ToDoSystem.h"
#include "CombatSystem.h"
#include "InventorySystem.h"

// engine
#include "Date.h"

// gameobject
#include "Player.h"
#include "Factory.h"
#include "Item.h"
#include "Nature.h"
#include "Ingredient.h"
#include "Mineral.h"
#include "PalBed.h"
#include "Chest.h"
#include "IronChest.h"
#include "Weapon.h"

// component
#include "Component.h"
#include "ColliderComponent.h"
#include "SphereComponent.h"
#include "BoxComponent.h"
#include "MeshComponent.h"
#include "BehaviorTreeComponent.h"
#include "ProgressBarComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"

// engine
#include "Model.h"
#include "Bone.h"
#include "MeshBuffer.h"
#include "Material.h"
#include "Panel.h"

// composite node
#include "CompositeNode.h"

// utility
#include "MathUtility.h"

HRESULT Client::CMonster::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// creature desc
	if (FAILED(CCreature::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	m_eCreatureType = ECreatureType::PAL;

	// monster desc
	shared_ptr<FMonsterDesc> spMonsterDesc = static_pointer_cast<FMonsterDesc>(_spGameObjectDesc);
	
	m_eMonsterType				= spMonsterDesc->m_eMonsterType;

	m_fCurHp					= CClientLoop::s_spClientDataSheet->k_mapPalHPs.at(m_eMonsterType);

	m_fMoveSpeed				= CClientLoop::s_spClientDataSheet->k_mapPalDefaultMoveSpeeds.at(m_eMonsterType);
	m_fRunSpeed					= CClientLoop::s_spClientDataSheet->k_mapPalDefaultRunSpeeds.at(m_eMonsterType);
	m_fCreepSpeed				= CClientLoop::s_spClientDataSheet->k_mapPalDefaultCreepSpeeds.at(m_eMonsterType);

	m_fAttackPower				= CClientLoop::s_spClientDataSheet->k_mapPalDefaultAttackPowers.at(m_eMonsterType);
	m_fDefensePower				= CClientLoop::s_spClientDataSheet->k_mapPalDefaultDefensePowers.at(m_eMonsterType);

	m_wstrActiveSkillNameA		= CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameA.at(m_eMonsterType);
	m_wstrActiveSkillNameB		= CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameB.at(m_eMonsterType);
	m_wstrActiveSkillNameC		= CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillNameC.at(m_eMonsterType);

	m_eActiveSkillElementTypeA = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeA.at(m_eMonsterType);
	m_eActiveSkillElementTypeB = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeB.at(m_eMonsterType);
	m_eActiveSkillElementTypeC = CClientLoop::s_spClientDataSheet->k_mapPalActiveSkillElementTypeC.at(m_eMonsterType);

	m_wstrPassiveSkillNameA		= CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameA.at(m_eMonsterType);
	m_wstrPassiveSkillNameB		= CClientLoop::s_spClientDataSheet->k_mapPalPassiveSkillNameB.at(m_eMonsterType);

	m_iPartnerSkillLevel		= 1; // spMonsterDesc->m_iPartnerSkillLevel;
	m_iActiveSkillLevelA		= 1; // spMonsterDesc->m_iActiveSkillALevel;
	m_iActiveSkillLevelB		= spMonsterDesc->m_iActiveSkillBLevel;
	m_iActiveSkillLevelC		= spMonsterDesc->m_iActiveSkillCLevel;

	m_iMonsterLevel				= spMonsterDesc->m_iMonsterLevel;
	m_eMonsterAIType			= spMonsterDesc->m_eMonsterAIType;
	m_eMonsterPersonalityType	= spMonsterDesc->m_eMonsterPersonalityType;

	// etc
	m_spCombatSystem			= CGameManager::GetInstance()->GetCombatSystem();
	m_spToDoSystem				= CGameManager::GetInstance()->GetToDoSystem();
	m_spPlayer					= CGameManager::GetInstance()->GetPlayer();
	m_wpUIManager				= CUIManager::GetInstance();

	InitBlackboardValues();

	if (EMonsterAIType::SLAVE_PAL == m_eMonsterAIType) { SetPalWorkPanel(); }
	m_spPalHPPanelInfo = make_shared<FPalHPInfo>();
	return S_OK;
}

HRESULT Client::CMonster::BeginPlay()
{
	m_spDate = CEngineModule::GetInstance()->GetTimerManager()->FindDate(L"Date120");

	if (FAILED(CCreature::BeginPlay())) { return E_FAIL; }

	// 거점 팰인 경우, 작업 시스템에 등록하고 작업을 요청합니다.
	if (m_eMonsterAIType == EMonsterAIType::SLAVE_PAL)
	{
		AddSlavePalToToDoSystem();
	}

	// ui 사이즈를 정해줍니다.
	SetUISize();

	return S_OK;
}

int32 Client::CMonster::PreTick(float _fDeltaSeconds)
{
	switch(CCreature::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	m_fToDoDeltaSeconds = _fDeltaSeconds;
	UpdateBlackboardValues();
	return 0;
}

int32 Client::CMonster::Tick(float _fDeltaSeconds)
{
	switch(CCreature::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	// 허기 게이지를 감소시킵니다.
	switch (m_eMonsterAIType)
	{
	case Client::EMonsterAIType::PARTNER_PAL:
	case Client::EMonsterAIType::SLAVE_PAL:
		// 허기 게이지는 나중에 다시 구현합니다.
		// DecreaseHungerGauge(_fDeltaSeconds);
		break;
	}

	// 체력 UI 위치를 갱신합니다.
	switch (m_eMonsterAIType)
	{
	case Client::EMonsterAIType::WILD_PAL:
	{
		if (m_bCombat && m_spPalHPPanelInfo->m_spPalHPPanel)
		{
			m_spPalHPPanelInfo->m_spPalHPPanel->SetPivotWorldPoint(GetPosition().value() + GetUISize());
			if (m_fCurHp < m_fPreHp)
			{
				if (m_wpUIManager.lock()->MinusHP(m_spPalHPPanelInfo->m_spHPProgress, m_fPreHp, m_fCurHp, m_fMaxHp, &m_fHPLerpTime, _fDeltaSeconds))
				{
					m_fPreHp = m_fCurHp;
				}
			}

		}
	}	
	break;

	case Client::EMonsterAIType::PARTNER_PAL:
	{
		if (m_spPalHPPanelInfo->m_spPalHPPanel)
		{
			m_spPalHPPanelInfo->m_spPalHPPanel->SetPivotWorldPoint(GetPosition().value() + GetUISize());
			if (m_fCurHp < m_fPreHp)
			{
				if (m_wpUIManager.lock()->MinusHP(m_spPalHPPanelInfo->m_spHPProgress, m_fPreHp, m_fCurHp, m_fMaxHp, &m_fHPLerpTime, _fDeltaSeconds))
				{
					m_fPreHp = m_fCurHp;
				}
			}
		}
	}
	break;
	}

	return 0;
}

int32 Client::CMonster::PostTick(float _fDeltaSeconds)
{
	switch(CCreature::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CMonster::EndPlay()
{
	if (FAILED(CCreature::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CMonster::Release()
{
	// 시스템
	m_spCombatSystem = nullptr;
	m_spToDoSystem = nullptr;

	// BT 루트 노드
	m_spRootNode->Release();
	m_spRootNode = nullptr;

	// 컴포넌트
	m_spBehaviorTreeComponent = nullptr;
	m_spHpRectComponent = nullptr;

	// 블랙보드
	for (auto& pair : m_umapBBGameObjects) { pair.second = nullptr; }
	m_umapBBGameObjects.clear();

	for (auto& pair : m_umapTools) { pair.second = nullptr; }
	m_umapTools.clear();

	m_spPlayer = nullptr;
	m_spDate = nullptr;

	// 패널
	m_spHpPanel = nullptr;

	ReturnPalWorkPanel();

	CCreature::Release();
}

bool Client::CMonster::GetBoolValue(EBlackboardBoolValue _eBBValue)
{
	return m_umapBBBools[_eBBValue];
}

float Client::CMonster::GetFloatValue(EBlackboardFloatValue _eBBValue)
{
	return m_umapBBFloats[_eBBValue];
}

int32 Client::CMonster::GetIntValue(EBlackboardIntValue _eBBValue)
{
	return m_umapBBInts[_eBBValue];
}

Vector3 Client::CMonster::GetVector3Value(EBlackboardVector3Value _eBBValue)
{
	return m_umapBBVector3s[_eBBValue];
}

shared_ptr<CGameObject> Client::CMonster::GetGameObjectValue(EBlackboardGameObjectValue _eBBValue)
{
	return m_umapBBGameObjects[_eBBValue];
}

const std::vector<EChannelType>& Client::CMonster::GetChannelValue(int32 _iAnimationID)
{
	return m_umapBBChannels[_iAnimationID];
}

HRESULT Client::CMonster::ModifyBoolValue(EBlackboardBoolValue _eBBValue, bool _bValue)
{
	if (!m_umapBBBools.contains(_eBBValue))
	{
		CHECKF(false, L"This bool value doesn't exist.");
		return E_FAIL;
	}

	m_umapBBBools[_eBBValue] = _bValue;
	return S_OK;
}

HRESULT Client::CMonster::ModifyFloatValue(EBlackboardFloatValue _eBBValue, float _fValue)
{
	if (!m_umapBBFloats.contains(_eBBValue))
	{
		CHECKF(false, L"This float value doesn't exist.");
		return E_FAIL;
	}

	m_umapBBFloats[_eBBValue] = _fValue;
	return S_OK;
}

HRESULT Client::CMonster::ModifyIntValue(EBlackboardIntValue _eBBValue, int32 _iValue)
{
	if (!m_umapBBInts.contains(_eBBValue))
	{
		CHECKF(false, L"This int value doesn't exist.");
		return E_FAIL;
	}

	m_umapBBInts[_eBBValue] = _iValue;
	return S_OK;
}

HRESULT Client::CMonster::ModifyVector3Value(EBlackboardVector3Value _eBBValue, const Vector3& _vValue)
{
	if (!m_umapBBVector3s.contains(_eBBValue))
	{
		CHECKF(false, L"This Vector3 value doesn't exist.");
		return E_FAIL;
	}

	m_umapBBVector3s[_eBBValue] = _vValue;
	return S_OK;
}

HRESULT Client::CMonster::ModifyGameObjectValue(EBlackboardGameObjectValue _eBBValue, const std::shared_ptr<CGameObject>& _spValue)
{
	if (!m_umapBBGameObjects.contains(_eBBValue))
	{
		CHECKF(false, L"This GameObject value doesn't exist.");
		return E_FAIL;
	}

	m_umapBBGameObjects[_eBBValue] = _spValue;
	return S_OK;
}

HRESULT Client::CMonster::ModifyChannelValue(int32 _iAnimationID, const std::vector<EChannelType>& _vChannelType)
{
	if (!m_umapBBChannels.contains(_iAnimationID))
	{
		m_umapBBChannels.emplace(_iAnimationID, _vChannelType);
		return S_OK;
	}

	m_umapBBChannels[_iAnimationID] = _vChannelType;
	return S_OK;
}

void Client::CMonster::RequestToDoCommand()
{
	m_spToDoSystem->RequestCommandFromSlavePal();
}

HRESULT Client::CMonster::StartToDoCommand(const std::shared_ptr<FToDoCommand>& _spToDoCommand)
{
	m_wpToDoCommand = _spToDoCommand;
	m_eMonsterToDoState = EMonsterToDoState::WORK;
	
	switch (_spToDoCommand->m_eToDoType)
	{
	// 생산하기
	case EToDoType::PRODUCE_ITEM:
	{
		m_wpFactory = dynamic_pointer_cast<CFactory>(_spToDoCommand->m_spHabitat);
		ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, _spToDoCommand->m_spHabitat);

		// 공장 크기에 따라 반지름을 설정합니다.
		Vector3 vExtents = m_wpFactory.lock()->GetColliderExtents();
		float fAcceptableRadius = max(vExtents.x, vExtents.z) + 0.5f;
		fAcceptableRadius += GetAcceptableRadiusFromColliderComponent();
		ModifyFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, fAcceptableRadius);
	}
	break;

	// 건설하기
	case EToDoType::BUILD_ARCHITECTURE:
	{
		m_wpArchitecture = dynamic_pointer_cast<CArchitecture>(_spToDoCommand->m_spHabitat);
		ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, _spToDoCommand->m_spHabitat);

		// 건물 크기에 따라 반지름을 설정합니다.
		Vector3 vExtents = m_wpArchitecture.lock()->GetColliderExtents();
		float fAcceptableRadius = max(vExtents.x, vExtents.z) + 0.5f;
		fAcceptableRadius += GetAcceptableRadiusFromColliderComponent();
		ModifyFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, fAcceptableRadius);
	}
	break;

	// 벌목소 나무 자르기
	case EToDoType::CUT_DOWN_LUMBERYARD:
	{
		m_wpFactory = dynamic_pointer_cast<CFactory>(_spToDoCommand->m_spHabitat);
		ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, _spToDoCommand->m_spHabitat);

		float fAcceptableRadius = GetAcceptableRadiusFromColliderComponent();
		ModifyFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, fAcceptableRadius);
	}
	break;

	// 채석장 채굴하기
	case EToDoType::MINE_STONE_PIT:
	{
		m_wpFactory = dynamic_pointer_cast<CFactory>(_spToDoCommand->m_spHabitat);
		ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, _spToDoCommand->m_spHabitat);

		float fAcceptableRadius = GetAcceptableRadiusFromColliderComponent();
		ModifyFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, fAcceptableRadius);
	}
	break;

	// 야생 나무 자르기
	case EToDoType::CUT_DOWN_RAW_TREE:
	{
		m_wpNature = dynamic_pointer_cast<CNature>(_spToDoCommand->m_spHabitat);
		ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, _spToDoCommand->m_spHabitat);

		float fAcceptableRadius = GetAcceptableRadiusFromColliderComponent();
		fAcceptableRadius += m_wpNature.lock()->GetAcceptableRadius();
		ModifyFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, fAcceptableRadius);
	}
	break;

	// 야생 바위 채굴하기
	case EToDoType::MINE_RAW_ROCK:
	{
		m_wpNature = dynamic_pointer_cast<CNature>(_spToDoCommand->m_spHabitat);
		ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, _spToDoCommand->m_spHabitat);

		float fAcceptableRadius = GetAcceptableRadiusFromColliderComponent();
		fAcceptableRadius += m_wpNature.lock()->GetAcceptableRadius();
		ModifyFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, fAcceptableRadius);
	}
	break;

	// 돌 조각, 나무 조각 옮기기
	case EToDoType::TRANSPORT_INGREDIENT:
	case EToDoType::TRANSPORT_MINERAL:
	{
		m_wpItem = dynamic_pointer_cast<CItem>(_spToDoCommand->m_spItem);
		
		shared_ptr<CArchitecture> spWoodenChest = FindArchitecture(EArchitectureType::WOODEN_CHEST);
		shared_ptr<CArchitecture> spIronChest = FindArchitecture(EArchitectureType::IRON_BOX);
		
		if (!spWoodenChest && spIronChest)			{ m_wpChestBox = spIronChest; }
		
		else if (spWoodenChest && !spIronChest)		{ m_wpChestBox = spWoodenChest; }

		else if (!spWoodenChest && !spIronChest)
		{
			if (CMathUtility::GetRandomBool(0.5f))	{ m_wpChestBox = spWoodenChest; }
			else { m_wpChestBox = spIronChest; }
		}
		else
		{
			CHECKF(false, L"Any chest box doesn't exist.");
		}

		float fAcceptableRadius = GetAcceptableRadiusFromColliderComponent();
		float fItemRadius = 0.5f;

		ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, _spToDoCommand->m_spItem);
		ModifyFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, fAcceptableRadius + fItemRadius);
	}
	break;

	// 농사 짓기
	case EToDoType::FARM_BERRY:
	{
		m_wpFactory = dynamic_pointer_cast<CFactory>(_spToDoCommand->m_spHabitat);
		ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, _spToDoCommand->m_spHabitat);

		Vector3 vExtents = m_wpFactory.lock()->GetColliderExtents();
		float fAcceptableRadius = max(vExtents.x, vExtents.z) + 0.5f;
		fAcceptableRadius += GetAcceptableRadiusFromColliderComponent();
		ModifyFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, fAcceptableRadius);
	}
	break;

	// 요리하기
	case EToDoType::COOK_FOOD:
	{
		m_wpFactory = dynamic_pointer_cast<CFactory>(_spToDoCommand->m_spHabitat);
		ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, _spToDoCommand->m_spHabitat);

		Vector3 vExtents = m_wpFactory.lock()->GetColliderExtents();
		float fAcceptableRadius = max(vExtents.x, vExtents.z) + 0.5f;
		fAcceptableRadius += GetAcceptableRadiusFromColliderComponent();
		ModifyFloatValue(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, fAcceptableRadius);
	}
	break;

	default:
		CHECKF(false, L"To do type is not designated.");
		break;
	}
	
	ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_START, true);
	return S_OK;
}

HRESULT Client::CMonster::FinishToDoCommand()
{
	if (!m_wpToDoCommand.expired())
	{
		m_wpToDoCommand.lock()->m_spHabitat = nullptr;
		m_wpToDoCommand.lock()->m_spItem = nullptr;
		m_wpToDoCommand.lock()->m_eToDoCommandState = EToDoCommandState::FINISHED;
		m_wpToDoCommand.reset();
	}

	m_eMonsterToDoState = EMonsterToDoState::NO_WORK;

	ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_START, false);
	ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_FINISH, false);
	ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, nullptr);

	// 작업 시스템에 명령을 요청합니다.
	m_spToDoSystem->RequestCommandFromSlavePal();
	return S_OK;
}

HRESULT Client::CMonster::PauseToDoCommand()
{
	if (m_wpToDoCommand.expired()) { return S_OK; }

	m_wpToDoCommand.lock()->m_eToDoCommandState = EToDoCommandState::PAUSED;
	EToDoType eToDoType = m_wpToDoCommand.lock()->m_eToDoType;

	switch (eToDoType)
	{
	case Client::EToDoType::BUILD_ARCHITECTURE:
	case Client::EToDoType::PRODUCE_ITEM:
	case Client::EToDoType::CUT_DOWN_LUMBERYARD:
	case Client::EToDoType::MINE_STONE_PIT:
	case Client::EToDoType::CUT_DOWN_RAW_TREE:
	case Client::EToDoType::MINE_RAW_ROCK:
	case Client::EToDoType::FARM_BERRY:
	case Client::EToDoType::COOK_FOOD:
		m_wpToDoCommand.lock()->m_spHabitat->RemoveWorkingSlavePal(m_iGameObjectID);
		break;

	case Client::EToDoType::TRANSPORT_INGREDIENT:
	{
		shared_ptr<CIngredient> spIngredient = dynamic_pointer_cast<CIngredient>(m_wpToDoCommand.lock()->m_spItem);
		spIngredient->SetGravity(true);// 중력을 복구합니다.
		spIngredient->RemoveWorkingSlavePal();
	}
	break;

	case Client::EToDoType::TRANSPORT_MINERAL:
	{
		shared_ptr<CMineral> spMineral = dynamic_pointer_cast<CMineral>(m_wpToDoCommand.lock()->m_spItem);
		spMineral->SetGravity(true);// 중력을 복구합니다.
		spMineral->RemoveWorkingSlavePal();
	}
	break;

	default:
		CHECKF(false, L"todo type is not designated.");
		break;
	}

	// work ui를 꺼줍니다.
	SetActivePalWorkPanel(false);

	m_wpToDoCommand.reset();
	m_eMonsterToDoState = EMonsterToDoState::REST;
	
	ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_START, false);
	ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_FINISH, false);
	ModifyGameObjectValue(EBlackboardGameObjectValue::TODO_TARGET, nullptr);

	// 작업 시스템에 명령을 반환합니다.
	m_spToDoSystem->PauseCommandFromSlavePal(eToDoType);
	return S_OK;
}

bool Client::CMonster::ExecuteToDoCommand()
{
	// 작업이 플레이어 혹은 다른 팰에 의해 완료된 경우
	if (m_wpToDoCommand.expired()) { return true; }

	if (m_wpToDoCommand.lock()->m_eToDoCommandState == EToDoCommandState::FINISHED) { return true; }
	
	float fToDoRatio = 0.0f;
	float fCreepSpeed = 0.0f;

	if (GetBoolValue(EBlackboardBoolValue::BOOL_HUNGER))
	{
		fToDoRatio = CClientLoop::s_spClientDataSheet->k_mapPalHungerToDoRatios.at(m_eMonsterType);
		fCreepSpeed = CClientLoop::s_spClientDataSheet->k_mapPalDefaultCreepSpeeds[m_eMonsterType] * 0.5f;
	}
	else
	{
		fToDoRatio = CClientLoop::s_spClientDataSheet->k_mapPalDefaultToDoRatios.at(m_eMonsterType);
		fCreepSpeed = CClientLoop::s_spClientDataSheet->k_mapPalDefaultCreepSpeeds[m_eMonsterType];
	}

	switch (m_wpToDoCommand.lock()->m_eToDoType)
	{
	case EToDoType::COOK_FOOD:
	case EToDoType::PRODUCE_ITEM:
	{
		// 함께 작업하는 다른 팰도 알 수 있도록 작업이 완료되었음을 명시합니다.
		EProduceItemState eProduceItemState = m_wpFactory.lock()->ProduceItem(fToDoRatio * m_fToDoDeltaSeconds);
		if (eProduceItemState == EProduceItemState::COMPLETED)
		{
			m_wpToDoCommand.lock()->m_eToDoCommandState = EToDoCommandState::FINISHED;

			// 아이템은 플레이어가 F키를 눌렀을 때, 명령을 제거합니다.
			return true;
		}
	}
	break;

	case EToDoType::BUILD_ARCHITECTURE:
	{
		if (bool bResult = m_wpArchitecture.lock()->BuildArchitecture(fToDoRatio * m_fToDoDeltaSeconds))
		{
			m_wpToDoCommand.lock()->m_eToDoCommandState = EToDoCommandState::FINISHED;
			
			// 명령을 제거합니다.
			m_wpArchitecture.lock()->UnRegisterCommandFromSystem(EToDoType::BUILD_ARCHITECTURE);
			return true;
		}
	}
	break;

	case EToDoType::MINE_RAW_ROCK:
	case EToDoType::CUT_DOWN_RAW_TREE:
	{
		if (bool bResult = m_wpNature.lock()->DestroyNature(fToDoRatio * m_fToDoDeltaSeconds))
		{
			// TODO; 명령에서 빼기
			m_wpToDoCommand.lock()->m_eToDoCommandState = EToDoCommandState::FINISHED;
			return true;
		}
	}
	break;

	case EToDoType::TRANSPORT_INGREDIENT:
	case EToDoType::TRANSPORT_MINERAL:
	{
		// 아이템을 팰 운반 본에 붙입니다.
		if (!m_wpTransportBone.expired())
		{
			Matrix matWorld = GetTransformMatrix().value();
			m_wpItem.lock()->TransportItem(m_wpTransportBone.lock(), matWorld);
		}

		Vector3 vChestBoxPosition = m_wpChestBox.lock()->GetPosition().value();
		Vector3 vMonsterPosition = GetPosition().value();
		Vector3 vDirection = vChestBoxPosition - vMonsterPosition;
		vDirection.Normalize();

		LookAt(vMonsterPosition + vDirection);
		MoveForward(CClientLoop::s_spClientDataSheet->k_mapPalDefaultCreepSpeeds[m_eMonsterType]);
		
		Vector3 vExtents = m_wpChestBox.lock()->GetColliderExtents();
		float fMaxAcceptableRadius = max(vExtents.x, vExtents.z) + GetAcceptableRadiusFromColliderComponent() + 0.2f;

		float fDistance = Vector3::Distance(vMonsterPosition, vChestBoxPosition);
		if (fDistance <= fMaxAcceptableRadius)
		{
			m_wpItem.lock()->SetCulled(true);
			ModifyBoolValue(EBlackboardBoolValue::BOOL_TODO_FINISH, true);

			// 아이템을 상자에 넣습니다.
			if (m_wpChestBox.lock()->GetArchitectureType() == EArchitectureType::WOODEN_CHEST)
			{
				static_pointer_cast<CChest>(m_wpChestBox.lock())->AddItemToWoodenChest(m_wpItem.lock()->GetItemType(), m_wpItem.lock()->GetNumItems());
			}
			else if (m_wpChestBox.lock()->GetArchitectureType() == EArchitectureType::IRON_BOX)
			{
				static_pointer_cast<CIronChest>(m_wpChestBox.lock())->AddItemToIronChest(m_wpItem.lock()->GetItemType(), m_wpItem.lock()->GetNumItems());
			}

			// 명령을 제거합니다.
			if (m_wpToDoCommand.lock()->m_eToDoType == EToDoType::TRANSPORT_INGREDIENT)
			{
				m_wpItem.lock()->UnRegisterCommandFromSystem(EToDoType::TRANSPORT_INGREDIENT);
			}
			else
			{
				m_wpItem.lock()->UnRegisterCommandFromSystem(EToDoType::TRANSPORT_MINERAL);
			}

			// 아이템을 메모리에서 해제합니다.
			return true;
		}
	}
	break;

	case EToDoType::CUT_DOWN_LUMBERYARD:
	case EToDoType::MINE_STONE_PIT:
	case EToDoType::FARM_BERRY:
	{
		// 벌목장, 채굴장, 농사는 작업이 절대로 완료되지 않습니다.
		EProduceItemState eProduceItemState = m_wpFactory.lock()->ProduceItem(fToDoRatio * m_fToDoDeltaSeconds);
		if (eProduceItemState == EProduceItemState::COMPLETED)
		{
			m_wpToDoCommand.lock()->m_eToDoCommandState = EToDoCommandState::FINISHED;
			return true;
		}
	}
	break;

	default:
		CHECKF(false, L"Todo type is not designated.");
		break;
	}

	return false;
}

void Client::CMonster::AddSlavePalToToDoSystem()
{
	// 거점 팰로 등록합니다.
	m_spToDoSystem->AddSlavePal(static_pointer_cast<CMonster>(shared_from_this()));

	// 작업을 요청합니다.
	m_spToDoSystem->RequestCommandFromSlavePal();
}

void Client::CMonster::RemoveSlavePalFromToDoSystem()
{
	// 작업을 정지합니다.
	PauseToDoCommand();

	// 거점 팰에서 제외합니다.
	m_spToDoSystem->RemoveSlavePal(static_pointer_cast<CMonster>(shared_from_this()));
}

HRESULT Client::CMonster::RequestCombatCommand()
{
	return m_spCombatSystem->CommandToPartnerPal();
}

HRESULT Client::CMonster::PariticipateInCombat()
{
	if (m_bCombat)
	{
		return S_OK;
	}
	else
	{
		if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
		{
			SetPalHPPanel();
		}
	}
	m_bCombat = true;

	ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, true);
	return m_spCombatSystem->AddCombatPal(static_pointer_cast<CMonster>(shared_from_this()));
}

HRESULT Client::CMonster::ParticipateOutCombat()
{
	if (!m_bCombat)
	{
		return S_OK;
	}
	else
	{
		if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
		{
			ReturnPalHPPanel();
		}
	}
	m_bCombat = false;

	ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, false);
	return m_spCombatSystem->RemoveCombatPal(m_iGameObjectID);
}

std::shared_ptr<CArchitecture> Client::CMonster::FindArchitecture(EArchitectureType _eArchitectureType)
{
	return m_spToDoSystem->FindArchitecture(_eArchitectureType);
}

void Client::CMonster::VacantPalBed()
{
	if (!m_wpPalBed.expired())
	{
		m_wpPalBed.lock()->SetOccupied(false);
	}
	m_wpPalBed.reset();
}

void Client::CMonster::OccupyPalBed(const std::shared_ptr<CPalBed>& _spPalBed)
{
	m_wpPalBed = _spPalBed;
}

void Client::CMonster::AddItemToChestBox(const std::shared_ptr<CChest>& _spChest)
{
	EItemType eItemType = m_wpToDoCommand.lock()->m_spItem->GetItemType();
	int32 iNumItems = m_wpToDoCommand.lock()->m_spItem->GetNumItems();

	_spChest->AddItemToWoodenChest(eItemType, iNumItems);
}

void Client::CMonster::RetrieveToPalSphere()
{
	// 전투에서 제외합니다.
	ParticipateOutCombat();

	ReturnPalHPPanel();

	// BT를 초기화합니다.
	m_spBehaviorTreeComponent->ResetBTNodeResult();

	// 모든 소리를 끕니다.
	StopSoundAll();

	// 팰 회수 이펙트를 출력합니다.
	m_wpEffectManager.lock()->ActivePalSummonParticle(shared_from_this(), GetMeshComponent()->GetModel());

	// 몬스터를 보이지 않는 곳으로 이동합니다.
	SetPosition({ 0.0f, -100.0f, 0.0f });
	SetActive(false);
}

HRESULT Client::CMonster::ReInitializePalBT(EMonsterAIType _eMonsterAIType)
{
	m_eMonsterAIType = _eMonsterAIType;
	if (EMonsterAIType::SLAVE_PAL  == m_eMonsterAIType)
	{
		SetPalWorkPanel();
	}

	return S_OK;
}

void Client::CMonster::InteractWithSlavePal(EMonsterInteractType _eMonsterInteractType)
{
	ModifyBoolValue(EBlackboardBoolValue::BOOL_INTERACTED, true);
	ModifyIntValue(EBlackboardIntValue::INT_INTERACT_TYPE, (int32)_eMonsterInteractType);
}

float Client::CMonster::GetAcceptableRadiusFromColliderComponent()
{
	float fRadius = 0.0f;

	shared_ptr<CColliderComponent> spColliderComponent = GetBodyColliderComponent();
	switch (spColliderComponent->GetComponentType())
	{
	case EComponentType::SPHERE:
	{
		shared_ptr<CSphereComponent> spSphereComponent = static_pointer_cast<CSphereComponent>(spColliderComponent);
		fRadius = spSphereComponent->GetRadius();
	}
	break;

	case EComponentType::BOX:
	{
		shared_ptr<CBoxComponent> spBoxComponent = static_pointer_cast<CBoxComponent>(spColliderComponent);
		Vector3 vExtents = spBoxComponent->GetExtents();
		fRadius = max(vExtents.x, vExtents.z);
	}
	break;

	default:
		CHECKF(false, L"Component is not collider component.");
		break;
	}

	return fRadius;
}

void Client::CMonster::GrabTool(EItemType _eItemType, bool _bGrab)
{
	if (!m_umapTools.contains(_eItemType)) { return; }

	m_umapTools[_eItemType]->SetActive(_bGrab);
}

void Client::CMonster::ChangeEye(EPalEyeType _ePalEyeType)
{
	if (!m_spMeshComponent) { return; }

	int32 iX, iY = -1;
	switch (_ePalEyeType)
	{
	case Client::EPalEyeType::DEFAULT:	iX = 0;	iY = 0;	break;
	case Client::EPalEyeType::TIRED:	iX = 0; iY = 1; break;
	case Client::EPalEyeType::CLOSED:	iX = 0;	iY = 2;	break;
	case Client::EPalEyeType::DAMAGED:	iX = 0;	iY = 3;	break;
	case Client::EPalEyeType::SMIZED:	iX = 1;	iY = 0;	break;
	case Client::EPalEyeType::ANGRY:	iX = 1;	iY = 1;	break;
	case Client::EPalEyeType::CRY:		iX = 1;	iY = 2;	break;
	case Client::EPalEyeType::FAINT:	iX = 1;	iY = 3;	break;
	default:							iX = 0;	iY = 0; break;
	}

	auto& umapMeshInfos = m_spMeshComponent->GetModel()->GetMeshInfos();
	for (auto& prShaderType : umapMeshInfos)
	{
		if (prShaderType.first == EShaderType::MONSTERFACE_ANIM)
		{
			for (auto& spMeshInfo : prShaderType.second)
			{
				shared_ptr<CMaterial> spMateial = spMeshInfo->m_spMeshBuffer->GetMaterial();
				if (spMateial->GetName().find(L"Eye") != wstring::npos)
				{
					spMateial->SetInt(L"Index X", iX);
					spMateial->SetInt(L"Index Y", iY);
				}
			}
		}
	}
}

void Client::CMonster::ChangeMouth(EPalMouthType _ePalMouthType)
{
	if (!m_spMeshComponent) { return; }

	int32 iX, iY = -1;
	switch (_ePalMouthType)
	{
	case Client::EPalMouthType::DEFAULT:iX = 0;	iY = 0;	break;
	case Client::EPalMouthType::DAMAGED:iX = 0;	iY = 1;	break;
	case Client::EPalMouthType::SMILE:	iX = 1;	iY = 0;	break;
	case Client::EPalMouthType::ANGRY:	iX = 1;	iY = 1;	break;
	default:							iX = 0;	iY = 0; break;
	}

	auto& umapMeshInfos = m_spMeshComponent->GetModel()->GetMeshInfos();
	for (auto& prShaderType : umapMeshInfos)
	{
		if (prShaderType.first == EShaderType::MONSTERFACE_ANIM)
		{
			for (auto& spMeshInfo : prShaderType.second)
			{
				shared_ptr<CMaterial> spMateial = spMeshInfo->m_spMeshBuffer->GetMaterial();
				if (spMateial->GetName().find(L"Mouth") != wstring::npos)
				{
					spMateial->SetInt(L"Index X", iX);
					spMateial->SetInt(L"Index Y", iY);
				}
			}
		}
	}
}

void Client::CMonster::StopSoundAll()
{
	for (auto& pair : m_umapBBChannels)
	{
		for (auto& eChannelType : pair.second)
		{
			if (eChannelType == EChannelType::ENUM_END) { continue; }
			
			m_wpSoundManager.lock()->StopChannelEx(eChannelType);
			eChannelType = EChannelType::ENUM_END;
		}
	}
}

void Client::CMonster::TakeDamage(const std::shared_ptr<CGameObject>& _spInstigator, float _fDamage)
{
	// 거점 팰은 공격받지 않습니다.
	if (m_eMonsterAIType == EMonsterAIType::SLAVE_PAL) { return; }

	// 이미 몬스터가 죽은 경우
	if (m_bDead) { return; }

	// 파트너 팰은 공격은 받지만, 대미지는 받지 않습니다.
	if (m_eMonsterAIType == EMonsterAIType::PARTNER_PAL)
	{
		ModifyBoolValue(EBlackboardBoolValue::BOOL_DAMAGED, true);
		ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, true);
		ModifyGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET, _spInstigator);
		return;
	}

	CCreature::TakeDamage(_spInstigator, _fDamage);

	if (m_fCurHp <= 0.0f)
	{
		switch (m_eMonsterType)
		{
		case Client::EMonsterType::BERRY_GOAT:
		case Client::EMonsterType::BOAR:
			m_spPlayer->AddExp(20.0f);
			break;

		case Client::EMonsterType::DREAM_DEMON:
		case Client::EMonsterType::HEDGE_HOG:
		case Client::EMonsterType::KITSUNEBI:
		case Client::EMonsterType::PENGUIN:
		case Client::EMonsterType::PINK_CAT:
		case Client::EMonsterType::SHEEP_BALL:
		case Client::EMonsterType::WOOL_FOX:
			m_spPlayer->AddExp(10.0f);
			break;

		case Client::EMonsterType::ELEC_PANDA:
		case Client::EMonsterType::GRASS_MAMMOTH:
			m_spPlayer->AddExp(50.0f);
			break;

		default:
			break;
		}

		m_fCurHp = 0.0f;
		m_bDead = true;

		ModifyBoolValue(EBlackboardBoolValue::BOOL_DEAD, true);
		ModifyBoolValue(EBlackboardBoolValue::BOOL_DAMAGED, false);
		ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, false);

		// 전투 중인 야생 팰에서 제외합니다.
		if (m_spPalHPPanelInfo->m_spPalHPPanel)
		{
			ReturnPalHPPanel();
		}

		ENSUREF(m_spCombatSystem->RemoveCombatPal(GetGameObjectID()), L"Failed to remove combat pal.");
		return;
	}

	if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
	{
		ModifyBoolValue(EBlackboardBoolValue::BOOL_DAMAGED, true);
		ModifyBoolValue(EBlackboardBoolValue::BOOL_AGGRO, true);
		ModifyGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET, _spInstigator);
		ENSUREF(PariticipateInCombat(), L"Failed to participate in combat.");
	}
}

std::tuple<bool, int32> Client::CMonster::ComputeCaptureProbability()
{
	float fRatio = (m_fMaxHp - m_fCurHp) / m_fMaxHp * 0.85f;
	m_iCapturePercentage = static_cast<int32>(100.0f * fRatio);

	m_bCaptureResult = CMathUtility::GetRandomBool(fRatio);
	int32 iNumShakes = CMathUtility::GetRandomInt(2, 4);

	switch (m_eMonsterType)
	{
	case Client::EMonsterType::ELEC_PANDA:
	case Client::EMonsterType::GRASS_MAMMOTH:
	case Client::EMonsterType::HEDGE_HOG_ICE:
		m_bCaptureResult = false;
		break;

	default:
		// 임시로 포획되는 것을 확인합니다.
		break;
	}

	if (m_bCaptureResult)
	{
		ModifyGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET, nullptr);

		// 체력을 초기화합니다.
		m_fCurHp = m_fMaxHp;
	}
	else
	{
		ModifyGameObjectValue(EBlackboardGameObjectValue::ATTACK_TARGET, m_spPlayer);
	}

	ModifyBoolValue(EBlackboardBoolValue::BOOL_CAPTURED, true);
	ModifyBoolValue(EBlackboardBoolValue::BOOL_CAPTURE_RESULT, m_bCaptureResult);
	return { m_bCaptureResult, iNumShakes };
}

void Client::CMonster::InitBlackboardValues()
{
	// bool values
	{
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_AGGRO,			false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_ATTACK,		false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_CAPTURED,		false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_CAPTURE_RESULT,false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_DAMAGED,		false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_DAY,			false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_DEAD,			false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_EMOTE,			false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_ESCAPE,		false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_FEEDED,		false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_HUNGER,		false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_HOLD_POSITION,	false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_INTERACTED,	false);
		
		switch (m_eMonsterAIType)
		{
		case Client::EMonsterAIType::WILD_PAL:		
		case Client::EMonsterAIType::SLAVE_PAL:
			m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_JUMP, true);
			break;
		
		case Client::EMonsterAIType::PARTNER_PAL:
			m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_JUMP, false);
			break;

		default:
			CHECKF(false, L"Monster AI type is not designated.");
			break;
		}

		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_PARTNER_SKILL,	false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_REACT,			false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_RUNAWAY,		false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_SLEEP,			false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_TODO_START,	false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_TODO_PAUSE,	false);
		m_umapBBBools.emplace(EBlackboardBoolValue::BOOL_TODO_FINISH,	false);
	}

	// int values
	{
		m_umapBBInts.emplace(EBlackboardIntValue::INT_INTERACT_TYPE, (int32)EMonsterInteractType::ENUM_END);
		m_umapBBInts.emplace(EBlackboardIntValue::INT_NUM_SHAKES, 0);
		m_umapBBInts.emplace(EBlackboardIntValue::INT_SKILL_TYPE, -1);
	}

	// float values
	{
		m_umapBBFloats.emplace(EBlackboardFloatValue::FLOAT_DELTA_SECONDS, 0.0f);
		m_umapBBFloats.emplace(EBlackboardFloatValue::FLOAT_ACCEPTABLE_RADIUS, 0.0f);
	}

	// Vector3 values
	{

	}

	// Gameobject values
	{
		m_umapBBGameObjects.emplace(EBlackboardGameObjectValue::ATTACK_TARGET,		nullptr);
		m_umapBBGameObjects.emplace(EBlackboardGameObjectValue::PLAYER_TARGET,		m_spPlayer);
		m_umapBBGameObjects.emplace(EBlackboardGameObjectValue::TODO_TARGET,		nullptr);
		m_umapBBGameObjects.emplace(EBlackboardGameObjectValue::EFFECT_GROUP_00,	nullptr);
		m_umapBBGameObjects.emplace(EBlackboardGameObjectValue::EFFECT_GROUP_01,	nullptr);
		m_umapBBGameObjects.emplace(EBlackboardGameObjectValue::EFFECT_GROUP_02,	nullptr);
		m_umapBBGameObjects.emplace(EBlackboardGameObjectValue::EFFECT_GROUP_03,	nullptr);
	}
}

void Client::CMonster::ResetBlackboardValues(EBlackboardValueType _eBlackboardValueType)
{
	switch (_eBlackboardValueType)
	{
	case Engine::EBlackboardValueType::BOOL:
		break;

	case Engine::EBlackboardValueType::INT:
		break;

	case Engine::EBlackboardValueType::FLOAT:
		break;

	case Engine::EBlackboardValueType::VECTOR3:
		break;

	case Engine::EBlackboardValueType::GAME_OBJECT:
		for (auto& pair : m_umapBBGameObjects) { pair.second = nullptr; }
		break;

	default:
		CHECKF(false, L"Blackboard value type is not designated.");
		break;
	}
}

void Client::CMonster::UpdateBlackboardValues()
{
	if (m_bDead)
	{
		ModifyBoolValue(EBlackboardBoolValue::BOOL_DEAD, true);
	}

	if (m_spDate->IsDay())
	{
		ModifyBoolValue(EBlackboardBoolValue::BOOL_DAY, true);
	}

	if (m_spDate->IsNight())
	{
		ModifyBoolValue(EBlackboardBoolValue::BOOL_DAY, false);
		ModifyBoolValue(EBlackboardBoolValue::BOOL_SLEEP, true);
	}

	if (m_fCurHungerGauge <= 10.0f)
	{
		// ModifyBoolValue(EBlackboardBoolValue::BOOL_HUNGER, true);
	}
}

void Client::CMonster::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 컨트롤러 컴포넌트
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		return;
	}
}

void Client::CMonster::OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CColliderComponent* pDstColliderComponent = static_cast<CColliderComponent*>(_pDstComponent);
	CColliderComponent* pSrcColliderComponent = static_cast<CColliderComponent*>(_pSrcComponent);

	if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::MONSTER &&
		(pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT) || (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT2))
	{
		m_bJump = false;
	}

	else if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::BOSS &&
		(pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT) || (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT2))
	{
		m_bJump = false;
	}

	else if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::ALLY &&
		(pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT) || (pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::ENVIRONMENT2))
	{
		m_bJump = false;
	}
}

void Client::CMonster::OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		return;
	}
}

void Client::CMonster::SetPalWorkPanel()
{
	m_prWorkPanel.first = m_wpUIManager.lock()->GetUsingPalWorkUI();
	m_prWorkPanel.second = m_prWorkPanel.first->FindComponent<CImageComponent>(L"ItemImage");

	m_prWorkPanel.first->SetActive(false);
}

void Client::CMonster::SetPalWorkPanelImage(EToDoType _eTodoType)
{
	if (nullptr == m_prWorkPanel.first) { return; }
	m_prWorkPanel.second->SetTextureFileName(m_wpUIManager.lock()->FindPalWorkTextureName(_eTodoType));
}

void Client::CMonster::SetActivePalWorkPanel(bool _bActive)
{
	if (nullptr == m_prWorkPanel.first) { return; }
	m_prWorkPanel.first->SetActive(_bActive);
}

void Client::CMonster::ReturnPalWorkPanel()
{
	if (nullptr == m_prWorkPanel.first) { return; }
	m_wpUIManager.lock()->ReturnPalWorkUI(m_prWorkPanel.first);
	m_prWorkPanel.first = nullptr;
	m_prWorkPanel.second = nullptr;
}

void Client::CMonster::SetUISize()
{
	if (EMonsterType::ELEC_PANDA == m_eMonsterType ||
		EMonsterType::GRASS_MAMMOTH == m_eMonsterType)
	{
		return;
	}

	switch (m_eMonsterSizeType)
	{
	case Client::EMonsterSizeType::LARGE:
		m_vUISize.y *= 3.0f;
		break;
	case Client::EMonsterSizeType::MIDUM:
		m_vUISize.y *= 2.5f;
		break;
	case Client::EMonsterSizeType::SMALL:
		m_eMonsterType == EMonsterType::DREAM_DEMON ? m_vUISize.y *= 2.5f : m_vUISize.y *= 1.3f;
		break;
	default:
		CHECKF(false, L"Monster personality type is not designated.");
		break;
	}
}

void Client::CMonster::SetPalHPPanel()
{
	if (EMonsterType::GRASS_MAMMOTH == m_eMonsterType || EMonsterType::ELEC_PANDA == m_eMonsterType)
	{
		return;
	}

	m_spPalHPPanelInfo->m_spPalHPPanel = m_wpUIManager.lock()->GetUsingPalHPUI();

	// 구조체 정보를 채워줍니다.
	{
		m_spPalHPPanelInfo->m_spElementImage = m_spPalHPPanelInfo->m_spPalHPPanel->FindComponent<CImageComponent>(L"ElementImage");
		m_spPalHPPanelInfo->m_spHPProgress = m_spPalHPPanelInfo->m_spPalHPPanel->FindComponent<CProgressBarComponent>(L"HPProgressBar");
		m_spPalHPPanelInfo->m_spNameBackImage = m_spPalHPPanelInfo->m_spPalHPPanel->FindComponent<CImageComponent>(L"NameBackImage");
		m_spPalHPPanelInfo->m_spLevelText = m_spPalHPPanelInfo->m_spPalHPPanel->FindComponent<CTextComponent>(L"LevelText");
		m_spPalHPPanelInfo->m_spNameText = m_spPalHPPanelInfo->m_spPalHPPanel->FindComponent<CTextComponent>(L"NameText");
	}

	// 컴포넌트의 기본값을 채워줍니다.
	{
		m_spPalHPPanelInfo->m_spElementImage->SetTextureFileName(m_wpUIManager.lock()->FindPalElementName(CClientLoop::s_spClientDataSheet->k_mapPalElementTypes.at(m_eMonsterType)));

		// 현재 체력에 근거하여 비율을 설정해줍니다.
		{
			float fRatio = (m_fMaxHp - m_fCurHp) / m_fMaxHp;
			m_spPalHPPanelInfo->m_spHPProgress->SetOtherRatio(1.0f - fRatio);
			m_spPalHPPanelInfo->m_spHPProgress->SetRatio(1.0f - fRatio);
		}

		//m_spPalHPPanelInfo->m_spHPProgress->SetRatio(1.f);
		//m_spPalHPPanelInfo->m_spHPProgress->SetOtherRatio(1.f);
		//
		if (EMonsterAIType::WILD_PAL == m_eMonsterAIType)
		{
			m_spPalHPPanelInfo->m_spHPProgress->SetColor(m_spPalHPPanelInfo->vRedColor);
			m_spPalHPPanelInfo->m_spNameBackImage->SetColor(m_spPalHPPanelInfo->vRedColor);
		}
		else
		{
			m_spPalHPPanelInfo->m_spHPProgress->SetColor(m_spPalHPPanelInfo->vGreenColor);
			m_spPalHPPanelInfo->m_spNameBackImage->SetColor(m_spPalHPPanelInfo->vBlueColor);
		}

		m_spPalHPPanelInfo->m_spLevelText->SetText(to_wstring(m_iMonsterLevel));
		m_spPalHPPanelInfo->m_spNameText->SetText(m_wpUIManager.lock()->FindPalName(m_eMonsterType));
	}
}

void Client::CMonster::ReturnPalHPPanel()
{
	if (!m_spPalHPPanelInfo->m_spPalHPPanel) { return; }
	m_wpUIManager.lock()->ReturnPalHPUI(m_spPalHPPanelInfo->m_spPalHPPanel);

	// 구조체 값들을 초기화해줍니다.
	{
		m_spPalHPPanelInfo->m_spPalHPPanel = nullptr;
		m_spPalHPPanelInfo->m_spElementImage = nullptr;
		m_spPalHPPanelInfo->m_spHPProgress = nullptr;
		m_spPalHPPanelInfo->m_spNameBackImage = nullptr;
		m_spPalHPPanelInfo->m_spLevelText = nullptr;
		m_spPalHPPanelInfo->m_spNameText = nullptr;
	}
}

void Client::CMonster::ChangeHPColor(EMonsterAIType _eMonsterAIType)
{
	if (EMonsterAIType::WILD_PAL == _eMonsterAIType)
	{
		m_spPalHPPanelInfo->m_spHPProgress->SetColor(m_spPalHPPanelInfo->vRedColor);
		m_spPalHPPanelInfo->m_spNameBackImage->SetColor(m_spPalHPPanelInfo->vRedColor);
	}
	else
	{
		m_spPalHPPanelInfo->m_spHPProgress->SetColor(m_spPalHPPanelInfo->vGreenColor);
		m_spPalHPPanelInfo->m_spNameBackImage->SetColor(m_spPalHPPanelInfo->vBlueColor);
	}
}

void Client::CMonster::SetActive(bool _bActive)
{
	if (_bActive && m_bDead) { return; }

	CGameObject::SetActive(_bActive);
}
