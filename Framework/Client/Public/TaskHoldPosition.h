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
	struct FTaskHoldPositionDesc : public FBTNodeDesc
	{
		int32 m_iAnimationID = -1;
	};

	class CTaskHoldPosition : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskHoldPosition() = default;

		virtual ~CTaskHoldPosition() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskHoldPosition> Create(const std::shared_ptr<FTaskHoldPositionDesc>& _spTaskHoldPositionDesc);

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

	private:
		int32 m_iAnimationID = -1;

		std::optional<Vector3> m_vPrePosition = nullopt;
	};
}
