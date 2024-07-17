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
		// ��� ���
		void SetCurBTNodeResult(EBTNodeResult _eCurBTNodeResult) { m_eCurBTNodeResult = _eCurBTNodeResult; }

		EBTNodeResult GetCurBTNodeResult() const { return m_eCurBTNodeResult; }

		// ��� Ÿ��
		EBTNodeType GetBTNodeType() const { return m_eBTNodeType; }

		const std::wstring& GetBTNodeName() { return m_wstrBTNodeName; }

		// �θ� ���
		void SetParentBTNode(const std::shared_ptr<CBTNode>& _spParentBTNode) { m_spParentBTNode = _spParentBTNode; }

		const shared_ptr<CBTNode>& GetParentBTNode() const { return m_spParentBTNode; }

		// ��Ʈ ���
		void SetRootBTNode(const std::shared_ptr<CBTNode>& _spRootBTNode) { m_spRootBTNode = _spRootBTNode; }

		const shared_ptr<CBTNode>& GetRootBTNode() const { return m_spRootBTNode; }

		// ���� ������Ʈ
		void SetOwnerGameObject(const std::shared_ptr<CGameObject>& _spOwnerGameObject) { m_wpOwnerGameObject = _spOwnerGameObject; }

		// ��Ʈ ��尡 ���� ���� �ڽ� ��� �ε���
		virtual int32 GetCurRootBasedChildNodeIndex() const { return -1; };

	/********************
		Data Members
	********************/
	protected:
		// ��Ʈ ��� ����
		bool m_bRoot = false;

		// �� ��带 ���� �ִ� ���� ������Ʈ
		std::weak_ptr<CGameObject> m_wpOwnerGameObject;

		// ��Ʈ ���
		std::shared_ptr<CBTNode> m_spRootBTNode = nullptr;

		// �θ� ���
		std::shared_ptr<CBTNode> m_spParentBTNode = nullptr;

		// ��� �̸�
		std::wstring m_wstrBTNodeName = L"";

		// ��� Ÿ��
		EBTNodeType m_eBTNodeType = EBTNodeType::ENUM_END;

		// ��� ���
		EBTNodeResult m_eCurBTNodeResult = EBTNodeResult::ENUM_END;

		// ���ڷ����� ����
		std::vector<std::shared_ptr<CDecoratorNode>> m_vecDecoratorNodes;

		// �����̺�� ����
		std::vector<std::shared_ptr<CBTNode>> m_vecBTNodes;

		// �����̺�� ������Ʈ
		std::weak_ptr<CBehaviorTreeComponent> m_wpBehaviorTreeComponent;
	};
}
