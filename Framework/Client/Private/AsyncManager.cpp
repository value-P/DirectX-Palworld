#include "pch.h"
#include "AsyncManager.h"

#include "EngineModule.h"
#include "TreeManager.h"
#include "ThreadManager.h"

#include "GameObject.h"
#include "OctreeNode.h"
#include "QuadtreeNode.h"

IMPLEMENT_SINGLETON(CAsyncManager)

HRESULT Client::CAsyncManager::Initialize()
{
	m_spTreeManager = CTreeManager::GetInstance();
	m_spThreadManager = CEngineModule::GetInstance()->GetThreadManager();

	AllocEnvs();
	AllocGameObject();
	return S_OK;
}

HRESULT Client::CAsyncManager::BeginPlay()
{
	for (int32 iOuterIndex = 0; iOuterIndex < 4; ++iOuterIndex)
	{
		for (int32 iInnerIndex = 0; iInnerIndex < 4; ++iInnerIndex)
		{
			m_spTreeManager->DeactivateEnvCollision(0, { iInnerIndex, iOuterIndex });
		}
	}

	for (int32 iOuterIndex = 0; iOuterIndex < 8; ++iOuterIndex)
	{
		for (int32 iMiddleIndex = 0; iMiddleIndex < 8; ++iMiddleIndex)
		{
			for (int32 iInnerIndex = 0; iInnerIndex < 8; ++iInnerIndex)
			{
				m_spTreeManager->DeactivateGameObject(0, { iInnerIndex, iMiddleIndex, iOuterIndex });
			}
		}
	}
	return S_OK;
}

int32 Client::CAsyncManager::Tick(float _fDeltaSeconds)
{
	if (m_wpSyncTarget.expired()) { return 0; }
	
	SyncTarget();
	DeactivateEnvCollision();
	ActivateEnvCollision();
	DeactivateGameObject();
	ActivateGameObject();
	return 0;
}

void Client::CAsyncManager::Release()
{
	m_spTreeManager = nullptr;
	m_spThreadManager = nullptr;
	
	// 싱글턴 매니저를 메모리에서 해제합니다.
	m_spInstance = nullptr;
}

