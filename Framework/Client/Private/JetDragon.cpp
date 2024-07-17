// default
#include "pch.h"
#include "JetDragon.h"

// manager
#include "InputManager.h"
#include "EffectManager.h"

// gameobject
#include "EffectGroup.h"

// component
#include "MathUtility.h"
#include "MeshComponent.h"
#include "FSMComponent.h"
#include "BoxComponent.h"
#include "SphereComponent.h"
#include "TrailComponent.h"

// etc
#include "Bone.h"
#include "FSMNode_JetDragonIdle.h"
#include "FSMNode_JetDragonMove.h"

shared_ptr<CJetDragon> Client::CJetDragon::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CJetDragon> spInstance = make_shared<CJetDragon>();
	ENSUREF(spInstance->Initialize(_spGameObjectDesc), L"Failed to initialize CJetDragon.");
	return spInstance;
}

HRESULT Client::CJetDragon::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CRidingObject::Initialize(_spGameObjectDesc);

	InitCombinedBones();
	InitRidingParticles();
	InitRidingTrails();

	OffRidingEffect();

	return S_OK;
}

HRESULT Client::CJetDragon::BeginPlay()
{
	CRidingObject::BeginPlay();

	return S_OK;
}

int32 Client::CJetDragon::PreTick(float _fDeltaSeconds)
{
	CRidingObject::PreTick(_fDeltaSeconds);

	if (m_tRidingObjectData.m_bOnGround || m_tRidingObjectData.m_fCurrentStaminaGauge <= 0.f)
	{
		m_spSphereCollider->SetUseGravity(true);
	}

	return 0;
}

int32 Client::CJetDragon::Tick(float _fDeltaSeconds)
{
	CRidingObject::Tick(_fDeltaSeconds);

	if (m_bActivateTrail) { TickTrail(0.075f); }

	return 0;
}

int32 Client::CJetDragon::PostTick(float _fDeltaSeconds)
{
	CRidingObject::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CJetDragon::EndPlay()
{
	CRidingObject::EndPlay();

	return S_OK;
}

void Client::CJetDragon::Release()
{
	for (uint32 i = 0; i < 4; ++i)
	{
		m_arrFlyingParticles[i] = nullptr;
	}

	for (uint32 i = 0; i < 2; ++i)
	{
		m_arrFlyingTrails[i] = nullptr;
	}

	CRidingObject::Release();
}

void Client::CJetDragon::KeyInput()
{
	if (!m_wpRider.expired())
	{
		if (m_wpInputManager.lock()->IsKeyPressed(DIK_SPACE))
		{
			if (m_tRidingObjectData.m_fCurrentStaminaGauge > 0.f)
			{
				m_spSphereCollider->SetUseGravity(false);
				m_tRidingObjectData.m_vMoveDirection.y = 1.f;
			}
		}
		else if (m_wpInputManager.lock()->IsKeyPressed(DIK_LCONTROL))
		{
			if (!m_tRidingObjectData.m_bOnGround && m_tRidingObjectData.m_fCurrentStaminaGauge > 0.f)
			{
				m_spSphereCollider->SetUseGravity(false);
				m_tRidingObjectData.m_vMoveDirection.y = -1.f;
			}
		}
		else
		{
			m_tRidingObjectData.m_vMoveDirection.y = 0.f;
		}
	}

	CRidingObject::KeyInput();
}

void Client::CJetDragon::TickTrail(float _fTrailSize)
{
#pragma region 1번 트레일
	Matrix matCombinedOffset = Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
	Matrix matCombinedBone = m_arrCombinedBones[0].lock()->GetCombinedTransformationMatrix();
	Matrix matCombinedObject = GetTransformMatrix().value();

	Matrix matFinal = matCombinedOffset * matCombinedBone * matCombinedObject;
	Vector3 vFinalPosition = matFinal.Translation();

	Vector3 vTrailStartPos = vFinalPosition;
	vTrailStartPos.y -= _fTrailSize;

	Vector3 vTrailEndPos = vFinalPosition;
	vTrailEndPos.y += _fTrailSize;

	m_arrFlyingTrails[0]->TickTrail(vTrailStartPos, vTrailEndPos);
#pragma endregion
#pragma region 2번 트레일
	matCombinedOffset = Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
	matCombinedBone = m_arrCombinedBones[3].lock()->GetCombinedTransformationMatrix();
	matCombinedObject = GetTransformMatrix().value();

	matFinal = matCombinedOffset * matCombinedBone * matCombinedObject;
	vFinalPosition = matFinal.Translation();

	vTrailStartPos = vFinalPosition;
	vTrailStartPos.y -= _fTrailSize;

	vTrailEndPos = vFinalPosition;
	vTrailEndPos.y += _fTrailSize;

	m_arrFlyingTrails[1]->TickTrail(vTrailStartPos, vTrailEndPos);
#pragma endregion
}

void Client::CJetDragon::InitRidingBone()
{
	m_wpRidingBone = m_spMeshComponent->GetBonePtr(L"neck_02");
}

void Client::CJetDragon::InitMeshComponent()
{
	FMeshComponentDesc tMeshCompnentDesc;
	tMeshCompnentDesc.m_wstrComponentName = L"MeshComponent";
	tMeshCompnentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshCompnentDesc.m_eComponentType = EComponentType::MESH;
	tMeshCompnentDesc.m_wpOwnerGameObject = shared_from_this();

	// mesh component desc
	tMeshCompnentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/JetDragon.dat";
	tMeshCompnentDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
	tMeshCompnentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(180.f));

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshCompnentDesc));
	AddComponent(tMeshCompnentDesc.m_wstrComponentName, m_spMeshComponent);

	m_spMeshComponent->ChangeAnimation((int32)EJetDragon::AS_JetDragon_Idle, false);
}

