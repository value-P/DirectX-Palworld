#include "pch.h"
#include "SoundComponent.h"

std::shared_ptr<CSoundComponent> Engine::CSoundComponent::Create(const std::shared_ptr<FSoundComponentDesc>& _spSoundComponentDesc)
{
	const std::shared_ptr<CSoundComponent> spSoundComponent = make_shared<CSoundComponent>();
	ENSUREF(spSoundComponent->InitializeComponent(_spSoundComponentDesc), L"Failed to initialize sound component.");
	return spSoundComponent;
}

HRESULT Engine::CSoundComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// component desc
	if (FAILED(CComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// sound component desc
	shared_ptr<FSoundComponentDesc> spSoundComponentDesc = static_pointer_cast<FSoundComponentDesc>(_spComponentDesc);
	m_bLoop = spSoundComponentDesc->m_bLoop;
	m_fCurVolume = spSoundComponentDesc->m_fCurVolume;
	m_fMaxVolume = spSoundComponentDesc->m_fMaxVolume;
	m_fMinVolume = spSoundComponentDesc->m_fMinVolume;
	m_wstrSoundFileName = spSoundComponentDesc->m_wstrSoundFileName;

	return S_OK;
}

HRESULT Engine::CSoundComponent::BeginPlay()
{
	return S_OK;
}

int32 Engine::CSoundComponent::PreTickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CSoundComponent::TickComponent(float _fDeltaSeconds)
{
	return 0;
}

int32 Engine::CSoundComponent::PostTickComponent(float _fDeltaSeconds)
{
	return 0;
}

HRESULT Engine::CSoundComponent::EndPlay()
{
	return S_OK;
}

void Engine::CSoundComponent::Render()
{
	return;
}

void Engine::CSoundComponent::Release()
{
	return;
}

HRESULT Engine::CSoundComponent::BindVariable()
{
	return S_OK;
}
