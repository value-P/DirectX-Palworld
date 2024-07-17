#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CSoundManager;
	class CMeshComponent;
}

namespace Client
{
	class CCreature;
	class CMonster;

	/********************
		Description
	********************/
	struct FTaskAttackLoopDesc : public FBTNodeDesc
	{
		std::shared_ptr<CCreature> m_spTarget = nullptr;
		
		int32 m_iAnimationID = -1;
	};

	class CTaskAttackLoop final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskAttackLoop() = default;

		virtual ~CTaskAttackLoop() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskAttackLoop> Create(const std::shared_ptr<FTaskAttackLoopDesc>& _spTaskAttackLoopDesc);

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

		std::weak_ptr<CSoundManager> m_wpSoundManager;

	private:
		int32 m_iAnimationID = -1;

	private:
		float m_fSumAttackDeltaSeconds = 0.0f;

		const float m_fMaxAttackDeltaSeconds = 3.0f;

		std::optional<Vector3> m_vTargetDirection = nullopt;
	};
}