void Client::CJetDragon::InitColliderComponent()
{
	// Body Sphere Collider
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
		tSphereComponentDesc.m_bDeferredAdd = false;


		tSphereComponentDesc.m_eCollisionFlag = ECollisionFlag::RIDING;
		tSphereComponentDesc.m_eCollisionLayer = ECollisionLayer::RIDING;

		tSphereComponentDesc.m_fMass = 10.0f;
		tSphereComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
		tSphereComponentDesc.m_vLocalPosition = Vector3(0.0f, 1.f, 0.0f);
		tSphereComponentDesc.m_vMaterial = Vector3(1.0f, 1.0f, 0.0f);

		// sphere component desc
		tSphereComponentDesc.m_fRadius = 1.f;

		m_spSphereCollider = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
		ENSURE(AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spSphereCollider));
	}

	// Jump Trigger
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
		tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::RIDING;
		tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::RIDING;
		tBoxComponentDesc.m_bGravity = false;
		tBoxComponentDesc.m_bTrigger = true;
		tBoxComponentDesc.m_bDeferredAdd = false;

		// box component desc
		tBoxComponentDesc.m_vExtents = Vector3(0.6f, 0.2f, 0.6f);

		m_spBoxTrigger_ForEnv = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxTrigger_ForEnv);

		tBoxComponentDesc.m_wstrComponentName = L"JumpBoxComponent2";
		m_spBoxTrigger_ForEnv2 = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxTrigger_ForEnv2);
	}
}

void Client::CJetDragon::InitFSMComponent()
{
	CRidingObject::InitFSMComponent();

	// FSM Component Node 채워넣기
	m_spFSMComponent->AddState((int32)ERidingObjectState::IDLE, CFSMNode_JetDragonIdle::Create(m_spMeshComponent, static_pointer_cast<CJetDragon>(shared_from_this())));
	m_spFSMComponent->AddState((int32)ERidingObjectState::MOVE, CFSMNode_JetDragonMove::Create(m_spMeshComponent, static_pointer_cast<CJetDragon>(shared_from_this())));
}

void Client::CJetDragon::InitCombinedBones()
{
	m_arrCombinedBones[0] = m_spMeshComponent->GetBonePtr(L"BeamSocket_L2"); // 맨 왼쪽 날개
	m_arrCombinedBones[1] = m_spMeshComponent->GetBonePtr(L"BeamSocket_L1");
	m_arrCombinedBones[2] = m_spMeshComponent->GetBonePtr(L"BeamSocket_R1");
	m_arrCombinedBones[3] = m_spMeshComponent->GetBonePtr(L"BeamSocket_R2"); // 맨 오른쪽 날개
}

void Client::CJetDragon::InitRidingParticles()
{
	for (uint32 iInnerIndex = 0; iInnerIndex < 4; ++iInnerIndex)
	{
		shared_ptr<CEffectGroup> spFlyingParticle = m_wpEffectManager.lock()->FindEffect(L"Group_JetDragon_Flying");
		CHECKF(spFlyingParticle, L"Failed to find Group_JetDragon_Flying : CJetDragon");

		m_arrFlyingParticles[iInnerIndex] = spFlyingParticle;
		m_arrFlyingParticles[iInnerIndex]->SetActive(true);
		m_arrFlyingParticles[iInnerIndex]->SetCombinedOffset(Vector3(1.1f, 0.f, 0.f));
		m_arrFlyingParticles[iInnerIndex]->SetCombinedBone(m_arrCombinedBones[iInnerIndex].lock());
		m_arrFlyingParticles[iInnerIndex]->SetCombinedGameObject(shared_from_this());

		AddChildGameObject(m_arrFlyingParticles[iInnerIndex]);
	}
}

