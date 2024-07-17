#pragma once

namespace Engine
{
	class CEngineModule;
	class CWorld;
}

namespace ToolAnimation
{
	class CImguiMgr;
	class CModelBlackBoard;
	class CEffectManager;

	class ToolAnimationLoop
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit ToolAnimationLoop() = default;

		virtual ~ToolAnimationLoop() = default;

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		HRESULT BeginPlay();

		int32 Tick();

		void Render();

		void Release();

	/********************
		Data Members
	********************/
	public:
		std::shared_ptr<CEngineModule>		m_spEngineModule = nullptr;
		std::shared_ptr<CWorld>				m_spWorld = nullptr;
		std::shared_ptr<CModelBlackBoard>	m_spModelBlackBoard = nullptr;
		std::shared_ptr<CImguiMgr>			m_spImguiMgr = nullptr;
		std::shared_ptr<CEffectManager>		m_spEffectMgr = nullptr;
	};
}
