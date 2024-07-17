#include "pch.h"
#include "SampleCapsuleCollisionObject.h"

#include "CapsuleComponent.h"

std::shared_ptr<CSampleCapsuleCollisionObject> Client::CSampleCapsuleCollisionObject::Create(const std::shared_ptr<FSampleCapsuleCollisionObjectDesc>& _spSampleCapsuleCollisionObjectDesc)
{
	const std::shared_ptr<CSampleCapsuleCollisionObject> spSampleCapsuleCollisionObject = make_shared<CSampleCapsuleCollisionObject>();
	ENSUREF(spSampleCapsuleCollisionObject->Initialize(_spSampleCapsuleCollisionObjectDesc), L"Failed to initialize sample capsule collision object");
	return spSampleCapsuleCollisionObject;
}

HRESULT Client::CSampleCapsuleCollisionObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// gameobject desc
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// sample box collision object desc
	shared_ptr<FSampleCapsuleCollisionObjectDesc> spSampleCapsuleCollisionObjectDesc = static_pointer_cast<FSampleCapsuleCollisionObjectDesc>(_spGameObjectDesc);

	// component desc
	FCapsuleComponentDesc tCapsuleComponentDesc;
	tCapsuleComponentDesc.m_wpOwnerGameObject	= shared_from_this();
	tCapsuleComponentDesc.m_eComponentType		= EComponentType::CAPSULE;
	tCapsuleComponentDesc.m_wstrComponentName	= L"CapsuleComponent";
	tCapsuleComponentDesc.m_eRenderType			= ERenderType::RENDER_DEBUG;

	// collider component desc
	tCapsuleComponentDesc.m_eMobilityType		= spSampleCapsuleCollisionObjectDesc->m_eMobilityType;
	tCapsuleComponentDesc.m_eColliderType		= EColliderType::CAPSULE;
	tCapsuleComponentDesc.m_eCollisionLayer		= spSampleCapsuleCollisionObjectDesc->m_eCollisionLayer;
	tCapsuleComponentDesc.m_eCollisionFlag		= spSampleCapsuleCollisionObjectDesc->m_eCollisionFlag;
	tCapsuleComponentDesc.m_fDensity			= 1.0f;
	tCapsuleComponentDesc.m_fMass				= 0.0f;
	tCapsuleComponentDesc.m_bGravity			= spSampleCapsuleCollisionObjectDesc->m_bGravity;
	tCapsuleComponentDesc.m_bTrigger			= false;
	tCapsuleComponentDesc.m_iAxisLockType		= 0; // (int32)EAxisLockType::LOCK_ANGULAR_X | (int32)EAxisLockType::LOCK_ANGULAR_Z;
	tCapsuleComponentDesc.m_vLocalPosition		= spSampleCapsuleCollisionObjectDesc->m_vCenter;
	tCapsuleComponentDesc.m_vLocalRotate		= spSampleCapsuleCollisionObjectDesc->m_vLocalRotate;
	tCapsuleComponentDesc.m_vMaterial			= Vector3(0.5f, 0.5f, 0.5f);

	// capsule component desc
	tCapsuleComponentDesc.m_fRadius				= spSampleCapsuleCollisionObjectDesc->m_fRadius;
	tCapsuleComponentDesc.m_fHalfHeight			= spSampleCapsuleCollisionObjectDesc->m_fHalfHeight;

	m_spCapsuleComponent = CCapsuleComponent::Create(make_shared<FCapsuleComponentDesc>(tCapsuleComponentDesc));
	AddComponent(L"CapsuleComponent", m_spCapsuleComponent);
	return S_OK;
}

HRESULT Client::CSampleCapsuleCollisionObject::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CSampleCapsuleCollisionObject::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CSampleCapsuleCollisionObject::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

int32 Client::CSampleCapsuleCollisionObject::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

HRESULT Client::CSampleCapsuleCollisionObject::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CSampleCapsuleCollisionObject::Release()
{
	m_spCapsuleComponent = nullptr;

	CGameObject::Release();
}
