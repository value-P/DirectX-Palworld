#include "pch.h"
#include "TreeManager.h"

#include "EngineModule.h"
#include "CameraManager.h"

#include "OctreeNode.h"
#include "QuadtreeNode.h"

IMPLEMENT_SINGLETON(CTreeManager)

HRESULT Client::CTreeManager::Initialize()
{
	m_wpCameraManager = CEngineModule::GetInstance()->GetCameraManager();
	
	// 쿼드 트리를 생성합니다.
	CreateTree(ETreeType::QUAD_TREE, 0, 2, { -3570.0f, 0.0f, -2550.f }, { 510.0f, 510.0f, 510.0f });
	CreateTree(ETreeType::OCTA_TREE, 0, 3, { -3570.0f, 0.0f, -2550.f }, { 510.0f, 510.0f, 510.0f });
	return S_OK;
}

HRESULT Client::CTreeManager::BeginPlay()
{
	return S_OK;
}

int32 Client::CTreeManager::Tick(float _fDeltaSeconds)
{
	/*
	for (auto& spRootNode : m_vecQuadtreeRootNodes)
	{
		if (spRootNode != nullptr)
		{
			spRootNode->Tick(_fDeltaSeconds);
		}
	}
	*/

	/*
	for (auto& spRootNode : m_vecOctreeRootNodes)
	{
		if (spRootNode != nullptr)
		{
			spRootNode->Tick(_fDeltaSeconds);
		}
	}
	*/
	return 0;
}

void Client::CTreeManager::Release()
{
	for (auto& vec4 : m_vecOctreeLeafNodes)
	{
		for (auto& vec3 : vec4)
		{
			for (auto& vec2 : vec3)
			{
				for (auto& vec1 : vec2)
				{
					vec1 = nullptr;
				}
				vec2.clear();
			}
			vec3.clear();
		}
		vec4.clear();
	}
	m_vecOctreeLeafNodes.clear();

	// 옥트리 자식 노드를 없앱니다.
	for (auto& vecChildNodes : m_vecOctreeChildNodes)
	{
		for (auto& spChildNode : vecChildNodes)
		{
			spChildNode = nullptr;
		}
		vecChildNodes.clear();
	}
	m_vecOctreeChildNodes.clear();

	for (auto& vec3 : m_vecQuadtreeLeafNodes)
	{
		for (auto& vec2 : vec3)
		{
			for (auto& vec1 : vec2)
			{
				vec1 = nullptr;
			}
			vec2.clear();
		}
		vec3.clear();
	}
	m_vecQuadtreeLeafNodes.clear();

	// 쿼드트리 자식 노드를 없앱니다.
	for (auto& vecChildNodes : m_vecQuadtreeChildNodes)
	{
		for (auto& spChildNode : vecChildNodes)
		{
			spChildNode = nullptr;
		}
		vecChildNodes.clear();
	}
	m_vecQuadtreeChildNodes.clear();

	// 옥트리 자식 노드를 해제합니다.
	for (auto& spRootNode : m_vecOctreeRootNodes)
	{
		spRootNode->Release();
		spRootNode = nullptr;
	}
	m_vecOctreeRootNodes.clear();

	// 쿼트트리 자식 노드를 해제합니다.
	for (auto& spRootNode : m_vecQuadtreeRootNodes)
	{
		spRootNode->Release();
		spRootNode = nullptr;
	}
	m_vecQuadtreeRootNodes.clear();

	// 트리 매니저를 메모리에서 해제합니다.
	m_spInstance = nullptr;
}

