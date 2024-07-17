#include "pch.h"
#include "MeshBuffer.h"
#include "Bone.h"

#include "EngineModule.h"
#include "RenderManager.h"

#include "MeshHandler.h"
#include "ShaderHandler.h"
#include "CameraManager.h"
#include "CameraComponent.h"

#include "Material.h"
#include "EffectShader.h"
#include "TextureHandler.h"

shared_ptr<CMeshBuffer> Engine::CMeshBuffer::Create(const wstring& _wstrModelFilePath, ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType, EMeshManageType _eMeshType)
{
	shared_ptr<CMeshBuffer> spInstance = make_shared<CMeshBuffer>();

	CHECKF(spInstance != nullptr, L"CMeshBuffer Create Failed");
	spInstance->Initialize(_wstrModelFilePath, _ifFile, _matPivot, _eModelType, _eMeshType);

	return spInstance;
}

HRESULT Engine::CMeshBuffer::Initialize(const wstring& _wstrModelFilePath, ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType, EMeshManageType _eMeshType)
{
	CBuffer::Initialize();

	m_wpMeshHandler = CMeshHandler::GetInstance();

	m_eModelType = _eModelType;
	m_eMeshType = _eMeshType;

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

	InitVertexBuffer(_ifFile, _matPivot, _eModelType);
	InitIndexBuffer(_ifFile);

	m_spMaterial = CMaterial::Create(_wstrModelFilePath, _ifFile, _eModelType, _eMeshType);

	m_eShaderType = m_spMaterial->GetShaderType();

	switch (m_eShaderType)
	{
	case Engine::EShaderType::PLAYEREYE_ANIM:
		m_anyMeshInfo = make_shared<FMeshInfo_PlayerEye>();
		break;
	case Engine::EShaderType::MONSTERFACE_ANIM:
		m_anyMeshInfo = make_shared<FMeshInfo_MonsterFace>();
		break;
	case Engine::EShaderType::DEFAULT_ANIM:
	case Engine::EShaderType::MONSTER_KITSUNEBI_FIRE:
		m_anyMeshInfo = make_shared<FMeshInfo_Default_Anim>();
		break;
	case Engine::EShaderType::DEFAULT_NONANIM:
		m_anyMeshInfo = make_shared<FMeshInfo_Default_NonAnim>();
		break;
	case Engine::EShaderType::DEFAULT_INSTANCE:
		m_anyMeshInfo = make_shared<FMeshInfo_Default_Instance>();
		break;
	case Engine::EShaderType::PALLIT_TOP_NONANIM:
		m_anyMeshInfo = make_shared<FMeshInfo_PalLitTop_NonAnim>();
		break;
	case Engine::EShaderType::PALLIT_TOP_INSTANCE:
		m_anyMeshInfo = make_shared<FMeshInfo_PalLitTop_Instance>();
		break;
	case Engine::EShaderType::EXTERN_INSTANCE:
		m_anyMeshInfo = make_shared<FMeshInfo_Extern_Instance>();
		break;
	case Engine::EShaderType::LANDSCAPE_NONANIM:
		m_anyMeshInfo = make_shared<FMeshInfo_Landscape>();
		break;
	case Engine::EShaderType::SKY_NONANIM:
		m_anyMeshInfo = make_shared<FMeshInfo_SkyDome>();
		break;
	case Engine::EShaderType::EXTERN_NONANIM:
		m_anyMeshInfo = make_shared<FMeshInfo_Extern_NonAnim>();
		break;
	case Engine::EShaderType::ARCHITECTURE:
		m_anyMeshInfo = make_shared<FMeshInfo_Architecture>();
		break;
	default:
		m_anyMeshInfo = nullptr;
		break;
	}

	// 리소스 갱신
	RefreshResources();

	return S_OK;
}

HRESULT Engine::CMeshBuffer::Render()
{
	if (!m_anyMeshInfo.has_value())
	{
		return S_OK;
	}
	BindBuffers_NonInstancing();
	m_cpDeviceContext->DrawIndexed(m_iNumIndices, 0, 0);
	return S_OK;
}

HRESULT Engine::CMeshBuffer::Render_Instance(ERenderType _eRenderType)
{
	BindBuffers_Instancing(_eRenderType);
	m_cpDeviceContext->DrawIndexedInstanced(m_iNumIndices, m_umapInstanceCurrentCount[_eRenderType], 0, 0, 0);
	m_umapRegistered[_eRenderType] = false;
	return S_OK;
}

void Engine::CMeshBuffer::SetMatrices(const Matrix& _matTransform, const Matrix& _matWorld)
{
	if (!m_anyMeshInfo.has_value())
	{
		return;
	}
	switch (m_eShaderType)
	{
	case Engine::EShaderType::DEFAULT_NONANIM:
		any_cast<shared_ptr<FMeshInfo_Default_NonAnim>>(m_anyMeshInfo)->matTransform = _matTransform;
		any_cast<shared_ptr<FMeshInfo_Default_NonAnim>>(m_anyMeshInfo)->matParentWorld = _matWorld;
		break;
	case Engine::EShaderType::PALLIT_TOP_NONANIM:
		any_cast<shared_ptr<FMeshInfo_PalLitTop_NonAnim>>(m_anyMeshInfo)->matTransform = _matTransform;
		any_cast<shared_ptr<FMeshInfo_PalLitTop_NonAnim>>(m_anyMeshInfo)->matParentWorld = _matWorld;
		break;
	case Engine::EShaderType::LANDSCAPE_NONANIM:
		any_cast<shared_ptr<FMeshInfo_Landscape>>(m_anyMeshInfo)->matCombindWorld = _matTransform * _matWorld;
		break;
	case Engine::EShaderType::SKY_NONANIM:
		any_cast<shared_ptr<FMeshInfo_SkyDome>>(m_anyMeshInfo)->matTransform = _matTransform;
		any_cast<shared_ptr<FMeshInfo_SkyDome>>(m_anyMeshInfo)->matParentWorld = _matWorld;
		break;
	case Engine::EShaderType::EXTERN_NONANIM:
		any_cast<shared_ptr<FMeshInfo_Extern_NonAnim>>(m_anyMeshInfo)->matTransform = _matTransform;
		any_cast<shared_ptr<FMeshInfo_Extern_NonAnim>>(m_anyMeshInfo)->matParentWorld = _matWorld;
		break;
	case Engine::EShaderType::ARCHITECTURE:
		any_cast<shared_ptr<FMeshInfo_Architecture>>(m_anyMeshInfo)->matTransform = _matTransform;
		any_cast<shared_ptr<FMeshInfo_Architecture>>(m_anyMeshInfo)->matParentWorld = _matWorld;
		break;
	}
}

