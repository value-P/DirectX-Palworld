#include "pch.h"
#include "TreeNode.h"

#include "EngineModule.h"
#include "CameraManager.h"
#include "DeviceManager.h"
#include "RenderManager.h"
#include "ShaderHandler.h"
#include "World.h"
#include "GameObject.h"

HRESULT Engine::CTreeNode::Initialize(int32 _iTreeIndex, uint32 _iNodeDepth, const Vector3& _vCenter, const Vector3& _vExtents)
{
	shared_ptr<CEngineModule> spEngineModule = CEngineModule::GetInstance();
	m_cpDeviceContext = spEngineModule->GetDeviceManager()->GetDeviceContext();
	m_wpRenderManager = spEngineModule->GetRenderManager();
	m_wpCameraManager = spEngineModule->GetCameraManager();

	// 섬 레벨
	m_wpCurLevel = CWorld::GetInstance()->GetPostFrontLevel();

	// 셰이더 핸들러
	shared_ptr<CShaderHandler> spShaderHandler = CShaderHandler::GetInstance();
	m_cpInputLayout = spShaderHandler->GetDebugDrawInputLayout();
	m_spPrimitiveBatch = spShaderHandler->GetPrimitiveBatch();
	m_spBasicEffect = spShaderHandler->GetBasicEffect();
	return S_OK;
}

void Engine::CTreeNode::Tick(float _fDeltaSeconds)
{
	scoped_lock<mutex> slLock(m_mtMutex);

	// 절두체 안에 있는 오브젝트만 켭니다.
	CullGameObject(m_wpCameraManager.lock()->GetCameraFrustum(), m_wpCameraManager.lock()->GetCameraCullSphere());
}

void Engine::CTreeNode::Render()
{
#ifdef _DEBUG
	m_cpDeviceContext->GSSetShader(nullptr, nullptr, 0);

	m_spBasicEffect->SetWorld(XMMatrixIdentity());

	m_spBasicEffect->SetView(m_wpCameraManager.lock()->GetCurViewMatrix());

	m_spBasicEffect->SetProjection(m_wpCameraManager.lock()->GetCurPerspectiveProjectionMatrix());

	m_spBasicEffect->Apply(m_cpDeviceContext.Get());

	m_cpDeviceContext->IASetInputLayout(m_cpInputLayout.Get());

	m_spPrimitiveBatch->Begin();

	DX::Draw(m_spPrimitiveBatch.get(), m_tDxAABB, m_bCollided ? Colors::Red : Colors::Black);

	m_spPrimitiveBatch->End();
#endif // _DEBUG
}

void Engine::CTreeNode::Release()
{
	for (auto& spChildNode : m_vecChildNodes)
	{
		spChildNode->Release();
		spChildNode = nullptr;
	}
	m_vecChildNodes.clear();

	for (auto& pair : m_umapStaticGameObjects)
	{
		pair.second = nullptr;
	}
	m_umapStaticGameObjects.clear();

	for (auto& pair : m_umapDynamicGameObjects)
	{
		pair.second = nullptr;
	}
	m_umapDynamicGameObjects.clear();

	m_spPrimitiveBatch = nullptr;
	m_spBasicEffect = nullptr;
	m_cpInputLayout = nullptr;
}

