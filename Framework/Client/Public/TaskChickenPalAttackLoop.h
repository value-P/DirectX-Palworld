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
	struct FTaskChickenPalAttackLoopDesc : public FBTNodeDesc
	{
		std::shared_ptr<CCreature> m_spTarget = nullptr;

		int32 m_iAnimationID = -1;
	};

	class CTaskChickenPalAttackLoop final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskChickenPalAttackLoop() = default;

		virtual ~CTaskChickenPalAttackLoop() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskChickenPalAttackLoop> Create(const std::shared_ptr<FTaskChickenPalAttackLoopDesc>& _spTaskChickenPalAttackLoopDesc);

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
		std::shared_ptr<CMonster> m_spMonster = nullptr;
		
		std::weak_ptr<CCreature> m_wpTarget;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

	private:
		int32 m_iAnimationID = -1;

	private:
		std::optional<Vector3> m_vTargetDirection = nullopt;
	};
}