std::optional<std::tuple<int32, int32, int32>> Client::CTreeManager::GetOctreeNodeIndex(const Vector3& _vPosition)
{
	bool bIn = false;

	Vector3 vExtents = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 vCenter = Vector3(0.0f, 0.0f, 0.0f);
	int32 iIndex = 0;

	for (; iIndex < (int32)m_vecOctreeRootNodes.size(); ++iIndex)
	{
		vExtents = m_vecOctreeRootNodes[iIndex]->GetExtents();
		vCenter = m_vecOctreeRootNodes[iIndex]->GetCenter();

		if (_vPosition.x < vCenter.x - vExtents.x || vCenter.x + vExtents.x < _vPosition.x) { continue; }

		if (_vPosition.y < vCenter.y - vExtents.y || vCenter.y + vExtents.y < _vPosition.y) { continue; }

		if (_vPosition.z < vCenter.z - vExtents.z || vCenter.z + vExtents.z < _vPosition.z) { continue; }

		// 옥트리 안에 있는 경우
		bIn = true;
		break;
	}

	if (!bIn) { return nullopt; }

	// 옥트리 왼쪽 아래 위치를 구합니다.
	float fX = vCenter.x - vExtents.x;
	float fY = vCenter.y - vExtents.y;
	float fZ = vCenter.z - vExtents.z;

	int32 iOctreeWidth = static_cast<int32>(m_vecOctreeWidths[iIndex]);
	int32 iIndexX = static_cast<int32>(_vPosition.x - fX) / iOctreeWidth;
	int32 iIndexY = static_cast<int32>(_vPosition.y - fY) / iOctreeWidth;
	int32 iIndexZ = static_cast<int32>(_vPosition.z - fZ) / iOctreeWidth;

	return tuple(iIndexX, iIndexY, iIndexZ);
}

std::optional<std::tuple<int32, int32>> Client::CTreeManager::GetQuadtreeNodeIndex(const Vector3& _vPosition)
{
	bool bIn = false;

	Vector3 vExtents = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 vCenter = Vector3(0.0f, 0.0f, 0.0f);
	int32 iIndex = 0;

	for (; iIndex < (int32)m_vecQuadtreeRootNodes.size(); ++iIndex)
	{
		vExtents = m_vecQuadtreeRootNodes[iIndex]->GetExtents();
		vCenter = m_vecQuadtreeRootNodes[iIndex]->GetCenter();

		if (_vPosition.x < vCenter.x - vExtents.x || vCenter.x + vExtents.x < _vPosition.x) { continue; }

		if (_vPosition.y < vCenter.y - vExtents.y || vCenter.y + vExtents.y < _vPosition.y) { continue; }

		if (_vPosition.z < vCenter.z - vExtents.z || vCenter.z + vExtents.z < _vPosition.z) { continue; }

		bIn = true;
		break;
	}

	if (!bIn) { return nullopt; }

	// 쿼드트리 왼쪽 아래 위치를 구합니다.
	float fX = vCenter.x - vExtents.x;
	float fZ = vCenter.z - vExtents.z;

	int32 iQuadtreeWidth = static_cast<int32>(m_vecQuadtreeWidths[iIndex]);
	int32 iIndexX = static_cast<int32>(_vPosition.x - fX) / iQuadtreeWidth;
	int32 iIndexZ = static_cast<int32>(_vPosition.z - fZ) / iQuadtreeWidth;

	return tuple(iIndexX, iIndexZ);
}

