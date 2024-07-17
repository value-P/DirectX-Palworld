#pragma once

namespace Engine
{
	class CThreadManager;

	class ENGINE_DLL CSoundHandler final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CSoundHandler)
		
	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		void Release();
		
	/********************
		Methods
	********************/
	public:
		HRESULT LoadSoundFromFile();

		HRESULT LoadSound(const std::filesystem::directory_entry& _fsEntry);

		HRESULT AddSound(const std::string& _strSoundFileName, FMOD_SOUND* _pFMODSound);

	public:
		EChannelType PlayBGM(const std::string& _strSoundFileName, float _fVolume = 1.0f);

		EChannelType PlaySoundEx(const std::string& _strSoundFileName, float _fVolume = 1.0f, bool _bLoop = false);

		bool PlaySoundEx(const std::string& _strSoundFileName, EChannelType _eManualChannelType, float _fVolume = 1.0f, bool _bLoop = false);

		HRESULT StopChannelEx(EChannelType _eChannelType);

		HRESULT PauseChannelEx(EChannelType _eChannelType, bool _bPause);

		HRESULT MuteChannelEx(EChannelType _eChannelType, bool _bMute);

		HRESULT ModifyChannelVolumeEx(EChannelType _eChannelType, float _fVolume);

	public:
		HRESULT StopChannelGroupEx(EChannelGroupType _eChannelGroupType);

		HRESULT PauseChannelGroupEx(EChannelGroupType _eChannelGroupType, bool _bPause);

		HRESULT MuteChannelGroupEx(EChannelGroupType _eChannelGroupType, bool _bMute);

		HRESULT ModifyChannelGroupVolumeEx(EChannelGroupType _eChannelGroupType, float _fVolume);

	/********************
		Data Members
	********************/
	private:
		// 사운드들
		std::unordered_map<std::string, FMOD_SOUND*> m_umapFMODSounds;

		// 사운드 채널들
		std::array<FMOD_CHANNEL*, (int32)EChannelType::ENUM_END> m_arrFMODChannels{ nullptr };
		
		// 사운드 채널 그룹들
		std::array<FMOD_CHANNELGROUP*, (int32)EChannelGroupType::ENUM_END> m_arrFMODChannelGroups{ nullptr };

	private:
		FMOD_SYSTEM* m_pFMODSystem = nullptr;
		
		// 사운드 파일 경로
		std::wstring m_wstrSoundFilePath = L"../../Resource/Sounds/";
	};
}
