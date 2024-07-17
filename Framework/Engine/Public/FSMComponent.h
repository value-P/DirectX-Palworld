#pragma once
#include "Component.h"
#include "FSMNode.h"

namespace Engine
{
	struct ENGINE_DLL FFSMComponentDesc : public CComponent::FComponentDesc
	{
		int32 m_iNumState = 0;

		int32 m_iInitialState = 0;
	};

	class ENGINE_DLL CFSMComponent final : public CComponent
	{
	/********************
		Enum
	********************/
		enum class ENodeState { OnEnter, Running, OnExit };

	/********************
		Ctor/Dtor
	********************/
	public:
		CFSMComponent() = default;

		virtual ~CFSMComponent() = default;

	/********************
		Getter/Setter
	********************/
	public:
		void AddState(int32 _iState, shared_ptr<CFSMNode> _spNode);

		int32 GetState() const { return m_iCurrentState; }

		int32 GetPrevState() const { return m_iPrevState; }

		int32 GetNextState() const { return m_iNextState; }

	/********************
		Events
	********************/
	public:
		static shared_ptr<CFSMComponent> Create(const std::shared_ptr<FComponentDesc>& _spComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

		virtual HRESULT BeginPlay() override { return S_OK; }

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override { return 0; }

		virtual HRESULT EndPlay() override { return S_OK; }

		virtual void Render(int32 _iShaderPath) override { }

		virtual void Release() override;

		virtual HRESULT BindVariable() override { return S_OK; }

	/********************
		Methods
	********************/
	public:
		void ChangeState(int32 _iNextState);

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CMeshComponent> m_wpCombinedMeshComponent;

		ENodeState m_eNodeState = ENodeState::OnEnter;

		std::vector<std::shared_ptr<CFSMNode>> m_vecNode;

		int32 m_iNumState = 0;

		int32 m_iNextState = 0;
		
		int32 m_iCurrentState = 0;

		int32 m_iPrevState = 0;
	};
}
