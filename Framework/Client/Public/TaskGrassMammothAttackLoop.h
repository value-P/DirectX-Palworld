#pragma once
#include "TaskNode.h"

namespace Engine
{
	class CSoundManager;
	class CBoxComponent;
	class CMeshComponent;
}

namespace Client
{
	class CCreature;
	class CMonster;
	class CGrassMammoth;
	class CEffectManager;
	class CProjectileEffectGroup;

	enum class EGrassMammothSkillType
	{
		DASH,
		POWER_BOMB,
		GRASS_TORNADO,
		EARTH_IMPACT,
		SPINE_VINE,
		ENUM_END
	};

	/********************
		Description
	********************/
	struct FTaskGrassMammothAttackLoopDesc : public FBTNodeDesc
	{
		ECollisionLayer m_eProjectileCollisionLayer = ECollisionLayer::ENUM_END;

		ECollisionFlag m_eProjectileCollsionFlag = ECollisionFlag::ENUM_END;

		std::shared_ptr<CCreature> m_spTarget = nullptr;

		int32 m_iStartAnimationID = -1;

		int32 m_iStartLoopAnimationID = -1;

		int32 m_iActionAnimationID = -1;

		int32 m_iActionLoopAnimationID = -1;

		int32 m_iEndAnimationID = -1;

		int32 m_iEarthImpactAnimationID = -1;

		int32 m_iRunAnimationID = -1;

		EGrassMammothSkillType m_eCurGrassMammothSkillType = EGrassMammothSkillType::ENUM_END;
	};

	class CTaskGrassMammothAttackLoop final : public CTaskNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTaskGrassMammothAttackLoop() = default;

		virtual ~CTaskGrassMammothAttackLoop() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTaskGrassMammothAttackLoop> Create(const std::shared_ptr<FTaskGrassMammothAttackLoopDesc>& _spTaskGrassMammothAttackLoopDesc);

		virtual HRESULT Initialize(const shared_ptr<FBTNodeDesc>& _spNodeDesc) override;

	public:
		virtual void OnNodeInit(float _fDeltaSecond) override;

		virtual void OnNodeEnter(float _fDeltaSecond) override;

		virtual EBTNodeResult ExecuteTaskNode(float _fDeltaSecond) override;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	private:
		void InitializeProjectileEffectGroups(int32 _iIndex, const std::wstring& _wstrProjectileEffectGroupName, ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag);
		
		HRESULT ActivateProjectileEffectGroup(int32 _iProjectileSlot, std::shared_ptr<CCreature> _spTarget, const Vector3 _vActivatedPosition);


	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CEffectManager> m_wpEffectManager;

		std::shared_ptr<CGrassMammoth> m_spGrassMammoth = nullptr;
		
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::shared_ptr<CBoxComponent> m_spDashColliderComponent;

		std::shared_ptr<CBoxComponent> m_spEarthImpactColliderComponent;

		std::weak_ptr<CCreature> m_wpTarget;

	private:
		EGrassMammothSkillType m_eCurGrassMammothSkillType = EGrassMammothSkillType::ENUM_END;

		int32 m_iStartAnimation = -1;

		int32 m_iStartLoopAnimation = -1;

		int32 m_iActionAnimationID = -1;

		int32 m_iActionLoopAnimationID = -1;

		int32 m_iEarthImpactAnimationID = -1;

		int32 m_iRunAnimationID = -1;

		int32 m_iEndAnimationID = -1;

	private:
		float m_fCreepSpeed = 1.0f;
		
		float m_fMoveSpeed = 3.0f;

		float m_fRunSpeed = 8.0f;

	private:
		// start
		bool m_bStartCompleted = false;

		// start loop
		float m_fSumStartLoopDeltaSeconds = 0.0f;

		const float m_fMaxStartLoopDeltaSeconds = 2.0f;

		bool m_bStartLoopCompleted = false;

		// action
		bool m_bActionCompleted = false;

		// action loop
		bool m_bActionLoopCompleted = false;

		float m_fSumActionLoopDeltaSeconds = 0.0f;

		const float m_fMaxActionLoopDeltaSeconds = 2.0f;

		// end
		bool m_bEndCompleted = false;

	private:
		// dash counter
		float m_fSumDashDeltaSeconds = 0.0f;

		const float m_fMaxDashDeltaSeconds = 4.0f;

	private:
		// earth impact counter
		float m_fSumEarthImpactDeltaSeconds = 0.0f;

		const float m_fMaxEarthImpactDeltaSecondsEnter = 2.0f;

		const float m_fMaxEarthImpactDeltaSecondsExit = 2.3f;

		bool m_bEarthImpact = false;

	private:
		std::unordered_map<int32, std::vector<std::shared_ptr<CProjectileEffectGroup>>> m_umapProjectileEffectGroups;

		std::weak_ptr<CSoundManager> m_wpSoundManager;
	};
}
