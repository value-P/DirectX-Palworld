#include "pch.h"
#include "NotifyManager.h"

#include "ClientModule.h"
#include "GameManager.h"
#include "Level.h"

#include "World.h"
#include "PersistentLevel.h"

#include "TriggerObject.h"

IMPLEMENT_SINGLETON(CNotifyManager)

HRESULT Client::CNotifyManager::Initialize()
{
	m_wpPersistentLevel = CWorld::GetInstance()->GetPersistentLevel();
	InitTriggerObjects();

	return S_OK;
}

HRESULT Client::CNotifyManager::BeginPlay()
{
	return S_OK;
}

void Client::CNotifyManager::Tick()
{
	for (auto usingTrigger = m_lstUsingTriggerObject.begin(); usingTrigger != m_lstUsingTriggerObject.end();)
	{
		if (!(*usingTrigger)->IsActive())
		{
			m_lstTriggerObject.push_back(*usingTrigger);
			usingTrigger = m_lstUsingTriggerObject.erase(usingTrigger);
		}
		else
		{
			++usingTrigger;
		}
	}
}

void Client::CNotifyManager::Release()
{
	for (auto& spTriggerObj: m_lstTriggerObject)
	{
		spTriggerObj = nullptr;
	}
	m_lstTriggerObject.clear();

	for (auto& spUsingTriggerObj: m_lstUsingTriggerObject)
	{
		spUsingTriggerObj = nullptr;
	}
	m_lstUsingTriggerObject.clear();

	m_spInstance = nullptr;

	m_spTriggerObjectDesc = nullptr;
}

void Client::CNotifyManager::InitTriggerObjects()
{
	FTriggerObjectDesc tObjDesc;
	tObjDesc.m_bActive = false;
	tObjDesc.m_eMobilityType = EMobilityType::DYNAMIC;
	m_spTriggerObjectDesc = make_shared<FTriggerObjectDesc>(tObjDesc);

	AddTriggerObjects(50);
}

void Client::CNotifyManager::AddTriggerObjects(int32 _iAddCount)
{
	for (int32 i = 0; i < _iAddCount; ++i)
	{
		shared_ptr<CTriggerObject> spTriggerObj = CTriggerObject::Create(m_spTriggerObjectDesc);
		m_wpPersistentLevel.lock()->AddGameObject(spTriggerObj);
		m_lstTriggerObject.push_back(spTriggerObj);
	}
}

void Client::CNotifyManager::NotifyCollison(const shared_ptr<CGameObject>& _spNotifyClient, const Vector3& _vWorldPosition, float _fRadius, float _fEndTime, ECollisionFlag _eCollisionFlag, ECollisionLayer _eCollisionLayer)
{
	if (m_lstTriggerObject.empty()) { AddTriggerObjects(5); }

	shared_ptr<CTriggerObject> spTriggerObj = m_lstTriggerObject.front();
	m_lstTriggerObject.pop_front();

	spTriggerObj->SetPosition(_vWorldPosition);
	spTriggerObj->ActivateTrigger(_spNotifyClient, _fEndTime, _fRadius, _eCollisionFlag, _eCollisionLayer);
	spTriggerObj->SetActive(true);

	m_lstUsingTriggerObject.push_back(spTriggerObj);
}