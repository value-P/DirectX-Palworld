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
	struct FTaskSleepEndDesc : public FBTNodeDesc
	{
		int32 m_iAnimationID = -1;
	};

	class CTaskSleepEnd : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskSleepEnd() = default;

		virtual ~CTaskSleepEnd() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskSleepEnd> Create(const std::shared_ptr<FTaskSleepEndDesc>& _spTaskSleepEndDesc);

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
		int32 m_iAnimationID = -1;

		std::optional<Vector3> m_vPrePosition = nullopt;
	};
}