void Engine::CMeshBuffer::SetMatrices(const Matrix& _matWorld, Matrix _matBones[256])
{
	if (!m_anyMeshInfo.has_value())
	{
		return;
	}
	switch (m_eShaderType)
	{
	case Engine::EShaderType::PLAYEREYE_ANIM:
		any_cast<shared_ptr<FMeshInfo_PlayerEye>>(m_anyMeshInfo)->matParentWorld = _matWorld;
		memcpy(any_cast<shared_ptr<FMeshInfo_PlayerEye>>(m_anyMeshInfo)->matBones, _matBones, sizeof(Matrix) * 256);
		break;
	case Engine::EShaderType::MONSTERFACE_ANIM:
		any_cast<shared_ptr<FMeshInfo_MonsterFace>>(m_anyMeshInfo)->matParentWorld = _matWorld;
		memcpy(any_cast<shared_ptr<FMeshInfo_MonsterFace>>(m_anyMeshInfo)->matBones, _matBones, sizeof(Matrix) * 256);
		break;
	case Engine::EShaderType::DEFAULT_ANIM:
	case Engine::EShaderType::MONSTER_KITSUNEBI_FIRE:
		any_cast<shared_ptr<FMeshInfo_Default_Anim>>(m_anyMeshInfo)->matParentWorld = _matWorld;
		memcpy(any_cast<shared_ptr<FMeshInfo_Default_Anim>>(m_anyMeshInfo)->matBones, _matBones, sizeof(Matrix) * 256);
		break;
	}
}

HRESULT Engine::CMeshBuffer::BindBuffers_NonInstancing()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_cpVertexBuffer.Get(),
	};

	uint32 iVertexStrides[] = {
		m_iVertexStride,
	};

	uint32 iOffsets[] = {
		0,
	};

	m_cpDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_cpDeviceContext->IASetIndexBuffer(m_cpIndexBuffer.Get(), m_eIndexFormat, 0);
	m_cpDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	return S_OK;
}

HRESULT Engine::CMeshBuffer::BindBuffers_Instancing(ERenderType _eRenderType)
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_cpVertexBuffer.Get(),
		m_umapInstanceBuffer[_eRenderType].Get()
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

void Engine::CMeshBuffer::AddRenderInstance(const Matrix& _matTransform, const Matrix& _matParentWorld, ERenderType _eRenderType)
{
	m_umapInstanceMeshMatrix[_eRenderType].push_back({ _matTransform,_matParentWorld });

	if (!m_umapRegistered[_eRenderType])
	{
		m_umapRegistered[_eRenderType] = true;
		m_wpMeshHandler.lock()->RegisterRender(_eRenderType, m_eShaderType, weak_from_this());
	}
}

void Engine::CMeshBuffer::RefreshInstances()
{
	const BoundingFrustum& tFrustum = CEngineModule::GetInstance()->GetCameraManager()->GetCurCamera()->GetCameraFrustum();
	const BoundingSphere& tSphere = CEngineModule::GetInstance()->GetCameraManager()->GetCurCamera()->GetCameraCullSphere();

	Vector3 vScale, vPosition;
	Quaternion qRotation;

	for (auto& Pair : m_umapInstanceMeshMatrix)
	{
		/*vector<FMeshMatrix> vecInstanceData(m_umapInstanceMeshMatrix[Pair.first].size());
		for (auto& tMeshMatrix : m_umapInstanceMeshMatrix[Pair.first])
		{
			(tMeshMatrix.matTransform * tMeshMatrix.matParentWorld).Decompose(vScale, qRotation, vPosition);

			if (ContainmentType::DISJOINT != tFrustum.Contains(vPosition) || ContainmentType::DISJOINT != tSphere.Contains(vPosition))
			{
				vecInstanceData.push_back(tMeshMatrix);
			}
		}*/

		m_umapInstanceCurrentCount[Pair.first] = (uint32)m_umapInstanceMeshMatrix[Pair.first].size();

		if (!m_umapInstanceBuffer.contains(Pair.first) || m_umapInstanceMaxCount[Pair.first] < m_umapInstanceCurrentCount[Pair.first])
		{
			if (m_umapInstanceMaxCount[Pair.first] == 0)
			{
				m_umapInstanceMaxCount[Pair.first] = DEFAULT_INSTANCE_MESH;
			}
			while (m_umapInstanceMaxCount[Pair.first] < m_umapInstanceCurrentCount[Pair.first]) m_umapInstanceMaxCount[Pair.first] *= 2;

			m_umapInstanceBuffer[Pair.first] = nullptr;

			m_iInstanceStride = sizeof(FTransformParentWorld);
			m_tBufferDesc.ByteWidth = m_iInstanceStride * m_umapInstanceMaxCount[Pair.first];
			m_tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			m_tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			m_tBufferDesc.MiscFlags = 0;
			m_tBufferDesc.StructureByteStride = m_iInstanceStride;
			FTransformParentWorld* pInstances = new FTransformParentWorld[m_umapInstanceMaxCount[Pair.first]];
			ZeroMemory(pInstances, sizeof(FTransformParentWorld) * m_umapInstanceMaxCount[Pair.first]);
			m_tInitialData.pSysMem = pInstances;
			memcpy(pInstances, m_umapInstanceMeshMatrix[Pair.first].data(), m_iInstanceStride * m_umapInstanceCurrentCount[Pair.first]);
			HRESULT resultIB = __super::CreateBuffer(m_umapInstanceBuffer[Pair.first].GetAddressOf());
			delete[] pInstances; pInstances = nullptr; ENSURE(resultIB);
		}
		else
		{
			D3D11_MAPPED_SUBRESOURCE tMappedResource = {};
			m_cpDeviceContext->Map(m_umapInstanceBuffer[Pair.first].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMappedResource);
			FTransformParentWorld* tDataPtr = (FTransformParentWorld*)tMappedResource.pData;
			memcpy(tDataPtr, m_umapInstanceMeshMatrix[Pair.first].data(), m_iInstanceStride * m_umapInstanceCurrentCount[Pair.first]);
			m_cpDeviceContext->Unmap(m_umapInstanceBuffer[Pair.first].Get(), 0);
		}
	}
}

