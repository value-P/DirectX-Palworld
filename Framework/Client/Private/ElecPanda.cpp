#include "pch.h"
#include "ElecPanda.h"
#include "StringUtility.h"

// engine manager
#include "EngineModule.h"
#include "CameraManager.h"
#include "InputManager.h"
#include "TimerManager.h"
#include "SoundManager.h"
#include "World.h"

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
#include "Bone.h"

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
#include "TaskElecPandaAttackLoop.h"
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

#include "ClientLoop.h"

std::shared_ptr<CElecPanda> Client::CElecPanda::Create(const std::shared_ptr<FElecPandaDesc>& _spElecPandaDesc)
{
	const std::shared_ptr<CElecPanda> spElecPanda = make_shared<CElecPanda>();
	ENSUREF(spElecPanda->Initialize(_spElecPandaDesc), L"Failed to initialize grass mammoth.");
	return spElecPanda;
}

HRESULT Client::CElecPanda::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CMonster::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();

	shared_ptr<FElecPandaDesc> spElecPandaDesc = static_pointer_cast<FElecPandaDesc>(_spGameObjectDesc);

	InitElecPanda(spElecPandaDesc);
	InitMeshComponent();
	InitColliderComponent();
	InitBehaviorTreeComponent();
	InitTrailCombinedBones();
	InitTrailComponent();
	InitProjectile();
	return S_OK;
}

HRESULT Client::CElecPanda::BeginPlay()
{
	if (FAILED(CMonster::BeginPlay())) { return E_FAIL; }

	LookAt(Vector3(500.f, 500.f, 500.0f - 20.0f));

	m_wpUIManager = CUIManager::GetInstance();

	return S_OK;
}

