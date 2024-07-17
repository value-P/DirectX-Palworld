#include "pch.h"
#include "SampleBoxTriggerObject.h"

#include "BoxComponent.h"

#include "EngineModule.h"
#include "InputManager.h"

std::shared_ptr<CSampleBoxTriggerObject> Client::CSampleBoxTriggerObject::Create(const std::shared_ptr<FSampleBoxTriggerObjectDesc>& _spSampleBoxTriggerObjectDesc)
{
	const std::shared_ptr<CSampleBoxTriggerObject> spSampleBoxTriggerObject = make_shared<CSampleBoxTriggerObject>();
	ENSUREF(spSampleBoxTriggerObject->Initialize(_spSampleBoxTriggerObjectDesc), L"Failed to initialize sample box trigger object.");
	return spSampleBoxTriggerObject;
}

HRESULT Client::CSampleBoxTriggerObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// gameobject desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// sample box trigger object desc
	shared_ptr<FSampleBoxTriggerObjectDesc> spSampleBoxTriggerObjectDesc = static_pointer_cast<FSampleBoxTriggerObjectDesc>(_spGameObjectDesc);

	// component desc
	FBoxComponentDesc tBoxComponentDesc;
	tBoxComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tBoxComponentDesc.m_eComponentType = EComponentType::BOX;
	tBoxComponentDesc.m_wstrComponentName = L"BoxComponent";
	tBoxComponentDesc.m_eRenderType = ERenderType::NONE;

	// collider component desc
	tBoxComponentDesc.m_eMobilityType = spSampleBoxTriggerObjectDesc->m_eMobilityType;
	tBoxComponentDesc.m_eColliderType = EColliderType::BOX;
	tBoxComponentDesc.m_eCollisionLayer = spSampleBoxTriggerObjectDesc->m_eCollisionLayer;
	tBoxComponentDesc.m_eCollisionFlag = spSampleBoxTriggerObjectDesc->m_eCollisionFlag;
	tBoxComponentDesc.m_fDensity = 1.0f;
	tBoxComponentDesc.m_fMass = 0.0f;
	tBoxComponentDesc.m_bGravity = false;
	tBoxComponentDesc.m_bTrigger = spSampleBoxTriggerObjectDesc->m_bTrigger;
	tBoxComponentDesc.m_iAxisLockType = (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Z;
	tBoxComponentDesc.m_vLocalPosition = Vector3(0.0f, 0.0f, 0.0f);
	tBoxComponentDesc.m_vMaterial = Vector3(1.0f, 1.0f, 1.0f);

	// box component desc
	tBoxComponentDesc.m_vExtents = spSampleBoxTriggerObjectDesc->m_vExtents;

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	AddComponent(L"BoxComponent", m_spBoxComponent);
	return S_OK;
}

HRESULT Client::CSampleBoxTriggerObject::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }



	return S_OK;
}

int32 Client::CSampleBoxTriggerObject::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	/*
	shared_ptr<CInputManager> spInputManager = CEngineModule::GetInstance()->GetInputManager();
	if (spInputManager->IsKeyPressed(DIK_0))
	{
		MoveDown(1000.0f * _fDeltaSeconds);
	}
	*/
	return 0;
}

int32 Client::CSampleBoxTriggerObject::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CSampleBoxTriggerObject::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

HRESULT Client::CSampleBoxTriggerObject::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CSampleBoxTriggerObject::Release()
{
	m_spBoxComponent = nullptr;
	CGameObject::Release();
}