void Engine::CMeshBuffer::RefreshResources()
{
	switch (m_eShaderType)
	{
	case Engine::EShaderType::DEFAULT_ANIM:
	case Engine::EShaderType::DEFAULT_NONANIM:
	case Engine::EShaderType::DEFAULT_INSTANCE:
	case Engine::EShaderType::MONSTER_KITSUNEBI_FIRE:
		RefreshResource_Default();
		break;
	case Engine::EShaderType::EXTERN_INSTANCE:
	case Engine::EShaderType::EXTERN_NONANIM:
		RefreshResource_Extern();
		break;
	case Engine::EShaderType::PALLIT_TOP_NONANIM:
	case Engine::EShaderType::PALLIT_TOP_INSTANCE:
		RefreshResource_PalLit_Top();
		break;
	case Engine::EShaderType::LANDSCAPE_NONANIM:
		RefreshResource_Landscape();
		break;
	case Engine::EShaderType::PLAYEREYE_ANIM:
		RefreshResource_PlayerEye();
		break;
	case Engine::EShaderType::MONSTERFACE_ANIM:
		RefreshResource_MonsterFace();
		break;
	case Engine::EShaderType::SKY_NONANIM:
		RefreshResource_SkyDome();
		break;
	case Engine::EShaderType::ARCHITECTURE:
		RefreshResource_Architecture();
		break;
	}

	for (auto pSRV : m_vecTextureSRVs)
	{
		if (pSRV == nullptr)
		{
			pSRV = CTextureHandler::GetInstance()->FindShaderResourceView(L"Default", ETextureType::DIFFUSE).Get();
		}
	}
}

void Engine::CMeshBuffer::Release()
{
	m_vecBoneIndices.clear();
	m_vecBoneIndices.shrink_to_fit();

	m_vecOffsetMatrices.clear();
	m_vecOffsetMatrices.shrink_to_fit();

	m_spMaterial = nullptr;
}

void Engine::CMeshBuffer::InitVertexBuffer(ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType)
{
	m_iNumVertexBuffers = m_eMeshType == EMeshManageType::INSTANCE ? 2 : 1;
	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
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

HRESULT Engine::CMeshBuffer::InitVertexBuffer_Anim(ifstream& _ifFile, Matrix _matPivot)
{
	m_iVertexStride = sizeof(FPositionNormalTexCoord2TangentBlendIndexBlendWeight);
	m_tBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_tBufferDesc.StructureByteStride = m_iVertexStride;

	FPositionNormalTexCoord2TangentBlendIndexBlendWeight* pVertices = new FPositionNormalTexCoord2TangentBlendIndexBlendWeight[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(FPositionNormalTexCoord2TangentBlendIndexBlendWeight) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vPosition.x), sizeof(pVertices[i].m_vPosition.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vPosition.y), sizeof(pVertices[i].m_vPosition.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vPosition.z), sizeof(pVertices[i].m_vPosition.z));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vNormal.x), sizeof(pVertices[i].m_vNormal.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vNormal.y), sizeof(pVertices[i].m_vNormal.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vNormal.z), sizeof(pVertices[i].m_vNormal.z));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTexCoord2.x), sizeof(pVertices[i].m_vTexCoord2.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTexCoord2.y), sizeof(pVertices[i].m_vTexCoord2.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTangent.x), sizeof(pVertices[i].m_vTangent.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTangent.y), sizeof(pVertices[i].m_vTangent.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTangent.z), sizeof(pVertices[i].m_vTangent.z));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vBlendIndices.x), sizeof(pVertices[i].m_vBlendIndices.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vBlendIndices.y), sizeof(pVertices[i].m_vBlendIndices.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vBlendIndices.z), sizeof(pVertices[i].m_vBlendIndices.z));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vBlendIndices.w), sizeof(pVertices[i].m_vBlendIndices.w));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vBlendWeights.x), sizeof(pVertices[i].m_vBlendWeights.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vBlendWeights.y), sizeof(pVertices[i].m_vBlendWeights.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vBlendWeights.z), sizeof(pVertices[i].m_vBlendWeights.z));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vBlendWeights.w), sizeof(pVertices[i].m_vBlendWeights.w));

		m_vecVertexPositions[i] = pVertices[i].m_vPosition;
	}
	m_tInitialData.pSysMem = pVertices;

	HRESULT resultVB = __super::CreateBuffer(m_cpVertexBuffer.GetAddressOf());

	delete[] pVertices;
	pVertices = nullptr;

	ENSURE(resultVB);

	return S_OK;
}

