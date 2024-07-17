#pragma once
#include "BTNode.h"

namespace Engine
{
	/********************
		Description
	********************/
	struct ENGINE_DLL FCompositeNodeDesc : public FBTNodeDesc
	{
	public:
		ECompositeType m_eCompositeType = ECompositeType::ENUM_END;
	};

    class ENGINE_DLL CCompositeNode : public CBTNode
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CCompositeNode() = default;

		virtual ~CCompositeNode() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CCompositeNode> Create(const shared_ptr<FCompositeNodeDesc>& _spCompositeNodeDesc);

		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteNode(float _fDeltaSecond) override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	protected:
		virtual EBTNodeResult ExecuteSelector(float _fDeltaSecond);

		virtual EBTNodeResult ExecuteSequence(float _fDeltaSecond);

		virtual EBTNodeResult ExecuteParallel(float _fDeltaSecond);

		virtual void ResetBTNodeResult() override;
		
	/********************
		Getter/Setter
	********************/
	public:
		virtual int32 GetCurRootBasedChildNodeIndex() const override { return m_iCurRootChildNodeIndex; }

	/********************
		Data Members
	********************/
	protected:
		ECompositeType m_eCompositeType = ECompositeType::ENUM_END;

		// 루트 노드가 실행하고 있는 자식 노드 인덱스
		int32 m_iCurRootChildNodeIndex = 0;

		// 현재 실행 중인 시퀀스 노드 인덱스
		int32 m_iCurSequenceNodeIndex = 0;
    };
}
