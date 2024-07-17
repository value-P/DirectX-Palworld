#pragma once
#include "Manager.h"

namespace Engine
{
	class CDeviceManager;
	class CInputManager;
	class CCameraManager;
	class CPipelineManager;
	class CGameObject;

	class ENGINE_DLL CPickManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CPickManager)
	
	/********************
		Events
	********************/
	public:
		HRESULT Initialize(HWND _hWnd, int32 _iWinSizeX, int32 _iWinSizeY);

		int32 Tick(float _fDeltaSeconds);

		virtual void Release() override;

	/********************
		Getter/Setter
	********************/
	public:
		void SetPickMode(bool _bPickMode) { m_bPickMode = _bPickMode; }

	public:
		const uint32 GetPickedGameObjectID() const { return m_iGameObjectID; }

		const Vector3 GetPickingPos() const { return m_vPickingPos; }

		void SetPickedGameObjectID() { m_iGameObjectID = 0; }

		void SetPickingPos(const Vector3& _vPickingPos) { m_vPickingPos = _vPickingPos; }
		
	/********************
		Methods
	********************/
	public:
		void PickGameObject();

		void PickMesh();

		void TransformToLocalSpace(const Matrix& _matWorld);

		bool PickingInLocal(const Vector3& _vPointA, const Vector3& _vPointB, const Vector3& _vPointC, Vector4& _vPickPos);


	/********************
		Data Members
	********************/
	private:
		HWND		m_hWnd{};

		bool		m_bPickMode = false;

		int32		m_iWinSizeY = 0;
		
		int32		m_iWinSizeX = 0;

		uint32		m_iGameObjectID = 0;

		Vector3		m_vPickingPos = {};
		
		Vector4		m_vRayPosInWorld = {};

		Vector4		m_vRayDirInWorld = {};
		
		Vector4		m_vRayPosInLocal = {};

		Vector4		m_vRayDirInLocal = {};

	private:
		std::weak_ptr<CDeviceManager> m_wpDeviceManager;

		std::weak_ptr<CInputManager> m_wpInputManager;

		std::weak_ptr<CCameraManager> m_wpCameraManager;

		std::weak_ptr<CPipelineManager> m_wpPipelineManager;

	private:
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		ComPtr<IDXGISwapChain> m_cpSwapChain = nullptr;

		ComPtr<ID3D11Texture2D> m_cpCopyTexture2D = nullptr;
	};
}
