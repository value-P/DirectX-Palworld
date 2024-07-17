#include "pch.h"
#include "SoundManager.h"

#include "EngineModule.h"
#include "SoundHandler.h"
#include "CameraManager.h"

HRESULT Engine::CSoundManager::Initialize()
{
	m_wpSoundHandler = CSoundHandler::GetInstance();
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();

	return S_OK;
}

int32 Engine::CSoundManager::Tick(float _fDeltaSeconds)
{
	return 0;
}

void Engine::CSoundManager::Release()
{
}

EChannelType Engine::CSoundManager::PlayBGM(const std::string& _strSoundFileName, float _fVolume)
{
	return m_wpSoundHandler.lock()->PlayBGM(_strSoundFileName, _fVolume);
}

EChannelType Engine::CSoundManager::PlaySoundEx(const std::string& _strSoundFileName, float _fVolume, bool _bLoop)
{
	return m_wpSoundHandler.lock()->PlaySoundEx(_strSoundFileName, _fVolume, _bLoop);
}

EChannelType Engine::CSoundManager::PlaySoundEx(const std::string& _strSoundFileName, const Vector3& _vSpeakPosition, float _fVolume, bool _bLoop)
{
	Vector3 vCameraPos = m_wpCameraManager.lock()->GetCurCameraWorldPosition();

	float fDistance = Vector3::Distance(vCameraPos, _vSpeakPosition);
	fDistance = max(m_fMinDist, fDistance);
	fDistance = min(m_fMaxDist, fDistance);

	float fVolume = _fVolume * (1.f - (fDistance / (m_fMaxDist - m_fMinDist)));

	return PlaySoundEx(_strSoundFileName, fVolume, _bLoop);
}

bool Engine::CSoundManager::PlaySoundEx(const std::string& _strSoundFileName, EChannelType _eManualChannelType, const Vector3& _vSpeakPosition, float _fVolume, bool _bLoop)
{
	// 수동으로 제어하는 채널에 소리를 재생합니다.
	switch (_eManualChannelType)
	{
	case Engine::EChannelType::MANUAL_26:
	case Engine::EChannelType::MANUAL_27:
	case Engine::EChannelType::MANUAL_28:
	case Engine::EChannelType::MANUAL_29:
		break;

	default:
		return false;
	}

	Vector3 vCameraPos = m_wpCameraManager.lock()->GetCurCameraWorldPosition();

	float fDistance = Vector3::Distance(vCameraPos, _vSpeakPosition);
	fDistance = max(m_fMinDist, fDistance);
	fDistance = min(m_fMaxDist, fDistance);

	float fVolume = _fVolume * (1.f - (fDistance / (m_fMaxDist - m_fMinDist)));

	PlaySoundEx(_strSoundFileName, fVolume, _bLoop);
	return true;
}

void Engine::CSoundManager::StopChannelEx(EChannelType _eChannelType)
{
	ENSURE(m_wpSoundHandler.lock()->StopChannelEx(_eChannelType));
}

void Engine::CSoundManager::PauseChannelEx(EChannelType _eChannelType, bool _bPause)
{
	ENSURE(m_wpSoundHandler.lock()->PauseChannelEx(_eChannelType, _bPause));
}

void Engine::CSoundManager::MuteChannelEx(EChannelType _eChannelType, bool _bMute)
{
	ENSURE(m_wpSoundHandler.lock()->MuteChannelEx(_eChannelType, _bMute));
}

void Engine::CSoundManager::ModifyChannelVolumeEx(EChannelType _eChannelType, float _fVolume)
{
	ENSURE(m_wpSoundHandler.lock()->ModifyChannelVolumeEx(_eChannelType, _fVolume));
}

void Engine::CSoundManager::ModifyChannelVolumeEx(EChannelType _eChannelType, const Vector3& _vPos, float _fVolume)
{
	Vector3 vCamPos = m_wpCameraManager.lock()->GetCurCameraWorldPosition();

	float fDistance = (vCamPos - _vPos).Length();

	fDistance = max(m_fMinDist, fDistance);
	fDistance = min(m_fMaxDist, fDistance);

	float fVolume = _fVolume * (1.f - (fDistance / (m_fMaxDist - m_fMinDist)));

	ModifyChannelVolumeEx(_eChannelType, fVolume);
}

void Engine::CSoundManager::StopChannelGroupEx(EChannelGroupType _eChannelGroupType)
{
	ENSURE(m_wpSoundHandler.lock()->StopChannelGroupEx(_eChannelGroupType));
}

void Engine::CSoundManager::PauseChannelGroupEx(EChannelGroupType _eChannelGroupType, bool _bPause)
{
	ENSURE(m_wpSoundHandler.lock()->PauseChannelGroupEx(_eChannelGroupType, _bPause));
}

void Engine::CSoundManager::MuteChannelGroupEx(EChannelGroupType _eChannelGroupType, bool _bMute)
{
	ENSURE(m_wpSoundHandler.lock()->MuteChannelGroupEx(_eChannelGroupType, _bMute));
}

void Engine::CSoundManager::ModifyChannelGroupVolumeEx(EChannelGroupType _eChannelGroupType, float _fVolume)
{
	ENSURE(m_wpSoundHandler.lock()->ModifyChannelGroupVolumeEx(_eChannelGroupType, _fVolume));
}
