#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
}

namespace Client
{
	class CCreature;
	class CMonster;

	/********************
		Description
	********************/
	struct FTaskChaseDesc : public FBTNodeDesc
	{
		std::shared_ptr<CCreature> m_spTarget = nullptr;

		int32 m_iAnimationID = -1;

		// 최대 추격 시간
		float m_fMaxChaseDeltaSeconds = 0.0f;

		float m_fAcceptableRadius = 5.0f;
	};

	class CTaskChase final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskChase() = default;

		virtual ~CTaskChase() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskChase> Create(const std::shared_ptr<FTaskChaseDesc>& _spTaskChaseDesc);

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

		std::weak_ptr<CCreature> m_wpTarget;

	private:
		int32 m_iAnimationID = -1;

	private:
		std::optional<float> m_fMaxChaseDeltaSeconds = nullopt;

		float m_fSumChaseDeltaSeconds = 0.0f;

		float m_fAcceptableRadius = 5.0f;
	};
}
