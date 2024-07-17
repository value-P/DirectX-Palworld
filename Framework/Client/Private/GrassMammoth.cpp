#include "pch.h"
#include "GrassMammoth.h"

// engine manager
#include "EngineModule.h"
#include "CameraManager.h"
#include "InputManager.h"
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
#include "DecoratorDay.h"
#include "DecoratorInteracted.h"
#include "DecoratorNear.h"
#include "DecoratorNight.h"
#include "DecoratorPartnerSkill.h"
#include "DecoratorSkillType.h"
#include "DecoratorSleep.h"

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
#include "TaskGrassMammothAttackLoop.h"
#include "TaskIdle.h"
#include "TaskNear.h"
#include "TaskNotice.h"
#include "TaskMove.h"
#include "TaskLookAt.h"
#include "TaskReact.h"
#include "TaskRest.h"
#include "TaskSleepStart.h"
#include "TaskSleepLoop.h"
#include "TaskSleepEnd.h"
#include "TaskToDo.h"
#include "TaskJumpLoop.h"
#include "TaskJumpEnd.h"
#include "TaskWithdraw.h"

// gameobejct
#include "Player.h"
#include "Factory.h"
#include "Architecture.h"

std::shared_ptr<CGrassMammoth> Client::CGrassMammoth::Create(const std::shared_ptr<FGrassMammothDesc>& _spGrassMammothDesc)
{
	const std::shared_ptr<CGrassMammoth> spGrassMammoth = make_shared<CGrassMammoth>();
	ENSUREF(spGrassMammoth->Initialize(_spGrassMammothDesc), L"Failed to initialize grass mammoth.");
	return spGrassMammoth;
}

HRESULT Client::CGrassMammoth::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CMonster::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	shared_ptr<FGrassMammothDesc> spGrassMammothDesc = static_pointer_cast<FGrassMammothDesc>(_spGameObjectDesc);
	InitGrassMammoth(spGrassMammothDesc);
	InitMeshComponent();
	InitColliderComponent();
	InitBehaviorTreeComponent(spGrassMammothDesc);
	return S_OK;
}

HRESULT Client::CGrassMammoth::BeginPlay()
{
	if (FAILED(CMonster::BeginPlay())) { return E_FAIL; }

	m_wpUIManager = CUIManager::GetInstance();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();
	m_spDashBoxComponent->RequestSimulation(false);
	m_spEarthImpactBoxComponent->RequestSimulation(false);
	return S_OK;
}