int32 Client::CTreeManager::CreateTree(ETreeType _eTreeType, int32 _iTreeIndex, int32 _iMaxNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents)
{
	switch (_eTreeType)
	{
	case ETreeType::QUAD_TREE:
	{
		m_vecQuadtreeRootNodes.push_back(CQuadtreeNode::Create(_iTreeIndex, 0, _vCenter, _vExtents));
		m_vecQuadtreeRootNodes.shrink_to_fit();

		m_vecQuadtreeLeafNodes.push_back(std::vector<std::vector<std::shared_ptr<CQuadtreeNode>>>());
		m_vecQuadtreeLeafNodes.shrink_to_fit();

		if ((int32)m_vecNumQuadtreeLeafNodesPerSides.size() <= _iTreeIndex + 1)
		{
			m_vecNumQuadtreeLeafNodesPerSides.reserve(_iTreeIndex + 1);
			m_vecNumQuadtreeLeafNodesPerSides.resize(_iTreeIndex + 1);
		}

		if ((int32)m_vecQuadtreeWidths.size() <= _iTreeIndex + 1)
		{
			m_vecQuadtreeWidths.reserve(_iTreeIndex + 1);
			m_vecQuadtreeWidths.resize(_iTreeIndex + 1);
		}

		int32 iNumLeafNodes = m_vecNumQuadtreeLeafNodesPerSides[_iTreeIndex] = static_cast<int32>(powf(2, (float)_iMaxNodeDepth));
		m_vecQuadtreeWidths[_iTreeIndex] = _vExtents.x * 2.0f / iNumLeafNodes;

		m_vecQuadtreeLeafNodes.back().reserve(iNumLeafNodes);
		m_vecQuadtreeLeafNodes.back().resize(iNumLeafNodes);

		for (int32 iX = 0; iX < iNumLeafNodes; ++iX)
		{
			m_vecQuadtreeLeafNodes.back()[iX].reserve(iNumLeafNodes);
			m_vecQuadtreeLeafNodes.back()[iX].resize(iNumLeafNodes, nullptr);
		}

		int32 iQuadtreeIndex = static_cast<int32>(m_vecQuadtreeRootNodes.size() - 1);

		m_vecQuadtreeRootNodes.back()->CreateQuadtreeChildNodes(_iMaxNodeDepth, iQuadtreeIndex);
		return iQuadtreeIndex;
	}
	
	case ETreeType::OCTA_TREE:
	{
		m_vecOctreeRootNodes.push_back(COctreeNode::Create(_iTreeIndex, 0, _vCenter, _vExtents));
		m_vecOctreeRootNodes.shrink_to_fit();

		m_vecOctreeLeafNodes.push_back(std::vector<std::vector<std::vector<std::shared_ptr<COctreeNode>>>>());
		m_vecOctreeLeafNodes.shrink_to_fit();

		if ((int32)m_vecNumOctreeLeafNodesPerSides.size() <= _iTreeIndex + 1)
		{
			m_vecNumOctreeLeafNodesPerSides.reserve(_iTreeIndex + 1);
			m_vecNumOctreeLeafNodesPerSides.resize(_iTreeIndex + 1);
		}

		if ((int32)m_vecOctreeWidths.size() <= _iTreeIndex + 1)
		{
			m_vecOctreeWidths.reserve(_iTreeIndex + 1);
			m_vecOctreeWidths.resize(_iTreeIndex + 1);
		}

		int32 iNumLeafNodes = m_vecNumOctreeLeafNodesPerSides[_iTreeIndex] = static_cast<int32>(powf(2, (float)_iMaxNodeDepth));
		m_vecOctreeWidths[_iTreeIndex] = _vExtents.x * 2.0f / iNumLeafNodes;

		m_vecOctreeLeafNodes.back().resize(iNumLeafNodes);
		m_vecOctreeLeafNodes.back().reserve(iNumLeafNodes);

		for (int32 iX = 0; iX < iNumLeafNodes; ++iX)
		{
			m_vecOctreeLeafNodes.back()[iX].resize(iNumLeafNodes);
			m_vecOctreeLeafNodes.back()[iX].reserve(iNumLeafNodes);

			for (int32 iY = 0; iY < iNumLeafNodes; ++iY)
			{
				m_vecOctreeLeafNodes.back()[iX][iY].resize(iNumLeafNodes, nullptr);
				m_vecOctreeLeafNodes.back()[iX][iY].reserve(iNumLeafNodes);
			}
		}

		int32 iOctreeIndex = static_cast<int32>(m_vecOctreeRootNodes.size() - 1);

		m_vecOctreeRootNodes.back()->CreateOctreeChildNodes(_iMaxNodeDepth, iOctreeIndex);
		return iOctreeIndex;
	}
	
	default:
		CHECKF(false, L"Tree type is not designated.");
		break;
	}

	return -1;
}

int32 Client::CTreeManager::AddOctreeChildNode(int32 iOctreeIndex, const std::shared_ptr<COctreeNode>& _spChildNode)
{
	m_vecOctreeChildNodes.resize(iOctreeIndex + 1);
	m_vecOctreeChildNodes.reserve(iOctreeIndex + 1);

	m_vecOctreeChildNodes[iOctreeIndex].push_back(_spChildNode);
	return static_cast<int32>(m_vecOctreeChildNodes[iOctreeIndex].size() - 1);
}

