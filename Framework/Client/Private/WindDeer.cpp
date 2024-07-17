// default
#include "pch.h"
#include "WindDeer.h"

// manager
#include "InputManager.h"
#include "EffectManager.h"

// gameobject
#include "GameObject.h"

// component
#include "MeshComponent.h"
#include "FSMComponent.h"
#include "BoxComponent.h"
#include "SphereComponent.h"
#include "TrailComponent.h"

// FSM node
#include "FSMNode_WindDeerIdle.h"
#include "FSMNode_WindDeerMove.h"
#include "FSMNode_WindDeerJump.h"

// etc
#include "Bone.h"

shared_ptr<CWindDeer> Client::CWindDeer::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CWindDeer> spInstance = make_shared<CWindDeer>();
	ENSUREF(spInstance->Initialize(_spGameObjectDesc), L"Failed to initialize CWindDeer.");
	return spInstance;
}

HRESULT Client::CWindDeer::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CRidingObject::Initialize(_spGameObjectDesc);

	InitCombinedBones();
	InitRidingParticles();
	InitRidingTrails();

	OffRidingEffect();

	return S_OK;
}

HRESULT Client::CWindDeer::BeginPlay()
{
	CRidingObject::BeginPlay();

	return S_OK;
}

int32 Client::CWindDeer::PreTick(float _fDeltaSeconds)
{
	CRidingObject::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CWindDeer::Tick(float _fDeltaSeconds)
{
	CRidingObject::Tick(_fDeltaSeconds);

	if (!m_wpRider.expired())
	{
		if (m_tRidingObjectData.m_bOnGround && m_wpInputManager.lock()->IsKeyDown(DIK_SPACE)) { Jump(); }
	}

	if (m_bActivateTrail) { TickTrail(0.05f); }

	return 0;
}

int32 Client::CWindDeer::PostTick(float _fDeltaSeconds)
{
	CRidingObject::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CWindDeer::EndPlay()
{
	CRidingObject::EndPlay();

	return S_OK;
}

void Client::CWindDeer::Release()
{
	CRidingObject::Release();
}

void Client::CWindDeer::InitRidingBone()
{
	m_wpRidingBone = m_spMeshComponent->GetBonePtr(L"spine_01");
}

void Client::CWindDeer::InitMeshComponent()
{
	FMeshComponentDesc tMeshCompnentDesc;
	tMeshCompnentDesc.m_wstrComponentName = L"MeshComponent";
	tMeshCompnentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tMeshCompnentDesc.m_eComponentType = EComponentType::MESH;
	tMeshCompnentDesc.m_wpOwnerGameObject = shared_from_this();

	// mesh component desc
	tMeshCompnentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/FengyunDeeper.dat";
	tMeshCompnentDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
	tMeshCompnentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateRotationY(XMConvertToRadians(180.f));

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tMeshCompnentDesc));
	AddComponent(tMeshCompnentDesc.m_wstrComponentName, m_spMeshComponent);

	m_spMeshComponent->ChangeAnimation((int32)EFengyunDeeper::AS_FengyunDeeper_Idle, false);

}

void Client::CWindDeer::InitColliderComponent()
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

		tSphereComponentDesc.m_fMass = 100.0f;
		tSphereComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Y | (int32)EAxisLockType::LOCK_ANGULAR_Z;
		tSphereComponentDesc.m_vLocalPosition = Vector3(0.0f, 0.5f, 0.0f);
		tSphereComponentDesc.m_vMaterial = Vector3(1.0f, 1.0f, 0.05f);

		// sphere component desc
		tSphereComponentDesc.m_fRadius = 0.5f;

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
		tBoxComponentDesc.m_vExtents = Vector3(0.6f, 0.6f, 0.6f);

		m_spBoxTrigger_ForEnv = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxTrigger_ForEnv);

		tBoxComponentDesc.m_wstrComponentName = L"JumpBoxComponent2";
		m_spBoxTrigger_ForEnv2 = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
		AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxTrigger_ForEnv2);
	}
}

void Client::CWindDeer::InitFSMComponent()
{
	CRidingObject::InitFSMComponent();

	// FSM Component Node 채워넣기
	m_spFSMComponent->AddState((int32)ERidingObjectState::IDLE, CFSMNode_WindDeerIdle::Create(m_spMeshComponent, static_pointer_cast<CWindDeer>(shared_from_this())));
	m_spFSMComponent->AddState((int32)ERidingObjectState::MOVE, CFSMNode_WindDeerMove::Create(m_spMeshComponent, static_pointer_cast<CWindDeer>(shared_from_this())));
	m_spFSMComponent->AddState((int32)ERidingObjectState::JUMP, CFSMNode_WindDeerJump::Create(m_spMeshComponent, static_pointer_cast<CWindDeer>(shared_from_this())));
}

void Client::CWindDeer::InitRidingParticles()
{
	// todo : 이펙트를 붙여야합니다.
}

