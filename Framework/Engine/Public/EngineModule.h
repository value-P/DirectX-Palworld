#pragma once

namespace Engine
{
	class CCameraManager;
	class CDeviceManager;
	class CEventManager;
	class CFrameManager;
	class CInputManager;
	class CLightManager;
	class CRenderManager;
	class CPipelineManager;
	class CPhysXManager;
	class CThreadManager;
	class CTimerManager;
	class CPickManager;
	class CSoundManager;

	class CFontHandler;
	class CShaderHandler;
	class CSoundHandler;
	class CTextureHandler;
	class CModelHandler;
	class CMeshHandler;
	/*
	* @detail
	* 엔진 모듈 클래스는 엔진에서 제공하는 모든 매니저와 핸들러를 생성, 초기화, 그리고 해제하는 싱글턴 클래스입니다.
	* 엔진 모듈에 선언된 매니저의 포인터가 필요한 경우, 아래와 같이 작성해주세요.
	*
	* @example
	* CEngineModule::GetInstance()->Get...();
	*/
	class ENGINE_DLL CEngineModule final
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		DECLARE_SINGLETON(CEngineModule)

		/********************
			Events
		********************/
	public:
		HRESULT Initialize(HWND _hWnd, HINSTANCE _hInst, int32 _iWindowSizeX, int32 _iWindowSizeY);

		int32 PreTick(float* _pDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);

		int32 PostTick(float _fDeltaSeconds);

		void PreRender();

		void PostRender();

		void HandleEvent();

		void Release();

		/********************
			Methods
		********************/
	public:
		void LoadResources();

		/********************
			Getter/Setter
		********************/
	public:
		std::shared_ptr<CCameraManager> GetCameraManager() const { return m_spCameraManager; }

		std::shared_ptr<CDeviceManager> GetDeviceManager() const { return m_spDeviceManager; }

		std::shared_ptr<CEventManager> GetEventManager() const { return m_spEventManager; }

		std::shared_ptr<CFrameManager> GetFrameManager() const { return m_spFrameManager; }

		std::shared_ptr<CInputManager> GetInputManager() const { return m_spInputManager; }

		std::shared_ptr<CLightManager> GetLightManager() const { return m_spLightManager; }

		std::shared_ptr<CRenderManager> GetRenderManager() const { return m_spRenderManager; }

		std::shared_ptr<CPipelineManager> GetPipelineManager() const { return m_spPipelineManager; }

		std::shared_ptr<CPhysXManager> GetPhysXManager() const { return m_spPhysXManager; }

		std::shared_ptr<CThreadManager> GetThreadManager() const { return m_spThreadManager; }

		std::shared_ptr<CTimerManager> GetTimerManager() const { return m_spTimerManager; }

		std::shared_ptr<CPickManager> GetPickManager() const { return m_spPickManager; }

		std::shared_ptr<CSoundManager> GetSoundManager() const { return m_spSoundManager; }

		/********************
			Data Members
		********************/
	private:
		std::shared_ptr<CCameraManager> m_spCameraManager = nullptr;

		std::shared_ptr<CDeviceManager> m_spDeviceManager = nullptr;

		std::shared_ptr<CEventManager> m_spEventManager = nullptr;

		std::shared_ptr<CInputManager> m_spInputManager = nullptr;

		std::shared_ptr<CLightManager> m_spLightManager = nullptr;

		std::shared_ptr<CRenderManager> m_spRenderManager = nullptr;

		std::shared_ptr<CFrameManager> m_spFrameManager = nullptr;

		std::shared_ptr<CPipelineManager> m_spPipelineManager = nullptr;

		std::shared_ptr<CPhysXManager> m_spPhysXManager = nullptr;

		std::shared_ptr<CThreadManager> m_spThreadManager = nullptr;

		std::shared_ptr<CTimerManager> m_spTimerManager = nullptr;

		std::shared_ptr<CPickManager> m_spPickManager = nullptr;

		std::shared_ptr<CSoundManager> m_spSoundManager = nullptr;

	private:
		std::shared_ptr<CShaderHandler> m_spShaderHandler = nullptr;

		std::shared_ptr<CTextureHandler> m_spTextureHandler = nullptr;

		std::shared_ptr<CFontHandler> m_spFontHandler = nullptr;

		std::shared_ptr<CSoundHandler> m_spSoundHandler = nullptr;

		std::shared_ptr<CModelHandler> m_spModelHandler = nullptr;

		std::shared_ptr<CMeshHandler> m_spMeshHandler = nullptr;

	private:
		// 60fps
		const float m_fFrameRate = 1.0f / 60.0f;

		float m_fSumDeltaSeconds = 0.0f;
	};
}
