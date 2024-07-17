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

		// �� ��ǥ�� ī�޶� ����
		Vector3 m_vSphericalCameraAxisOffset = Vector3::Zero;

		Vector3 m_vSphericalCameraEyeOffset = Vector3::Zero;

		float m_fCurSphericalCameraRadius = 5.0f;

		float m_fMinElevationDegree = 0.0f;

		float m_fMaxElevationDegree = 60.0f;

		// �ø� ����ü ����
		float m_fCullFrustumNear = 0.1f;

		float m_fCullFrustumFar = 1000.0f;

		// �����¿� ��� ����
		Vector4 m_fCullFrustumSlops = Vector4(1.0f, -1.0f, -1.0f, 1.0f);

		// �ø� �� ����
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

		// Keyboad Sensitivity /* 24-04-05 �̵��� �߰� */
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

		// 24-04-01 �̵��� �߰�
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

		// ���콺 ���� ����
		bool m_bMouseControl = true;

		// Ű���� ���� ����
		bool m_bKeyboardControl = true;

		// ���콺 ����
		float m_fMouseSensitivity = 0.1f;

		// Ű���� ����
		float m_fKeyboardSensitivity = 5.0f;

	private:
		EProjectionType m_eProjectionType = EProjectionType::ENUM_END;

		float m_fAspectRatio = 16.0f / 9.0f;

		float m_fCurFieldOfView = 60.0f;

		float m_fFarPlane = 1'000.0f;

		float m_fNearPlane = 0.1f;

	private:
		// �� ��ǥ�� ���� ȸ���� ���� �Ǵ� ������(Owner�� Position���� �󸶸�ŭ ������ ��ġ�� ȸ���� ���� �� ���ΰ�)
		Vector3 m_vSphericalCameraAxisOffset = Vector3::Zero;

		// ���� ��ǥ ����� ���� ��ġ�� �������� �Ͽ� Eye�� �����̰��� �ϴ� ������(x : ī�޶�Right��ŭ, y: ī�޶�Up��ŭ, z : ī�޶�Look��ŭ �̵���Ŵ)
		Vector3 m_vSphericalCameraEyeOffset = Vector3::Zero;

		// ���� ������
		float m_fCurSphericalCameraRadius = 5.0f;

		// ������
		float m_fCurAzimuthRadian = 0.0f;

		// �ּ� ������
		float m_fMinAzimuthDegree = -720.0f;

		// �ִ� ������
		float m_fMaxAzimuthDegree = 720.0f;

		// �Ӱ�
		float m_fCurElevationRadian = 0.0f;

		// �ּ� �Ӱ�
		float m_fMinElevationDegree = 5.0f;

		// �ִ� �Ӱ�
		float m_fMaxElevationDegree = 60.0f;

	private: // ����ŷ ����
		array<FShakeData, (int32)EShakeType::ENUM_END> m_arrShakeData;
		
		Matrix m_matShakeRotX = Matrix::Identity;
		Matrix m_matShakeRotY = Matrix::Identity;
		Matrix m_matShakeRotZ = Matrix::Identity;

		Vector4 m_vShakeXYZfov = Vector4(0.f);

		bool m_bUseShake = false;
		bool m_bUseFovShake = false;

	private:
		// �� ��ǥ�� ���� ��ġ
		Vector3 m_vSphericalCameraCartesian = Vector3::Zero;

		// �� ��ǥ�� ���� �߽�
		Vector3 m_vSphericalCameraCenter = Vector3::Zero;

		// �� ��ǥ�� ������ ����
		Vector3 m_vSphericalCameraForward = Vector3::UnitZ;

		// �� ��ǥ�� ����Ʈ ����
		Vector3 m_vSphericalCameraRight = Vector3::UnitX;

	private:
		Matrix m_matView = Matrix::Identity;

		Matrix m_matPerspectiveProjection = Matrix::Identity;

		Matrix m_matOrthogonalProjection = Matrix::Identity;

		// ī�޶� �ø� ����ü
		BoundingFrustum m_tCameraCullFrustum;

		// ī�޶� �ø� ��
		BoundingSphere m_tCameraCullSphere;

	private:
		std::weak_ptr<CInputManager> m_wpInputManager;

		std::weak_ptr<CCameraManager> m_wpCameraManager;
	};
}
