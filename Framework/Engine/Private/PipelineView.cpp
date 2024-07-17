#include "pch.h"
#include "PipelineView.h"

Engine::CPipelineView::CPipelineView(int32 _iNumViews)
{
	m_vecRenderTargetViews.resize(_iNumViews);
	m_vecRenderTargetViews.reserve(_iNumViews);
	m_vecClearColors.resize(_iNumViews);
	m_vecClearColors.reserve(_iNumViews);
}

void Engine::CPipelineView::Release()
{
	for (auto& cpRenderTargetView : m_vecRenderTargetViews)
	{
		cpRenderTargetView = nullptr;
	}
	m_vecRenderTargetViews.clear();

	m_cpDepthStencilView = nullptr;
}

void Engine::CPipelineView::BeginPipelineView(const ComPtr<ID3D11DeviceContext>& _cpDeviceContext)
{
	UINT iNumViews = static_cast<UINT>(m_vecRenderTargetViews.size());
	_cpDeviceContext->OMSetRenderTargets(iNumViews, m_vecRenderTargetViews.data()->GetAddressOf(), m_cpDepthStencilView.Get());
}

void Engine::CPipelineView::ClearPipelineView(const ComPtr<ID3D11DeviceContext>& _cpDeviceContext)
{
	for (int32 iInnerIndex = 0; iInnerIndex < m_vecRenderTargetViews.size(); ++iInnerIndex)
	{
		_cpDeviceContext->ClearRenderTargetView(m_vecRenderTargetViews[iInnerIndex].Get(), reinterpret_cast<float*>(&m_vecClearColors[iInnerIndex]));
	}

	_cpDeviceContext->ClearDepthStencilView(m_cpDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.f, 0);
}

const ComPtr<ID3D11RenderTargetView> Engine::CPipelineView::GetRenderTargetView(uint16 _iIndex)
{
	if (_iIndex > m_vecRenderTargetViews.size() - 1) { return nullptr; }

	return m_vecRenderTargetViews[_iIndex];
}
