#include "pch.h"
#include "Armor.h"

std::shared_ptr<CArmor> Client::CArmor::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CArmor> spInstance = make_shared<CArmor>();
	CHECKF(spInstance, L"CArmor : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CArmor::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	if (FAILED(CItem::Initialize(_spGameObjectDesc))) { return E_FAIL; }
	return S_OK;
}

HRESULT Client::CArmor::BeginPlay()
{
	if (FAILED(CItem::BeginPlay())) { return E_FAIL; }
	return S_OK;
}

int32 Client::CArmor::PreTick(float _fDeltaSeconds)
{
	switch(CItem::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CArmor::Tick(float _fDeltaSeconds)
{
	switch(CItem::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

int32 Client::CArmor::PostTick(float _fDeltaSeconds)
{
	switch(CItem::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}
	return 0;
}

HRESULT Client::CArmor::EndPlay()
{
	if (FAILED(CItem::EndPlay())) { return E_FAIL; }
	return S_OK;
}

void Client::CArmor::Release()
{
	CItem::Release();
}
