#pragma once
#include "Manager.h"

namespace Engine
{
	class CCameraComponent;

	/*
	* @detail:	카메라 매니저
	* 카메라 매니저는 레벨 안에 있는 카메라 컴포넌트들을 멤버 변수로 갖고 있습니다.
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
		* @detail:	현재 촬영 중인 카메라의 뷰 행렬을 반환합니다.
		*/
		const Matrix& GetCurViewMatrix() const { return m_matCurView; }

		/*
		* @detail:	현재 촬영 중인 카메라의 원근 투영 행렬을 반환합니다.
		*/
		const Matrix& GetCurPerspectiveProjectionMatrix() const { return m_matCurPerspectiveProjection; }

		/*
		* @detail:	현재 촬영 중인 카메라의 직교 투영 행렬을 반환합니다.
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
		// 카메라 컴포넌트들
		std::unordered_map<std::wstring, std::weak_ptr<CCameraComponent>> m_umapCameraComponents;

		// 현재 카메라 컴포넌트
		std::weak_ptr<CCameraComponent> m_wpCurCameraComponent;

		// 현재 카메라 절두체
		BoundingFrustum m_tFrustum{};

		BoundingSphere m_tSphere{};

	private:
		// 현재 뷰 행렬
		Matrix m_matCurView = Matrix::Identity;

		// 현재 원근 투영 행렬
		Matrix m_matCurPerspectiveProjection = Matrix::Identity;

		// 현재 직교 투영 행렬
		Matrix m_matCurOrthogonalProjection = Matrix::Identity;

		// Bind
		FCameraInfo m_tCameraInfo = {};
	};
}
