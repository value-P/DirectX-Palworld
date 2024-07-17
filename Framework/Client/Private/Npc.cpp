#include "pch.h"
#include "Npc.h"

#include "EngineModule.h"
#include "GameManager.h"
#include "QuestSystem.h"
#include "InventorySystem.h"
#include "ClientModule.h"
#include "UIManager.h"
#include "SoundManager.h"

#include "ColliderComponent.h"
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "TextComponent.h"
#include "Panel.h"

#include "Player.h"

HRESULT Client::CNpc::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// gameobject desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// npc desc
	shared_ptr<FNpcDesc> spDesc = static_pointer_cast<FNpcDesc>(_spGameObjectDesc);
	m_wstrNpcName = spDesc->m_wstrNpcName;
	m_vInitialLook = spDesc->m_vInitialLook;

	LookAt(GetPosition().value() + m_vInitialLook);

	// etc
	m_wpUIManager = CClientModule::GetInstance()->GetUIManager();
	m_wpQuestSystem = CGameManager::GetInstance()->GetQuestSystem();
	m_wpInventorySystem = CGameManager::GetInstance()->GetInventorySystem();
	m_wpSoundManager = CEngineModule::GetInstance()->GetSoundManager();
	
	m_vDstLook = GetForwardVector().value();

	m_wpRidingUI = m_wpUIManager.lock()->FindParentsPanel(L"RidingUI");
	m_wpPlayer = CGameManager::GetInstance()->GetPlayer();

	InitMeshComponent(spDesc);
	InitColliderBox();
	InitCycle();

	return S_OK;
}

HRESULT Client::CNpc::BeginPlay()
{
	CGameObject::BeginPlay();

	m_wpDialogPanel = m_wpUIManager.lock()->FindParentsPanel(L"DialogueUI");

	m_wpNameTextComponent = m_wpDialogPanel.lock()->FindComponent<CTextComponent>(L"NameText");
	m_wpDialogTextComponent = m_wpDialogPanel.lock()->FindComponent<CTextComponent>(L"DialogueText");
	m_wpDialogPanel.lock()->SetActive(false);

	return 0;
}

int32 Client::CNpc::PreTick(float _fDeltaSeconds)
{
	CGameObject::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CNpc::Tick(float _fDeltaSeconds)
{
	CGameObject::Tick(_fDeltaSeconds);

	CoumputeLook();

	ComputeAnimation();

	return 0;
}

int32 Client::CNpc::PostTick(float _fDeltaSeconds)
{
	CGameObject::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CNpc::EndPlay()
{
	CGameObject::EndPlay();

	return S_OK;
}

void Client::CNpc::Release()
{
	SharedNull(m_spMeshComponent);
	SharedNull(m_spBoxCollider);

	CGameObject::Release();
}

void Client::CNpc::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 컨트롤러 컴포넌트
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER) { return; }

	CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	if (pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::NPC &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::INTERACTION)
	{
		m_wpInteractObject = pDstColliderComponent->GetOwnerGameObject();
	}
}

void Client::CNpc::OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	// 컨트롤러 컴포넌트
	if (_pDstComponent->GetComponentType() == EComponentType::CONTROLLER) { return; }

	CColliderComponent* pSrcColliderComponent = dynamic_cast<CColliderComponent*>(_pSrcComponent);
	CColliderComponent* pDstColliderComponent = dynamic_cast<CColliderComponent*>(_pDstComponent);

	if ( pSrcColliderComponent->GetCollisionLayer() == ECollisionLayer::NPC &&
		pDstColliderComponent->GetCollisionLayer() == ECollisionLayer::INTERACTION)
	{
		if (!m_wpInteractObject.expired()) { m_wpInteractObject.reset(); }
	}

}

void Client::CNpc::InitMeshComponent(const std::shared_ptr<FNpcDesc>& _spGameObjectDesc)
{
	// component desc
	FMeshComponentDesc tBodyMeshComponentDesc;
	tBodyMeshComponentDesc.m_wstrComponentName = L"BodyMeshComponent";
	tBodyMeshComponentDesc.m_eRenderType = ERenderType::RENDER_NBLEND;
	tBodyMeshComponentDesc.m_eComponentType = EComponentType::MESH;
	tBodyMeshComponentDesc.m_wpOwnerGameObject = shared_from_this();

	// mesh component desc
	tBodyMeshComponentDesc.m_wstrShaderName = L"FX_AnimMesh_PNT2TBIBW";
	tBodyMeshComponentDesc.m_matPivot = Matrix::CreateScale(0.0001f) * Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), XMConvertToRadians(180.f)));

	ENpcModelType eType = _spGameObjectDesc->m_eModelType;

	switch (eType)
	{
	case Client::ENpcModelType::NPC_Female_People02:
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/NPC/NPC_Female_People02.dat";
		break;
	case Client::ENpcModelType::NPC_Female_People03:
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/NPC/NPC_Female_People03.dat";
		break;
	case Client::ENpcModelType::NPC_Female_Soldier04:
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/NPC/NPC_Female_Soldier04.dat";
		break;
	case Client::ENpcModelType::NPC_Male_Believer01:
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/NPC/NPC_Male_Believer01.dat";
		break;
	case Client::ENpcModelType::NPC_Male_DesertPeople01:
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/NPC/NPC_Male_DesertPeople01.dat";
		break;
	case Client::ENpcModelType::NPC_Male_People02:
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/NPC/NPC_Male_People02.dat";
		break;
	case Client::ENpcModelType::NPC_Male_Police00:
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/NPC/NPC_Male_Police00.dat";
		break;
	case Client::ENpcModelType::NPC_Male_Trader03:
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/NPC/NPC_Male_Trader03.dat";
		break;
	case Client::ENpcModelType::NPC_Outfit_GrassBoss001_ALL:
		tBodyMeshComponentDesc.m_wstrModelFilePath = L"../../Resource/Models/Dat/Anim/NPC/NPC_Outfit_GrassBoss001_ALL.dat";
		break;
	default:
		CHECKF(true, L"No NpcModelType");
		return;
	}

	m_spMeshComponent = CMeshComponent::Create(make_shared<FMeshComponentDesc>(tBodyMeshComponentDesc));
	AddComponent(tBodyMeshComponentDesc.m_wstrComponentName, m_spMeshComponent);

	m_spMeshComponent->ChangeAnimation((int32)m_eDefaultAnim);
}

