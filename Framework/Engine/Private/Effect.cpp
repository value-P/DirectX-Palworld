#include "pch.h"
#include "Effect.h"

#include "EffectMeshComponent.h"

std::shared_ptr<CEffect> Engine::CEffect::Create(const std::shared_ptr<FEffectDesc>& _spEffectDesc)
{
	const std::shared_ptr<CEffect> spEffect = make_shared<CEffect>();
	ENSUREF(spEffect->Initialize(_spEffectDesc), L"Failed to initialize effect.");
	return spEffect;
}

HRESULT Engine::CEffect::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// game object
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// effect
	shared_ptr<FEffectDesc> spEffectDsec = static_pointer_cast<FEffectDesc>(_spGameObjectDesc);
	
	// etc
	// LoadFromJsonFile(m_wstrJsonFileName);
	return S_OK;
}

HRESULT Engine::CEffect::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CEffect::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return -1;
}

int32 Engine::CEffect::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return -1;
}

int32 Engine::CEffect::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1: return -1;
	default: break;
	}

	return 0;
}

HRESULT Engine::CEffect::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CEffect::Release()
{
	CGameObject::Release();
}

void Engine::CEffect::RecycleFromPool()
{
	// ������Ʈ Ǯ�κ��� �������� �� �ڵ����� ����� �Լ��Դϴ�.
	// -> ����Ʈ ���� �ÿ� �ʱ�ȭ ���Ѿ��� ���̳� �������� ���⿡ �ۼ��ϸ� �˴ϴ�.
}
