// default
#include "pch.h"
#include "PinkCat.h"

// engine manager
#include "EngineModule.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "TimerManager.h"
#include "SoundManager.h"

// client manager
#include "GameManager.h"
#include "UIManager.h"
#include "EffectManager.h"
#include "ClientLoop.h"

// component
#include "BehaviorTreeComponent.h"
#include "BoxComponent.h"
#include "ControllerComponent.h"
#include "ColliderComponent.h"
#include "MeshComponent.h"
#include "SphereComponent.h"
#include "TextComponent.h"
#include "TrailComponent.h"

// system
#include "ToDoSystem.h"

// engine
#include "Level.h"
#include "Date.h"
#include "Model.h"
#include "Panel.h"

// composite node
#include "CompositeNode.h"

// decoreator node
#include "DecoratorBBBool.h"
#include "DecoratorInteracted.h"
#include "DecoratorSleep.h"
#include "DecoratorPartnerSkill.h"
#include "DecoratorDay.h"
#include "DecoratorNight.h"
#include "DecoratorNear.h"
#include "DecoratorHunger.h"

// task node
#include "TaskAttackStart.h"
#include "TaskAttackLoop.h"
#include "TaskAttackEnd.h"
#include "TaskCaptured.h"
#include "TaskChase.h"
#include "TaskDamaged.h"
#include "TaskDead.h"
#include "TaskEncount.h"
#include "TaskEmote.h"
#include "TaskFollow.h"
#include "TaskHoldPosition.h"
#include "TaskIdle.h"
#include "TaskNear.h"
#include "TaskNotice.h"
#include "TaskMove.h"
#include "TaskLookAt.h"
#include "TaskReact.h"
#include "TaskRest.h"
#include "TaskRunaway.h"
#include "TaskSleepStart.h"
#include "TaskSleepLoop.h"
#include "TaskSleepEnd.h"
#include "TaskToDo.h"
#include "TaskTurn.h"
#include "TaskJumpLoop.h"
#include "TaskJumpEnd.h"
#include "TaskWithdraw.h"

// gameobject
#include "Player.h"
#include "Factory.h"
#include "Architecture.h"
#include "Weapon.h"

std::shared_ptr<CPinkCat> Client::CPinkCat::Create(const std::shared_ptr<FPinkCatDesc>& _spPinkCatDesc)
{
	const std::shared_ptr<CPinkCat> spPinkCat = make_shared<CPinkCat>();
	ENSUREF(spPinkCat->Initialize(_spPinkCatDesc), L"Failed to initialize pinkcat");
	return spPinkCat;
}

HRESULT Client::CPinkCat::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CMonster::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FPinkCatDesc> spPinkcatDesc = static_pointer_cast<FPinkCatDesc>(_spGameObjectDesc);
	InitPinkCat(spPinkcatDesc);
	InitMeshComponent();
	InitColliderComponent();
	InitBehaviorTreeComponent(spPinkcatDesc);
	return S_OK;
}