int32 Client::CElecPanda::PreTick(float _fDeltaSeconds)
{
	switch (CMonster::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	// 거리 비교
	if (20.f >= Vector3::Distance(m_spPlayer->GetPosition().value(), GetPosition().value()) && m_fCurHp > 0.f)
	{
		shared_ptr<CPanel> spPanel = m_wpUIManager.lock()->GetBossInfo()->spBossPanel;

		// 가까울 경우 ui를 켜줍니다.
		if (false == spPanel->IsActive())
		{
			spPanel->SetActive(true);
			m_wpUIManager.lock()->SetBossHP(m_fMaxHp, m_fCurHp, m_eMonsterType);
		}
	}
	else
	{
		shared_ptr<CPanel> spPanel = m_wpUIManager.lock()->GetBossInfo()->spBossPanel;

		// 멀어졌을 경우 ui를 꺼줍니다.
		if (true == spPanel->IsActive())
		{
			spPanel->SetActive(false);
		}
	}

	return 0;
}

int32 Client::CElecPanda::Tick(float _fDeltaSeconds)
{
	switch (CMonster::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	// 체력 ui를 연동시켜줍니다.
	if (m_fPreHp != m_fCurHp)
	{
		// 텍스트를 연동합니다.
		if (m_wpUIManager.lock()->MinusBossHP(m_wpUIManager.lock()->GetBossInfo()->spBossHPProgress, m_wpUIManager.lock()->GetBossInfo()->spCurHPText, m_fPreHp, m_fCurHp, m_fMaxHp, &m_fHPLerpTime, _fDeltaSeconds))
		{
			m_fPreHp = m_fCurHp;
		}
		if (m_fCurHp <= 0.f)
		{
			m_wpUIManager.lock()->FindParentsPanel(L"BossDieEventUI")->SetActive(true);
			m_wpUIManager.lock()->GetBossInfo()->spBossPanel->SetActive(false);
		}
	}


	if (m_bOnTrail) 
	{
		// TickTrail(0.1f);
		// CheckTrailActiveTime(_fDeltaSeconds);
	}
	
	return 0;
}

int32 Client::CElecPanda::PostTick(float _fDeltaSeconds)
{
	switch (CMonster::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CElecPanda::EndPlay()
{
	if (FAILED(CMonster::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CElecPanda::Release()
{
	m_spBodySphereComponent = nullptr;
	m_spAttackSphereComponent = nullptr;

	for (auto& spTrailComponent : m_arrLeftNailTrails) { spTrailComponent = nullptr; }
	for (auto& spTrailComponent : m_arrRightNailTrails) { spTrailComponent = nullptr; }
	
	for (auto& pair : m_umapProjectileEffectGroups)
	{
		for (auto& spEffectGroup : pair.second)
		{
			spEffectGroup->Release();
			spEffectGroup = nullptr;
		}
		pair.second.clear();
	}
	m_umapProjectileEffectGroups.clear();
	
	CMonster::Release();
}

void Client::CElecPanda::InitElecPanda(const std::shared_ptr<FElecPandaDesc>& _spElecPandaDesc)
{
	// gameobject desc
	m_wstrGameObjectName = L"ElecPanda";
	m_eMobilityType = EMobilityType::DYNAMIC;

	m_fMaxHp = 2222;
	m_fCurHp = 2222;

	// monster desc
	m_eMonsterType = EMonsterType::ELEC_PANDA;
	m_eMonsterSizeType = EMonsterSizeType::LARGE;
}

void Client::CElecPanda::InitMeshComponent()
{
	// component desc
	FMeshComponentDesc tMeshComponentDesc;
	tMeshComponentDesc.m_wstrComponentName = L"MeshComponent";
	tMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

	// mesh component desc
	tMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/ElecPanda.dat";
	tMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.00013f) * Matrix::CreateRotationY(XMConvertToRadians(180.0f));
	tMeshComponentDesc.m_wstrShaderName = L"";

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshComponentDesc));
	ENSURE(AddComponent(tMeshComponentDesc.m_wstrComponentName, m_spMeshComponent));
}

void Client::CElecPanda::InitColliderComponent()
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
		tSphereComponentDesc.m_vLocalPosition = Vector3(0.0f, 1.8f, 0.0f);

		// sphere component desc
		tSphereComponentDesc.m_fRadius = 1.8f;

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
		tSphereComponentDesc.m_vLocalPosition = Vector3(0.0f, 1.8f, 0.0f);
		tSphereComponentDesc.m_vMaterial = Vector3(1.0f, 1.0f, 0.05f);

		// sphere component desc
		tSphereComponentDesc.m_fRadius = 3.0f;

		m_spAttackSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		ENSURE(AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spAttackSphereComponent));
	}
}

void Client::CElecPanda::InitBehaviorTreeComponent()
{
	shared_ptr<CGameObject> spElecPanda = shared_from_this();

	// 1. 야생 팰 비헤이비어 트리
	if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
	{
		// behavior tree component
		{
			// component desc
			FBTComponentDesc tBTComponentDesc;
			tBTComponentDesc.m_wstrComponentName = L"BehaviorTreeComponent";
			tBTComponentDesc.m_eComponentType = EComponentType::BEHAVIOR_TREE;
			tBTComponentDesc.m_eRenderType = ERenderType::NONE;
			tBTComponentDesc.m_wpOwnerGameObject = spElecPanda;

			m_spBehaviorTreeComponent = CBehaviorTreeComponent::Create(make_shared<FBTComponentDesc>(tBTComponentDesc));
			AddComponent(tBTComponentDesc.m_wstrComponentName, m_spBehaviorTreeComponent);
		}

		// behavior tree node
		{
			FCompositeNodeDesc tSelectorNodeDesc;
			tSelectorNodeDesc.m_spOwnerGameObject = spElecPanda;
			tSelectorNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
			tSelectorNodeDesc.m_eCompositeType = ECompositeType::SELECTOR;

			FCompositeNodeDesc tSequenceNodeDesc;
			tSequenceNodeDesc.m_spOwnerGameObject = spElecPanda;
			tSequenceNodeDesc.m_eBTNodeType = EBTNodeType::COMPOSITE;
			tSequenceNodeDesc.m_eCompositeType = ECompositeType::SEQUENCE;

			FDecoratorBBBoolDesc tDecoBBBoolDesc;
			tDecoBBBoolDesc.m_spOwnerGameObject = spElecPanda;
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
				tTaskDeadDesc.m_spOwnerGameObject = spElecPanda;
				tTaskDeadDesc.m_wstrBTNodeName = L"DeadTask";
				tTaskDeadDesc.m_eBTNodeType = EBTNodeType::TASK;

				// dead task desc
				tTaskDeadDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_SleepStart;
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
					tTaskJumpLoopDesc.m_spOwnerGameObject = spElecPanda;
					tTaskJumpLoopDesc.m_wstrBTNodeName = L"JumpLoopTask";
					tTaskJumpLoopDesc.m_eBTNodeType = EBTNodeType::TASK;

					// move task desc
					tTaskJumpLoopDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_JumpLoop;
					spJumpSequenceNode->AddBTNode(CTaskJumpLoop::Create(make_shared<FTaskJumpLoopDesc>(tTaskJumpLoopDesc)));
				}

				// jump end task node
				{
					// task desc
					FTaskJumpEndDesc tTaskJumpEndDesc;
					tTaskJumpEndDesc.m_spOwnerGameObject = spElecPanda;
					tTaskJumpEndDesc.m_wstrBTNodeName = L"JumpEndTask";
					tTaskJumpEndDesc.m_eBTNodeType = EBTNodeType::TASK;

					// move task desc
					tTaskJumpEndDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_JumpEnd;
					spJumpSequenceNode->AddBTNode(CTaskJumpEnd::Create(make_shared<FTaskJumpEndDesc>(tTaskJumpEndDesc)));
				}
			}

			//// damage task node
			//{
			//	// node desc
			//	FTaskDamageDesc tTaskDamageDesc;
			//	tTaskDamageDesc.m_spOwnerGameObject = spElecPanda;
			//	tTaskDamageDesc.m_wstrBTNodeName = L"DamageTask";
			//	tTaskDamageDesc.m_eBTNodeType = EBTNodeType::TASK;

			//	// damage task desc
			//	tTaskDamageDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Damage;
			//	tTaskDamageDesc.m_spTarget = m_spPlayer;
			//	m_spRootNode->AddBTNode(CTaskDamaged::Create(make_shared<FTaskDamageDesc>(tTaskDamageDesc)));
			//}

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

				// Snipping sequence node
				{
					tSequenceNodeDesc.m_wstrBTNodeName = L"SnippingSequence";
					shared_ptr<CCompositeNode> spSnipeSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
					spAttackSelectorNode->AddBTNode(spSnipeSequenceNode);

					// skill type deco
					{
						FDecoratorSkillTypeDesc tDecoSkillType;
						tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
						tDecoSkillType.m_spOwnerGameObject = spElecPanda;
						tDecoSkillType.m_wstrBTNodeName = L"SnippingSkillTypeDecorator";

						tDecoSkillType.m_iSkillType = (int32)EElecPandaSkillType::SNIPE;
						spSnipeSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
					}

					// look at task node
					{
						// task desc
						FTaskLookAtDesc tTaskLookAtDesc;
						tTaskLookAtDesc.m_spOwnerGameObject = spElecPanda;
						tTaskLookAtDesc.m_wstrBTNodeName = L"SnippingLookAtTask";
						tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

						// look at task desc
						tTaskLookAtDesc.m_spTarget = m_spPlayer;
						tTaskLookAtDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Walk;
						spSnipeSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
					}

					// Elecpanda attack loop task node; Snipping
					{
						FTaskElecPandaAttackLoopDesc tTaskElecPandaAttackLoop;
						tTaskElecPandaAttackLoop.m_eBTNodeType = EBTNodeType::TASK;
						tTaskElecPandaAttackLoop.m_spOwnerGameObject = spElecPanda;
						tTaskElecPandaAttackLoop.m_wstrBTNodeName = L"SnippingAttackLoopTask";

						tTaskElecPandaAttackLoop.m_spPlayer						= m_spPlayer;
						tTaskElecPandaAttackLoop.m_eCurElecPandaSkillType		= EElecPandaSkillType::SNIPE;
						tTaskElecPandaAttackLoop.m_iStartAnimation				= (int32)EElecPanda::AS_ElecPanda_FarSkill_Start;
						tTaskElecPandaAttackLoop.m_iStartLoopAnimation			= (int32)EElecPanda::AS_ElecPanda_FarSkill_StartLoop;
						tTaskElecPandaAttackLoop.m_fMaxStartLoopDeltaSeconds	= 3.0f;
						tTaskElecPandaAttackLoop.m_iAction00AnimationID			= (int32)EElecPanda::AS_ElecPanda_FarSkill_Action;
						tTaskElecPandaAttackLoop.m_iEndAnimationID				= (int32)EElecPanda::AS_ElecPanda_FarSkill_End;
						spSnipeSequenceNode->AddBTNode(CTaskElecPandaAttackLoop::Create(make_shared<FTaskElecPandaAttackLoopDesc>(tTaskElecPandaAttackLoop)));
					}
				}

				// discharge sequence node
				{
					tSequenceNodeDesc.m_wstrBTNodeName = L"DischargeSequence";
					shared_ptr<CCompositeNode> spDischargeSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
					spAttackSelectorNode->AddBTNode(spDischargeSequenceNode);

					// skill type deco
					{
						FDecoratorSkillTypeDesc tDecoSkillType;
						tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
						tDecoSkillType.m_spOwnerGameObject = spElecPanda;
						tDecoSkillType.m_wstrBTNodeName = L"DischargeSkillTypeDecorator";

						tDecoSkillType.m_iSkillType = (int32)EElecPandaSkillType::DISCHARGE;
						spDischargeSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
					}

					// look at task node
					{
						// task desc
						FTaskLookAtDesc tTaskLookAtDesc;
						tTaskLookAtDesc.m_spOwnerGameObject = spElecPanda;
						tTaskLookAtDesc.m_wstrBTNodeName = L"DischargeLookAtTask";
						tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

						// look at task desc
						tTaskLookAtDesc.m_spTarget = m_spPlayer;
						tTaskLookAtDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Walk;
						spDischargeSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
					}

					// encount task node
					{
						FTaskEncountDesc tEncountDesc;
						tEncountDesc.m_eBTNodeType = EBTNodeType::TASK;
						tEncountDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Encount;
						tEncountDesc.m_spTarget = m_spPlayer;
						tEncountDesc.m_spOwnerGameObject = spElecPanda;

						spDischargeSequenceNode->AddBTNode(CTaskEncount::Create(make_shared<FTaskEncountDesc>(tEncountDesc)));
					}
					// chase task
					{
						FTaskChaseDesc tChaseTask;
						tChaseTask.m_eBTNodeType = EBTNodeType::TASK;
						tChaseTask.m_fAcceptableRadius = 3.0f;
						tChaseTask.m_fMaxChaseDeltaSeconds = 15.0f;

						tChaseTask.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Run;
						tChaseTask.m_spOwnerGameObject = shared_from_this();
						tChaseTask.m_spTarget = CGameManager::GetInstance()->GetPlayer();
						tChaseTask.m_wstrBTNodeName = L"SnippingToDischargeChaseTask";

						spDischargeSequenceNode->AddBTNode(CTaskChase::Create(make_shared<FTaskChaseDesc>(tChaseTask)));
					}

					// Elecpanda attack loop task node; discharge
					{
						FTaskElecPandaAttackLoopDesc tTaskElecPandaAttackLoop;
						tTaskElecPandaAttackLoop.m_eBTNodeType = EBTNodeType::TASK;
						tTaskElecPandaAttackLoop.m_spOwnerGameObject = spElecPanda;
						tTaskElecPandaAttackLoop.m_wstrBTNodeName = L"DischargeAttackLoopTask";

						tTaskElecPandaAttackLoop.m_spPlayer						= m_spPlayer;
						tTaskElecPandaAttackLoop.m_eCurElecPandaSkillType		= EElecPandaSkillType::DISCHARGE;
						tTaskElecPandaAttackLoop.m_iStartAnimation				= (int32)EElecPanda::AS_ElecPanda_FarSkill_Start;
						tTaskElecPandaAttackLoop.m_iStartLoopAnimation			= (int32)EElecPanda::AS_ElecPanda_FarSkill_StartLoop;
						tTaskElecPandaAttackLoop.m_fMaxStartLoopDeltaSeconds	= 1.0f;
						tTaskElecPandaAttackLoop.m_iAction00AnimationID			= (int32)EElecPanda::AS_ElecPanda_FarSkill_Action;
						tTaskElecPandaAttackLoop.m_iActionLoop00AnimationID		= (int32)EElecPanda::AS_ElecPanda_FarSkill_ActionLoop;
						tTaskElecPandaAttackLoop.m_fMaxActionLoop00DeltaSeconds	= 1.0f;
						tTaskElecPandaAttackLoop.m_iEndAnimationID				= (int32)EElecPanda::AS_ElecPanda_FarSkill_End;
						spDischargeSequenceNode->AddBTNode(CTaskElecPandaAttackLoop::Create(make_shared<FTaskElecPandaAttackLoopDesc>(tTaskElecPandaAttackLoop)));
					}

				}

				// melee sequence
				{
					tSequenceNodeDesc.m_wstrBTNodeName = L"MeleeSequence";
					shared_ptr<CCompositeNode> spMeleeSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
					spAttackSelectorNode->AddBTNode(spMeleeSequenceNode);

					// skill type deco
					{
						FDecoratorSkillTypeDesc tDecoSkillType;
						tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
						tDecoSkillType.m_spOwnerGameObject = spElecPanda;
						tDecoSkillType.m_wstrBTNodeName = L"MeleeSkillTypeDecorator";

						tDecoSkillType.m_iSkillType = (int32)EElecPandaSkillType::MELEE;
						spMeleeSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
					}

					// look at task node
					{
						// task desc
						FTaskLookAtDesc tTaskLookAtDesc;
						tTaskLookAtDesc.m_spOwnerGameObject = spElecPanda;
						tTaskLookAtDesc.m_wstrBTNodeName = L"MeleeLookAtTask";
						tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

						// look at task desc
						tTaskLookAtDesc.m_spTarget = m_spPlayer;
						tTaskLookAtDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Walk;
						spMeleeSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
					}

					// Elecpanda attack loop task node; melee
					{
						FTaskElecPandaAttackLoopDesc tTaskElecPandaAttackLoop;
						tTaskElecPandaAttackLoop.m_eBTNodeType = EBTNodeType::TASK;
						tTaskElecPandaAttackLoop.m_spOwnerGameObject = spElecPanda;
						tTaskElecPandaAttackLoop.m_wstrBTNodeName = L"MeleeAttackLoopTask";

						tTaskElecPandaAttackLoop.m_spPlayer = m_spPlayer;
						tTaskElecPandaAttackLoop.m_eCurElecPandaSkillType = EElecPandaSkillType::MELEE;
						tTaskElecPandaAttackLoop.m_iStartAnimation = (int32)EElecPanda::AS_ElecPanda_Hikkaki_Start;
						tTaskElecPandaAttackLoop.m_iAction00AnimationID = (int32)EElecPanda::AS_ElecPanda_Hikkaki_Action_01;
						tTaskElecPandaAttackLoop.m_iAction01AnimationID = (int32)EElecPanda::AS_ElecPanda_Hikkaki_Action_02;
						tTaskElecPandaAttackLoop.m_iAction02AnimationID = (int32)EElecPanda::AS_ElecPanda_Hikkaki_Action_03;
						tTaskElecPandaAttackLoop.m_iEndAnimationID = (int32)EElecPanda::AS_ElecPanda_Hikkaki_End;
						spMeleeSequenceNode->AddBTNode(CTaskElecPandaAttackLoop::Create(make_shared<FTaskElecPandaAttackLoopDesc>(tTaskElecPandaAttackLoop)));
					}

				}

				// electric arrow sequence
				{
					tSequenceNodeDesc.m_wstrBTNodeName = L"ElectricArrowSequence";
					shared_ptr<CCompositeNode> spArrowSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
					spAttackSelectorNode->AddBTNode(spArrowSequenceNode);

					// skill type deco
					{
						FDecoratorSkillTypeDesc tDecoSkillType;
						tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
						tDecoSkillType.m_spOwnerGameObject = spElecPanda;
						tDecoSkillType.m_wstrBTNodeName = L"ElectricArrowSkillTypeDecorator";

						tDecoSkillType.m_iSkillType = (int32)EElecPandaSkillType::ELECTRIC_ARROW;
						spArrowSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
					}

					// look at task node
					{
						// task desc
						FTaskLookAtDesc tTaskLookAtDesc;
						tTaskLookAtDesc.m_spOwnerGameObject = spElecPanda;
						tTaskLookAtDesc.m_wstrBTNodeName = L"ElectricArrowLookAtTask";
						tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

						// look at task desc
						tTaskLookAtDesc.m_spTarget = m_spPlayer;
						tTaskLookAtDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Walk;
						spArrowSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
					}

					// encount task node
					{
						FTaskEncountDesc tEncountDesc;
						tEncountDesc.m_eBTNodeType = EBTNodeType::TASK;
						tEncountDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Encount;
						tEncountDesc.m_spTarget = m_spPlayer;
						tEncountDesc.m_spOwnerGameObject = spElecPanda;

						spArrowSequenceNode->AddBTNode(CTaskEncount::Create(make_shared<FTaskEncountDesc>(tEncountDesc)));
					}

					// Elecpanda attack loop task node; electric arrow
					{
						FTaskElecPandaAttackLoopDesc tTaskElecPandaAttackLoop;
						tTaskElecPandaAttackLoop.m_eBTNodeType				= EBTNodeType::TASK;
						tTaskElecPandaAttackLoop.m_spOwnerGameObject		= spElecPanda;
						tTaskElecPandaAttackLoop.m_wstrBTNodeName			= L"ElectricArrowAttackLoopTask";

						tTaskElecPandaAttackLoop.m_spPlayer							= m_spPlayer;
						tTaskElecPandaAttackLoop.m_eCurElecPandaSkillType			= EElecPandaSkillType::ELECTRIC_ARROW;
						tTaskElecPandaAttackLoop.m_iStartAnimation					= (int32)EElecPanda::AS_ElecPanda_FarSkill_Start;
						tTaskElecPandaAttackLoop.m_iAction00AnimationID				= (int32)EElecPanda::AS_ElecPanda_FarSkill_Action;
						tTaskElecPandaAttackLoop.m_iActionLoop00AnimationID			= (int32)EElecPanda::AS_ElecPanda_FarSkill_ActionLoop;
						tTaskElecPandaAttackLoop.m_fMaxActionLoop00DeltaSeconds		= 5.0f;
						tTaskElecPandaAttackLoop.m_iEndAnimationID					= (int32)EElecPanda::AS_ElecPanda_FarSkill_End;
						spArrowSequenceNode->AddBTNode(CTaskElecPandaAttackLoop::Create(make_shared<FTaskElecPandaAttackLoopDesc>(tTaskElecPandaAttackLoop)));
					}

				}

				/*
				// electric ball sequence
				{
					tSequenceNodeDesc.m_wstrBTNodeName = L"ElectricBallSequence";
					shared_ptr<CCompositeNode> spMeleeSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
					spAttackSelectorNode->AddBTNode(spMeleeSequenceNode);

					// skill type deco
					{
						FDecoratorSkillTypeDesc tDecoSkillType;
						tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
						tDecoSkillType.m_spOwnerGameObject = spElecPanda;
						tDecoSkillType.m_wstrBTNodeName = L"ElectricBallSkillTypeDecorator";

						tDecoSkillType.m_iSkillType = (int32)EElecPandaSkillType::ELECTRIC_BALL;
						spMeleeSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
					}

					// wide berth; 거리를 벌리다.
					{

					}

					// look at task node
					{
						// task desc
						FTaskLookAtDesc tTaskLookAtDesc;
						tTaskLookAtDesc.m_spOwnerGameObject = spElecPanda;
						tTaskLookAtDesc.m_wstrBTNodeName	= L"ElectricBallLookAtTask";
						tTaskLookAtDesc.m_eBTNodeType		= EBTNodeType::TASK;

						// look at task desc
						tTaskLookAtDesc.m_spTarget			= m_spPlayer;
						tTaskLookAtDesc.m_iAnimationID		= (int32)EElecPanda::AS_ElecPanda_Walk;
						spMeleeSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
					}

					// Elecpanda attack loop task node; electric ball
					{
						FTaskElecPandaAttackLoopDesc tTaskElecPandaAttackLoop;
						tTaskElecPandaAttackLoop.m_eBTNodeType					= EBTNodeType::TASK;
						tTaskElecPandaAttackLoop.m_spOwnerGameObject			= spElecPanda;
						tTaskElecPandaAttackLoop.m_wstrBTNodeName				= L"ElectricBallAttackLoopTask";

						tTaskElecPandaAttackLoop.m_spPlayer						= m_spPlayer;
						tTaskElecPandaAttackLoop.m_eCurElecPandaSkillType		= EElecPandaSkillType::ELECTRIC_BALL;
						tTaskElecPandaAttackLoop.m_iStartAnimation				= (int32)EElecPanda::AS_ElecPanda_FarSkill_Start;
						tTaskElecPandaAttackLoop.m_iStartLoopAnimation			= (int32)EElecPanda::AS_ElecPanda_FarSkill_StartLoop;
						tTaskElecPandaAttackLoop.m_fMaxStartLoopDeltaSeconds	= 1.0f;
						tTaskElecPandaAttackLoop.m_iAction00AnimationID			= (int32)EElecPanda::AS_ElecPanda_FarSkill_Action;
						tTaskElecPandaAttackLoop.m_iEndAnimationID				= (int32)EElecPanda::AS_ElecPanda_FarSkill_End;
						spMeleeSequenceNode->AddBTNode(CTaskElecPandaAttackLoop::Create(make_shared<FTaskElecPandaAttackLoopDesc>(tTaskElecPandaAttackLoop)));
					}
				}
				*/

				// lightning sequence
				{
					tSequenceNodeDesc.m_wstrBTNodeName = L"LightningSequence";
					shared_ptr<CCompositeNode> spLightningSequenceNode = CCompositeNode::Create(make_shared<FCompositeNodeDesc>(tSequenceNodeDesc));
					spAttackSelectorNode->AddBTNode(spLightningSequenceNode);

					// skill type deco
					{
						FDecoratorSkillTypeDesc tDecoSkillType;
						tDecoSkillType.m_eBTNodeType = EBTNodeType::DECORATOR;
						tDecoSkillType.m_spOwnerGameObject = spElecPanda;
						tDecoSkillType.m_wstrBTNodeName = L"LightningSkillTypeDecorator";

						tDecoSkillType.m_iSkillType = (int32)EElecPandaSkillType::LIGHTNING;
						spLightningSequenceNode->AddBTNode(CDecoratorSkillType::Create(make_shared<FDecoratorSkillTypeDesc>(tDecoSkillType)));
					}

					// look at task node
					{
						// task desc
						FTaskLookAtDesc tTaskLookAtDesc;
						tTaskLookAtDesc.m_spOwnerGameObject = spElecPanda;
						tTaskLookAtDesc.m_wstrBTNodeName = L"LightningLookAtTask";
						tTaskLookAtDesc.m_eBTNodeType = EBTNodeType::TASK;

						// look at task desc
						tTaskLookAtDesc.m_spTarget = m_spPlayer;
						tTaskLookAtDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Walk;
						spLightningSequenceNode->AddBTNode(CTaskLookAt::Create(make_shared<FTaskLookAtDesc>(tTaskLookAtDesc)));
					}

					// chase task
					{
						FTaskChaseDesc tChaseTask;
						tChaseTask.m_eBTNodeType = EBTNodeType::TASK;
						tChaseTask.m_fAcceptableRadius = 6.0f;
						tChaseTask.m_fMaxChaseDeltaSeconds = 15.0f;

						tChaseTask.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Run;
						tChaseTask.m_spOwnerGameObject = shared_from_this();
						tChaseTask.m_spTarget = CGameManager::GetInstance()->GetPlayer();
						tChaseTask.m_wstrBTNodeName = L"SnippingToDischargeChaseTask";

						spLightningSequenceNode->AddBTNode(CTaskChase::Create(make_shared<FTaskChaseDesc>(tChaseTask)));
					}

					// Elecpanda attack loop task node; lightning
					{
						FTaskElecPandaAttackLoopDesc tTaskElecPandaAttackLoop;
						tTaskElecPandaAttackLoop.m_eBTNodeType				= EBTNodeType::TASK;
						tTaskElecPandaAttackLoop.m_spOwnerGameObject		= spElecPanda;
						tTaskElecPandaAttackLoop.m_wstrBTNodeName			= L"LightningAttackLoopTask";

						tTaskElecPandaAttackLoop.m_spPlayer						= m_spPlayer;
						tTaskElecPandaAttackLoop.m_eCurElecPandaSkillType		= EElecPandaSkillType::LIGHTNING;
						tTaskElecPandaAttackLoop.m_iStartAnimation				= (int32)EElecPanda::AS_ElecPanda_FarSkill_Start;
						tTaskElecPandaAttackLoop.m_iStartLoopAnimation			= (int32)EElecPanda::AS_ElecPanda_FarSkill_StartLoop;
						tTaskElecPandaAttackLoop.m_fMaxStartLoopDeltaSeconds	= 1.5f;
						tTaskElecPandaAttackLoop.m_iAction00AnimationID			= (int32)EElecPanda::AS_ElecPanda_FarSkill_Action;
						tTaskElecPandaAttackLoop.m_iActionLoop00AnimationID		= (int32)EElecPanda::AS_ElecPanda_FarSkill_ActionLoop;
						tTaskElecPandaAttackLoop.m_fMaxActionLoop00DeltaSeconds = 3.0f;
						tTaskElecPandaAttackLoop.m_iEndAnimationID				= (int32)EElecPanda::AS_ElecPanda_FarSkill_End;
						spLightningSequenceNode->AddBTNode(CTaskElecPandaAttackLoop::Create(make_shared<FTaskElecPandaAttackLoopDesc>(tTaskElecPandaAttackLoop)));
					}
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
					tTaskIdleDesc.m_spOwnerGameObject = spElecPanda;
					tTaskIdleDesc.m_wstrBTNodeName = L"IdleTask";
					tTaskIdleDesc.m_eBTNodeType = EBTNodeType::TASK;

					// idle task desc
					tTaskIdleDesc.m_iAnimationID = (int32)EElecPanda::AS_ElecPanda_Idle;
					tTaskIdleDesc.m_fMaxIdleDeltaSeconds = 10.0f;
					spWanderSequenceNode->AddBTNode(CTaskIdle::Create(make_shared<FTaskIdleDesc>(tTaskIdleDesc)));
				}
			}
		}
	}

	// behavior tree desc
	m_spBehaviorTreeComponent->SetRootNode(m_spRootNode);
	m_spBehaviorTreeComponent->SetBehaveTree(true);
}

