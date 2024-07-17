// default
#include "pch.h"
#include "Character.h"

#include "MeshComponent.h"
#include "CameraComponent.h"

std::shared_ptr<CCharacter> Engine::CCharacter::Create(const std::shared_ptr<FCharacterDesc>& _spCharacterDesc)
{
	const std::shared_ptr<CCharacter> spCharacter = make_shared<CCharacter>();
	ENSUREF(spCharacter->Initialize(_spCharacterDesc), L"Failed to initialize character.")
	return spCharacter;
}

HRESULT Engine::CCharacter::Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc)
{
	// game object
	if (FAILED(CGameObject::Initialize(_spGameObjectDesc))) { return E_FAIL; }

	// character
	shared_ptr<FCharacterDesc> spCharacterDesc = static_pointer_cast<FCharacterDesc>(_spGameObjectDesc);
	m_fJumpSpeed = spCharacterDesc->m_fJumpSpeed;
	m_fMoveSpeed = spCharacterDesc->m_fMoveSpeed;
	return S_OK;
}

HRESULT Engine::CCharacter::BeginPlay()
{
	if (FAILED(CGameObject::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CCharacter::PreTick(float _fDeltaSeconds)
{
	switch (CGameObject::PreTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 Engine::CCharacter::Tick(float _fDeltaSeconds)
{
	switch (CGameObject::Tick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

int32 Engine::CCharacter::PostTick(float _fDeltaSeconds)
{
	switch (CGameObject::PostTick(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return S_OK;
}

HRESULT Engine::CCharacter::EndPlay()
{
	if (FAILED(CGameObject::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CCharacter::Release()
{
	m_spCameraComponent = nullptr;
	m_spMeshComponent = nullptr;

	CGameObject::Release();
}
