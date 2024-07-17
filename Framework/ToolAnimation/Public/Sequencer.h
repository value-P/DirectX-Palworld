#pragma once
#include "WindowBase.h"

namespace ToolAnimation
{	
	class CSequencer : public CWindowBase
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CSequencer() = default;
		virtual ~CSequencer() = default;

		/********************
			Events
		********************/
	public:
		virtual void Initialize() override;
		virtual void Tick(float _fDeltaSeconds) override;
		virtual void Release() override;

		/********************
			Methods
		********************/
	public:

	private:
		void SequenceTab();
		
		/********************
			Getter/Setter
		********************/
	public:

		/********************
			Data Members
		********************/
	private:
		float m_fAnimationSpeed = 1.f;
	};
}