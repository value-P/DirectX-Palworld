#pragma once
#include "Manager.h"

namespace Engine
{
	class ENGINE_DLL CDeviceManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDeviceManager() = default;

		virtual ~CDeviceManager() = default;

	/********************
		Events
	********************/
	public:
		HRESULT Initialize(HWND _hWnd, HINSTANCE _hInst, int32 _iWindowSizeX, int32 _iWindowSizeY);

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	private:
		HRESULT InitializeDevice();

		HRESULT InitializeSwapChain();

		HRESULT InitializeViewports();

	public:
		HRESULT Present();
		
	/********************
		Getter/Setter
	********************/
	public:
		HWND GetWindowHandle() const { return m_hWnd; }

		HINSTANCE GetInstanceHandle() const { return m_hInst; }

		int32 GetWindowSizeX() const { return m_iWindowSizeX; }

		int32 GetWindowSizeY() const { return m_iWindowSizeY; }

		const ComPtr<ID3D11Device>& GetDevice() { return m_cpDevice; }

		const ComPtr<ID3D11DeviceContext>& GetDeviceContext() { return m_cpDeviceContext; }

		const ComPtr<ID3D11DeviceContext>& GetDeferredDeviceContext() { return m_cpDeferredDeviceContext; }

		const ComPtr<IDXGISwapChain>& GetSwapChain() { return m_cpSwapChain; }

		const ComPtr<ID3D11Multithread> GetMultithread() {return m_cpMultithread; }

	/********************
		Data Members
	********************/
	private:
		HWND m_hWnd{};

		HINSTANCE m_hInst{};

		int32 m_iWindowSizeX = 1280;

		int32 m_iWindowSizeY = 720;

	private:
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		ComPtr<ID3D11DeviceContext> m_cpDeferredDeviceContext = nullptr;

		ComPtr<IDXGISwapChain> m_cpSwapChain = nullptr;

		ComPtr<ID3D11Multithread> m_cpMultithread = nullptr;
	};
}
