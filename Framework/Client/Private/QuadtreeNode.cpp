#include "pch.h"
#include "QuadtreeNode.h"

#include "TreeManager.h"
#include "EnvMesh.h"
#include "EnvCollision.h"
#include "Level.h"

std::shared_ptr<CQuadtreeNode> Client::CQuadtreeNode::Create(int32 _iQuadtreeIndex, uint32 _iNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents)
{
	const shared_ptr<CQuadtreeNode> spQuadTreeNode = make_shared<CQuadtreeNode>();
	ENSUREF(spQuadTreeNode->Initialize(_iQuadtreeIndex, _iNodeDepth, _vCenter, _vExtents), L"Failed to initialize quadtree node.");
	return spQuadTreeNode;
}

HRESULT Client::CQuadtreeNode::Initialize(int32 _iTreeIndex, uint32 _iNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents)
{
	if (FAILED(CTreeNode::Initialize(_iTreeIndex, _iNodeDepth, _vCenter, _vExtents))) { return E_FAIL; }

	// Ʈ�� �Ŵ���
	m_wpTreeManager = CTreeManager::GetInstance();
	m_iNodeIndex = m_wpTreeManager.lock()->AddQuadtreeChildNode(_iTreeIndex, static_pointer_cast<CQuadtreeNode>(shared_from_this()));
	m_iNodeDepth = _iNodeDepth;
	m_tDxAABB = BoundingBox(_vCenter, _vExtents);
	return S_OK;
}

HRESULT Client::CQuadtreeNode::CreateQuadtreeChildNodes(uint32 _iMaxNodeDepth, int32 _iQuadtreeIndex)
{
	// ���� �����Դϴ�.
	if (m_iNodeDepth == _iMaxNodeDepth)
	{
		shared_ptr<CTreeNode> spRootNode = m_wpTreeManager.lock()->GetQuadtreeRootNode(_iQuadtreeIndex);
		Vector3 vRootNodeCenter = spRootNode->GetCenter();
		Vector3 vRootNodeExtents = spRootNode->GetExtents();

		float fX = vRootNodeCenter.x - vRootNodeExtents.x;
		float fZ = vRootNodeCenter.z - vRootNodeExtents.z;

		Vector3 vCenter = m_tDxAABB.Center;
		Vector3 vExtents = m_tDxAABB.Extents;

		int32 iIndexX = static_cast<int32>(vCenter.x - fX) / (static_cast<int32>(vExtents.x * 2.0f));
		int32 iIndexZ = static_cast<int32>(vCenter.z - fZ) / (static_cast<int32>(vExtents.z * 2.0f));

		m_tpQuadtreeIndex = { iIndexX, iIndexZ };
		m_bLeaf = true;

		m_wpTreeManager.lock()->SetQuadtreeLeafNode(_iQuadtreeIndex, { iIndexX, iIndexZ }, static_pointer_cast<CQuadtreeNode>(shared_from_this()));
		return S_OK;
	}

	for (int32 iOuterIndex = -1; iOuterIndex < 2; iOuterIndex += 2)
	{
		for (int32 iInnerIndex = -1; iInnerIndex < 2; iInnerIndex += 2)
		{
			Vector3 vExtents = m_tDxAABB.Extents;
			vExtents.x *= 0.5f;
			vExtents.z *= 0.5f;

			Vector3 vCenter = m_tDxAABB.Center;
			vCenter.x += iInnerIndex * vExtents.x;
			vCenter.z += iOuterIndex * vExtents.z;

			shared_ptr<CQuadtreeNode> spChildNode = CQuadtreeNode::Create(_iQuadtreeIndex, m_iNodeDepth + 1, vCenter, vExtents);
			m_vecChildNodes.push_back(spChildNode);
		}
	}

	for (auto& spChildNode : m_vecChildNodes)
	{
		static_pointer_cast<CQuadtreeNode>(spChildNode)->CreateQuadtreeChildNodes(_iMaxNodeDepth, _iQuadtreeIndex);
	}

	return S_OK;
}

