#pragma once
#include "DecoratorNode.h"

namespace Engine
{
    class CDate;
}

namespace Client
{
	class FDecoratorDayDesc : public FDecoratorNodeDesc
	{
	public:
	};

    class CDecoratorDay : public CDecoratorNode
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDecoratorDay() = default;

		virtual ~CDecoratorDay() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDecoratorDay> Create(const std::shared_ptr<FDecoratorDayDesc>& _spDecoratorDayDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc);

		virtual bool EvaluateDecoNode() override;
		
		virtual void Release() override;
		
	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CDate> m_spDate = nullptr;
    };
}
