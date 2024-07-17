#pragma once
#include "Manager.h"

namespace Engine
{
	class CLevel;
	class CGameObject;
	class CThreadManager;
}

namespace Client
{
	class CTreeManager;
	class COctreeNode;
	class CQuadtreeNode;

	class CAsyncManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CAsyncManager)

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		HRESULT BeginPlay();

		int32 Tick(float _fDeltaSeconds);

		void Release();
		
	/********************
		Methds
	********************/
	public:
		void SyncTarget();

	private:
		void CalculateAddedTreeIndices();

		void CalculateRemovedTreeIndices();

	private:
		// env mesh, env collision
		void AllocEnvs();

		void ActivateEnvCollision();

		void DeactivateEnvCollision();

	private:
		void AllocGameObject();

		void ActivateGameObject();

		void DeactivateGameObject();

	/********************
		Getter/Setter
	********************/
	public:
		void SetSyncTarget(const std::shared_ptr<CGameObject>& _spSyncTarget) { m_wpSyncTarget = _spSyncTarget; }
		
	/********************
		Data Members
	********************/
	private:
		// Ʈ�� �Ŵ���
		std::shared_ptr<CTreeManager> m_spTreeManager = nullptr;

		// ������ �Ŵ���
		std::shared_ptr<CThreadManager> m_spThreadManager = nullptr;

		// Ÿ��, �� �÷��̾�
		std::weak_ptr<CGameObject> m_wpSyncTarget;

		// ���� ����
		std::weak_ptr<CLevel> m_wpCurLevel;

	private:
		// Ÿ���� �־��� ���� ����Ʈ�� �ε���
		std::tuple<int32, int32> m_tpPreQuadtreeNodeIndex{ -1, -1 };

		// Ÿ���� �ִ� ���� ����Ʈ�� �ε���
		std::tuple<int32, int32> m_tpCurQuadtreeNodeIndex{ -1, -1 };

		// ���� Ʈ�� �� ���� ����
		int32 m_iQuadtreeWidth = -1;

		// ������ ����Ʈ�� ����� �������� �ε�����
		std::set<std::tuple<int32, int32>> m_sRemoveQuadtreeIndexGroup;

		// �߰��� ����Ʈ�� ����� �������� �ε�����
		std::set<std::tuple<int32, int32>> m_sAddQuadtreeIndexGroup;

		// ������ ������ ������ ����Ʈ�� ����� �ε�����
		std::deque<std::tuple<int32, int32>> m_dqRemoveQuadtreeIndexGroup;

		// ������ ������ �߰��� ����Ʈ�� ����� �ε�����
		std::deque<std::tuple<int32, int32>> m_dqAddQuadtreeIndexGroup;

	private:
		// Ÿ���� �־��� ���� ��Ʈ�� �ε���
		std::tuple<int32, int32, int32> m_tpCurOctreeNodeIndex{ -1, -1, -1 };

		// Ÿ���� �ִ� ���� ��Ʈ�� �ε���
		std::tuple<int32, int32, int32> m_tpPreOctreeNodeIndex{ -1, -1, -1 };

		// ��Ʈ�� �� ���� ����
		int32 m_iOctreeWidth = -1;

		// ������ ��Ʈ�� ����� �������� �ε�����
		std::set<tuple<int32, int32, int32>> m_sRemoveOctreeIndexGroup;

		// �߰��� ��Ʈ�� ����� �������� �ε�����
		std::set<tuple<int32, int32, int32>> m_sAddOctreeIndexGroup;

		// ������ ������ ������ ��Ʈ�� ����� �ε�����
		std::deque<std::tuple<int32, int32, int32>> m_dqRemoveOctreeIndexGroup;

		// ������ ������ �߰��� ��Ʈ�� ����� �ε�����
		std::deque<std::tuple<int32, int32, int32>> m_dqAddOctreeIndexGroup;
	};
}
