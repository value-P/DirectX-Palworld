#pragma once
#include "LightManager.h"
#include "Level.h"

namespace Engine
{
	class CInputManager;
	class CCamera;
}

namespace ToolAnimation
{
    class CToolAnimationLevel : public CLevel
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CToolAnimationLevel() = default;

		virtual ~CToolAnimationLevel() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize() override;

		virtual HRESULT BeginPlay() override;
		
		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		HRESULT LockCameraViewDir();
				
	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CCamera> m_spCamera;

		std::weak_ptr<CInputManager> m_wpInputManager;

		bool m_bLockCameraViewDir = false;
		bool m_bLockKeyBoardInput = false;
    };
}

