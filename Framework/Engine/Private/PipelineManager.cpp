// default
#include "pch.h"
#include "PipelineManager.h"

// module
#include "EngineModule.h"

// manager
#include "DeviceManager.h"

// handler
#include "ShaderHandler.h"

#include "PipelineState.h"
#include "PipelineView.h"
#include "EffectShader.h"

IMPLEMENT_SINGLETON(CPipelineManager)

HRESULT Engine::CPipelineManager::Initialize()
{
	m_wpDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_cpDevice = m_wpDeviceManager.lock()->GetDevice();
	m_cpDeviceContext = m_wpDeviceManager.lock()->GetDeviceContext();
	m_cpSwapChain = m_wpDeviceManager.lock()->GetSwapChain();
	m_cpMultiThread = m_wpDeviceManager.lock()->GetMultithread();
	m_tHWND = m_wpDeviceManager.lock()->GetWindowHandle();

	// 기본 스테이트를 생성합니다.
	InitializeRasterizerStates();
	InitializeBlendStates();
	InitializeDepthStencilStates();
	InitializePipelineStates();

	// 기본 뷰를 생성합니다.
	InitializeRenderTargetViews();
	InitializeDepthStencilViews();
	InitializePipelineViews();

	// 백버퍼를 카피해오기 위한 텍스처를 준비합니다.
	InitializeBackBufferCopyTexture();

	return S_OK;
}

void Engine::CPipelineManager::Release()
{
	// 뷰 파이프라인 상태 객체를 해제합니다.
	for (auto& pair : m_umapPipelineViews)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapPipelineViews.clear();

	// 스테이트 파이프라인 상태 객체를 해제합니다.
	for (auto& pair : m_umapPipelineStates)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapPipelineStates.clear();

	// 이펙트 셰이더를 널포인터로 초기화합니다. 이펙트 셰이더는 셰이더 핸들러에서 해제합니다.
	for (auto& pair : m_umapEffectShaders)
	{
		pair.second = nullptr;
	}
	m_umapEffectShaders.clear();

	for (auto& pair : m_umapRasterizerStates) { pair.second = nullptr; }
	m_umapRasterizerStates.clear();

	for (auto& pair : m_umapDepthStencilStates) { pair.second = nullptr; }
	m_umapDepthStencilStates.clear();

	for (auto& pair : m_umapBlendStates) { pair.second = nullptr; }
	m_umapBlendStates.clear();

	for (auto& pair : m_umapSamplerStates) { pair.second = nullptr; }
	m_umapSamplerStates.clear();

	for (auto& pair : m_umapShaderResourceViews) { pair.second = nullptr; }
	m_umapShaderResourceViews.clear();

	for (auto& pair : m_umapRenderTargetViews) { pair.second = nullptr; }
	m_umapRenderTargetViews.clear();

	for (auto& pair : m_umapDepthStencilViews) { pair.second = nullptr; }
	m_umapDepthStencilViews.clear();

	m_cpDeviceContext = nullptr;
	m_cpDevice = nullptr;

	m_cpBackBufferCopyTexture = nullptr;
	m_cpBackBufferSRV = nullptr;

	m_spInstance = nullptr;
}

void Engine::CPipelineManager::InitializeRasterizerStates()
{
	{
		D3D11_RASTERIZER_DESC tRasterizerDesc;
		ZeroMemory(&tRasterizerDesc, sizeof(tRasterizerDesc));
		tRasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		tRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		tRasterizerDesc.FrontCounterClockwise = false;
		tRasterizerDesc.DepthClipEnable = true;

		ComPtr<ID3D11RasterizerState> cpRasterizerState = nullptr;
		ENSURE(m_cpDevice->CreateRasterizerState(&tRasterizerDesc, cpRasterizerState.GetAddressOf()));
		m_umapRasterizerStates.insert({ L"RS_FILL_SOLID_CULL_BACK", cpRasterizerState });
	}

	{
		D3D11_RASTERIZER_DESC tRasterizerDesc;
		ZeroMemory(&tRasterizerDesc, sizeof(tRasterizerDesc));
		tRasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		tRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		tRasterizerDesc.FrontCounterClockwise = false;
		tRasterizerDesc.DepthClipEnable = true;

		ComPtr<ID3D11RasterizerState> cpRasterizerState = nullptr;
		ENSURE(m_cpDevice->CreateRasterizerState(&tRasterizerDesc, cpRasterizerState.GetAddressOf()));
		m_umapRasterizerStates.insert({ L"RS_FILL_SOLID_CULL_NONE", cpRasterizerState });
	}
}