int32 Client::CGrassMammoth::PreTick(float _fDeltaSeconds)
{
	switch (CMonster::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	// 거리 비교를 통해 ui를 끌지 말지 선택해줍니다.
	if (20.f >= Vector3::Distance(m_spPlayer->GetPosition().value(), GetPosition().value()) && m_fCurHp > 0.f)
	{
		// 가까울 경우 ui를 켜줍니다.
		if (false == m_wpUIManager.lock()->GetBossInfo()->spBossPanel->IsActive())
		{
			m_wpUIManager.lock()->GetBossInfo()->spBossPanel->SetActive(true);
			m_wpUIManager.lock()->SetBossHP(m_fMaxHp, m_fCurHp, m_eMonsterType);
		}
	}
	else
	{
		// 멀어졌을 경우 ui를 꺼줍니다.
		if (true == m_wpUIManager.lock()->GetBossInfo()->spBossPanel->IsActive())
		{
			m_wpUIManager.lock()->GetBossInfo()->spBossPanel->SetActive(false);
		}
	}

	return 0;
}

int32 Client::CGrassMammoth::Tick(float _fDeltaSeconds)
{
	switch (CMonster::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	// 체력 ui를 연동합니다.
	if (m_fPreHp != m_fCurHp)
	{
		// 텍스트를 연동합니다.
		if (m_wpUIManager.lock()->MinusBossHP(m_wpUIManager.lock()->GetBossInfo()->spBossHPProgress, m_wpUIManager.lock()->GetBossInfo()->spCurHPText, m_fPreHp, m_fCurHp, m_fMaxHp, &m_fHPLerpTime, _fDeltaSeconds))
		{
			m_fPreHp = m_fCurHp;
		}

		if (m_fCurHp <= 0.0f)
		{
			m_wpUIManager.lock()->GetBossInfo()->spBossPanel->SetActive(false);
		}
	}

	return 0;
}

int32 Client::CGrassMammoth::PostTick(float _fDeltaSeconds)
{
	switch (CMonster::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CGrassMammoth::EndPlay()
{
	if (FAILED(CMonster::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CGrassMammoth::Release()
{
	m_spBodySphereComponent = nullptr;
	m_spDashBoxComponent = nullptr;
	m_spEarthImpactBoxComponent = nullptr;

	CMonster::Release();
}

void Client::CGrassMammoth::InitGrassMammoth(const std::shared_ptr<FGrassMammothDesc>& _spGrassMammothDesc)
{
	m_fMaxHp = 777.0f;

	// gameobject desc
	m_wstrGameObjectName = L"GrassMammoth";
	m_eMobilityType = EMobilityType::DYNAMIC;

	// monster desc
	m_eMonsterType = EMonsterType::GRASS_MAMMOTH;
	m_eMonsterSizeType = EMonsterSizeType::LARGE;
}

void Client::CGrassMammoth::InitMeshComponent()
{
	// component desc
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

	// mesh component desc
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/GrassMammoth.dat";
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(180.0f));
	tMeshComponentDesc.m_wstrShaderName = L"";

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	ENSURE(AddComponent(tMeshComponentDesc.m_wstrComponentName, m_spMeshComponent));

	// 운반 본 포인터를 구합니다.
	m_wpTransportBone = m_spMeshComponent->GetBonePtr(L"Socket_Transport");
}

void Client::CGrassMammoth::InitColliderComponent()
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

		tSphereComponentDesc.m_fMass				= 100.0f;
		tSphereComponentDesc.m_iAxisLockType		= (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
		tSphereComponentDesc.m_vLocalPosition		= Vector3(0.0f, 2.5f, 0.0f);
		tSphereComponentDesc.m_vMaterial			= Vector3(1.0f, 1.0f, 0.05f);

		// sphere component desc
		tSphereComponentDesc.m_fRadius = 2.5f;

		m_spBodySphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		ENSURE(AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spBodySphereComponent));
	}

	// dash box component
	{
		// component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_wstrComponentName = L"DashBoxComponent";
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		// collider component desc
		tBoxComponentDesc.m_bTrigger = true;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		tBoxComponentDesc.m_bDeferredAdd = true;
		tBoxComponentDesc.m_bEnableSimulation = false;

		switch (m_eMonsterAIType)
		{
		case Client::EMonsterAIType::WILD_PAL:
			tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::MONSTER_WEAPON;
			tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::MONSTER_WEAPON;
			break;

		case Client::EMonsterAIType::PARTNER_PAL:
		case Client::EMonsterAIType::SLAVE_PAL:
			tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::ALLY_WEAPON;
			tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::ALLY_WEAPON;
			break;

		default:
			break;
		}

		tBoxComponentDesc.m_fMass = 0.0f;
		tBoxComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
		tBoxComponentDesc.m_vLocalPosition = Vector3(0.0f, 1.0f, 2.0f);
		tBoxComponentDesc.m_vMaterial = Vector3(1.0f, 1.0f, 0.05f);

		// box component desc
		tBoxComponentDesc.m_vExtents = Vector3(2.0f, 2.0f, 0.5f);

		m_spDashBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spDashBoxComponent));
	}

	// earth impact box component
	{
		// component desc
		FBoxComponentDesc tBoxComponentDesc;
		tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
		tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
		tBoxComponentDesc.m_wstrComponentName = L"EarthImpactBoxComponent";
		tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

		// collider component desc
		tBoxComponentDesc.m_bTrigger = true;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
		tBoxComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
		tBoxComponentDesc.m_bDeferredAdd = true;
		tBoxComponentDesc.m_bEnableSimulation = false;

		switch (m_eMonsterAIType)
		{
		case Client::EMonsterAIType::WILD_PAL:
			tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::MONSTER_WEAPON;
			tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::MONSTER_WEAPON;
			break;

		case Client::EMonsterAIType::PARTNER_PAL:
		case Client::EMonsterAIType::SLAVE_PAL:
			tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::ALLY_WEAPON;
			tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::ALLY_WEAPON;
			break;

		default:
			break;
		}

		tBoxComponentDesc.m_fMass = 0.0f;
		tBoxComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
		tBoxComponentDesc.m_vLocalPosition = Vector3(0.0f, 0.0f, 0.0f);
		tBoxComponentDesc.m_vMaterial = Vector3(1.0f, 1.0f, 0.05f);

		// box component desc
		tBoxComponentDesc.m_vExtents = Vector3(15.0f, 0.6f, 15.0f);

		m_spEarthImpactBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		ENSURE(AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spEarthImpactBoxComponent));
	}
}

void Client::CGrassMammoth::InitBehaviorTreeComponent(const std::shared_ptr<FGrassMammothDesc>& _spGrassMammothDesc)
{
	// 1. 야생 팰 비헤이비어 트리
	if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
	{
		InitializeWildPalBT(_spGrassMammothDesc);
	}

	// 2. 파트너 팰 비헤이비어 트리
	else if (m_eMonsterAIType == EMonsterAIType::PARTNER_PAL)
	{
		InitializePartnerPalBT(_spGrassMammothDesc);
	}

	// 3. 거점 팰 비헤이비어 트리
	else if (m_eMonsterAIType == EMonsterAIType::SLAVE_PAL)
	{
		InitializeSlavePalBT(_spGrassMammothDesc);
	}

	// behavior tree desc
	m_spBehaviorTreeComponent->SetRootNode(m_spRootNode);
	m_spBehaviorTreeComponent->SetBehaveTree(true);
}

void Client::CGrassMammoth::InitializeWildPalBT(const std::shared_ptr<FGrassMammothDesc> _spGrassMammothDesc, bool _bInit)
{
	shared_ptr<CGameObject> spGrassMammoth = shared_from_this();

	// behavior tree component
	if (_bInit)
	{
		// component desc
		FBTComponentDesc tBTComponentDesc;
		tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
		tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
		tBTComponentDesc.m_eRenderType = ERenderType::NONE;
		tBTComponentDesc.m_wpOwnerGameObject = spGrassMammoth;

		m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
		AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
	}

	// behavior tree node
	{
		FCompositeNodeDesc tSelectorNodeDesc;
		tSelectorNodeDesc.m_spOwnerGameObject = spGrassMammoth;
		tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

		FCompositeNodeDesc tSequenceNodeDesc;
		tSequenceNodeDesc.m_spOwnerGameObject = spGrassMammoth;
		tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
		tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

		FDecoratorBBBoolDesc tDecoBBBoolDesc;
		tDecoBBBoolDesc.m_spOwnerGameObject = spGrassMammoth;
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
			tTaskDeadDesc.m_spOwnerGameObject = spGrassMammoth;
			tTaskDeadDesc.m_wstrBTNodeName = L"DeadTask";
			tTaskDeadDesc.m_eBTNodeType = EBTNodeType::TASK;

			// dead task desc
			tTaskDeadDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_SleepStart;
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
				tTaskCapturedDesc.m_spOwnerGameObject = spGrassMammoth;
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
				tTaskJumpLoopDesc.m_spOwnerGameObject = spGrassMammoth;
				tTaskJumpLoopDesc.m_wstrBTNodeName = L"JumpLoopTask";
				tTaskJumpLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpLoopDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_JumpLoop;
				spJumpSequenceNode->AddBTNode(CTaskJumpLoop::Create(make_shared<FTaskJumpLoopDesc>(tTaskJumpLoopDesc)));
			}

			// jump end task node
			{
				// task desc
				FTaskJumpEndDesc tTaskJumpEndDesc;
				tTaskJumpEndDesc.m_spOwnerGameObject = spGrassMammoth;
				tTaskJumpEndDesc.m_wstrBTNodeName = L"JumpEndTask";
				tTaskJumpEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// move task desc
				tTaskJumpEndDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_JumpEnd;
				spJumpSequenceNode->AddBTNode(CTaskJumpEnd::Create(make_shared<FTaskJumpEndDesc>(tTaskJumpEndDesc)));
			}
		}

		// damage task node
		{
			// node desc
			FTaskDamageDesc tTaskDamageDesc;
			tTaskDamageDesc.m_spOwnerGameObject = spGrassMammoth;
			tTaskDamageDesc.m_wstrBTNodeName = L"DamageTask";
			tTaskDamageDesc.m_eBTNodeType = EBTNodeType::TASK;

			// damage task desc
			tTaskDamageDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Damage;
			tTaskDamageDesc.m_spTarget = m_spPlayer;
			m_spRootNode->AddBTNode(CTaskDamaged::Create(make_shared<FTaskDamageDesc>(tTaskDamageDesc)));
		}

		// attack selector node
		{
			tSelectorNodeDesc.m_wstrBTNodeName = L"AttackSelector";
			shared_ptr<CCompositeNode> spAttackSelectorNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSelectorNodeDesc));
			m_spRootNode->AddBTNode(spAttackSelectorNode);

			// aggro deco node
			{
				tDecoBBBoolDesc.m_wstrBTNodeName = L"AggroDeco";
				tDecoBBBoolDesc.m_eBoolValue = EBlackboardBoolValue::BOOL_AGGRO;
				spAttackSelectorNode->AddBTNode(CDecoratorBBBool::Create(make_shared<FDecoratorBBBoolDesc>(tDecoBBBoolDesc)));
			}

			// dash sequence node
			{
				tSequenceNodeDesc.m_wstrBTNodeName = L"DashSequence";
				shared_ptr<CCompositeNode> spDashSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
				spAttackSelectorNode->AddBTNode(spDashSequenceNode);

				// skill type deco
				{
					FDecoratorSkillTypeDesc tDecoSkillType;
					tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
					tDecoSkillType.m_spOwnerGameObject = spGrassMammoth;
					tDecoSkillType.m_wstrBTNodeName = L"DashSkillTypeDecorator";

					tDecoSkillType.m_iSkillType = (int32)EGrassMammothSkillType::DASH;
					spDashSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
				}

				// encount task node
				{
					FTaskEncountDesc tTaskEncountDesc;
					tTaskEncountDesc.m_spOwnerGameObject = spGrassMammoth;
					tTaskEncountDesc.m_wstrBTNodeName = L"DashEnount";
					tTaskEncountDesc.m_eBTNodeType = EBTNodeType::TASK;

					tTaskEncountDesc.m_spTarget = m_spPlayer;
					tTaskEncountDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Walk;
					spDashSequenceNode->AddBTNode(CTaskEncount::Create(make_shared<FTaskEncountDesc>(tTaskEncountDesc)));
				}

				// look at task node
				{
					// task desc
					FTaskLookAtDesc tTaskLookAtDesc;
					tTaskLookAtDesc.m_spOwnerGameObject = spGrassMammoth;
					tTaskLookAtDesc.m_wstrBTNodeName = L"DashLookAtTask";
					tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

					// look at task desc
					tTaskLookAtDesc.m_spTarget = m_spPlayer;
					tTaskLookAtDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Walk;
					spDashSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
				}

				// grass mammoth attack loop task node; Dash
				{
					FTaskGrassMammothAttackLoopDesc tTaskGrassMammothAttackLoop;
					tTaskGrassMammothAttackLoop.m_eBTNodeType = EBTNodeType::TASK;
					tTaskGrassMammothAttackLoop.m_spOwnerGameObject = spGrassMammoth;
					tTaskGrassMammothAttackLoop.m_wstrBTNodeName = L"DashAttackLoopTask";

					tTaskGrassMammothAttackLoop.m_eProjectileCollisionLayer = ECollisionLayer::MONSTER_PROJECTILE;
					tTaskGrassMammothAttackLoop.m_eProjectileCollsionFlag = ECollisionFlag::MONSTER_PROJECTILE;

					tTaskGrassMammothAttackLoop.m_spTarget = m_spPlayer;
					tTaskGrassMammothAttackLoop.m_eCurGrassMammothSkillType = EGrassMammothSkillType::DASH;
					tTaskGrassMammothAttackLoop.m_iStartAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_Start;
					tTaskGrassMammothAttackLoop.m_iStartLoopAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_StartLoop;
					tTaskGrassMammothAttackLoop.m_iRunAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Run;
					spDashSequenceNode->AddBTNode(CTaskGrassMammothAttackLoop::Create(make_shared<FTaskGrassMammothAttackLoopDesc>(tTaskGrassMammothAttackLoop)));
				}
			}

			// power bomb sequence node
			{
				tSequenceNodeDesc.m_wstrBTNodeName = L"PowerBombSequence";
				shared_ptr<CCompositeNode> spPowerBombSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
				spAttackSelectorNode->AddBTNode(spPowerBombSequenceNode);

				// skill type deco
				{
					FDecoratorSkillTypeDesc tDecoSkillType;
					tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
					tDecoSkillType.m_spOwnerGameObject = spGrassMammoth;
					tDecoSkillType.m_wstrBTNodeName = L"PowerBombSkillTypeDecorator";

					tDecoSkillType.m_iSkillType = (int32)EGrassMammothSkillType::POWER_BOMB;
					spPowerBombSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
				}

				// wide berth; 거리를 벌리다.
				{

				}

				// look at task node
				{
					// task desc
					FTaskLookAtDesc tTaskLookAtDesc;
					tTaskLookAtDesc.m_spOwnerGameObject = spGrassMammoth;
					tTaskLookAtDesc.m_wstrBTNodeName = L"PowerBombLookAtTask";
					tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

					// look at task desc
					tTaskLookAtDesc.m_spTarget = m_spPlayer;
					tTaskLookAtDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Walk;
					spPowerBombSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
				}

				// encount task node
				{
					FTaskEncountDesc tTaskEncountDesc;
					tTaskEncountDesc.m_spOwnerGameObject = spGrassMammoth;
					tTaskEncountDesc.m_wstrBTNodeName = L"PowerBombEnount";
					tTaskEncountDesc.m_eBTNodeType = EBTNodeType::TASK;

					tTaskEncountDesc.m_spTarget = m_spPlayer;
					tTaskEncountDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Walk;
					spPowerBombSequenceNode->AddBTNode(CTaskEncount::Create(make_shared<FTaskEncountDesc>(tTaskEncountDesc)));
				}

				// grass mammoth attack loop task node; power bomb
				{
					FTaskGrassMammothAttackLoopDesc tTaskGrassMammothAttackLoop;
					tTaskGrassMammothAttackLoop.m_eBTNodeType = EBTNodeType::TASK;
					tTaskGrassMammothAttackLoop.m_spOwnerGameObject = spGrassMammoth;
					tTaskGrassMammothAttackLoop.m_wstrBTNodeName = L"PowerBombAttackLoopTask";

					tTaskGrassMammothAttackLoop.m_eProjectileCollisionLayer = ECollisionLayer::MONSTER_PROJECTILE;
					tTaskGrassMammothAttackLoop.m_eProjectileCollsionFlag = ECollisionFlag::MONSTER_PROJECTILE;

					tTaskGrassMammothAttackLoop.m_spTarget = m_spPlayer;
					tTaskGrassMammothAttackLoop.m_eCurGrassMammothSkillType = EGrassMammothSkillType::POWER_BOMB;
					tTaskGrassMammothAttackLoop.m_iStartAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_Start;
					tTaskGrassMammothAttackLoop.m_iStartLoopAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_StartLoop;
					tTaskGrassMammothAttackLoop.m_iActionAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_Action;
					tTaskGrassMammothAttackLoop.m_iEndAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_End;
					spPowerBombSequenceNode->AddBTNode(CTaskGrassMammothAttackLoop::Create(make_shared<FTaskGrassMammothAttackLoopDesc>(tTaskGrassMammothAttackLoop)));
				}
			}

			// grass tornado sequence node
			{
				tSequenceNodeDesc.m_wstrBTNodeName = L"GrassTornadoSequence";
				shared_ptr<CCompositeNode> spGrassTornadoSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
				spAttackSelectorNode->AddBTNode(spGrassTornadoSequenceNode);

				// skill type deco
				{
					FDecoratorSkillTypeDesc tDecoSkillType;
					tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
					tDecoSkillType.m_spOwnerGameObject = spGrassMammoth;
					tDecoSkillType.m_wstrBTNodeName = L"GrassTornadoSkillTypeDecorator";

					tDecoSkillType.m_iSkillType = (int32)EGrassMammothSkillType::GRASS_TORNADO;
					spGrassTornadoSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
				}

				// look at task node
				{
					// task desc
					FTaskLookAtDesc tTaskLookAtDesc;
					tTaskLookAtDesc.m_spOwnerGameObject = spGrassMammoth;
					tTaskLookAtDesc.m_wstrBTNodeName = L"GrassTornadoLookAtTask";
					tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

					// look at task desc
					tTaskLookAtDesc.m_spTarget = m_spPlayer;
					tTaskLookAtDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Walk;
					spGrassTornadoSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
				}

				// grass mammoth attack loop task node; GrassTornado
				{
					FTaskGrassMammothAttackLoopDesc tTaskGrassMammothAttackLoop;
					tTaskGrassMammothAttackLoop.m_eBTNodeType = EBTNodeType::TASK;
					tTaskGrassMammothAttackLoop.m_spOwnerGameObject = spGrassMammoth;
					tTaskGrassMammothAttackLoop.m_wstrBTNodeName = L"GrassTornadoAttackLoopTask";

					tTaskGrassMammothAttackLoop.m_eProjectileCollisionLayer = ECollisionLayer::MONSTER_PROJECTILE;
					tTaskGrassMammothAttackLoop.m_eProjectileCollsionFlag = ECollisionFlag::MONSTER_PROJECTILE;

					tTaskGrassMammothAttackLoop.m_spTarget = m_spPlayer;
					tTaskGrassMammothAttackLoop.m_eCurGrassMammothSkillType = EGrassMammothSkillType::GRASS_TORNADO;
					tTaskGrassMammothAttackLoop.m_iStartAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_Start;
					tTaskGrassMammothAttackLoop.m_iStartLoopAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_StartLoop;
					tTaskGrassMammothAttackLoop.m_iActionAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_Action;
					tTaskGrassMammothAttackLoop.m_iEndAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_End;
					spGrassTornadoSequenceNode->AddBTNode(CTaskGrassMammothAttackLoop::Create(make_shared<FTaskGrassMammothAttackLoopDesc>(tTaskGrassMammothAttackLoop)));
				}
			}

			// earth impact sequence node
			{
				tSequenceNodeDesc.m_wstrBTNodeName = L"EarthImpactSequence";
				shared_ptr<CCompositeNode> spEarthImpactSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
				spAttackSelectorNode->AddBTNode(spEarthImpactSequenceNode);

				// skill type deco
				{
					FDecoratorSkillTypeDesc tDecoSkillType;
					tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
					tDecoSkillType.m_spOwnerGameObject = spGrassMammoth;
					tDecoSkillType.m_wstrBTNodeName = L"EarthImpactSkillTypeDecorator";

					tDecoSkillType.m_iSkillType = (int32)EGrassMammothSkillType::EARTH_IMPACT;
					spEarthImpactSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
				}

				// look at task node
				{
					// task desc
					FTaskLookAtDesc tTaskLookAtDesc;
					tTaskLookAtDesc.m_spOwnerGameObject = spGrassMammoth;
					tTaskLookAtDesc.m_wstrBTNodeName = L"EarthImpactLookAtTask";
					tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

					// look at task desc
					tTaskLookAtDesc.m_spTarget = m_spPlayer;
					tTaskLookAtDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Walk;
					spEarthImpactSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
				}

				// chase task node
				{
					// node desc
					FTaskChaseDesc tTaskChaseDesc;
					tTaskChaseDesc.m_spOwnerGameObject = spGrassMammoth;
					tTaskChaseDesc.m_wstrBTNodeName = L"EarthImpactChaseTask";
					tTaskChaseDesc.m_eBTNodeType = EBTNodeType::TASK;

					// chase task desc
					tTaskChaseDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Run;
					tTaskChaseDesc.m_spTarget = m_spPlayer;
					tTaskChaseDesc.m_fMaxChaseDeltaSeconds = 0.0f;
					tTaskChaseDesc.m_fAcceptableRadius = 12.0f;
					spEarthImpactSequenceNode->AddBTNode(CTaskChase::Create(make_shared<FTaskChaseDesc>(tTaskChaseDesc)));
				}

				// grass mammoth attack loop task node; EarthImpact
				{
					FTaskGrassMammothAttackLoopDesc tTaskGrassMammothAttackLoop;
					tTaskGrassMammothAttackLoop.m_eBTNodeType = EBTNodeType::TASK;
					tTaskGrassMammothAttackLoop.m_spOwnerGameObject = spGrassMammoth;
					tTaskGrassMammothAttackLoop.m_wstrBTNodeName = L"EarthImpactAttackLoopTask";

					tTaskGrassMammothAttackLoop.m_eProjectileCollisionLayer = ECollisionLayer::MONSTER_PROJECTILE;
					tTaskGrassMammothAttackLoop.m_eProjectileCollsionFlag = ECollisionFlag::MONSTER_PROJECTILE;

					tTaskGrassMammothAttackLoop.m_spTarget = m_spPlayer;
					tTaskGrassMammothAttackLoop.m_eCurGrassMammothSkillType = EGrassMammothSkillType::EARTH_IMPACT;
					tTaskGrassMammothAttackLoop.m_iEarthImpactAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Earthquake;
					spEarthImpactSequenceNode->AddBTNode(CTaskGrassMammothAttackLoop::Create(make_shared<FTaskGrassMammothAttackLoopDesc>(tTaskGrassMammothAttackLoop)));
				}
			}

			// spine vine sequence node
			{
				tSequenceNodeDesc.m_wstrBTNodeName = L"SpineVineSequence";
				shared_ptr<CCompositeNode> spSpineVineSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
				spAttackSelectorNode->AddBTNode(spSpineVineSequenceNode);

				// skill type deco
				{
					FDecoratorSkillTypeDesc tDecoSkillType;
					tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
					tDecoSkillType.m_spOwnerGameObject = spGrassMammoth;
					tDecoSkillType.m_wstrBTNodeName = L"SpineVineSkillTypeDecorator";

					tDecoSkillType.m_iSkillType = (int32)EGrassMammothSkillType::SPINE_VINE;
					spSpineVineSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
				}

				// look at task node
				{
					// task desc
					FTaskLookAtDesc tTaskLookAtDesc;
					tTaskLookAtDesc.m_spOwnerGameObject = spGrassMammoth;
					tTaskLookAtDesc.m_wstrBTNodeName = L"SpineVineLookAtTask";
					tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

					// look at task desc
					tTaskLookAtDesc.m_spTarget = m_spPlayer;
					tTaskLookAtDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Walk;
					spSpineVineSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
				}

				// grass mammoth attack loop task node; SpineVine
				{
					FTaskGrassMammothAttackLoopDesc tTaskGrassMammothAttackLoop;
					tTaskGrassMammothAttackLoop.m_eBTNodeType = EBTNodeType::TASK;
					tTaskGrassMammothAttackLoop.m_spOwnerGameObject = spGrassMammoth;
					tTaskGrassMammothAttackLoop.m_wstrBTNodeName = L"SpineVineAttackLoopTask";

					tTaskGrassMammothAttackLoop.m_eProjectileCollisionLayer = ECollisionLayer::MONSTER_PROJECTILE;
					tTaskGrassMammothAttackLoop.m_eProjectileCollsionFlag = ECollisionFlag::MONSTER_PROJECTILE;

					tTaskGrassMammothAttackLoop.m_spTarget = m_spPlayer;
					tTaskGrassMammothAttackLoop.m_eCurGrassMammothSkillType = EGrassMammothSkillType::SPINE_VINE;
					tTaskGrassMammothAttackLoop.m_iStartAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_Start;
					tTaskGrassMammothAttackLoop.m_iStartLoopAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_StartLoop;
					tTaskGrassMammothAttackLoop.m_iActionAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_Action;
					tTaskGrassMammothAttackLoop.m_iEndAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_FarSkill_End;
					spSpineVineSequenceNode->AddBTNode(CTaskGrassMammothAttackLoop::Create(make_shared<FTaskGrassMammothAttackLoopDesc>(tTaskGrassMammothAttackLoop)));
				}
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
				tDecoSleepDesc.m_spOwnerGameObject = spGrassMammoth;
				tDecoSleepDesc.m_eBTNodeType = EBTNodeType::DECORATOR;
				tDecoSleepDesc.m_wstrBTNodeName = L"SleepDeco";
				spSleepSequenceNode->AddBTNode(CDecoratorSleep::Create(make_shared<FDecoratorSleepDesc>(tDecoSleepDesc)));
			}

			// sleep start task node
			{
				// node desc
				FTaskSleepStartDesc tTaskSleepStartDesc;
				tTaskSleepStartDesc.m_spOwnerGameObject = spGrassMammoth;
				tTaskSleepStartDesc.m_wstrBTNodeName = L"SleepStartTask";
				tTaskSleepStartDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep start task desc
				tTaskSleepStartDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_SleepStart;
				spSleepSequenceNode->AddBTNode(CTaskSleepStart::Create(make_shared<FTaskSleepStartDesc>(tTaskSleepStartDesc)));
			}

			// sleep loop task node
			{
				// node desc
				FTaskSleepLoopDesc tTaskSleepLoopDesc;
				tTaskSleepLoopDesc.m_spOwnerGameObject = spGrassMammoth;
				tTaskSleepLoopDesc.m_wstrBTNodeName = L"SleepLoopTask";
				tTaskSleepLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep loop task desc
				tTaskSleepLoopDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_SleepLoop;
				spSleepSequenceNode->AddBTNode(CTaskSleepLoop::Create(make_shared<FTaskSleepLoopDesc>(tTaskSleepLoopDesc)));
			}

			// sleep end task node
			{
				// node desc
				FTaskSleepEndDesc tTaskSleepEndDesc;
				tTaskSleepEndDesc.m_spOwnerGameObject = spGrassMammoth;
				tTaskSleepEndDesc.m_wstrBTNodeName = L"SleepEndTask";
				tTaskSleepEndDesc.m_eBTNodeType = EBTNodeType::TASK;

				// sleep end task desc
				tTaskSleepEndDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_SleepEnd;
				spSleepSequenceNode->AddBTNode(CTaskSleepEnd::Create(make_shared<FTaskSleepEndDesc>(tTaskSleepEndDesc)));
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
				tTaskIdleDesc.m_spOwnerGameObject = spGrassMammoth;
				tTaskIdleDesc.m_wstrBTNodeName = L"IdleTask";
				tTaskIdleDesc.m_eBTNodeType = EBTNodeType::TASK;

				// idle task desc
				tTaskIdleDesc.m_iAnimationID = (int32)EGrassMammoth::AS_GrassMammoth_Idle;
				tTaskIdleDesc.m_fMaxIdleDeltaSeconds = 10.0f;
				spWanderSequenceNode->AddBTNode(CTaskIdle::Create(make_shared<FTaskIdleDesc>(tTaskIdleDesc)));
			}

			// move task node
			{
				// node desc
				FTaskMoveDesc tTaskMoveDesc;
				tTaskMoveDesc.m_spOwnerGameObject		= shared_from_this();
				tTaskMoveDesc.m_wstrBTNodeName			= L"WanderMoveTask";
				tTaskMoveDesc.m_eBTNodeType				= EBTNodeType::TASK;

				// move task desc
				tTaskMoveDesc.m_iAnimationID			= (int32)EGrassMammoth::AS_GrassMammoth_Walk;
				tTaskMoveDesc.m_vSpawnPosition			= _spGrassMammothDesc->m_vInitPosition;
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
				tTaskRestDesc.m_iAnimationID			= (int32)EKisunebi::AS_SheepBall_Rest02;
				tTaskRestDesc.m_fMaxRestDeltaSeconds	= 10.0f;

				shared_ptr<CTaskRest> spTaskRestNode	= CTaskRest::Create(make_shared<FTaskRestDesc>(tTaskRestDesc));
				spWanderSequenceNode->AddBTNode(spTaskRestNode);
			}
			*/
		}
	}
}

