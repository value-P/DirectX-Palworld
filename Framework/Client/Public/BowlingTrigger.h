#pragma once
#include "GameObject.h"

namespace Engine
{
	class CBoxComponent;
}

namespace Client
{
	struct FBowlingTriggerDesc : public CGameObject::FGameObjectDesc
	{
		Vector3 m_vExtents = Vector3(1.f, 1.f, 1.f);
	};

	class CBowlingTrigger : public CGameObject
	{
		/********************
	Ctor/Dtor
********************/
	public:
		explicit CBowlingTrigger() = default;

		virtual ~CBowlingTrigger() = default;

		/********************
			Events
		********************/
	public:
		static shared_ptr<CBowlingTrigger> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT BeginPlay() override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Getter/Setter
	********************/

	/********************
		Methods
	********************/
	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CBoxComponent> m_spBoxTrigger = nullptr;
	};
}