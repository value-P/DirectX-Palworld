// default
#include "pch.h"
#include "MathUtility.h"
#include "RedRazor.h"

// manager
#include "EffectManager.h"

// gameobject
#include "Player.h"
#include "ElecPanda.h"

shared_ptr<CRedRazor> Client::CRedRazor::Create(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CRedRazor> spInstance = make_shared<CRedRazor>();
	CHECKF(spInstance, L"Failed to Create : CRedRazor");

	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}
shared_ptr<CRedRazor> Client::CRedRazor::CreateFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	shared_ptr<CRedRazor> spInstance = make_shared<CRedRazor>();
	CHECKF(spInstance, L"Failed to CreateFromJson : CRedRazor");

	spInstance->InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName);

	return spInstance;
}

HRESULT Client::CRedRazor::Initialize(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CEffectGroup::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	return S_OK;
}

HRESULT Client::CRedRazor::InitializeFromJson(const shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const wstring& _wstrJsonFileName)
{
	if (FAILED(CEffectGroup::InitializeFromJson(_spGameObjectDesc, _wstrJsonFileName))) { return E_FAIL; }

	// manager
	m_wpEffectManager = CEffectManager::GetInstance();

	return S_OK;
}

HRESULT Client::CRedRazor::BeginPlay()
{
	if (FAILED(CEffectGroup::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CRedRazor::PreTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Client::CRedRazor::Tick(float _fDeltaSeconds)
{
	switch (CEffectGroup::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	if (m_spPlayer && m_spElecPanda)
	{
		SetPosition(m_spPlayer->GetPosition().value() + Vector3(0.f, 1.2f, 0.f));
		LookAt(m_spElecPanda->GetPosition().value() + Vector3(0.f, 2.5f, 0.f));
	}

	return 0;
}

int32 Client::CRedRazor::PostTick(float _fDeltaSeconds)
{
	switch (CEffectGroup::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Client::CRedRazor::EndPlay()
{
	if (FAILED(CEffectGroup::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CRedRazor::Release()
{
	CEffectGroup::Release();
}