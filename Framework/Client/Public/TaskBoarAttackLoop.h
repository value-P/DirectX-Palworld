#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CMeshComponent;
	class CSoundManager;
}

namespace Client
{
	class CEffectManager;
	class CEffectGroup;
	class CCreature;
	class CMonster;

	/********************
		Description
	********************/
	struct FTaskBoarAttackLoopDesc : public FBTNodeDesc
	{
		std::shared_ptr<CCreature> m_spTarget = nullptr;

		int32 m_iAnimationID = -1;
	};

	class CTaskBoarAttackLoop final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskBoarAttackLoop() = default;

		virtual ~CTaskBoarAttackLoop() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskBoarAttackLoop> Create(const std::shared_ptr<FTaskBoarAttackLoopDesc>& _spTaskBoarAttackLoopDesc);

		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

	public:
		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteTaskNode(float _fDeltaSecond) override;

		virtual void Release() override;

	/********************
		Data Members
	********************/
	public:
		void OffRushEffect();

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CEffectManager> m_wpEffectManager;

		std::weak_ptr<CSoundManager> m_wpSoundManager;

		std::shared_ptr<CMonster> m_spMonster = nullptr;
		
		std::weak_ptr<CCreature> m_wpTarget;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

	private:
		int32 m_iAnimationID = -1;

	private:
		float m_fSumAttackDeltaSeconds = 0.0f;

		const float m_fMaxAttackDeltaSeconds = 1.5f;

		std::optional<Vector3> m_vTargetDirection = nullopt;

		std::weak_ptr<CEffectGroup> m_wpRushEffect;

		EChannelType m_eChannelType = EChannelType::ENUM_END;
	};
}
