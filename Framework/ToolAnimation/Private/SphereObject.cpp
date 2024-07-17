#include "pch.h"
#include "SphereObject.h"
#include "SphereComponent.h"

std::shared_ptr<CSphereObject> ToolAnimation::CSphereObject::Create(const std::shared_ptr<FGameObjectDesc>& _spPlayerDesc)
{
	shared_ptr<CSphereObject> spInstance = make_shared<CSphereObject>();
	ENSUREF(spInstance->Initialize(_spPlayerDesc), L"Failed to initialize CSphereObject.");
	return spInstance;
}

HRESULT ToolAnimation::CSphereObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CGameObject::Initialize(_spGameObjectDesc);

	FSphereComponentDesc tSphereDesc;
	tSphereDesc.m_bTrigger = true;
	tSphereDesc.m_bGravity = false;
	tSphereDesc.m_eColliderType = EColliderType::SPHERE;
	tSphereDesc.m_eComponentType = EComponentType::SPHERE;
	tSphereDesc.m_eMobilityType = EMobilityType::DYNAMIC;
	tSphereDesc.m_eRenderType = ERenderType::RENDER_DEBUG;
	tSphereDesc.m_eCollisionFlag = ECollisionFlag::ENVIRONMENT;
	tSphereDesc.m_eCollisionLayer= ECollisionLayer::ENVIRONMENT;
	tSphereDesc.m_fDensity = 1.0f;
	tSphereDesc.m_fMass = 0.0f;
	tSphereDesc.m_fRadius = 0.1f;
	tSphereDesc.m_wstrComponentName = L"Sphere";
	tSphereDesc.m_wpOwnerGameObject = shared_from_this();

	m_spSphereComponent = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereDesc));
	AddComponent(tSphereDesc.m_wstrComponentName, m_spSphereComponent);

	return S_OK;
}

HRESULT ToolAnimation::CSphereObject::BeginPlay()
{
	return CGameObject::BeginPlay();
}

int32 ToolAnimation::CSphereObject::PreTick(float _fDeltaSeconds)
{
	return CGameObject::PreTick(_fDeltaSeconds);
}

int32 ToolAnimation::CSphereObject::Tick(float _fDeltaSeconds)
{
	return CGameObject::Tick(_fDeltaSeconds);
}

int32 ToolAnimation::CSphereObject::PostTick(float _fDeltaSeconds)
{
	return CGameObject::PostTick(_fDeltaSeconds);
}

HRESULT ToolAnimation::CSphereObject::EndPlay()
{
	return CGameObject::EndPlay();
}

void ToolAnimation::CSphereObject::Release()
{
	CGameObject::Release();

	if (m_spSphereComponent)
	{
		m_spSphereComponent = nullptr;
	}
}

void ToolAnimation::CSphereObject::SetRadius(float _fRadius)
{
	m_spSphereComponent->SetRadius(_fRadius);
}

void ToolAnimation::CSphereObject::SetLocalPos(Vector3 _vLocalPos)
{
	m_spSphereComponent->SetLocalPosition(_vLocalPos);
}

float ToolAnimation::CSphereObject::GetRadius()
{
	return m_spSphereComponent->GetRadius();
}

Vector3 ToolAnimation::CSphereObject::GetLocalPos()
{
	return m_spSphereComponent->GetLocalPosition();
}
