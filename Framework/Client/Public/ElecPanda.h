#pragma once
#include "Monster.h"
#include "ProjectileEffectGroup.h"

namespace Engine
{
	class CSphereComponent;
	class CColliderComponent;
	class CTrailComponent;
	class CBone;
}

namespace Client
{
	class CUIManager;

	enum class EElecPandaEffectSlots
	{
		ELECTRIC_ARROW,
		ELECTRIC_BALL,
		LIGHTING,
		ENUM_END
	};

	/********************
		Descriptor
	********************/
	struct FElecPandaDesc final : public FMonsterDesc
	{
	};

	class CElecPanda : public CMonster
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CElecPanda() = default;

		virtual ~CElecPanda() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CElecPanda> Create(const std::shared_ptr<FElecPandaDesc>& _spElecPandaDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		void InitElecPanda(const std::shared_ptr<FElecPandaDesc>& _spElecPandaDesc);

		void InitMeshComponent();

		void InitColliderComponent();

		void InitBehaviorTreeComponent();

		void InitTrailCombinedBones();

		void InitTrailComponent();

		void InitProjectile();

	public:
		virtual void LookAt(const Vector3& _vPosition) override;

		virtual void Turn(const Vector3& _vAxis, float _fAngularSpeed) override;

		virtual void MoveForward(float _fSpeed) override;

		virtual void MoveBackward(float _fSpeed) override;

		virtual void MoveRight(float _fSpeed) override;

		virtual void MoveLeft(float _fSpeed) override;

		virtual void MoveUp(float _fSpeed) override;

		virtual void MoveDown(float _fSpeed) override;

	public:
		virtual void UpdateBlackboardValues() override;

		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	public:
		void ActiveEffectObject(EElecPandaEffectSlots _eSlot, const Vector3& _vStartPos, const Vector3& _vDestPos, shared_ptr<CCreature> _spTarget);

		void OnLNailTrails(float _fActiveTime);

		void OnRNailTrails(float _fActiveTime);

		void OffAllNailTrails();

		void TickTrail(float _fTrailSize);

		void CheckTrailActiveTime(float _fDeltaSeconds);

	protected:
		virtual void PlaySoundWithID(const FAnimNotifyData& _tNotifyData) override;

	/********************
		Getter/Setter
	********************/
	public:
		virtual std::shared_ptr<CColliderComponent> GetBodyColliderComponent() override;

		virtual std::shared_ptr<CColliderComponent> GetAttackColliderComponent() override;

		virtual void SetLinearVelocity(const PxVec3& _vLinearVelocity) override;

		virtual void SetAngularVelocity(const PxVec3& _vLinearVelocity) override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CSphereComponent> m_spBodySphereComponent = nullptr;

		std::shared_ptr<CSphereComponent> m_spAttackSphereComponent = nullptr;

	private:
		float m_fElectricArrowGeneratorLifeTime_Left = 0.0f;

		float m_fElectricArrowGeneratorLifeTime_Right = 0.0f;

		float m_fElectricArrowGeneratorCreateTime_Left = 0.0f;

		float m_fElectricArrowGeneratorCreateTime_Right = 0.0f;

		std::unordered_map<int32, std::vector<std::shared_ptr<CProjectileEffectGroup>>> m_umapProjectileEffectGroups;

		// ∆Æ∑π¿œ
		std::array<std::weak_ptr<CBone>, 3> m_arrLeftNailBones;

		std::array<std::weak_ptr<CBone>, 3> m_arrRightNailBones;

		std::array<std::shared_ptr<CTrailComponent>, 3> m_arrLeftNailTrails;

		std::array<std::shared_ptr<CTrailComponent>, 3> m_arrRightNailTrails;

		bool m_bOnTrail = false;

		float m_fTrailActiveTime = 1.0f;

		float m_fTrailTimeAcc = 0.0f;

	private:
		std::weak_ptr<CUIManager> m_wpUIManager;
	};
}
