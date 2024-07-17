#pragma once
#include "Component.h"

namespace Engine
{
	class CInputManager;
	class CCameraManager;

	/********************
		Description
	********************/
	struct ENGINE_DLL FCameraComponentDesc : public CComponent::FComponentDesc
	{
		ECameraType m_eCameraType = ECameraType::DEFAULT;

		bool m_bMouseControl = true;

		bool m_bKeyboardControl = true;

		float m_fMouseSensitivity = 0.1f;

		float m_fKeyboardSensitivity = 5.0f;

		EProjectionType m_eProjectionType = EProjectionType::PERSPECTIVE;

		float m_fAspectRatio = 16.0f / 9.0f;

		float m_fFieldOfView = 60.0f;

		float m_fFarPlane = 1'000.0f;

		float m_fNearPlane = 0.1f;

		// 구 좌표계 카메라 문맥
		Vector3 m_vSphericalCameraAxisOffset = Vector3::Zero;

		Vector3 m_vSphericalCameraEyeOffset = Vector3::Zero;

		float m_fCurSphericalCameraRadius = 5.0f;

		float m_fMinElevationDegree = 0.0f;

		float m_fMaxElevationDegree = 60.0f;

		// 컬링 절두체 문맥
		float m_fCullFrustumNear = 0.1f;

		float m_fCullFrustumFar = 1000.0f;

		// 상하좌우 평면 기울기
		Vector4 m_fCullFrustumSlops = Vector4(1.0f, -1.0f, -1.0f, 1.0f);

		// 컬링 원 문맥
		float m_fCullSphereRadius = 50.0f;
	};

	struct FShakeData
	{
		bool m_bUseShake = false;
		float m_fMinShakePower = 0.f;
		float m_fShakePower = 0.f;
		float m_fShakeCheck = 0.f;
		float m_fShakeTime = 0.f;
	};

	class ENGINE_DLL CCameraComponent : public CComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CCameraComponent() = default;

		virtual ~CCameraComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CCameraComponent> Create(const std::shared_ptr<FCameraComponentDesc>& _spCameraComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc);

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
		Getter/Setter
	********************/
	public:
		// Projection Type
		void SetProjectionType(EProjectionType _eProjectionType) { m_eProjectionType = _eProjectionType; }

		// Aspect Ratio
		void SetAspectRatio(float _fAspectRatio) { m_fAspectRatio = _fAspectRatio; };

		// FOV
		void SetFieldOfView(float _fFieldOfView) { m_fCurFieldOfView = _fFieldOfView; }

		float GetFieldOfView() const { return m_fCurFieldOfView; }

		// Far/Near
		float GetFarPlane() { return m_fFarPlane; }
		void SetFarPlane(float _fFarPlane) { m_fFarPlane = _fFarPlane; }
		float GetNearPlane() { return m_fNearPlane; }
		void SetNearPlane(float _fNearPlane) { m_fNearPlane = _fNearPlane; }

		// Mouse Sensitivity
		void SetMouseSensitivity(float _fMouseSensitivity) { m_fMouseSensitivity = _fMouseSensitivity; }

		float GetMouseSensitivity() const { return m_fMouseSensitivity; }

		// Keyboad Sensitivity /* 24-04-05 이동준 추가 */
		void SetKeyboardSensitivity(float _fKeyboardSensitivity) { m_fKeyboardSensitivity = _fKeyboardSensitivity; }

		float GetKeyboardSensitivity() const { return m_fKeyboardSensitivity; }

		// Spherical Coordinates
		void SetSphericalCameraCenter(const Vector3& _vSphereCenter) { m_vSphericalCameraCenter = _vSphereCenter; }

		const Vector3& GetSphericalCameraCenter() const { return m_vSphericalCameraCenter; }

		void SetSphericalCameraAxisOffset(const Vector3 _vSphericalCameraAxisOffset) { m_vSphericalCameraAxisOffset = _vSphericalCameraAxisOffset; }
		
		Vector3 GetSphericalCameraAxisOffset() { return m_vSphericalCameraAxisOffset; }
		
		void SetSphericalCameraEyeOffset(const Vector3 _vSphericalCameraEyeOffset) { m_vSphericalCameraEyeOffset = _vSphericalCameraEyeOffset; }

		void SetSphericalCameraRadius(float _fCurSphericalCameraRadius) { m_fCurSphericalCameraRadius = _fCurSphericalCameraRadius; }

		float GetSphericalCameraRadius() const { return m_fCurSphericalCameraRadius; }

		float GetCurAzimuthRadian() const { return m_fCurAzimuthRadian; }

		void SetCurAzimuthRadian(float _fAzimuthRadian) { m_fCurAzimuthRadian = _fAzimuthRadian; }

		float GetCurElevationRadian() const { return m_fCurElevationRadian; }

		void SetCurElevationRadian(float _fElevationRadian) { m_fCurElevationRadian = _fElevationRadian; }