HRESULT Engine::CMeshBuffer::InitVertexBuffer_NonAnim(ifstream& _ifFile, Matrix _matPivot)
{
	m_iVertexStride = sizeof(FPositionNormalTexCoord2Tangent);
	m_tBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_tBufferDesc.StructureByteStride = m_iVertexStride;

	FPositionNormalTexCoord2Tangent* pVertices = new FPositionNormalTexCoord2Tangent[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(FPositionNormalTexCoord2Tangent) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vPosition.x), sizeof(pVertices[i].m_vPosition.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vPosition.y), sizeof(pVertices[i].m_vPosition.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vPosition.z), sizeof(pVertices[i].m_vPosition.z));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vNormal.x), sizeof(pVertices[i].m_vNormal.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vNormal.y), sizeof(pVertices[i].m_vNormal.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vNormal.z), sizeof(pVertices[i].m_vNormal.z));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTexCoord2.x), sizeof(pVertices[i].m_vTexCoord2.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTexCoord2.y), sizeof(pVertices[i].m_vTexCoord2.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTangent.x), sizeof(pVertices[i].m_vTangent.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTangent.y), sizeof(pVertices[i].m_vTangent.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTangent.z), sizeof(pVertices[i].m_vTangent.z));
		m_vecVertexPositions[i] = pVertices[i].m_vPosition;
	}

	m_tInitialData.pSysMem = pVertices;

	HRESULT resultVB = __super::CreateBuffer(m_cpVertexBuffer.GetAddressOf());

	delete[] pVertices;
	pVertices = nullptr;

	ENSURE(resultVB);

	return S_OK;
}

HRESULT Engine::CMeshBuffer::InitVertexBuffer_Landscape(ifstream& _ifFile, Matrix _matPivot)
{
	m_iVertexStride = sizeof(FPositionNormalTexCoord2TexCoord2Tangent);
	m_tBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_tBufferDesc.StructureByteStride = m_iVertexStride;

	FPositionNormalTexCoord2TexCoord2Tangent* pVertices = new FPositionNormalTexCoord2TexCoord2Tangent[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(FPositionNormalTexCoord2TexCoord2Tangent) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vPosition.x), sizeof(pVertices[i].m_vPosition.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vPosition.y), sizeof(pVertices[i].m_vPosition.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vPosition.z), sizeof(pVertices[i].m_vPosition.z));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vNormal.x), sizeof(pVertices[i].m_vNormal.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vNormal.y), sizeof(pVertices[i].m_vNormal.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vNormal.z), sizeof(pVertices[i].m_vNormal.z));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTexCoord21.x), sizeof(pVertices[i].m_vTexCoord21.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTexCoord21.y), sizeof(pVertices[i].m_vTexCoord21.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTexCoord22.x), sizeof(pVertices[i].m_vTexCoord22.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTexCoord22.y), sizeof(pVertices[i].m_vTexCoord22.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTangent.x), sizeof(pVertices[i].m_vTangent.x));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTangent.y), sizeof(pVertices[i].m_vTangent.y));
		_ifFile.read(reinterpret_cast<char*>(&pVertices[i].m_vTangent.z), sizeof(pVertices[i].m_vTangent.z));

		m_vecVertexPositions[i] = pVertices[i].m_vPosition;
	}
	m_tInitialData.pSysMem = pVertices;

	HRESULT resultVB = __super::CreateBuffer(m_cpVertexBuffer.GetAddressOf());

	delete[] pVertices;
	pVertices = nullptr;

	ENSURE(resultVB);

	return S_OK;
}

void Engine::CMeshBuffer::InitIndexBuffer(ifstream& _ifFile)
{
	m_tBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = 0;

	uint32* pIndices = new uint32[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(uint32) * m_iNumIndices);

	m_vecIndices.resize(m_iNumIndices);
	for (size_t i = 0; i < m_iNumIndices; ++i)
	{
		_ifFile.read(reinterpret_cast<char*>(&pIndices[i]), sizeof(pIndices[i]));
		m_vecIndices[i] = pIndices[i];
	}

	m_tInitialData.pSysMem = pIndices;

	HRESULT resultIB = __super::CreateBuffer(m_cpIndexBuffer.GetAddressOf());

	delete[] pIndices;
	pIndices = nullptr;

	ENSURE(resultIB);
}

void Engine::CMeshBuffer::InitInstanceBuffer()
{
	/*m_iInstanceStride = sizeof(FTransformParentWorld);

	m_tBufferDesc.ByteWidth = m_iInstanceStride * DEFAULT_INSTANCE_MESH;
	m_tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iInstanceStride;

	FTransformParentWorld* pInstances = new FTransformParentWorld[DEFAULT_INSTANCE_MESH];
	ZeroMemory(pInstances, sizeof(FTransformParentWorld) * DEFAULT_INSTANCE_MESH);

	m_tInitialData.pSysMem = pInstances;

	HRESULT resultIB = __super::CreateBuffer(m_cpInstanceBuffer.GetAddressOf());

	delete[] pInstances;
	pInstances = nullptr;

	ENSURE(resultIB);*/
}




