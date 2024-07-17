#pragma once
#include "TreeNode.h"

namespace Engine
{
	class CLevel;
}

namespace Client
{
	class CTreeManager;
	class CHabitat;
	class CMonster;

	class COctreeNode : public CTreeNode
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit COctreeNode() = default;

		virtual ~COctreeNode() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<COctreeNode> Create(int32 _iOctreeIndex, uint32 _iNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents);

		virtual HRESULT Initialize(int32 _iTreeIndex, uint32 _iNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents) override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		HRESULT CreateOctreeChildNodes(uint32 _iMaxNodeDepth, int32 _iOctreeIndex);

	public:
		std::shared_ptr<COctreeNode> LoadJsonAndAllocGameObject();

		HRESULT ActivateGameObject();

		HRESULT DeactivateGameObject();

	private:
		HRESULT CreateRocks(const FRockCache& _tRockCache);

		HRESULT CreateTrees(const FTreeCache& _tTreeCache);

		HRESULT CreatePals(const FPalCache& _tPalCache);

	/********************
		Data Members
	********************/
	private:
		// �� ��带 ���� �ִ� Ʈ�� �Ŵ���
		std::weak_ptr<CTreeManager> m_wpTreeManager;

	private:
		// �� ����� ��Ʈ�� �ε���
		std::tuple<int32, int32, int32> m_tpOctreeIndex{ -1, -1, -1 };

		// �� ��尡 ���� �ִ� ��������
		// std::vector<std::shared_ptr<CHabitat>> m_vecHabitats;

		// �� ��尡 ���� �ִ� �ӵ�
		// std::vector<std::shared_ptr<CMonster>> m_vecPals;

		std::wstring m_wstrGameObjectJsonFileName = L"Octree8_";

	private:
		std::atomic<bool> m_bGameObjectAlloced = false;

		std::mutex m_mtMutex;
	};
}