std::shared_ptr<CQuadtreeNode> Client::CQuadtreeNode::AllocEnvCollision()
{
	// �̹� ȯ�� �ݸ����� �Ҵ�Ǿ� �ִ� ���
	if (m_bEnvCollisionAlloced) { return nullptr; }

	int32 iX = get<0>(m_tpQuadtreeIndex);
	int32 iZ = get<1>(m_tpQuadtreeIndex);

	if ((iX == 0 && iZ == 0) ||
		(iX == 0 && iZ == 1) ||
		(iX == 3 && iZ == 0) ||
		(iX == 3 && iZ == 1))
	{
		return nullptr;
	}

	// ȯ�� �޽� ��θ� ���մϴ�.
	
	
	// z-brush�� ���� ���� ���� Ʈ���̾ޱ� �޽� �����Դϴ�.wwwww
	wstring wstrEnvCollisionFilePath
		= L"../../Resource/Models/Dat/MapMesh/Grid/Env/EnvCollision/DefaultObject_Gameplay_0_"
		+ to_wstring(iX)
		+ to_wstring(iZ)
		+ L"_Collision.dat";
	
	
	
	// ���� ���� ������ ���� Ʈ���̾ޱ� �޽� �����Դϴ�.
	/*
	wstring wstrEnvCollisionFilePath
		= L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_"
		+ to_wstring(iX)
		+ to_wstring(iZ)
		+ L".dat";
	*/

	FEnvCollisionDesc tEnvCollisionDesc;
	tEnvCollisionDesc.m_wstrGameObjectName = L"EnvCollision";
	tEnvCollisionDesc.m_wstrEnvCollisionFilePath = wstrEnvCollisionFilePath;

	// �ſ� �����ؼ� �߰��մϴ�.
	tEnvCollisionDesc.m_bInsertDirectly = false;

	m_spEnvCollision = CEnvCollision::Create(std::make_shared<FEnvCollisionDesc>(tEnvCollisionDesc));
	if (!m_spEnvCollision) { return nullptr; }

	m_wpCurLevel.lock()->AddGameObject(m_spEnvCollision);
	m_bEnvCollisionAlloced.store(true);
	return dynamic_pointer_cast<CQuadtreeNode>(shared_from_this());
}

HRESULT Client::CQuadtreeNode::ActivateEnvCollision()
{
	if (!m_bEnvCollisionAlloced) { return S_OK; }

	return m_spEnvCollision->ActivateEnvCollision();
}

HRESULT Client::CQuadtreeNode::DeactivateEnvCollision()
{
	// ȯ�� �ݸ����� �Ҵ�Ǿ� ���� ���� ���
	if (!m_bEnvCollisionAlloced) { return S_OK; }

	return m_spEnvCollision->DeactivateEnvCollision();
}

std::shared_ptr<CQuadtreeNode> Client::CQuadtreeNode::AllocEnvMesh()
{
	// �̹� ȯ�� �޽ð� �Ҵ�Ǿ� �ִ� ���
	if (m_bEnvMeshAlloced.load()) { return nullptr; }

	int32 iX = get<0>(m_tpQuadtreeIndex);
	int32 iZ = get<1>(m_tpQuadtreeIndex);

	if ((iX == 0 && iZ == 0) ||
		(iX == 0 && iZ == 1) ||
		(iX == 3 && iZ == 0) ||
		(iX == 3 && iZ == 1))
	{
		return nullptr;
	}

	// ȯ�� �޽� ��θ� ���մϴ�.
	wstring wstrEnvMeshFilePath
		= L"../../Resource/Models/Dat/Map_DefaultObject/DefaultObject_Gameplay_0_"
		+ to_wstring(iX)
		+ to_wstring(iZ)
		+ L".dat";

	FEnvMeshDesc tEnvMeshDesc;
	tEnvMeshDesc.m_wstrGameObjectName = L"EnvMesh";
	tEnvMeshDesc.m_wstrEnvMeshFilePath = wstrEnvMeshFilePath;

	m_spEnvMesh = CEnvMesh::Create(std::make_shared<FEnvMeshDesc>(tEnvMeshDesc));

	m_wpCurLevel.lock()->AddGameObject(m_spEnvMesh);
	m_bEnvMeshAlloced.store(true);
	return dynamic_pointer_cast<CQuadtreeNode>(shared_from_this());
}