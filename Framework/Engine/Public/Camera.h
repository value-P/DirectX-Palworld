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
		* @detail: 이 카메라를 현재 방송하는 카메라로 변경합니다.
		*/
		HRESULT SetOnAir();

		// 24-04-01 이동준 추가
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
		// 카메라 타입; 자유, 구, 연출
		ECameraType m_eCameraType = ECameraType::DEFAULT;

		// 마우스 입력 여부
		bool m_bMouseControl = true;

		// 키보드 입력 여부
		bool m_bKeyboardControl = true;

		// 마우스 입력 감도
		float m_fMouseSensitivity = 0.1f;

		// 키보드 입력 감도
		float m_fKeyboardSensitivity = 5.0f;

		// 카메라 투영 타입; 직교 투영, 원근 투영
		EProjectionType m_eProjectionType = EProjectionType::ENUM_END;

		// 종횡비
		float m_fAspectRatio = 16.0f / 9.0f;

		float m_fFieldOfView = 60.0f;

		// 원평면 거리
		float m_fFarPlane = 1'000.0f;

		// 근평면 거리
		float m_fNearPlane = 0.1f;

		Vector3 m_vCurSphericalCameraOffset = Vector3::Zero;

		float m_fCurSphericalCameraRadius = 5.0f;

		// 최소, 최대 방위각
		float m_fMinAzimuthDegree = -360.0f;

		float m_fMaxAzimuthDegree = 360.0f;

		// 최소, 최대 앙각
		float m_fMinElevationDegree = 0.0f;

		float m_fMaxElevationDegree = 60.0f;
		*/
	};
}
