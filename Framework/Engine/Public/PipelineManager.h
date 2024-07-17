#pragma once
#include "Manager.h"

namespace Engine
{
	class CDeviceManager;
	class CShaderHandler;
	class CPipelineState;
	class CPipelineView;
	class CEffectShader;
	class CRectBuffer;

	class ENGINE_DLL CPipelineManager final : public CManager
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		DECLARE_SINGLETON(CPipelineManager)

		/********************
			Events
		********************/
	public:
		HRESULT Initialize();

		virtual void Release() override;

		/********************
			Methods
		********************/
	private:
		void InitializeRasterizerStates();

		void InitializeBlendStates();

		void InitializeDepthStencilStates();

		void InitializeSamplerStates();

		void InitializeRenderTargetViews();

		void InitializeDepthStencilViews();

	public:
		void InitializePipelineStates();

		void InitializePipelineViews();

		void InitializeEffectShaders();

		void InitializeBackBufferCopyTexture();

	public:
		HRESULT AddPipelineState(const std::wstring& _wstrPipelineStateName, const std::shared_ptr<CPipelineState>& _spPipelineState);

		HRESULT AddPipelineView(EPipelineViewType _ePipelineViewType, const std::shared_ptr<CPipelineView>& _spPipelineView);

		std::shared_ptr<CPipelineState> FindPipelineState(const std::wstring& _wstrPipelineStateName);

		std::shared_ptr<CPipelineView> FindPipelineView(EPipelineViewType _ePipelineViewType);

		ComPtr<ID3D11ShaderResourceView> FindShaderResourceView(EViewType _eViewType);

	public:
		HRESULT BeginEffectShader(const std::wstring& _wstrEffectShaderName, int32 _iPassIndex, int32 _iTechniqueIndex = 0);

		HRESULT BeginPipelineState(const std::wstring& _wstrPipelineName);

		HRESULT BeginPipelineView(EPipelineViewType _ePipelineViewType);

		void EndEffectShader();

		void EndPipelineState();

		void EndPipelineView();

		void ClearPipelineView(EPipelineViewType _ePipelineViewType);

		void ClearPipelineViews();

		HRESULT CopyBackBufferTexture();

		ComPtr<ID3D11ShaderResourceView> GetBackBufferCopySRV() { return m_cpBackBufferSRV; }

		/********************
			Data Members
		********************/
	private:
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		ComPtr<IDXGISwapChain> m_cpSwapChain = nullptr;

		ComPtr<ID3D11Multithread> m_cpMultiThread = nullptr;

	private:
		// 백 버퍼 카피를 위해 추가 -동연-
		ComPtr<ID3D11Texture2D> m_cpBackBufferCopyTexture = nullptr;
		ComPtr<ID3D11ShaderResourceView> m_cpBackBufferSRV = nullptr;

	private:
		// Rasterizer States
		std::unordered_map<std::wstring, ComPtr<ID3D11RasterizerState>> m_umapRasterizerStates;

		// Depth Stencil States
		std::unordered_map<std::wstring, ComPtr<ID3D11DepthStencilState>> m_umapDepthStencilStates;

		// Blend States
		std::unordered_map<std::wstring, ComPtr<ID3D11BlendState>> m_umapBlendStates;

		// Sampler States
		std::unordered_map<std::wstring, ComPtr<ID3D11SamplerState>> m_umapSamplerStates;

		// Render Target Views
		std::unordered_map<EViewType, ComPtr<ID3D11RenderTargetView>> m_umapRenderTargetViews;

		// Shader Resource Views
		std::unordered_map<EViewType, ComPtr<ID3D11ShaderResourceView>> m_umapShaderResourceViews;

		// Depth Stencil Views
		std::unordered_map<EViewType, ComPtr<ID3D11DepthStencilView>> m_umapDepthStencilViews;

	private:
		// Pipeline Views
		std::unordered_map<EPipelineViewType, std::shared_ptr<CPipelineView>> m_umapPipelineViews;

		// Pipeline States
		std::unordered_map<wstring, std::shared_ptr<CPipelineState>> m_umapPipelineStates;

		// Effect Shaders
		std::unordered_map<wstring, std::shared_ptr<CEffectShader>> m_umapEffectShaders;

	private:
#ifdef _DEBUG
		uint32 m_iDebugState = 0;
#endif _DEBUG
		std::weak_ptr<CDeviceManager> m_wpDeviceManager;

		std::weak_ptr<CShaderHandler> m_wpShaderHandler;

		HWND m_tHWND{};
	};
}