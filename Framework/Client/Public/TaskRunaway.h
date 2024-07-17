#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
}

namespace Client
{
	class CCreature;
	class CPlayer;
	class CMonster;

	struct FTaskRunawayDesc : public FBTNodeDesc
	{
		std::shared_ptr<CCreature> m_spTarget = nullptr;

		int32 m_iAnimationID = -1;

		float m_fMaxRunawayDeltaSeconds = 0.0f;
	};

	class CTaskRunaway final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskRunaway() = default;

		virtual ~CTaskRunaway() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskRunaway> Create(const std::shared_ptr<FTaskRunawayDesc>& _spTaskRunawayLoopDesc);

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
		
		std::weak_ptr<CCreature> m_wpTarget;

		std::weak_ptr<CPlayer> m_wpPlayer;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

	private:
		int32 m_iAnimationID = -1;

	private:
		float m_fSumLerpDeltaSeconds = 0.0f;

		const float m_fMaxLerpDeltaSeconds = 3.0f;

		float m_fSumRunawayDeltaSeconds = 0.0f;

		float m_fMaxRunawayDeltaSeconds = 5.0f;

		std::optional<Vector3> m_vTargetDirection = nullopt;
	};
}