int32 Client::CTreeManager::AddQuadtreeChildNode(int32 iQuadtreeIndex, const std::shared_ptr<CQuadtreeNode>& _spChildNode)
{
	m_vecQuadtreeChildNodes.resize(iQuadtreeIndex + 1);
	m_vecQuadtreeChildNodes.reserve(iQuadtreeIndex + 1);

	m_vecQuadtreeChildNodes[iQuadtreeIndex].push_back(_spChildNode);
	return static_cast<int32>(m_vecQuadtreeChildNodes[iQuadtreeIndex].size() - 1);
}

HRESULT Client::CTreeManager::SetOctreeLeafNode(int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpLeafNodeIndex, const std::shared_ptr<COctreeNode>& _spTreeNode)
{
	if (FAILED(ThrowOctreeException(_iOctreeIndex, _tpLeafNodeIndex))) { return E_FAIL; }

	auto& [iX, iY, iZ] = _tpLeafNodeIndex;
	m_vecOctreeLeafNodes[_iOctreeIndex][iX][iY][iZ] = _spTreeNode;
	return S_OK;
}

HRESULT Client::CTreeManager::SetQuadtreeLeafNode(int32 _iQuadtreeIndex, const std::tuple<int32, int32>& _tpLeafNodeIndex, const std::shared_ptr<CQuadtreeNode>& _spTreeNode)
{
	if (FAILED(ThrowQuadtreeException(_iQuadtreeIndex, _tpLeafNodeIndex))) { return E_FAIL; }

	auto& [iX, iZ] = _tpLeafNodeIndex;
	m_vecQuadtreeLeafNodes[_iQuadtreeIndex][iX][iZ] = _spTreeNode;
	return S_OK;
}

HRESULT Client::CTreeManager::AddGameObject(const std::shared_ptr<CGameObject>& _spGameObejct, int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpLeafNodeIndex)
{
	auto& [x, y, z] = _tpLeafNodeIndex;
	if (x < 0 || y < 0 || z < 0) { return E_FAIL; }

	if (_iOctreeIndex < 0) { return E_FAIL; }

	return m_vecOctreeLeafNodes[_iOctreeIndex][x][y][z]->AddGameObjectToNode(_spGameObejct);
}

HRESULT Client::CTreeManager::RemoveGameObject(EMobilityType _eMobilityType, int32 _iGameObjectID, int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpLeafNodeIndex)
{
	auto& [x, y, z] = _tpLeafNodeIndex;
	if (x < 0 || y < 0 || z < 0) { return E_FAIL; }

	if (_iOctreeIndex < 0) { return E_FAIL; }

	return m_vecOctreeLeafNodes[_iOctreeIndex][x][y][z]->RemoveGameObjectFromNodeByID(_eMobilityType, _iGameObjectID);
}

std::shared_ptr<CGameObject> Client::CTreeManager::FindGameObject(EMobilityType _eMobilityType, int32 _iGameObjectID, int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpLeafNodeIndex)
{
	auto& [x, y, z] = _tpLeafNodeIndex;
	if (x < 0 || y < 0 || z < 0) { return nullptr; }

	if (_iOctreeIndex < 0) { return nullptr; }

	return m_vecOctreeLeafNodes[_iOctreeIndex][x][y][z]->FindGameObjectFromNode(_eMobilityType, _iGameObjectID);
}

void Client::CTreeManager::MoveGameObject
(
	EMobilityType _eMobilityType,
	uint32 _iGameObjectID,
	int32 _iFromOctreeIndex,
	const std::tuple<int32, int32, int32>& _tpFromLeafNodeIndex,
	int32 _iToOctreeIndex,
	const std::tuple<int32, int32, int32>& _tpToLeafNodeIndex
)
{
	shared_ptr<CGameObject> spGameObject = FindGameObject(_eMobilityType, _iGameObjectID, _iFromOctreeIndex, _tpFromLeafNodeIndex);
	CHECKF(spGameObject, L"GameObject is nullptr.");

	AddGameObject(spGameObject, _iToOctreeIndex, _tpToLeafNodeIndex);
	RemoveGameObject(_eMobilityType, _iGameObjectID, _iFromOctreeIndex, _tpFromLeafNodeIndex);
}

