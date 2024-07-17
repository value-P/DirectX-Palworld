// default
#include "pch.h"
#include "MathUtility.h"
#include "PalSummonParticle.h"

// manager
#include "EffectManager.h"

// gameobject
#include "ParticleObject.h"

// component
#include "ParticleComponent.h"
#include "Model.h"
#include "Bone.h"

shared_ptr<CPalSummonParticle> Client::CPalSummonParticle::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CPalSummonParticle> spInstance = make_shared<CPalSummonParticle>();
	CHECKF(spInstance, L"Failed to Create : CPalSummonParticle");

	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}
shared_ptr<CPalSummonParticle> Client::CPalSummonParticle::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CPalSummonParticle> spInstance = make_shared<CPalSummonParticle>();
	CHECKF(spInstance, L"Failed to CreateFromJson : CPalSummonParticle");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT Client::CPalSummonParticle::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CEffectGroup::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// gameobject
	shared_ptr<FPalSummonParticleDesc> spPalSummonParticleDesc = static_pointer_cast<FPalSummonParticleDesc>(_spGameObjectDesc);

	return S_OK;
}

HRESULT Client::CPalSummonParticle::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectGroup::InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	return S_OK;
}

HRESULT Client::CPalSummonParticle::BeginPlay()
{
	if (FAILED(CEffectGroup::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CPalSummonParticle::PreTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPalSummonParticle::Tick(float _fDeltaSeconds)
{
	switch (CEffectGroup::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPalSummonParticle::PostTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CPalSummonParticle::EndPlay()
{
	if (FAILED(CEffectGroup::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CPalSummonParticle::Release()
{
	CEffectGroup::Release();
}

void Client::CPalSummonParticle::ResetAtBonePosition(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel)
{
	SetActive(true);
	SetCulled(false);
	m_fTrackPosition = 0.f;

	for (auto& pair : m_mapEffects)
	{
		shared_ptr<CParticleComponent> spParticleCom = static_pointer_cast<CParticleObject>(pair.second)->GetParticleCom();
		CHECKF(spParticleCom, L"Failed to ResetAtBonePosition : CPalSummonParticle");

		spParticleCom->ResetInstanceBufferByBonePosition(_spGameObject, _spModel);
	}
}
