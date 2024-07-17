#pragma once
#include "DecoratorNode.h"

namespace Client
{
	class CMonster;
	
	/********************
		Description
	********************/
	class FDecoratorSleepDesc : public FDecoratorNodeDesc
	{
	};

	class CDecoratorSleep : public CDecoratorNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDecoratorSleep() = default;

		virtual ~CDecoratorSleep() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDecoratorSleep> Create(const std::shared_ptr<FDecoratorSleepDesc>& _spDecoratorSleepDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc);

		virtual bool EvaluateDecoNode() override;
		
		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;
	};
}
