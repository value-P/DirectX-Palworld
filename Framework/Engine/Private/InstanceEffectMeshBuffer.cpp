#include "pch.h"
#include "InstanceEffectMeshBuffer.h"
#include "MathUtility.h"
#include "Material.h"

shared_ptr<CInstanceEffectMeshBuffer> Engine::CInstanceEffectMeshBuffer::Create(const wstring& _wstrModelFilePath, ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo)
{
	shared_ptr<CInstanceEffectMeshBuffer> spInstanceEffectMeshBuffer = make_shared<CInstanceEffectMeshBuffer>();
	ENSUREF(spInstanceEffectMeshBuffer->Initialize(_wstrModelFilePath, _ifFile, _matPivot, _eModelType, _tInstanceEffectMeshInfo), L"Failed to initialize instance mesh buffer.");
	return spInstanceEffectMeshBuffer;
}

HRESULT Engine::CInstanceEffectMeshBuffer::Initialize(const wstring& _wstrModelFilePath, ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo)
{
	if (FAILED(CBuffer::Initialize())) { return E_FAIL; }

	m_tInstanceEffectMeshInfo = _tInstanceEffectMeshInfo;

	InitializeNameBoneOffset(_ifFile);
	InitializeVertexBuffer(_ifFile, _matPivot, _eModelType);
	InitializeIndexBuffer(_ifFile);

	m_spMaterial = CMaterial::Create(_wstrModelFilePath, _ifFile, _eModelType, EMeshManageType::UNIQUE);
	m_eShaderType = m_spMaterial->GetShaderType();

	InitializeInstanceBuffer(_tInstanceEffectMeshInfo);
	return S_OK;
}

HRESULT Engine::CInstanceEffectMeshBuffer::Render()
{
	BindBuffers();
	m_cpDeviceContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_tInstanceEffectMeshInfo.m_iNumInstance, 0, 0, 0);
	return S_OK;
}
HRESULT Engine::CInstanceEffectMeshBuffer::BindBuffers()
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

