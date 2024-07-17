// default
#include "pch.h"
#include "MathUtility.h"
#include "PalDeadParticle.h"

// manager
#include "EffectManager.h"

// gameobject
#include "ParticleObject.h"

// component
#include "ParticleComponent.h"
#include "Model.h"
#include "Bone.h"

shared_ptr<CPalDeadParticle> Client::CPalDeadParticle::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CPalDeadParticle> spInstance = make_shared<CPalDeadParticle>();
	CHECKF(spInstance, L"Failed to Create : CPalDeadParticle");

	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}
shared_ptr<CPalDeadParticle> Client::CPalDeadParticle::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CPalDeadParticle> spInstance = make_shared<CPalDeadParticle>();
	CHECKF(spInstance, L"Failed to CreateFromJson : CPalDeadParticle");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT Client::CPalDeadParticle::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CEffectGroup::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// gameobject
	shared_ptr<FPalDeadParticleDesc> spPalDeadParticleDesc = static_pointer_cast<FPalDeadParticleDesc>(_spGameObjectDesc);

	return S_OK;
}

HRESULT Client::CPalDeadParticle::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectGroup::InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	return S_OK;
}

HRESULT Client::CPalDeadParticle::BeginPlay()
{
	if (FAILED(CEffectGroup::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CPalDeadParticle::PreTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPalDeadParticle::Tick(float _fDeltaSeconds)
{
	switch (CEffectGroup::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CPalDeadParticle::PostTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CPalDeadParticle::EndPlay()
{
	if (FAILED(CEffectGroup::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CPalDeadParticle::Release()
{
	CEffectGroup::Release();
}

void Client::CPalDeadParticle::ResetAtBonePosition(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel)
{
	SetActive(true);
	SetCulled(false);
	m_fTrackPosition = 0.f;

	for (auto& pair : m_mapEffects)
	{
		shared_ptr<CParticleComponent> spParticleCom = static_pointer_cast<CParticleObject>(pair.second)->GetParticleCom();
		CHECKF(spParticleCom, L"Failed to ResetAtBonePosition : CPalDeadParticle");

		spParticleCom->ResetInstanceBufferByBonePosition(_spGameObject, _spModel);
	}
}
