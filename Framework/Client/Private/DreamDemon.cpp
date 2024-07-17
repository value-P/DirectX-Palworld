#include "pch.h"
#include "DreamDemon.h"

// engine manager
#include "EngineModule.h"
#include "CameraManager.h"
#include "InputManager.h"
#include "TimerManager.h"

// client manager
#include "GameManager.h"
#include "UIManager.h"
#include "EffectManager.h"

// component
#include "BehaviorTreeComponent.h"
#include "BoxComponent.h"
#include "ColliderComponent.h"
#include "ControllerComponent.h"
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

// task node
#include "TaskAttackStart.h"
#include "TaskAttackLoop.h"
#include "TaskAttackEnd.h"
#include "TaskCaptured.h"
#include "TaskChase.h"
#include "TaskDamaged.h"
#include "TaskDead.h"
#include "TaskDreamDemonAttackLoop.h"
#include "TaskEncount.h"
#include "TaskEmote.h"
#include "TaskFollow.h"
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

// gameobejct
#include "Player.h"
#include "Factory.h"
#include "Architecture.h"
#include "Weapon.h"

std::shared_ptr<CDreamDemon> Client::CDreamDemon::Create(const std::shared_ptr<FDreamDemonDesc>& _spDreamDemonDesc)
{
	const std::shared_ptr<CDreamDemon> spDreamDemon = make_shared<CDreamDemon>();
	ENSUREF(spDreamDemon->Initialize(_spDreamDemonDesc), L"Failed to initialize dreamdemon.");
	return spDreamDemon;
}

HRESULT Client::CDreamDemon::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CMonster::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FDreamDemonDesc> spDreamDemonDesc = static_pointer_cast<FDreamDemonDesc>(_spGameObjectDesc);
	InitDreamDemon(spDreamDemonDesc);
	InitMeshComponent();
	InitColliderComponent();
	InitBehaviorTreeComponent(spDreamDemonDesc);
	return S_OK;
}

