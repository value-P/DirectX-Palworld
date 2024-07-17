#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CSoundManager;
	class CMeshComponent;
}

namespace Client
{
	class CEffectManager;
	class CMonster;
	class CPlayer;

	/********************
		Description
	********************/
	struct FTaskNearDesc : public FBTNodeDesc
	{
		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

		int32 m_iAnimationID = -1;

		float m_fAcceptableRadius = 5.0f;
	};

	class CTaskNear final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskNear() = default;

		virtual ~CTaskNear() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskNear> Create(const std::shared_ptr<FTaskNearDesc>& _spTaskNearDesc);

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
		std::weak_ptr<CEffectManager> m_wpEffectManager;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

	private:
		int32 m_iAnimationID = -1;

	private:
		float m_fSumLerpDeltaSeconds = 0.0f;

		const float m_fMaxLerpDeltaSeconds = 3.0f;

		float m_fAcceptableRadius = 5.0f;

		std::optional<Vector3> m_vTargetDirection = nullopt;

		std::optional<Vector3> m_vPrePosition = nullopt;
	};
}