void Client::CGrassMammoth::InitializePartnerPalBT(const std::shared_ptr<FGrassMammothDesc> _spGrassMammothDesc, bool _bInit)
{
	shared_ptr<CGameObject> spGrassMammoth = shared_from_this();

	// 그린모스는 파트너 팰로 사용하지 않습니다.
}

void Client::CGrassMammoth::InitializeSlavePalBT(const std::shared_ptr<FGrassMammothDesc> _spGrassMammothDesc, bool _bInit)
{
	shared_ptr<CGameObject> spGrassMammoth = shared_from_this();

	// 그린모스는 거점 팰로 사용하지 않습니다.
}

HRESULT Client::CGrassMammoth::ReInitializePalBT(EMonsterAIType _eMonsterAIType)
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

void Client::CGrassMammoth::LookAt(const Vector3& _vPosition)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->LookAt(_vPosition);
}

void Client::CGrassMammoth::Turn(const Vector3& _vAxis, float _fAngularSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->Turn(_vAxis, _fAngularSpeed);
}

void Client::CGrassMammoth::MoveForward(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderForward(_fSpeed);
}

void Client::CGrassMammoth::MoveBackward(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderBackward(_fSpeed);
}

void Client::CGrassMammoth::MoveRight(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderRight(_fSpeed);
}

