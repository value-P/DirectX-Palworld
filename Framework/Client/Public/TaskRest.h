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
	struct FTaskRestDesc : public FBTNodeDesc
	{
		int32 m_iAnimationID = -1;

		float m_fMaxRestDeltaSeconds = 10.0f;
	};
	
	class CTaskRest : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskRest() = default;

		virtual ~CTaskRest() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskRest> Create(const std::shared_ptr<FTaskRestDesc>& _spTaskRestDesc);

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

	private:
		float m_fMaxRestDeltaSeconds = 10.0f;

		float m_fSumRestDeltaSeconds = 0.0f;
	};
}
