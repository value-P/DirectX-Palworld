// default
#include "pch.h"
#include "MathUtility.h"
#include "BuildStartParticle.h"

// manager
#include "EffectManager.h"

// gameobject
#include "ParticleObject.h"

// component
#include "ParticleComponent.h"
#include "Model.h"
#include "Bone.h"

shared_ptr<CBuildStartParticle> Client::CBuildStartParticle::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CBuildStartParticle> spInstance = make_shared<CBuildStartParticle>();
	CHECKF(spInstance, L"Failed to Create : CBuildStartParticle");

	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}
shared_ptr<CBuildStartParticle> Client::CBuildStartParticle::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CBuildStartParticle> spInstance = make_shared<CBuildStartParticle>();
	CHECKF(spInstance, L"Failed to CreateFromJson : CBuildStartParticle");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT Client::CBuildStartParticle::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CEffectGroup::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// gameobject
	shared_ptr<FBuildStartParticleDesc> spBuildStartParticleDesc = static_pointer_cast<FBuildStartParticleDesc>(_spGameObjectDesc);

	return S_OK;
}

HRESULT Client::CBuildStartParticle::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectGroup::InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	return S_OK;
}

HRESULT Client::CBuildStartParticle::BeginPlay()
{
	if (FAILED(CEffectGroup::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CBuildStartParticle::PreTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CBuildStartParticle::Tick(float _fDeltaSeconds)
{
	switch (CEffectGroup::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CBuildStartParticle::PostTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CBuildStartParticle::EndPlay()
{
	if (FAILED(CEffectGroup::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CBuildStartParticle::Release()
{
	CEffectGroup::Release();
}

void Client::CBuildStartParticle::ResetAtBonePosition(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel)
{
	SetActive(true);
	SetCulled(false);
	m_fTrackPosition = 0.f;

	for (auto& pair : m_mapEffects)
	{
		shared_ptr<CParticleComponent> spParticleCom = static_pointer_cast<CParticleObject>(pair.second)->GetParticleCom();
		CHECKF(spParticleCom, L"Failed to ResetAtBonePosition : CBuildStartParticle");

		spParticleCom->ResetInstanceBufferByBonePosition(_spGameObject, _spModel);
	}
}
