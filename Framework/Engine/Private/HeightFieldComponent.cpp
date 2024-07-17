// default
#include "pch.h"
#include "HeightFieldComponent.h"

// actor
#include "Model.h"
#include "MeshBuffer.h"

shared_ptr<CHeightFieldComponent> Engine::CHeightFieldComponent::Create(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	shared_ptr<CHeightFieldComponent> spHeightFieldComponent = make_shared<CHeightFieldComponent>();
	ENSUREF(spHeightFieldComponent->InitializeComponent(_spComponentDesc), L"Failed to initialize height field component.");
	return spHeightFieldComponent;
}

HRESULT Engine::CHeightFieldComponent::InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc)
{
	// collider component desc      
 	if (FAILED(CColliderComponent::InitializeComponent(_spComponentDesc))) { return E_FAIL; }

	// height field component desc
	std::shared_ptr<FHeightFieldComponentDesc> spHeighFieldComponentDesc = std::static_pointer_cast<FHeightFieldComponentDesc>(_spComponentDesc);
	m_wstrHeightFieldFilePath = spHeighFieldComponentDesc->m_wstrHeightFieldFilePath;
	m_vHeightFieldScale = spHeighFieldComponentDesc->m_vHeightFieldScale;
	m_vHeightFieldRotate = spHeighFieldComponentDesc->m_vHeightFieldRotate;
	m_vHeightFieldTranslate = spHeighFieldComponentDesc->m_vHeightFieldTranslate;
	m_iWidth = spHeighFieldComponentDesc->m_iWidth;
	m_iHeight = spHeighFieldComponentDesc->m_iHeight;

	// etc
	CreateHeightFieldFromFile();
	return S_OK;
}

HRESULT Engine::CHeightFieldComponent::BeginPlay()
{
	if (FAILED(CColliderComponent::BeginPlay())) { return E_FAIL; }

	return S_OK;
}

int32 Engine::CHeightFieldComponent::PreTickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::PreTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CHeightFieldComponent::TickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::TickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

int32 Engine::CHeightFieldComponent::PostTickComponent(float _fDeltaSeconds)
{
	switch (CColliderComponent::PostTickComponent(_fDeltaSeconds))
	{
	case -1:	return -1;
	default:	break;
	}

	return 0;
}

HRESULT Engine::CHeightFieldComponent::EndPlay()
{
	if (FAILED(CColliderComponent::EndPlay())) { return E_FAIL; }

	return S_OK;
}

void Engine::CHeightFieldComponent::Render(int32 _iShaderPath)
{
	CColliderComponent::Render(_iShaderPath);
}

void Engine::CHeightFieldComponent::Release()
{
	m_spModel->Release();

	if (m_pStaticRigidActor)
	{
		m_pScene->removeActor(*m_pStaticRigidActor);
		PX_RELEASE(m_pStaticRigidActor);
	}

	CColliderComponent::Release();
}

