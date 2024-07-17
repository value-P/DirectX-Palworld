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
	struct FTaskDamageDesc : public FTaskNodeDesc
	{
		std::shared_ptr<CCreature> m_spTarget = nullptr;

		int32 m_iAnimationID = -1;
	};

    class CTaskDamaged final : public CTaskNode
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskDamaged() = default;
	
		virtual ~CTaskDamaged() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskDamaged> Create(const std::shared_ptr<FTaskDamageDesc>& _spTaskDamageDesc);

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
    };
}