HRESULT Client::CPinkCat::BeginPlay()
{
	if (FAILED(CMonster::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CPinkCat::PreTick(float _fDeltaSeconds)
{
	switch (CMonster::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPinkCat::Tick(float _fDeltaSeconds)
{
	switch (CMonster::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPinkCat::PostTick(float _fDeltaSeconds)
{
	switch (CMonster::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CPinkCat::EndPlay()
{
	if (FAILED(CMonster::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CPinkCat::Release()
{
	m_spBodySphereComponent = nullptr;
	m_spAttackSphereComponent = nullptr;

	CMonster::Release();
}

void Client::CPinkCat::InitPinkCat(const shared_ptr<FPinkCatDesc>& _spPinkcatDesc)
{
	// gameobject desc
	m_wstrGameObjectName = L"PinkCat";
	m_eMobilityType = EMobilityType::DYNAMIC;

	// monster desc
	m_eMonsterType = EMonsterType::PINK_CAT;
	m_eMonsterSizeType = EMonsterSizeType::SMALL;
}

void Client::CPinkCat::InitMeshComponent()
{
	{
		// component desc
		FMeshComponentDesc tMeshComponentDesc;
		tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";
		tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

		// mesh component desc
		tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/PinkCat.dat";
		tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(180.f));
		tMeshComponentDesc.m_wstrShaderName = L"";

		m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
		ENSURE(AddComponent(tMeshComponentDesc.m_wstrComponentName, m_spMeshComponent));
	}

	{
		// gameobject desc
		FWeaponDesc tMeleeDesc;
		tMeleeDesc.m_wstrGameObjectName = L"PalConstructionHammer";

		// item desc
		tMeleeDesc.m_eCurItemType = EItemType::CONSTRUCT_HAMMER;
		tMeleeDesc.m_eCurItemKindType = EItemKindType::WEAPON;

		// weapon desc
		tMeleeDesc.m_bIsAnimed = false;
		tMeleeDesc.m_eWeaponActionType = EWeaponActionType::MELEE;
		tMeleeDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeleeDesc.m_eWeaponType = EWeaponType::ENUM_END;

		// mesh component desc
		tMeleeDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/NonAnim/Weapon/Weapon_ConstructionTool_001.dat";
		tMeleeDesc.m_matPivot = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		tMeleeDesc.m_spAttachedBone = m_spMeshComponent->GetBonePtr(L"Socket_Weapon_R");

		m_umapTools.insert({ EItemType::CONSTRUCT_HAMMER, CWeapon::Create(make_shared<FWeaponDesc>(tMeleeDesc)) });
		AddChildGameObject(m_umapTools[EItemType::CONSTRUCT_HAMMER]);

		m_umapTools[EItemType::CONSTRUCT_HAMMER]->SetActive(false);
	}

	// 운반 본 포인터를 구합니다.
	m_wpTransportBone = m_spMeshComponent->GetBonePtr(L"Socket_Transport");
}

void Client::CPinkCat::InitColliderComponent()
{
	// body sphere component
	{
		// component desc
		FSphereComponentDesc tSphereComponentDesc;
		tSphereComponentDesc.m_eComponentType		= EComponentType::SPHERE;
		tSphereComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tSphereComponentDesc.m_wstrComponentName	= L"BodySphereComponent";
		tSphereComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

		// collider component desc
		tSphereComponentDesc.m_bTrigger				= false;
		tSphereComponentDesc.m_bGravity				= true;
		tSphereComponentDesc.m_eColliderType		= EColliderType::SPHERE;
		tSphereComponentDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tSphereComponentDesc.m_bDeferredAdd			= true;

		switch (m_eMonsterAIType)
		{
		case Client::EMonsterAIType::WILD_PAL:
			tSphereComponentDesc.m_eCollisionFlag	= ECollisionFlag::MONSTER;
			tSphereComponentDesc.m_eCollisionLayer	= ECollisionLayer::MONSTER;
			break;

		case Client::EMonsterAIType::PARTNER_PAL:
		case Client::EMonsterAIType::SLAVE_PAL:
			tSphereComponentDesc.m_eCollisionFlag	= ECollisionFlag::ALLY;
			tSphereComponentDesc.m_eCollisionLayer	= ECollisionLayer::ALLY;
			break;

		default:
			CHECKF(false, L"Monster AI type is not designated.");
			break;
		}

		tSphereComponentDesc.m_fMass				= 10.0f;
		tSphereComponentDesc.m_iAxisLockType		= (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
		tSphereComponentDesc.m_vLocalPosition		= Vector3(0.0f, 0.5f, 0.0f);
		tSphereComponentDesc.m_vMaterial			= Vector3(1.0f, 1.0f, 0.05f);

		// sphere component desc
		tSphereComponentDesc.m_fRadius = 0.5f;

		m_spBodySphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		ENSURE(AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spBodySphereComponent));
	}

	// attack sphere component
	{
		// component desc
		FSphereComponentDesc tSphereComponentDesc;
		tSphereComponentDesc.m_eComponentType		= EComponentType::SPHERE;
		tSphereComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tSphereComponentDesc.m_wstrComponentName	= L"AttackSphereComponent";
		tSphereComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

		// collider component desc
		tSphereComponentDesc.m_bTrigger				= true;
		tSphereComponentDesc.m_bGravity				= false;
		tSphereComponentDesc.m_eColliderType		= EColliderType::SPHERE;
		tSphereComponentDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tSphereComponentDesc.m_bDeferredAdd			= true;

		switch (m_eMonsterAIType)
		{
		case Client::EMonsterAIType::WILD_PAL:
			tSphereComponentDesc.m_eCollisionFlag = ECollisionFlag::MONSTER_WEAPON;
			tSphereComponentDesc.m_eCollisionLayer = ECollisionLayer::MONSTER_WEAPON;
			break;

		case Client::EMonsterAIType::PARTNER_PAL:
		case Client::EMonsterAIType::SLAVE_PAL:
			tSphereComponentDesc.m_eCollisionFlag = ECollisionFlag::ALLY_WEAPON;
			tSphereComponentDesc.m_eCollisionLayer = ECollisionLayer::ALLY_WEAPON;
			break;

		default:
			CHECKF(false, L"Monster AI type is not designated.");
			break;
		}

		tSphereComponentDesc.m_fMass				= 0.0f;
		tSphereComponentDesc.m_iAxisLockType		= (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
		tSphereComponentDesc.m_vLocalPosition		= Vector3(0.0f, 0.4f, 0.0f);
		tSphereComponentDesc.m_vMaterial			= Vector3(1.0f, 1.0f, 0.05f);

		// sphere component desc
		tSphereComponentDesc.m_fRadius				= 0.8f;

		m_spAttackSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		ENSURE(AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spAttackSphereComponent));
	}
}

void Client::CPinkCat::InitBehaviorTreeComponent(const std::shared_ptr<FPinkCatDesc>& _spPinkcatDesc)
{
	// 1. 야생 팰 비헤이비어 트리
	if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
	{
		InitializeWildPalBT(_spPinkcatDesc);
	}

	// 2. 파트너 팰 비헤이비어 트리
	else if (m_eMonsterAIType == EMonsterAIType::PARTNER_PAL)
	{
		InitializePartnerPalBT(_spPinkcatDesc);
	}

	// 3. 거점 팰 비헤이비어 트리
	else if (m_eMonsterAIType == EMonsterAIType::SLAVE_PAL)
	{
		InitializeSlavePalBT(_spPinkcatDesc);
	}

	// behavior tree desc
	m_spBehaviorTreeComponent->SetRootNode(m_spRootNode);
	m_spBehaviorTreeComponent->SetBehaveTree(true);
}

void Client::CPinkCat::InitializeWildPalBT(const std::shared_ptr<FPinkCatDesc> _spPinkcatDesc, bool _bInit)
{
	shared_ptr<CGameObject> spPinkCat = shared_from_this();

	// behavior tree component
	if (_bInit)
	{
		// component desc
		FBTComponentDesc tBTComponentDesc;
		tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
		tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
		tBTComponentDesc.m_eRenderType = ERenderType::NONE;
		tBTComponentDesc.m_wpOwnerGameObject = spPinkCat;

		m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
		AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
	}

	// behavior tree node
	{
		FCompositeNodeDesc tSelectorNodeDesc;
		tSelectorNodeDesc.m_spOwnerGameObject = spPinkCat;
		tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

		FCompositeNodeDesc tSequenceNodeDesc;
		tSequenceNodeDesc.m_spOwnerGameObject = spPinkCat;
		tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

		FDecoratorBBBoolDesc tDecoBBBoolDesc;
		tDecoBBBoolDesc.m_spOwnerGameObject = spPinkCat;
		tDecoBBBoolDesc.m_eBTNodeType = EBTNodeType::DECORATOR;

		// root selector node
		{
			tSelectorNodeDesc.m_bRoot = true;
			tSelectorNodeDesc.m_wstrBTNodeName = L"RootSelector";
			m_spRootNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSelectorNodeDesc));

			tSelectorNodeDesc.m_bRoot = false;
		}

		// dead task node
		{
			// node desc
			FTaskDeadDesc tTaskDeadDesc;
			tTaskDeadDesc.m_spOwnerGameObject = spPinkCat;
			tTaskDeadDesc.m_wstrBTNodeName = L"DeadTask";
			tTaskDeadDesc.m_eBTNodeType = EBTNodeType::TASK;

			// dead task desc
			tTaskDeadDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Carrying;
			m_spRootNode->AddBTNode(CTaskDead::Create(make_shared<FTaskDeadDesc>(tTaskDeadDesc)));
		}

		// captured selector node
		{
			tSelectorNodeDesc.m_wstrBTNodeName = L"CapturedSelector";
			shared_ptr<CCompositeNode> spCapturedSelectorNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSelectorNodeDesc));
			m_spRootNode->AddBTNode(spCapturedSelectorNode);

			// captured deco node
			{
				tDecoBBBoolDesc.m_wstrBTNodeName = L"CapturedDeco";
				tDecoBBBoolDesc.m_eBoolValue = EBlackboardBoolValue::BOOL_CAPTURED;
				spCapturedSelectorNode->AddBTNode(CDecoratorBBBool::Create(make_shared<FDecoratorBBBoolDesc>(tDecoBBBoolDesc)));
			}

			// captured task node
			{
				// task node desc
				FTaskCapturedDesc tTaskCapturedDesc;
				tTaskCapturedDesc.m_spOwnerGameObject = spPinkCat;
				tTaskCapturedDesc.m_wstrBTNodeName = L"CapturedTask";
				tTaskCapturedDesc.m_eBTNodeType = EBTNodeType::TASK;
				spCapturedSelectorNode->AddBTNode(CTaskCaptured::Create(make_shared<FTaskCapturedDesc>(tTaskCapturedDesc)));
			}
		}

		// jump sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"JumpSequence";
			shared_ptr<CCompositeNode> spJumpSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spJumpSequenceNode);

			// jump deco node
			{
				tDecoBBBoolDesc.m_wstrBTNodeName = L"JumpDeco";
				tDecoBBBoolDesc.m_eBoolValue = EBlackboardBoolValue::BOOL_JUMP;
				spJumpSequenceNode->AddBTNode(CDecoratorBBBool::Create(make_shared<FDecoratorBBBoolDesc>(tDecoBBBoolDesc)));
			}

			// jump loop task node
			{
				// task desc
				FTaskJumpLoopDesc tTaskJumpLoopDesc;
				tTaskJumpLoopDesc.m_spOwnerGameObject = spPinkCat;
				tTaskJumpLoopDesc.m_wstrBTNodeName = L"JumpLoopTask";
				tTaskJumpLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpLoopDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_JumpLoop;
				spJumpSequenceNode->AddBTNode(CTaskJumpLoop::Create(make_shared<FTaskJumpLoopDesc>(tTaskJumpLoopDesc)));
			}

			// jump end task node
			{
				// task desc
				FTaskJumpEndDesc tTaskJumpEndDesc;
				tTaskJumpEndDesc.m_spOwnerGameObject = spPinkCat;
				tTaskJumpEndDesc.m_wstrBTNodeName = L"JumpEndTask";
				tTaskJumpEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpEndDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_JumpEnd;
				spJumpSequenceNode->AddBTNode(CTaskJumpEnd::Create(make_shared<FTaskJumpEndDesc>(tTaskJumpEndDesc)));
			}
		}

		// damage task node
		{
			// node desc
			FTaskDamageDesc tTaskDamageDesc;
			tTaskDamageDesc.m_spOwnerGameObject = spPinkCat;
			tTaskDamageDesc.m_wstrBTNodeName = L"DamageTask";
			tTaskDamageDesc.m_eBTNodeType = EBTNodeType::TASK;

			// damage task desc
			tTaskDamageDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Damage;
			tTaskDamageDesc.m_spTarget = m_spPlayer;
			m_spRootNode->AddBTNode(CTaskDamaged::Create(make_shared<FTaskDamageDesc>(tTaskDamageDesc)));
		}

		// attack sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"AttackSequence";
			shared_ptr<CCompositeNode> spAttackSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spAttackSequenceNode);

			// aggro deco node
			{
				tDecoBBBoolDesc.m_wstrBTNodeName = L"AggroDeco";
				tDecoBBBoolDesc.m_eBoolValue = EBlackboardBoolValue::BOOL_AGGRO;
				spAttackSequenceNode->AddBTNode(CDecoratorBBBool::Create(make_shared<FDecoratorBBBoolDesc>(tDecoBBBoolDesc)));
			}

			// look at task node
			{
				// task desc
				FTaskLookAtDesc tTaskLookAtDesc;
				tTaskLookAtDesc.m_spOwnerGameObject = spPinkCat;
				tTaskLookAtDesc.m_wstrBTNodeName = L"AttackLookAtTask";
				tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

				// look at task desc
				tTaskLookAtDesc.m_spTarget = m_spPlayer;
				tTaskLookAtDesc.m_fMaxLerpDeltaSeconds = 1.0f;
				tTaskLookAtDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Walk;
				spAttackSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
			}

			// withdraw task node
			{
				// task desc
				FTaskWithdrawDesc tTaskWithdrawDesc;
				tTaskWithdrawDesc.m_spOwnerGameObject = spPinkCat;
				tTaskWithdrawDesc.m_wstrBTNodeName = L"AttackWithdrawTask";
				tTaskWithdrawDesc.m_eBTNodeType = EBTNodeType::TASK;

				// withdraw task desc
				tTaskWithdrawDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
				tTaskWithdrawDesc.m_fMinDistance = 5.0f;

				spAttackSequenceNode->AddBTNode(CTaskWithdraw::Create(make_shared<FTaskWithdrawDesc>(tTaskWithdrawDesc)));
			}

			// chase task node
			{
				// node desc
				FTaskChaseDesc tTaskChaseDesc;
				tTaskChaseDesc.m_spOwnerGameObject = spPinkCat;
				tTaskChaseDesc.m_wstrBTNodeName = L"AttackChaseTask";
				tTaskChaseDesc.m_eBTNodeType = EBTNodeType::TASK;

				// chase task desc
				tTaskChaseDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Run;
				tTaskChaseDesc.m_spTarget = m_spPlayer;
				tTaskChaseDesc.m_fMaxChaseDeltaSeconds = 0.0f;
				tTaskChaseDesc.m_fAcceptableRadius = 5.0f;
				spAttackSequenceNode->AddBTNode(CTaskChase::Create(make_shared<FTaskChaseDesc>(tTaskChaseDesc)));
			}

			// encount task node
			{
				// node desc
				FTaskEncountDesc tTaskEncountDesc;
				tTaskEncountDesc.m_spOwnerGameObject = spPinkCat;
				tTaskEncountDesc.m_wstrBTNodeName = L"EncountTask";
				tTaskEncountDesc.m_eBTNodeType = EBTNodeType::TASK;

				// encount task desc
				tTaskEncountDesc.m_spTarget = m_spPlayer;
				tTaskEncountDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Encount;
				spAttackSequenceNode->AddBTNode(CTaskEncount::Create(make_shared<FTaskEncountDesc>(tTaskEncountDesc)));
			}

			// attack start task node
			{
				// node desc
				FTaskAttackStartDesc tTaskAttackStartDesc;
				tTaskAttackStartDesc.m_spOwnerGameObject = spPinkCat;
				tTaskAttackStartDesc.m_wstrBTNodeName = L"AttackStartTask";
				tTaskAttackStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack start task desc
				tTaskAttackStartDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_NekoPunch_Start;
				spAttackSequenceNode->AddBTNode(CTaskAttackStart::Create(make_shared<FTaskAttackStartDesc>(tTaskAttackStartDesc)));
			}

			// attack loop task node
			{
				// node desc
				FTaskAttackLoopDesc tTaskAttackLoopDesc;
				tTaskAttackLoopDesc.m_spOwnerGameObject = spPinkCat;
				tTaskAttackLoopDesc.m_wstrBTNodeName = L"AttackLoopTask";
				tTaskAttackLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack loop task desc
				tTaskAttackLoopDesc.m_spTarget = m_spPlayer;
				tTaskAttackLoopDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_NekoPunch_Loop;
				spAttackSequenceNode->AddBTNode(CTaskAttackLoop::Create(make_shared<FTaskAttackLoopDesc>(tTaskAttackLoopDesc)));
			}

			// attack end task node
			{
				// node desc
				FTaskAttackEndDesc tTaskAttackEndDesc;
				tTaskAttackEndDesc.m_spOwnerGameObject = spPinkCat;
				tTaskAttackEndDesc.m_wstrBTNodeName = L"AttackEndTask";
				tTaskAttackEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack end task desc
				tTaskAttackEndDesc.m_iAttackEndAnimationID = (int32)EPinkCat::AS_PinkCat_NekoPunch_End;
				spAttackSequenceNode->AddBTNode(CTaskAttackEnd::Create(make_shared<FTaskAttackEndDesc>(tTaskAttackEndDesc)));
			}
		}

		// runaway node
		{
			// task desc
			FTaskRunawayDesc tRunawayDesc;
			tRunawayDesc.m_spOwnerGameObject = spPinkCat;
			tRunawayDesc.m_wstrBTNodeName = L"RunawayTask";
			tRunawayDesc.m_eBTNodeType = EBTNodeType::TASK;

			// runaway task desc
			tRunawayDesc.m_spTarget = m_spPlayer;
			tRunawayDesc.m_fMaxRunawayDeltaSeconds = 3.0f;
			tRunawayDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Run;
			m_spRootNode->AddBTNode(CTaskRunaway::Create(make_shared<FTaskRunawayDesc>(tRunawayDesc)));
		}

		// sleep sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"SleepSequence";
			shared_ptr<CCompositeNode> spSleepSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spSleepSequenceNode);

			// sleep deco node
			{
				FDecoratorSleepDesc tDecoSleepDesc;
				tDecoSleepDesc.m_spOwnerGameObject = spPinkCat;
				tDecoSleepDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
				tDecoSleepDesc.m_wstrBTNodeName = L"SleepDeco";
				spSleepSequenceNode->AddBTNode(CDecoratorSleep::Create(make_shared<FDecoratorSleepDesc>(tDecoSleepDesc)));
			}

			// sleep start task node
			{
				// node desc
				FTaskSleepStartDesc tTaskSleepStartDesc;
				tTaskSleepStartDesc.m_spOwnerGameObject = spPinkCat;
				tTaskSleepStartDesc.m_wstrBTNodeName = L"SleepStartTask";
				tTaskSleepStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep start task desc
				tTaskSleepStartDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_SleepStart;
				spSleepSequenceNode->AddBTNode(CTaskSleepStart::Create(make_shared<FTaskSleepStartDesc>(tTaskSleepStartDesc)));
			}

			// sleep loop task node
			{
				// node desc
				FTaskSleepLoopDesc tTaskSleepLoopDesc;
				tTaskSleepLoopDesc.m_spOwnerGameObject = spPinkCat;
				tTaskSleepLoopDesc.m_wstrBTNodeName = L"SleepLoopTask";
				tTaskSleepLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep loop task desc
				tTaskSleepLoopDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_SleepLoop;
				spSleepSequenceNode->AddBTNode(CTaskSleepLoop::Create(make_shared<FTaskSleepLoopDesc>(tTaskSleepLoopDesc)));
			}

			// sleep end task node
			{
				// node desc
				FTaskSleepEndDesc tTaskSleepEndDesc;
				tTaskSleepEndDesc.m_spOwnerGameObject = spPinkCat;
				tTaskSleepEndDesc.m_wstrBTNodeName = L"SleepEndTask";
				tTaskSleepEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep end task desc
				tTaskSleepEndDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_SleepEnd;
				spSleepSequenceNode->AddBTNode(CTaskSleepEnd::Create(make_shared<FTaskSleepEndDesc>(tTaskSleepEndDesc)));
			}
		}

		// near task node
		{
			// node desc
			FTaskNearDesc tTaskNearDesc;
			tTaskNearDesc.m_spOwnerGameObject = spPinkCat;
			tTaskNearDesc.m_wstrBTNodeName = L"NearTask";
			tTaskNearDesc.m_eBTNodeType = EBTNodeType::TASK;

			// near task desc
			tTaskNearDesc.m_spPlayer = m_spPlayer;
			tTaskNearDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
			m_spRootNode->AddBTNode(CTaskNear::Create(make_shared<FTaskNearDesc>(tTaskNearDesc)));
		}

		// wander sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"WanderSequence";
			shared_ptr<CCompositeNode> spWanderSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spWanderSequenceNode);

			// idle task node
			{
				// node desc
				FTaskIdleDesc tTaskIdleDesc;
				tTaskIdleDesc.m_spOwnerGameObject = spPinkCat;
				tTaskIdleDesc.m_wstrBTNodeName = L"IdleTask";
				tTaskIdleDesc.m_eBTNodeType = EBTNodeType::TASK;

				// idle task desc
				tTaskIdleDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
				tTaskIdleDesc.m_fMaxIdleDeltaSeconds = 10.0f;
				spWanderSequenceNode->AddBTNode(CTaskIdle::Create(make_shared<FTaskIdleDesc>(tTaskIdleDesc)));
			}

			// move task node
			{
				// node desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = shared_from_this();
				tTaskMoveDesc.m_wstrBTNodeName = L"WanderMoveTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Walk;
				tTaskMoveDesc.m_vSpawnPosition = _spPinkcatDesc->m_vInitPosition;
				spWanderSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			/*
			// rest task node
			{
				// node desc
				FTaskRestDesc tTaskRestDesc;
				tTaskRestDesc.m_spOwnerGameObject = shared_from_this();
				tTaskRestDesc.m_wstrBTNodeName = L"RestTask";
				tTaskRestDesc.m_eBTNodeType = EBTNodeType::TASK;

				// rest task desc
				tTaskRestDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Rest02;
				tTaskRestDesc.m_fMaxRestDeltaSeconds = 10.0f;

				shared_ptr<CTaskRest> spTaskRestNode = CTaskRest::Create(make_shared<FTaskRestDesc>(tTaskRestDesc));
				spWanderSequenceNode->AddBTNode(spTaskRestNode);
			}
			*/
		}
	}
}