void Client::CJetDragon::InitRidingTrails()
{
	for (uint32 iInnerIndex = 0; iInnerIndex < 2; ++iInnerIndex)
	{
		FTrailComponentDesc tTrailComponentDesc;
		tTrailComponentDesc.m_wstrComponentName = L"TrailComponent" + to_wstring(iInnerIndex);
		tTrailComponentDesc.m_eRenderType		= ERenderType::RENDER_GLOW;
		tTrailComponentDesc.m_eComponentType	= EComponentType::PARTICLE;
		tTrailComponentDesc.m_wpOwnerGameObject = shared_from_this();

		tTrailComponentDesc.m_iNumRects			= 20;
		tTrailComponentDesc.m_fMaxLifeTime		= 0.002f;
		tTrailComponentDesc.m_vColor			= Color(1.f, 0.52f, 0.94f, 1.f);

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
		CHECKF(spTrailComponent, L"Failed to Create TrailComponent : CJetDragon");

		m_arrFlyingTrails[iInnerIndex] = spTrailComponent;
		
		AddComponent(tTrailComponentDesc.m_wstrComponentName, spTrailComponent);
	}
}

void Client::CJetDragon::OnRidingEffect()
{
	for (uint32 i = 0; i < 4; ++i)
	{
		if (!m_arrFlyingParticles[i]) { continue;}

		m_arrFlyingParticles[i]->SetCombinedOffset(Vector3(1.1f, 0.f, 0.f));
		m_arrFlyingParticles[i]->SetCombinedBone(m_arrCombinedBones[i].lock());
		m_arrFlyingParticles[i]->SetCombinedGameObject(shared_from_this());
		m_arrFlyingParticles[i]->Reset();
	}

#pragma region 1번 트레일
	Matrix matCombinedOffset	= Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
	Matrix matCombinedBone		= m_arrCombinedBones[0].lock()->GetCombinedTransformationMatrix();
	Matrix matCombinedObject	= GetTransformMatrix().value();

	Matrix matFinal = matCombinedOffset * matCombinedBone * matCombinedObject;
	Vector3 vFinalPosition = matFinal.Translation();

	Vector3 vTrailStartPos = vFinalPosition;
	vTrailStartPos.y -= 0.075f;

	Vector3 vTrailEndPos = vFinalPosition;
	vTrailEndPos.y += 0.075f;

	m_arrFlyingTrails[0]->ClearRectList(vTrailStartPos, vTrailEndPos);
#pragma endregion
#pragma region 2번 트레일
	matCombinedOffset	= Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
	matCombinedBone		= m_arrCombinedBones[3].lock()->GetCombinedTransformationMatrix();
	matCombinedObject	= GetTransformMatrix().value();

	matFinal = matCombinedOffset * matCombinedBone * matCombinedObject;
	vFinalPosition = matFinal.Translation();

	vTrailStartPos = vFinalPosition;
	vTrailStartPos.y -= 0.075f;

	vTrailEndPos = vFinalPosition;
	vTrailEndPos.y += 0.075f;

	m_arrFlyingTrails[1]->ClearRectList(vTrailStartPos, vTrailEndPos);
#pragma endregion

	m_bActivateTrail = true;
}

void Client::CJetDragon::OffRidingEffect()
{
	for (uint32 i = 0; i < 4; ++i)
	{
		if (!m_arrFlyingParticles[i]) { continue; }

		m_arrFlyingParticles[i]->EndLifeTime();
	}

#pragma region 1번 트레일
	Matrix matCombinedOffset = Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
	Matrix matCombinedBone = m_arrCombinedBones[0].lock()->GetCombinedTransformationMatrix();
	Matrix matCombinedObject = GetTransformMatrix().value();

	Matrix matFinal = matCombinedOffset * matCombinedBone * matCombinedObject;
	Vector3 vFinalPosition = matFinal.Translation();

	Vector3 vTrailStartPos = vFinalPosition;
	vTrailStartPos.y -= 0.1f;

	Vector3 vTrailEndPos = vFinalPosition;
	vTrailEndPos.y += 0.1f;

	m_arrFlyingTrails[0]->ClearRectList(vTrailStartPos, vTrailEndPos);
#pragma endregion
#pragma region 2번 트레일
	matCombinedOffset = Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
	matCombinedBone = m_arrCombinedBones[3].lock()->GetCombinedTransformationMatrix();
	matCombinedObject = GetTransformMatrix().value();

	matFinal = matCombinedOffset * matCombinedBone * matCombinedObject;
	vFinalPosition = matFinal.Translation();

	vTrailStartPos = vFinalPosition;
	vTrailStartPos.y -= 0.1f;

	vTrailEndPos = vFinalPosition;
	vTrailEndPos.y += 0.1f;

	m_arrFlyingTrails[1]->ClearRectList(vTrailStartPos, vTrailEndPos);
#pragma endregion


	m_bActivateTrail = false;
}