void Engine::CPipelineManager::InitializeBlendStates()
{
	{
		D3D11_BLEND_DESC tBlendDesc;
		ZeroMemory(&tBlendDesc, sizeof(tBlendDesc));
		tBlendDesc.AlphaToCoverageEnable = false;
		tBlendDesc.IndependentBlendEnable = false;

		tBlendDesc.RenderTarget[0].BlendEnable = true;
		tBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		tBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		tBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		tBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		tBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		tBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		tBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		ComPtr<ID3D11BlendState> cpBlendState = nullptr;
		ENSURE(m_cpDevice->CreateBlendState(&tBlendDesc, cpBlendState.GetAddressOf()));
		m_umapBlendStates.insert({ L"BS_ENABLE_BLEND_SRC_INV_SRC", cpBlendState });
	}

	{
		D3D11_BLEND_DESC tBlendDesc;
		ZeroMemory(&tBlendDesc, sizeof(tBlendDesc));
		tBlendDesc.AlphaToCoverageEnable = false;
		tBlendDesc.IndependentBlendEnable = false;

		tBlendDesc.RenderTarget[0].BlendEnable = true;
		tBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		tBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		tBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		tBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		tBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		tBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		tBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		ComPtr<ID3D11BlendState> cpBlendState = nullptr;
		ENSURE(m_cpDevice->CreateBlendState(&tBlendDesc, cpBlendState.GetAddressOf()));
		m_umapBlendStates.insert({ L"BS_ENABLE_BLEND_ONE_ONE", cpBlendState });
	}

	{
		D3D11_BLEND_DESC tBlendDesc;
		ZeroMemory(&tBlendDesc, sizeof(tBlendDesc));
		tBlendDesc.AlphaToCoverageEnable = false;
		tBlendDesc.IndependentBlendEnable = false;

		tBlendDesc.RenderTarget[0].BlendEnable = false;
		tBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		tBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		tBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		tBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		tBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		tBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		tBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		ComPtr<ID3D11BlendState> cpBlendState = nullptr;
		ENSURE(m_cpDevice->CreateBlendState(&tBlendDesc, cpBlendState.GetAddressOf()));
		m_umapBlendStates.insert({ L"BS_DISABLE_BLEND", cpBlendState });
	}
}

void Engine::CPipelineManager::InitializeDepthStencilStates()
{
	{
		D3D11_DEPTH_STENCIL_DESC tDepthStencilDesc;
		ZeroMemory(&tDepthStencilDesc, sizeof(tDepthStencilDesc));
		tDepthStencilDesc.DepthEnable = true;
		tDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		tDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		ComPtr<ID3D11DepthStencilState> cpDepthStencilState = nullptr;
		ENSURE(m_cpDevice->CreateDepthStencilState(&tDepthStencilDesc, cpDepthStencilState.GetAddressOf()));
		m_umapDepthStencilStates.insert({ L"DSS_ENABLE_DEPTH_COMPARE_LESS", cpDepthStencilState });
	}

	{
		D3D11_DEPTH_STENCIL_DESC tDepthStencilDesc;
		ZeroMemory(&tDepthStencilDesc, sizeof(tDepthStencilDesc));
		tDepthStencilDesc.DepthEnable = false;
		tDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		tDepthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;

		ComPtr<ID3D11DepthStencilState> cpDepthStencilState = nullptr;
		ENSURE(m_cpDevice->CreateDepthStencilState(&tDepthStencilDesc, cpDepthStencilState.GetAddressOf()));
		m_umapDepthStencilStates.insert({ L"DSS_DISABLE_DEPTH", cpDepthStencilState });
	}
}

