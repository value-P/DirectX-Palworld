#include "pch.h"
#include "CubeBuffer.h"

std::shared_ptr<CCubeBuffer> Engine::CCubeBuffer::Create()
{
	const std::shared_ptr<CCubeBuffer> spInstance = make_shared<CCubeBuffer>();

	CHECKF(spInstance != nullptr, L"CCubeBuffer Create Failed");
	spInstance->Initialize();

	return spInstance;
}

HRESULT Engine::CCubeBuffer::Initialize()
{
	__super::Initialize();

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(FPositionTexCoord3);
	m_iNumVertices = 8;
	m_vecVertexPositions.resize(m_iNumVertices);

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = 36;
	m_vecIndices.resize(m_iNumIndices);
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	m_tBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iVertexStride;

	std::vector<FPositionTexCoord3> vecVertices;
	vecVertices.resize(m_iNumVertices);

	m_vecVertexPositions[0] = vecVertices[0].m_vPosition = Vector3(-0.5f, 0.5f, -0.5f);
	vecVertices[0].m_vTexCoord3 = vecVertices[0].m_vPosition;

	m_vecVertexPositions[1] = vecVertices[1].m_vPosition = Vector3(0.5f, 0.5f, -0.5f);
	vecVertices[1].m_vTexCoord3 = vecVertices[1].m_vPosition;

	m_vecVertexPositions[2] = vecVertices[2].m_vPosition = Vector3(0.5f, -0.5f, -0.5f);
	vecVertices[2].m_vTexCoord3 = vecVertices[2].m_vPosition;

	m_vecVertexPositions[3] = vecVertices[3].m_vPosition = Vector3(-0.5f, -0.5f, -0.5f);
	vecVertices[3].m_vTexCoord3 = vecVertices[3].m_vPosition;

	m_vecVertexPositions[4] = vecVertices[4].m_vPosition = Vector3(-0.5f, 0.5f, 0.5f);
	vecVertices[4].m_vTexCoord3 = vecVertices[4].m_vPosition;

	m_vecVertexPositions[5] = vecVertices[5].m_vPosition = Vector3(0.5f, 0.5f, 0.5f);
	vecVertices[5].m_vTexCoord3 = vecVertices[5].m_vPosition;

	m_vecVertexPositions[6] = vecVertices[6].m_vPosition = Vector3(0.5f, -0.5f, 0.5f);
	vecVertices[6].m_vTexCoord3 = vecVertices[6].m_vPosition;

	m_vecVertexPositions[7] = vecVertices[7].m_vPosition = Vector3(-0.5f, -0.5f, 0.5f);
	vecVertices[7].m_vTexCoord3 = vecVertices[7].m_vPosition;

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

	/* +X */
	m_vecIndices[0] = vecIndices[0] = 1;
	m_vecIndices[1] = vecIndices[1] = 5;
	m_vecIndices[2] = vecIndices[2] = 6;

	m_vecIndices[3] = vecIndices[3] = 1;
	m_vecIndices[4] = vecIndices[4] = 6;
	m_vecIndices[5] = vecIndices[5] = 2;

	/* -X */
	m_vecIndices[6] = vecIndices[6] = 4;
	m_vecIndices[7] = vecIndices[7] = 0;
	m_vecIndices[8] = vecIndices[8] = 3;

	m_vecIndices[9] = vecIndices[9] = 4;
	m_vecIndices[10] = vecIndices[10] = 3;
	m_vecIndices[11] = vecIndices[11] = 7;

	/* +Y */
	m_vecIndices[12] = vecIndices[12] = 4;
	m_vecIndices[13] = vecIndices[13] = 5;
	m_vecIndices[14] = vecIndices[14] = 1;

	m_vecIndices[15] = vecIndices[15] = 4;
	m_vecIndices[16] = vecIndices[16] = 1;
	m_vecIndices[17] = vecIndices[17] = 0;

	/* -Y */
	m_vecIndices[18] = vecIndices[18] = 3;
	m_vecIndices[19] = vecIndices[19] = 2;
	m_vecIndices[20] = vecIndices[20] = 6;

	m_vecIndices[21] = vecIndices[21] = 3;
	m_vecIndices[22] = vecIndices[22] = 6;
	m_vecIndices[23] = vecIndices[23] = 7;

	/* +Z */
	m_vecIndices[24] = vecIndices[24] = 5;
	m_vecIndices[25] = vecIndices[25] = 4;
	m_vecIndices[26] = vecIndices[26] = 7;

	m_vecIndices[27] = vecIndices[27] = 5;
	m_vecIndices[28] = vecIndices[28] = 7;
	m_vecIndices[29] = vecIndices[29] = 6;

	/* -Z */
	m_vecIndices[30] = vecIndices[30] = 0;
	m_vecIndices[31] = vecIndices[31] = 1;
	m_vecIndices[32] = vecIndices[32] = 2;

	m_vecIndices[33] = vecIndices[33] = 0;
	m_vecIndices[34] = vecIndices[34] = 2;
	m_vecIndices[35] = vecIndices[35] = 3;

	m_tInitialData.pSysMem = vecIndices.data();

	HRESULT resultIB = __super::CreateBuffer(m_cpIndexBuffer.GetAddressOf());

	ENSURE(resultIB);

#pragma endregion

	return S_OK;
}