void Client::CAsyncManager::SyncTarget()
{
	// 게임 오브젝트의 위치를 구합니다.
	if (m_wpSyncTarget.expired()) { return; }
	Vector3 vPosition = m_wpSyncTarget.lock()->GetPosition().value();

	// 옥트리 인덱스를 구합니다.
	if (!m_spTreeManager->GetOctreeNodeIndex(vPosition).has_value()) { return; }
	const tuple<int32, int32, int32> tpOctreeNodeIndex = m_spTreeManager->GetOctreeNodeIndex(vPosition).value();
	auto& [iOctreeIndexX, iOctreeIndexY, iOctreeIndexZ] = tpOctreeNodeIndex;
	
	// 쿼드트리 인덱스를 구합니다.
	const tuple<int32, int32> tpQuadtreeNodeIndex = { iOctreeIndexX / 2, iOctreeIndexZ / 2 };
	auto& [iQuadtreeIndexX, iQuadtreeIndexZ] = tpQuadtreeNodeIndex;

	// 현재 옥트리 위치를 업데이트합니다.
	bool bOctreeChanged = false;
	if (m_tpCurOctreeNodeIndex != tpOctreeNodeIndex)
	{
		m_tpPreOctreeNodeIndex = m_tpCurOctreeNodeIndex;
		m_tpCurOctreeNodeIndex = tpOctreeNodeIndex;
		bOctreeChanged = true;
	}

	if (!bOctreeChanged) { return; }
	bOctreeChanged = false;

	CalculateRemovedTreeIndices();
	CalculateAddedTreeIndices();

	// 쿼드트리 노드 변경 사항을 갱신합니다.
	vector<tuple<int32, int32>> tpQuadRemoved;

	if (!m_dqAddQuadtreeIndexGroup.empty() && !m_sRemoveQuadtreeIndexGroup.empty())
	{
		sort(m_dqAddQuadtreeIndexGroup.begin(), m_dqAddQuadtreeIndexGroup.end());

		for (auto iterSet = m_sRemoveQuadtreeIndexGroup.begin(); iterSet != m_sRemoveQuadtreeIndexGroup.end(); ++iterSet)
		{
			auto iterDeque = std::find(m_dqAddQuadtreeIndexGroup.begin(), m_dqAddQuadtreeIndexGroup.end(), *iterSet);

			if (iterDeque != m_dqAddQuadtreeIndexGroup.end())
			{
				m_dqAddQuadtreeIndexGroup.erase(iterDeque);
				tpQuadRemoved.push_back(*iterSet);
			}
		}
	}

	for (const auto& tpIndex : tpQuadRemoved)
	{
		m_sRemoveQuadtreeIndexGroup.erase(tpIndex);
	}

	// if (!m_dqAddQuadtreeIndexGroup.empty()) { m_dqAddQuadtreeIndexGroup.clear(); }
	for (const auto& tpNodeIndex : m_sAddQuadtreeIndexGroup) { m_dqAddQuadtreeIndexGroup.push_back(tpNodeIndex); }
	m_sAddQuadtreeIndexGroup.clear();

	// if (!m_dqRemoveQuadtreeIndexGroup.empty()) { m_dqRemoveQuadtreeIndexGroup.clear(); }
	for (const auto& tpNodeIndex : m_sRemoveQuadtreeIndexGroup) { m_dqRemoveQuadtreeIndexGroup.push_back(tpNodeIndex); }
	m_sRemoveQuadtreeIndexGroup.clear();


	// 옥트리 노드 변경 사항을 갱신합니다.
	vector<tuple<int32, int32, int32>> tpOcRemoved;

	if (!m_dqAddOctreeIndexGroup.empty() && !m_sRemoveOctreeIndexGroup.empty())
	{
		sort(m_dqAddOctreeIndexGroup.begin(), m_dqAddOctreeIndexGroup.end());

		for (auto iterSet = m_sRemoveOctreeIndexGroup.begin(); iterSet != m_sRemoveOctreeIndexGroup.end(); ++iterSet)
		{
			auto iterDeque = std::find(m_dqAddOctreeIndexGroup.begin(), m_dqAddOctreeIndexGroup.end(), *iterSet);

			if (iterDeque != m_dqAddOctreeIndexGroup.end())
			{
				m_dqAddOctreeIndexGroup.erase(iterDeque);
				tpOcRemoved.push_back(*iterSet);
			}
		}
	}

	for (const auto& tpIndex : tpOcRemoved)
	{
		m_sRemoveOctreeIndexGroup.erase(tpIndex);
	}

	// if (!m_dqAddOctreeIndexGroup.empty()) { m_dqAddOctreeIndexGroup.clear(); }
	for (const auto& tpNodeIndex : m_sAddOctreeIndexGroup) { m_dqAddOctreeIndexGroup.push_back(tpNodeIndex); }
	m_sAddOctreeIndexGroup.clear();

	// if (!m_dqRemoveOctreeIndexGroup.empty()) { m_dqRemoveOctreeIndexGroup.clear(); }
	for (const auto& tpNodeIndex : m_sRemoveOctreeIndexGroup) { m_dqRemoveOctreeIndexGroup.push_back(tpNodeIndex); }
	m_sRemoveOctreeIndexGroup.clear();
}

