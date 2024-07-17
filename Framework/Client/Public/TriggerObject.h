#pragma once
#include "GameObject.h"

namespace Engine
{
	class CSphereComponent;
}

namespace Client
{
	struct FTriggerObjectDesc : public CGameObject::FGameObjectDesc
	{
	};

	class CTriggerObject : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTriggerObject() = default;

		virtual ~CTriggerObject() = default;
		
	/********************
		Events
	********************/
	public:
		static shared_ptr<CTriggerObject> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT BeginPlay() override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Getter/Setter
	********************/
	public:
		virtual void SetPosition(const Vector3& _vPosition) override;

	/********************
		Methods
	********************/
	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	public:
		void ActivateTrigger(shared_ptr<CGameObject> _spClientObject, float _fDisableTime, float _fRadius, ECollisionFlag _eCollisionFlag, ECollisionLayer _eCollisionLayer);

	/********************
		Data Members
	********************/
	protected:
		std::shared_ptr<CSphereComponent> m_spSphereTrigger = nullptr;

		std::weak_ptr<CGameObject> m_wpNotifyClient; // �̺�Ʈ�� ��û�� �Ƿ���(��ü) : ���������� OnTrigger �̺�Ʈ�� �޾ƾ��� ��ü

		float m_fDisableTimeChecker = 0.0f;

		float m_fDisableTime = 0.0f;
	};
}
