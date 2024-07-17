#pragma once
#include "Component.h"

namespace Engine
{
	class CGameObject;
	class CInputManager;
	class CPhysXManager;
	class CControllerComponent;

	/********************
		Callback
	********************/
	class ENGINE_DLL CControllerComponentBehaviorCallback : public PxControllerBehaviorCallback
	{
	public:
		virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor) override;

		virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller) override;

		virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) override;

	public:
		std::shared_ptr<CGameObject> m_spGameObject = nullptr;

		std::shared_ptr<CControllerComponent> m_spControllerComponent = nullptr;
	};

	class ENGINE_DLL CControllerComponentHitReport : public PxUserControllerHitReport
	{
	public:
		virtual void onShapeHit(const PxControllerShapeHit& hit) override;

		virtual void onControllerHit(const PxControllersHit& hit) override;

		virtual void onObstacleHit(const PxControllerObstacleHit& hit) override;

	public:
		std::shared_ptr<CGameObject> m_spGameObject = nullptr;

		std::shared_ptr<CControllerComponent> m_spControllerComponent = nullptr;
	};

	/********************
		Description
	********************/
	struct ENGINE_DLL FControllerComponentDesc : public CComponent::FComponentDesc
	{
		EControllerShapeType m_eControllerShapeType = EControllerShapeType::ENUM_END;

		ECollisionLayer m_eControllerLayer = ECollisionLayer::ENUM_END;

		ECollisionFlag m_eControllerFlag = ECollisionFlag::ENUM_END;

		Vector3 m_vMaterials = Vector3(0.5f, 0.5f, 0.5f);

		float m_fSlopeLimit = 60.0f;

		float m_fStepOffset = 0.1f;

		float m_fContactOffset = 0.05f;

		float m_fCapsuleHalfHeight = 1.0f;

		float m_fCapsuleRadius = 1.0f;

		Vector3 m_vExtents = Vector3(1.0f, 1.0f, 1.0f);

		Vector3 m_vLocalPosition = Vector3(0.0f, 0.0f, 0.0f);

		Vector3 m_vLocalRotate = Vector3(0.0f, 0.0f, 0.0f);

		CControllerComponentBehaviorCallback* m_pControllerComponentBehaviorCallback = nullptr;

		CControllerComponentHitReport* m_pControllerComponentHitReport = nullptr;
	};

	class ENGINE_DLL CControllerComponent : public CComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CControllerComponent() = default;

		virtual ~CControllerComponent() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CControllerComponent> Create(const std::shared_ptr<FControllerComponentDesc>& _spControllerComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

		virtual HRESULT BindVariable() override;

	/********************
		Methods
	********************/
	protected:
		virtual void UpdateControllerComponentFromScene(float _fDeltaSeconds);
		
	/********************
		Getter/Setter
	********************/
	public:
		bool IsOnAir() { return m_bAir; }

		void SetOnAir(bool _bAir) { m_bAir = _bAir; }

		bool IsJumping() { return m_bJumping; }

		void SetJumping(bool _bJumping) { m_bJumping = _bJumping; }

		EVerticalMovementType GetVerticalMovementType();

		void SetCurMoveSpeed(float _fCurMoveSpeed) { m_fCurMoveSpeed = _fCurMoveSpeed; }

		const Vector3& GetMoveDirection() { return m_vMoveDirection; }

		void SetTerminalVelocity(float _fTerminalVelocity) { m_fTerminalVelocity = _fTerminalVelocity; }

		ECollisionLayer GetCollisionLayer() const { return m_eControllerLayer; }

		ECollisionFlag GetCollisionFlag() const { return m_eControllerFlag; }

	/********************
		Methods
	********************/
	public:
		void SetTerminalVelocityAsDefault() { m_fTerminalVelocity = m_fDefaultTerminalVelocity; }

	/********************
		Data Members
	********************/
	protected:
		std::weak_ptr<CInputManager> m_wpInputManager;

		std::weak_ptr<CPhysXManager> m_wpPhysXManager;

	protected:
		// 컨트롤러
		PxController* m_pController = nullptr;

		// 이 컨트롤러를 관리하는 매니저
		PxControllerManager* m_pControllerManager = nullptr;

		PxControllerFilters m_oControllerFilters;
		
		PxControllerCollisionFlags m_oControllerCollisionFlags;

	protected:
		PxScene* m_pScene = nullptr;

		PxPhysics* m_pPhysics = nullptr;

		PxMaterial* m_pMaterial = nullptr;

	protected:
		CControllerComponentBehaviorCallback m_oBehaviorCallback;

		CControllerComponentHitReport m_oHitReportCallback;

	protected:
		EControllerShapeType m_eControllerShapeType = EControllerShapeType::ENUM_END;

		ECollisionLayer m_eControllerLayer = ECollisionLayer::ENUM_END;

		ECollisionFlag m_eControllerFlag = ECollisionFlag::ENUM_END;

		// 중력
		float m_fGravity = 25.8f;

		// 이동 방향
		Vector3 m_vMoveDirection{ 0.0f, 0.0f, 0.0f };

		// 이동 속력
		float m_fCurMoveSpeed = 3.0f;

		float m_fMinMoveSpeed = 0.0f;

		float m_fMaxMoveSpeed = 30.0f;

		// 점프 속도
		float m_fJumpVelocity =  4.5f;

		float m_fSumJumpSeconds = 0.0f;

		float m_fCurJumpVelocity = 0.0f;

		// 종단 속도
		float m_fDefaultTerminalVelocity = 15.0f;

		float m_fTerminalVelocity = 15.0f;

		// 플래그
		bool m_bAir = false;

		bool m_bJumping = false;

		bool m_bKeyPressed = false;

		bool m_bJumpKeyPressed = false;

	protected:
		// controller
		float m_fSlopeLimit = 60.0f;

		float m_fStepOffset = 0.1f;

		float m_fContactOffset = 0.1f;

		PxShape* m_pShape = nullptr;

		Vector3 m_vLocalPosition = Vector3(0.0f, 0.0f, 0.0f);

		Vector3 m_vLocalRotate = Vector3(0.0f, 0.0f, 0.0f);
	};
}