void Engine::CPipelineManager::InitializeSamplerStates()
{
	{
		D3D11_SAMPLER_DESC tSamplerDesc;
		ZeroMemory(&tSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		tSamplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		tSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		tSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		tSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		tSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		tSamplerDesc.MinLOD = 0;
		tSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		ComPtr<ID3D11SamplerState> cpSamplerState = nullptr;
		ENSURE(m_cpDevice->CreateSamplerState(&tSamplerDesc, cpSamplerState.GetAddressOf()));
		m_umapSamplerStates.insert({ L"SS_FILTER_LINEAR_ADDRESS_WRAP", cpSamplerState });
	}

	{
		D3D11_SAMPLER_DESC tSamplerDesc;
		ZeroMemory(&tSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		tSamplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		tSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		tSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		tSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		tSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		tSamplerDesc.MinLOD = 0;
		tSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		ComPtr<ID3D11SamplerState> cpSamplerState = nullptr;
		ENSURE(m_cpDevice->CreateSamplerState(&tSamplerDesc, cpSamplerState.GetAddressOf()));
		m_umapSamplerStates.insert({ L"SS_FILTER_POINT_ADDRESS_WRAP", cpSamplerState });
	}
}

void Engine::CPipelineManager::InitializeRenderTargetViews()
{
	// 백 버퍼 렌더 타겟 뷰
	ComPtr<ID3D11Texture2D> cpBackBufferTexture2D = nullptr;

	HRESULT hr = m_cpSwapChain->GetBuffer(0, IID_PPV_ARGS(cpBackBufferTexture2D.GetAddressOf()));
	// ENSURE(m_cpSwapChain->GetBuffer(0, IID_PPV_ARGS(cpBackBufferTexture2D.GetAddressOf())));
	// ENSURE(m_cpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)cpBackBufferTexture2D.GetAddressOf()));

	ComPtr<ID3D11RenderTargetView> cpBackBufferRenderTargetView = nullptr;
	ENSURE(m_cpDevice->CreateRenderTargetView(cpBackBufferTexture2D.Get(), nullptr, cpBackBufferRenderTargetView.GetAddressOf()));
	m_umapRenderTargetViews.insert({ EViewType::BACKBUFFER, cpBackBufferRenderTargetView });

	D3D11_TEXTURE2D_DESC tTexDesc;
	cpBackBufferTexture2D->GetDesc(&tTexDesc);
	tTexDesc.MipLevels = 1;
	tTexDesc.ArraySize = 1;
	tTexDesc.SampleDesc.Quality = 0;
	tTexDesc.SampleDesc.Count = 1;
	tTexDesc.Usage = D3D11_USAGE_DEFAULT;
	tTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tTexDesc.CPUAccessFlags = 0;
	tTexDesc.MiscFlags = 0;

	// 기초 정보 기록
	{
		tTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;
			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::DIFFUSE,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::DIFFUSE,cpRtv });
		}

		tTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;
			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::NORMAL,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::NORMAL,cpRtv });
		}

		tTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;
			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::BACKBUFFER_COPY,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::BACKBUFFER_COPY,cpRtv });
		}

		tTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;
			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::MRSO,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::MRSO,cpRtv });
		}

		tTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;
			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::EMISSION,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::EMISSION,cpRtv });
		}

		tTexDesc.Format = DXGI_FORMAT_R32_FLOAT;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;
			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::DEPTH,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::DEPTH,cpRtv });
		}
	}

	// 이펙트에 필요한 렌더타겟
	{
		/* EFFECT_GLOW */
		tTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;

			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::GLOW,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::GLOW,cpRtv });
		}
		/* BLOOM */
		tTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;

			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::BLOOM,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::BLOOM,cpRtv });
		}
		/* BLUR_X */
		tTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;

			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::BLUR_X,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::BLUR_X,cpRtv });
		}
		/* BLUR_Y */
		tTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;

			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::BLUR_Y,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::BLUR_Y,cpRtv });
		}
		/* GLOW */
		tTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;

			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::GLOW_RESULT,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::GLOW_RESULT,cpRtv });
		}
		/* DISTORTION */
		tTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;

			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::DISTORTION,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::DISTORTION,cpRtv });
		}
		/* DISTORTION_RESULT */
		tTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;

			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::DISTORTION_RESULT,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::DISTORTION_RESULT,cpRtv });
		}
	}

	// 모자이크 타겟
	{
		tTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;
			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::MOSAIC,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::MOSAIC,cpRtv });
		}
		tTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		{
			ComPtr<ID3D11Texture2D> cpTex = nullptr;
			ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
			ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;
			ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
			ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
			ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
			m_umapShaderResourceViews.insert({ EViewType::MOSAIC_RESULT,cpSrv });
			m_umapRenderTargetViews.insert({ EViewType::MOSAIC_RESULT,cpRtv });
		}
	}

	// 피킹
	tTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	tTexDesc.Width = 1280;
	tTexDesc.Height = 720;
	{
		ComPtr<ID3D11Texture2D> cpTex = nullptr;
		ComPtr<ID3D11ShaderResourceView> cpSrv = nullptr;
		ComPtr<ID3D11RenderTargetView> cpRtv = nullptr;

		D3D11_RENDER_TARGET_VIEW_DESC tRenderTargetViewDesc = {};
		ZeroMemory(&tRenderTargetViewDesc, sizeof(tRenderTargetViewDesc));
		tRenderTargetViewDesc.Format = tTexDesc.Format;
		tRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		tRenderTargetViewDesc.Texture2D.MipSlice = 0;

		ENSURE(m_cpDevice->CreateTexture2D(&tTexDesc, nullptr, cpTex.GetAddressOf()));
		ENSURE(m_cpDevice->CreateShaderResourceView(cpTex.Get(), nullptr, cpSrv.GetAddressOf()));
		ENSURE(m_cpDevice->CreateRenderTargetView(cpTex.Get(), nullptr, cpRtv.GetAddressOf()));
		m_umapShaderResourceViews.insert({ EViewType::WORLDPOSITION,cpSrv });
		m_umapRenderTargetViews.insert({ EViewType::WORLDPOSITION,cpRtv });
	}
}

