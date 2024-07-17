#pragma once
#include "DecoratorNode.h"

namespace Client
{
	class CMonster;

	struct FDecoratorInteractedDesc : public FDecoratorNodeDesc
	{
	};

	class CDecoratorInteracted final : public CDecoratorNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDecoratorInteracted() = default;

		virtual ~CDecoratorInteracted() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDecoratorInteracted> Create(const std::shared_ptr<FDecoratorInteractedDesc>& _spDecoratorInteractedDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FBTNodeDesc>& _spNodeDesc);

		virtual bool EvaluateDecoNode() override;
		
		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;

		bool m_bInit = false;
	};
}
