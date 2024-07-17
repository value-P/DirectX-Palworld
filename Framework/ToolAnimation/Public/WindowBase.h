#pragma once

namespace Engine
{
	class CLevel;
}

namespace ToolAnimation
{
	class CModelBlackBoard;

	class CWindowBase
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CWindowBase() = default;

		virtual ~CWindowBase() = default;

	/********************
		Events
	********************/
	public:
		virtual void Initialize();

		virtual void BeginPlay();
		
		virtual void PreTick(float _fDeltaSeconds);

		virtual void Tick(float _fDeltaSeconds);

		virtual void PostTick(float _fDeltaSeconds);

		virtual void EndPlay();

		virtual void Release();

	/********************
		Methods
	********************/
	public:

	/********************
		Getter/Setter
	********************/
	public:
		void SetCurrentLevel(shared_ptr<CLevel> _spCurLevel);
				
	/********************
		Data Members
	********************/
	protected:
		std::weak_ptr<CLevel> m_wpLevel;
		std::weak_ptr<CModelBlackBoard> m_wpModelBlackBoard;
		FAnimationData m_tAnimData;

		ImGuiWindowFlags m_eWindowFlag = 0;
	};
}


