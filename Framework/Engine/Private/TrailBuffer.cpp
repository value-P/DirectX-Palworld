#include "pch.h"
#include "TrailBuffer.h"

shared_ptr<CTrailBuffer> Engine::CTrailBuffer::Create(const shared_ptr<FTrailBufferDesc>& _spTrailBufferDesc)
{
	const shared_ptr<CTrailBuffer> spTrailBuffer = make_shared<CTrailBuffer>();

	CHECKF(spTrailBuffer != nullptr, L"CTrailBuffer Create Failed");
	spTrailBuffer->Initialize(_spTrailBufferDesc);

	return spTrailBuffer;
}

HRESULT Engine::CTrailBuffer::Initialize(const std::shared_ptr<FTrailBufferDesc>& _spTrailBufferDesc)
{
	__super::Initialize();

	m_iNumRect = _spTrailBufferDesc->m_iNumRect;

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(FPositionTexCoord2);
	m_iNumVertices = (m_iNumRect + 1) * 2;
	m_vecVertexPositions.resize(m_iNumVertices);

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = m_iNumRect * 6;
	m_vecIndices.resize(m_iNumIndices);
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	m_tBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iVertexStride;

	FPositionTexCoord2* pVertices = new FPositionTexCoord2[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(FPositionTexCoord2) * m_iNumVertices);

	m_tInitialData.pSysMem = pVertices;

	HRESULT hrVertexBuffer = __super::CreateBuffer(m_cpVertexBuffer.GetAddressOf());
	
	delete[] pVertices;
	pVertices = nullptr;

	ENSURE(hrVertexBuffer);

#pragma endregion

#pragma region INDEX_BUFFER

	m_tBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = 0;

	uint16* pIndices = new uint16[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(uint16) * m_iNumIndices);

	m_vecIndices.resize(m_iNumIndices);

	uint16 iRectCnt = 0;
	for (uint16 i = 0; i < m_iNumIndices;)
	{
		pIndices[i++] = (iRectCnt * 2) + 3;
		pIndices[i++] = (iRectCnt * 2) + 1;
		pIndices[i++] = (iRectCnt * 2);

		pIndices[i++] = (iRectCnt * 2) + 3;
		pIndices[i++] = (iRectCnt * 2);
		pIndices[i++] = (iRectCnt * 2) + 2;

		++iRectCnt;
	}

	m_tInitialData.pSysMem = pIndices;

	HRESULT hrIndexBuffer = __super::CreateBuffer(m_cpIndexBuffer.GetAddressOf());

	delete[] pIndices;
	pIndices = nullptr;

	ENSURE(hrIndexBuffer);

#pragma endregion

	return S_OK;
}