void Client::CElecPanda::InitTrailCombinedBones()
{
	m_arrLeftNailBones[0] = m_spMeshComponent->GetBonePtr(L"finger_01_01_l");
	m_arrLeftNailBones[1] = m_spMeshComponent->GetBonePtr(L"finger_02_01_l");
	m_arrLeftNailBones[2] = m_spMeshComponent->GetBonePtr(L"finger_03_01_l");

	m_arrRightNailBones[0] = m_spMeshComponent->GetBonePtr(L"finger_01_01_r");
	m_arrRightNailBones[1] = m_spMeshComponent->GetBonePtr(L"finger_02_01_r");
	m_arrRightNailBones[2] = m_spMeshComponent->GetBonePtr(L"finger_03_01_r");
}

void Client::CElecPanda::InitTrailComponent()
{
	#pragma region 왼손
	for (uint32 iInnerIndex = 0; iInnerIndex < 3; ++iInnerIndex)
	{
		FTrailComponentDesc tTrailComponentDesc;
		tTrailComponentDesc.m_wstrComponentName = L"LNail_TrailComponent" + to_wstring(iInnerIndex);
		tTrailComponentDesc.m_eRenderType		= ERenderType::RENDER_GLOW;
		tTrailComponentDesc.m_eComponentType	= EComponentType::PARTICLE;
		tTrailComponentDesc.m_wpOwnerGameObject = shared_from_this();

		tTrailComponentDesc.m_iNumRects			= 10;
		tTrailComponentDesc.m_fMaxLifeTime		= 0.002f;
		tTrailComponentDesc.m_vColor			= Color(1.f, 1.f, 0.25f, 1.f);

		tTrailComponentDesc.m_wstrEffectShaderName = L"FX_Trail_PT2";

		tTrailComponentDesc.m_bUseDiffuse		= false;
		tTrailComponentDesc.m_bUseNoise			= false;
		tTrailComponentDesc.m_bUseMask			= true;
		tTrailComponentDesc.m_bUseDistortion	= false;
		tTrailComponentDesc.m_bUseDissolve		= false;

		tTrailComponentDesc.m_wstrDiffuseTextureName	= L"";
		tTrailComponentDesc.m_wstrNoiseTextureName		= L"";
		tTrailComponentDesc.m_wstrMaskTextureName		= L"T_Gradient11";
		tTrailComponentDesc.m_wstrDistortionTextureName = L"";
		tTrailComponentDesc.m_wstrDissolveTextureName	= L"";

		shared_ptr<CTrailComponent> spTrailComponent = CTrailComponent::Create(make_shared<FTrailComponentDesc>(tTrailComponentDesc));
		CHECKF(spTrailComponent, L"Failed to Create TrailComponent : CElecPanda");

		m_arrLeftNailTrails[iInnerIndex] = spTrailComponent;
		
		AddComponent(tTrailComponentDesc.m_wstrComponentName, spTrailComponent);
	}
#pragma endregion
	#pragma region 오른손
	for (uint32 iInnerIndex = 0; iInnerIndex < 3; ++iInnerIndex)
	{
		FTrailComponentDesc tTrailComponentDesc;
		tTrailComponentDesc.m_wstrComponentName = L"RNail_TrailComponent" + to_wstring(iInnerIndex);
		tTrailComponentDesc.m_eRenderType = ERenderType::RENDER_GLOW;
		tTrailComponentDesc.m_eComponentType = EComponentType::PARTICLE;
		tTrailComponentDesc.m_wpOwnerGameObject = shared_from_this();

		tTrailComponentDesc.m_iNumRects = 10;
		tTrailComponentDesc.m_fMaxLifeTime = 0.002f;
		tTrailComponentDesc.m_vColor = Color(1.f, 1.f, 0.25f, 1.f);

		tTrailComponentDesc.m_wstrEffectShaderName = L"FX_Trail_PT2";

		tTrailComponentDesc.m_bUseDiffuse = false;
		tTrailComponentDesc.m_bUseNoise = false;
		tTrailComponentDesc.m_bUseMask = true;
		tTrailComponentDesc.m_bUseDistortion = false;
		tTrailComponentDesc.m_bUseDissolve = false;

		tTrailComponentDesc.m_wstrDiffuseTextureName = L"";
		tTrailComponentDesc.m_wstrNoiseTextureName = L"";
		tTrailComponentDesc.m_wstrMaskTextureName = L"T_Gradient11";
		tTrailComponentDesc.m_wstrDistortionTextureName = L"";
		tTrailComponentDesc.m_wstrDissolveTextureName = L"";

		shared_ptr<CTrailComponent> spTrailComponent = CTrailComponent::Create(make_shared<FTrailComponentDesc>(tTrailComponentDesc));
		CHECKF(spTrailComponent, L"Failed to Create TrailComponent : CElecPanda");

		m_arrRightNailTrails[iInnerIndex] = spTrailComponent;

		AddComponent(tTrailComponentDesc.m_wstrComponentName, spTrailComponent);
	}
#pragma endregion
}

