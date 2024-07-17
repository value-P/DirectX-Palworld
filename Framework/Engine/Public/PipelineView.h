#pragma once

namespace Engine
{
	class CDeviceManager;
	class CRectBuffer;
	class CEffectShader;

	class ENGINE_DLL CPipelineView
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPipelineView(int32 _iNumViews);

		virtual ~CPipelineView() = default;
		
	/********************
		Events
	********************/
	public:
		void Release();
		
	/********************
		Methods
	********************/
	public:
		void BeginPipelineView(const ComPtr<ID3D11DeviceContext>& _cpDeviceContext);

		void ClearPipelineView(const ComPtr<ID3D11DeviceContext>& _cpDeviceContext);

		const ComPtr<ID3D11RenderTargetView> GetRenderTargetView(uint16 _iIndex);

	/********************
		Data Members
	********************/
	public:
		std::vector<ComPtr<ID3D11RenderTargetView>> m_vecRenderTargetViews;

		std::vector<Color> m_vecClearColors;

		ComPtr<ID3D11DepthStencilView> m_cpDepthStencilView = nullptr;
	};
}
