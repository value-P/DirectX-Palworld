#pragma once
#include "BTNode.h"

namespace Engine
{
	struct FDecoratorNodeDesc : public FBTNodeDesc
	{

	};

    class ENGINE_DLL CDecoratorNode abstract : public CBTNode
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDecoratorNode() = default;

		virtual ~CDecoratorNode() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

		virtual void OnNodeInit(float _fDeltaSecond) override sealed {};

		virtual void OnNodeEnter(float _fDeltaSecond) override sealed {};

		virtual EBTNodeResult ExecuteNode(float _fDeltaSecond) override sealed { return EBTNodeResult::ENUM_END; }

		virtual void Release() override;

	public:
		virtual bool EvaluateDecoNode() = 0;
    };
}