void Client::CPinkCat::InitializePartnerPalBT(const std::shared_ptr<FPinkCatDesc> _spPinkcatDesc, bool _bInit)
{
	shared_ptr<CGameObject> spPinkCat = shared_from_this();

	// behavior tree component
	if (_bInit)
	{
		// component desc
		FBTComponentDesc tBTComponentDesc;
		tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
		tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
		tBTComponentDesc.m_eRenderType = ERenderType::NONE;
		tBTComponentDesc.m_wpOwnerGameObject = spPinkCat;

		m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
		AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
	}

	// behavior tree node
	{
		FCompositeNodeDesc tSelectorNodeDesc;
		tSelectorNodeDesc.m_spOwnerGameObject = spPinkCat;
		tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

		FCompositeNodeDesc tSequenceNodeDesc;
		tSequenceNodeDesc.m_spOwnerGameObject = spPinkCat;
		tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

		FDecoratorBBBoolDesc tDecoBBBoolDesc;
		tDecoBBBoolDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
		tDecoBBBoolDesc.m_spOwnerGameObject = spPinkCat;

		// root selector node
		{
			tSelectorNodeDesc.m_bRoot = true;
			tSelectorNodeDesc.m_wstrBTNodeName = L"RootSelector";
			m_spRootNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSelectorNodeDesc));

			tSelectorNodeDesc.m_bRoot = false;
		}

		// dead task node
		{
			// node desc
			FTaskDeadDesc tTaskDeadDesc;
			tTaskDeadDesc.m_spOwnerGameObject = spPinkCat;
			tTaskDeadDesc.m_wstrBTNodeName = L"DeadTask";
			tTaskDeadDesc.m_eBTNodeType = EBTNodeType::TASK;

			// dead task desc
			tTaskDeadDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Carrying;
			m_spRootNode->AddBTNode(CTaskDead::Create(make_shared<FTaskDeadDesc>(tTaskDeadDesc)));
		}

		// damage task node
		{
			// node desc
			FTaskDamageDesc tTaskDamageDesc;
			tTaskDamageDesc.m_spOwnerGameObject = spPinkCat;
			tTaskDamageDesc.m_wstrBTNodeName = L"DamageTask";
			tTaskDamageDesc.m_eBTNodeType = EBTNodeType::TASK;

			// damage task desc
			tTaskDamageDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Damage;
			tTaskDamageDesc.m_spTarget = nullptr;
			m_spRootNode->AddBTNode(CTaskDamaged::Create(make_shared<FTaskDamageDesc>(tTaskDamageDesc)));
		}

		// attack sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"AttackSequence";
			shared_ptr<CCompositeNode> spAttackSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spAttackSequenceNode);

			// aggro deco node
			{
				tDecoBBBoolDesc.m_wstrBTNodeName = L"AggroDeco";
				tDecoBBBoolDesc.m_eBoolValue = EBlackboardBoolValue::BOOL_AGGRO;
				spAttackSequenceNode->AddBTNode(CDecoratorBBBool::Create(make_shared<FDecoratorBBBoolDesc>(tDecoBBBoolDesc)));
			}

			// look at task node
			{
				// task desc
				FTaskLookAtDesc tTaskLookAtDesc;
				tTaskLookAtDesc.m_spOwnerGameObject = spPinkCat;
				tTaskLookAtDesc.m_wstrBTNodeName = L"AttackLookAtTask";
				tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

				// look at task desc
				tTaskLookAtDesc.m_spTarget = nullptr;
				tTaskLookAtDesc.m_fMaxLerpDeltaSeconds = 1.0f;
				tTaskLookAtDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Walk;
				spAttackSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
			}

			// withdraw task node
			{
				// task desc
				FTaskWithdrawDesc tTaskWithdrawDesc;
				tTaskWithdrawDesc.m_spOwnerGameObject = spPinkCat;
				tTaskWithdrawDesc.m_wstrBTNodeName = L"WithdrawTask";
				tTaskWithdrawDesc.m_eBTNodeType = EBTNodeType::TASK;

				// withdraw task desc
				tTaskWithdrawDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
				spAttackSequenceNode->AddBTNode(CTaskWithdraw::Create(make_shared<FTaskWithdrawDesc>(tTaskWithdrawDesc)));
			}

			// chase task node
			{
				// node desc
				FTaskChaseDesc tTaskChaseDesc;
				tTaskChaseDesc.m_spOwnerGameObject = spPinkCat;
				tTaskChaseDesc.m_wstrBTNodeName = L"ChaseTask";
				tTaskChaseDesc.m_eBTNodeType = EBTNodeType::TASK;

				// chase task desc;
				tTaskChaseDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Run;
				tTaskChaseDesc.m_spTarget = nullptr;
				tTaskChaseDesc.m_fMaxChaseDeltaSeconds = 0.0f;
				tTaskChaseDesc.m_fAcceptableRadius = 5.0f;
				spAttackSequenceNode->AddBTNode(CTaskChase::Create(make_shared<FTaskChaseDesc>(tTaskChaseDesc)));
			}

			// encount task node
			{
				// node desc
				FTaskEncountDesc tTaskEncountDesc;
				tTaskEncountDesc.m_spOwnerGameObject = spPinkCat;
				tTaskEncountDesc.m_wstrBTNodeName = L"EncountTask";
				tTaskEncountDesc.m_eBTNodeType = EBTNodeType::TASK;

				// encount task desc
				tTaskEncountDesc.m_spTarget = nullptr;
				tTaskEncountDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Encount;
				spAttackSequenceNode->AddBTNode(CTaskEncount::Create(make_shared<FTaskEncountDesc>(tTaskEncountDesc)));
			}

			// attack start task node
			{
				// node desc
				FTaskAttackStartDesc tTaskAttackStartDesc;
				tTaskAttackStartDesc.m_spOwnerGameObject = spPinkCat;
				tTaskAttackStartDesc.m_wstrBTNodeName = L"AttackStartTask";
				tTaskAttackStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack start task desc
				tTaskAttackStartDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_NekoPunch_Start;
				spAttackSequenceNode->AddBTNode(CTaskAttackStart::Create(make_shared<FTaskAttackStartDesc>(tTaskAttackStartDesc)));
			}

			// attack loop task node
			{
				// node desc
				FTaskAttackLoopDesc tTaskAttackLoopDesc;
				tTaskAttackLoopDesc.m_spOwnerGameObject = spPinkCat;
				tTaskAttackLoopDesc.m_wstrBTNodeName = L"AttackLoopTask";
				tTaskAttackLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack loop task desc
				tTaskAttackLoopDesc.m_spTarget = nullptr;
				tTaskAttackLoopDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_NekoPunch_Loop;
				spAttackSequenceNode->AddBTNode(CTaskAttackLoop::Create(make_shared<FTaskAttackLoopDesc>(tTaskAttackLoopDesc)));
			}

			// attack end task node
			{
				// node desc
				FTaskAttackEndDesc tTaskAttackEndDesc;
				tTaskAttackEndDesc.m_spOwnerGameObject = spPinkCat;
				tTaskAttackEndDesc.m_wstrBTNodeName = L"AttackEndTask";
				tTaskAttackEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack end task desc
				tTaskAttackEndDesc.m_iAttackEndAnimationID = (int32)EPinkCat::AS_PinkCat_NekoPunch_End;
				spAttackSequenceNode->AddBTNode(CTaskAttackEnd::Create(make_shared<FTaskAttackEndDesc>(tTaskAttackEndDesc)));
			}
		}

		// follow sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"FollowSequence";
			shared_ptr<CCompositeNode> spFollowSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spFollowSequenceNode);

			// follow task node
			{
				// node desc
				FTaskFollowDesc tTaskFollowDesc;
				tTaskFollowDesc.m_spOwnerGameObject = spPinkCat;
				tTaskFollowDesc.m_wstrBTNodeName = L"FollowTask";
				tTaskFollowDesc.m_eBTNodeType = EBTNodeType::TASK;

				// follow task desc
				tTaskFollowDesc.m_spPlayer = m_spPlayer;
				tTaskFollowDesc.m_iWalkAnimationID = (int32)EPinkCat::AS_PinkCat_Walk;
				tTaskFollowDesc.m_iRunAnimationID = (int32)EPinkCat::AS_PinkCat_Run;
				spFollowSequenceNode->AddBTNode(CTaskFollow::Create(make_shared<FTaskFollowDesc>(tTaskFollowDesc)));
			}

			// turn task node
			{
				// node desc
				FTaskTurnDesc tTaskTurnDesc;
				tTaskTurnDesc.m_spOwnerGameObject = spPinkCat;
				tTaskTurnDesc.m_wstrBTNodeName = L"FollowTurnTask";
				tTaskTurnDesc.m_eBTNodeType = EBTNodeType::TASK;

				// turn task desc
				tTaskTurnDesc.m_spTarget = m_spPlayer;
				tTaskTurnDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
				spFollowSequenceNode->AddBTNode(CTaskTurn::Create(make_shared<FTaskTurnDesc>(tTaskTurnDesc)));
			}
		}
	}
}