HRESULT Client::CDreamDemon::BeginPlay()
{
	if (FAILED(CMonster::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CDreamDemon::PreTick(float _fDeltaSeconds)
{
	switch (CMonster::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CDreamDemon::Tick(float _fDeltaSeconds)
{
	switch (CMonster::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CDreamDemon::PostTick(float _fDeltaSeconds)
{
	switch (CMonster::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CDreamDemon::EndPlay()
{
	if (FAILED(CMonster::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CDreamDemon::Release()
{
	m_spBodyBoxComponent = nullptr;

	CMonster::Release();
}

void Client::CDreamDemon::InitDreamDemon(const shared_ptr<FDreamDemonDesc>& _spDreamDemonDesc)
{
	// gameobject desc
	m_wstrGameObjectName = L"DreamDemon";
	m_eMobilityType = EMobilityType::DYNAMIC;

	// monster desc
	m_eMonsterType = EMonsterType::DREAM_DEMON;
	m_eMonsterSizeType = EMonsterSizeType::SMALL;
}

void Client::CDreamDemon::InitMeshComponent()
{
	{
		// component desc
		FMeshComponentDesc tMeshComponentDesc;
		tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";
		tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
		tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
		tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

		// mesh component desc
		tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/DreamDemon.dat";
		tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(180.0f));
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

void Client::CDreamDemon::InitColliderComponent()
{
	// body box component
	{
		// component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
		tBoxComponentDesc.m_wstrComponentName	= L"BodySphereComponent";
		tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
		tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

		// collider component desc
		tBoxComponentDesc.m_bTrigger			= false;
		tBoxComponentDesc.m_bGravity			= true;
		tBoxComponentDesc.m_eColliderType		= EColliderType::BOX;
		tBoxComponentDesc.m_eMobilityType		= EMobilityType::DYNAMIC;
		tBoxComponentDesc.m_bDeferredAdd		= true;

		switch (m_eMonsterAIType)
		{
		case Client::EMonsterAIType::WILD_PAL:
			tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::MONSTER;
			tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::MONSTER;
			break;

		case Client::EMonsterAIType::PARTNER_PAL:
		case Client::EMonsterAIType::SLAVE_PAL:
			tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::ALLY;
			tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::ALLY;
			break;

		default:
			CHECKF(false, L"Monster AI type is not designated.");
			break;
		}
		
		tBoxComponentDesc.m_fMass				= 10.0f;
		tBoxComponentDesc.m_iAxisLockType		= (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
		tBoxComponentDesc.m_vLocalPosition		= Vector3(0.0f, 1.2f, 0.0f);
		tBoxComponentDesc.m_vMaterial			= Vector3(1.0f, 1.0f, 0.05f);

		// sphere component desc
		tBoxComponentDesc.m_vExtents = Vector3(0.3f, 1.0f, 0.3f);

		m_spBodyBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBodyBoxComponent));
	}
}

void Client::CDreamDemon::InitBehaviorTreeComponent(const std::shared_ptr<FDreamDemonDesc>& _spDreamDemonDesc)
{
	// 1. 야생 팰 비헤이비어 트리
	if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
	{
		InitializeWildPalBT(_spDreamDemonDesc);
	}

	// 2. 파트너 팰 비헤이비어 트리
	else if (m_eMonsterAIType == EMonsterAIType::PARTNER_PAL)
	{
		InitializePartnerPalBT(_spDreamDemonDesc);
	}

	// 3. 거점 팰 비헤이비어 트리
	else if (m_eMonsterAIType == EMonsterAIType::SLAVE_PAL)
	{
		InitializeSlavePalBT(_spDreamDemonDesc);
	}

	// behavior tree desc
	m_spBehaviorTreeComponent->SetRootNode(m_spRootNode);
	m_spBehaviorTreeComponent->SetBehaveTree(true);
}

void Client::CDreamDemon::InitializeWildPalBT(const std::shared_ptr<FDreamDemonDesc> _spDreamDemonDesc, bool _bInit)
{
	shared_ptr<CGameObject> spDreamDemon = shared_from_this();

	// behavior tree component
	if (_bInit)
	{
		// component desc
		FBTComponentDesc tBTComponentDesc;
		tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
		tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
		tBTComponentDesc.m_eRenderType = ERenderType::NONE;
		tBTComponentDesc.m_wpOwnerGameObject = spDreamDemon;

		m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
		AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
	}

	// behavior tree node
	{
		FCompositeNodeDesc tSelectorNodeDesc;
		tSelectorNodeDesc.m_spOwnerGameObject = spDreamDemon;
		tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

		FCompositeNodeDesc tSequenceNodeDesc;
		tSequenceNodeDesc.m_spOwnerGameObject = spDreamDemon;
		tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

		FDecoratorBBBoolDesc tDecoBBBoolDesc;
		tDecoBBBoolDesc.m_spOwnerGameObject = spDreamDemon;
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
			tTaskDeadDesc.m_spOwnerGameObject = spDreamDemon;
			tTaskDeadDesc.m_wstrBTNodeName = L"DeadTask";
			tTaskDeadDesc.m_eBTNodeType = EBTNodeType::TASK;

			// dead task desc
			tTaskDeadDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Carrying;
			m_spRootNode->AddBTNode(CTaskDead::Create(make_shared<FTaskDeadDesc>(tTaskDeadDesc)));
		}

		// captured task node
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
				tTaskCapturedDesc.m_spOwnerGameObject = spDreamDemon;
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
				tTaskJumpLoopDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskJumpLoopDesc.m_wstrBTNodeName = L"JumpLoopTask";
				tTaskJumpLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpLoopDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_JumpLoop;
				spJumpSequenceNode->AddBTNode(CTaskJumpLoop::Create(make_shared<FTaskJumpLoopDesc>(tTaskJumpLoopDesc)));
			}

			// jump end task node
			{
				// task desc
				FTaskJumpEndDesc tTaskJumpEndDesc;
				tTaskJumpEndDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskJumpEndDesc.m_wstrBTNodeName = L"JumpEndTask";
				tTaskJumpEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpEndDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_JumpEnd;
				spJumpSequenceNode->AddBTNode(CTaskJumpEnd::Create(make_shared<FTaskJumpEndDesc>(tTaskJumpEndDesc)));
			}
		}

		// damage task node
		{
			// node desc
			FTaskDamageDesc tTaskDamageDesc;
			tTaskDamageDesc.m_spOwnerGameObject = spDreamDemon;
			tTaskDamageDesc.m_wstrBTNodeName = L"DamageTask";
			tTaskDamageDesc.m_eBTNodeType = EBTNodeType::TASK;

			// damage task desc
			tTaskDamageDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Damage;
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
				tTaskLookAtDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskLookAtDesc.m_wstrBTNodeName = L"AttackLookAtTask";
				tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

				// look at task desc
				tTaskLookAtDesc.m_spTarget = m_spPlayer;
				tTaskLookAtDesc.m_fMaxLerpDeltaSeconds = 1.0f;
				tTaskLookAtDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Walk;
				spAttackSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
			}

			// 거리가 너무 먼 경우
			// chase task node
			{
				// node desc
				FTaskChaseDesc tTaskChaseDesc;
				tTaskChaseDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskChaseDesc.m_wstrBTNodeName = L"ChaseTask";
				tTaskChaseDesc.m_eBTNodeType = EBTNodeType::TASK;

				// chase task desc;
				tTaskChaseDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Run;
				tTaskChaseDesc.m_spTarget = m_spPlayer;
				tTaskChaseDesc.m_fMaxChaseDeltaSeconds = 0.0f;
				tTaskChaseDesc.m_fAcceptableRadius = 6.0f;
				spAttackSequenceNode->AddBTNode(CTaskChase::Create(make_shared<FTaskChaseDesc>(tTaskChaseDesc)));
			}

			// attack start task node
			{
				// node desc
				FTaskAttackStartDesc tTaskAttackStartDesc;
				tTaskAttackStartDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskAttackStartDesc.m_wstrBTNodeName = L"AttackStartTask";
				tTaskAttackStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack start task desc
				tTaskAttackStartDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_FarSkill_Start;
				spAttackSequenceNode->AddBTNode(CTaskAttackStart::Create(make_shared<FTaskAttackStartDesc>(tTaskAttackStartDesc)));
			}

			// dream demon attack loop task node
			{
				FTaskDreamDemonAttackLoopDesc tAttackLoopDesc;
				tAttackLoopDesc.m_eBTNodeType = EBTNodeType::TASK;
				tAttackLoopDesc.m_spOwnerGameObject = spDreamDemon;
				tAttackLoopDesc.m_wstrBTNodeName = L"AttackLoopTask";

				tAttackLoopDesc.m_wstrProjectileEffectName = L"Group_DreamDemon_Ball";
				tAttackLoopDesc.m_eProjectileCollisionLayer = ECollisionLayer::MONSTER_PROJECTILE;
				tAttackLoopDesc.m_eProjectileCollsionFlag = ECollisionFlag::MONSTER_PROJECTILE;

				tAttackLoopDesc.m_spTarget = m_spPlayer;
				tAttackLoopDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_FarSkill_Action;
				spAttackSequenceNode->AddBTNode(CTaskDreamDemonAttackLoop::Create(make_shared<FTaskDreamDemonAttackLoopDesc>(tAttackLoopDesc)));
			}

			// attack end task node
			{
				// node desc
				FTaskAttackEndDesc tTaskAttackEndDesc;
				tTaskAttackEndDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskAttackEndDesc.m_wstrBTNodeName = L"AttackEndTask";
				tTaskAttackEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack end task desc
				tTaskAttackEndDesc.m_iAttackEndAnimationID = (int32)EDreamDemon::AS_DreamDemon_FarSkill_End;
				spAttackSequenceNode->AddBTNode(CTaskAttackEnd::Create(make_shared<FTaskAttackEndDesc>(tTaskAttackEndDesc)));
			}

			// 좌우 무작위 방향으로 이동합니다.
			// move task node
			{
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskMoveDesc.m_wstrBTNodeName = L"MoveToRandomTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				tTaskMoveDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Walk;
				spAttackSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// encount task node
			{
				// node desc
				FTaskEncountDesc tTaskEncountDesc;
				tTaskEncountDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskEncountDesc.m_wstrBTNodeName = L"EncountTask";
				tTaskEncountDesc.m_eBTNodeType = EBTNodeType::TASK;

				// encount task desc
				tTaskEncountDesc.m_spTarget = m_spPlayer;
				tTaskEncountDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Encount;
				spAttackSequenceNode->AddBTNode(CTaskEncount::Create(make_shared<FTaskEncountDesc>(tTaskEncountDesc)));
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
				tDecoSleepDesc.m_spOwnerGameObject = spDreamDemon;
				tDecoSleepDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
				tDecoSleepDesc.m_wstrBTNodeName = L"SleepDeco";
				spSleepSequenceNode->AddBTNode(CDecoratorSleep::Create(make_shared<FDecoratorSleepDesc>(tDecoSleepDesc)));
			}

			// sleep start task node
			{
				// node desc
				FTaskSleepStartDesc tTaskSleepStartDesc;
				tTaskSleepStartDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskSleepStartDesc.m_wstrBTNodeName = L"SleepStartTask";
				tTaskSleepStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep start task desc
				tTaskSleepStartDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_SleepStart;
				spSleepSequenceNode->AddBTNode(CTaskSleepStart::Create(make_shared<FTaskSleepStartDesc>(tTaskSleepStartDesc)));
			}

			// sleep loop task node
			{
				// node desc
				FTaskSleepLoopDesc tTaskSleepLoopDesc;
				tTaskSleepLoopDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskSleepLoopDesc.m_wstrBTNodeName = L"SleepLoopTask";
				tTaskSleepLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep loop task desc
				tTaskSleepLoopDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_SleepLoop;
				spSleepSequenceNode->AddBTNode(CTaskSleepLoop::Create(make_shared<FTaskSleepLoopDesc>(tTaskSleepLoopDesc)));
			}

			// sleep end task node
			{
				// node desc
				FTaskSleepEndDesc tTaskSleepEndDesc;
				tTaskSleepEndDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskSleepEndDesc.m_wstrBTNodeName = L"SleepEndTask";
				tTaskSleepEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep end task desc
				tTaskSleepEndDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_SleepEnd;
				spSleepSequenceNode->AddBTNode(CTaskSleepEnd::Create(make_shared<FTaskSleepEndDesc>(tTaskSleepEndDesc)));
			}
		}

		// near task node
		{
			// node desc
			FTaskNearDesc tTaskNearDesc;
			tTaskNearDesc.m_spOwnerGameObject = spDreamDemon;
			tTaskNearDesc.m_wstrBTNodeName = L"NearTask";
			tTaskNearDesc.m_eBTNodeType = EBTNodeType::TASK;

			// near task desc
			tTaskNearDesc.m_spPlayer = m_spPlayer;
			tTaskNearDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Idle;
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
				tTaskIdleDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskIdleDesc.m_wstrBTNodeName = L"IdleTask";
				tTaskIdleDesc.m_eBTNodeType = EBTNodeType::TASK;

				// idle task desc
				tTaskIdleDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Idle;
				tTaskIdleDesc.m_fMaxIdleDeltaSeconds = 10.0f;
				spWanderSequenceNode->AddBTNode(CTaskIdle::Create(make_shared<FTaskIdleDesc>(tTaskIdleDesc)));
			}

			// move task node
			{
				// node desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject		= spDreamDemon;
				tTaskMoveDesc.m_wstrBTNodeName			= L"WanderMoveTask";
				tTaskMoveDesc.m_eBTNodeType				= EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID			= (int32)EDreamDemon::AS_DreamDemon_Walk;
				tTaskMoveDesc.m_vSpawnPosition			= _spDreamDemonDesc->m_vInitPosition;
				spWanderSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			/*
			// rest task node
			{
				// node desc
				FTaskRestDesc tTaskRestDesc;
				tTaskRestDesc.m_spOwnerGameObject		= shared_from_this();
				tTaskRestDesc.m_wstrBTNodeName			= L"RestTask";
				tTaskRestDesc.m_eBTNodeType				= EBTNodeType::TASK;

				// rest task desc
				tTaskRestDesc.m_iAnimationID			= (int32)EDreamDemon::AS_SheepBall_Rest02;
				tTaskRestDesc.m_fMaxRestDeltaSeconds	= 10.0f;

				shared_ptr<CTaskRest> spTaskRestNode	= CTaskRest::Create(make_shared<FTaskRestDesc>(tTaskRestDesc));
				spWanderSequenceNode->AddBTNode(spTaskRestNode);
			}
			*/
		}
	}
}

void Client::CDreamDemon::InitializePartnerPalBT(const std::shared_ptr<FDreamDemonDesc> _spDreamDemonDesc, bool _bInit)
{
	shared_ptr<CGameObject> spDreamDemon = shared_from_this();

	// behavior tree component
	if (_bInit)
	{
		// component desc
		FBTComponentDesc tBTComponentDesc;
		tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
		tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
		tBTComponentDesc.m_eRenderType = ERenderType::NONE;
		tBTComponentDesc.m_wpOwnerGameObject = spDreamDemon;

		m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
		AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
	}

	// behavior tree node
	{
		FCompositeNodeDesc tSelectorNodeDesc;
		tSelectorNodeDesc.m_spOwnerGameObject = spDreamDemon;
		tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

		FCompositeNodeDesc tSequenceNodeDesc;
		tSequenceNodeDesc.m_spOwnerGameObject = spDreamDemon;
		tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

		FDecoratorBBBoolDesc tDecoBBBoolDesc;
		tDecoBBBoolDesc.m_spOwnerGameObject = spDreamDemon;
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
			tTaskDeadDesc.m_spOwnerGameObject = spDreamDemon;
			tTaskDeadDesc.m_wstrBTNodeName = L"DeadTask";
			tTaskDeadDesc.m_eBTNodeType = EBTNodeType::TASK;

			// dead task desc
			tTaskDeadDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Carrying;
			m_spRootNode->AddBTNode(CTaskDead::Create(make_shared<FTaskDeadDesc>(tTaskDeadDesc)));
		}

		// damage task node
		{
			// node desc
			FTaskDamageDesc tTaskDamageDesc;
			tTaskDamageDesc.m_spOwnerGameObject = spDreamDemon;
			tTaskDamageDesc.m_wstrBTNodeName = L"DamageTask";
			tTaskDamageDesc.m_eBTNodeType = EBTNodeType::TASK;

			// damage task desc
			tTaskDamageDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Damage;
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
				tTaskLookAtDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskLookAtDesc.m_wstrBTNodeName = L"AttackLookAtTask";
				tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

				// look at task desc
				tTaskLookAtDesc.m_spTarget = nullptr;
				tTaskLookAtDesc.m_fMaxLerpDeltaSeconds = 1.0f;
				tTaskLookAtDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Walk;
				spAttackSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
			}

			// chase task node
			{
				// node desc
				FTaskChaseDesc tTaskChaseDesc;
				tTaskChaseDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskChaseDesc.m_wstrBTNodeName = L"ChaseTask";
				tTaskChaseDesc.m_eBTNodeType = EBTNodeType::TASK;

				// chase task desc;
				tTaskChaseDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Run;
				tTaskChaseDesc.m_spTarget = nullptr;
				tTaskChaseDesc.m_fMaxChaseDeltaSeconds = 0.0f;
				tTaskChaseDesc.m_fAcceptableRadius = 6.0f;
				spAttackSequenceNode->AddBTNode(CTaskChase::Create(make_shared<FTaskChaseDesc>(tTaskChaseDesc)));
			}

			// attack start task node
			{
				// node desc
				FTaskAttackStartDesc tTaskAttackStartDesc;
				tTaskAttackStartDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskAttackStartDesc.m_wstrBTNodeName = L"AttackStartTask";
				tTaskAttackStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack start task desc
				tTaskAttackStartDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_FarSkill_Start;
				spAttackSequenceNode->AddBTNode(CTaskAttackStart::Create(make_shared<FTaskAttackStartDesc>(tTaskAttackStartDesc)));
			}

			// dream demon attack loop task node
			{
				FTaskDreamDemonAttackLoopDesc tAttackLoopDesc;
				tAttackLoopDesc.m_eBTNodeType = EBTNodeType::TASK;
				tAttackLoopDesc.m_spOwnerGameObject = spDreamDemon;
				tAttackLoopDesc.m_wstrBTNodeName = L"AttackLoopTask";

				tAttackLoopDesc.m_wstrProjectileEffectName = L"Group_DreamDemon_Ball";
				tAttackLoopDesc.m_eProjectileCollisionLayer = ECollisionLayer::ALLY_PROJECTILE;
				tAttackLoopDesc.m_eProjectileCollsionFlag = ECollisionFlag::ALLY_PROJECTILE;

				tAttackLoopDesc.m_spTarget = nullptr;
				tAttackLoopDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_FarSkill_Action;
				spAttackSequenceNode->AddBTNode(CTaskDreamDemonAttackLoop::Create(make_shared<FTaskDreamDemonAttackLoopDesc>(tAttackLoopDesc)));
			}

			// attack end task node
			{
				// node desc
				FTaskAttackEndDesc tTaskAttackEndDesc;
				tTaskAttackEndDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskAttackEndDesc.m_wstrBTNodeName = L"AttackEndTask";
				tTaskAttackEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack end task desc
				tTaskAttackEndDesc.m_iAttackEndAnimationID = (int32)EDreamDemon::AS_DreamDemon_FarSkill_End;
				spAttackSequenceNode->AddBTNode(CTaskAttackEnd::Create(make_shared<FTaskAttackEndDesc>(tTaskAttackEndDesc)));
			}

			// move task node
			{
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskMoveDesc.m_wstrBTNodeName = L"MoveToRandomTask";

				tTaskMoveDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Walk;
				spAttackSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// encount task node
			{
				// node desc
				FTaskEncountDesc tTaskEncountDesc;
				tTaskEncountDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskEncountDesc.m_wstrBTNodeName = L"EncountTask";
				tTaskEncountDesc.m_eBTNodeType = EBTNodeType::TASK;

				// encount task desc
				tTaskEncountDesc.m_spTarget = nullptr;
				tTaskEncountDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Encount;
				spAttackSequenceNode->AddBTNode(CTaskEncount::Create(make_shared<FTaskEncountDesc>(tTaskEncountDesc)));
			}
		}

		// follow selector node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"FollowSequence";
			shared_ptr<CCompositeNode> spFollowSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spFollowSequenceNode);

			// follow task node
			{
				// node desc
				FTaskFollowDesc tTaskFollowDesc;
				tTaskFollowDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskFollowDesc.m_wstrBTNodeName = L"FollowTask";
				tTaskFollowDesc.m_eBTNodeType = EBTNodeType::TASK;

				// follow task desc
				tTaskFollowDesc.m_spPlayer = m_spPlayer;
				tTaskFollowDesc.m_iWalkAnimationID = (int32)EDreamDemon::AS_DreamDemon_Walk;
				tTaskFollowDesc.m_iRunAnimationID = (int32)EDreamDemon::AS_DreamDemon_Run;
				spFollowSequenceNode->AddBTNode(CTaskFollow::Create(make_shared<FTaskFollowDesc>(tTaskFollowDesc)));
			}

			// turn task node
			{
				// task desc
				FTaskTurnDesc tTaskTurnDesc;
				tTaskTurnDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskTurnDesc.m_wstrBTNodeName = L"FollowTurnTask";
				tTaskTurnDesc.m_eBTNodeType = EBTNodeType::TASK;

				// turn task desc
				tTaskTurnDesc.m_spTarget = m_spPlayer;
				tTaskTurnDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Idle;
				spFollowSequenceNode->AddBTNode(CTaskTurn::Create(make_shared<FTaskTurnDesc>(tTaskTurnDesc)));
			}
		}
	}
}

void Client::CDreamDemon::InitializeSlavePalBT(const std::shared_ptr<FDreamDemonDesc> _spDreamDemonDesc, bool _bInit)
{
	shared_ptr<CGameObject> spDreamDemon = shared_from_this();

	// behavior tree component
	if (_bInit)
	{
		// component desc
		FBTComponentDesc tBTComponentDesc;
		tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
		tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
		tBTComponentDesc.m_eRenderType = ERenderType::NONE;
		tBTComponentDesc.m_wpOwnerGameObject = spDreamDemon;

		m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
		AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
	}

	// behavior tree node
	{
		FCompositeNodeDesc tSelectorNodeDesc;
		tSelectorNodeDesc.m_spOwnerGameObject = spDreamDemon;
		tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

		FCompositeNodeDesc tSequenceNodeDesc;
		tSequenceNodeDesc.m_spOwnerGameObject = spDreamDemon;
		tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

		FDecoratorBBBoolDesc tDecoBBBoolDesc;
		tDecoBBBoolDesc.m_spOwnerGameObject = spDreamDemon;
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
			tTaskDeadDesc.m_spOwnerGameObject = spDreamDemon;
			tTaskDeadDesc.m_wstrBTNodeName = L"DeadTask";
			tTaskDeadDesc.m_eBTNodeType = EBTNodeType::TASK;

			// dead task desc
			tTaskDeadDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Carrying;
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
				tTaskJumpLoopDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskJumpLoopDesc.m_wstrBTNodeName = L"JumpLoopTask";
				tTaskJumpLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpLoopDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_JumpLoop;
				spJumpSequenceNode->AddBTNode(CTaskJumpLoop::Create(make_shared<FTaskJumpLoopDesc>(tTaskJumpLoopDesc)));
			}

			// jump end task node
			{
				// task desc
				FTaskJumpEndDesc tTaskJumpEndDesc;
				tTaskJumpEndDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskJumpEndDesc.m_wstrBTNodeName = L"JumpEndTask";
				tTaskJumpEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpEndDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_JumpEnd;
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
				tDecoInteractedDesc.m_spOwnerGameObject = spDreamDemon;
				tDecoInteractedDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
				tDecoInteractedDesc.m_wstrBTNodeName = L"InteractedDeco";
				spInteractSequenceNode->AddBTNode(CDecoratorInteracted::Create(make_shared<FDecoratorInteractedDesc>(tDecoInteractedDesc)));
			}

			// turn task node
			{
				// node desc
				FTaskTurnDesc tTaskTurnDesc;
				tTaskTurnDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskTurnDesc.m_wstrBTNodeName = L"InteractedTurnTask";
				tTaskTurnDesc.m_eBTNodeType = EBTNodeType::TASK;

				// turn task desc
				tTaskTurnDesc.m_spTarget = m_spPlayer;
				tTaskTurnDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Idle;
				spInteractSequenceNode->AddBTNode(CTaskTurn::Create(make_shared<FTaskTurnDesc>(tTaskTurnDesc)));
			}

			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskMoveDesc.m_wstrBTNodeName = L"InteractMoveTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Run;
				spInteractSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// react task node
			{
				// task desc
				FTaskReactDesc tTaskReactDesc;
				tTaskReactDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskReactDesc.m_wstrBTNodeName = L"InteractReactTask";
				tTaskReactDesc.m_eBTNodeType = EBTNodeType::TASK;

				// react task desc
				tTaskReactDesc.m_iEatAnimationID = (int32)EDreamDemon::AS_DreamDemon_Eat;
				tTaskReactDesc.m_iPetAnimationID = (int32)EDreamDemon::AS_DreamDemon_Walk;
				tTaskReactDesc.m_iSlaughterAnimationID = (int32)EDreamDemon::AS_DreamDemon_Carrying;
				spInteractSequenceNode->AddBTNode(CTaskReact::Create(make_shared<FTaskReactDesc>(tTaskReactDesc)));
			}

			// emote task node
			{
				// task desc
				FTaskEmoteDesc tTaskEmoteDesc;
				tTaskEmoteDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskEmoteDesc.m_wstrBTNodeName = L"InteractEmoteTask";
				tTaskEmoteDesc.m_eBTNodeType = EBTNodeType::TASK;

				// react task desc
				tTaskEmoteDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Idle;
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
				tDecoSleepDesc.m_spOwnerGameObject = spDreamDemon;
				tDecoSleepDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
				tDecoSleepDesc.m_wstrBTNodeName = L"SleepDeco";
				spSleepSequenceNode->AddBTNode(CDecoratorSleep::Create(make_shared<FDecoratorSleepDesc>(tDecoSleepDesc)));
			}

			// 침대가 있는 경우, 침대로 이동합니다.
			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskMoveDesc.m_wstrBTNodeName = L"SleepMoveTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Walk;
				spSleepSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// sleep start task node
			{
				// node desc
				FTaskSleepStartDesc tTaskSleepStartDesc;
				tTaskSleepStartDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskSleepStartDesc.m_wstrBTNodeName = L"SleepStartTask";
				tTaskSleepStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep start task desc
				tTaskSleepStartDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_SleepStart;
				spSleepSequenceNode->AddBTNode(CTaskSleepStart::Create(make_shared<FTaskSleepStartDesc>(tTaskSleepStartDesc)));
			}

			// sleep loop task node
			{
				// node desc
				FTaskSleepLoopDesc tTaskSleepLoopDesc;
				tTaskSleepLoopDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskSleepLoopDesc.m_wstrBTNodeName = L"SleepLoopTask";
				tTaskSleepLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep loop task desc
				tTaskSleepLoopDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_SleepLoop;
				spSleepSequenceNode->AddBTNode(CTaskSleepLoop::Create(make_shared<FTaskSleepLoopDesc>(tTaskSleepLoopDesc)));
			}

			// sleep end task node
			{
				// node desc
				FTaskSleepEndDesc tTaskSleepEndDesc;
				tTaskSleepEndDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskSleepEndDesc.m_wstrBTNodeName = L"SleepEndTask";
				tTaskSleepEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep end task desc
				tTaskSleepEndDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_SleepEnd;
				spSleepSequenceNode->AddBTNode(CTaskSleepEnd::Create(make_shared<FTaskSleepEndDesc>(tTaskSleepEndDesc)));
			}
		}

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
				tTaskNoticeDesc.m_spOwnerGameObject = spDreamDemon;

				// notice task desc
				tTaskNoticeDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Idle;
				tTaskNoticeDesc.m_eMonsterNoticeType = EMonsterNoticeType::LIGHT_BULB;
				spToDoSequenceNode->AddBTNode(CTaskNotice::Create(make_shared<FTaskNoticeDesc>(tTaskNoticeDesc)));
			}

			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskMoveDesc.m_wstrBTNodeName = L"MoveToWorkTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Run;
				spToDoSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// todo task node
			{
				// task desc
				FTaskToDoDesc tTaskToDoDesc;
				tTaskToDoDesc.m_wstrBTNodeName = L"ToDoTask";
				tTaskToDoDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskToDoDesc.m_spOwnerGameObject = spDreamDemon;
				
				// todo task desc
				tTaskToDoDesc.m_iProduceAnimationID = (int32)EDreamDemon::AS_DreamDemon_CommonWork;
				tTaskToDoDesc.m_iBuildAnimationID = (int32)EDreamDemon::AS_DreamDemon_Architecture;
				tTaskToDoDesc.m_iTransportAnimationID = (int32)EDreamDemon::AS_DreamDemon_Transport;
				spToDoSequenceNode->AddBTNode(CTaskToDo::Create(make_shared<FTaskToDoDesc>(tTaskToDoDesc)));
			}

			// emote task node
			{
				// task desc
				FTaskEmoteDesc tTaskEmoteDesc;
				tTaskEmoteDesc.m_wstrBTNodeName = L"ToDoEmoteTask";
				tTaskEmoteDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskEmoteDesc.m_spOwnerGameObject = spDreamDemon;

				// emote task desc
				tTaskEmoteDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Idle;
				tTaskEmoteDesc.m_fMaxEmoteDeltaSeconds = 3.0f;
				spToDoSequenceNode->AddBTNode(CTaskEmote::Create(make_shared<FTaskEmoteDesc>(tTaskEmoteDesc)));
			}
		}

		/*
		// hunger sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"HungerSequence";
			shared_ptr<CCompositeNode> spHungerSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spHungerSequenceNode);

			// hunger deco node
			{
				tDecoBBBoolDesc.m_wstrBTNodeName = L"HungerDeco";
				tDecoBBBoolDesc.m_eBoolValue = EBlackboardBoolValue::BOOL_HUNGER;
				spHungerSequenceNode->AddBTNode(CDecoratorBBBool::Create(make_shared<FDecoratorBBBoolDesc>(tDecoBBBoolDesc)));
			}

			// notice task node
			{
				// task desc
				FTaskNoticeDesc tTaskNoticeDesc;
				tTaskNoticeDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskNoticeDesc.m_wstrBTNodeName = L"HungerNoticeTask";
				tTaskNoticeDesc.m_spOwnerGameObject = spDreamDemon;

				// notice task desc
				tTaskNoticeDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Idle;
				tTaskNoticeDesc.m_eMonsterNoticeType = EMonsterNoticeType::LIGHT_BULB;
				spHungerSequenceNode->AddBTNode(CTaskNotice::Create(make_shared<FTaskNoticeDesc>(tTaskNoticeDesc)));
			}

			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskMoveDesc.m_wstrBTNodeName = L"HungerMoveTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Run;
				spHungerSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// react task node
			{
				// task desc
				FTaskReactDesc tTaskReactDesc;
				tTaskReactDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskReactDesc.m_wstrBTNodeName = L"HungerReactTask";
				tTaskReactDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskReactDesc.m_iEatAnimationID = (int32)EDreamDemon::AS_DreamDemon_Eat;
				spHungerSequenceNode->AddBTNode(CTaskReact::Create(make_shared<FTaskReactDesc>(tTaskReactDesc)));
			}

			// emote task node
			{
				// task desc
				FTaskEmoteDesc tTaskEmoteDesc;
				tTaskEmoteDesc.m_wstrBTNodeName = L"HungerEmoteTask";
				tTaskEmoteDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskEmoteDesc.m_spOwnerGameObject = spDreamDemon;

				// emote task desc
				tTaskEmoteDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Idle;
				tTaskEmoteDesc.m_fMaxEmoteDeltaSeconds = 3.0f;
				spHungerSequenceNode->AddBTNode(CTaskEmote::Create(make_shared<FTaskEmoteDesc>(tTaskEmoteDesc)));
			}
		}
		*/

		// wander sequence node
		{
			tSequenceNodeDesc.m_wstrBTNodeName = L"WanderSequence";
			shared_ptr<CCompositeNode> spWanderSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
			m_spRootNode->AddBTNode(spWanderSequenceNode);

			// idle task node
			{
				// node desc
				FTaskIdleDesc tTaskIdleDesc;
				tTaskIdleDesc.m_spOwnerGameObject = spDreamDemon;
				tTaskIdleDesc.m_wstrBTNodeName = L"WanderIdleTask";
				tTaskIdleDesc.m_eBTNodeType = EBTNodeType::TASK;

				// idle task desc
				tTaskIdleDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Idle;
				tTaskIdleDesc.m_fMaxIdleDeltaSeconds = 5.0f;
				spWanderSequenceNode->AddBTNode(CTaskIdle::Create(make_shared<FTaskIdleDesc>(tTaskIdleDesc)));
			}

			// move task node
			{
				// node desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject	= spDreamDemon;
				tTaskMoveDesc.m_wstrBTNodeName		= L"WanderMoveTask";
				tTaskMoveDesc.m_eBTNodeType			= EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EDreamDemon::AS_DreamDemon_Walk;

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
				tTaskRestDesc.m_spOwnerGameObject = spSheepBall;
				tTaskRestDesc.m_wstrBTNodeName = L"RestTask";
				tTaskRestDesc.m_eBTNodeType = EBTNodeType::TASK;

				// rest task desc
				tTaskRestDesc.m_iAnimationID = (int32)EDreamDemon::AS_SheepBall_Rest03;
				tTaskRestDesc.m_fMaxRestDeltaSeconds = 5.0f;
				spWanderSequenceNode->AddBTNode(CTaskRest::Create(make_shared<FTaskRestDesc>(tTaskRestDesc)));
			}
			*/
		}
	}
}

HRESULT Client::CDreamDemon::ReInitializePalBT(EMonsterAIType _eMonsterAIType)
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

void Client::CDreamDemon::LookAt(const Vector3& _vPosition)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->LookAt(_vPosition);
}

void Client::CDreamDemon::Turn(const Vector3& _vAxis, float _fAngularSpeed)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->Turn(_vAxis, _fAngularSpeed);
}

