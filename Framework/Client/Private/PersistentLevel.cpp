#include "pch.h"
#include "PersistentLevel.h"

HRESULT Client::CPersistentLevel::Initialize()
{
	{
		m_iLevelType = static_cast<int32>(ELevelType::PERSISTENT);
		SetLevelName(L"PersistentLevel");
	}
	
	cerr << "영구 레벨이 초기화되었습니다.\n";
    return CLevel::Initialize();
}

HRESULT Client::CPersistentLevel::BeginPlay()
{
    if (FAILED(CLevel::BeginPlay())) { return E_FAIL; }

	cerr << "영구 레벨이 시작되었습니다.\n";
    return S_OK;
}

int32 Client::CPersistentLevel::PreTick(float _fDeltaSeconds)
{
	switch (CLevel::PreTick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

int32 Client::CPersistentLevel::Tick(float _fDeltaSeconds)
{
	switch (CLevel::Tick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

int32 Client::CPersistentLevel::PostTick(float _fDeltaSeconds)
{
	switch (CLevel::PostTick(_fDeltaSeconds))
	{
	case -1:	break;
	default:	break;
	}

	return 0;
}

void Client::CPersistentLevel::Release()
{
	CLevel::Release();
}