#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CGameObject;
	class CMeshComponent;
}

namespace Client
{
	class CMonster;
	class CPlayer;

	/********************
		Description
	********************/
	struct FTaskTurnDesc : public FBTNodeDesc
	{
		std::shared_ptr<CGameObject> m_spTarget = nullptr;

		int32 m_iAnimationID = -1;
	};

	class CTaskTurn : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskTurn() = default;

		virtual ~CTaskTurn() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskTurn> Create(const std::shared_ptr<FTaskTurnDesc>& _spTaskTurnDesc);

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

		std::weak_ptr<CGameObject> m_wpTarget;

	private:
		int32 m_iAnimationID = -1;

	private:
		float m_fSumTurnDeltaSeconds = 0.0f;

		const float m_fMaxTurnDeltaSeconds = 2.0f;

		std::optional<Vector3> m_vForward = std::nullopt;

		std::optional<Vector3> m_vPrePosition = nullopt;
	};
}
