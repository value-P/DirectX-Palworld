#include "pch.h"
#include "NonInteractionNPC.h"

shared_ptr<CNonInteractionNPC> Client::CNonInteractionNPC::Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	shared_ptr<CNonInteractionNPC> spInstance = make_shared<CNonInteractionNPC>();
	CHECKF(spInstance, L"CNonInteractionNPC : Create Failed");
	spInstance->Initialize(_spGameObjectDesc);

	return spInstance;
}

HRESULT Client::CNonInteractionNPC::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	CNpc::Initialize(_spGameObjectDesc);

	ChangeDefaultAnimation(static_pointer_cast<FNonInteractionNPC>(_spGameObjectDesc)->m_eDefaultAnim);

	return S_OK;
}

HRESULT Client::CNonInteractionNPC::BeginPlay()
{
	CNpc::BeginPlay();

	return S_OK;
}

int32 Client::CNonInteractionNPC::PreTick(float _fDeltaSeconds)
{
	CNpc::PreTick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNonInteractionNPC::Tick(float _fDeltaSeconds)
{
	CNpc::Tick(_fDeltaSeconds);

	return S_OK;
}

int32 Client::CNonInteractionNPC::PostTick(float _fDeltaSeconds)
{
	CNpc::PostTick(_fDeltaSeconds);

	return S_OK;
}

HRESULT Client::CNonInteractionNPC::EndPlay()
{
	CNpc::EndPlay();

	return S_OK;
}

void Client::CNonInteractionNPC::Release()
{
	CNpc::Release();
}

void Client::CNonInteractionNPC::InitCycle()
{
}

void Client::CNonInteractionNPC::Interact()
{
}
