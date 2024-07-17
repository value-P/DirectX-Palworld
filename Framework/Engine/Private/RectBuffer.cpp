#include "pch.h"
#include "RectBuffer.h"

std::shared_ptr<CRectBuffer> Engine::CRectBuffer::Create()
{
	const std::shared_ptr<CRectBuffer> spInstance = make_shared<CRectBuffer>();

	CHECKF(spInstance != nullptr, L"CRectBuffer Create Failed");
	spInstance->Initialize();

	return spInstance;
}

HRESULT Engine::CRectBuffer::Initialize()
{
	__super::Initialize();

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(FPositionTexCoord2);
	m_iNumVertices = 4;
	m_vecVertexPositions.resize(m_iNumVertices);

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = 6;
	m_vecIndices.resize(m_iNumIndices);
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	m_tBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iVertexStride;

	std::vector<FPositionTexCoord2> vecVertices;
	vecVertices.resize(m_iNumVertices);

	m_vecVertexPositions[0] = vecVertices[0].m_vPosition = Vector3(-0.5f, 0.5f, 0.f);
	vecVertices[0].m_vTexCoord2 = Vector2(0.f, 0.f);

	m_vecVertexPositions[1] = vecVertices[1].m_vPosition = Vector3(0.5f, 0.5f, 0.f);
	vecVertices[1].m_vTexCoord2 = Vector2(1.f, 0.f);

	m_vecVertexPositions[2] = vecVertices[2].m_vPosition = Vector3(0.5f, -0.5f, 0.f);
	vecVertices[2].m_vTexCoord2 = Vector2(1.f, 1.f);

	m_vecVertexPositions[3] = vecVertices[3].m_vPosition = Vector3(-0.5f, -0.5f, 0.f);
	vecVertices[3].m_vTexCoord2 = Vector2(0.f, 1.f);

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
	m_vecIndices[1] = vecIndices[1] = 1;
	m_vecIndices[2] = vecIndices[2] = 2;

	m_vecIndices[3] = vecIndices[3] = 0;
	m_vecIndices[4] = vecIndices[4] = 2;
	m_vecIndices[5] = vecIndices[5] = 3;

	m_tInitialData.pSysMem = vecIndices.data();

	HRESULT resultIB = __super::CreateBuffer(m_cpIndexBuffer.GetAddressOf());

	ENSURE(resultIB);

#pragma endregion

	return S_OK;
}