void Client::CElecPanda::InitProjectile()
{
	// component desc
	FTrailComponentDesc tTrailComponentDesc;
	tTrailComponentDesc.m_wstrComponentName = L"TrailComponent";
	tTrailComponentDesc.m_eRenderType = ERenderType::RENDER_GLOW;
	tTrailComponentDesc.m_eComponentType = EComponentType::PARTICLE;
	tTrailComponentDesc.m_wpOwnerGameObject = shared_from_this();

	// trail component desc
	tTrailComponentDesc.m_iNumRects = 10;
	tTrailComponentDesc.m_fMaxLifeTime = 0.001f;
	tTrailComponentDesc.m_vColor = Color(1.f, 1.f, 0.6f, 1.f);

	tTrailComponentDesc.m_wstrEffectShaderName = L"FX_Trail_PT2";

	tTrailComponentDesc.m_bUseDiffuse = false;
	tTrailComponentDesc.m_bUseNoise = false;
	tTrailComponentDesc.m_bUseMask = true;
	tTrailComponentDesc.m_bUseDistortion = false;
	tTrailComponentDesc.m_bUseDissolve = false;

	tTrailComponentDesc.m_wstrDiffuseTextureName = L"";
	tTrailComponentDesc.m_wstrNoiseTextureName = L"";
	tTrailComponentDesc.m_wstrMaskTextureName = L"T_Gradient11";
	tTrailComponentDesc.m_wstrDistortionTextureName = L"";
	tTrailComponentDesc.m_wstrDissolveTextureName = L"";

	// projectile effect group desc
	FProjectileEffectGroupDesc tProjectileEffectGroupDesc;
	ZeroMemory(&tProjectileEffectGroupDesc, sizeof(FProjectileEffectGroupDesc));
	tProjectileEffectGroupDesc.m_bActive = false;
	tProjectileEffectGroupDesc.m_vInitScale = Vector3(1.f);
	tProjectileEffectGroupDesc.m_spOwner = static_pointer_cast<CCreature>(shared_from_this());
	tProjectileEffectGroupDesc.m_spTarget = nullptr;
	tProjectileEffectGroupDesc.m_eOwnerMonsterType = EMonsterType::ELEC_PANDA;
	tProjectileEffectGroupDesc.m_bTrail = true;
	tProjectileEffectGroupDesc.m_spTrailCompoonentDesc = make_shared<FTrailComponentDesc>(tTrailComponentDesc);

	shared_ptr<CLevel> spLevel = CWorld::GetInstance()->GetPostFrontLevel();

	// Electric Arrow 
	{
		tProjectileEffectGroupDesc.m_iProjectileSlot = (int32)EElecPandaEffectSlots::ELECTRIC_ARROW;
		tProjectileEffectGroupDesc.m_wstrEffectGroupName = L"Group_ElecPanda_ElectricBall";

		for (int32 iInnerIndex = 0; iInnerIndex < 30; ++iInnerIndex)
		{
			shared_ptr<CProjectileEffectGroup> spProjectileEffectGroup = CProjectileEffectGroup::CreateFromJson(make_shared<FProjectileEffectGroupDesc>(tProjectileEffectGroupDesc), L"Group_ElecPanda_ElectricBall");
			CHECKF(spProjectileEffectGroup, L"Failed to Create CProjectileEffect : CTaskDreamDemonAttackLoop");

			spLevel->AddGameObject(spProjectileEffectGroup);
			spProjectileEffectGroup->SetPosition(Vector3(0.f, -1000.f, 0.f));
			m_umapProjectileEffectGroups[(int32)EElecPandaEffectSlots::ELECTRIC_ARROW].push_back(spProjectileEffectGroup);
		}
	}

	// Electric Trace Ball 
	{
		tProjectileEffectGroupDesc.m_bTrail = false;
		tProjectileEffectGroupDesc.m_spTrailCompoonentDesc = nullptr;
		tProjectileEffectGroupDesc.m_iProjectileSlot = (int32)EElecPandaEffectSlots::ELECTRIC_BALL;
		tProjectileEffectGroupDesc.m_wstrEffectGroupName = L"Group_ElecPanda_ElectricBall_Trace";

		for (int32 iInnerIndex = 0; iInnerIndex < 10; ++iInnerIndex)
		{
			shared_ptr<CProjectileEffectGroup> spProjectileEffectGroup = CProjectileEffectGroup::CreateFromJson(make_shared<FProjectileEffectGroupDesc>(tProjectileEffectGroupDesc), L"Group_ElecPanda_ElectricBall_Trace");
			CHECKF(spProjectileEffectGroup, L"Failed to Create CProjectileEffect : CTaskDreamDemonAttackLoop");

			spLevel->AddGameObject(spProjectileEffectGroup);
			spProjectileEffectGroup->SetPosition(Vector3(0.f, -1000.f, 0.f));
			m_umapProjectileEffectGroups[(int32)EElecPandaEffectSlots::ELECTRIC_BALL].push_back(spProjectileEffectGroup);
		}
	}

	// Lightning
	{
		tProjectileEffectGroupDesc.m_bTrail = false;
		tProjectileEffectGroupDesc.m_spTrailCompoonentDesc = nullptr;
		tProjectileEffectGroupDesc.m_iProjectileSlot = (int32)EElecPandaEffectSlots::LIGHTING;
		tProjectileEffectGroupDesc.m_wstrEffectGroupName = L"Group_ElecPanda_ThunderPillar_ShockWave";

		for (int32 iInnerIndex = 0; iInnerIndex < 10; ++iInnerIndex)
		{
			shared_ptr<CProjectileEffectGroup> spProjectileEffectGroup = CProjectileEffectGroup::CreateFromJson(make_shared<FProjectileEffectGroupDesc>(tProjectileEffectGroupDesc), L"Group_ElecPanda_ThunderPillar_ShockWave");
			CHECKF(spProjectileEffectGroup, L"Failed to Create CProjectileEffect : CTaskDreamDemonAttackLoop");

			spLevel->AddGameObject(spProjectileEffectGroup);
			spProjectileEffectGroup->SetPosition(Vector3(0.f, -1000.f, 0.f));
			m_umapProjectileEffectGroups[(int32)EElecPandaEffectSlots::LIGHTING].push_back(spProjectileEffectGroup);
		}
	}
}