void Client::CPinkCat::InitializeSlavePalBT(const std::shared_ptr<FPinkCatDesc> _spPinkcatDesc, bool _bInit)
{
	shared_ptr<CGameObject> spPinkCat = shared_from_this();

	// behavior tree component
	if (_bInit)
	{
		// component desc
		FBTComponentDesc tBTComponentDesc;
		tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
		tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
		tBTComponentDesc.m_eRenderType = ERenderType::NONE;
		tBTComponentDesc.m_wpOwnerGameObject = spPinkCat;

		m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
		AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
	}

	// behavior tree node
	{
		FCompositeNodeDesc tSelectorNodeDesc;
		tSelectorNodeDesc.m_spOwnerGameObject = spPinkCat;
		tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

		FCompositeNodeDesc tSequenceNodeDesc;
		tSequenceNodeDesc.m_spOwnerGameObject = spPinkCat;
		tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

		FDecoratorBBBoolDesc tDecoBBBoolDesc;
		tDecoBBBoolDesc.m_spOwnerGameObject = spPinkCat;
		tDecoBBBoolDesc.m_eBTNodeType = EBTNodeType::DECORATOR;

		// root selector node
		{
			tSelectorNodeDesc.m_bRoot = true;
			tSelectorNodeDesc.m_wstrBTNodeName = L"RootSelector";
			m_spRootNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSelectorNodeDesc));

			tSelectorNodeDesc.m_bRoot = false;
		}

		// dead task node
		{
			// node desc
			FTaskDeadDesc tTaskDeadDesc;
			tTaskDeadDesc.m_spOwnerGameObject = spPinkCat;
			tTaskDeadDesc.m_wstrBTNodeName = L"DeadTask";
			tTaskDeadDesc.m_eBTNodeType = EBTNodeType::TASK;

			// dead task desc
			tTaskDeadDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Carrying;
			m_spRootNode->AddBTNode(CTaskDead::Create(make_shared<FTaskDeadDesc>(tTaskDeadDesc)));
		}

		// jump sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"JumpSequence";
			shared_ptr<CCompositeNode> spJumpSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spJumpSequenceNode);

			// jump deco node
			{
				tDecoBBBoolDesc.m_wstrBTNodeName = L"JumpDeco";
				tDecoBBBoolDesc.m_eBoolValue = EBlackboardBoolValue::BOOL_JUMP;
				spJumpSequenceNode->AddBTNode(CDecoratorBBBool::Create(make_shared<FDecoratorBBBoolDesc>(tDecoBBBoolDesc)));
			}

			// jump loop task node
			{
				// task desc
				FTaskJumpLoopDesc tTaskJumpLoopDesc;
				tTaskJumpLoopDesc.m_spOwnerGameObject = spPinkCat;
				tTaskJumpLoopDesc.m_wstrBTNodeName = L"JumpLoopTask";
				tTaskJumpLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpLoopDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_JumpLoop;
				spJumpSequenceNode->AddBTNode(CTaskJumpLoop::Create(make_shared<FTaskJumpLoopDesc>(tTaskJumpLoopDesc)));
			}

			// jump end task node
			{
				// task desc
				FTaskJumpEndDesc tTaskJumpEndDesc;
				tTaskJumpEndDesc.m_spOwnerGameObject = spPinkCat;
				tTaskJumpEndDesc.m_wstrBTNodeName = L"JumpEndTask";
				tTaskJumpEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpEndDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_JumpEnd;
				spJumpSequenceNode->AddBTNode(CTaskJumpEnd::Create(make_shared<FTaskJumpEndDesc>(tTaskJumpEndDesc)));
			}
		}

		// interacted sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"InteractedSequence";
			shared_ptr<CCompositeNode> spInteractSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spInteractSequenceNode);

			// interacted deco node
			{
				FDecoratorInteractedDesc tDecoInteractedDesc;
				tDecoInteractedDesc.m_spOwnerGameObject = spPinkCat;
				tDecoInteractedDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
				tDecoInteractedDesc.m_wstrBTNodeName = L"InteractedDeco";
				spInteractSequenceNode->AddBTNode(CDecoratorInteracted::Create(make_shared<FDecoratorInteractedDesc>(tDecoInteractedDesc)));
			}

			// turn task node
			{
				// node desc
				FTaskTurnDesc tTaskTurnDesc;
				tTaskTurnDesc.m_spOwnerGameObject = spPinkCat;
				tTaskTurnDesc.m_wstrBTNodeName = L"InteractedTurnTask";
				tTaskTurnDesc.m_eBTNodeType = EBTNodeType::TASK;

				// turn task desc
				tTaskTurnDesc.m_spTarget = m_spPlayer;
				tTaskTurnDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
				spInteractSequenceNode->AddBTNode(CTaskTurn::Create(make_shared<FTaskTurnDesc>(tTaskTurnDesc)));
			}

			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spPinkCat;
				tTaskMoveDesc.m_wstrBTNodeName = L"InteractedMoveTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Walk;
				spInteractSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// react task node
			{
				// task desc
				FTaskReactDesc tTaskReactDesc;
				tTaskReactDesc.m_wstrBTNodeName = L"InteractedReactTask";
				tTaskReactDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskReactDesc.m_spOwnerGameObject = spPinkCat;

				// react task desc
				tTaskReactDesc.m_iEatAnimationID = (int32)EPinkCat::AS_PinkCat_Eat;
				tTaskReactDesc.m_iPetAnimationID = (int32)EPinkCat::AS_PinkCat_Petting;
				tTaskReactDesc.m_iSlaughterAnimationID = (int32)EPinkCat::AS_PinkCat_Carrying;
				spInteractSequenceNode->AddBTNode(CTaskReact::Create(make_shared<FTaskReactDesc>(tTaskReactDesc)));
			}

			// emote task node
			{
				// task desc
				FTaskEmoteDesc tTaskEmoteDesc;
				tTaskEmoteDesc.m_wstrBTNodeName = L"InteractedEmoteTask";
				tTaskEmoteDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskEmoteDesc.m_spOwnerGameObject = spPinkCat;

				// react task desc
				tTaskEmoteDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
				tTaskEmoteDesc.m_fMaxEmoteDeltaSeconds = 3.0f;
				spInteractSequenceNode->AddBTNode(CTaskEmote::Create(make_shared<FTaskEmoteDesc>(tTaskEmoteDesc)));
			}
		}

		// sleep sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"SleepSequence";
			shared_ptr<CCompositeNode> spSleepSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spSleepSequenceNode);

			// sleep deco node
			{
				FDecoratorSleepDesc tDecoSleepDesc;
				tDecoSleepDesc.m_spOwnerGameObject = spPinkCat;
				tDecoSleepDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
				tDecoSleepDesc.m_wstrBTNodeName = L"SleepDeco";
				spSleepSequenceNode->AddBTNode(CDecoratorSleep::Create(make_shared<FDecoratorSleepDesc>(tDecoSleepDesc)));
			}

			// 침대가 있는 경우, 침대로 이동합니다.
			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spPinkCat;
				tTaskMoveDesc.m_wstrBTNodeName = L"SleepMoveTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Walk;
				spSleepSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// sleep start task node
			{
				// node desc
				FTaskSleepStartDesc tTaskSleepStartDesc;
				tTaskSleepStartDesc.m_spOwnerGameObject = spPinkCat;
				tTaskSleepStartDesc.m_wstrBTNodeName = L"SleepStartTask";
				tTaskSleepStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep start task desc
				tTaskSleepStartDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_SleepStart;
				spSleepSequenceNode->AddBTNode(CTaskSleepStart::Create(make_shared<FTaskSleepStartDesc>(tTaskSleepStartDesc)));
			}

			// sleep loop task node
			{
				// node desc
				FTaskSleepLoopDesc tTaskSleepLoopDesc;
				tTaskSleepLoopDesc.m_spOwnerGameObject = spPinkCat;
				tTaskSleepLoopDesc.m_wstrBTNodeName = L"SleepLoopTask";
				tTaskSleepLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep loop task desc
				tTaskSleepLoopDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_SleepLoop;
				spSleepSequenceNode->AddBTNode(CTaskSleepLoop::Create(make_shared<FTaskSleepLoopDesc>(tTaskSleepLoopDesc)));
			}

			// sleep end task node
			{
				// node desc
				FTaskSleepEndDesc tTaskSleepEndDesc;
				tTaskSleepEndDesc.m_spOwnerGameObject = spPinkCat;
				tTaskSleepEndDesc.m_wstrBTNodeName = L"SleepEndTask";
				tTaskSleepEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep end task desc
				tTaskSleepEndDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_SleepEnd;
				spSleepSequenceNode->AddBTNode(CTaskSleepEnd::Create(make_shared<FTaskSleepEndDesc>(tTaskSleepEndDesc)));
			}
		}

		// hunger sequence node
		/*
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"HungerSequence";
			shared_ptr<CCompositeNode> spHungerSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spHungerSequenceNode);

			// hunger deco node
			{
				FDecoratorHungerDesc tDecoHungerDesc;
				tDecoHungerDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
				tDecoHungerDesc.m_wstrBTNodeName = L"HungerDeco";
				tDecoHungerDesc.m_spOwnerGameObject = spPinkCat;
				spHungerSequenceNode->AddBTNode(CDecoratorHunger::Create(make_shared<FDecoratorHungerDesc>(tDecoHungerDesc)));
			}

			// notice task node
			{
				// task desc
				FTaskNoticeDesc tTaskNoticeDesc;
				tTaskNoticeDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskNoticeDesc.m_wstrBTNodeName = L"HungerNoticeTask";
				tTaskNoticeDesc.m_spOwnerGameObject = spPinkCat;

				// notice task desc
				tTaskNoticeDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
				tTaskNoticeDesc.m_eMonsterNoticeType = EMonsterNoticeType::LIGHT_BULB;
				spHungerSequenceNode->AddBTNode(CTaskNotice::Create(make_shared<FTaskNoticeDesc>(tTaskNoticeDesc)));
			}

			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spPinkCat;
				tTaskMoveDesc.m_wstrBTNodeName = L"HungerMoveTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Run;
				spHungerSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// react task node
			{
				// task desc
				FTaskReactDesc tTaskReactDesc;
				tTaskReactDesc.m_spOwnerGameObject = spPinkCat;
				tTaskReactDesc.m_wstrBTNodeName = L"HungerReactTask";
				tTaskReactDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskReactDesc.m_iEatAnimationID = (int32)EPinkCat::AS_PinkCat_Eat;
				spHungerSequenceNode->AddBTNode(CTaskReact::Create(make_shared<FTaskReactDesc>(tTaskReactDesc)));
			}

			// emote task node
			{
				// task desc
				FTaskEmoteDesc tTaskEmoteDesc;
				tTaskEmoteDesc.m_spOwnerGameObject = spPinkCat;
				tTaskEmoteDesc.m_wstrBTNodeName = L"HungerEmoteTask";
				tTaskEmoteDesc.m_eBTNodeType = EBTNodeType::TASK;

				// emote task desc
				tTaskEmoteDesc.m_iAnimationID = (int32)ESheepBall::AS_SheepBall_Idle;
				tTaskEmoteDesc.m_fMaxEmoteDeltaSeconds = 3.0f;
				spHungerSequenceNode->AddBTNode(CTaskEmote::Create(make_shared<FTaskEmoteDesc>(tTaskEmoteDesc)));
			}
		}
		*/

		// todo sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"ToDoSequence";
			shared_ptr<CCompositeNode> spToDoSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spToDoSequenceNode);

			// todo deco node
			{
				tDecoBBBoolDesc.m_wstrBTNodeName = L"ToDoDeco";
				tDecoBBBoolDesc.m_eBoolValue = EBlackboardBoolValue::BOOL_TODO_START;
				spToDoSequenceNode->AddBTNode(CDecoratorBBBool::Create(make_shared<FDecoratorBBBoolDesc>(tDecoBBBoolDesc)));
			}

			// notice task node
			{
				// task desc
				FTaskNoticeDesc tTaskNoticeDesc;
				tTaskNoticeDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskNoticeDesc.m_wstrBTNodeName = L"NoticeTask";
				tTaskNoticeDesc.m_spOwnerGameObject = spPinkCat;

				// notice task desc
				tTaskNoticeDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
				tTaskNoticeDesc.m_eMonsterNoticeType = EMonsterNoticeType::LIGHT_BULB;
				spToDoSequenceNode->AddBTNode(CTaskNotice::Create(make_shared<FTaskNoticeDesc>(tTaskNoticeDesc)));
			}

			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spPinkCat;
				tTaskMoveDesc.m_wstrBTNodeName = L"MoveToWorkTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Walk;
				spToDoSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// todo task node
			{
				// task desc
				FTaskToDoDesc tTaskToDoDesc;
				tTaskToDoDesc.m_wstrBTNodeName = L"ToDoTask";
				tTaskToDoDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskToDoDesc.m_spOwnerGameObject = spPinkCat;

				// todo task desc
				tTaskToDoDesc.m_iProduceAnimationID = (int32)EPinkCat::AS_PinkCat_CommonWork;
				tTaskToDoDesc.m_iBuildAnimationID = (int32)EPinkCat::AS_PinkCat_Architecture;
				tTaskToDoDesc.m_iCutDownAnimationID = (int32)EPinkCat::AS_PinkCat_Mining;
				tTaskToDoDesc.m_iMineAnimationID = (int32)EPinkCat::AS_PinkCat_Mining;
				tTaskToDoDesc.m_iTransportAnimationID = (int32)EPinkCat::AS_PinkCat_Transport;
				spToDoSequenceNode->AddBTNode(CTaskToDo::Create(make_shared<FTaskToDoDesc>(tTaskToDoDesc)));
			}

			// emote task node
			{
				// task desc
				FTaskEmoteDesc tTaskEmoteDesc;
				tTaskEmoteDesc.m_wstrBTNodeName = L"ToDoEmoteTask";
				tTaskEmoteDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskEmoteDesc.m_spOwnerGameObject = spPinkCat;

				// emote task desc
				tTaskEmoteDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
				tTaskEmoteDesc.m_fMaxEmoteDeltaSeconds = 3.0f;
				spToDoSequenceNode->AddBTNode(CTaskEmote::Create(make_shared<FTaskEmoteDesc>(tTaskEmoteDesc)));
			}
		}

		// wander sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"WanderSequence";
			shared_ptr<CCompositeNode> spWanderSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spWanderSequenceNode);

			// idle task node
			{
				// node desc
				FTaskIdleDesc tTaskIdleDesc;
				tTaskIdleDesc.m_spOwnerGameObject = spPinkCat;
				tTaskIdleDesc.m_wstrBTNodeName = L"WanderIdleTask";
				tTaskIdleDesc.m_eBTNodeType = EBTNodeType::TASK;

				// idle task desc
				tTaskIdleDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Idle;
				tTaskIdleDesc.m_fMaxIdleDeltaSeconds = 5.0f;
				spWanderSequenceNode->AddBTNode(CTaskIdle::Create(make_shared<FTaskIdleDesc>(tTaskIdleDesc)));
			}

			// move task node
			{
				// node desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject	= spPinkCat;
				tTaskMoveDesc.m_wstrBTNodeName		= L"WanderMoveTask";
				tTaskMoveDesc.m_eBTNodeType			= EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID		= (int32)EPinkCat::AS_PinkCat_Walk;

				shared_ptr<CArchitecture> spPalBox = FindArchitecture(EArchitectureType::PAL_BOX);
				Vector3 vPalBoxPosition = spPalBox->GetPosition().value();
				tTaskMoveDesc.m_vSpawnPosition = vPalBoxPosition;

				spWanderSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			/*
			// rest task node
			{
				// node desc
				FTaskRestDesc tTaskRestDesc;
				tTaskRestDesc.m_spOwnerGameObject = spPinkCat;
				tTaskRestDesc.m_wstrBTNodeName = L"RestTask";
				tTaskRestDesc.m_eBTNodeType = EBTNodeType::TASK;

				// rest task desc
				tTaskRestDesc.m_iAnimationID = (int32)EPinkCat::AS_PinkCat_Rest03;
				tTaskRestDesc.m_fMaxRestDeltaSeconds = 5.0f;
				spWanderSequenceNode->AddBTNode(CTaskRest::Create(make_shared<FTaskRestDesc>(tTaskRestDesc)));
			}
			*/
		}
	}
}

