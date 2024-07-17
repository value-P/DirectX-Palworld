#include "pch.h"
#include "EffectBase.h"

#include "EffectGroup.h"

HRESULT ToolAnimation::CEffectBase::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	/* gameobject */
	m_eEffectType = static_pointer_cast<FEffectBaseDesc>(_spGameObjectDesc)->m_eEffectType;

	return S_OK;
}

HRESULT ToolAnimation::CEffectBase::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 ToolAnimation::CEffectBase::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 ToolAnimation::CEffectBase::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 ToolAnimation::CEffectBase::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

HRESULT ToolAnimation::CEffectBase::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void ToolAnimation::CEffectBase::Release()
{
	CGameObject::Release();
}
