#pragma once
#include "Manager.h"

namespace Engine
{
	class CCameraComponent;

	/*
	* @detail:	ī�޶� �Ŵ���
	* ī�޶� �Ŵ����� ���� �ȿ� �ִ� ī�޶� ������Ʈ���� ��� ������ ���� �ֽ��ϴ�.
	*
	*/
	class ENGINE_DLL CCameraManager final : public CManager
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CCameraManager() = default;

		virtual ~CCameraManager() = default;

		/********************
			Events
		********************/
	public:
		HRESULT Initialize();

		int32 Tick(float _fDeltaSeconds);

		virtual void Release() override;

		/********************
			Getter/Setter
		********************/
	public:
		void SetCurViewMatrix(const Matrix& _matCurView) { m_matCurView = _matCurView; }

		void SetCurPerspectiveProjectionMatrix(const Matrix& _matCurPerspectiveProjection) { m_matCurPerspectiveProjection = _matCurPerspectiveProjection; }

		void SetCurOrthogonalProjectionMatrix(const Matrix& _matCurOrthogonalProjection) { m_matCurOrthogonalProjection = _matCurOrthogonalProjection; }

		/*
		* @detail:	���� �Կ� ���� ī�޶��� �� ����� ��ȯ�մϴ�.
		*/
		const Matrix& GetCurViewMatrix() const { return m_matCurView; }

		/*
		* @detail:	���� �Կ� ���� ī�޶��� ���� ���� ����� ��ȯ�մϴ�.
		*/
		const Matrix& GetCurPerspectiveProjectionMatrix() const { return m_matCurPerspectiveProjection; }

		/*
		* @detail:	���� �Կ� ���� ī�޶��� ���� ���� ����� ��ȯ�մϴ�.
		*/
		const Matrix& GetCurOrthogonalProjectionMatrix() const { return m_matCurOrthogonalProjection; }

	public:
		HRESULT SetCurCamera(const std::wstring& _wstrCameraComponentName);

 		std::shared_ptr<CCameraComponent> GetCurCamera() const { return m_wpCurCameraComponent.expired() ? nullptr : m_wpCurCameraComponent.lock(); }

		Vector3 GetCurCameraWorldPosition() const;

		Vector3 GetCurCameraWorldForwardVector() const;

		Vector3 GetCurCameraWorldRightVector() const;

		Vector3 GetCurCameraWorldUpVector() const;

		const FCameraInfo& GetCurCameraInfo();

		BoundingFrustum& GetCameraFrustum() { return m_tFrustum; }

		BoundingSphere& GetCameraCullSphere() { return m_tSphere; }

		/********************
			Methods
		********************/
	public:
		HRESULT AddCameraComponent(const std::wstring& _wstrCameraComponentName, const std::shared_ptr<CCameraComponent>& _spCameraComponent);

		HRESULT RemoveCameraComponent(const std::wstring& _wstrCameraComponentName);

		std::shared_ptr<CCameraComponent> FindCameraComponent(const std::wstring& _wstrCameraComponentName);

		HRESULT ClearCameraManager();

		/********************
			Data Members
		********************/
	private:
		// ī�޶� ������Ʈ��
		std::unordered_map<std::wstring, std::weak_ptr<CCameraComponent>> m_umapCameraComponents;

		// ���� ī�޶� ������Ʈ
		std::weak_ptr<CCameraComponent> m_wpCurCameraComponent;

		// ���� ī�޶� ����ü
		BoundingFrustum m_tFrustum{};

		BoundingSphere m_tSphere{};

	private:
		// ���� �� ���
		Matrix m_matCurView = Matrix::Identity;

		// ���� ���� ���� ���
		Matrix m_matCurPerspectiveProjection = Matrix::Identity;

		// ���� ���� ���� ���
		Matrix m_matCurOrthogonalProjection = Matrix::Identity;

		// Bind
		FCameraInfo m_tCameraInfo = {};
	};
}
