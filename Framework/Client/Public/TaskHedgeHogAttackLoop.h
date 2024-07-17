#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
	class CColliderComponent;
}

namespace Client
{
	class CEffectManager;
	class CMonster;

	/********************
		Description
	********************/
	struct FTaskHedgeHogAttackLoopDesc : public FBTNodeDesc
	{
		int32 m_iActionAnimationID = -1;

		int32 m_iActionLoopAnimationID = -1;
	};

	class CTaskHedgeHogAttackLoop final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskHedgeHogAttackLoop() = default;

		virtual ~CTaskHedgeHogAttackLoop() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskHedgeHogAttackLoop> Create(const std::shared_ptr<FTaskHedgeHogAttackLoopDesc>& _spTaskAttackLoopDesc);

		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

	public:
		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteTaskNode(float _fDeltaSecond) override;

		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

	private:
		int32 m_iActionAnimationID = -1;

		int32 m_iActionLoopAnimationID = -1;

		bool m_bActionCompleted = false;

		float m_fSumAttackDeltaSeconds = 0.0f;

		const float m_fMaxAttackDeltaSeconds = 2.0f;

	private:
		std::weak_ptr<CEffectManager> m_wpEffectManager;

		std::weak_ptr<CColliderComponent> m_wpColliderComponent;
	};
}