void Client::CElecPanda::LookAt(const Vector3& _vPosition)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->LookAt(_vPosition);
}

void Client::CElecPanda::Turn(const Vector3& _vAxis, float _fAngularSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->Turn(_vAxis, _fAngularSpeed);
}

void Client::CElecPanda::MoveForward(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderForward(_fSpeed);
}

void Client::CElecPanda::MoveBackward(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderBackward(_fSpeed);
}

void Client::CElecPanda::MoveRight(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderRight(_fSpeed);
}

void Client::CElecPanda::MoveLeft(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderLeft(_fSpeed);
}

void Client::CElecPanda::MoveUp(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderUp(_fSpeed);
}

void Client::CElecPanda::MoveDown(float _fSpeed)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->MoveColliderDown(_fSpeed);
}

void Client::CElecPanda::UpdateBlackboardValues()
{
	CMonster::UpdateBlackboardValues();
}

void Client::CElecPanda::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CMonster::OnTriggerEnter(_pSrcComponent, _pDstComponent);

	// 컨트롤러 컴포넌트
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER)
	{
		CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
		CControllerComponent* pDstControllerComponent = dynamic_cast<CControllerComponent*>(_pDstComponent);

		ECollisionLayer eSrcCollisionLayer = pSrcColliderComponent->GetCollisionLayer();
		ECollisionLayer eDstCollisionLayer = pDstControllerComponent->GetCollisionLayer();

		// 일렉 판다가 플레이어를 공격합니다.
		if (m_eMonsterAIType == EMonsterAIType::WILD_PAL)
		{
			if (eSrcCollisionLayer == ECollisionLayer::MONSTER_WEAPON	// 야생 일렉 판다의 공격 콜라이더가
				&& eDstCollisionLayer == ECollisionLayer::PLAYER)		// 플레이어와 부딪힌 경우
			{
				ModifyBoolValue(EBlackboardBoolValue::BOOL_ATTACKED_TARGET, true);

				shared_ptr<CPlayer> spPlayer = static_pointer_cast<CPlayer>(pDstControllerComponent->GetOwnerGameObject());

				// 플레이어에게 대미지를 줍니다.
				spPlayer->TakeDamage(shared_from_this(), CClientLoop::s_spClientDataSheet->k_mapPalDefaultAttackPowers[EMonsterType::ELEC_PANDA]);
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

void Client::CElecPanda::OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	CMonster::OnCollisionEnter(_pSrcComponent, _pDstComponent);
}

void Client::CElecPanda::ActiveEffectObject(EElecPandaEffectSlots _eSlot, const Vector3& _vStartPos, const Vector3& _vDestPos, shared_ptr<CCreature> _spTarget)
{
	switch (_eSlot)
	{
	case Client::EElecPandaEffectSlots::ELECTRIC_ARROW:
		for (auto& spEffect : m_umapProjectileEffectGroups[(int32)_eSlot])
		{
			if (!spEffect->IsActive())
			{
				spEffect->SetPosition(_vStartPos);
				spEffect->SetTarget(_spTarget);
				spEffect->Reset();
				spEffect->LookAt(_spTarget->GetPosition().value());
				spEffect->ClearTrail();
				spEffect->SetDestinationPos(_vDestPos);
				spEffect->SetSourcePos(_vStartPos);

				break;
			}
		}
		break;

	case Client::EElecPandaEffectSlots::ELECTRIC_BALL:
		for (auto& spEffect : m_umapProjectileEffectGroups[(int32)_eSlot])
		{
			if (!spEffect->IsActive())
			{
				spEffect->SetPosition(GetPosition().value() + Vector3(0.f, 2.5f, 0.f));
				spEffect->SetTarget(_spTarget);
				spEffect->Reset();
				spEffect->LookAt(_spTarget->GetPosition().value());
				break;
			}
		}
		break;

	case Client::EElecPandaEffectSlots::LIGHTING:
		for (auto& spEffect : m_umapProjectileEffectGroups[(int32)_eSlot])
		{
			if (!spEffect->IsActive())
			{
				spEffect->SetPosition(_vStartPos);
				spEffect->SetTarget(_spTarget);
				spEffect->Reset();
				spEffect->LookAt(_spTarget->GetPosition().value());
				break;
			}
		}
		break;
	}
}

void Client::CElecPanda::TickTrail(float _fTrailSize)
{
#pragma region 왼손
	for (uint32 i = 0; i < 3; ++i)
	{
		if (!m_arrLeftNailTrails[i]->IsActive()) { continue; }

		// Matrix matCombinedOffset = Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
		Matrix matCombinedBone = m_arrLeftNailBones[i].lock()->GetCombinedTransformationMatrix();
		Matrix matCombinedObject = GetTransformMatrix().value();

		Matrix matFinal = /*matCombinedOffset */matCombinedBone * matCombinedObject;
		Vector3 vFinalPosition = matFinal.Translation();

		Vector3 vTrailStartPos = vFinalPosition;
		vTrailStartPos.y -= _fTrailSize;

		Vector3 vTrailEndPos = vFinalPosition;
		vTrailEndPos.y += _fTrailSize;

		m_arrLeftNailTrails[i]->TickTrail(vTrailStartPos, vTrailEndPos);
	}
#pragma endregion
#pragma region 오른손
	for (uint32 i = 0; i < 3; ++i)
	{
		if (!m_arrRightNailTrails[i]->IsActive()) { continue; }

		// Matrix matCombinedOffset = Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
		Matrix matCombinedBone = m_arrRightNailBones[i].lock()->GetCombinedTransformationMatrix();
		Matrix matCombinedObject = GetTransformMatrix().value();

		Matrix matFinal = /*matCombinedOffset */matCombinedBone * matCombinedObject;
		Vector3 vFinalPosition = matFinal.Translation();

		Vector3 vTrailStartPos = vFinalPosition;
		vTrailStartPos.y -= _fTrailSize;

		Vector3 vTrailEndPos = vFinalPosition;
		vTrailEndPos.y += _fTrailSize;

		m_arrRightNailTrails[i]->TickTrail(vTrailStartPos, vTrailEndPos);
	}
#pragma endregion
}

void Client::CElecPanda::OnLNailTrails(float _fActiveTime)
{
	for (uint32 i = 0; i < 3; ++i)
	{
		Matrix matCombinedBone = m_arrLeftNailBones[i].lock()->GetCombinedTransformationMatrix();
		Matrix matCombinedObject = GetTransformMatrix().value();

		Matrix matFinal = /*matCombinedOffset */matCombinedBone * matCombinedObject;
		Vector3 vFinalPos = matFinal.Translation();

		Vector3 vStartPos = vFinalPos;
		vStartPos.y -= 0.1f;

		Vector3 vEndPos = vFinalPos;
		vEndPos.y += 0.1f;

		m_arrLeftNailTrails[i]->ClearRectList(vStartPos, vEndPos);
		m_arrLeftNailTrails[i]->SetActive(true);
	}
	m_bOnTrail = true;
	m_fTrailTimeAcc = 0.f;
	m_fTrailActiveTime = _fActiveTime;
}

void Client::CElecPanda::OnRNailTrails(float _fActiveTime)
{
	for (uint32 i = 0; i < 3; ++i)
	{
		Matrix matCombinedBone = m_arrRightNailBones[i].lock()->GetCombinedTransformationMatrix();
		Matrix matCombinedObject = GetTransformMatrix().value();

		Matrix matFinal = /*matCombinedOffset */matCombinedBone * matCombinedObject;
		Vector3 vFinalPos = matFinal.Translation();

		Vector3 vStartPos = vFinalPos;
		vStartPos.y -= 0.1f;

		Vector3 vEndPos = vFinalPos;
		vEndPos.y += 0.1f;

		m_arrRightNailTrails[i]->ClearRectList(vStartPos, vEndPos);
		m_arrRightNailTrails[i]->SetActive(true);
	}
	m_bOnTrail = true;
	m_fTrailTimeAcc = 0.f;
	m_fTrailActiveTime = _fActiveTime;
}

void Client::CElecPanda::OffAllNailTrails()
{
	for (uint32 i = 0; i < 3; ++i)
	{
		m_arrLeftNailTrails[i]->SetActive(false);
		m_arrRightNailTrails[i]->SetActive(false);
	}
	m_bOnTrail = true;
	m_fTrailTimeAcc = 0.f;
}

void Client::CElecPanda::CheckTrailActiveTime(float _fDeltaSeconds)
{
	m_fTrailTimeAcc += _fDeltaSeconds;
	// 트레일 지속 시간이 끝난 경우
	if (m_fTrailActiveTime <= m_fTrailTimeAcc)
	{
		for (auto& trailcom : m_arrLeftNailTrails)
		{
			trailcom->SetActive(false);
		}
		for (auto& trailcom : m_arrRightNailTrails)
		{
			trailcom->SetActive(false);
		}

		m_bOnTrail = false;
		m_fTrailTimeAcc = 0.f;
	}
}

void Client::CElecPanda::PlaySoundWithID(const FAnimNotifyData& _tNotifyData)
{
	FSoundNotifyData tSoundNotifyData = _tNotifyData.m_tSoundNotify;

	m_wpSoundManager.lock()->PlaySoundEx(CStringUtility::ToString(tSoundNotifyData.m_wstrSoundID));
}

std::shared_ptr<CColliderComponent> Client::CElecPanda::GetBodyColliderComponent()
{
	return m_spBodySphereComponent;
}

std::shared_ptr<CColliderComponent> Client::CElecPanda::GetAttackColliderComponent()
{
	return m_spAttackSphereComponent;
}

void Client::CElecPanda::SetLinearVelocity(const PxVec3& _vLinearVelocity)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetLinearVelocity(_vLinearVelocity);
}

void Client::CElecPanda::SetAngularVelocity(const PxVec3& _vLinearVelocity)
{
	if (!m_spBodySphereComponent) { return; }

	m_spBodySphereComponent->SetAngularVelocity(_vLinearVelocity);
}
