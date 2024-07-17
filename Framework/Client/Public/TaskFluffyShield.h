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
	struct FTaskFluffyShieldDesc : public FTaskNodeDesc
	{
		int32 m_iAnimationID = -1;
	};

	class CTaskFluffyShield final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskFluffyShield() = default;

		virtual ~CTaskFluffyShield() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskFluffyShield> Create(const std::shared_ptr<FTaskFluffyShieldDesc>& _spTaskFluffyShieldDesc);

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

		std::shared_ptr<CMonster> m_spMonster = nullptr;
	
	private:
		float m_fSumDurationDeltaSeconds = 0.0f;

		const float m_fMaxDurationDeltaSeconds = 30.0f;

		int32 m_iAnimationID = -1;
	};
}
