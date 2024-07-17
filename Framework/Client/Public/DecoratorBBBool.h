#pragma once
#include "DecoratorNode.h"

namespace Client
{
	class CMonster;

	/********************
		Description
	********************/
	struct FDecoratorBBBoolDesc : public FDecoratorNodeDesc
	{
		EBlackboardBoolValue m_eBoolValue = EBlackboardBoolValue::ENUM_END;
	};

	class CDecoratorBBBool : public CDecoratorNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDecoratorBBBool() = default;

		virtual ~CDecoratorBBBool() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDecoratorBBBool> Create(const std::shared_ptr<FDecoratorBBBoolDesc>& _spDecoratorBBBoolDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc);

		virtual bool EvaluateDecoNode() override;
		
		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;

		EBlackboardBoolValue m_eBoolValue = EBlackboardBoolValue::ENUM_END;
	};
}
