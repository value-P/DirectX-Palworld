#pragma once
#include "Manager.h"

namespace Engine
{
	class CSoundHandler;
	class CCameraManager;

	class ENGINE_DLL CSoundManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CSoundManager() = default;

		virtual ~CSoundManager() = default;

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		int32 Tick(float _fDeltaSeconds);

		virtual void Release() override;
	
	/********************
		Methods
	********************/
	public:
		EChannelType PlayBGM(const std::string& _strSoundFileName, float _fVolume = 1.0f);

		EChannelType PlaySoundEx(const std::string& _strSoundFileName, float _fVolume = 1.0f, bool _bLoop = false);

		EChannelType PlaySoundEx(const std::string& _strSoundFileName, const Vector3& _vSpeakPosition, float _fVolume = 1.0f, bool _bLoop = false);

		bool PlaySoundEx(const std::string& _strSoundFileName, EChannelType _eManualChannelType, const Vector3& _vSpeakPosition, float _fVolume = 1.0f, bool _bLoop = false);

		void StopChannelEx(EChannelType _eChannelType);

		void PauseChannelEx(EChannelType _eChannelType, bool _bPause);

		void MuteChannelEx(EChannelType _eChannelType, bool _bMute);

		void ModifyChannelVolumeEx(EChannelType _eChannelType, float _fVolume);

		void ModifyChannelVolumeEx(EChannelType _eChannelType, const Vector3& _vPos, float _fVolume);

	public:
		void StopChannelGroupEx(EChannelGroupType _eChannelGroupType);

		void PauseChannelGroupEx(EChannelGroupType _eChannelGroupType, bool _bPause);

		void MuteChannelGroupEx(EChannelGroupType _eChannelGroupType, bool _bMute);

		void ModifyChannelGroupVolumeEx(EChannelGroupType _eChannelGroupType, float _fVolume);

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CSoundHandler> m_wpSoundHandler;
		std::weak_ptr<CCameraManager> m_wpCameraManager;

	private:
		float m_fMinDist = 0.5f;
		float m_fMaxDist = 20.f;
	};
}
