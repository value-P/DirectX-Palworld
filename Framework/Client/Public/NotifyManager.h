#pragma once
#include "Manager.h"

namespace Engine
{
	class CLevel;
	class CGameObject;
}

namespace Client
{
	class CTriggerObject;

	class CNotifyManager : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CNotifyManager)

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();
		HRESULT BeginPlay();
		void Tick();
		virtual void Release() override;
				
	private:
		void InitTriggerObjects();
		void AddTriggerObjects(int32 _iAddCount);

	/********************
		Methods
	********************/
	public:
		void NotifyCollison(const shared_ptr<CGameObject>& _spNotifyClient, const Vector3& _vWorldPosition, float _fRadius, float _fEndTime, ECollisionFlag _eCollisionFlag, ECollisionLayer _eCollisionLayer);

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CLevel> m_wpPersistentLevel;

		shared_ptr<struct FTriggerObjectDesc> m_spTriggerObjectDesc = nullptr;
		list<shared_ptr<CTriggerObject>> m_lstTriggerObject;
		list<shared_ptr<CTriggerObject>> m_lstUsingTriggerObject;
	};
}