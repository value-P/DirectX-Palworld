#pragma once
#include "ControllerComponent.h"

namespace Engine
{
	class CCharacter;
	class CCameraComponent;
}

namespace Client
{
	class CUIManager;

	/********************
		Description
	********************/
	struct FPlayerControllerComponentDesc : public FControllerComponentDesc
	{
		std::shared_ptr<CCameraComponent> m_spCameraComponent = nullptr;
	};

	class CPlayerControllerComponent final : public CControllerComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPlayerControllerComponent() = default;

		virtual ~CPlayerControllerComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPlayerControllerComponent> Create(const std::shared_ptr<FPlayerControllerComponentDesc>& _spPlayerControllerComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

	/********************
		Getter/Setter
	********************/
	public:
		void ChangeControllerMode(EPlayerControllerMode _eControlMode);

		void SetCanJump(bool _bCanJump) { m_bCanJump = _bCanJump; m_bJumping = false; }

		void SetFixedMoveDirection(Vector3 _vFixedMoveDir) { m_vFixedMoveDirection = _vFixedMoveDir; }

		void ActiveLedgeMode() { if (m_eControlMode == EPlayerControllerMode::CLIMB_MODE) { m_bLedgeMode = true; } }

		void SetPosition(const Vector3& _vPos);

		EPlayerControllerMode GetCurrentControlMode() { return m_eControlMode; }

		Vector3 GetPosition() const;

		/********************
			Methods
		********************/
	public:
		void AddForce(const Vector3& _vDirection, float _fPower);

	protected:
		virtual void UpdateControllerComponentFromScene(float _fDeltaSeconds) override;

	/********************
		Methods
	********************/
	protected:
		std::weak_ptr<CCameraComponent> m_wpPlayerCameraComponent;

		std::weak_ptr<CUIManager> m_wpUIManager;

	protected:
		Vector3 m_vFixedMoveDirection = Vector3(0.0f, 0.0f, 0.0f);

		EPlayerControllerMode m_eControlMode = EPlayerControllerMode::DEFAULT_MODE;

		bool m_bCanJump = true;

		bool m_bLedgeMode = false;
		float m_fLedgeModeChecker = 0.f;
		float m_fLedgeModeTime = 0.6f;

		bool m_bAddForce = false;
		PxVec3 m_vForceDirection = PxVec3(0.f, 0.f, 0.f);
		float m_fForce = 0.f;

#ifdef DEBUG
		bool m_bGodMode = false;
#endif // DEBUG
	};
}
