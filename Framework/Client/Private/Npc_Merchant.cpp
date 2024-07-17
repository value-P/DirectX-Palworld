#include "pch.h"
#include "Npc_Merchant.h"

shared_ptr<CNpc_Merchant> Client::CNpc_Merchant::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CNpc_Merchant> spInstance = make_shared<CNpc_Merchant>();
	ENSUREF(spInstance->Initialize(_spGameObjectDesc), L"Failed to initialize CNpc_Merchant.");
	return spInstance;
}

HRESULT Client::CNpc_Merchant::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CNpc::Initialize(_spGameObjectDesc);

	return S_OK;
}

HRESULT Client::CNpc_Merchant::BeginPlay()
{
	CNpc::BeginPlay();

	return S_OK;
}

int32 Client::CNpc_Merchant::PreTick(float _fDeltaSeconds)
{
	CNpc::PreTick(_fDeltaSeconds);

	return 0;
}

int32 Client::CNpc_Merchant::Tick(float _fDeltaSeconds)
{
	CNpc::Tick(_fDeltaSeconds);

	return 0;
}

int32 Client::CNpc_Merchant::PostTick(float _fDeltaSeconds)
{
	CNpc::PostTick(_fDeltaSeconds);

	return 0;
}

HRESULT Client::CNpc_Merchant::EndPlay()
{
	CNpc::EndPlay();

	return S_OK;
}

void Client::CNpc_Merchant::Release()
{
	CNpc::Release();
}

void Client::CNpc_Merchant::Interact()
{
	if (!m_bTalked)
	{
		m_bTalked = true;
		ChangeState(ENpcStateType::Talking);
	}
	else
	{
		if (m_vecTextList.size() - 1 < m_iCurrentTextIndex)
		{
			++m_iCurrentTextIndex;
		}
		else
		{

		}
	}
}

void Client::CNpc_Merchant::StateIdle(float _fDeltaSeconds)
{
}

void Client::CNpc_Merchant::StateTalking(float _fDeltaSeconds)
{
}

void Client::CNpc_Merchant::StateGive(float _fDeltaSeconds)
{
}

void Client::CNpc_Merchant::StateUseDevice(float _fDeltaSeconds)
{
}