void Client::CDreamDemon::MoveForward(float _fSpeed)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->MoveColliderForward(_fSpeed);
}

void Client::CDreamDemon::MoveBackward(float _fSpeed)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->MoveColliderBackward(_fSpeed);
}

void Client::CDreamDemon::MoveRight(float _fSpeed)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->MoveColliderRight(_fSpeed);
}

void Client::CDreamDemon::MoveLeft(float _fSpeed)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->MoveColliderLeft(_fSpeed);
}

void Client::CDreamDemon::MoveUp(float _fSpeed)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->MoveColliderUp(_fSpeed);
}

void Client::CDreamDemon::MoveDown(float _fSpeed)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->MoveColliderDown(_fSpeed);
}

void Client::CDreamDemon::SetPosition(const Vector3& _vPosition)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->SetPosition(_vPosition);
}

const std::optional<Vector3> Client::CDreamDemon::GetPosition() const
{
	if (!m_spBodyBoxComponent) { return nullopt; }

	return m_spBodyBoxComponent->GetPosition();
}

void Client::CDreamDemon::UpdateBlackboardValues()
{
	CMonster::UpdateBlackboardValues();
}

void Client::CDreamDemon::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CMonster::OnTriggerEnter(_pSrcComponent, _pDstComponent);

	// 컨트롤러 컴포넌트
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
		CControllerComponent* pDstControllerComponent = dynamic_cast<CControllerComponent*>(_pDstComponent);

		ECollisionLayer eSrcCollisionLayer = pSrcColliderComponent->GetCollisionLayer();
		ECollisionLayer eDstCollisionLayer = pDstControllerComponent->GetCollisionLayer();
		return;
	}

	// 콜라이더 컴포넌트
	CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	ECollisionLayer eSrcCollisionLayer = pSrcColliderComponent->GetCollisionLayer();
	ECollisionLayer eDstCollisionLayer = pDstColliderComponent->GetCollisionLayer();
}

void Client::CDreamDemon::OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CMonster::OnCollisionEnter(_pSrcComponent, _pDstComponent);
}

void Client::CDreamDemon::SetAllyCollisionLayer()
{
	m_spBodyBoxComponent->SetCollisionLayer(ECollisionLayer::ALLY, ECollisionFlag::ALLY);
}

shared_ptr<CColliderComponent> Client::CDreamDemon::GetBodyColliderComponent()
{
	if (!m_spBodyBoxComponent) { return nullptr; }

	return m_spBodyBoxComponent;
}

void Client::CDreamDemon::SetLinearVelocity(const PxVec3& _vLinearVelocity)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->SetLinearVelocity(_vLinearVelocity);
}

void Client::CDreamDemon::SetAngularVelocity(const PxVec3& _vLinearVelocity)
{
	if (!m_spBodyBoxComponent) { return; }

	m_spBodyBoxComponent->SetAngularVelocity(_vLinearVelocity);
}
