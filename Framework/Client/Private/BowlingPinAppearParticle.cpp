// default
#include "pch.h"
#include "MathUtility.h"
#include "BowlingPinAppearParticle.h"

// manager
#include "EffectManager.h"

// gameobject
#include "ParticleObject.h"

// component
#include "ParticleComponent.h"
#include "Model.h"
#include "Bone.h"

shared_ptr<CBowlingPinAppearParticle> Client::CBowlingPinAppearParticle::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CBowlingPinAppearParticle> spInstance = make_shared<CBowlingPinAppearParticle>();
	CHECKF(spInstance, L"Failed to Create : CBowlingPinAppearParticle");

	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}
shared_ptr<CBowlingPinAppearParticle> Client::CBowlingPinAppearParticle::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CBowlingPinAppearParticle> spInstance = make_shared<CBowlingPinAppearParticle>();
	CHECKF(spInstance, L"Failed to CreateFromJson : CBowlingPinAppearParticle");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT Client::CBowlingPinAppearParticle::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CEffectGroup::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	// gameobject
	shared_ptr<FBowlingPinAppearParticleDesc> spBowlingPinAppearParticleDesc = static_pointer_cast<FBowlingPinAppearParticleDesc>(_spGameObjectDesc);

	return S_OK;
}

HRESULT Client::CBowlingPinAppearParticle::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectGroup::InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	return S_OK;
}

HRESULT Client::CBowlingPinAppearParticle::BeginPlay()
{
	if (FAILED(CEffectGroup::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CBowlingPinAppearParticle::PreTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CBowlingPinAppearParticle::Tick(float _fDeltaSeconds)
{
	switch (CEffectGroup::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CBowlingPinAppearParticle::PostTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CBowlingPinAppearParticle::EndPlay()
{
	if (FAILED(CEffectGroup::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CBowlingPinAppearParticle::Release()
{
	CEffectGroup::Release();
}

void Client::CBowlingPinAppearParticle::ResetAtBonePosition(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel)
{
	SetActive(true);
	SetCulled(false);
	m_fTrackPosition = 0.f;

	for (auto& pair : m_mapEffects)
	{
		shared_ptr<CParticleComponent> spParticleCom = static_pointer_cast<CParticleObject>(pair.second)->GetParticleCom();
		CHECKF(spParticleCom, L"Failed to ResetAtBonePosition : CBowlingPinAppearParticle");

		spParticleCom->ResetInstanceBufferByBonePosition(_spGameObject, _spModel);
	}
}
