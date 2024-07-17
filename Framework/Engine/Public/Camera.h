#pragma once
#include "GameObject.h"

namespace Engine
{
	class CCameraComponent;
	class CInputManager;

	struct ENGINE_DLL FCameraDesc : public CGameObject::FGameObjectDesc
	{
		bool m_bUseMouseInput = true;
		bool m_bUseKeyInput = true;
	};

	class ENGINE_DLL CCamera : public CGameObject
	{
	public:
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CCamera() = default;

		virtual ~CCamera() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CCamera> Create(const std::shared_ptr<FCameraDesc>& _spCameraDesc);

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
		/*
		* @detail: �� ī�޶� ���� ����ϴ� ī�޶�� �����մϴ�.
		*/
		HRESULT SetOnAir();

		// 24-04-01 �̵��� �߰�
		HRESULT LockCameraViewDirection(bool _bLockCameraViewDirection);

		void SetKeyboardSensitivity(float _fKeyboardSensitivity);

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CInputManager> m_wpInputManager;

		std::weak_ptr<CCameraComponent> m_wpCameraComponent;

		/*
	private:
		// ī�޶� Ÿ��; ����, ��, ����
		ECameraType m_eCameraType = ECameraType::DEFAULT;

		// ���콺 �Է� ����
		bool m_bMouseControl = true;

		// Ű���� �Է� ����
		bool m_bKeyboardControl = true;

		// ���콺 �Է� ����
		float m_fMouseSensitivity = 0.1f;

		// Ű���� �Է� ����
		float m_fKeyboardSensitivity = 5.0f;

		// ī�޶� ���� Ÿ��; ���� ����, ���� ����
		EProjectionType m_eProjectionType = EProjectionType::ENUM_END;

		// ��Ⱦ��
		float m_fAspectRatio = 16.0f / 9.0f;

		float m_fFieldOfView = 60.0f;

		// ����� �Ÿ�
		float m_fFarPlane = 1'000.0f;

		// ����� �Ÿ�
		float m_fNearPlane = 0.1f;

		Vector3 m_vCurSphericalCameraOffset = Vector3::Zero;

		float m_fCurSphericalCameraRadius = 5.0f;

		// �ּ�, �ִ� ������
		float m_fMinAzimuthDegree = -360.0f;

		float m_fMaxAzimuthDegree = 360.0f;

		// �ּ�, �ִ� �Ӱ�
		float m_fMinElevationDegree = 0.0f;

		float m_fMaxElevationDegree = 60.0f;
		*/
	};
}
