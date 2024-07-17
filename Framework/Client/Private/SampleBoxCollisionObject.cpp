#include "pch.h"
#include "SampleBoxCollisionObject.h"

#include "BoxComponent.h"

std::shared_ptr<CSampleBoxCollisionObject> Client::CSampleBoxCollisionObject::Create(const std::shared_ptr<FSampleBoxCollisionObjectDesc>& _spSampleBoxCollisionObjectDesc)
{
	const std::shared_ptr<CSampleBoxCollisionObject> spSampleBoxCollisionObject = make_shared<CSampleBoxCollisionObject>();
	ENSUREF(spSampleBoxCollisionObject->Initialize(_spSampleBoxCollisionObjectDesc), L"Failed to initialize sample box collision object");
	return spSampleBoxCollisionObject;
}

HRESULT Client::CSampleBoxCollisionObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// gameobject desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// sample box collision object desc
	shared_ptr<FSampleBoxCollisionObjectDesc> spSampleBoxCollisionObjectDesc = static_pointer_cast<FSampleBoxCollisionObjectDesc>(_spGameObjectDesc);

	// component desc
	FBoxComponentDesc tBoxComponentDesc;
	tBoxComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tBoxComponentDesc.m_eComponentType		= EComponentType::BOX;
	tBoxComponentDesc.m_wstrComponentName	= L"BoxComponent";
	tBoxComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	// collider component desc
	tBoxComponentDesc.m_eMobilityType	= spSampleBoxCollisionObjectDesc->m_eMobilityType;
	tBoxComponentDesc.m_eColliderType	= EColliderType::BOX;
	tBoxComponentDesc.m_eCollisionLayer = spSampleBoxCollisionObjectDesc->m_eCollisionLayer;
	tBoxComponentDesc.m_eCollisionFlag	= spSampleBoxCollisionObjectDesc->m_eCollisionFlag;
	tBoxComponentDesc.m_fDensity		= 1.0f;
	tBoxComponentDesc.m_fMass			= 0.0f;
	tBoxComponentDesc.m_bGravity		= spSampleBoxCollisionObjectDesc->m_bGravity;
	tBoxComponentDesc.m_bTrigger		= false;
	tBoxComponentDesc.m_iAxisLockType	= 0; // (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Z;
	tBoxComponentDesc.m_vLocalPosition	= spSampleBoxCollisionObjectDesc->m_vCenter;
	tBoxComponentDesc.m_vLocalRotate	= spSampleBoxCollisionObjectDesc->m_vLocalRotate;
	tBoxComponentDesc.m_vMaterial		= Vector3(0.5f, 0.5f, 0.5f);

	// box component desc
	tBoxComponentDesc.m_vExtents = spSampleBoxCollisionObjectDesc->m_vExtents;

	m_spBoxComponent = CBoxComponent::Create(make_shared<FBoxComponentDesc>(tBoxComponentDesc));
	AddComponent(L"BoxComponent", m_spBoxComponent);
	return S_OK;
}

HRESULT Client::CSampleBoxCollisionObject::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CSampleBoxCollisionObject::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CSampleBoxCollisionObject::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CSampleBoxCollisionObject::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

HRESULT Client::CSampleBoxCollisionObject::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CSampleBoxCollisionObject::Release()
{
	m_spBoxComponent = nullptr;

	CGameObject::Release();
}
