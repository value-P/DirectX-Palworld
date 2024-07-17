#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CPhysXManager;
	class CSoundManager;
	class CMeshComponent;
	class CColliderComponent;
}

namespace Client
{
	class CUIManager;
	class CEffectManager;
	class CCreature;
	class CPlayer;
	class CElecPanda;
	class CEffectGroup;
	class CProjectileEffectGroup;
	class CRedRazor;

	enum class EElecPandaSkillType
	{
		SNIPE,			// 저격			-> 레이저 위치로 레이 쏴서 플레이어 충돌됐으면 takedamage호출
		DISCHARGE,		// 방전			-> 콜라이더로 플레이어 공격 + 넉백 
		MELEE,			// 근접			-> 콜라이더로 플레이어 공격
		ELECTRIC_ARROW,	// 전기 화살
		ELECTRIC_BALL,	// 전기 구슬ㄴ
		LIGHTNING,		// 낙뢰
		ENUM_END
	};

	/********************
		Description
	********************/
	struct FTaskElecPandaAttackLoopDesc : public FBTNodeDesc
	{
		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

		// 시작 애니메이션
		int32 m_iStartAnimation = -1;

		// 시작 루프 애니메이션
		int32 m_iStartLoopAnimation = -1;
		float m_fMaxStartLoopDeltaSeconds = 0.0f;	// 최대 시간

		// 액션 애니메이션
		int32 m_iAction00AnimationID = -1;			// 애니메이션 ID 1
		int32 m_iAction01AnimationID = -1;			// 애니메이션 ID 2
		int32 m_iAction02AnimationID = -1;			// 애니메이션 ID 3

		// 액션 루프 애니메이션
		int32 m_iActionLoop00AnimationID = -1;		// 애니메이션 ID 1		
		int32 m_iActionLoop01AnimationID = -1;		// 애니메이션 ID 2	
		int32 m_iActionLoop02AnimationID = -1;		// 애니메이션 ID 3
		float m_fMaxActionLoop00DeltaSeconds = 0.0f;	// 목표 시간 1
		float m_fMaxActionLoop01DeltaSeconds = 0.0f;	// 목표 시간 2
		float m_fMaxActionLoop02DeltaSeconds = 0.0f;	// 목표 시간 3

		// 종료 애니메이션
		int32 m_iEndAnimationID = -1;				// 애니메이션 ID

		EElecPandaSkillType m_eCurElecPandaSkillType = EElecPandaSkillType::ENUM_END;
	};

	class CTaskElecPandaAttackLoop final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskElecPandaAttackLoop() = default;

		virtual ~CTaskElecPandaAttackLoop() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskElecPandaAttackLoop> Create(const std::shared_ptr<FTaskElecPandaAttackLoopDesc>& _spTaskElecPandaAttackLoopDesc);

		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

	public:
		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteTaskNode(float _fDeltaSecond) override;

		virtual void Release() override;

		void InitializeRazorEffect();

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CEffectManager> m_wpEffectManager;

		std::weak_ptr<CPhysXManager> m_wpPhysXManager;

		std::weak_ptr<CSoundManager> m_wpSoundManager;

	private:
		std::shared_ptr<CElecPanda> m_spMonster = nullptr;

		std::shared_ptr<CColliderComponent> m_spAttackColliderComponent = nullptr;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;
		
		std::weak_ptr<CPlayer> m_wpPlayer;

		std::deque<Vector3> m_dqPlayerPositions;

		std::deque<Vector3> m_dqLightningPositions;

		std::weak_ptr<CEffectGroup> m_wpDischargeThunder;

		std::shared_ptr<CRedRazor> m_spRedRazor;

		std::optional<Vector3> m_vPrePosition = nullopt;

	private:
		EElecPandaSkillType m_eCurElecPandaSkillType = EElecPandaSkillType::ENUM_END;

		// start
		int32	m_iStartAnimation = -1;					// 애니메이션 ID
		bool	m_bStartCompleted = false;				// 완료 여부

		// start loop
		int32	m_iStartLoopAnimation = -1;				// 애니메이션
		float	m_fMaxStartLoopDeltaSeconds = 2.0f;		// 목표 시간
		bool	m_bStartLoopCompleted = false;			// 완료 여부

		// action
		int32 m_iAction00AnimationID	= -1;			// 애니메이션 ID
		int32 m_iAction01AnimationID	= -1;			// 애니메이션 ID
		int32 m_iAction02AnimationID	= -1;			// 애니메이션 ID
		float m_fSumLoopDeltaSeconds	= 0.0f;			// 목표 시간
		bool m_bActionCompleted			= false;		// 완료 여부

		// action loop
		int32	m_iActionLoop00AnimationID = -1;		// 애니메이션 ID		
		int32	m_iActionLoop01AnimationID = -1;		// 애니메이션 ID	
		int32	m_iActionLoop02AnimationID = -1;		// 애니메이션 ID
		float	m_fMaxActionLoop00DeltaSeconds = 1.0f;	// 목표 시간
		float	m_fMaxActionLoop01DeltaSeconds = 1.0f;	// 목표 시간
		float	m_fMaxActionLoop02DeltaSeconds = 1.0f;	// 목표 시간
		bool	m_bActionLoopCompleted = false;			// 완료 여부

		// end
		int32	m_iEndAnimationID = -1;		// 애니메이션 ID
		bool	m_bEndCompleted = false;	// 완료 여부

	private:
		float m_fSumDischargeDeltaSeconds = 0.0f;

		const float m_fMaxDischargeDeltaSeconds = 0.2f;

		float m_fSumArrowDeltaSeconds = 0.0f;

		const float m_fMaxArrowDeltaSeconds = 0.6f;

		float m_fSumLightningDeltaSeconds = 0.0f;

		const float m_fMaxLightningDeltaSeconds = 0.15f;

	private:
		std::weak_ptr<CUIManager> m_wpUIManager;
	};
}