HRESULT Engine::CHeightFieldComponent::CreateHeightFieldFromFile()
{
	m_spModel = CModel::Create(m_wstrHeightFieldFilePath, Matrix::Identity, EMeshManageType::UNIQUE, nullptr);
	auto& umapMeshes = m_spModel->GetMeshInfos();
	for (auto& Pair : umapMeshes)
	{
		for (auto& tMeshInfo : Pair.second)
		{
			vector<Vector3>& vecVertexPositions = tMeshInfo->m_spMeshBuffer->GetVertexPositions();

			Matrix matWorld = tMeshInfo->m_matTransform * Matrix::CreateScale(0.01f);
			for (auto& vVector3 : vecVertexPositions)
			{
				XMStoreFloat3(&vVector3, XMVector3Transform(vVector3, matWorld));
			}
			// 정점을 x, z 오름차순으로 정렬합니다.
			sort(vecVertexPositions.begin(), vecVertexPositions.end(), [](const Vector3& _vPrePoint, const Vector3& _vPostPoint) -> bool
				{
					if (_vPrePoint.x < _vPostPoint.x) { return true; }
					else if (_vPrePoint.x == _vPostPoint.x) { return _vPrePoint.z < _vPostPoint.z; }
					else { return false; }
				});

			// 인접한 두 정점이 중복되는 경우, 없앱니다.
			std::vector<Vector3> vecPureVertexPositions;
			vecPureVertexPositions.reserve(vecVertexPositions.size());

			for (int32 iIndex = 0; iIndex < vecVertexPositions.size() - 1; ++iIndex)
			{
				if (vecVertexPositions[iIndex] == vecVertexPositions[iIndex + 1])
				{
					continue;
				}

				vecPureVertexPositions.push_back(vecVertexPositions[iIndex]);

				if (iIndex == vecVertexPositions.size() - 2)
				{
					vecPureVertexPositions.push_back(vecVertexPositions[iIndex + 1]);
				}
			}

			// 최대 높이와 최소 높이를 구합니다.
			PxReal fMinHeight = PX_MAX_F32;
			PxReal fMaxHeight = -PX_MAX_F32;
			uint32 iSize = m_iWidth * m_iHeight;
			for (PxU32 iIndex = 0; iIndex < iSize; ++iIndex)
			{
				fMinHeight = PxMin(fMinHeight, vecPureVertexPositions[iIndex].y);
				fMaxHeight = PxMax(fMaxHeight, vecPureVertexPositions[iIndex].y);
			}

			Vector3 fOffsets = vecPureVertexPositions[iSize - 1] - vecPureVertexPositions[0];
			float fOffsetX = fOffsets.x / static_cast<float>(m_iWidth);
			float fOffsetZ = fOffsets.z / static_cast<float>(m_iHeight);

			PxReal fDeltaHeight = fMaxHeight - fMinHeight;
			PxReal fQuantization = (PxReal)0x7fff;
			PxReal fHeightScale = PxMax(fDeltaHeight / fQuantization, PX_MIN_HEIGHTFIELD_Y_SCALE);

			// 하이트 필드를 생성합니다.
			std::vector<PxHeightFieldSample> vecSamples(iSize);
			for (uint32 iIndex = 0; iIndex < iSize; ++iIndex)
			{
				PxI16 iHeight = PxI16(fQuantization * (vecPureVertexPositions[iIndex].y - fMinHeight) / fDeltaHeight);
				vecSamples[iIndex].height = iHeight;
			}

			// 높이 0이 xz 평면 위에 있도록 합니다.
			m_oPxTransform.p += PxVec3(m_vHeightFieldTranslate.x, fMinHeight, m_vHeightFieldTranslate.z);

			PxHeightFieldDesc tHeightFieldDesc;
			tHeightFieldDesc.format = PxHeightFieldFormat::eS16_TM;
			tHeightFieldDesc.nbColumns = m_iWidth;
			tHeightFieldDesc.nbRows = m_iHeight;
			tHeightFieldDesc.samples.data = vecSamples.data();
			tHeightFieldDesc.samples.stride = sizeof(PxHeightFieldSample);
			tHeightFieldDesc.flags = PxHeightFieldFlags();

			// 하이트 필드를 생성합니다.
			PxHeightField* pHeightField = PxCreateHeightField(tHeightFieldDesc);

			// 하이트 필드 지오메트리를 생성합니다.
			PxHeightFieldGeometry oGeometry;
			oGeometry.columnScale = fOffsetZ;
			oGeometry.rowScale = fOffsetX;
			oGeometry.heightScale = fDeltaHeight != 0.0f ? fHeightScale : 1.0f;
			oGeometry.heightField = pHeightField;

			PxShape* pShape = m_pPhysics->createShape(oGeometry, *m_pMaterial);
			// 콜리전 레이어와 플래그를 설정합니다.
			if (m_eCollisionLayer != ECollisionLayer::ENUM_END && m_eCollisionFlag != ECollisionFlag::ENUM_END)
			{
				const PxFilterData tTriggerFilterData((PxU32)m_eCollisionLayer, (PxU32)m_eCollisionFlag, 0xffffffff, 0xffffffff);
				pShape->setSimulationFilterData(tTriggerFilterData);
				pShape->setQueryFilterData(tTriggerFilterData);
			}

			m_pStaticRigidActor = m_pPhysics->createRigidStatic(m_oPxTransform);
			m_pStaticRigidActor->userData = this;
			m_pStaticRigidActor->attachShape(*pShape);

			if (!m_bDeferredAdd)
			{
				m_pScene->addActor(*m_pStaticRigidActor);
			}

			pShape->release();
			pHeightField->release();
		}
	}
	return S_OK;
}
