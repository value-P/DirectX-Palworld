#pragma once
#include "GameObject.h"

namespace Engine
{
    class CBoxComponent;
}

namespace Client
{
	struct FSampleBoxTriggerObjectDesc : public CGameObject::FGameObjectDesc
	{
		EMobilityType m_eMobilityType = EMobilityType::ENUM_END;

		ECollisionLayer m_eCollisionLayer = ECollisionLayer::ENUM_END;

		ECollisionFlag m_eCollisionFlag = ECollisionFlag::ENUM_END;

		bool m_bTrigger = true;

		Vector3 m_vExtents = Vector3(1.0f, 1.0f, 1.0f);
	};

    class CSampleBoxTriggerObject : public CGameObject
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CSampleBoxTriggerObject() = default;

		virtual ~CSampleBoxTriggerObject() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CSampleBoxTriggerObject> Create(const std::shared_ptr<FSampleBoxTriggerObjectDesc>& _spSampleBoxTriggerObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

	public:
		virtual HRESULT BeginPlay();

		virtual int32 PreTick(float _fDeltaSeconds);

		virtual int32 Tick(float _fDeltaSeconds);
		
		virtual int32 PostTick(float _fDeltaSeconds);

		virtual HRESULT EndPlay();

		virtual void Release();
		
	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CBoxComponent> m_spBoxComponent = nullptr;
    };
}