void Engine::CInstanceEffectMeshBuffer::InitializeNameBoneOffset(ifstream& _ifFile)
{
	_ifFile.read(reinterpret_cast<char*>(&m_iNumVertices), sizeof(m_iNumVertices));
	_ifFile.read(reinterpret_cast<char*>(&m_iNumIndices), sizeof(m_iNumIndices));

	int32 iBoneSize = 0;
	_ifFile.read(reinterpret_cast<char*>(&iBoneSize), sizeof(iBoneSize));
	for (size_t i = 0; i < iBoneSize; ++i)
	{
		int32 iBoneNameLen = 0;
		wchar_t wszBoneName[MAX_PATH] = {};
		_ifFile.read(reinterpret_cast<char*>(&iBoneNameLen), sizeof(iBoneNameLen));
		_ifFile.read(reinterpret_cast<char*>(wszBoneName), iBoneNameLen * sizeof(wchar_t));
		m_vecBoneNames.push_back(wszBoneName);

		uint32 iBoneIndex = 0;
		_ifFile.read(reinterpret_cast<char*>(&iBoneIndex), sizeof(iBoneIndex));
		m_vecBoneIndices.push_back(iBoneIndex);
	}

	int32 iOffsetSize = 0;
	_ifFile.read(reinterpret_cast<char*>(&iOffsetSize), sizeof(iOffsetSize));
	for (size_t i = 0; i < iBoneSize; ++i)
	{
		Matrix matOffset;
		for (size_t j = 0; j < 4; ++j)
		{
			for (size_t k = 0; k < 4; ++k)
			{
				_ifFile.read(reinterpret_cast<char*>(&matOffset.m[j][k]), sizeof(matOffset.m[j][k]));
			}
		}
		m_vecOffsetMatrices.push_back(matOffset);
	}
}
void Engine::CInstanceEffectMeshBuffer::InitializeVertexBuffer(ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType)
{
	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(FPositionNormalTexCoord2Tangent);

	m_iIndexCountPerInstance = m_iNumIndices;
	m_iIndexStride = 4;
	m_iNumIndices = m_iIndexCountPerInstance * m_tInstanceEffectMeshInfo.m_iNumInstance;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	m_vecVertexPositions.resize(m_iNumVertices);
	switch (_eModelType)
	{
	case EModelType::ANIM:
		InitVertexBuffer_Anim(_ifFile, _matPivot);
		break;
	case EModelType::NONANIM:
	case EModelType::MAP_OBJECT:
	case EModelType::MAP_OBJECT_COLLISION:
	case EModelType::MAP_DEBUG:
	case EModelType::EXTERN:
		InitVertexBuffer_NonAnim(_ifFile, _matPivot);
		break;
	case EModelType::MAP_LANDSCAPE:
		InitVertexBuffer_Landscape(_ifFile, _matPivot);
		break;
	}
}
void Engine::CInstanceEffectMeshBuffer::InitializeIndexBuffer(ifstream& _ifFile)
{
	m_tBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = 0;

	uint32* pIndices = new uint32[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(uint32) * m_iNumIndices);

	m_vecIndices.resize(m_iNumIndices);
	for (int32 iOuterIndex = 0; iOuterIndex < m_tInstanceEffectMeshInfo.m_iNumInstance; ++iOuterIndex)
	{
		for (size_t iInnerIndex = 0; iInnerIndex < m_iIndexCountPerInstance; ++iInnerIndex)
		{
			_ifFile.read(reinterpret_cast<char*>(&pIndices[iInnerIndex]), sizeof(pIndices[iInnerIndex]));
			m_vecIndices[(iOuterIndex * m_iIndexCountPerInstance) + iInnerIndex] = pIndices[iInnerIndex];
		}
	}

	m_tInitialData.pSysMem = pIndices;

	HRESULT resultIB = __super::CreateBuffer(m_cpIndexBuffer.GetAddressOf());

	delete[] pIndices;
	pIndices = nullptr;

	ENSURE(resultIB);
}
void Engine::CInstanceEffectMeshBuffer::InitializeInstanceBuffer(FInstanceEffectMeshInfo _tInstanceEffectMeshInfo)
{
	m_iInstanceStride = sizeof(FRightUpLookPositionPrevPosition);

	m_tBufferDesc.ByteWidth = m_iInstanceStride * _tInstanceEffectMeshInfo.m_iNumInstance;
	m_tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iVertexStride;

	FRightUpLookPositionPrevPosition* pInstances = new FRightUpLookPositionPrevPosition[_tInstanceEffectMeshInfo.m_iNumInstance];
	ZeroMemory(pInstances, sizeof(_tInstanceEffectMeshInfo.m_iNumInstance) * _tInstanceEffectMeshInfo.m_iNumInstance);

	for (int32 i = 0; i < _tInstanceEffectMeshInfo.m_iNumInstance; ++i)
	{
		float fSize = CMathUtility::GetRandomFloat(_tInstanceEffectMeshInfo.m_vSize.x, _tInstanceEffectMeshInfo.m_vSize.y);

		pInstances[i].m_vRight = Vector4(fSize, 0.f, 0.f, 0.f);
		pInstances[i].m_vUp = Vector4(0.f, fSize, 0.f, 0.f);
		pInstances[i].m_vLook = Vector4(0.f, 0.f, fSize, 0.f);
		pInstances[i].m_vTranslation = Vector4
		(
			CMathUtility::GetRandomFloat(_tInstanceEffectMeshInfo.m_vRange.x * -0.5f, _tInstanceEffectMeshInfo.m_vRange.x * 0.5f) * CMathUtility::GetRandomFloat(_tInstanceEffectMeshInfo.m_vRadius.x, _tInstanceEffectMeshInfo.m_vRadius.y),
			CMathUtility::GetRandomFloat(_tInstanceEffectMeshInfo.m_vRange.y * -0.5f, _tInstanceEffectMeshInfo.m_vRange.y * 0.5f) * CMathUtility::GetRandomFloat(_tInstanceEffectMeshInfo.m_vRadius.x, _tInstanceEffectMeshInfo.m_vRadius.y),
			CMathUtility::GetRandomFloat(_tInstanceEffectMeshInfo.m_vRange.z * -0.5f, _tInstanceEffectMeshInfo.m_vRange.z * 0.5f) * CMathUtility::GetRandomFloat(_tInstanceEffectMeshInfo.m_vRadius.x, _tInstanceEffectMeshInfo.m_vRadius.y), 1.0f
		);

		// 회전 변환
		Matrix matRandomRotate = Matrix::Identity;
		Matrix matRotateX = Matrix::CreateRotationX(CMathUtility::GetRandomFloat(-_tInstanceEffectMeshInfo.m_vRotation.x, _tInstanceEffectMeshInfo.m_vRotation.x));
		Matrix matRotateY = Matrix::CreateRotationY(CMathUtility::GetRandomFloat(-_tInstanceEffectMeshInfo.m_vRotation.y, _tInstanceEffectMeshInfo.m_vRotation.y));
		Matrix matRotateZ = Matrix::CreateRotationZ(CMathUtility::GetRandomFloat(-_tInstanceEffectMeshInfo.m_vRotation.z, _tInstanceEffectMeshInfo.m_vRotation.z));

		matRandomRotate = matRotateX * matRotateY * matRotateZ;

		Matrix matOrigin = Matrix::Identity;
		matOrigin.Right(Vector3(fSize, 0.f, 0.f));
		matOrigin.Up(Vector3(0.f, fSize, 0.f));
		matOrigin.Forward(Vector3(0.f, 0.f, fSize));
		matOrigin.Translation(Vector3(pInstances[i].m_vTranslation.x, pInstances[i].m_vTranslation.y, pInstances[i].m_vTranslation.z));

		matOrigin *= matRandomRotate;

		pInstances[i].m_vRight = Vector4(matOrigin.Right().x, matOrigin.Right().y, matOrigin.Right().z, 0.f);
		pInstances[i].m_vUp = Vector4(matOrigin.Up().x, matOrigin.Up().y, matOrigin.Up().z, 0.f);
		pInstances[i].m_vLook = Vector4(matOrigin.Forward().x, matOrigin.Forward().y, matOrigin.Forward().z, 0.f);
		pInstances[i].m_vTranslation = Vector4(matOrigin.Translation().x, matOrigin.Translation().y, matOrigin.Translation().z, 1.f);
		pInstances[i].m_vPrevTranslation = Vector4::Zero;
	}

	m_tInitialData.pSysMem = pInstances;

	ENSURE(CBuffer::CreateBuffer(m_cpInstanceBuffer.GetAddressOf()));

	delete[] pInstances;
	pInstances = nullptr;
}
