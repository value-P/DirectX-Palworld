#include "pch.h"
#include "SoundHandler.h"

#include "EngineModule.h"
#include "ThreadManager.h"

IMPLEMENT_SINGLETON(CSoundHandler)

HRESULT Engine::CSoundHandler::Initialize()
{
	FMOD_System_Create(&m_pFMODSystem);
	FMOD_System_Init(m_pFMODSystem, 32, FMOD_INIT_NORMAL, nullptr);
	return S_OK;
}

void Engine::CSoundHandler::Release()
{
	for (auto& pChannelGroup : m_arrFMODChannelGroups)
	{
		FMOD_ChannelGroup_Release(pChannelGroup);
		pChannelGroup = nullptr;
	}

	for (auto& pair : m_umapFMODSounds)
	{
		FMOD_Sound_Release(pair.second);
	}
	m_umapFMODSounds.clear();

	FMOD_System_Release(m_pFMODSystem);
	FMOD_System_Close(m_pFMODSystem);

	m_spInstance = nullptr;
}

HRESULT Engine::CSoundHandler::LoadSoundFromFile()
{
	// 사운드를 생성합니다.
	for (const std::filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrSoundFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		// wav 파일이 아닌 경우
		if (fsEntry.path().extension().generic_string() != ".wav" && fsEntry.path().extension().generic_string() != ".WAV") { continue; }

		LoadSound(fsEntry);
	}

	// 채널 그룹을 생성합니다.
	FMOD_System_CreateChannelGroup(m_pFMODSystem, "Master",			&m_arrFMODChannelGroups[(int32)EChannelGroupType::MASTER]);
	FMOD_System_CreateChannelGroup(m_pFMODSystem, "Background",		&m_arrFMODChannelGroups[(int32)EChannelGroupType::BACKGROUND]);
	FMOD_System_CreateChannelGroup(m_pFMODSystem, "Effect",			&m_arrFMODChannelGroups[(int32)EChannelGroupType::EFFECT]);
	FMOD_System_CreateChannelGroup(m_pFMODSystem, "UserInterface",	&m_arrFMODChannelGroups[(int32)EChannelGroupType::USER_INTERFACE]);
	FMOD_System_CreateChannelGroup(m_pFMODSystem, "Voice",			&m_arrFMODChannelGroups[(int32)EChannelGroupType::VOICE]);
	FMOD_System_CreateChannelGroup(m_pFMODSystem, "Environment",	&m_arrFMODChannelGroups[(int32)EChannelGroupType::ENVIRONMENT]);

	// 시스템을 업데이트합니다.
	FMOD_System_Update(m_pFMODSystem);
	return S_OK;
}

HRESULT Engine::CSoundHandler::LoadSound(const std::filesystem::directory_entry& _fsEntry)
{
	string strSoundFilePath = _fsEntry.path().generic_string();

	FMOD_SOUND* pFMODSound = nullptr;
	if (FMOD_System_CreateSound(m_pFMODSystem, strSoundFilePath.c_str(), FMOD_DEFAULT, 0, &pFMODSound) != FMOD_OK) { return E_FAIL; }

	string strSoundFileName = _fsEntry.path().stem().generic_string();
	return AddSound(strSoundFileName, pFMODSound);
}

HRESULT Engine::CSoundHandler::AddSound(const std::string& _strSoundFileName, FMOD_SOUND* _pFMODSound)
{
	if (m_umapFMODSounds.contains(_strSoundFileName)) { return E_FAIL; }

	m_umapFMODSounds.emplace(_strSoundFileName, _pFMODSound);
	return S_OK;
}

EChannelType Engine::CSoundHandler::PlayBGM(const std::string& _strSoundFileName, float _fVolume)
{
	if (!m_umapFMODSounds.contains(_strSoundFileName)) { return EChannelType::ENUM_END; }

	FMOD_System_PlaySound(m_pFMODSystem, m_umapFMODSounds[_strSoundFileName], nullptr, false, &m_arrFMODChannels[(int32)EChannelType::BACKGROUND_00]);
	FMOD_Channel_SetVolume(m_arrFMODChannels[(int32)EChannelType::BACKGROUND_00], _fVolume);
	FMOD_Channel_SetMode(m_arrFMODChannels[(int32)EChannelType::BACKGROUND_00], FMOD_LOOP_NORMAL);

	FMOD_System_Update(m_pFMODSystem);

	return EChannelType::BACKGROUND_00;
}

EChannelType Engine::CSoundHandler::PlaySoundEx(const std::string& _strSoundFileName, float _fVolume, bool _bLoop)
{
	if (!m_umapFMODSounds.contains(_strSoundFileName)) { return EChannelType::ENUM_END; }

	for (int32 iChannelIndex = (int32)EChannelType::AUTO_00; iChannelIndex < (int32)EChannelType::ENUM_END; ++iChannelIndex)
	{
		FMOD_BOOL bResult = FALSE;
		FMOD_Channel_IsPlaying(m_arrFMODChannels[iChannelIndex], &bResult);

		if (!bResult)
		{
			FMOD_System_PlaySound(m_pFMODSystem, m_umapFMODSounds[_strSoundFileName], nullptr, false, &m_arrFMODChannels[iChannelIndex]);
			FMOD_Channel_SetVolume(m_arrFMODChannels[iChannelIndex], _fVolume);
			
			if (_bLoop) 
			{
				FMOD_Channel_SetMode(m_arrFMODChannels[iChannelIndex], FMOD_LOOP_NORMAL); 
			}

			FMOD_System_Update(m_pFMODSystem);

			return (EChannelType)iChannelIndex;
		}
	}

	return EChannelType::ENUM_END;
}

