#include "pch.h"
#include "Layer.h"

#include "GameObject.h"

shared_ptr<CLayer> Engine::CLayer::Create(const std::wstring& _wstrLayerName)
{
	const shared_ptr<CLayer> spLayer = make_shared<CLayer>();
	ENSUREF(spLayer->Initialize(_wstrLayerName), L"Failed to initialize layer.");
	return spLayer;
}

HRESULT Engine::CLayer::Initialize(const std::wstring& _wstrLayerName)
{
	m_wstrLayerName = _wstrLayerName;
	return S_OK;
}

void Engine::CLayer::Release()
{
	// 게임 오브젝트의 릴리즈 함수는 레벨의 릴리즈 함수에서 호출하므로, 레이어 안에 있는 게임 오브젝트는 널포인터로만 초기화합니다.
	for (auto& pair : m_umapGameObjects)
	{
		pair.second = nullptr;
	}
	m_umapGameObjects.clear();
}

HRESULT Engine::CLayer::AddGameObjectToLayer(const std::wstring& _wstrGameObjectName, const std::shared_ptr<CGameObject>& _spGameObject)
{
	if (m_umapGameObjects.contains(_wstrGameObjectName)) { return E_FAIL; }

	m_umapGameObjects.emplace(_wstrGameObjectName, _spGameObject);

	m_umapGameObjects[_wstrGameObjectName]->SetLayer(shared_from_this());
	return S_OK;
}

HRESULT Engine::CLayer::RemoveGameObjectFromLayer(const std::wstring& _wstrGameObjectName)
{
	if (!m_umapGameObjects.contains(_wstrGameObjectName)) { return E_FAIL; }

	m_umapGameObjects[_wstrGameObjectName]->SetLayer(nullptr);
	m_umapGameObjects[_wstrGameObjectName] = nullptr;

	m_umapGameObjects.erase(_wstrGameObjectName);
	return S_OK;
}

std::shared_ptr<CGameObject> Engine::CLayer::FindGameObjectFromLayer(const std::wstring& _wstrGameObjectName)
{
	if (!m_umapGameObjects.contains(_wstrGameObjectName)) { return nullptr; }

	return m_umapGameObjects[_wstrGameObjectName];
}
