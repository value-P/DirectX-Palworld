#include "pch.h"
#include "World.h"

#include "Level.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CWorld)

HRESULT Engine::CWorld::Initialize
(
	const std::shared_ptr<CLevel>& _spPersistentLevel,
	const std::shared_ptr<CLevel>& _spCurLevel,
	const std::shared_ptr<CLevel>& _spLoadLevel
)
{
	// 로고 레벨
	m_spFrontLevel = _spCurLevel;
	m_spFrontLevel->Initialize();
	return S_OK;
}

HRESULT Engine::CWorld::Initialize
(
	const std::shared_ptr<CLevel>& _spPersistentLevel,
	const std::shared_ptr<CLevel>& _spLoadLevel
)
{
	if (!_spPersistentLevel) { return E_FAIL; }
	if (!_spLoadLevel) { return E_FAIL; }

	// Initialize
	m_spPersistentLevel = _spPersistentLevel;
	m_spPersistentLevel->SetLevelState(ELevelState::INITIALIZING);
	ENSURE(m_spPersistentLevel->Initialize());
	m_spPersistentLevel->SetLevelState(ELevelState::INITIALIZED);

	m_spLoadLevel = _spLoadLevel;
	m_spLoadLevel->SetLevelState(ELevelState::INITIALIZING);
	ENSURE(m_spLoadLevel->Initialize());
	m_spLoadLevel->SetLevelState(ELevelState::INITIALIZED);

	// 필요한 리소스를 로드합니다.
	m_spLoadLevel->LoadResource(2);// logo level

	m_spFrontLevel = m_spLoadLevel;
	m_spBackLevel = m_spPersistentLevel;
	return S_OK;
}

HRESULT Engine::CWorld::BeginPlay()
{
	// BeginPlay
	if (m_spBackLevel)
	{
		m_spBackLevel->SetLevelState(ELevelState::BEGIN_PLAYING);
		ENSURE(m_spBackLevel->BeginPlay());
		m_spBackLevel->SetLevelState(ELevelState::BEGIN_PLAYED);
	}

	m_spFrontLevel->SetLevelState(ELevelState::BEGIN_PLAYING);
	ENSURE(m_spFrontLevel->BeginPlay());
	m_spFrontLevel->SetLevelState(ELevelState::BEGIN_PLAYED);
	return S_OK;
}

int32 Engine::CWorld::Tick(float _fDeltaSeconds)
{
	// PreTick
	if (m_spBackLevel)
	{
		m_spBackLevel->SetLevelState(ELevelState::PRE_TICKING);
		m_spBackLevel->PreTick(_fDeltaSeconds);
		m_spBackLevel->SetLevelState(ELevelState::PRE_TICKED);
	}

	m_spFrontLevel->SetLevelState(ELevelState::PRE_TICKING);
	m_spFrontLevel->PreTick(_fDeltaSeconds);
	m_spFrontLevel->SetLevelState(ELevelState::PRE_TICKED);

	// Tick
	if (m_spBackLevel)
	{
		m_spBackLevel->SetLevelState(ELevelState::TICKING);
		m_spBackLevel->Tick(_fDeltaSeconds);
		m_spBackLevel->SetLevelState(ELevelState::TICKED);
	}

	m_spFrontLevel->SetLevelState(ELevelState::TICKING);
	m_spFrontLevel->Tick(_fDeltaSeconds);
	m_spFrontLevel->SetLevelState(ELevelState::TICKED);

	// PostTick
	if (m_spBackLevel)
	{
		m_spBackLevel->SetLevelState(ELevelState::POST_TICKING);
		m_spBackLevel->PostTick(_fDeltaSeconds);
		m_spBackLevel->SetLevelState(ELevelState::POST_TICKED);
	}

	m_spFrontLevel->SetLevelState(ELevelState::POST_TICKING);
	m_spFrontLevel->PostTick(_fDeltaSeconds);
	m_spFrontLevel->SetLevelState(ELevelState::POST_TICKED);

	return 0;
}

void Engine::CWorld::Clear()
{
 	if (m_spBackLevel)
	{
		m_spBackLevel->SetLevelState(ELevelState::CLEARING);
		m_spBackLevel->Clear();
		m_spBackLevel->SetLevelState(ELevelState::CLEARED);
	}

	m_spFrontLevel->SetLevelState(ELevelState::CLEARING);
	m_spFrontLevel->Clear();
	m_spFrontLevel->SetLevelState(ELevelState::CLEARED);
}

void Engine::CWorld::Release()
{
	if (m_spBackLevel)
	{
		// 이곳에서 백 레벨은 해제하지 않습니다.
		m_spBackLevel = nullptr;
	}

	if (m_spLoadLevel)
	{
		m_spLoadLevel->SetLevelState(ELevelState::RELEASING);
		m_spLoadLevel->Release();
		m_spLoadLevel->SetLevelState(ELevelState::RELEASED);
		m_spLoadLevel = nullptr;
	}

	if (m_spPersistentLevel)
	{
		m_spPersistentLevel->SetLevelState(ELevelState::RELEASING);
		m_spPersistentLevel->Release();
		m_spPersistentLevel->SetLevelState(ELevelState::RELEASED);
		m_spPersistentLevel = nullptr;
	}

	if (m_spFrontLevel)
	{
		m_spFrontLevel->SetLevelState(ELevelState::RELEASING);
		m_spFrontLevel->Release();
		m_spFrontLevel->SetLevelState(ELevelState::RELEASED);
		m_spFrontLevel = nullptr;
	}

	// 싱글턴 월드를 메모리에서 해제합니다.
	m_spInstance = nullptr;
}

HRESULT Engine::CWorld::OpenLevel
(
	const std::shared_ptr<CLevel>& _spPostFrontLevel,
	const std::shared_ptr<CLevel>& _spPostBackLevel
)
{
	if (_spPostBackLevel)
	{
		m_spBackLevel = _spPostBackLevel;
	}
	else
	{
		m_spBackLevel = m_spPersistentLevel;
	}
	
	if (_spPostFrontLevel)
	{
		m_spFrontLevel = _spPostFrontLevel;
	}

	return S_OK;
}

HRESULT Engine::CWorld::LoadLevel(int32 _iPostLevelType)
{
	// 현재 레벨을 해제합니다.
	int32 iLevelType = m_spFrontLevel->GetLevelType();

	if (0 < iLevelType)
	{
		if (FAILED(ClearLevel())) { return E_FAIL; }
	}

	// 로드 레벨에서 필요한 리소스를 로드합니다.
	m_spLoadLevel->LoadResource(_iPostLevelType);

	m_spBackLevel = nullptr;
	m_spFrontLevel = m_spLoadLevel;
	return S_OK;
}

HRESULT Engine::CWorld::ClearLevel()
{
	// 현재 레벨을 메모리에서 해제합니다.
	m_spFrontLevel->Release();
	m_spFrontLevel = nullptr;

	// 매니저를 메모리에서 해제합니다.
	return S_OK;
}
