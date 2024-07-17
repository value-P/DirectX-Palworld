#include "pch.h"
#include "PipelineState.h"

void Engine::CPipelineState::Release()
{
	m_cpVertexShader		= nullptr;
	m_cpPixelShader			= nullptr;
	m_cpHullShader			= nullptr;
	m_cpDomainShader		= nullptr;
	m_cpGeometryShader		= nullptr;
	m_cpInputLayout			= nullptr;
	m_cpBlendState			= nullptr;
	m_cpDepthStencilState	= nullptr;
	m_cpRasterizerState		= nullptr;
}

void Engine::CPipelineState::SetBlendFactor(const float _fBlendFactor[4])
{
	::memcpy(m_fBlendFactor, _fBlendFactor, sizeof(float) * 4);
}

void Engine::CPipelineState::BeginPipelineState(const ComPtr<ID3D11DeviceContext>& _cpDeviceContext)
{
	_cpDeviceContext->VSSetShader				(m_cpVertexShader.Get(), 0, 0);
	_cpDeviceContext->PSSetShader				(m_cpPixelShader.Get(), 0, 0);
	_cpDeviceContext->HSSetShader				(m_cpHullShader.Get(), 0, 0);
	_cpDeviceContext->DSSetShader				(m_cpDomainShader.Get(), 0, 0);
	_cpDeviceContext->GSSetShader				(m_cpGeometryShader.Get(), 0, 0);
	_cpDeviceContext->IASetInputLayout			(m_cpInputLayout.Get());
	_cpDeviceContext->RSSetState				(m_cpRasterizerState.Get());
	_cpDeviceContext->OMSetBlendState			(m_cpBlendState.Get(), m_fBlendFactor, 0xffff'ffff);
	_cpDeviceContext->OMSetDepthStencilState	(m_cpDepthStencilState.Get(), m_iStencilRef);
	_cpDeviceContext->IASetPrimitiveTopology	(m_primitiveTopology);
}
