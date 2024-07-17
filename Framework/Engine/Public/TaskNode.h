#pragma once
#include "BTNode.h"

namespace Engine
{
	struct FTaskNodeDesc : public FBTNodeDesc
	{

	};

    class ENGINE_DLL CTaskNode : public CBTNode
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskNode() = default;

		virtual ~CTaskNode() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteNode(float _fDeltaSecond) override sealed;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	public:
		virtual EBTNodeResult ExecuteTaskNode(float _fDeltaSecond);
	};
}
