#pragma once
#include "Component.h"

namespace Engine
{
    class CBTNode;
	class CCompositeNode;

	/********************
		Description
	********************/
	struct ENGINE_DLL FBTComponentDesc : public CComponent::FComponentDesc
	{
		std::shared_ptr<CBTNode> m_spRootNode = nullptr;

		bool m_bBehaveTree = false;
	};

    class ENGINE_DLL CBehaviorTreeComponent final : public CComponent
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CBehaviorTreeComponent() = default;

		virtual ~CBehaviorTreeComponent() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CBehaviorTreeComponent> Create(const std::shared_ptr<FBTComponentDesc>& _spBTComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

		virtual HRESULT BindVariable() override;
		
	/********************
		Methods
	********************/
	public:
		void ResetBTNodeResult();

		HRESULT ResetBTNodeResult(const std::wstring& _wstrBTNodeName);

		HRESULT ResetBTNodeResult(const std::shared_ptr<CBTNode>& _spBTRootNode);

		HRESULT ResetBehindBTNodeResult();

		HRESULT AddBTNode(const std::shared_ptr<CBTNode>& _spBTNode);

		HRESULT AddRootBTNode(const std::shared_ptr<CBTNode>& _spBTNode);

	/********************
		Getter/Setter
	********************/
	public:
		void SetBehaveTree(bool _bBehaveTree) { m_bBehaveTree = _bBehaveTree; }

		bool IsBehaveTree() const { return m_bBehaveTree; }

		void SetRootNode(const std::shared_ptr<CBTNode>& _spBTRootNode) { m_spRootNode = _spBTRootNode; }

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CBTNode> m_spRootNode = nullptr;

		// ��� �����̺�� Ʈ�� ����
		std::unordered_map<std::wstring, std::shared_ptr<CBTNode>> m_umapBTNodes;

		// ��Ʈ�� ���� �ڽ� ����
		std::vector<std::shared_ptr<CBTNode>> m_vecRootBTNodes;

		bool m_bBehaveTree = false;
    };
}
