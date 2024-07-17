#include "pch.h"
#include "TriggerObject.h"

#include "SphereComponent.h"

shared_ptr<CTriggerObject> Client::CTriggerObject::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CTriggerObject> spInstance = make_shared<CTriggerObject>();
	ENSUREF(spInstance->Initialize(_spGameObjectDesc), L"Failed to initialize CTriggerObject.");
	return spInstance;
}

HRESULT Client::CTriggerObject::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; };

	// component desc
	FSphereComponentDesc tSphereComponentDesc;
	tSphereComponentDesc.m_eComponentType = EComponentType::SPHERE;
	tSphereComponentDesc.m_wpOwnerGameObject = shared_from_this();
	tSphereComponentDesc.m_wstrComponentName = L"TriggerComponent";
	tSphereComponentDesc.m_eRenderType = ERenderType::RENDER_DEBUG;

	tSphereComponentDesc.m_eMobilityType = EMobilityType::DYNAMIC;
	tSphereComponentDesc.m_eColliderType = EColliderType::SPHERE;
	tSphereComponentDesc.m_eCollisionLayer = ECollisionLayer::PLAYER_WEAPON;
	tSphereComponentDesc.m_eCollisionFlag = ECollisionFlag::PLAYER_WEAPON;

	tSphereComponentDesc.m_fDensity = 1.0f;
	tSphereComponentDesc.m_fMass = 0.0f;
	tSphereComponentDesc.m_bGravity = false;
	tSphereComponentDesc.m_bTrigger = true;
	tSphereComponentDesc.m_bEnableSimulation = false;

	m_spSphereTrigger = CSphereComponent::Create(make_shared<FSphereComponentDesc>(tSphereComponentDesc));
	return AddComponent(tSphereComponentDesc.m_wstrComponentName, m_spSphereTrigger);
}

HRESULT Client::CTriggerObject::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; };

	return S_OK;
}

int32 Client::CTriggerObject::Tick(float _fDeltaSeconds)
{
	CGameObject::Tick(_fDeltaSeconds);

	m_fDisableTimeChecker += _fDeltaSeconds;

	if (m_fDisableTimeChecker >= m_fDisableTime)
	{
		m_fDisableTimeChecker = 0.f;
		SetActive(false);
	}

	return 0;
}

HRESULT Client::CTriggerObject::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CTriggerObject::Release()
{
	if (m_spSphereTrigger) { m_spSphereTrigger = nullptr; }

	CGameObject::Release();
}

void Client::CTriggerObject::OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	m_wpNotifyClient.lock()->OnTriggerEnter(_pSrcComponent, _pDstComponent);
}

void Client::CTriggerObject::OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent)
{
	m_wpNotifyClient.lock()->OnTriggerExit(_pSrcComponent, _pDstComponent);
}

void Client::CTriggerObject::SetPosition(const Vector3& _vPosition)
{
	m_spSphereTrigger->SetWorldPosition(_vPosition);
}

void Client::CTriggerObject::ActivateTrigger(shared_ptr<CGameObject> _spClientObject, float _fDisableTime, float _fRadius, ECollisionFlag _eCollisionFlag, ECollisionLayer _eCollisionLayer)
{
	m_wpNotifyClient = _spClientObject;
	m_fDisableTime = _fDisableTime;

	m_spSphereTrigger->SetCollisionLayer(_eCollisionLayer, _eCollisionFlag);
	m_spSphereTrigger->SetRadius(_fRadius);
	m_spSphereTrigger->RequestSimulation(true);
}
