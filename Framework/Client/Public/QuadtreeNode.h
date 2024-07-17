#pragma once
#include "TreeNode.h"

namespace Client
{
	class CTreeManager;

	class CEnvCollision;
	class CEnvMesh;

	class CQuadtreeNode : public CTreeNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CQuadtreeNode() = default;

		virtual ~CQuadtreeNode() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CQuadtreeNode> Create(int32 _iQuadtreeIndex, uint32 _iNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents);

		virtual HRESULT Initialize(int32 _iTreeIndex, uint32 _iNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents) override;
		
	/********************
		Methods
	********************/
	public:
		HRESULT CreateQuadtreeChildNodes(uint32 _iMaxNodeDepth, int32 _iQuadtreeIndex);

	public:
		std::shared_ptr<CQuadtreeNode> AllocEnvCollision();

		HRESULT ActivateEnvCollision();

		HRESULT DeactivateEnvCollision();

	public:
		std::shared_ptr<CQuadtreeNode> AllocEnvMesh();

	/********************
		Data Members
	********************/
	private:
		// �� ��带 ���� �ִ� Ʈ�� �Ŵ���
		std::weak_ptr<CTreeManager> m_wpTreeManager;

	private:
		// �� ����� ��Ʈ�� �ε���
		std::tuple<int32, int32> m_tpQuadtreeIndex{ -1, -1 };

		// �� ��尡 ���� �ִ� ȯ�� �ݸ���
		std::shared_ptr<CEnvCollision> m_spEnvCollision = nullptr;

		// �� ��尡 ���� �ִ� ȯ�� �޽�
		std::shared_ptr<CEnvMesh> m_spEnvMesh = nullptr;

	private:
		std::atomic<bool> m_bEnvCollisionAlloced = false;

		std::atomic<bool> m_bEnvMeshAlloced = false;
	};
}