void Engine::CPipelineManager::InitializeDepthStencilViews()
{
	{
		RECT tRect{};
		::GetClientRect(m_tHWND, &tRect);

		D3D11_TEXTURE2D_DESC tTexture2DDesc;
		ZeroMemory(&tTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
		tTexture2DDesc.Width = tRect.right - tRect.left;
		tTexture2DDesc.Height = tRect.bottom - tRect.top;
		tTexture2DDesc.MipLevels = 1;
		tTexture2DDesc.ArraySize = 1;
		tTexture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		tTexture2DDesc.SampleDesc.Count = 1;
		tTexture2DDesc.SampleDesc.Quality = 0;
		tTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
		tTexture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		tTexture2DDesc.CPUAccessFlags = 0;
		tTexture2DDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> cpTexture2D = nullptr;
		ENSURE(m_cpDevice->CreateTexture2D(&tTexture2DDesc, nullptr, cpTexture2D.GetAddressOf()));

		ComPtr<ID3D11DepthStencilView> cpDepthStencilView = nullptr;
		ENSURE(m_cpDevice->CreateDepthStencilView(cpTexture2D.Get(), nullptr, cpDepthStencilView.GetAddressOf()));
		m_umapDepthStencilViews.insert({ EViewType::BACKBUFFER, cpDepthStencilView });
	}
}

void Engine::CPipelineManager::InitializePipelineViews()
{
	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(6);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::DIFFUSE];
		spPipelineView->m_vecRenderTargetViews[1] = m_umapRenderTargetViews[EViewType::NORMAL];
		spPipelineView->m_vecRenderTargetViews[2] = m_umapRenderTargetViews[EViewType::MRSO];
		spPipelineView->m_vecRenderTargetViews[3] = m_umapRenderTargetViews[EViewType::EMISSION];
		spPipelineView->m_vecRenderTargetViews[4] = m_umapRenderTargetViews[EViewType::DEPTH];
		spPipelineView->m_vecRenderTargetViews[5] = m_umapRenderTargetViews[EViewType::WORLDPOSITION];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.f, 0.f, 0.f, 0.f);
		spPipelineView->m_vecClearColors[1] = Color(0.f, 0.f, 0.f, 0.f);
		spPipelineView->m_vecClearColors[2] = Color(0.f, 0.f, 0.f, 0.f);
		spPipelineView->m_vecClearColors[3] = Color(0.f, 0.f, 0.f, 0.f);
		spPipelineView->m_vecClearColors[4] = Color(1.f, 1.f, 1.f, 1.f);
		spPipelineView->m_vecClearColors[5] = Color(0.f, 0.f, 0.f, -1.f);

		AddPipelineView(EPipelineViewType::DNMEDW, spPipelineView);
	}

	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(2);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecRenderTargetViews[1] = m_umapRenderTargetViews[EViewType::BACKBUFFER_COPY];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.5f, 0.20f, 0.32f, 1.0f);
		spPipelineView->m_vecClearColors[1] = Color(0.5f, 0.20f, 0.32f, 1.0f);

		AddPipelineView(EPipelineViewType::BACKBUFFER, spPipelineView);
	}

	// 04.20 동연 추가 - 이펙트에 필요한 파이프라인뷰가 필요
	/* GLOW_DIFFUSE */
	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(1);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::GLOW];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.f, 0.f, 0.f, 0.f);

		AddPipelineView(EPipelineViewType::GLOW, spPipelineView);
	}

	/* BLOOM */
	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(1);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::BLOOM];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.f, 0.f, 0.f, 0.f);

		AddPipelineView(EPipelineViewType::BLOOM, spPipelineView);
	}

	/* BLUR_X */
	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(1);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::BLUR_X];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.f, 0.f, 0.f, 0.f);

		AddPipelineView(EPipelineViewType::BLUR_X, spPipelineView);
	}

	/* BLUR_Y */
	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(1);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::BLUR_Y];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.f, 0.f, 0.f, 0.f);

		AddPipelineView(EPipelineViewType::BLUR_Y, spPipelineView);
	}

	/* GLOW */
	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(1);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::GLOW_RESULT];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.f, 0.f, 0.f, 0.f);

		AddPipelineView(EPipelineViewType::GLOW_RESULT, spPipelineView);
	}

	/* DISTORTION */
	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(1);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::DISTORTION];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.f, 0.f, 0.f, 0.f);

		AddPipelineView(EPipelineViewType::DISTORTION, spPipelineView);
	}
	/* DISTORTION_RESULT */
	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(1);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::DISTORTION_RESULT];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.f, 0.f, 0.f, 0.f);

		AddPipelineView(EPipelineViewType::DISTORTION_RESULT, spPipelineView);
	}
	/* MOSAIC */
	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(1);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::MOSAIC];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.f, 0.f, 0.f, 0.f);

		AddPipelineView(EPipelineViewType::MOSAIC, spPipelineView);
	}
	/* MOSAIC_RESULT */
	{
		shared_ptr<CPipelineView> spPipelineView = make_shared<CPipelineView>(1);

		spPipelineView->m_vecRenderTargetViews[0] = m_umapRenderTargetViews[EViewType::MOSAIC_RESULT];
		spPipelineView->m_cpDepthStencilView = m_umapDepthStencilViews[EViewType::BACKBUFFER];
		spPipelineView->m_vecClearColors[0] = Color(0.f, 0.f, 0.f, 0.f);

		AddPipelineView(EPipelineViewType::MOSAIC_RESULT, spPipelineView);
	}
}

