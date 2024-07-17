#pragma once
#include "Monster.h"

namespace Engine
{
    class CSphereComponent;
	class CColliderComponent;
}

namespace Client
{
	/********************
		Descriptor
	********************/
	struct FHedgeHogDesc : public FMonsterDesc
	{
	};

    class CHedgeHog final : public CMonster
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHedgeHog() = default;

		virtual ~CHedgeHog() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHedgeHog> Create(const std::shared_ptr<FHedgeHogDesc>& _spHedgeHogDesc);

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
		void InitHedgeHog(const shared_ptr<FHedgeHogDesc>& _spHedgeHogDesc);

		void InitMeshComponent();

		void InitColliderComponent();

		void InitBehaviorTreeComponent(const std::shared_ptr<FHedgeHogDesc>& _spHedgeHogDesc);

	private:
		void InitializeWildPalBT(const std::shared_ptr<FHedgeHogDesc> _spHedgeHogDesc, bool _bInit = true);

		void InitializePartnerPalBT(const std::shared_ptr<FHedgeHogDesc> _spHedgeHogDesc, bool _bInit = true);

		void InitializeSlavePalBT(const std::shared_ptr<FHedgeHogDesc> _spHedgeHogDesc, bool _bInit = true);

		virtual HRESULT ReInitializePalBT(EMonsterAIType _eMonsterAIType) override;

	public:
		virtual void LookAt(const Vector3& _vPosition) override;

		virtual void Turn(const Vector3& _vAxis, float _fAngularSpeed) override;

		virtual void MoveForward(float _fSpeed) override;

		virtual void MoveBackward(float _fSpeed) override;

		virtual void MoveRight(float _fSpeed) override;

		virtual void MoveLeft(float _fSpeed) override;

		virtual void MoveUp(float _fSpeed) override;

		virtual void MoveDown(float _fSpeed) override;

		virtual void SetPosition(const Vector3& _vPosition) override;

		virtual const std::optional<Vector3> GetPosition() const override;

	public:
		virtual void UpdateBlackboardValues() override;

		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	/********************
		Getter/Setter
	********************/
	public:
		virtual void SetAllyCollisionLayer() override;

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
    };
}