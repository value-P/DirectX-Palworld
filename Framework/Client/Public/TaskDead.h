#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CPanel;
	class CSoundManager;
	class CMeshComponent;
	class CAnimationImageComponent;
}

namespace Client
{
	class CPalBoxSystem;
	class CInventorySystem;
	class CEffectManager;
	class CMonster;

	/********************
		Description
	********************/
	struct FTaskDeadDesc : public FTaskNodeDesc
	{
		int32 m_iAnimationID = -1;
	};

	class CTaskDead final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskDead() = default;

		virtual ~CTaskDead() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskDead> Create(const std::shared_ptr<FTaskDeadDesc>& _spTaskDeadDesc);

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
		std::weak_ptr<CMonster> m_wpMonster;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::weak_ptr<CEffectManager> m_wpEffectManager;


		// 24-05-22 이동준 추가
		std::weak_ptr<CSoundManager> m_wpSoundManager;

		std::weak_ptr<CPalBoxSystem> m_wpPalBoxSystem;

		std::weak_ptr<CInventorySystem> m_wpInventorySystem;

		std::weak_ptr<CPanel>	m_wpGetItemParentPanel;

		std::weak_ptr<CAnimationImageComponent>	m_wpGetItemAnimationImageComponentSlot0;

		std::weak_ptr<CAnimationImageComponent>	m_wpGetItemAnimationImageComponentSlot1;

	private:
		int32 m_iAnimationID = -1;

		EMonsterType m_eMonsterType = EMonsterType::ENUM_END;

	private:
		float m_fSumDeadDeltaSeconds = 0.0f;

		const float m_fMaxDeadDeltaSeconds = 2.0f;
	};
}
