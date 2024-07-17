#pragma once
#include "DecoratorNode.h"

namespace Client
{
	class CMonster;

	/********************
		Description
	********************/
	class FDecoratorHungerDesc : public FDecoratorNodeDesc
	{
	};

	class CDecoratorHunger final : public CDecoratorNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDecoratorHunger() = default;

		virtual ~CDecoratorHunger() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDecoratorHunger> Create(const std::shared_ptr<FDecoratorHungerDesc>& _spDecoratorHungerDesc);

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
