#pragma once
#include "Monster.h"

namespace Engine
{
	class CSphereComponent;
	class CBoxComponent;
	class CColliderComponent;
}

namespace Client
{
	class CUIManager;

	/********************
		Descriptor
	********************/
	struct FGrassMammothDesc final : public FMonsterDesc
	{
	};

	class CGrassMammoth : public CMonster
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CGrassMammoth() = default;

		virtual ~CGrassMammoth() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CGrassMammoth> Create(const std::shared_ptr<FGrassMammothDesc>& _spGrassMammothDesc);

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
		void InitGrassMammoth(const std::shared_ptr<FGrassMammothDesc>& _spGrassMammothDesc);

		void InitMeshComponent();

		void InitColliderComponent();

		void InitBehaviorTreeComponent(const std::shared_ptr<FGrassMammothDesc>& _spGrassMammothDesc);

	private:
		void InitializeWildPalBT(const std::shared_ptr<FGrassMammothDesc> _spGrassMammothDesc, bool _bInit = true);

		void InitializePartnerPalBT(const std::shared_ptr<FGrassMammothDesc> _spGrassMammothDesc, bool _bInit = true);

		void InitializeSlavePalBT(const std::shared_ptr<FGrassMammothDesc> _spGrassMammothDesc, bool _bInit = true);

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

		virtual void ChangeMouth(EPalMouthType _ePalMouthType) override;

	public:
		virtual void UpdateBlackboardValues() override;

		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	/********************
		Getter/Setter
	********************/
	public:
		virtual std::shared_ptr<CColliderComponent> GetBodyColliderComponent() override;

		virtual void SetLinearVelocity(const PxVec3& _vLinearVelocity) override;

		virtual void SetAngularVelocity(const PxVec3& _vLinearVelocity) override;

		std::shared_ptr<CBoxComponent> GetDashColliderComponent() { return m_spDashBoxComponent; }

		std::shared_ptr<CBoxComponent> GetEarthImpactBoxComponent() { return m_spEarthImpactBoxComponent; }

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CSphereComponent> m_spBodySphereComponent = nullptr;

		std::shared_ptr<CBoxComponent> m_spDashBoxComponent = nullptr;

		std::shared_ptr<CBoxComponent> m_spEarthImpactBoxComponent = nullptr;

	private:
		std::weak_ptr<CUIManager> m_wpUIManager;
	};
}
