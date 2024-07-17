#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
	class CColliderComponent;
}

namespace Client
{
	class CMonster;

	struct FTaskWithdrawDesc : public FBTNodeDesc
	{
		int32 m_iAnimationID = -1;

		float m_fMinDistance = 3.0f;
	};

	class CTaskWithdraw final : public CTaskNode
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CTaskWithdraw() = default;

		virtual ~CTaskWithdraw() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CTaskWithdraw> Create(const std::shared_ptr<FTaskWithdrawDesc>& _spTaskWithdrawDesc);

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
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::shared_ptr<CColliderComponent> m_spColliderComponent = nullptr;

		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::weak_ptr<CGameObject> m_wpTarget;

	private:
		int32 m_iAnimationID = -1;

	private:
		const float m_fPower = 150.0f;

		const float m_fMaxWithdrawDeltaSeconds = 2.0f;

		float m_fSumWithdrawDeltaSeconds = 0.0f;

		float m_fMinDistance = 5.0f;

		bool m_bWithDrawRequired = true;
	};
}