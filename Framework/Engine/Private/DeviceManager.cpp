// default
#include "pch.h"
#include "DeviceManager.h"

HRESULT Engine::CDeviceManager::Initialize(HWND _hWnd, HINSTANCE _hInst, int32 _iWindowSizeX, int32 _iWindowSizeY)
{
	m_hWnd = _hWnd;
	m_hInst = _hInst;
	m_iWindowSizeX = _iWindowSizeX;
	m_iWindowSizeY = _iWindowSizeY;

	ENSUREF(InitializeDevice(), L"Failed to initialize device, device context.");
	ENSUREF(InitializeSwapChain(), L"Failed to initialize swap chain.");
	ENSUREF(InitializeViewports(), L"Failed to initialize viewport");
	return S_OK;
}

void Engine::CDeviceManager::Release()
{
	m_cpMultithread = nullptr;
	m_cpSwapChain = nullptr;
	
	m_cpDeferredDeviceContext = nullptr;

	m_cpDeviceContext = nullptr;
	m_cpDevice = nullptr;
}

HRESULT Engine::CDeviceManager::InitializeDevice()
{
	UINT iCreateDeviceFlags = 0;

#ifdef _DEBUG
	iCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL eFeatureLevel;

	const D3D_FEATURE_LEVEL eFeatureLevels[2] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_9_3
	};

	// 디바이스를 생성하면서 DX11을 지원하는지 파악합니다.
	if (FAILED(D3D11CreateDevice
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		iCreateDeviceFlags,
		eFeatureLevels,
		ARRAYSIZE(eFeatureLevels),
		D3D11_SDK_VERSION,
		m_cpDevice.GetAddressOf(),
		&eFeatureLevel,
		m_cpDeviceContext.GetAddressOf()
	)))
	{
		return E_FAIL;
	}
	
	m_cpDevice->CreateDeferredContext(0, m_cpDeferredDeviceContext.GetAddressOf());

	// DX11을 지원하지 않는 경우, 실패합니다.
	if (eFeatureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MESSAGE_BOX(L"D3D feature level 11 is not supported.");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Engine::CDeviceManager::InitializeSwapChain()
{
	ComPtr<IDXGIDevice> cpGIDevice = nullptr;
	m_cpDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&cpGIDevice);

	ComPtr<IDXGIAdapter> cpGIAdapter = nullptr;
	cpGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&cpGIAdapter);

	ComPtr<IDXGIFactory> cpGIFactory = nullptr;
	cpGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&cpGIFactory);

	// 멀티스레드가 지원되는지 테스트합니다.
	ENSUREF(m_cpDevice->QueryInterface(__uuidof(ID3D11Multithread), (void**)&m_cpMultithread), L"Failed to initialize ID3D11Multithread.");
	m_cpMultithread->SetMultithreadProtected(TRUE);

	// 스왑 체인을 생성합니다.
	DXGI_SWAP_CHAIN_DESC tSwapChainDesc;
	ZeroMemory(&tSwapChainDesc, sizeof(tSwapChainDesc));
	tSwapChainDesc.BufferDesc.Width						= m_iWindowSizeX;
	tSwapChainDesc.BufferDesc.Height					= m_iWindowSizeY;
	tSwapChainDesc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	tSwapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tSwapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	tSwapChainDesc.BufferCount							= 2;
	tSwapChainDesc.BufferDesc.RefreshRate.Numerator		= 60;
	tSwapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	tSwapChainDesc.SampleDesc.Count						= 1;
	tSwapChainDesc.SampleDesc.Quality					= 0;
	tSwapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tSwapChainDesc.OutputWindow							= m_hWnd;
	tSwapChainDesc.Windowed								= TRUE;
	tSwapChainDesc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	tSwapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;

	return cpGIFactory->CreateSwapChain(m_cpDevice.Get(), &tSwapChainDesc, m_cpSwapChain.GetAddressOf());
}

HRESULT Engine::CDeviceManager::InitializeViewports()
{
	D3D11_VIEWPORT tViewport;
	ZeroMemory(&tViewport, sizeof(tViewport));
	tViewport.TopLeftX	= 0.0f;
	tViewport.TopLeftY	= 0.0f;	tViewport.Width		= static_cast<FLOAT>(m_iWindowSizeX);
	tViewport.Height	= static_cast<FLOAT>(m_iWindowSizeY);
	tViewport.MinDepth	= 0.0f;
	tViewport.MaxDepth	= 1.0f;
	m_cpDeviceContext->RSSetViewports(1, &tViewport);
	return S_OK;
}

HRESULT Engine::CDeviceManager::Present()
{
	return m_cpSwapChain->Present(0, 0);
}