void Engine::CPipelineManager::InitializeEffectShaders()
{
	m_wpShaderHandler = CShaderHandler::GetInstance();
	m_umapEffectShaders = m_wpShaderHandler.lock()->GetEffectShaders();
}

void Engine::CPipelineManager::InitializeBackBufferCopyTexture()
{
	ClearPipelineView(EPipelineViewType::BACKBUFFER);
	BeginPipelineView(EPipelineViewType::BACKBUFFER);

	ComPtr<ID3D11RenderTargetView> cpBackBufferRTV = nullptr;
	m_cpDeviceContext->OMGetRenderTargets(1, cpBackBufferRTV.GetAddressOf(), nullptr);

	D3D11_TEXTURE2D_DESC tBackBufferCopyDesc;
	ZeroMemory(&tBackBufferCopyDesc, sizeof(D3D11_TEXTURE2D_DESC));

	tBackBufferCopyDesc.Width = 1280;
	tBackBufferCopyDesc.Height = 720;
	tBackBufferCopyDesc.MipLevels = 1;
	tBackBufferCopyDesc.ArraySize = 1;
	tBackBufferCopyDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tBackBufferCopyDesc.SampleDesc.Quality = 0;
	tBackBufferCopyDesc.SampleDesc.Count = 1;
	tBackBufferCopyDesc.Usage = D3D11_USAGE_DEFAULT;
	tBackBufferCopyDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tBackBufferCopyDesc.CPUAccessFlags = 0;
	tBackBufferCopyDesc.MiscFlags = 0;

	ENSURE(m_cpDevice->CreateTexture2D(&tBackBufferCopyDesc, nullptr, &m_cpBackBufferCopyTexture));
}