void Engine::CMeshBuffer::RefreshResource_PlayerEye()
{
	m_vecTextureSRVs.clear();

	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Cornea_0").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Cornea_1").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Cornea_2").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Cornea_3").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Highlight_0").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Highlight_1").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Highlight_2").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Highlight_3").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Highlight_4").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Highlight_5").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Pupil_0").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Pupil_1").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Pupil_2").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Pupil_3").Get());

	weak_ptr<FMeshInfo_PlayerEye> wpMeshInfo = any_cast<shared_ptr<FMeshInfo_PlayerEye>>(m_anyMeshInfo);
	wpMeshInfo.lock()->iCorneaIndex = m_spMaterial->GetInt(L"Cornea Index");
	wpMeshInfo.lock()->iHighlightIndex = m_spMaterial->GetInt(L"Highlight Index");
	wpMeshInfo.lock()->iPupilIndex = m_spMaterial->GetInt(L"Pupil Index");
	wpMeshInfo.lock()->bCorneaUseAlternativeColor = m_spMaterial->GetInt(L"Cornea Use Alternative Color");
	wpMeshInfo.lock()->bHighlightUseAlternativeColor = m_spMaterial->GetInt(L"Highlight Use Alternative Color");
	wpMeshInfo.lock()->bPupilUseAlternativeColor = m_spMaterial->GetInt(L"Pupil Use Alternative Color");
	wpMeshInfo.lock()->vCorneaAlternativeColor = m_spMaterial->GetVector3(L"Cornea Alternative Color");
	wpMeshInfo.lock()->vHighlightAlternativeColor = m_spMaterial->GetVector3(L"Highlight Alternative Color");
	wpMeshInfo.lock()->vPupliAlternativeColor = m_spMaterial->GetVector3(L"Pupil Alternative Color");
	wpMeshInfo.lock()->fCorneaEmission = m_spMaterial->GetFloat(L"Cornea Emision");
	wpMeshInfo.lock()->fHighlightEmission = m_spMaterial->GetFloat(L"Highlight Emission");
	wpMeshInfo.lock()->fPupliEmission = m_spMaterial->GetFloat(L"Pupli Emission");
}

void Engine::CMeshBuffer::RefreshResource_MonsterFace()
{
	m_vecTextureSRVs.clear();

	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Base Texture").Get());

	weak_ptr<FMeshInfo_MonsterFace> wpMeshInfo = any_cast<shared_ptr<FMeshInfo_MonsterFace>>(m_anyMeshInfo);
	wpMeshInfo.lock()->iMaxIndexX = m_spMaterial->GetInt(L"MaxIndex X");
	wpMeshInfo.lock()->iMaxIndexY = m_spMaterial->GetInt(L"MaxIndex Y");
	wpMeshInfo.lock()->iIndexX = m_spMaterial->GetInt(L"Index X");
	wpMeshInfo.lock()->iIndexY = m_spMaterial->GetInt(L"Index Y");
}

void Engine::CMeshBuffer::RefreshResource_Default()
{
	m_vecTextureSRVs.clear();

	int32 bDiffuse = false;
	int32 bNormal = false;
	int32 bMRSO = false;
	int32 bEmission = false;

	// 디퓨즈
	{
		ComPtr<ID3D11ShaderResourceView> cpDiffuseSRV = m_spMaterial->GetSRV(L"Base Texture");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"Base Color");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"Diffuse");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"PM_Diffuse");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"Difuse texture");
		if (cpDiffuseSRV == nullptr)
		{
			bDiffuse = false;
			m_vecTextureSRVs.push_back(nullptr);

#ifdef _DEBUG
			// 디퓨즈는 무조건 있어야 하기 때문에 출력함
			//wcout << m_wstrMeshName << L": Diffuse" << endl;
#endif // _DEBUG

		}
		else
		{
			bDiffuse = true;
			m_vecTextureSRVs.push_back(cpDiffuseSRV.Get());
		}
	}

	// 노말
	{
		ComPtr<ID3D11ShaderResourceView> cpNormalSRV = m_spMaterial->GetSRV(L"Base Normal");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"Normal Map");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"Normal");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"Normals");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"PM_Normals");
		if (cpNormalSRV == nullptr)
		{
			bNormal = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bNormal = true;
			m_vecTextureSRVs.push_back(cpNormalSRV.Get());
		}
	}

	// MRSO
	{
		ComPtr<ID3D11ShaderResourceView> cpMRSOSRV = m_spMaterial->GetSRV(L"MetallicRoughnessOcclusionSpecularTexture");
		if (cpMRSOSRV == nullptr)
		{
			bMRSO = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bMRSO = true;
			m_vecTextureSRVs.push_back(cpMRSOSRV.Get());
		}
	}

	// Emission
	{
		ComPtr<ID3D11ShaderResourceView> cpEmissionSRV = m_spMaterial->GetSRV(L"Emissive Texture");
		if (cpEmissionSRV == nullptr) cpEmissionSRV = m_spMaterial->GetSRV(L"PM_Emissive");
		if (cpEmissionSRV == nullptr)
		{
			bEmission = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bEmission = true;
			m_vecTextureSRVs.push_back(cpEmissionSRV.Get());
		}
	}

	switch (m_eShaderType)
	{
	case EShaderType::DEFAULT_ANIM:
	case EShaderType::MONSTER_KITSUNEBI_FIRE:
	{
		weak_ptr<FMeshInfo_Default_Anim> wpMeshInfo = any_cast<shared_ptr<FMeshInfo_Default_Anim>>(m_anyMeshInfo);
		wpMeshInfo.lock()->bDiffuse = bDiffuse;
		wpMeshInfo.lock()->bNormal = bNormal;
		wpMeshInfo.lock()->bMRSO = bMRSO;
		wpMeshInfo.lock()->bEmission = bEmission;
		wpMeshInfo.lock()->bUseAlternativeColor = m_spMaterial->GetInt(L"Use Alternative Color");
		wpMeshInfo.lock()->vAlternativeColor = m_spMaterial->GetVector3(L"Alternative Color");
		wpMeshInfo.lock()->fRoughnessAdd = m_spMaterial->GetFloat(L"Roughness Add");
		wpMeshInfo.lock()->fOcclusionAdd = m_spMaterial->GetFloat(L"Occlusion Add");
	}
	break;
	case EShaderType::DEFAULT_NONANIM:
	{
		weak_ptr<FMeshInfo_Default_NonAnim> wpMeshInfo = any_cast<shared_ptr<FMeshInfo_Default_NonAnim>>(m_anyMeshInfo);
		wpMeshInfo.lock()->bDiffuse = bDiffuse;
		wpMeshInfo.lock()->bNormal = bNormal;
		wpMeshInfo.lock()->bMRSO = bMRSO;
		wpMeshInfo.lock()->bEmission = bEmission;
		wpMeshInfo.lock()->bUseAlternativeColor = m_spMaterial->GetInt(L"Use Alternative Color");
		wpMeshInfo.lock()->vAlternativeColor = m_spMaterial->GetVector3(L"Alternative Color");
		wpMeshInfo.lock()->fRoughnessAdd = m_spMaterial->GetFloat(L"Roughness Add");
		wpMeshInfo.lock()->fOcclusionAdd = m_spMaterial->GetFloat(L"Occlusion Add");
	}
	break;
	case EShaderType::DEFAULT_INSTANCE:
	{
		weak_ptr<FMeshInfo_Default_Instance> wpMeshInfo = any_cast<shared_ptr<FMeshInfo_Default_Instance>>(m_anyMeshInfo);
		wpMeshInfo.lock()->bDiffuse = bDiffuse;
		wpMeshInfo.lock()->bNormal = bNormal;
		wpMeshInfo.lock()->bMRSO = bMRSO;
		wpMeshInfo.lock()->bEmission = bEmission;
		wpMeshInfo.lock()->bUseAlternativeColor = m_spMaterial->GetInt(L"Use Alternative Color");
		wpMeshInfo.lock()->vAlternativeColor = m_spMaterial->GetVector3(L"Alternative Color");
		wpMeshInfo.lock()->fRoughnessAdd = m_spMaterial->GetFloat(L"Roughness Add");
		wpMeshInfo.lock()->fOcclusionAdd = m_spMaterial->GetFloat(L"Occlusion Add");
	}
	break;
	}
}