void Client::CAsyncManager::CalculateRemovedTreeIndices()
{
	// 삭제된 노드 인덱스를 구합니다.
	for (int32 iX = -1; iX <= 1; ++iX)
	{
		for (int32 iZ = -1; iZ <= 1; ++iZ)
		{
			int32 iPreX = -1;
			int32 iPreY = -1;
			int32 iPreZ = -1;

			for (int32 iY = -1; iY <= 1; ++iY)
			{
				iPreX = get<0>(m_tpPreOctreeNodeIndex);
				iPreY = get<1>(m_tpPreOctreeNodeIndex);
				iPreZ = get<2>(m_tpPreOctreeNodeIndex);

				if (iPreX == -1 && iPreY == -1 && iPreZ == -1) { return; }

				int32 iOctreePreX = iPreX + iX;
				int32 iOctreePreY = iPreY + iY;
				int32 iOctreePreZ = iPreZ + iZ;

				if (iOctreePreX < 0 || iOctreePreY < 0 || iOctreePreZ < 0) { continue; }
				if (8 <= iOctreePreX || 8 <= iOctreePreY || 8 <= iOctreePreZ) { continue; }

				m_sRemoveOctreeIndexGroup.insert({ iOctreePreX, iOctreePreY, iOctreePreZ });
			}

			// 네 꼭지점에 있는 노드가 소속된 쿼드트리 인덱스를 구합니다.
			if ((iX == -1 && iZ == -1) ||
				(iX == -1 && iZ == +1) ||
				(iX == +1 && iZ == -1) ||
				(iX == +1 && iZ == +1))
			{
				int32 iOctreePreX = iPreX + iX;
				int32 iOctreePreZ = iPreZ + iZ;

				if (iOctreePreX < 0 || iOctreePreZ < 0) { continue; }
				if (8 <= iOctreePreX || 8 <= iOctreePreZ) { continue; }

				int32 iQuadtreePreX = (iPreX + iX) / 2;
				int32 iQuadtreePreZ = (iPreZ + iZ) / 2;

				m_sRemoveQuadtreeIndexGroup.insert({ iQuadtreePreX, iQuadtreePreZ });
			}
		}
	}
}

void Client::CAsyncManager::CalculateAddedTreeIndices()
{
	// 추가된 노드 인덱스를 구합니다.
	for (int32 iX = -1; iX <= 1; ++iX)
	{
		for (int32 iZ = -1; iZ <= 1; ++iZ)
		{
			int32 iCurX = -1;
			int32 iCurY = -1;
			int32 iCurZ = -1;

			for (int32 iY = -1; iY <= 1; ++iY)
			{
				// 범위를 벗어나는 인덱스를 제외합니다.
				iCurX = get<0>(m_tpCurOctreeNodeIndex);
				iCurY = get<1>(m_tpCurOctreeNodeIndex);
				iCurZ = get<2>(m_tpCurOctreeNodeIndex);

				int32 iOctreeCurX = iCurX + iX;
				int32 iOctreeCurY = iCurY + iY;
				int32 iOctreeCurZ = iCurZ + iZ;

				if (iOctreeCurX < 0 || iOctreeCurY < 0 || iOctreeCurZ < 0) { continue; }
				if (8 <= iOctreeCurX || 8 <= iOctreeCurY || 8 <= iOctreeCurZ) { continue; }

				// 범위 안에 있는 인덱스로 옥트리 이전, 이후 인덱스 묶음을 평가합니다.
				if (!m_sRemoveOctreeIndexGroup.contains({ iOctreeCurX, iOctreeCurY, iOctreeCurZ }))
				{
					m_sAddOctreeIndexGroup.insert({ iOctreeCurX, iOctreeCurY, iOctreeCurZ });
				}
				else
				{
					m_sRemoveOctreeIndexGroup.erase({ iOctreeCurX, iOctreeCurY, iOctreeCurZ });
				}
			}

			// 네 꼭지점에 있는 노드가 소속된 쿼드트리 인덱스를 구합니다.
			if ((iX == -1 && iZ == -1) ||
				(iX == -1 && iZ == +1) ||
				(iX == +1 && iZ == -1) ||
				(iX == +1 && iZ == +1))
			{
				// 범위를 벗어나는 인덱스를 제외합니다.
				int32 iOctreeCurX = iCurX + iX;
				int32 iOctreeCurZ = iCurZ + iZ;

				if (iOctreeCurX < 0 || iOctreeCurZ < 0) { continue; }
				if (8 <= iOctreeCurX || 8 <= iOctreeCurZ) { continue; }

				// 범위 안에 있는 인덱스로 쿼드트리 이전, 이후 인덱스 묶음을 평가합니다.
				int32 iQuadtreeCurX = (iCurX + iX) / 2;
				int32 iQuadtreeCurZ = (iCurZ + iZ) / 2;

				if (!m_sRemoveQuadtreeIndexGroup.contains({ iQuadtreeCurX, iQuadtreeCurZ }))
				{
					m_sAddQuadtreeIndexGroup.insert({ iQuadtreeCurX, iQuadtreeCurZ });
				}
				else
				{
					m_sRemoveQuadtreeIndexGroup.erase({ iQuadtreeCurX, iQuadtreeCurZ });
				}
			}
		}
	}
}