void Engine::CPipelineManager::InitializePipelineStates()
{
	{
		shared_ptr<CPipelineState> spPipelineState = make_shared<CPipelineState>();

		spPipelineState->m_cpVertexShader = nullptr;
		spPipelineState->m_cpPixelShader = nullptr;
		spPipelineState->m_cpHullShader = nullptr;
		spPipelineState->m_cpComputeShader = nullptr;
		spPipelineState->m_cpGeometryShader = nullptr;
		spPipelineState->m_cpInputLayout = nullptr;

		spPipelineState->m_cpSamplerState = m_umapSamplerStates[L"SS_FILTER_LINEAR_ADDRESS_WRAP"];
		spPipelineState->m_cpBlendState = m_umapBlendStates[L"BS_DISABLE_BLEND"];
		spPipelineState->m_cpDepthStencilState = m_umapDepthStencilStates[L"DSS_ENABLE_DEPTH_COMPARE_LESS"];
		spPipelineState->m_cpRasterizerState = m_umapRasterizerStates[L"RS_FILL_SOLID_CULL_BACK"];
		spPipelineState->m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		AddPipelineState(L"DEFAULT_STATE", spPipelineState);
	}
}

HRESULT Engine::CPipelineManager::AddPipelineState(const std::wstring& _wstrPipelineStateName, const std::shared_ptr<CPipelineState>& _spPipelineState)
{
	if (m_umapPipelineStates.contains(_wstrPipelineStateName)) { return E_FAIL; }

	m_umapPipelineStates.emplace(_wstrPipelineStateName, _spPipelineState);
	return S_OK;
}

HRESULT Engine::CPipelineManager::AddPipelineView(EPipelineViewType _ePipeLineViewType, const std::shared_ptr<CPipelineView>& _spPipelineView)
{
	if (m_umapPipelineViews.contains(_ePipeLineViewType)) { return E_FAIL; }

	m_umapPipelineViews.emplace(_ePipeLineViewType, _spPipelineView);
	return S_OK;
}

std::shared_ptr<CPipelineState> Engine::CPipelineManager::FindPipelineState(const std::wstring& _wstrPipelineStateName)
{
	if (!m_umapPipelineStates.contains(_wstrPipelineStateName)) { return nullptr; }

	return m_umapPipelineStates[_wstrPipelineStateName];
}

