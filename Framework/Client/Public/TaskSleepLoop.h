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
	struct FTaskSleepLoopDesc : public FBTNodeDesc
	{
		int32 m_iAnimationID = -1;
	};

	class CTaskSleepLoop : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskSleepLoop() = default;

		virtual ~CTaskSleepLoop() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskSleepLoop> Create(const std::shared_ptr<FTaskSleepLoopDesc>& _spTaskSleepLoopDesc);

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
		std::shared_ptr<CMonster> m_spMonster;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

	private:
		int32 m_iAnimationID = -1;

		std::optional<Vector3> m_vPrePosition = nullopt;
	};
}