// env collision
std::shared_ptr<CQuadtreeNode> Client::CTreeManager::AllocEnvCollision(int32 _iTreeIndex, const std::tuple<int32, int32> _tpQuadtreeNodeIndex)
{
	auto& [iX, iZ] = _tpQuadtreeNodeIndex;
	return m_vecQuadtreeLeafNodes[_iTreeIndex][iX][iZ]->AllocEnvCollision();
}

HRESULT Client::CTreeManager::ActivateEnvCollision(int32 _iTreeIndex, const std::tuple<int32, int32>& _tpQuadtreeNodeIndex)
{
	auto& [iX, iZ] = _tpQuadtreeNodeIndex;
	return m_vecQuadtreeLeafNodes[_iTreeIndex][iX][iZ]->ActivateEnvCollision();
}

HRESULT Client::CTreeManager::DeactivateEnvCollision(int32 _iTreeIndex, const std::tuple<int32, int32>& _tpQuadtreeNodeIndex)
{
	auto& [iX, iZ] = _tpQuadtreeNodeIndex;
	return m_vecQuadtreeLeafNodes[_iTreeIndex][iX][iZ]->DeactivateEnvCollision();
}

// env mesh
std::shared_ptr<CQuadtreeNode> Client::CTreeManager::AllocEnvMesh(int32 _iTreeIndex, const std::tuple<int32, int32> _tpQuadtreeNodeIndex)
{
	auto& [iX, iZ] = _tpQuadtreeNodeIndex;
	return m_vecQuadtreeLeafNodes[_iTreeIndex][iX][iZ]->AllocEnvMesh();
}

// gameobject
std::shared_ptr<COctreeNode> Client::CTreeManager::AllocGameObject(int32 _iOctreeIndex, const std::tuple<int32, int32, int32> _tpOctreeNodeIndex)
{
	auto& [iX, iY, iZ] = _tpOctreeNodeIndex;
	return m_vecOctreeLeafNodes[_iOctreeIndex][iX][iY][iZ]->LoadJsonAndAllocGameObject();
}

HRESULT Client::CTreeManager::ActivateGameObject(int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpOctreeNodeIndex)
{
	auto& [iX, iY, iZ] = _tpOctreeNodeIndex;
	return m_vecOctreeLeafNodes[_iOctreeIndex][iX][iY][iZ]->ActivateGameObject();
}

HRESULT Client::CTreeManager::DeactivateGameObject(int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpOctreeNodeIndex)
{
	auto& [iX, iY, iZ] = _tpOctreeNodeIndex;
	return m_vecOctreeLeafNodes[_iOctreeIndex][iX][iY][iZ]->DeactivateGameObject();
}

// exception
HRESULT Client::CTreeManager::ThrowOctreeException(int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpLeafNodeIndex)
{
	if (m_vecNumOctreeLeafNodesPerSides[_iOctreeIndex] < 0) { return E_FAIL; }

	if (_iOctreeIndex < 0) { return E_FAIL; }

	auto& [iX, iY, iZ] = _tpLeafNodeIndex;

	if (iX < 0 || m_vecNumOctreeLeafNodesPerSides[_iOctreeIndex] <= iX) { return E_FAIL; }

	if (iY < 0 || m_vecNumOctreeLeafNodesPerSides[_iOctreeIndex] <= iY) { return E_FAIL; }

	if (iZ < 0 || m_vecNumOctreeLeafNodesPerSides[_iOctreeIndex] <= iZ) { return E_FAIL; }

	return S_OK;
}

HRESULT Client::CTreeManager::ThrowQuadtreeException(int32 _iQuadtreeIndex, const std::tuple<int32, int32>& _tpLeafNodeIndex)
{
	if (m_vecNumQuadtreeLeafNodesPerSides[_iQuadtreeIndex] < 0) { return E_FAIL; }

	if (_iQuadtreeIndex < 0) { return E_FAIL; }

	auto& [iX, iZ] = _tpLeafNodeIndex;

	if (iX < 0 || m_vecNumQuadtreeLeafNodesPerSides[_iQuadtreeIndex] <= iX) { return E_FAIL; }

	if (iZ < 0 || m_vecNumQuadtreeLeafNodesPerSides[_iQuadtreeIndex] <= iZ) { return E_FAIL; }

	return S_OK;
}
