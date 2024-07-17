#pragma once

namespace Engine
{
	class CBehaviorTreeComponent;
	class CDecoratorNode;
	class CGameObject;

	/********************
		Description
	********************/
	struct ENGINE_DLL FBTNodeDesc
	{
		std::shared_ptr<CGameObject> m_spOwnerGameObject = nullptr;

		EBTNodeType m_eBTNodeType = EBTNodeType::ENUM_END;

		std::wstring m_wstrBTNodeName = L"";

		bool m_bRoot = false;
	};

	class ENGINE_DLL CBTNode abstract : public enable_shared_from_this<CBTNode>
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CBTNode() = default;

		virtual ~CBTNode() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc);

		virtual void OnNodeInit(float _fDeltaSecond) = 0;

		virtual void OnNodeEnter(float _fDeltaSecond) = 0;

		virtual EBTNodeResult ExecuteNode(float _fDeltaSecond);

		virtual void Release();

	/********************
		Methods
	********************/
	public:
		HRESULT AddBTNode(const std::shared_ptr<CBTNode>& _spBTNode);

		virtual void ResetBTNodeResult();

	/********************
		Getter/Setter
	********************/
	public:
		// 노드 결과
		void SetCurBTNodeResult(EBTNodeResult _eCurBTNodeResult) { m_eCurBTNodeResult = _eCurBTNodeResult; }

		EBTNodeResult GetCurBTNodeResult() const { return m_eCurBTNodeResult; }

		// 노드 타입
		EBTNodeType GetBTNodeType() const { return m_eBTNodeType; }

		const std::wstring& GetBTNodeName() { return m_wstrBTNodeName; }

		// 부모 노드
		void SetParentBTNode(const std::shared_ptr<CBTNode>& _spParentBTNode) { m_spParentBTNode = _spParentBTNode; }

		const shared_ptr<CBTNode>& GetParentBTNode() const { return m_spParentBTNode; }

		// 루트 노드
		void SetRootBTNode(const std::shared_ptr<CBTNode>& _spRootBTNode) { m_spRootBTNode = _spRootBTNode; }

		const shared_ptr<CBTNode>& GetRootBTNode() const { return m_spRootBTNode; }

		// 게임 오브젝트
		void SetOwnerGameObject(const std::shared_ptr<CGameObject>& _spOwnerGameObject) { m_wpOwnerGameObject = _spOwnerGameObject; }

		// 루트 노드가 실행 중인 자식 노드 인덱스
		virtual int32 GetCurRootBasedChildNodeIndex() const { return -1; };

	/********************
		Data Members
	********************/
	protected:
		// 루트 노드 여부
		bool m_bRoot = false;

		// 이 노드를 갖고 있는 게임 오브젝트
		std::weak_ptr<CGameObject> m_wpOwnerGameObject;

		// 루트 노드
		std::shared_ptr<CBTNode> m_spRootBTNode = nullptr;

		// 부모 노드
		std::shared_ptr<CBTNode> m_spParentBTNode = nullptr;

		// 노드 이름
		std::wstring m_wstrBTNodeName = L"";

		// 노드 타입
		EBTNodeType m_eBTNodeType = EBTNodeType::ENUM_END;

		// 노드 결과
		EBTNodeResult m_eCurBTNodeResult = EBTNodeResult::ENUM_END;

		// 데코레이터 노드들
		std::vector<std::shared_ptr<CDecoratorNode>> m_vecDecoratorNodes;

		// 비헤이비어 노드들
		std::vector<std::shared_ptr<CBTNode>> m_vecBTNodes;

		// 비헤이비어 컴포넌트
		std::weak_ptr<CBehaviorTreeComponent> m_wpBehaviorTreeComponent;
	};
}