void Client::CGrassMammoth::MoveLeft(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderLeft(_fSpeed);
}

void Client::CGrassMammoth::MoveUp(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderUp(_fSpeed);
}

void Client::CGrassMammoth::MoveDown(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderDown(_fSpeed);
}

void Client::CGrassMammoth::SetPosition(const Vector3& _vPosition)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetPosition(_vPosition);
}

const std::optional<Vector3> Client::CGrassMammoth::GetPosition() const
{
	if (!m_spBodySphereComponent) { return nullopt; }

	return m_spBodySphereComponent->GetPosition();
}

void Client::CGrassMammoth::ChangeMouth(EPalMouthType _ePalMouthType)
{
	return;
}

void Client::CGrassMammoth::UpdateBlackboardValues()
{
	CMonster::UpdateBlackboardValues();
}

void Client::CGrassMammoth::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CMonster::OnTriggerEnter(_pSrcComponent, _pDstComponent);

	// 컨트롤러 컴포넌트
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
		CControllerComponent* pDstControllerComponent = dynamic_cast<CControllerComponent*>(_pDstComponent);

		ECollisionLayer eSrcCollisionLayer = pSrcColliderComponent->GetCollisionLayer();
		ECollisionLayer eDstCollisionLayer = pDstControllerComponent->GetCollisionLayer();

		// 야생 그린모스가 플레이어를 공격합니다.
		if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
		{
			if (
				GetBoolValue(EBlackboardBoolValue::BOOL_ATTACK) == true		// 야생 그린모스가 공격하는 중이고
				&& (eSrcCollisionLayer == ECollisionLayer::MONSTER_WEAPON)	// 야생 그린모스의 공격 콜라이더가
				&& (eDstCollisionLayer == ECollisionLayer::PLAYER)			// 플레이어와 부딪힌 경우
				)
			{
				/*
				if (m_spDashBoxComponent.get() == pSrcColliderComponent)
				{
					m_wpSoundManager.lock()->PlaySoundEx("AKE_General_Hit_PowerShot_592405218");
				}
				*/
				ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, true);

				// 플레이어를 넉백시킵니다.
				shared_ptr<CPlayer> spPlayer = static_pointer_cast<CPlayer>(pDstControllerComponent->GetOwnerGameObject());

				Vector3 vPlayerPosition = spPlayer->GetPosition().value();
				Vector3 vMonsterPosition = GetPosition().value();
				Vector3 vForceDirection = vPlayerPosition - vMonsterPosition;
				vForceDirection.Normalize();
				
				spPlayer->AddForce(vForceDirection, CClientLoop::s_spClientDataSheet->k_mapKnockBackValue[EMonsterType::GRASS_MAMMOTH]);

				// 플레이어에게 대미지를 줍니다.
				spPlayer->TakeDamage(shared_from_this(), m_fAttackPower);

				// 피격 이펙트를 출력합니다.
				m_wpEffectManager.lock()->ActivateEffect(L"Group_Hit2", vPlayerPosition + Vector3(0.f, 1.f, 0.f));

				// 피격 사운드를 출력합니다.
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
}

void Client::CGrassMammoth::OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CMonster::OnCollisionEnter(_pSrcComponent, _pDstComponent);
}

std::shared_ptr<CColliderComponent> Client::CGrassMammoth::GetBodyColliderComponent()
{
	return m_spBodySphereComponent;
}

void Client::CGrassMammoth::SetLinearVelocity(const PxVec3& _vLinearVelocity)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetLinearVelocity(_vLinearVelocity);
}

void Client::CGrassMammoth::SetAngularVelocity(const PxVec3& _vLinearVelocity)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetAngularVelocity(_vLinearVelocity);
}