void Engine::CMeshBuffer::RefreshResource_PalLit_Top()
{
	m_vecTextureSRVs.clear();

	int32 bDiffuse = false;
	int32 bNormal = false;
	int32 bMRSO = false;
	int32 bEmission = false;
	int32 bTopDIffuse = false;
	int32 bTopNormal = false;

	// 디퓨즈
	{
		ComPtr<ID3D11ShaderResourceView> cpDiffuseSRV = m_spMaterial->GetSRV(L"Base Texture");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"Base Color");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"Diffuse");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"PM_Diffuse");
		if (cpDiffuseSRV == nullptr)
		{
			bDiffuse = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bDiffuse = true;
			m_vecTextureSRVs.push_back(cpDiffuseSRV.Get());
		}
	}

	// 노말
	{
		ComPtr<ID3D11ShaderResourceView> cpNormalSRV = m_spMaterial->GetSRV(L"Base Normal");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"Normal Map");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"Normal");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"Normals");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"PM_Normals");
		if (cpNormalSRV == nullptr)
		{
			bNormal = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bNormal = true;
			m_vecTextureSRVs.push_back(cpNormalSRV.Get());
		}
	}

	// MRSO
	{
		ComPtr<ID3D11ShaderResourceView> cpMRSOSRV = m_spMaterial->GetSRV(L"MetallicRoughnessOcclusionSpecularTexture");
		if (cpMRSOSRV == nullptr)
		{
			bMRSO = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bMRSO = true;
			m_vecTextureSRVs.push_back(cpMRSOSRV.Get());
		}
	}

	// Emission
	{
		ComPtr<ID3D11ShaderResourceView> cpEmissionSRV = m_spMaterial->GetSRV(L"Emissive Texture");
		if (cpEmissionSRV == nullptr)
		{
			bEmission = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bEmission = true;
			m_vecTextureSRVs.push_back(cpEmissionSRV.Get());
		}
	}

	// Top Diffuse
	{
		ComPtr<ID3D11ShaderResourceView> cpTopDiffuseSRV = m_spMaterial->GetSRV(L"Top Base Texture");
		if (cpTopDiffuseSRV == nullptr)
		{
			bTopDIffuse = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bTopDIffuse = true;
			m_vecTextureSRVs.push_back(cpTopDiffuseSRV.Get());
		}
	}

	// Top Normal
	{
		ComPtr<ID3D11ShaderResourceView> cpTopNormalSRV = m_spMaterial->GetSRV(L"Top Normal Map");
		if (cpTopNormalSRV == nullptr)
		{
			bTopNormal = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bTopNormal = true;
			m_vecTextureSRVs.push_back(cpTopNormalSRV.Get());
		}
	}

	switch (m_eShaderType)
	{
	case EShaderType::PALLIT_TOP_NONANIM:
	{
		weak_ptr<FMeshInfo_PalLitTop_NonAnim> wpMeshInfo = any_cast<shared_ptr<FMeshInfo_PalLitTop_NonAnim>>(m_anyMeshInfo);
		wpMeshInfo.lock()->bDiffuse = bDiffuse;
		wpMeshInfo.lock()->bNormal = bNormal;
		wpMeshInfo.lock()->bMRSO = bMRSO;
		wpMeshInfo.lock()->bEmission = bEmission;
		wpMeshInfo.lock()->bTopDiffuse = bTopDIffuse;
		wpMeshInfo.lock()->bTopNormal = bTopNormal;
		wpMeshInfo.lock()->fRoughnessAdd = m_spMaterial->GetFloat(L"Roughness Add");
		wpMeshInfo.lock()->fOcclusionAdd = m_spMaterial->GetFloat(L"Occlusion Add");
	}
	break;
	case EShaderType::PALLIT_TOP_INSTANCE:
	{
		weak_ptr<FMeshInfo_PalLitTop_Instance> spMeshInfo = any_cast<shared_ptr<FMeshInfo_PalLitTop_Instance>>(m_anyMeshInfo);
		spMeshInfo.lock()->bDiffuse = bDiffuse;
		spMeshInfo.lock()->bNormal = bNormal;
		spMeshInfo.lock()->bMRSO = bMRSO;
		spMeshInfo.lock()->bEmission = bEmission;
		spMeshInfo.lock()->bTopDiffuse = bTopDIffuse;
		spMeshInfo.lock()->bTopNormal = bTopNormal;
		spMeshInfo.lock()->fRoughnessAdd = m_spMaterial->GetFloat(L"Roughness Add");
		spMeshInfo.lock()->fOcclusionAdd = m_spMaterial->GetFloat(L"Occlusion Add");
	}
	break;
	}
}

