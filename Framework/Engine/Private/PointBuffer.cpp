#include "pch.h"
#include "PointBuffer.h"

std::shared_ptr<CPointBuffer> Engine::CPointBuffer::Create()
{
	const std::shared_ptr<CPointBuffer> spPointBuffer = make_shared<CPointBuffer>();
	ENSUREF(spPointBuffer->Initialize(), L"Failed to initialize point buffer.");
	return spPointBuffer;
}

HRESULT Engine::CPointBuffer::Initialize()
{
	__super::Initialize();

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(FPositionSize);
	m_iNumVertices = 1;
	m_vecVertexPositions.resize(m_iNumVertices);

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = 1;
	m_vecIndices.resize(m_iNumIndices);
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER

	m_tBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iVertexStride;

	std::vector<FPositionSize> vecVertices;
	vecVertices.resize(m_iNumVertices);

	m_vecVertexPositions[0] = vecVertices[0].m_vPosition = Vector3(0.f, 0.f, 0.f);
	vecVertices[0].m_vPSize = Vector2(1.f, 1.f);

	m_tInitialData.pSysMem = vecVertices.data();

	HRESULT resultVB = __super::CreateBuffer(m_cpVertexBuffer.GetAddressOf());
	
	ENSURE(resultVB);

#pragma endregion

#pragma region INDEX_BUFFER

	m_tBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iIndexStride;

	std::vector<uint16> vecIndices;
	vecIndices.resize(m_iNumIndices);

	m_vecIndices[0] = vecIndices[0] = 0;
	
	m_tInitialData.pSysMem = vecIndices.data();

	HRESULT resultIB = __super::CreateBuffer(m_cpIndexBuffer.GetAddressOf());

	ENSURE(resultIB);

#pragma endregion

	return S_OK;
}

HRESULT Engine::CPointBuffer::Render()
{
	//CBuffer::Render();

	BindBuffers();

	m_cpDeviceContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

void Engine::CPointBuffer::Release()
{
	m_cpDeviceContext.Reset();
}

HRESULT Engine::CPointBuffer::BindBuffers()
{
	ID3D11Buffer* pVertexBuffers[] = { m_cpVertexBuffer.Get(), };

	uint32 iVertexStrides[] = { m_iVertexStride, };

	uint32 iOffsets[] = { 0, };

	m_cpDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_cpDeviceContext->IASetIndexBuffer(m_cpIndexBuffer.Get(), m_eIndexFormat, 0);
	m_cpDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}