HRESULT Client::CPinkCat::ReInitializePalBT(EMonsterAIType _eMonsterAIType)
{
	if (FAILED(CMonster::ReInitializePalBT(_eMonsterAIType))) { return E_FAIL; }

	// 이전 비헤이비어 트리를 해제합니다.
	if (m_spRootNode) { m_spRootNode = nullptr; }

	m_spBehaviorTreeComponent->Release();

	// 새로운 비헤이비어 트리를 할당합니다.
	switch (_eMonsterAIType)
	{
	case Client::EMonsterAIType::WILD_PAL:		InitializeWildPalBT(nullptr, false);	break;
	case Client::EMonsterAIType::PARTNER_PAL:	InitializePartnerPalBT(nullptr, false);	break;
	case Client::EMonsterAIType::SLAVE_PAL:		InitializeSlavePalBT(nullptr, false);	break;
	}

	// behavior tree desc
	m_spBehaviorTreeComponent->SetRootNode(m_spRootNode);
	m_spBehaviorTreeComponent->SetBehaveTree(true);
	return S_OK;
}

void Client::CPinkCat::LookAt(const Vector3& _vPosition)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->LookAt(_vPosition);
}

void Client::CPinkCat::Turn(const Vector3& _vAxis, float _fAngularSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->Turn(_vAxis, _fAngularSpeed);
}