void Engine::CMeshBuffer::RefreshResource_Extern()
{
	m_vecTextureSRVs.clear();

	int32 bDiffuse = false;
	int32 bNormal = false;
	int32 bSpecularInvRoughness = false;

	// 디퓨즈
	{
		ComPtr<ID3D11ShaderResourceView> cpDiffuseSRV = m_spMaterial->GetSRV(L"Base Color");
		if (cpDiffuseSRV == nullptr)
		{
			bDiffuse = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bDiffuse = true;
			m_vecTextureSRVs.push_back(cpDiffuseSRV.Get());
		}
	}

	// 노말
	{
		ComPtr<ID3D11ShaderResourceView> cpNormalSRV = m_spMaterial->GetSRV(L"Base Normal");
		if (cpNormalSRV == nullptr)
		{
			bNormal = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bNormal = true;
			m_vecTextureSRVs.push_back(cpNormalSRV.Get());
		}
	}

	// SpecularInvRoughness
	{
		ComPtr<ID3D11ShaderResourceView> cpSpecularInvRoughnessSRV = m_spMaterial->GetSRV(L"SpecularInvRoughness");
		if (cpSpecularInvRoughnessSRV == nullptr)
		{
			bSpecularInvRoughness = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bSpecularInvRoughness = true;
			m_vecTextureSRVs.push_back(cpSpecularInvRoughnessSRV.Get());
		}
	}

	if (m_eShaderType == EShaderType::EXTERN_INSTANCE)
	{
		weak_ptr<FMeshInfo_Extern_Instance> wpMeshInfo = any_cast<shared_ptr<FMeshInfo_Extern_Instance>>(m_anyMeshInfo);

		wpMeshInfo.lock()->bDiffuse = bDiffuse;
		wpMeshInfo.lock()->bNormal = bNormal;
		wpMeshInfo.lock()->bSpecularInvRoughness = bSpecularInvRoughness;
	}
	else
	{
		weak_ptr<FMeshInfo_Extern_NonAnim> wpMeshInfo = any_cast<shared_ptr<FMeshInfo_Extern_NonAnim>>(m_anyMeshInfo);
		
		wpMeshInfo.lock()->bDiffuse = bDiffuse;
		wpMeshInfo.lock()->bNormal = bNormal;
		wpMeshInfo.lock()->bSpecularInvRoughness = bSpecularInvRoughness;
	}
}

void Engine::CMeshBuffer::RefreshResource_Landscape()
{
	m_vecTextureSRVs.clear();
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color GrassA").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal GrassA").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"GrassA Color Variation").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color GrassB").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color LongGrass").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color DirtAndRock").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal DirtAndRock").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color Cliff").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Mask Cliff").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal Cliff").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color SandA").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal SandA").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color SandB").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal SandB").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color SandC").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal SandC").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color NaturalRoadPath").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal NaturalRoadPath").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color DirtDry").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal DirtDry").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color DirtWet").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal DirtWet").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color RiverBed").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Mask RiverBed").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal RiverBed").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color CoastBeach").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Mask CoastBeach").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal CoastBeach").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color CoastRock").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Mask CoastRock").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal CoastRock").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color ForestMoss").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal ForestMoss").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Color ForestDeadBranches").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Normal ForestDeadBranches").Get());

	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Heightmap").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_SlopeMaterialA").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_Cliff").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_CoastBeach").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_DirtAndRock").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_DirtDry").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_Foliage_Remover").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_GrassA").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_LongGrass").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_NaturalRoadPath").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_RiverBed").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_SandA").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"LayerInfo_SandC").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"Voronoi Noise").Get());

	weak_ptr<FMeshInfo_Landscape> wpMeshInfo = any_cast<shared_ptr<FMeshInfo_Landscape>>(m_anyMeshInfo);
	wpMeshInfo.lock()->fVoronoiCellSizeX = m_spMaterial->GetFloat(L"CellSize X");
	wpMeshInfo.lock()->fVoronoiCellSizeY = m_spMaterial->GetFloat(L"CellSize Y");
	wpMeshInfo.lock()->fNumRepeatX = m_spMaterial->GetFloat(L"NumRepeat X");
	wpMeshInfo.lock()->fNumRepeatY = m_spMaterial->GetFloat(L"NumRepeat Y");
}

void Engine::CMeshBuffer::RefreshResource_SkyDome()
{
	m_vecTextureSRVs.clear();
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"T_Sky_Blue").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"T_Sky_Clouds_M").Get());
	m_vecTextureSRVs.push_back(m_spMaterial->GetSRV(L"T_Sky_Stars").Get());
}

