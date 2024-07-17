#include "pch.h"
#include "ParticleObject.h"

#include "ParticleComponent.h"

shared_ptr<CParticleObject> ToolAnimation::CParticleObject::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, FParticleInfo _tParticleInfo)
{
	shared_ptr<CParticleObject> spInstance = make_shared<CParticleObject>();
	CHECKF(spInstance, L"CParticleObject : Create Failed");

	spInstance->SetObjectParticleInfo(_tParticleInfo);
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

shared_ptr<CParticleObject> ToolAnimation::CParticleObject::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CParticleObject> spInstance = make_shared<CParticleObject>();
	CHECKF(spInstance, L"CParticleObject : Create from json failed");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT ToolAnimation::CParticleObject::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
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

HRESULT ToolAnimation::CParticleObject::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectBase::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	/* ��ƼŬ ������Ʈ */
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

HRESULT ToolAnimation::CParticleObject::BeginPlay()
{
	if (FAILED(CEffectBase::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 ToolAnimation::CParticleObject::PreTick(float _fDeltaSeconds)
{
	switch (CEffectBase::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 ToolAnimation::CParticleObject::Tick(float _fDeltaSeconds)
{
	switch (CEffectBase::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 ToolAnimation::CParticleObject::PostTick(float _fDeltaSeconds)
{
	switch (CEffectBase::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

HRESULT ToolAnimation::CParticleObject::EndPlay()
{
	if (FAILED(CEffectBase::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void ToolAnimation::CParticleObject::Release()
{
	CEffectBase::Release();
}

void ToolAnimation::CParticleObject::RecycleFromPool()
{
}

void ToolAnimation::CParticleObject::NoneActivate()
{
}

void ToolAnimation::CParticleObject::EndLifeTime()
{
}

void ToolAnimation::CParticleObject::Loop()
{
}

uint32 ToolAnimation::CParticleObject::GetCurKeyFrameIndex()
{
	return m_spParticle->GetCurKeyFrameIndex();
}

vector<FParticleKeyFrame>& ToolAnimation::CParticleObject::GetKeyFrames()
{
	return m_spParticle->GetKeyFrames();
}