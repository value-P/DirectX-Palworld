#include "pch.h"
#include "ParticleObject.h"

#include "ParticleComponent.h"

shared_ptr<CParticleObject> Client::CParticleObject::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, FParticleInfo _tParticleInfo)
{
	shared_ptr<CParticleObject> spInstance = make_shared<CParticleObject>();
	CHECKF(spInstance, L"CParticleObject : Create Failed");

	spInstance->SetObjectParticleInfo(_tParticleInfo);
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

shared_ptr<CParticleObject> Client::CParticleObject::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CParticleObject> spInstance = make_shared<CParticleObject>();
	CHECKF(spInstance, L"CParticleObject : Create from json failed");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT Client::CParticleObject::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CEffectBase::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	/* component */
	FParticleComponentDesc tParticleDesc;

	tParticleDesc.m_wpOwnerGameObject = shared_from_this();
	tParticleDesc.m_eComponentType = EComponentType::PARTICLE;
	tParticleDesc.m_wstrComponentName = L"Particle Component";
	tParticleDesc.m_eRenderType = m_tParticleInfo.m_eRenderType;

	tParticleDesc.m_eEffectType = EEffectType::PARTICLE;

	m_spParticle = CParticleComponent::Create(make_shared<FParticleComponentDesc>(tParticleDesc), m_tParticleInfo);
	AddComponent(L"Particle Component", m_spParticle);

	return S_OK;
}

HRESULT Client::CParticleObject::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectBase::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	/* 파티클 컴포넌트 */
	FParticleComponentDesc tParticleDesc;

	tParticleDesc.m_wpOwnerGameObject = shared_from_this();
	tParticleDesc.m_eComponentType = EComponentType::PARTICLE;
	tParticleDesc.m_wstrComponentName = L"Particle Component";
	tParticleDesc.m_eRenderType = m_tParticleInfo.m_eRenderType;

	tParticleDesc.m_eEffectType = EEffectType::PARTICLE;

	m_spParticle = CParticleComponent::CreateFromJson(make_shared<FParticleComponentDesc>(tParticleDesc), _wstrJsonFileName);
	AddComponent(L"Particle Component", m_spParticle);

	return S_OK;
}

HRESULT Client::CParticleObject::BeginPlay()
{
	if (FAILED(CEffectBase::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CParticleObject::PreTick(float _fDeltaSeconds)
{
	switch (CEffectBase::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 Client::CParticleObject::Tick(float _fDeltaSeconds)
{
	switch (CEffectBase::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 Client::CParticleObject::PostTick(float _fDeltaSeconds)
{
	switch (CEffectBase::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

HRESULT Client::CParticleObject::EndPlay()
{
	if (FAILED(CEffectBase::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CParticleObject::Release()
{
	CEffectBase::Release();
}

void Client::CParticleObject::RecycleFromPool()
{
}

void Client::CParticleObject::NoneActivate()
{
}

void Client::CParticleObject::EndLifeTime()
{
}

void Client::CParticleObject::Loop()
{
}

uint32 Client::CParticleObject::GetCurKeyFrameIndex()
{
	return m_spParticle->GetCurKeyFrameIndex();
}

vector<FParticleKeyFrame>& Client::CParticleObject::GetKeyFrames()
{
	return m_spParticle->GetKeyFrames();
}