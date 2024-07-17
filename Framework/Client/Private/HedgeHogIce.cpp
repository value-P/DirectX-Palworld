// default
#include "pch.h"
#include "HedgeHogIce.h"

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
#include "TaskChase.h"
#include "TaskCaptured.h"
#include "TaskDamaged.h"
#include "TaskDead.h"
#include "TaskEncount.h"
#include "TaskEmote.h"
#include "TaskFollow.h"
#include "TaskHedgeHogAttackLoop.h"
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
#include "Ranch.h"

std::shared_ptr<CHedgeHogIce> Client::CHedgeHogIce::Create(const std::shared_ptr<FHedgeHogIceDesc>& _spHedgeHogIceDesc)
{
	const std::shared_ptr<CHedgeHogIce> spHedgeHogIceDesc = make_shared<CHedgeHogIce>();
	ENSUREF(spHedgeHogIceDesc->Initialize(_spHedgeHogIceDesc), L"Failed to initialize hedgehogice.");
	return spHedgeHogIceDesc;
}

HRESULT Client::CHedgeHogIce::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CMonster::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FHedgeHogIceDesc> spHedgeHogIceDesc = static_pointer_cast<FHedgeHogIceDesc>(_spGameObjectDesc);
	InitHedgeHogIce(spHedgeHogIceDesc);
	InitMeshComponent();
	InitColliderComponent();
	InitBehaviorTreeComponent(spHedgeHogIceDesc);
	return S_OK;
}