void Client::CWindDeer::InitRidingTrails()
{
	for (uint32 iInnerIndex = 0; iInnerIndex < 4; ++iInnerIndex)
	{
		FTrailComponentDesc tTrailComponentDesc;
		tTrailComponentDesc.m_wstrComponentName = L"TrailComponent" + to_wstring(iInnerIndex);
		tTrailComponentDesc.m_eRenderType		= ERenderType::RENDER_GLOW;
		tTrailComponentDesc.m_eComponentType	= EComponentType::PARTICLE;
		tTrailComponentDesc.m_wpOwnerGameObject = shared_from_this();

		tTrailComponentDesc.m_iNumRects			= 10;
		tTrailComponentDesc.m_fMaxLifeTime		= 0.001f;
		tTrailComponentDesc.m_vColor			= Color(0.74f, 0.90f, 1.0f, 1.f);

		tTrailComponentDesc.m_wstrEffectShaderName = L"FX_Trail_PT2";

		tTrailComponentDesc.m_bUseDiffuse		= false;
		tTrailComponentDesc.m_bUseNoise			= false;
		tTrailComponentDesc.m_bUseMask			= true;
		tTrailComponentDesc.m_bUseDistortion	= false;
		tTrailComponentDesc.m_bUseDissolve		= false;

		tTrailComponentDesc.m_wstrDiffuseTextureName	= L"";
		tTrailComponentDesc.m_wstrNoiseTextureName		= L"";
		tTrailComponentDesc.m_wstrMaskTextureName		= L"T_Trail07";
		tTrailComponentDesc.m_wstrDistortionTextureName = L"";
		tTrailComponentDesc.m_wstrDissolveTextureName	= L"";

		shared_ptr<CTrailComponent> spTrailComponent = CTrailComponent::Create(make_shared<FTrailComponentDesc>(tTrailComponentDesc));
		CHECKF(spTrailComponent, L"Failed to Create TrailComponent : CWindDeer");

		m_arrRidingTrails[iInnerIndex] = spTrailComponent;
		
		AddComponent(tTrailComponentDesc.m_wstrComponentName, spTrailComponent);
	}
}

void Client::CWindDeer::InitCombinedBones()
{
	m_arrCombinedBones[0] = m_spMeshComponent->GetBonePtr(L"cloud_front_foot_01_l");
	m_arrCombinedBones[1] = m_spMeshComponent->GetBonePtr(L"cloud_front_foot_02_r");
	m_arrCombinedBones[2] = m_spMeshComponent->GetBonePtr(L"cloud_back_foot_01_l");
	m_arrCombinedBones[3] = m_spMeshComponent->GetBonePtr(L"cloud_back_foot_02_r");
}

void Client::CWindDeer::Jump()
{
	m_wpEffectManager.lock()->ActivateEffect(L"Group_WindDeer_Jump", GetPosition().value());

	m_spSphereCollider->AddForce(Vector3(0.f, 1.f, 0.f), m_tRidingObjectData.m_fJumpPower);
}

void Client::CWindDeer::TickTrail(float _fTrailSize)
{
	for (uint32 i = 0; i < 4; ++i)
	{
		// Matrix matCombinedOffset = Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
		Matrix matCombinedBone = m_arrCombinedBones[i].lock()->GetCombinedTransformationMatrix();
		Matrix matCombinedObject = GetTransformMatrix().value();

		Matrix matFinal = /*matCombinedOffset * */matCombinedBone * matCombinedObject;
		Vector3 vFinalPosition = matFinal.Translation();

		Vector3 vTrailStartPos = vFinalPosition;
		vTrailStartPos.y -= _fTrailSize;

		Vector3 vTrailEndPos = vFinalPosition;
		vTrailEndPos.y += _fTrailSize;

		m_arrRidingTrails[i]->TickTrail(vTrailStartPos, vTrailEndPos);
	}
}

void Client::CWindDeer::OnRidingEffect()
{
	for (uint32 i = 0; i < 4; ++i)
	{
		// Matrix matCombinedOffset = Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
		Matrix matCombinedBone = m_arrCombinedBones[i].lock()->GetCombinedTransformationMatrix();
		Matrix matCombinedObject = GetTransformMatrix().value();

		Matrix matFinal = /*matCombinedOffset * */matCombinedBone * matCombinedObject;
		Vector3 vFinalPosition = matFinal.Translation();

		Vector3 vTrailStartPos = vFinalPosition;
		vTrailStartPos.y -= 0.05f;

		Vector3 vTrailEndPos = vFinalPosition;
		vTrailEndPos.y += 0.05f;

		m_arrRidingTrails[i]->ClearRectList(vTrailStartPos, vTrailEndPos);

		m_bActivateTrail = true;
	}
}

void Client::CWindDeer::OffRidingEffect()
{
	for (uint32 i = 0; i < 4; ++i)
	{
		// Matrix matCombinedOffset = Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
		Matrix matCombinedBone = m_arrCombinedBones[i].lock()->GetCombinedTransformationMatrix();
		Matrix matCombinedObject = GetTransformMatrix().value();

		Matrix matFinal = /*matCombinedOffset * */matCombinedBone * matCombinedObject;
		Vector3 vFinalPosition = matFinal.Translation();

		Vector3 vTrailStartPos = vFinalPosition;
		vTrailStartPos.y -= 0.05f;

		Vector3 vTrailEndPos = vFinalPosition;
		vTrailEndPos.y += 0.05f;

		m_arrRidingTrails[i]->ClearRectList(vTrailStartPos, vTrailEndPos);

		m_bActivateTrail = false;
	}
}