void Client::CPinkCat::MoveForward(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderForward(_fSpeed);
}

void Client::CPinkCat::MoveBackward(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderBackward(_fSpeed);
}

void Client::CPinkCat::MoveRight(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderRight(_fSpeed);
}

void Client::CPinkCat::MoveLeft(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderLeft(_fSpeed);
}

void Client::CPinkCat::MoveUp(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderUp(_fSpeed);
}

void Client::CPinkCat::MoveDown(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderDown(_fSpeed);
}

void Client::CPinkCat::SetPosition(const Vector3& _vPosition)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetPosition(_vPosition);
}

const std::optional<Vector3> Client::CPinkCat::GetPosition() const
{
	if (!m_spBodySphereComponent) { return nullopt; }

	return m_spBodySphereComponent->GetPosition();
}

void Client::CPinkCat::SetAllyCollisionLayer()
{
	m_spBodySphereComponent->SetCollisionLayer(ECollisionLayer::ALLY, ECollisionFlag::ALLY);

	m_spAttackSphereComponent->SetCollisionLayer(ECollisionLayer::ALLY_WEAPON, ECollisionFlag::ALLY_WEAPON);
}

void Client::CPinkCat::UpdateBlackboardValues()
{
	CMonster::UpdateBlackboardValues();
}