HRESULT Client::CHedgeHogIce::BeginPlay()
{
	if (FAILED(CMonster::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CHedgeHogIce::PreTick(float _fDeltaSeconds)
{
	switch (CMonster::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CHedgeHogIce::Tick(float _fDeltaSeconds)
{
	switch (CMonster::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CHedgeHogIce::PostTick(float _fDeltaSeconds)
{
	switch (CMonster::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CHedgeHogIce::EndPlay()
{
	if (FAILED(CMonster::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CHedgeHogIce::Release()
{
	m_spBodySphereComponent = nullptr;
	m_spAttackSphereComponent = nullptr;

	CMonster::Release();
}

void Client::CHedgeHogIce::InitHedgeHogIce(const shared_ptr<FHedgeHogIceDesc>& _spHedgeHogDesc)
{
	// gameobject desc
	m_wstrGameObjectName = L"HedgeHogIce";
	m_eMobilityType = EMobilityType::DYNAMIC;

	// monster desc
	m_eMonsterType = EMonsterType::HEDGE_HOG_ICE;
	m_eMonsterSizeType = EMonsterSizeType::MIDUM;
}

void Client::CHedgeHogIce::InitMeshComponent()
{
	// component desc
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

	// mesh component desc
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/Hedgehog_Ice.dat";
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0003f) * Matrix::CreateRotationY(XMConvertToRadians(180.0f));
	tMeshComponentDesc.m_wstrShaderName = L"";

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	ENSURE(AddComponent(tMeshComponentDesc.m_wstrComponentName, m_spMeshComponent));

	// 운반 본 포인터를 구합니다.
	m_wpTransportBone = m_spMeshComponent->GetBonePtr(L"Socket_Transport");
}

void Client::CHedgeHogIce::InitColliderComponent()
{
	// body sphere component
	{
		// component desc
		FSphereComponentDesc tSphereComponentDesc;
		tSphereComponentDesc.m_eComponentType = EComponentType::SPHERE;
		tSphereComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tSphereComponentDesc.m_wstrComponentName = L"BodySphereComponent";
		tSphereComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		// collider component desc
		tSphereComponentDesc.m_bTrigger = false;
		tSphereComponentDesc.m_bGravity = true;
		tSphereComponentDesc.m_eColliderType = EColliderType::SPHERE;
		tSphereComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		tSphereComponentDesc.m_bDeferredAdd = true;

		switch (m_eMonsterAIType)
		{
		case Client::EMonsterAIType::WILD_PAL:
			tSphereComponentDesc.m_eCollisionFlag = ECollisionFlag::MONSTER;
			tSphereComponentDesc.m_eCollisionLayer = ECollisionLayer::MONSTER;
			break;

		case Client::EMonsterAIType::PARTNER_PAL:
		case Client::EMonsterAIType::SLAVE_PAL:
			tSphereComponentDesc.m_eCollisionFlag = ECollisionFlag::ALLY;
			tSphereComponentDesc.m_eCollisionLayer = ECollisionLayer::ALLY;
			break;

		default:
			CHECKF(false, L"Monster AI type is not designated.");
			break;
		}

		tSphereComponentDesc.m_fMass = 10.0f;
		tSphereComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
		tSphereComponentDesc.m_vLocalPosition = Vector3(0.0f, 1.0f, 0.0f);
		tSphereComponentDesc.m_vMaterial = Vector3(1.0f, 1.0f, 0.05f);

		// sphere component desc
		tSphereComponentDesc.m_fRadius = 1.0f;

		m_spBodySphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		ENSURE(AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spBodySphereComponent));
	}

	// attack sphere component
	{
		// component desc
		FSphereComponentDesc tSphereComponentDesc;
		tSphereComponentDesc.m_eComponentType = EComponentType::SPHERE;
		tSphereComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tSphereComponentDesc.m_wstrComponentName = L"AttackSphereComponent";
		tSphereComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		// collider component desc
		tSphereComponentDesc.m_bTrigger = true;
		tSphereComponentDesc.m_bGravity = false;
		tSphereComponentDesc.m_eColliderType = EColliderType::SPHERE;
		tSphereComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		tSphereComponentDesc.m_bDeferredAdd = true;

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

		tSphereComponentDesc.m_fMass = 0.0f;
		tSphereComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
		tSphereComponentDesc.m_vLocalPosition = Vector3(0.0f, 1.0f, 0.0f);
		tSphereComponentDesc.m_vMaterial = Vector3(1.0f, 1.0f, 0.05f);

		// sphere component desc
		tSphereComponentDesc.m_fRadius = 6.0f;

		m_spAttackSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		ENSURE(AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spAttackSphereComponent));
	}
}

void Client::CHedgeHogIce::InitBehaviorTreeComponent(const std::shared_ptr<FHedgeHogIceDesc>& _spHedgeHogIceDesc)
{
	// 1. 야생 팰 비헤이비어 트리
	if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
	{
		InitializeWildPalBT(_spHedgeHogIceDesc);
	}

	// 2. 파트너 팰 비헤이비어 트리
	else if (m_eMonsterAIType == EMonsterAIType::PARTNER_PAL)
	{
		InitializePartnerPalBT(_spHedgeHogIceDesc);
	}

	// 3. 거점 팰 비헤이비어 트리
	else if (m_eMonsterAIType == EMonsterAIType::SLAVE_PAL)
	{
		InitializeSlavePalBT(_spHedgeHogIceDesc);
	}

	// behavior tree desc
	m_spBehaviorTreeComponent->SetRootNode(m_spRootNode);
	m_spBehaviorTreeComponent->SetBehaveTree(true);
}

void Client::CHedgeHogIce::InitializeWildPalBT(const std::shared_ptr<FHedgeHogIceDesc> _spHedgeHogIceDesc, bool _bInit)
{
	shared_ptr<CGameObject> spHedgeHog = shared_from_this();

	// behavior tree component
	if (_bInit)
	{
		// component desc
		FBTComponentDesc tBTComponentDesc;
		tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
		tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
		tBTComponentDesc.m_eRenderType = ERenderType::NONE;
		tBTComponentDesc.m_wpOwnerGameObject = spHedgeHog;

		m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
		AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
	}

	// behavior tree node
	{
		FCompositeNodeDesc tSelectorNodeDesc;
		tSelectorNodeDesc.m_spOwnerGameObject = spHedgeHog;
		tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

		FCompositeNodeDesc tSequenceNodeDesc;
		tSequenceNodeDesc.m_spOwnerGameObject = spHedgeHog;
		tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

		FDecoratorBBBoolDesc tDecoBBBoolDesc;
		tDecoBBBoolDesc.m_spOwnerGameObject = spHedgeHog;
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
			tTaskDeadDesc.m_spOwnerGameObject = spHedgeHog;
			tTaskDeadDesc.m_wstrBTNodeName = L"DeadTask";
			tTaskDeadDesc.m_eBTNodeType = EBTNodeType::TASK;

			// dead task desc
			tTaskDeadDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Carrying;
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
				tTaskCapturedDesc.m_spOwnerGameObject = spHedgeHog;
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
				tTaskJumpLoopDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskJumpLoopDesc.m_wstrBTNodeName = L"JumpLoopTask";
				tTaskJumpLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpLoopDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_JumpLoop;
				spJumpSequenceNode->AddBTNode(CTaskJumpLoop::Create(make_shared<FTaskJumpLoopDesc>(tTaskJumpLoopDesc)));
			}

			// jump end task node
			{
				// task desc
				FTaskJumpEndDesc tTaskJumpEndDesc;
				tTaskJumpEndDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskJumpEndDesc.m_wstrBTNodeName = L"JumpEndTask";
				tTaskJumpEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpEndDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_JumpEnd;
				spJumpSequenceNode->AddBTNode(CTaskJumpEnd::Create(make_shared<FTaskJumpEndDesc>(tTaskJumpEndDesc)));
			}
		}

		// damage task node
		{
			// node desc
			FTaskDamageDesc tTaskDamageDesc;
			tTaskDamageDesc.m_spOwnerGameObject = spHedgeHog;
			tTaskDamageDesc.m_wstrBTNodeName = L"DamageTask";
			tTaskDamageDesc.m_eBTNodeType = EBTNodeType::TASK;

			// damage task desc
			tTaskDamageDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Damage;
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
				tTaskLookAtDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskLookAtDesc.m_wstrBTNodeName = L"AttackLookAtTask";
				tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

				// look at task desc
				tTaskLookAtDesc.m_spTarget = m_spPlayer;
				tTaskLookAtDesc.m_fMaxLerpDeltaSeconds = 1.0f;
				tTaskLookAtDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Walk;
				spAttackSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
			}

			// withdraw task node
			{
				// task desc
				FTaskWithdrawDesc tTaskWithdrawDesc;
				tTaskWithdrawDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskWithdrawDesc.m_wstrBTNodeName = L"WithdrawTask";
				tTaskWithdrawDesc.m_eBTNodeType = EBTNodeType::TASK;

				// withdraw task desc
				tTaskWithdrawDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Idle;
				tTaskWithdrawDesc.m_fMinDistance = 6.0f;
				spAttackSequenceNode->AddBTNode(CTaskWithdraw::Create(make_shared<FTaskWithdrawDesc>(tTaskWithdrawDesc)));
			}

			// encount task node
			{
				// node desc
				FTaskEncountDesc tTaskEncountDesc;
				tTaskEncountDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskEncountDesc.m_wstrBTNodeName = L"EncountTask";
				tTaskEncountDesc.m_eBTNodeType = EBTNodeType::TASK;

				// encount task desc
				tTaskEncountDesc.m_spTarget = m_spPlayer;
				tTaskEncountDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Encount;
				spAttackSequenceNode->AddBTNode(CTaskEncount::Create(make_shared<FTaskEncountDesc>(tTaskEncountDesc)));
			}

			// chase task node
			{
				// node desc
				FTaskChaseDesc tTaskChaseDesc;
				tTaskChaseDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskChaseDesc.m_wstrBTNodeName = L"ChaseTask";
				tTaskChaseDesc.m_eBTNodeType = EBTNodeType::TASK;

				// chase task desc
				tTaskChaseDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Run;
				tTaskChaseDesc.m_spTarget = m_spPlayer;
				tTaskChaseDesc.m_fMaxChaseDeltaSeconds = 0.0f;
				tTaskChaseDesc.m_fAcceptableRadius = 3.0f;
				spAttackSequenceNode->AddBTNode(CTaskChase::Create(make_shared<FTaskChaseDesc>(tTaskChaseDesc)));
			}

			// attack start task node
			{
				FTaskAttackStartDesc tTaskAttackStartDesc;
				tTaskAttackStartDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskAttackStartDesc.m_wstrBTNodeName = L"AttackStartTask";
				tTaskAttackStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				tTaskAttackStartDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_FarSkill_Start;
				spAttackSequenceNode->AddBTNode(CTaskAttackStart::Create(make_shared<FTaskAttackStartDesc>(tTaskAttackStartDesc)));
			}

			// hedgehog attack loop task node
			{
				// task desc
				FTaskHedgeHogAttackLoopDesc tTaskAttackLoopDesc;
				tTaskAttackLoopDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskAttackLoopDesc.m_wstrBTNodeName = L"AttackLoopTask";
				tTaskAttackLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack loop task desc
				tTaskAttackLoopDesc.m_iActionAnimationID = (int32)EHedgeHog::AS_Hedgehog_FarSkill_Action;
				tTaskAttackLoopDesc.m_iActionLoopAnimationID = (int32)EHedgeHog::AS_Hedgehog_FarSkill_ActionLoop;
				spAttackSequenceNode->AddBTNode(CTaskHedgeHogAttackLoop::Create(make_shared<FTaskHedgeHogAttackLoopDesc>(tTaskAttackLoopDesc)));
			}

			// attack end task node
			{
				FTaskAttackEndDesc tTaskAttackEndDesc;
				tTaskAttackEndDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskAttackEndDesc.m_wstrBTNodeName = L"AttackEndTask";
				tTaskAttackEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				tTaskAttackEndDesc.m_iAttackEndAnimationID = (int32)EHedgeHog::AS_Hedgehog_FarSkill_End;
				spAttackSequenceNode->AddBTNode(CTaskAttackEnd::Create(make_shared<FTaskAttackEndDesc>(tTaskAttackEndDesc)));
			}

			// move task node
			{
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskMoveDesc.m_wstrBTNodeName = L"AttackMoveTask";

				tTaskMoveDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Run;
				spAttackSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}
		}

		// runaway node
		{
			// task desc
			FTaskRunawayDesc tRunawayDesc;
			tRunawayDesc.m_spOwnerGameObject = spHedgeHog;
			tRunawayDesc.m_wstrBTNodeName = L"RunawayTask";
			tRunawayDesc.m_eBTNodeType = EBTNodeType::TASK;

			// runaway task desc
			tRunawayDesc.m_spTarget = m_spPlayer;
			tRunawayDesc.m_fMaxRunawayDeltaSeconds = 10.0f;
			tRunawayDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Run;

			m_spRootNode->AddBTNode(CTaskRunaway::Create(make_shared<FTaskRunawayDesc>(tRunawayDesc)));
		}

		// near task node
		{
			// node desc
			FTaskNearDesc tTaskNearDesc;
			tTaskNearDesc.m_spOwnerGameObject = spHedgeHog;
			tTaskNearDesc.m_wstrBTNodeName = L"NearTask";
			tTaskNearDesc.m_eBTNodeType = EBTNodeType::TASK;

			// near task desc
			tTaskNearDesc.m_spPlayer = m_spPlayer;
			tTaskNearDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Idle;
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
				tTaskIdleDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskIdleDesc.m_wstrBTNodeName = L"IdleTask";
				tTaskIdleDesc.m_eBTNodeType = EBTNodeType::TASK;

				// idle task desc
				tTaskIdleDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Idle;
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
				tTaskMoveDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Walk;
				tTaskMoveDesc.m_vSpawnPosition = _spHedgeHogIceDesc->m_vInitPosition;

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
				tTaskRestDesc.m_iAnimationID = (int32)EHEDGEHOG::AS_PinkCat_Rest02;
				tTaskRestDesc.m_fMaxRestDeltaSeconds = 10.0f;
				shared_ptr<CTaskRest> spTaskRestNode = CTaskRest::Create(make_shared<FTaskRestDesc>(tTaskRestDesc));
				spWanderSequenceNode->AddBTNode(spTaskRestNode);
			}
			*/
		}
	}
}

void Client::CHedgeHogIce::InitializePartnerPalBT(const std::shared_ptr<FHedgeHogIceDesc> _spHedgeHogIceDesc, bool _bInit)
{
	shared_ptr<CGameObject> spHedgeHog = shared_from_this();

	// behavior tree component
	if (_bInit)
	{
		// component desc
		FBTComponentDesc tBTComponentDesc;
		tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
		tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
		tBTComponentDesc.m_eRenderType = ERenderType::NONE;
		tBTComponentDesc.m_wpOwnerGameObject = spHedgeHog;

		m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
		AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
	}

	// behavior tree node
	{
		FCompositeNodeDesc tSelectorNodeDesc;
		tSelectorNodeDesc.m_spOwnerGameObject = spHedgeHog;
		tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

		FCompositeNodeDesc tSequenceNodeDesc;
		tSequenceNodeDesc.m_spOwnerGameObject = spHedgeHog;
		tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

		FDecoratorBBBoolDesc tDecoBBBoolDesc;
		tDecoBBBoolDesc.m_spOwnerGameObject = spHedgeHog;
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
			tTaskDeadDesc.m_spOwnerGameObject = spHedgeHog;
			tTaskDeadDesc.m_wstrBTNodeName = L"DeadTask";
			tTaskDeadDesc.m_eBTNodeType = EBTNodeType::TASK;

			// dead task desc
			tTaskDeadDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Carrying;
			m_spRootNode->AddBTNode(CTaskDead::Create(make_shared<FTaskDeadDesc>(tTaskDeadDesc)));
		}

		// damage task node
		{
			// node desc
			FTaskDamageDesc tTaskDamageDesc;
			tTaskDamageDesc.m_spOwnerGameObject = spHedgeHog;
			tTaskDamageDesc.m_wstrBTNodeName = L"DamageTask";
			tTaskDamageDesc.m_eBTNodeType = EBTNodeType::TASK;

			// damage task desc
			tTaskDamageDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Damage;
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

			// withdraw task node
			{
				// task desc
				FTaskWithdrawDesc tTaskWithdrawDesc;
				tTaskWithdrawDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskWithdrawDesc.m_wstrBTNodeName = L"WithdrawTask";
				tTaskWithdrawDesc.m_eBTNodeType = EBTNodeType::TASK;

				// withdraw task desc
				tTaskWithdrawDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Idle;
				spAttackSequenceNode->AddBTNode(CTaskWithdraw::Create(make_shared<FTaskWithdrawDesc>(tTaskWithdrawDesc)));
			}

			// encount task node
			{
				// node desc
				FTaskEncountDesc tTaskEncountDesc;
				tTaskEncountDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskEncountDesc.m_wstrBTNodeName = L"EncountTask";
				tTaskEncountDesc.m_eBTNodeType = EBTNodeType::TASK;

				// encount task desc
				tTaskEncountDesc.m_spTarget = nullptr;
				tTaskEncountDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Encount;
				spAttackSequenceNode->AddBTNode(CTaskEncount::Create(make_shared<FTaskEncountDesc>(tTaskEncountDesc)));
			}

			// chase task node
			{
				// node desc
				FTaskChaseDesc tTaskChaseDesc;
				tTaskChaseDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskChaseDesc.m_wstrBTNodeName = L"ChaseTask";
				tTaskChaseDesc.m_eBTNodeType = EBTNodeType::TASK;

				// chase task desc;
				tTaskChaseDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Run;
				tTaskChaseDesc.m_spTarget = nullptr;
				tTaskChaseDesc.m_fMaxChaseDeltaSeconds = 0.0f;
				tTaskChaseDesc.m_fAcceptableRadius = 5.0f;
				spAttackSequenceNode->AddBTNode(CTaskChase::Create(make_shared<FTaskChaseDesc>(tTaskChaseDesc)));
			}

			// attack start task node
			{
				// node desc
				FTaskAttackStartDesc tTaskAttackStartDesc;
				tTaskAttackStartDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskAttackStartDesc.m_wstrBTNodeName = L"AttackStartTask";
				tTaskAttackStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack start task desc
				tTaskAttackStartDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_FarSkill_Start;
				spAttackSequenceNode->AddBTNode(CTaskAttackStart::Create(make_shared<FTaskAttackStartDesc>(tTaskAttackStartDesc)));
			}

			// hedgehog attack loop task node
			{
				// task desc
				FTaskHedgeHogAttackLoopDesc tTaskAttackLoopDesc;
				tTaskAttackLoopDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskAttackLoopDesc.m_wstrBTNodeName = L"AttackLoopTask";
				tTaskAttackLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack loop task desc
				tTaskAttackLoopDesc.m_iActionAnimationID = (int32)EHedgeHog::AS_Hedgehog_FarSkill_Action;
				tTaskAttackLoopDesc.m_iActionLoopAnimationID = (int32)EHedgeHog::AS_Hedgehog_FarSkill_ActionLoop;
				spAttackSequenceNode->AddBTNode(CTaskHedgeHogAttackLoop::Create(make_shared<FTaskHedgeHogAttackLoopDesc>(tTaskAttackLoopDesc)));
			}

			// attack end task node
			{
				// node desc
				FTaskAttackEndDesc tTaskAttackEndDesc;
				tTaskAttackEndDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskAttackEndDesc.m_wstrBTNodeName = L"AttackEndTask";
				tTaskAttackEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// attack end task desc
				tTaskAttackEndDesc.m_iAttackEndAnimationID = (int32)EHedgeHog::AS_Hedgehog_FarSkill_End;
				spAttackSequenceNode->AddBTNode(CTaskAttackEnd::Create(make_shared<FTaskAttackEndDesc>(tTaskAttackEndDesc)));
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
				tTaskFollowDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskFollowDesc.m_wstrBTNodeName = L"FollowTask";
				tTaskFollowDesc.m_eBTNodeType = EBTNodeType::TASK;

				// follow task desc
				tTaskFollowDesc.m_spPlayer = m_spPlayer;
				tTaskFollowDesc.m_iWalkAnimationID = (int32)EHedgeHog::AS_Hedgehog_Walk;
				tTaskFollowDesc.m_iRunAnimationID = (int32)EHedgeHog::AS_Hedgehog_Run;
				spFollowSequenceNode->AddBTNode(CTaskFollow::Create(make_shared<FTaskFollowDesc>(tTaskFollowDesc)));
			}

			// turn task node
			{
				// task desc
				FTaskTurnDesc tTaskTurnDesc;
				tTaskTurnDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskTurnDesc.m_wstrBTNodeName = L"FollowTurnTask";
				tTaskTurnDesc.m_eBTNodeType = EBTNodeType::TASK;

				// turn task desc
				tTaskTurnDesc.m_spTarget = m_spPlayer;
				tTaskTurnDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Idle;
				spFollowSequenceNode->AddBTNode(CTaskTurn::Create(make_shared<FTaskTurnDesc>(tTaskTurnDesc)));
			}
		}
	}
}

void Client::CHedgeHogIce::InitializeSlavePalBT(const std::shared_ptr<FHedgeHogIceDesc> _spHedgeHogIceDesc, bool _bInit)
{
	shared_ptr<CGameObject> spHedgeHog = shared_from_this();

	// behavior tree component
	if (_bInit)
	{
		// component desc
		FBTComponentDesc tBTComponentDesc;
		tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
		tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
		tBTComponentDesc.m_eRenderType = ERenderType::NONE;
		tBTComponentDesc.m_wpOwnerGameObject = spHedgeHog;

		m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
		AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
	}

	// behavior tree node
	{
		FCompositeNodeDesc tSelectorNodeDesc;
		tSelectorNodeDesc.m_spOwnerGameObject = spHedgeHog;
		tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

		FCompositeNodeDesc tSequenceNodeDesc;
		tSequenceNodeDesc.m_spOwnerGameObject = spHedgeHog;
		tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

		FDecoratorBBBoolDesc tDecoBBBoolDesc;
		tDecoBBBoolDesc.m_spOwnerGameObject = spHedgeHog;
		tDecoBBBoolDesc.m_eBTNodeType = EBTNodeType::DECORATOR;

		// root selector node
		{
			tSelectorNodeDesc.m_bRoot = true;
			tSelectorNodeDesc.m_wstrBTNodeName = L"RootSelector";
			m_spRootNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSelectorNodeDesc));

			tSelectorNodeDesc.m_bRoot = false;
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
				tTaskJumpLoopDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskJumpLoopDesc.m_wstrBTNodeName = L"JumpLoopTask";
				tTaskJumpLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpLoopDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_JumpLoop;
				spJumpSequenceNode->AddBTNode(CTaskJumpLoop::Create(make_shared<FTaskJumpLoopDesc>(tTaskJumpLoopDesc)));
			}

			// jump end task node
			{
				// task desc
				FTaskJumpEndDesc tTaskJumpEndDesc;
				tTaskJumpEndDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskJumpEndDesc.m_wstrBTNodeName = L"JumpEndTask";
				tTaskJumpEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpEndDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_JumpEnd;
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
				tDecoInteractedDesc.m_spOwnerGameObject = spHedgeHog;
				tDecoInteractedDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
				tDecoInteractedDesc.m_wstrBTNodeName = L"InteractedDeco";
				spInteractSequenceNode->AddBTNode(CDecoratorInteracted::Create(make_shared<FDecoratorInteractedDesc>(tDecoInteractedDesc)));
			}

			// turn task node
			{
				// node desc
				FTaskTurnDesc tTaskTurnDesc;
				tTaskTurnDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskTurnDesc.m_wstrBTNodeName = L"InteractedTurnTask";
				tTaskTurnDesc.m_eBTNodeType = EBTNodeType::TASK;

				// turn task desc
				tTaskTurnDesc.m_spTarget = m_spPlayer;
				tTaskTurnDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Idle;
				spInteractSequenceNode->AddBTNode(CTaskTurn::Create(make_shared<FTaskTurnDesc>(tTaskTurnDesc)));
			}

			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskMoveDesc.m_wstrBTNodeName = L"InteractedMoveTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Walk;
				spInteractSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// react task node
			{
				// task desc
				FTaskReactDesc tTaskReactDesc;
				tTaskReactDesc.m_wstrBTNodeName = L"InteractedReactTask";
				tTaskReactDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskReactDesc.m_spOwnerGameObject = spHedgeHog;

				// react task desc
				tTaskReactDesc.m_iEatAnimationID = (int32)EHedgeHog::AS_Hedgehog_Eat;
				tTaskReactDesc.m_iPetAnimationID = (int32)EHedgeHog::AS_Hedgehog_Petting;
				tTaskReactDesc.m_iSlaughterAnimationID = (int32)EHedgeHog::AS_Hedgehog_Carrying;
				spInteractSequenceNode->AddBTNode(CTaskReact::Create(make_shared<FTaskReactDesc>(tTaskReactDesc)));
			}

			// emote task node
			{
				// task desc
				FTaskEmoteDesc tTaskEmoteDesc;
				tTaskEmoteDesc.m_wstrBTNodeName = L"InteractedEmoteTask";
				tTaskEmoteDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskEmoteDesc.m_spOwnerGameObject = spHedgeHog;

				// react task desc
				tTaskEmoteDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Idle;
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
				tDecoSleepDesc.m_spOwnerGameObject = spHedgeHog;
				tDecoSleepDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
				tDecoSleepDesc.m_wstrBTNodeName = L"SleepDeco";
				spSleepSequenceNode->AddBTNode(CDecoratorSleep::Create(make_shared<FDecoratorSleepDesc>(tDecoSleepDesc)));
			}

			// 침대가 있는 경우, 침대로 이동합니다.
			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskMoveDesc.m_wstrBTNodeName = L"SleepMoveTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Walk;
				spSleepSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// sleep start task node
			{
				// node desc
				FTaskSleepStartDesc tTaskSleepStartDesc;
				tTaskSleepStartDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskSleepStartDesc.m_wstrBTNodeName = L"SleepStartTask";
				tTaskSleepStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep start task desc
				tTaskSleepStartDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_SleepStart;
				spSleepSequenceNode->AddBTNode(CTaskSleepStart::Create(make_shared<FTaskSleepStartDesc>(tTaskSleepStartDesc)));
			}

			// sleep loop task node
			{
				// node desc
				FTaskSleepLoopDesc tTaskSleepLoopDesc;
				tTaskSleepLoopDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskSleepLoopDesc.m_wstrBTNodeName = L"SleepLoopTask";
				tTaskSleepLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep loop task desc
				tTaskSleepLoopDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_SleepLoop;
				spSleepSequenceNode->AddBTNode(CTaskSleepLoop::Create(make_shared<FTaskSleepLoopDesc>(tTaskSleepLoopDesc)));
			}

			// sleep end task node
			{
				// node desc
				FTaskSleepEndDesc tTaskSleepEndDesc;
				tTaskSleepEndDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskSleepEndDesc.m_wstrBTNodeName = L"SleepEndTask";
				tTaskSleepEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep end task desc
				tTaskSleepEndDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_SleepEnd;
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
				tTaskNoticeDesc.m_wstrBTNodeName = L"ToDoNoticeTask";
				tTaskNoticeDesc.m_spOwnerGameObject = spHedgeHog;

				// notice task desc
				tTaskNoticeDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Idle;
				tTaskNoticeDesc.m_eMonsterNoticeType = EMonsterNoticeType::LIGHT_BULB;
				spToDoSequenceNode->AddBTNode(CTaskNotice::Create(make_shared<FTaskNoticeDesc>(tTaskNoticeDesc)));
			}

			// move task node
			{
				// task desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskMoveDesc.m_wstrBTNodeName = L"ToDoMoveTask";
				tTaskMoveDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Walk;
				spToDoSequenceNode->AddBTNode(CTaskMove::Create(make_shared<FTaskMoveDesc>(tTaskMoveDesc)));
			}

			// todo task node
			{
				// task desc
				FTaskToDoDesc tTaskToDoDesc;
				tTaskToDoDesc.m_wstrBTNodeName = L"ToDoTask";
				tTaskToDoDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskToDoDesc.m_spOwnerGameObject = spHedgeHog;

				// todo task desc
				// warning: 찌릿도치는 할 일이 없습니다.
				spToDoSequenceNode->AddBTNode(CTaskToDo::Create(make_shared<FTaskToDoDesc>(tTaskToDoDesc)));
			}

			// emote task node
			{
				// task desc
				FTaskEmoteDesc tTaskEmoteDesc;
				tTaskEmoteDesc.m_wstrBTNodeName = L"ToDoEmoteTask";
				tTaskEmoteDesc.m_eBTNodeType = EBTNodeType::TASK;
				tTaskEmoteDesc.m_spOwnerGameObject = spHedgeHog;

				// emote task desc
				tTaskEmoteDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Idle;
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
				tTaskIdleDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskIdleDesc.m_wstrBTNodeName = L"WanderIdleTask";
				tTaskIdleDesc.m_eBTNodeType = EBTNodeType::TASK;

				// idle task desc
				tTaskIdleDesc.m_iAnimationID = (int32)EHedgeHog::AS_Hedgehog_Idle;
				tTaskIdleDesc.m_fMaxIdleDeltaSeconds = 5.0f;
				spWanderSequenceNode->AddBTNode(CTaskIdle::Create(make_shared<FTaskIdleDesc>(tTaskIdleDesc)));
			}

			// move task node
			{
				// node desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject	= spHedgeHog;
				tTaskMoveDesc.m_wstrBTNodeName		= L"WanderMoveTask";
				tTaskMoveDesc.m_eBTNodeType			= EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID		= (int32)EHedgeHog::AS_Hedgehog_Walk;

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
				tTaskRestDesc.m_spOwnerGameObject = spHedgeHog;
				tTaskRestDesc.m_wstrBTNodeName = L"RestTask";
				tTaskRestDesc.m_eBTNodeType = EBTNodeType::TASK;

				// rest task desc
				tTaskRestDesc.m_iAnimationID = (int32)EHedgeHog::AS_SheepBall_Rest03;
				tTaskRestDesc.m_fMaxRestDeltaSeconds = 5.0f;
				spWanderSequenceNode->AddBTNode(CTaskRest::Create(make_shared<FTaskRestDesc>(tTaskRestDesc)));
			}
			*/
		}
	}
}

HRESULT Client::CHedgeHogIce::ReInitializePalBT(EMonsterAIType _eMonsterAIType)
{
	if (FAILED(CMonster::ReInitializePalBT(_eMonsterAIType))) { return E_FAIL; }

	// 이전 비헤이비어 트리를 해제합니다.
	if (m_spRootNode) { m_spRootNode = nullptr; }

	m_spBehaviorTreeComponent->Release();

	// 새로운 비헤이비어 트리를 할당합니다.
	switch (_eMonsterAIType)
	{
	case Client::EMonsterAIType::WILD_PAL:		InitializeWildPalBT(nullptr, false);		break;
	case Client::EMonsterAIType::PARTNER_PAL:	InitializePartnerPalBT(nullptr, false);	break;
	case Client::EMonsterAIType::SLAVE_PAL:		InitializeSlavePalBT(nullptr, false);	break;
	}

	// behavior tree desc
	m_spBehaviorTreeComponent->SetRootNode(m_spRootNode);
	m_spBehaviorTreeComponent->SetBehaveTree(true);
	return S_OK;
}

void Client::CHedgeHogIce::LookAt(const Vector3& _vPosition)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->LookAt(_vPosition);
}