std::shared_ptr<CPipelineView> Engine::CPipelineManager::FindPipelineView(EPipelineViewType _ePipeLineViewType)
{
	if (!m_umapPipelineViews.contains(_ePipeLineViewType)) { return nullptr; }

	return m_umapPipelineViews[_ePipeLineViewType];
}

ComPtr<ID3D11ShaderResourceView> Engine::CPipelineManager::FindShaderResourceView(EViewType _eViewType)
{
	if (!m_umapShaderResourceViews.contains(_eViewType)) { return nullptr; }

	return m_umapShaderResourceViews[_eViewType];
}

HRESULT Engine::CPipelineManager::BeginEffectShader(const std::wstring& _wstrEffectShaderName, int32 _iPassIndex, int32 _iTechniqueIndex)
{
	if (!m_umapEffectShaders.contains(_wstrEffectShaderName)) { return E_FAIL; }

	m_umapEffectShaders[_wstrEffectShaderName]->BeginPass(_iPassIndex, _iTechniqueIndex);
	return S_OK;
}

HRESULT Engine::CPipelineManager::BeginPipelineState(const std::wstring& _wstrPipelineName)
{
	if (!m_umapPipelineStates.contains(_wstrPipelineName)) { return E_FAIL; }

	m_umapPipelineStates[_wstrPipelineName]->BeginPipelineState(m_cpDeviceContext);
	return S_OK;
}

HRESULT Engine::CPipelineManager::BeginPipelineView(EPipelineViewType _ePipelineViewType)
{
	if (!m_umapPipelineViews.contains(_ePipelineViewType)) { return E_FAIL; }

	m_umapPipelineViews[_ePipelineViewType]->BeginPipelineView(m_cpDeviceContext);
	return S_OK;
}

void Engine::CPipelineManager::EndEffectShader()
{

}

void Engine::CPipelineManager::EndPipelineState()
{
	m_umapPipelineStates[L"DEFAULT_STATE"]->BeginPipelineState(m_cpDeviceContext);
}

void Engine::CPipelineManager::EndPipelineView()
{
	m_umapPipelineViews[EPipelineViewType::BACKBUFFER]->BeginPipelineView(m_cpDeviceContext);
}

void Engine::CPipelineManager::ClearPipelineView(EPipelineViewType _ePipelineViewType)
{
	// m_cpMultiThread->Enter();

	m_umapPipelineViews[_ePipelineViewType]->ClearPipelineView(m_cpDeviceContext);

	// m_cpMultiThread->Leave();
}

void Engine::CPipelineManager::ClearPipelineViews()
{
	for (auto& Pair : m_umapPipelineViews)
	{
		Pair.second->ClearPipelineView(m_cpDeviceContext);
	}
}

HRESULT Engine::CPipelineManager::CopyBackBufferTexture()
{
	// 현재 렌더 타겟을 가져옵니다.(백버퍼)
	ComPtr<ID3D11RenderTargetView> cpBackBufferRTV = m_umapPipelineViews[EPipelineViewType::BACKBUFFER]->GetRenderTargetView(0);

	// 백버퍼의 렌더타겟 텍스처를 가져옵니다.
	ComPtr<ID3D11Resource> cpBackBufferResource = nullptr;
	cpBackBufferRTV->GetResource(cpBackBufferResource.GetAddressOf());

	// 백버퍼의 텍스처를 m_cpBackBufferCopyTexture로 복사합니다.
	m_cpDeviceContext->CopyResource(m_cpBackBufferCopyTexture.Get(), cpBackBufferResource.Get());

	// ShaderResourceView를 m_cpBackBufferCopyTexture토대로 만듭니다.
	ENSURE(m_cpDevice->CreateShaderResourceView(m_cpBackBufferCopyTexture.Get(), nullptr, m_cpBackBufferSRV.ReleaseAndGetAddressOf()));

	return S_OK;
}
