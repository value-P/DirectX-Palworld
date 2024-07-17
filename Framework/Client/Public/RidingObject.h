#pragma once
#include "GameObject.h"

namespace Engine
{
	class CCameraManager;
	class CFSMComponent;
	class CBoxComponent;
	class CSphereComponent;
	class CMeshComponent;
	class CInputManager;
	class CPhysXManager;
	class CBone;
}

namespace Client
{
	class CEffectManager;
	class CPlayer;

	struct FRidingObjectDesc : public CGameObject::FGameObjectDesc
	{
		float m_fMoveSpeed = 0.f;
		float m_fJumpPower = 0.f;

		Vector3 m_vRidingOffset = Vector3(0.f);
	};

	enum class ERidingObjectState
	{
		IDLE, MOVE, JUMP, ENUM_END
	};

    class CRidingObject : public CGameObject
    {		
    /********************
		Struct
	********************/
	public:
		struct FRindingStateData
		{
			bool m_bOnGround = true;
			
			float m_fMaxStaminaGauge = 100.f;
			float m_fCurrentStaminaGauge = 100.f;

			float m_fMoveSpeed = 0.f;
			float m_fJumpPower = 0.f;

			Vector3 m_vMoveDirection = Vector3(0.f);
			Vector3 m_vLinearVelocity = Vector3(0.f);
		};

    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CRidingObject() = default;

		virtual ~CRidingObject() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;
		virtual void OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void LookAt(const Vector3& _vPosition) override;

		virtual void Jump() {}

		virtual void OnRidingEffect() {};
		virtual void OffRidingEffect() {};

		shared_ptr<CColliderComponent> GetBodyColliderComponent();

		void SetState(ERidingObjectState _eState);

		virtual void SetPosition(const Vector3& _vPos) override;

		virtual const std::optional<Vector3> GetPosition() const override;

	protected:
		virtual void InitRidingBone() = 0;
		virtual void InitMeshComponent() = 0;
		virtual void InitColliderComponent() = 0;
		virtual void InitFSMComponent();

		virtual void KeyInput();

	/********************
		Getter/Setter
	********************/
	public:
		shared_ptr<CBone> GetRidingBone() { return m_wpRidingBone.lock(); }
		
		Vector3 GetRidingOffset() { return m_vRidingOffset; }

		FRindingStateData GetObjectData();

		shared_ptr<CMeshComponent> GetMeshComponent() { return m_spMeshComponent; }

		void SetRideMode(shared_ptr<CPlayer> _spPlayer);

		void SetSpeed(float _fMoveSpeed) { m_tRidingObjectData.m_fMoveSpeed = _fMoveSpeed; }

		void SetLinearVelocity(Vector3 _vLinearVelocity);

	/********************
		Methods
	********************/
	public:
		void SetRiderCamera(bool _bSprint);

	protected:
		void ComputeRotation(float _fDeltaSeconds);

		void Move(float _fDeltaSeconds);

	/********************
		Data Members
	********************/
	protected:
		std::weak_ptr<CInputManager> m_wpInputManager;
		std::weak_ptr<CCameraManager> m_wpCameraManager;
		std::weak_ptr<CEffectManager> m_wpEffectManager;
		std::weak_ptr<CPhysXManager> m_wpPhysXManager;

		std::shared_ptr<CFSMComponent> m_spFSMComponent = nullptr;
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;
		std::shared_ptr<CSphereComponent> m_spSphereCollider = nullptr;
		std::shared_ptr<CBoxComponent> m_spBoxTrigger_ForEnv = nullptr;
		std::shared_ptr<CBoxComponent> m_spBoxTrigger_ForEnv2 = nullptr;
		std::weak_ptr<CBone> m_wpRidingBone;

		FRindingStateData m_tRidingObjectData;

		Vector3 m_vDestLook = Vector3(0.f);
		Vector3 m_vMoveDirection = Vector3(0.f);
		Vector3 m_vRidingOffset = Vector3(0.f);

		std::weak_ptr<CPlayer> m_wpRider;

		float m_fDriftZRotation = 30.f;
		float m_fCurrentDriftZSpeed = 0.f;

		bool m_bEnv1Entered = false;
		bool m_bEnv2Entered = false;
    };
}