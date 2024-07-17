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
		// 트리 매니저
		std::shared_ptr<CTreeManager> m_spTreeManager = nullptr;

		// 스레드 매니저
		std::shared_ptr<CThreadManager> m_spThreadManager = nullptr;

		// 타겟, 즉 플레이어
		std::weak_ptr<CGameObject> m_wpSyncTarget;

		// 현재 레벨
		std::weak_ptr<CLevel> m_wpCurLevel;

	private:
		// 타겟이 있었던 이전 쿼드트리 인덱스
		std::tuple<int32, int32> m_tpPreQuadtreeNodeIndex{ -1, -1 };

		// 타겟이 있는 현재 쿼드트리 인덱스
		std::tuple<int32, int32> m_tpCurQuadtreeNodeIndex{ -1, -1 };

		// 쿼드 트리 한 변의 길이
		int32 m_iQuadtreeWidth = -1;

		// 삭제된 쿼드트리 노드의 오름차순 인덱스들
		std::set<std::tuple<int32, int32>> m_sRemoveQuadtreeIndexGroup;

		// 추가된 쿼드트리 노드의 오름차순 인덱스들
		std::set<std::tuple<int32, int32>> m_sAddQuadtreeIndexGroup;

		// 덱으로 나열된 삭제된 쿼드트리 노드의 인덱스들
		std::deque<std::tuple<int32, int32>> m_dqRemoveQuadtreeIndexGroup;

		// 덱으로 나열된 추가된 쿼드트리 노드의 인덱스들
		std::deque<std::tuple<int32, int32>> m_dqAddQuadtreeIndexGroup;

	private:
		// 타겟이 있었던 이전 옥트리 인덱스
		std::tuple<int32, int32, int32> m_tpCurOctreeNodeIndex{ -1, -1, -1 };

		// 타겟이 있는 현재 옥트리 인덱스
		std::tuple<int32, int32, int32> m_tpPreOctreeNodeIndex{ -1, -1, -1 };

		// 옥트리 한 변의 길이
		int32 m_iOctreeWidth = -1;

		// 삭제된 옥트리 노드의 오름차순 인덱스들
		std::set<tuple<int32, int32, int32>> m_sRemoveOctreeIndexGroup;

		// 추가된 옥트리 노드의 오름차순 인덱스들
		std::set<tuple<int32, int32, int32>> m_sAddOctreeIndexGroup;

		// 덱으로 나열된 삭제된 옥트리 노드의 인덱스들
		std::deque<std::tuple<int32, int32, int32>> m_dqRemoveOctreeIndexGroup;

		// 덱으로 나열된 추가된 옥트리 노드의 인덱스들
		std::deque<std::tuple<int32, int32, int32>> m_dqAddOctreeIndexGroup;
	};
}
