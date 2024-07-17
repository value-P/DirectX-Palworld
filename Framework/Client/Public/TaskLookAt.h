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
	class CPlayer;

	/********************
		Description
	********************/
	struct FTaskLookAtDesc : public FBTNodeDesc
	{
		std::shared_ptr<CCreature> m_spTarget = nullptr;

		float m_fMaxLerpDeltaSeconds = 2.0f;

		int32 m_iAnimationID = -1;
	};

	class CTaskLookAt final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskLookAt() = default;

		virtual ~CTaskLookAt() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskLookAt> Create(const std::shared_ptr<FTaskLookAtDesc>& _spTaskLookAtDesc);

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
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::weak_ptr<CCreature> m_wpTarget;

	private:
		int32 m_iAnimationID = -1;

	private:
		float m_fSumLerpDeltaSeconds = 0.0f;

		float m_fMaxLerpDeltaSeconds = 2.0f;

		std::optional<Vector3> m_vForward = std::nullopt;
	};
}