void Client::CPinkCat::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CMonster::OnTriggerEnter(_pSrcComponent, _pDstComponent);

	// 컨트롤러 컴포넌트
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
		CControllerComponent* pDstControllerComponent = dynamic_cast<CControllerComponent*>(_pDstComponent);

		ECollisionLayer eSrcCollisionLayer = pSrcColliderComponent->GetCollisionLayer();
		ECollisionLayer eDstCollisionLayer = pDstControllerComponent->GetCollisionLayer();

		if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
		{
			// 야생 까부냥이 플레이어를 공격합니다.
			if
			(
				GetBoolValue(EBlackboardBoolValue::BOOL_ATTACK) == true		// 야생 까부냥이 공격하는 중이고
				&& (eSrcCollisionLayer == ECollisionLayer::MONSTER_WEAPON)	// 야생 까부냥의 공격 콜라이더가
				&& (eDstCollisionLayer == ECollisionLayer::PLAYER)			// 플레이어와 부딪힌 경우
			)
			{
				ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, true);

				shared_ptr<CPlayer> spPlayer = static_pointer_cast<CPlayer>(pDstControllerComponent->GetOwnerGameObject());

				Vector3 vPlayerPosition = spPlayer->GetPosition().value();
				Vector3 vMonsterPosition = GetPosition().value();
				Vector3 vForceDirection = vPlayerPosition - vMonsterPosition;
				vForceDirection.Normalize();

				spPlayer->AddForce(vForceDirection, CClientLoop::s_spClientDataSheet->k_mapKnockBackValue[EMonsterType::PINK_CAT]);
				spPlayer->TakeDamage(shared_from_this(), m_fAttackPower);

				// 타격 이펙트를 출력합니다.
				m_wpEffectManager.lock()->ActivateEffect(L"Group_Hit3", GetPosition().value() + Vector3(0.f, 1.f, 0.f));

				// 타격 사운드를 출력합니다.
				m_wpSoundManager.lock()->PlaySoundEx("Punch", 0.5f, false);
			}
		}

		return;
	}

	// 콜라이더 컴포넌트
	CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	ECollisionLayer eSrcCollisionLayer = pSrcColliderComponent->GetCollisionLayer();
	ECollisionLayer eDstCollisionLayer = pDstColliderComponent->GetCollisionLayer();

	// 파트너 까부냥이 야생 팰을 공격합니다.
	if (m_eMonsterAIType == EMonsterAIType::PARTNER_PAL)
	{
		if
		(
			GetBoolValue(EBlackboardBoolValue::BOOL_ATTACK) == true	// 파트너 까부냥이 공격하는 중이고
			&& (eSrcCollisionLayer == ECollisionLayer::ALLY_WEAPON)	// 파트너 까부냥의 공격 콜라이더가
			&& (eDstCollisionLayer == ECollisionLayer::MONSTER)		// 야생 몬스터와 부딪친 경우
		)
		{
			ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, true);
			static_pointer_cast<CMonster>(pDstColliderComponent->GetOwnerGameObject())->TakeDamage(shared_from_this(), m_fAttackPower);
		}
	}

	// 야생 까부냥이 파트너 팰을 공격합니다.
	else if
	(
		GetBoolValue(EBlackboardBoolValue::BOOL_ATTACK) == true		// 야생 까부냥이 공격하는 중이고
		&& (eSrcCollisionLayer == ECollisionLayer::MONSTER_WEAPON)	// 야생 까부냥의 공격 콜라이더가
		&& (eDstCollisionLayer == ECollisionLayer::ALLY)			// 파트너 팰과 부딪힌 경우
	)
	{
		ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, true);
		static_pointer_cast<CMonster>(pDstColliderComponent->GetOwnerGameObject())->TakeDamage(shared_from_this(), 0.0f);
	}
}

void Client::CPinkCat::OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CMonster::OnCollisionEnter(_pSrcComponent, _pDstComponent);
}

shared_ptr<CColliderComponent> Client::CPinkCat::GetBodyColliderComponent()
{
	return m_spBodySphereComponent;
}

void Client::CPinkCat::SetLinearVelocity(const PxVec3& _vLinearVelocity)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetLinearVelocity(_vLinearVelocity);
}

void Client::CPinkCat::SetAngularVelocity(const PxVec3& _vLinearVelocity)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetAngularVelocity(_vLinearVelocity);
}
