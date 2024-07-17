// default
#include "pch.h"
#include "Level.h"

#include "World.h"
#include "Layer.h"
#include "GameObject.h"
#include "ColliderComponent.h"

#include "EngineModule.h"
#include "RenderManager.h"

HRESULT Engine::CLevel::Initialize()
{
	m_wpOwnerWorld = CWorld::GetInstance();

	return S_OK;
}

HRESULT Engine::CLevel::BeginPlay()
{
	DeferredAddGameObject();

	for (const auto& spGameObject : m_vecGameObjects)
	{
		if (FAILED(spGameObject->BeginPlay())) { return E_FAIL; }
	}

	CEngineModule::GetInstance()->GetRenderManager()->SetActive(true);

	return S_OK;
}

int32 Engine::CLevel::PreTick(float _fDeltaSeconds)
{
	DeferredAddGameObject();

	//std::scoped_lock<mutex> slLock(m_mtMutex);

	for (const auto& spGameObject : m_vecGameObjects)
	{
		if (!spGameObject->IsActive()) { continue; }

		switch (spGameObject->PreTick(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	return 0;
}

int32 Engine::CLevel::Tick(float _fDeltaSeconds)
{
	//std::scoped_lock<mutex> slLock(m_mtMutex);

	for (const auto& spGameObject : m_vecGameObjects)
	{
		if (!spGameObject->IsActive()) { continue; }

		switch (spGameObject->Tick(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	return 0;
}

int32 Engine::CLevel::PostTick(float _fDeltaSeconds)
{
	//std::scoped_lock<mutex> slLock(m_mtMutex);

	for (const auto& spGameObject : m_vecGameObjects)
	{
		if (!spGameObject->IsActive()) { continue; }

		switch (spGameObject->PostTick(_fDeltaSeconds))
		{
		case -1:	return -1;
		default:	break;
		}
	}

	return 0;
}

HRESULT Engine::CLevel::EndPlay()
{
	for (const auto& spGameObject : m_vecGameObjects)
	{
		if (FAILED(spGameObject->EndPlay())) { return E_FAIL; }
	}

	return S_OK;
}

void Engine::CLevel::Release()
{
	for (auto& pair : m_umapLayers)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapLayers.clear();

	for (auto& spColliderComponent : m_vecDeferredSimulationColliderComponents)
	{
		spColliderComponent = nullptr;
	}
	m_vecDeferredSimulationColliderComponents.clear();

	for (auto& spColliderComponent : m_vecDeferredTransformColliderComponents)
	{
		spColliderComponent = nullptr;
	}
	m_vecDeferredTransformColliderComponents.clear();

	for (auto& spGameObject : m_vecGameObjects)
	{
		spGameObject->Release();
		spGameObject = nullptr;
	}
	m_vecGameObjects.clear();

	for (auto& spDeferredGameObject : m_vecDeferredAddGameObjects)
	{
		spDeferredGameObject->Release();
		spDeferredGameObject = nullptr;
	}
	m_vecDeferredAddGameObjects.clear();
}

void Engine::CLevel::Clear()
{
	DeferredSimulationColliderComponents();

	DeferredTransformColliderComponents();

	DeferredRemoveGameObject();
}

void Engine::CLevel::LoadResource(int32 _iLevelType)
{
	return;
}

void Engine::CLevel::AddGameObject(const std::shared_ptr<CGameObject>& _spGameObject)
{
	// TODO: 멀티스레드를 사용할 경우, 경쟁 상태를 처리해야 합니다.

	_spGameObject->SetOwnerLevel(shared_from_this());
	_spGameObject->SetOwnerWorld(m_wpOwnerWorld.lock());

	switch (m_eLevelState.load())
	{
	case ELevelState::INITIALIZING:
		m_vecGameObjects.push_back(_spGameObject);
		break;

	default:
		m_vecDeferredAddGameObjects.push_back(_spGameObject);
		break;
	}
}

void Engine::CLevel::RemoveGameObject(const std::shared_ptr<CGameObject>& _spGameObject)
{
	std::scoped_lock slLock(m_mtMutex);

	m_vecDeferredRemoveGameObjects.push_back(_spGameObject);

	/*
	// 게임 오브젝트의 Release 함수에서 매니저를 정리해야 합니다.
	int32 iGameObjectID = _spGameObject->GetGameObjectID();

	for (auto iter = m_vecGameObjects.begin(); iter != m_vecGameObjects.end(); ++iter)
	{
		if ((*iter)->GetGameObjectID() == iGameObjectID)
		{
			(*iter)->Release();
			m_vecGameObjects.erase(iter);
			break;
		}
	}
	*/
}

std::shared_ptr<CGameObject> Engine::CLevel::FindGameObjectByName(const std::wstring& _wstrGameObjectName)
{
	if (_wstrGameObjectName.empty()) { return nullptr; }

	for (const auto& spGameObject : m_vecGameObjects)
	{
		if (spGameObject->GetGameObjectName() == _wstrGameObjectName)
		{
			return spGameObject;
		}
	}

	return nullptr;
}

std::shared_ptr<CGameObject> Engine::CLevel::FindGameObjectByID(uint32 _iGameObjectID)
{
	for (const auto& spGameObject : m_vecGameObjects)
	{
		if (spGameObject->GetGameObjectID() == _iGameObjectID)
		{
			return spGameObject;
		}
	}

	return nullptr;
}

void Engine::CLevel::RequestSimulationColliderComponent(const std::shared_ptr<CColliderComponent>& _spColliderComponent)
{
	scoped_lock<mutex> slLock(m_mtColliderComponentMutex);

	m_vecDeferredSimulationColliderComponents.push_back(_spColliderComponent);
}

void Engine::CLevel::RequestTransformColliderComponent(const std::shared_ptr<CColliderComponent>& _spColliderComponent)
{
	scoped_lock<mutex> slLock(m_mtColliderComponentMutex);

	m_vecDeferredTransformColliderComponents.push_back(_spColliderComponent);
}

void Engine::CLevel::DeferredAddGameObject()
{
	std::scoped_lock<mutex> slLock(m_mtMutex);

	if (!m_vecDeferredAddGameObjects.empty())
	{
		for (const auto& spGameObject : m_vecDeferredAddGameObjects)
		{
   			ENSUREF(spGameObject->BeginPlay(), L"Failed to beginplay gameobject.");
		}

		m_vecGameObjects.insert(m_vecGameObjects.end(), m_vecDeferredAddGameObjects.begin(), m_vecDeferredAddGameObjects.end());
		m_vecDeferredAddGameObjects.clear();
	}
}

/*
HRESULT Engine::CLevel::RemoveGameObjectByID(int32 _iGameObjectID)
{
	std::scoped_lock slLock(m_mtMutex);

	for (auto iter = m_vecGameObjects.begin(); iter != m_vecGameObjects.end(); ++iter)
	{
		if ((*iter)->GetGameObjectID() == _iGameObjectID)
		{
			(*iter)->Release();
			m_vecGameObjects.erase(iter);
			return S_OK;
		}
	}
	return S_OK;
}
*/

void Engine::CLevel::DeferredRemoveGameObject()
{
	std::scoped_lock<mutex> slLock(m_mtMutex);

	for (const auto& spGameObject : m_vecDeferredRemoveGameObjects)
	{
		int32 iGameObjectID = spGameObject->GetGameObjectID();

		bool bFound = false;
		for (auto iter = m_vecGameObjects.begin(); iter != m_vecGameObjects.end(); ++iter)
		{
			if ((*iter)->GetGameObjectID() == iGameObjectID)
			{
				(*iter)->Release();
				m_vecGameObjects.erase(iter);
				bFound = true;
				break;
			}
		}

		if (bFound == false)
		{
			CHECKF(false, L"Game object doesn't exist in this level.");
		}
	}

	for (auto& spGameObject : m_vecDeferredRemoveGameObjects)
	{
		spGameObject = nullptr;
	}
	m_vecDeferredRemoveGameObjects.clear();
}

void Engine::CLevel::DeferredSimulationColliderComponents()
{
	if (m_vecDeferredSimulationColliderComponents.empty()) { return; }

	for (auto& spColliderComponent : m_vecDeferredSimulationColliderComponents)
	{
		spColliderComponent->ApplySimulation();
	}
	m_vecDeferredSimulationColliderComponents.clear();
}

void Engine::CLevel::DeferredTransformColliderComponents()
{
	if (m_vecDeferredTransformColliderComponents.empty()) { return; }

	for (auto& spColliderComponent : m_vecDeferredTransformColliderComponents)
	{
		spColliderComponent->ApplyTransform();
	}
	m_vecDeferredTransformColliderComponents.clear();
}

HRESULT Engine::CLevel::AddGameObjectToLayer(const std::wstring& _wstrLayerName, const std::shared_ptr<CGameObject>& _spGameObject)
{
	std::scoped_lock<mutex> slLock(m_mtMutex);

	if (_wstrLayerName.empty()) { return S_OK; }

	wstring wstrGameObjectName = _spGameObject->GetGameObjectName();

	if (m_umapLayers.contains(_wstrLayerName))
	{
		if (FAILED(m_umapLayers[_wstrLayerName]->AddGameObjectToLayer(wstrGameObjectName, _spGameObject)))
		{
			return E_FAIL;
		}
	}
	else
	{
		std::shared_ptr<CLayer> spLayer = CLayer::Create(_wstrLayerName);
		m_umapLayers.emplace(_wstrLayerName, spLayer);

		if (FAILED(m_umapLayers[_wstrLayerName]->AddGameObjectToLayer(wstrGameObjectName, _spGameObject)))
		{
			spLayer->Release();
			spLayer = nullptr;

			CHECKF(false, L"Game Object with the same name already exists in the layer.");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT Engine::CLevel::RemoveGameObjectFromLayer(const std::wstring& _wstrLayerName, const std::wstring& _wstrGameObjectName)
{
	if (_wstrLayerName.empty() || _wstrGameObjectName.empty()) { return E_FAIL; }

	if (!m_umapLayers.contains(_wstrLayerName)) { return E_FAIL; }

	if (FAILED(m_umapLayers[_wstrLayerName]->RemoveGameObjectFromLayer(_wstrGameObjectName))) { return E_FAIL; }

	return S_OK;
}

std::shared_ptr<CLayer> Engine::CLevel::FindLayer(const std::wstring& _wstrLayerName)
{
	if (m_umapLayers.contains(_wstrLayerName)) { return m_umapLayers[_wstrLayerName]; }

	return nullptr;
}

std::shared_ptr<CWorld> Engine::CLevel::GetOwnerWorld() const
{
	return m_wpOwnerWorld.expired() ? nullptr : m_wpOwnerWorld.lock();
}