void Client::CHedgeHogIce::Turn(const Vector3& _vAxis, float _fAngularSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->Turn(_vAxis, _fAngularSpeed);
}

void Client::CHedgeHogIce::MoveForward(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderForward(_fSpeed);
}

void Client::CHedgeHogIce::MoveBackward(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderBackward(_fSpeed);
}

void Client::CHedgeHogIce::MoveRight(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderRight(_fSpeed);
}

void Client::CHedgeHogIce::MoveLeft(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderLeft(_fSpeed);
}

void Client::CHedgeHogIce::MoveUp(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderUp(_fSpeed);
}

void Client::CHedgeHogIce::MoveDown(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderDown(_fSpeed);
}

void Client::CHedgeHogIce::SetPosition(const Vector3& _vPosition)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetPosition(_vPosition);
}

const std::optional<Vector3> Client::CHedgeHogIce::GetPosition() const
{
	if (!m_spBodySphereComponent) { return nullopt; }

	return m_spBodySphereComponent->GetPosition();
}

void Client::CHedgeHogIce::UpdateBlackboardValues()
{
	CMonster::UpdateBlackboardValues();
}

void Client::CHedgeHogIce::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
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
			// 야생 찌릿도치가 플레이어를 공격합니다.
			if
				(
					GetBoolValue(EBlackboardBoolValue::BOOL_ATTACK) == true		// 야생 찌릿도치가 공격하는 중이고
					&& (eSrcCollisionLayer == ECollisionLayer::MONSTER_WEAPON)	// 야생 찌릿도치의 공격 콜라이더가
					&& (eDstCollisionLayer == ECollisionLayer::PLAYER)			// 플레이어와 부딪힌 경우
				)
			{
				ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, true);
				static_pointer_cast<CPlayer>(pDstControllerComponent->GetOwnerGameObject())->TakeDamage(shared_from_this(), m_fAttackPower);
			}
		}

		return;
	}

	// 콜라이더 컴포넌트
	CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	ECollisionLayer eSrcCollisionLayer = pSrcColliderComponent->GetCollisionLayer();
	ECollisionLayer eDstCollisionLayer = pDstColliderComponent->GetCollisionLayer();

	// 파트너 찌릿도치가 야생 팰을 공격합니다.
	if (m_eMonsterAIType == EMonsterAIType::PARTNER_PAL)
	{
		if
			(
				GetBoolValue(EBlackboardBoolValue::BOOL_ATTACK) == true	// 파트너 찌릿도치가 공격하는 중이고
				&& (eSrcCollisionLayer == ECollisionLayer::ALLY_WEAPON)	// 파트너 찌릿도치의 공격 콜라이더가
				&& (eDstCollisionLayer == ECollisionLayer::MONSTER)		// 야생 몬스터와 부딪친 경우
			)
		{
			ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, true);
			static_pointer_cast<CMonster>(pDstColliderComponent->GetOwnerGameObject())->TakeDamage(shared_from_this(), m_fAttackPower);
		}
	}

	// 야생 찌릿도치가 파트너 팰을 공격합니다.
	else if
		(
			GetBoolValue(EBlackboardBoolValue::BOOL_ATTACK) == true		// 야생 찌릿도치가 공격하는 중이고
			&& (eSrcCollisionLayer == ECollisionLayer::MONSTER_WEAPON)	// 야생 찌릿도치의 공격 콜라이더가
			&& (eDstCollisionLayer == ECollisionLayer::ALLY)			// 파트너 팰과 부딪힌 경우
		)
	{
		ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, true);
		static_pointer_cast<CMonster>(pDstColliderComponent->GetOwnerGameObject())->TakeDamage(shared_from_this(), 0.0f);
	}

	// 야생 아이스도치가 목장으로 이동합니다.
	if (m_eMonsterAIType == EMonsterAIType::WILD_PAL
		&& eDstCollisionLayer == ECollisionLayer::BUILDING)
	{
		shared_ptr<CArchitecture> spArchitecture = dynamic_pointer_cast<CArchitecture>(pDstColliderComponent->GetOwnerGameObject());
		
		if (spArchitecture->GetArchitectureType() == EArchitectureType::RANCH)
		{
			ChangeEye(EPalEyeType::SMIZED);
			ChangeMouth(EPalMouthType::SMILE);
			m_spMeshComponent->ChangeAnimation((int32)EHedgeHog::AS_Hedgehog_Idle, false);
			m_spBehaviorTreeComponent->SetBehaveTree(false);

			// 전설의 도치 퀘스트를 완료합니다.
			shared_ptr<CRanch> spRanch = dynamic_pointer_cast<CRanch>(spArchitecture);
			spRanch->CompleteQuest();
		}
	}
}

void Client::CHedgeHogIce::OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CMonster::OnCollisionEnter(_pSrcComponent, _pDstComponent);
}

shared_ptr<CColliderComponent> Client::CHedgeHogIce::GetBodyColliderComponent()
{
	return m_spBodySphereComponent;
}

std::shared_ptr<CColliderComponent> Client::CHedgeHogIce::GetAttackColliderComponent()
{
	return m_spAttackSphereComponent;
}

void Client::CHedgeHogIce::SetLinearVelocity(const PxVec3& _vLinearVelocity)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetLinearVelocity(_vLinearVelocity);
}

void Client::CHedgeHogIce::SetAngularVelocity(const PxVec3& _vLinearVelocity)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetAngularVelocity(_vLinearVelocity);
}