		// Matrices
		const Matrix& GetViewMatrix() const { return m_matView; }

		const Matrix& GetPerspectiveProjectionMatrix() const { return m_matPerspectiveProjection; }

		const Matrix& GetOrthogonalProjectionMatrix() const { return m_matOrthogonalProjection; }

		// Forward/Right
		const Vector3& GetSphericalCameraForwardVector() const { return m_vSphericalCameraForward; }

		const Vector3& GetSphericalCameraRightVector() const { return m_vSphericalCameraRight; }

		// Frustum
		const BoundingFrustum& GetCameraFrustum() const { return m_tCameraCullFrustum; }

		const BoundingSphere& GetCameraCullSphere() const { return m_tCameraCullSphere; }

		// 24-04-01 이동준 추가
		void SetMouseControl(bool _bMouseControl) { m_bMouseControl = _bMouseControl; }

		void SetKeybardControl(bool _bKeybardControl) { m_bKeyboardControl = _bKeybardControl; }

	public:
		HRESULT SetOnAir();

	/********************
		Methods
	********************/
	private:
		Vector3 RecalcCartesian();

		void PlayShake(float _fDeltaSeconds);

		Matrix ShakeMatrix(const Matrix& _matCam);

	public:
		void PreRecalcCameraMatrix();

		void RecalcCameraMatrix();

		void RecalcCameraFrustum();

		// Shake
		void ShakeCamera(EShakeType _eShakeType, float _fMinPower, float _fMaxPower, float _fTime);

	/********************
		Data Members
	********************/
	private:
		ECameraType m_eCameraType = ECameraType::DEFAULT;

		// 마우스 제어 여부
		bool m_bMouseControl = true;

		// 키보드 제어 여부
		bool m_bKeyboardControl = true;

		// 마우스 감도
		float m_fMouseSensitivity = 0.1f;

		// 키보드 감도
		float m_fKeyboardSensitivity = 5.0f;

	private:
		EProjectionType m_eProjectionType = EProjectionType::ENUM_END;

		float m_fAspectRatio = 16.0f / 9.0f;

		float m_fCurFieldOfView = 60.0f;

		float m_fFarPlane = 1'000.0f;

		float m_fNearPlane = 0.1f;

	private:
		// 구 좌표계 에서 회전의 축이 되는 오프셋(Owner의 Position에서 얼마만큼 떨어진 위치가 회전의 축이 될 것인가)
		Vector3 m_vSphericalCameraAxisOffset = Vector3::Zero;

		// 구면 좌표 계산이 끝난 위치를 기준으로 하여 Eye를 움직이고자 하는 오프셋(x : 카메라Right만큼, y: 카메라Up만큼, z : 카메라Look만큼 이동시킴)
		Vector3 m_vSphericalCameraEyeOffset = Vector3::Zero;

		// 구의 반지름
		float m_fCurSphericalCameraRadius = 5.0f;

		// 방위각
		float m_fCurAzimuthRadian = 0.0f;

		// 최소 방위각
		float m_fMinAzimuthDegree = -720.0f;

		// 최대 방위각
		float m_fMaxAzimuthDegree = 720.0f;

		// 앙각
		float m_fCurElevationRadian = 0.0f;

		// 최소 앙각
		float m_fMinElevationDegree = 5.0f;

		// 최대 앙각
		float m_fMaxElevationDegree = 60.0f;

	private: // 쉐이킹 관련
		array<FShakeData, (int32)EShakeType::ENUM_END> m_arrShakeData;
		
		Matrix m_matShakeRotX = Matrix::Identity;
		Matrix m_matShakeRotY = Matrix::Identity;
		Matrix m_matShakeRotZ = Matrix::Identity;

		Vector4 m_vShakeXYZfov = Vector4(0.f);

		bool m_bUseShake = false;
		bool m_bUseFovShake = false;

	private:
		// 구 좌표계 구의 위치
		Vector3 m_vSphericalCameraCartesian = Vector3::Zero;

		// 구 좌표계 구의 중심
		Vector3 m_vSphericalCameraCenter = Vector3::Zero;

		// 구 좌표계 포워드 벡터
		Vector3 m_vSphericalCameraForward = Vector3::UnitZ;

		// 구 좌표계 라이트 벡터
		Vector3 m_vSphericalCameraRight = Vector3::UnitX;

	private:
		Matrix m_matView = Matrix::Identity;

		Matrix m_matPerspectiveProjection = Matrix::Identity;

		Matrix m_matOrthogonalProjection = Matrix::Identity;

		// 카메라 컬링 절두체
		BoundingFrustum m_tCameraCullFrustum;

		// 카메라 컬링 원
		BoundingSphere m_tCameraCullSphere;

	private:
		std::weak_ptr<CInputManager> m_wpInputManager;

		std::weak_ptr<CCameraManager> m_wpCameraManager;
	};
}
