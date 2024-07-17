#pragma once
#include "GameObject.h"

namespace Engine
{
	class CSphereComponent;
}

namespace ToolAnimation
{
    class CSphereObject : public CGameObject
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CSphereObject() = default;

		virtual ~CSphereObject() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CSphereObject> Create(const std::shared_ptr<FGameObjectDesc>& _spPlayerDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		void SetRadius(float _fRadius);
		void SetLocalPos(Vector3 _vLocalPos);

		float GetRadius();
		Vector3 GetLocalPos();

	/********************
		Data Members
	********************/
	private:
		shared_ptr< CSphereComponent> m_spSphereComponent = nullptr;
    };
}


