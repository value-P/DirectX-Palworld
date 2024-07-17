#pragma once
#include "EffectGroup.h"
#include "TrailComponent.h"

namespace Engine
{
	class CTrailComponent;
}

namespace Client
{
	class CEffectManager;
	class CCreature;

	struct FProjectileEffectGroupDesc : public FEffectGroupDesc
	{
		std::shared_ptr<CCreature> m_spOwner = nullptr;

		std::shared_ptr<CCreature> m_spTarget = nullptr;

		EMonsterType m_eOwnerMonsterType = EMonsterType::ENUM_END;

		int32 m_iProjectileSlot = -1;

		bool m_bTrail = false;

		std::shared_ptr<FTrailComponentDesc> m_spTrailCompoonentDesc;
	};

	class CProjectileEffectGroup final : public CEffectGroup
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CProjectileEffectGroup() = default;
		virtual ~CProjectileEffectGroup() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CProjectileEffectGroup> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);
		static std::shared_ptr<CProjectileEffectGroup> CreateFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);
		HRESULT InitializeFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	public:
		HRESULT InitializeTrailCompoonent(const std::shared_ptr<FTrailComponentDesc>& _spTrailComponentDesc);

	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent);
		
	/********************
		Methods
	********************/
	public:
		void ClearTrail();
		void PlayHitEffect(uint32 _iSlotIndex);
		void EndLaunchTime();

	public:
		int32 ProgressPankyProjectile(float _fDeltaSeconds);
		int32 ProgressKitsunebiProjectile(float _fDeltaSeconds);
		int32 ProgressDreamDemonProjectile(float _fDeltaSeconds);
		int32 ProgressBerryGoatProjectile(float _fDeltaSeconds);
		int32 ProgressGreenMammothProjectile(float _fDeltaSeconds);
		int32 ProgressElecPandaProjectile(float _fDeltaSeconds);

	/********************
		Getter / Setter
	********************/
	public:
		void SetTarget(std::shared_ptr<CCreature> _spTarget) { m_wpTarget = _spTarget; }
		std::shared_ptr<CCreature> GetTarget() 
		{
			if (!m_wpTarget.expired())
			{ 
				return m_wpTarget.lock(); 
			}
			else 
			{
				return nullptr;
			}
		}

		void SetSourcePos(const Vector3 _vSourcePos) { m_vSourcePos = _vSourcePos; }
		void SetDestinationPos(const Vector3 _vDestinationPos) { m_vDestinationPos = _vDestinationPos; }
		void SetLaunchDir(const Vector3& _vLaunchDir) { m_vLaunchDir = _vLaunchDir; }

	/********************
		Data Members
	********************/
	private:
		// 트레일
		std::shared_ptr<CTrailComponent> m_spTrailComponent = nullptr;

		// 이펙트 매니저
		std::weak_ptr<CEffectManager> m_wpEffectManager;

	private:
		// 타겟
		std::weak_ptr<CCreature> m_wpTarget;

		// 투사체의 주인
		std::weak_ptr<CCreature> m_wpOwner;
		EMonsterType m_eOwnerMonsterType = EMonsterType::ENUM_END;

		// 투사체 종류 구분용
		int32 m_iProjectileSlot = -1;

		// 시작 지점
		Vector3 m_vSourcePos = Vector3(0.f);

		// 목표 지점
		Vector3 m_vDestinationPos = Vector3(0.f);

	private:
		Vector3 m_vGravityVector = Vector3(0.f, 1.f, 0.f);

		Vector3 m_vLaunchDir = Vector3(0.f);

		float m_fLaunchTimeAcc = 0.f;
	};
}