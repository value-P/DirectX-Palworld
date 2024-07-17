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
	struct FTaskIdleDesc : public FBTNodeDesc
	{
		int32 m_iAnimationID = -1;

		float m_fMaxIdleDeltaSeconds = 10.0f;
	};

	class CTaskIdle : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskIdle() = default;

		virtual ~CTaskIdle() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskIdle> Create(const std::shared_ptr<FTaskIdleDesc>& _spTaskIdleDesc);

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

	private:
		float m_fMaxIdleDeltaSeconds = 10.0f;

		float m_fSumIdleDeltaSeconds = 0.0f;

		std::optional<Vector3> m_vPrePosition = nullopt;
	};
}
