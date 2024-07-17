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
	class CEffectManager;
	class CEffectGroup;

	/********************
		Description
	********************/
	struct FTaskSheepBallAttackLoopDesc : public FBTNodeDesc
	{
		std::shared_ptr<CCreature> m_spTarget = nullptr;

		int32 m_iAnimationID = -1;
	};

	class CTaskSheepballAttackLoop final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskSheepballAttackLoop() = default;

		virtual ~CTaskSheepballAttackLoop() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskSheepballAttackLoop> Create(const std::shared_ptr<FTaskSheepBallAttackLoopDesc>& _spTaskSheepBallAttackLoopDesc);

		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

	public:
		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteTaskNode(float _fDeltaSecond) override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		void OffEffects();

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMonster> m_spMonster = nullptr;

		std::weak_ptr<CCreature> m_wpTarget;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::weak_ptr<CEffectManager> m_wpEffectManager;

		std::weak_ptr<CSoundManager> m_wpSoundManager;

	private:
		int32 m_iAnimationID = -1;

	private:
		float m_fSumAttackDeltaSeconds = 0.0f;

		const float m_fMaxAttackDeltaSeconds = 5.0f;

		float m_fSumLerpDeltaSeconds = 0.0f;

		const float m_fMaxLerpDeltaSeconds = 5.0f;

	private:
		// »∏¿¸ ¿Ã∆Â∆Æ
		std::weak_ptr<CEffectGroup> m_wpSpinEffect;

		uint32 m_iEffectCount = 0;

		EChannelType m_eChannelType = EChannelType::ENUM_END;
	};
}
