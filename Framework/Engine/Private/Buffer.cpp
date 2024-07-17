#include "pch.h"
#include "Buffer.h"
#include "EngineModule.h"
#include "DeviceManager.h"

HRESULT Engine::CBuffer::Initialize()
{
	m_cpDevice = CEngineModule::GetInstance()->GetDeviceManager()->GetDevice();
	m_cpDeviceContext = CEngineModule::GetInstance()->GetDeviceManager()->GetDeviceContext();

#ifdef _DEBUG
	CHECKF(m_cpDevice != nullptr, L"CBuffer Initialize Failed");
	CHECKF(m_cpDeviceContext != nullptr, L"CBuffer Initialize Failed");
#endif

	return S_OK;
}

HRESULT Engine::CBuffer::Render()
{

	BindBuffers();

	m_cpDeviceContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

void Engine::CBuffer::Release()
{
	m_vecVertexPositions.clear();
	m_vecIndices.clear();

	m_vecVertexPositions.shrink_to_fit();
	m_vecIndices.shrink_to_fit();

	m_cpDevice.Reset();
	m_cpDeviceContext.Reset();	
	m_cpVertexBuffer.Reset();
	m_cpIndexBuffer.Reset();
	m_cpDevice.Reset();
	m_cpDeviceContext.Reset();
}

HRESULT Engine::CBuffer::BindBuffers()
{
	ID3D11Buffer* pVertexBuffers[] = { m_cpVertexBuffer.Get(),};

	uint32 iVertexStrides[] = { m_iVertexStride, };

	uint32 iOffsets[] = { 0, };

	m_cpDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_cpDeviceContext->IASetIndexBuffer(m_cpIndexBuffer.Get(), m_eIndexFormat, 0);
	m_cpDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT Engine::CBuffer::CreateBuffer(ID3D11Buffer** _ppBuffer)
{
	HRESULT hr = m_cpDevice->CreateBuffer(&m_tBufferDesc, &m_tInitialData, _ppBuffer);

#ifdef _DEBUG
	ENSUREF(hr, L"CBuffer Failed CreateBuffer");
#endif

	return S_OK;
}