#pragma once
#include "GameObject.h"

namespace Engine
{
	class CCapsuleComponent;
}

namespace Client
{
    struct FSampleCapsuleCollisionObjectDesc : public CGameObject::FGameObjectDesc
    {
        EMobilityType m_eMobilityType = EMobilityType::ENUM_END;

        ECollisionLayer m_eCollisionLayer = ECollisionLayer::ENUM_END;

        ECollisionFlag m_eCollisionFlag = ECollisionFlag::ENUM_END;

        bool m_bGravity = true;

        float m_fHalfHeight = 1.0f;

        float m_fRadius = 1.0f;

		Vector3 m_vCenter = Vector3(0.0f, 0.0f, 0.0f);

        Vector3 m_vLocalRotate = Vector3(0.0f, 0.0f, 0.0f);
    };

    class CSampleCapsuleCollisionObject : public CGameObject
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CSampleCapsuleCollisionObject() = default;

		virtual ~CSampleCapsuleCollisionObject() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CSampleCapsuleCollisionObject> Create(const std::shared_ptr<FSampleCapsuleCollisionObjectDesc>& _spSampleCapsuleCollisionObjectDesc);

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
		std::shared_ptr<CCapsuleComponent> m_spCapsuleComponent = nullptr;
    };
}
