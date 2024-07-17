#include "pch.h"
#include "Instancing.h"

#include "ParticleComponent.h"
#include "MathUtility.h"

std::shared_ptr<CInstancing> Engine::CInstancing::Create(FParticleInfo _tParticleInfo)
{
	const std::shared_ptr<CInstancing> spInstance = make_shared<CInstancing>();

	CHECKF(spInstance != nullptr, L"CInstancing Create Failed");
	spInstance->SetParticleInfo(_tParticleInfo);
	spInstance->Initialize();

	return spInstance;
}

HRESULT Engine::CInstancing::Initialize()
{
	__super::Initialize();

	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(FPoint);
	m_iNumVertices = 1;

	m_iIndexCountPerInstance = 1;
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = m_iIndexCountPerInstance * m_tParticleInfo.m_iNumInstance;
	m_vecIndices.resize(m_iNumIndices);
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER

	m_tBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iVertexStride;

	FPoint* pVertices = new FPoint[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(FPoint) * m_iNumVertices);

	pVertices[0].m_vPosition = Vector3(0.f, 0.f, 0.f);
	pVertices[0].m_vPSize = Vector2(1.f, 1.f);

	m_tInitialData.pSysMem = pVertices;

	HRESULT resultVB = __super::CreateBuffer(m_cpVertexBuffer.GetAddressOf());

	delete[] pVertices;
	pVertices = nullptr;

	ENSURE(resultVB);

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

	m_tInitialData.pSysMem = pIndices;

	HRESULT resultIB = __super::CreateBuffer(m_cpIndexBuffer.GetAddressOf());

	delete[] pIndices;
	pIndices = nullptr;

	ENSURE(resultIB);

#pragma endregion

#pragma region INSTANCE_BUFFER

	m_iInstanceStride = sizeof(FRightUpLookPositionColor);

	m_tBufferDesc.ByteWidth = m_iInstanceStride * m_tParticleInfo.m_iNumInstance;
	m_tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iVertexStride;

	FRightUpLookPositionColor* pInstances = new FRightUpLookPositionColor[m_tParticleInfo.m_iNumInstance];
	ZeroMemory(pInstances, sizeof(m_tParticleInfo.m_iNumInstance) * m_tParticleInfo.m_iNumInstance);

	for (int32 i = 0; i < m_tParticleInfo.m_iNumInstance; ++i)
	{
		float fSize = CMathUtility::GetRandomFloat(m_tParticleInfo.m_vSize.x, m_tParticleInfo.m_vSize.y);

		pInstances[i].m_vRight			= Vector4(fSize, 0.f, 0.f, 0.f);
		pInstances[i].m_vUp				= Vector4(0.f, fSize, 0.f, 0.f);
		pInstances[i].m_vLook			= Vector4(0.f, 0.f, fSize, 0.f);
		
		if (EParticleType::REVOLUTION == m_tParticleInfo.m_eParticleType)
		{
			Vector4 vZ = Vector4(0.f, 0.f, m_tParticleInfo.m_vRadius.x, 0.f);
			Matrix matRotationY = Matrix::CreateRotationY(XMConvertToRadians((360.f / m_tParticleInfo.m_iNumInstance) * (float)i));

			vZ = Vector4::Transform(vZ, matRotationY);
			vZ.w = 1.f;
			vZ *= m_tParticleInfo.m_vRadius.x;

			pInstances[i].m_vTranslation = vZ;
		}
		else if (EParticleType::TONADO == m_tParticleInfo.m_eParticleType)
		{
			pInstances[i].m_vTranslation = Vector4(m_tParticleInfo.m_vOffset.x + CMathUtility::GetRandomFloat(0.f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
												   m_tParticleInfo.m_vOffset.y + CMathUtility::GetRandomFloat(0.f, 1.f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
												   m_tParticleInfo.m_vOffset.z + CMathUtility::GetRandomFloat(0.f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y), 1.f);
		}
		else if (EParticleType::GROUNDED == m_tParticleInfo.m_eParticleType)
		{
			pInstances[i].m_vTranslation = Vector4(m_tParticleInfo.m_vOffset.x + CMathUtility::GetRandomFloat(0.f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
												   m_tParticleInfo.m_vOffset.y + 0.5f,
												   m_tParticleInfo.m_vOffset.z + CMathUtility::GetRandomFloat(0.f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y), 1.f);
		}
		else
		{
			pInstances[i].m_vTranslation = Vector4(m_tParticleInfo.m_vOffset.x + CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
												   m_tParticleInfo.m_vOffset.y + CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y),
												   m_tParticleInfo.m_vOffset.z + CMathUtility::GetRandomFloat(-0.5f, 0.5f) * CMathUtility::GetRandomFloat(m_tParticleInfo.m_vRadius.x, m_tParticleInfo.m_vRadius.y), 1.f);
		}
		if (EParticleType::ALPHADECREASE == m_tParticleInfo.m_eParticleType)
		{
			pInstances[i].m_vColor = m_tParticleInfo.m_vColor1;
			pInstances[i].m_vColor.w = CMathUtility::GetRandomFloat(0.5f, 1.f);
		}
		else
		{
			pInstances[i].m_vColor = m_tParticleInfo.m_vColor1;
			// pInstances[i].m_vColor.w = CMathUtility::GetRandomFloat(0.5f, 1.f);
		}

		pInstances[i].m_vMinUV = Vector2(0.f, 0.f);
		pInstances[i].m_vMaxUV = Vector2(1.f / (float)m_tParticleInfo.m_iSpriteRow, 1.f / (float)m_tParticleInfo.m_iSpriteCol);
	}

	m_tInitialData.pSysMem = pInstances;

	HRESULT resultInstanceBuffer = __super::CreateBuffer(m_cpInstanceBuffer.GetAddressOf());

	ENSURE(resultInstanceBuffer);

	delete[] pInstances;
	pInstances = nullptr;

#pragma endregion INSTANCE_BUFFER

	return S_OK;
}


HRESULT Engine::CInstancing::Render()
{
	BindBuffers();
	m_cpDeviceContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_tParticleInfo.m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT Engine::CInstancing::BindBuffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_cpVertexBuffer.Get(),
		m_cpInstanceBuffer.Get()
	};

	uint32 iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	uint32 iOffsets[] = {
		0,
		0
	};

	m_cpDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_cpDeviceContext->IASetIndexBuffer(m_cpIndexBuffer.Get(), m_eIndexFormat, 0);
	m_cpDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}
