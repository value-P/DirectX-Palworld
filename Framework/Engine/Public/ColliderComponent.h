#pragma once
#include "Component.h"

namespace Engine
{
	class CPhysXManager;
	class CCameraManager;
	class CRenderManager;

	/********************
		Description
	********************/
	struct ENGINE_DLL FColliderComponentDesc : public CComponent::FComponentDesc
	{
	public:
		// 정적, 동적 여부
		EMobilityType m_eMobilityType = EMobilityType::ENUM_END;

		// 콜라이더 모양
		EColliderType m_eColliderType = EColliderType::ENUM_END;

		// 콜리전 레이어
		ECollisionLayer m_eCollisionLayer = ECollisionLayer::ENUM_END;

		// 콜리전 플래그
		ECollisionFlag m_eCollisionFlag = ECollisionFlag::ENUM_END;

	public:
		float m_fDensity = 1.0f;

		float m_fMass = 1.0f;

		bool m_bGravity = true;

		bool m_bTrigger = false;

		// 액터를 잠시 지연해서 신에 추가합니다.
		bool m_bDeferredAdd = true;

		bool m_bEnableSimulation = true;

		// 축 고정
		int32 m_iAxisLockType = 0;

		// 로컬 이동
		Vector3 m_vLocalPosition = Vector3::Zero;

		// 로컬 회전
		Vector3 m_vLocalRotate = Vector3::Zero;

		// static friction, dynamic friction, restitution
		Vector3 m_vMaterial = Vector3(1.0f, 1.0f, 0.05f);
	};

	class ENGINE_DLL CColliderComponent : public CComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CColliderComponent() = default;

		virtual ~CColliderComponent() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual HRESULT BindVariable() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	public:
		virtual void LookAt(const Vector3& _vPosition);

		virtual void Turn(const Vector3& _vAxis, float _fAngularSpeed);

		virtual void MoveColliderForward(float _fSpeed);

		virtual void MoveColliderBackward(float _fSpeed);

		virtual void MoveColliderRight(float _fSpeed);

		virtual void MoveColliderLeft(float _fSpeed);

		virtual void MoveColliderUp(float _fSpeed);

		virtual void MoveColliderDown(float _fSpeed);

		virtual void SetPosition(const Vector3& _vPosition);

		virtual Vector3 GetPosition() const;

		virtual Quaternion GetQuaternion() const;

	public:
		HRESULT DeferredAddActorToScene();

		HRESULT AddRigidActorToScene();

		virtual HRESULT RemoveRigidActorFromScene();

	public:
		void AddForce(const Vector3& _vDirection, float _fPower);

		void SetLinearVelocity(const PxVec3& _vLinearVelocity);

		void SetAngularVelocity(const PxVec3& _vAngularVelocity);

	public:
		void RequestSimulation(bool _bSimulation);

		void RequestTransform(std::optional<Vector3> _vTranslate = std::nullopt, std::optional<Vector3> _vRotate = std::nullopt);

		virtual void ApplySimulation();

		virtual void ApplyTransform();

	public:
		virtual void ApplySimulationDirectly(bool _bSimulation);

	/********************
		Getter/Setter
	********************/
	public:
		EMobilityType GetMobilityType() const { return m_eMobilityType; }

		ECollisionLayer GetCollisionLayer() const { return m_eCollisionLayer; }

		Vector3 GetLinearVelocity() const;

		Vector3 GetAngularVelocity() const;

		virtual Vector3 GetLocalPosition();

		virtual void SetCollisionLayer(ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag) { return; }

		virtual void SetLocalPosition(const Vector3& _vLocalPosition);

		virtual void SetWorldPosition(const Vector3& _vWorldPosition);

		void SetUseGravity(bool _bUseGravity);

		virtual void SetMaterial(const Vector3& _vMatrial) { return; };

		void SetLockFlags(int32 _iAxisLockType);

		void SetMassLocalPose(Vector3& _vPosition);

	public:
		void SetPhysxTransformMatrix(const Matrix& _matTransform);

	public:
		virtual void SetCulled(bool _bCulled) override;

		virtual void SetActive(bool _bActive) override;

	/********************
		Data Members
	********************/
	protected:
		PxMaterial* m_pMaterial = nullptr;

		PxTransform m_oPxTransform;

	protected:
		PxPhysics* m_pPhysics = nullptr;

		PxScene* m_pScene = nullptr;

		PxRigidStatic* m_pStaticRigidActor = nullptr;

		PxRigidDynamic* m_pDynamicRigidActor = nullptr;

	protected:
		EMobilityType m_eMobilityType = EMobilityType::ENUM_END;

		EColliderType m_eColliderType = EColliderType::ENUM_END;
		
		ECollisionLayer m_eCollisionLayer = ECollisionLayer::ENUM_END;

		ECollisionFlag m_eCollisionFlag = ECollisionFlag::ENUM_END;

		int32 m_iAxisLockType = 0;

		float m_fDensity = 1.0f;

		float m_fMass = 1.0f;

		bool m_bGravity = true;

		bool m_bTrigger = false;

		bool m_bDeferredAdd = true;

		bool m_bEnableSimulation = true;

		Vector3 m_vLocalPosition = Vector3::Zero;

		Vector3 m_vLocalRotate = Vector3::Zero;

		Vector3 m_vMaterial = Vector3(0.5f, 0.5f, 0.5f);

		Vector3 m_vRotate = Vector3(0.f, 0.f, 0.f);

		bool m_bRenderDebug = true;

	protected:
		std::weak_ptr<CRenderManager> m_wpRenderManager;

		std::weak_ptr<CCameraManager> m_wpCameraManager;

		std::weak_ptr<CPhysXManager> m_wpPhysxManager;

	protected:
		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		std::shared_ptr<PrimitiveBatch<VertexPositionColor>> m_spPrimitiveBatch = nullptr;

		std::shared_ptr<BasicEffect> m_spBasicEffect = nullptr;

		ComPtr<ID3D11InputLayout> m_cpInputLayout = nullptr;

		bool m_bCollided = false;

	protected:
		float m_fDeltaSeconds = 0.0f;

		Matrix m_matLocal;

		std::atomic<bool> m_bRequestSimulationFlag = false;

		std::atomic<bool> m_bRequestedSimulation = false;

		std::atomic<bool> m_bRequestTransformFlag = false;

		std::optional<Vector3> m_bRequestedTranslate = Vector3(0.0f);

		std::optional<Vector3> m_bRequestedRotate = Vector3(0.0f);
	};
}
