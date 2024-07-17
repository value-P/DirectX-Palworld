#pragma once
#include "DecoratorNode.h"

namespace Engine
{
	class CDate;
}

namespace Client
{
	class FDecoratorNightDesc : public FDecoratorNodeDesc
	{
	public:
	};

    class CDecoratorNight : public CDecoratorNode
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDecoratorNight() = default;

		virtual ~CDecoratorNight() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDecoratorNight> Create(const std::shared_ptr<FDecoratorNightDesc>& _spDecoratorNightDesc);

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
