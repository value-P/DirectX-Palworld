#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
}

namespace Client
{
	class CMonster;

	/********************
		Description
	********************/
	struct FTaskAttackEndDesc : public FBTNodeDesc
	{
		int32 m_iAttackEndAnimationID = -1;

		int32 m_iGetUpAnimationID = -1;
	};

	class CTaskAttackEnd : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskAttackEnd() = default;

		virtual ~CTaskAttackEnd() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskAttackEnd> Create(const std::shared_ptr<FTaskAttackEndDesc>& _spTaskAttackEndDesc);

		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

	public:
		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteTaskNode(float _fDeltaSecond) override;

		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

	private:
		int32 m_iAttackEndAnimationID = -1;

		int32 m_iGetUpAnimationID = -1;
	};
}