void Engine::CTreeNode::CullGameObject(const BoundingFrustum& _tFrustum, const BoundingSphere& _tSphere)
{
	// m_wpRenderManager.lock()->AddTreeNode(shared_from_this());

	// 정적 오브젝트를 끕니다.
	for (const auto& pair : m_umapStaticGameObjects)
	{
		pair.second->SetCulled(true);
	}

	// 동적 오브젝트를 끕니다.
	for (const auto& pair : m_umapDynamicGameObjects)
	{
		pair.second->SetCulled(true);
	}

	ContainmentType eContainmentType = _tFrustum.Contains(m_tDxAABB);

	switch (eContainmentType)
	{
	case DirectX::DISJOINT:
		m_bCollided = false;
		return;

	case DirectX::INTERSECTS:
	{
		m_bCollided = true;

		if (m_vecChildNodes.empty())
		{
			for (const auto& pair : m_umapStaticGameObjects)
			{
				// 절두체 안에 있는 게임 오브젝트만 렌더합니다.
				if (_tFrustum.Contains(pair.second->GetPosition().value()) || _tSphere.Contains(pair.second->GetPosition().value()))
				{
					pair.second->SetCulled(false);
				}
			}
			return;
		}

		for (const auto& spChildNode : m_vecChildNodes)
		{
			spChildNode->CullGameObject(_tFrustum, _tSphere);
		}
	}
	break;

	case DirectX::CONTAINS:
	{
		m_bCollided = true;

		for (auto& pair : m_umapStaticGameObjects)
		{
			// 절두체 안에 있는 게임 오브젝트만 렌더합니다.
			if (_tFrustum.Contains(pair.second->GetPosition().value()) || _tSphere.Contains(pair.second->GetPosition().value()))
			{
				pair.second->SetCulled(false);
			}
		}
	}
	break;

	default:
		CHECKF(false, L"Containment type is not designated.");
		break;
	}
}

HRESULT Engine::CTreeNode::AddGameObjectToNode(const std::shared_ptr<CGameObject>& _spGameObject)
{
	scoped_lock<mutex> slLock(m_mtMutex);

	int32 iGameObjectID = _spGameObject->GetGameObjectID();

	switch (_spGameObject->GetMobilityType())
	{
	case EMobilityType::STATIC:
	{
		if (m_umapStaticGameObjects.contains(iGameObjectID)) { return E_FAIL; }

		m_umapStaticGameObjects.emplace(iGameObjectID, _spGameObject);
	}
	break;

	case EMobilityType::DYNAMIC:
	{
		if (m_umapDynamicGameObjects.contains(iGameObjectID)) { return E_FAIL; }

		m_umapDynamicGameObjects.emplace(iGameObjectID, _spGameObject);
	}
	break;

	default:
		CHECKF(false, L"Mobility type is not designated.");
		break;
	}

	return S_OK;
}

HRESULT Engine::CTreeNode::RemoveGameObjectFromNodeByID(EMobilityType _eMobilityType, int32 _iGameObjectID)
{
	scoped_lock<mutex> slLock(m_mtMutex);

	switch (_eMobilityType)
	{
	case EMobilityType::STATIC:
	{
		if (!m_umapStaticGameObjects.contains(_iGameObjectID)) { return E_FAIL; }

		m_umapStaticGameObjects.erase(_iGameObjectID);
	}
	break;

	case EMobilityType::DYNAMIC:
	{
		if (!m_umapDynamicGameObjects.contains(_iGameObjectID)) { return E_FAIL; }

		m_umapDynamicGameObjects.erase(_iGameObjectID);
	}
	break;

	default:
		CHECKF(false, L"Mobility type is not designated.");
		break;
	}

	return S_OK;
}

std::shared_ptr<CGameObject> Engine::CTreeNode::FindGameObjectFromNode(EMobilityType _eMobilityType, int32 _iGameObjectID)
{
	scoped_lock<mutex> slLock(m_mtMutex);

	switch (_eMobilityType)
	{
	case EMobilityType::STATIC:
	{
		if (!m_umapStaticGameObjects.contains(_iGameObjectID)) { return nullptr; }

		return m_umapStaticGameObjects[_iGameObjectID];
	}
	break;

	case EMobilityType::DYNAMIC:
	{
		if (!m_umapDynamicGameObjects.contains(_iGameObjectID)) { return nullptr; }

		return m_umapDynamicGameObjects[_iGameObjectID];
	}
	break;

	default:
		CHECKF(false, L"Mobility type is not designated.");
		break;
	}

	return nullptr;
}