void Engine::CMeshBuffer::RefreshResource_Architecture()
{
	m_vecTextureSRVs.clear();

	int32 bDiffuse = false;
	int32 bNormal = false;
	int32 bMRSO = false;
	int32 bEmission = false;

	// 디퓨즈
	{
		ComPtr<ID3D11ShaderResourceView> cpDiffuseSRV = m_spMaterial->GetSRV(L"Base Texture");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"Base Color");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"Diffuse");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"PM_Diffuse");
		if (cpDiffuseSRV == nullptr) cpDiffuseSRV = m_spMaterial->GetSRV(L"Difuse texture");
		if (cpDiffuseSRV == nullptr)
		{
			bDiffuse = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bDiffuse = true;
			m_vecTextureSRVs.push_back(cpDiffuseSRV.Get());
		}
	}

	// 노말
	{
		ComPtr<ID3D11ShaderResourceView> cpNormalSRV = m_spMaterial->GetSRV(L"Base Normal");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"Normal Map");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"Normal");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"Normals");
		if (cpNormalSRV == nullptr) cpNormalSRV = m_spMaterial->GetSRV(L"PM_Normals");
		if (cpNormalSRV == nullptr)
		{
			bNormal = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bNormal = true;
			m_vecTextureSRVs.push_back(cpNormalSRV.Get());
		}
	}

	// MRSO
	{
		ComPtr<ID3D11ShaderResourceView> cpMRSOSRV = m_spMaterial->GetSRV(L"MetallicRoughnessOcclusionSpecularTexture");
		if (cpMRSOSRV == nullptr)
		{
			bMRSO = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bMRSO = true;
			m_vecTextureSRVs.push_back(cpMRSOSRV.Get());
		}
	}

	// Emission
	{
		ComPtr<ID3D11ShaderResourceView> cpEmissionSRV = m_spMaterial->GetSRV(L"Emissive Texture");
		if (cpEmissionSRV == nullptr) cpEmissionSRV = m_spMaterial->GetSRV(L"PM_Emissive");
		if (cpEmissionSRV == nullptr)
		{
			bEmission = false;
			m_vecTextureSRVs.push_back(nullptr);
		}
		else
		{
			bEmission = true;
			m_vecTextureSRVs.push_back(cpEmissionSRV.Get());
		}
	}

	weak_ptr<FMeshInfo_Architecture> wpMeshInfo = any_cast<shared_ptr<FMeshInfo_Architecture>>(m_anyMeshInfo);
	wpMeshInfo.lock()->bDiffuse = bDiffuse;
	wpMeshInfo.lock()->bNormal = bNormal;
	wpMeshInfo.lock()->bMRSO = bMRSO;
	wpMeshInfo.lock()->bEmission = bEmission;
	wpMeshInfo.lock()->fRoughnessAdd = m_spMaterial->GetFloat(L"Roughness Add");
	wpMeshInfo.lock()->fOcclusionAdd = m_spMaterial->GetFloat(L"Occlusion Add");
	wpMeshInfo.lock()->iState = m_spMaterial->GetInt(L"State");
	wpMeshInfo.lock()->fAlpha = m_spMaterial->GetFloat(L"Alpha");
	wpMeshInfo.lock()->fExtendY = m_spMaterial->GetFloat(L"ExtendY");
	wpMeshInfo.lock()->fProgress = m_spMaterial->GetFloat(L"Progress");
}

void Engine::CMeshBuffer::BindVariable(const ComPtr<ID3DX11EffectVariable>& _cpVariableMeshInfo, const ComPtr<ID3DX11EffectShaderResourceVariable>& _cpVariableTextures)
{
	if (!m_anyMeshInfo.has_value())
	{
		return;
	}

	if (m_spMaterial->GetUpdated())
	{
		RefreshResources();
		m_spMaterial->ResetUpdated();
	}

	switch (m_eShaderType)
	{
	case Engine::EShaderType::PLAYEREYE_ANIM:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_PlayerEye>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_PlayerEye)));
		break;
	case Engine::EShaderType::MONSTERFACE_ANIM:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_MonsterFace>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_MonsterFace)));
		break;
	case Engine::EShaderType::DEFAULT_ANIM:
	case Engine::EShaderType::MONSTER_KITSUNEBI_FIRE:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_Default_Anim>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_Default_Anim)));
		break;
	case Engine::EShaderType::DEFAULT_NONANIM:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_Default_NonAnim>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_Default_NonAnim)));
		break;
	case Engine::EShaderType::DEFAULT_INSTANCE:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_Default_Instance>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_Default_Instance)));
		break;
	case Engine::EShaderType::PALLIT_TOP_NONANIM:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_PalLitTop_NonAnim>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_PalLitTop_NonAnim)));
		break;
	case Engine::EShaderType::PALLIT_TOP_INSTANCE:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_PalLitTop_Instance>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_PalLitTop_Instance)));
		break;
	case Engine::EShaderType::EXTERN_INSTANCE:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_Extern_Instance>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_Extern_Instance)));
		break;
	case Engine::EShaderType::LANDSCAPE_NONANIM:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_Landscape>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_Landscape)));
		break;
	case Engine::EShaderType::SKY_NONANIM:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_SkyDome>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_SkyDome)));
		break;
	case Engine::EShaderType::EXTERN_NONANIM:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_Extern_NonAnim>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_Extern_NonAnim)));
		break;
	case Engine::EShaderType::ARCHITECTURE:
		ENSURE(_cpVariableMeshInfo->SetRawValue(any_cast<shared_ptr<FMeshInfo_Architecture>>(m_anyMeshInfo).get(), 0, sizeof(FMeshInfo_Architecture)));
		break;
	default:
		return;
	}
	ENSURE(_cpVariableTextures->SetResourceArray(m_vecTextureSRVs.data(), 0, (int32)m_vecTextureSRVs.size()));
}

void Engine::CMeshBuffer::SetUpBoneMatrices(vector<Matrix>& _vecBoneMatrices, const vector<shared_ptr<CBone>>& _vecBones)
{
	for (size_t i = 0; i < m_vecBoneIndices.size(); i++)
	{
		_vecBoneMatrices[i] = m_vecOffsetMatrices[i] * _vecBones[m_vecBoneIndices[i]]->GetCombinedTransformationMatrix();
	}
}