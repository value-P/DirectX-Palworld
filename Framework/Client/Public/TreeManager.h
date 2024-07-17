#pragma once
#include "Manager.h"

namespace Engine
{
	class CCameraManager;
	class CGameObject;
	class CTreeNode;
}

namespace Client
{
	class CQuadtreeNode;
	class COctreeNode;

	enum class ETreeType
	{
		QUAD_TREE,
		OCTA_TREE,
		ENUM_END
	};

	class CTreeManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CTreeManager)

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		HRESULT BeginPlay();

		int32 Tick(float _fDeltaSeconds);

		virtual void Release() override;
		
	/********************
		Getter/Setter
	********************/
	public:
		std::shared_ptr<COctreeNode> GetOctreeRootNode(int32 _iOctreeIndex) { return m_vecOctreeRootNodes[_iOctreeIndex]; }

		std::shared_ptr<CQuadtreeNode> GetQuadtreeRootNode(int32 _iQuadtreeIndex) { return m_vecQuadtreeRootNodes[_iQuadtreeIndex]; }

		std::optional<std::tuple<int32, int32, int32>> GetOctreeNodeIndex(const Vector3& _vPosition);

		std::optional<std::tuple<int32, int32>> GetQuadtreeNodeIndex(const Vector3& _vPosition);

		HRESULT SetOctreeLeafNode(int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpLeafNodeIndex, const std::shared_ptr<COctreeNode>& _spTreeNode);

		HRESULT SetQuadtreeLeafNode(int32 _iQuadtreeIndex, const std::tuple<int32, int32>& _tpLeafNodeIndex, const std::shared_ptr<CQuadtreeNode>& _spTreeNode);

	/********************
		Methods
	********************/
	public:
		int32 CreateTree(ETreeType _eTreeType, int32 _iTreeIndex, int32 _iMaxNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents);

		int32 AddOctreeChildNode(int32 iOctreeIndex, const std::shared_ptr<COctreeNode>& _spChildNode);

		int32 AddQuadtreeChildNode(int32 iQuadtreeIndex, const std::shared_ptr<CQuadtreeNode>& _spChildNode);

	public:
		HRESULT AddGameObject(const std::shared_ptr<CGameObject>& _spGameObejct, int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpLeafNodeIndex);

		HRESULT RemoveGameObject(EMobilityType _eMobilityType, int32 _iGameObjectID, int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpLeafNodeIndex);

		std::shared_ptr<CGameObject> FindGameObject(EMobilityType _eMobilityType, int32 _iGameObjectID, int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpLeafNodeIndex);

		void MoveGameObject
		(
			EMobilityType _eMobilityType,
			uint32 _iGameObjectID,
			int32 _iFromOctreeIndex,
			const std::tuple<int32, int32, int32>& _tpFromLeafNodeIndex,
			int32 _iToOctreeIndex,
			const std::tuple<int32, int32, int32>& _tpToLeafNodeIndex
		);

	public:
		// env collisions
		std::shared_ptr<CQuadtreeNode> AllocEnvCollision(int32 _iTreeIndex, const std::tuple<int32, int32> _tpQuadtreeNodeIndex);

		HRESULT ActivateEnvCollision(int32 _iTreeIndex, const std::tuple<int32, int32>& _tpQuadtreeNodeIndex);

		HRESULT DeactivateEnvCollision(int32 _iTreeIndex, const std::tuple<int32, int32>& _tpQuadtreeNodeIndex);

	public:
		// env meshes
		std::shared_ptr<CQuadtreeNode> AllocEnvMesh(int32 _iTreeIndex, const std::tuple<int32, int32> _tpQuadtreeNodeIndex);

	public:
		// habitats
		std::shared_ptr<COctreeNode> AllocGameObject(int32 _iOctreeIndex, const std::tuple<int32, int32, int32> _tpOctreeNodeIndex);

		HRESULT ActivateGameObject(int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpOctreeNodeIndex);

		HRESULT DeactivateGameObject(int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpOctreeNodeIndex);

	private:
		// exception
		HRESULT ThrowOctreeException(int32 _iOctreeIndex, const std::tuple<int32, int32, int32>& _tpLeafNodeIndex);

		HRESULT ThrowQuadtreeException(int32 _iQuadtreeIndex, const std::tuple<int32, int32>& _tpLeafNodeIndex);

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CCameraManager> m_wpCameraManager;

	private:
		// ��Ʈ�� �� ���� ����
		std::vector<float> m_vecOctreeWidths;

		// ��Ʈ�� �� ���� �ִ� ���� ��� ��
		std::vector<int32> m_vecNumOctreeLeafNodesPerSides;

		// ��Ʈ�� ��Ʈ ���
		std::vector<std::shared_ptr<COctreeNode>> m_vecOctreeRootNodes;

		// ��� ��Ʈ�� �ڽ� ����
		std::vector<std::vector<std::shared_ptr<COctreeNode>>> m_vecOctreeChildNodes;

		// 3���� �迭�� �� ��Ʈ�� ���� ����
		std::vector<std::vector<std::vector<std::vector<std::shared_ptr<COctreeNode>>>>> m_vecOctreeLeafNodes;
		
	private:
		// ����Ʈ�� �� ���� ����
		std::vector<float> m_vecQuadtreeWidths;

		// ����Ʈ�� �� ���� �ִ� ���� ��� ��
		std::vector<int32> m_vecNumQuadtreeLeafNodesPerSides;

		// ����Ʈ�� ��Ʈ ���
		std::vector<std::shared_ptr<CQuadtreeNode>> m_vecQuadtreeRootNodes;

		// ��� ����Ʈ�� �ڽ� ����
		std::vector<std::vector<std::shared_ptr<CQuadtreeNode>>> m_vecQuadtreeChildNodes;

		// 2���� �迭�� �� ����Ʈ�� ���� ����
		std::vector<std::vector<std::vector<std::shared_ptr<CQuadtreeNode>>>> m_vecQuadtreeLeafNodes;
	};
}