bool Engine::CSoundHandler::PlaySoundEx(const std::string& _strSoundFileName, EChannelType _eManualChannelType, float _fVolume, bool _bLoop)
{
	if (!m_umapFMODSounds.contains(_strSoundFileName)) { return false; }

	FMOD_BOOL bResult = FALSE;
	FMOD_Channel_IsPlaying(m_arrFMODChannels[(int32)_eManualChannelType], &bResult);

	// 채널이 비어있지 않은 경우
	if (bResult)
	{
		StopChannelEx(_eManualChannelType);
	}

	FMOD_System_PlaySound(m_pFMODSystem, m_umapFMODSounds[_strSoundFileName], nullptr, false, &m_arrFMODChannels[(int32)_eManualChannelType]);
	FMOD_Channel_SetVolume(m_arrFMODChannels[(int32)_eManualChannelType], _fVolume);

	if (_bLoop)
	{
		FMOD_Channel_SetMode(m_arrFMODChannels[(int32)_eManualChannelType], FMOD_LOOP_NORMAL);
	}

	FMOD_System_Update(m_pFMODSystem);
	return true;
}

HRESULT Engine::CSoundHandler::StopChannelEx(EChannelType _eChannelType)
{
	if (FMOD_Channel_Stop(m_arrFMODChannels[static_cast<int32>(_eChannelType)]) != FMOD_RESULT::FMOD_OK) { return E_FAIL; }

	if (FMOD_System_Update(m_pFMODSystem)) { return E_FAIL; }

	return S_OK;
}

HRESULT Engine::CSoundHandler::PauseChannelEx(EChannelType _eChannelType, bool _bPause)
{
	if (FMOD_Channel_SetPaused(m_arrFMODChannels[static_cast<int32>(_eChannelType)], _bPause) != FMOD_RESULT::FMOD_OK) { return E_FAIL; }

	if (FMOD_System_Update(m_pFMODSystem)) { return E_FAIL; }

	return S_OK;
}

HRESULT Engine::CSoundHandler::MuteChannelEx(EChannelType _eChannelType, bool _bMute)
{
	if (FMOD_Channel_SetMute(m_arrFMODChannels[static_cast<int32>(_eChannelType)], _bMute) != FMOD_RESULT::FMOD_OK) { return E_FAIL; }

	if (FMOD_System_Update(m_pFMODSystem)) { return E_FAIL; }

	return S_OK;
}

HRESULT Engine::CSoundHandler::ModifyChannelVolumeEx(EChannelType _eChannelType, float _fVolume)
{
	if (FMOD_Channel_SetVolume(m_arrFMODChannels[static_cast<int32>(_eChannelType)], _fVolume) != FMOD_RESULT::FMOD_OK) { return E_FAIL; }

	if (FMOD_System_Update(m_pFMODSystem)) { return E_FAIL; }

	return S_OK;
}

HRESULT Engine::CSoundHandler::StopChannelGroupEx(EChannelGroupType _eChannelGroupType)
{
	if (FMOD_ChannelGroup_Stop(m_arrFMODChannelGroups[static_cast<int32>(_eChannelGroupType)]) != FMOD_RESULT::FMOD_OK) { return E_FAIL; }

	if (FMOD_System_Update(m_pFMODSystem)) { return E_FAIL; }

	return S_OK;
}

HRESULT Engine::CSoundHandler::PauseChannelGroupEx(EChannelGroupType _eChannelGroupType, bool _bPause)
{
	if (FMOD_ChannelGroup_SetPaused(m_arrFMODChannelGroups[static_cast<int32>(_eChannelGroupType)], _bPause) != FMOD_RESULT::FMOD_OK) { return E_FAIL; }

	if (FMOD_System_Update(m_pFMODSystem)) { return E_FAIL; }

	return S_OK;
}

HRESULT Engine::CSoundHandler::MuteChannelGroupEx(EChannelGroupType _eChannelGroupType, bool _bMute)
{
	if (FMOD_ChannelGroup_SetMute(m_arrFMODChannelGroups[static_cast<int32>(_eChannelGroupType)], _bMute) != FMOD_RESULT::FMOD_OK) { return E_FAIL; }

	if (FMOD_System_Update(m_pFMODSystem)) { return E_FAIL; }

	return S_OK;
}

HRESULT Engine::CSoundHandler::ModifyChannelGroupVolumeEx(EChannelGroupType _eChannelGroupType, float _fVolume)
{
	if (FMOD_ChannelGroup_SetVolume(m_arrFMODChannelGroups[static_cast<int32>(_eChannelGroupType)], _fVolume) != FMOD_RESULT::FMOD_OK) { return E_FAIL; }

	if (FMOD_System_Update(m_pFMODSystem)) { return E_FAIL; }

	return S_OK;
}