void Client::CNpc::InitColliderBox()
{
	// component
	FBoxComponentDesc tBoxComponentDesc;
	tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
	tBoxComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
	tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tBoxComponentDesc.m_wstrComponentName = L"BoxCollider";

	// collider component desc
	tBoxComponentDesc.m_eMobilityType = EMobilityType::STATIC;
	tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
	tBoxComponentDesc.m_eCollisionFlag = ECollisionFlag::NPC;
	tBoxComponentDesc.m_eCollisionLayer = ECollisionLayer::NPC;
	tBoxComponentDesc.m_bGravity = false;
	tBoxComponentDesc.m_bTrigger = false;

	// box component desc
	tBoxComponentDesc.m_vLocalPosition = Vector3(0.f, 0.f, 0.f);
	tBoxComponentDesc.m_vExtents = Vector3(0.5f, 4.f, 0.5f);

	m_spBoxCollider = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	AddComponent(tBoxComponentDesc.m_wstrComponentName, m_spBoxCollider);
}

void Client::CNpc::Interact()
{
	// 다이얼로그가 꺼져있다면
	if (!m_bDialogActivate)
	{
		PlayDialog();

		PlayText();

		if (!m_bInteracted)
		{
			m_vDstLook = m_wpInteractObject.lock()->GetPosition().value() - GetPosition().value();
			m_vDstLook.y = 0.f;
			m_vDstLook.Normalize();
			m_bInteracted = true;
		}
	}
	else
	{
		// 마지막 대사였다면 dialog를 종료한다
		if (m_umapCycle[m_wstrCurrentCycle].IsLast()) { EndDialog(); }
		else
		{
			PlayText();
		}
	}
}

void Client::CNpc::PlayText()
{
	FNpcTextStruct tStruct = m_umapCycle[m_wstrCurrentCycle].Current().value();

	m_wpDialogTextComponent.lock()->SetText(tStruct.m_wstrText);

	if (tStruct.m_iAnimID != -1)
	{
		m_spMeshComponent->ChangeAnimation(tStruct.m_iAnimID);
	}

	if (tStruct.m_funcEvent)
	{
		tStruct.m_funcEvent();
	}

	if (!m_bCycleChanged)
	{
		m_umapCycle[m_wstrCurrentCycle].Next();
	}
	else
	{
		m_bCycleChanged = false;
	}
}

void Client::CNpc::PlayDialog()
{
	m_wpNameTextComponent.lock()->SetText(m_wstrNpcName);
	m_bDialogActivate = true;
	m_wpDialogPanel.lock()->SetActive(true);
	m_wpUIManager.lock()->SetTopPanel(m_wpDialogPanel.lock());
	m_wpUIManager.lock()->GetHUDParentPanel()->SetActive(false);
	m_wpRidingUI.lock()->SetActive(false);
}

void Client::CNpc::EndDialog()
{
	m_bDialogActivate = false;
	m_wpDialogPanel.lock()->SetActive(false);
	m_wpUIManager.lock()->ResetTopPanel();
	m_wpUIManager.lock()->GetHUDParentPanel()->SetActive(true);
	m_wpRidingUI.lock()->SetActive(true);
}

void Client::CNpc::CoumputeLook()
{
	Vector3 vPos = GetPosition().value();
	Vector3 vCurrentLook = GetForwardVector().value();

	vCurrentLook = Vector3::Lerp(vCurrentLook, m_vDstLook, 0.1f);

	LookAt(vPos + vCurrentLook);
}

void Client::CNpc::ComputeAnimation()
{
	FModelAnimData tAnimData =  m_spMeshComponent->GetModelAnimationData();

	if (tAnimData.m_tBaseAnimData.m_iCurrentAnimID != (int32)m_eDefaultAnim)
	{
		if (tAnimData.m_tBaseAnimData.m_bLoopEnd)
		{
			m_spMeshComponent->ChangeAnimation((int32)m_eDefaultAnim);
		}
	}
}

void Client::CNpc::ChangeLookToInitial()
{
	m_vDstLook = m_vInitialLook;
}

void Client::CNpc::ChangeDefaultAnimation(ENPCANIM _eNpcAnim)
{
	m_eDefaultAnim = _eNpcAnim;
}

void Client::CNpc::AddExp()
{
	m_wpPlayer.lock()->AddExp(100.f);
}

void Client::CNpc::NpcPlaySound(const string& _strSoundID)
{
	m_eCurrentChannel = m_wpSoundManager.lock()->PlaySoundEx(_strSoundID, GetPosition().value());
}

void Client::CNpc::ChangeCycle(wstring _wstrCycleName)
{
	if (!m_umapCycle.contains(_wstrCycleName)) { return; }

	EndDialog();
	m_wstrCurrentCycle = _wstrCycleName;
	m_bCycleChanged = true;
}