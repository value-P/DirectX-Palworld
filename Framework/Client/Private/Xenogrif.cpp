#include "pch.h"
#include "Xenogrif.h"

std::shared_ptr<CXenogrif> Client::CXenogrif::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CXenogrif> spXenogrif = make_shared<CXenogrif>();
	ENSUREF(spXenogrif->Initialize(_spGameObjectDesc), L"Failed to initialize xenogrif.");
	return spXenogrif;
}

HRESULT Client::CXenogrif::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CMonster::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	return S_OK;
}

HRESULT Client::CXenogrif::BeginPlay()
{
	if (FAILED(CMonster::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Client::CXenogrif::PreTick(float _fDeltaSeconds)
{
	switch(CMonster::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CXenogrif::Tick(float _fDeltaSeconds)
{
	switch(CMonster::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CXenogrif::PostTick(float _fDeltaSeconds)
{
	switch(CMonster::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CXenogrif::EndPlay()
{
	if (FAILED(CMonster::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Client::CXenogrif::Release()
{
	CMonster::Release();
}
