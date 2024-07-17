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
		SNIPE,			// ����			-> ������ ��ġ�� ���� ���� �÷��̾� �浹������ takedamageȣ��
		DISCHARGE,		// ����			-> �ݶ��̴��� �÷��̾� ���� + �˹� 
		MELEE,			// ����			-> �ݶ��̴��� �÷��̾� ����
		ELECTRIC_ARROW,	// ���� ȭ��
		ELECTRIC_BALL,	// ���� ������
		LIGHTNING,		// ����
		ENUM_END
	};

	/********************
		Description
	********************/
	struct FTaskElecPandaAttackLoopDesc : public FBTNodeDesc
	{
		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

		// ���� �ִϸ��̼�
		int32 m_iStartAnimation = -1;

		// ���� ���� �ִϸ��̼�
		int32 m_iStartLoopAnimation = -1;
		float m_fMaxStartLoopDeltaSeconds = 0.0f;	// �ִ� �ð�

		// �׼� �ִϸ��̼�
		int32 m_iAction00AnimationID = -1;			// �ִϸ��̼� ID 1
		int32 m_iAction01AnimationID = -1;			// �ִϸ��̼� ID 2
		int32 m_iAction02AnimationID = -1;			// �ִϸ��̼� ID 3

		// �׼� ���� �ִϸ��̼�
		int32 m_iActionLoop00AnimationID = -1;		// �ִϸ��̼� ID 1		
		int32 m_iActionLoop01AnimationID = -1;		// �ִϸ��̼� ID 2	
		int32 m_iActionLoop02AnimationID = -1;		// �ִϸ��̼� ID 3
		float m_fMaxActionLoop00DeltaSeconds = 0.0f;	// ��ǥ �ð� 1
		float m_fMaxActionLoop01DeltaSeconds = 0.0f;	// ��ǥ �ð� 2
		float m_fMaxActionLoop02DeltaSeconds = 0.0f;	// ��ǥ �ð� 3

		// ���� �ִϸ��̼�
		int32 m_iEndAnimationID = -1;				// �ִϸ��̼� ID

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
		int32	m_iStartAnimation = -1;					// �ִϸ��̼� ID
		bool	m_bStartCompleted = false;				// �Ϸ� ����

		// start loop
		int32	m_iStartLoopAnimation = -1;				// �ִϸ��̼�
		float	m_fMaxStartLoopDeltaSeconds = 2.0f;		// ��ǥ �ð�
		bool	m_bStartLoopCompleted = false;			// �Ϸ� ����

		// action
		int32 m_iAction00AnimationID	= -1;			// �ִϸ��̼� ID
		int32 m_iAction01AnimationID	= -1;			// �ִϸ��̼� ID
		int32 m_iAction02AnimationID	= -1;			// �ִϸ��̼� ID
		float m_fSumLoopDeltaSeconds	= 0.0f;			// ��ǥ �ð�
		bool m_bActionCompleted			= false;		// �Ϸ� ����

		// action loop
		int32	m_iActionLoop00AnimationID = -1;		// �ִϸ��̼� ID		
		int32	m_iActionLoop01AnimationID = -1;		// �ִϸ��̼� ID	
		int32	m_iActionLoop02AnimationID = -1;		// �ִϸ��̼� ID
		float	m_fMaxActionLoop00DeltaSeconds = 1.0f;	// ��ǥ �ð�
		float	m_fMaxActionLoop01DeltaSeconds = 1.0f;	// ��ǥ �ð�
		float	m_fMaxActionLoop02DeltaSeconds = 1.0f;	// ��ǥ �ð�
		bool	m_bActionLoopCompleted = false;			// �Ϸ� ����

		// end
		int32	m_iEndAnimationID = -1;		// �ִϸ��̼� ID
		bool	m_bEndCompleted = false;	// �Ϸ� ����

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