void Client::CAsyncManager::AllocEnvs()
{
	for (int32 iIndex = 0; iIndex < 4; ++iIndex)
	{
		for (int32 jIndex = 0; jIndex < 4; ++jIndex)
		{
			m_spTreeManager->AllocEnvMesh(0, { iIndex, jIndex });
			m_spTreeManager->AllocEnvCollision(0, { iIndex, jIndex });
		}
	}
}

void Client::CAsyncManager::ActivateEnvCollision()
{
	if (m_wpSyncTarget.expired()) { return; }

	// 추가된 노드가 있는 경우
	if (m_dqAddQuadtreeIndexGroup.empty()) { return; }

	// 동기로 서식지, 팰을 활성화합니다.
	for (const auto& tpAddIndex : m_dqAddQuadtreeIndexGroup)
	{
		m_spTreeManager->ActivateEnvCollision(0, tpAddIndex);
	}
	m_dqAddQuadtreeIndexGroup.clear();
}

void Client::CAsyncManager::DeactivateEnvCollision()
{
	if (m_wpSyncTarget.expired()) { return; }

	// 추가된 노드가 있는 경우
	if (m_dqRemoveQuadtreeIndexGroup.empty()) { return; }

	// 동기로 서식지, 팰을 활성화합니다.
	for (const auto& tpRemoveIndex : m_dqRemoveQuadtreeIndexGroup)
	{
		m_spTreeManager->DeactivateEnvCollision(0, tpRemoveIndex);
	}
	m_dqRemoveQuadtreeIndexGroup.clear();
}

void Client::CAsyncManager::AllocGameObject()
{
	for (int32 iIndex = 0; iIndex < 8; ++iIndex)
	{
		for (int32 jIndex = 0; jIndex < 8; ++jIndex)
		{
			for (int32 kIndex = 0; kIndex < 8; ++kIndex)
			{
				m_spTreeManager->AllocGameObject(0, { iIndex, jIndex, kIndex });
			}
		}
	}
}

void Client::CAsyncManager::ActivateGameObject()
{
	if (m_wpSyncTarget.expired()) { return; }

	// 추가된 노드가 있는 경우
	if (m_dqAddOctreeIndexGroup.empty()) { return; }

	// 동기로 서식지, 팰을 활성화합니다.
	for (const auto& tpAddIndex : m_dqAddOctreeIndexGroup)
	{
		m_spTreeManager->ActivateGameObject(0, tpAddIndex);
	}
	m_dqAddOctreeIndexGroup.clear();
}

void Client::CAsyncManager::DeactivateGameObject()
{
	if (m_wpSyncTarget.expired()) { return; }

	// 삭제된 노드가 없는 경우
	if (m_dqRemoveOctreeIndexGroup.empty()) { return; }

	// 동기로 서식지, 팰을 비활성화합니다.
	for (const auto& tpRemoveIndex : m_dqRemoveOctreeIndexGroup)
	{
		m_spTreeManager->DeactivateGameObject(0, tpRemoveIndex);
	}
	m_dqRemoveOctreeIndexGroup.clear();
}
