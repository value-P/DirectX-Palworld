#pragma once
#include "ColliderComponent.h"

namespace Engine
{
	/********************
		Description
	********************/
	struct ENGINE_DLL FSphereComponentDesc : public FColliderComponentDesc
	{
		float m_fRadius = 1.0f;
	};

	class ENGINE_DLL CSphereComponent : public CColliderComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CSphereComponent() = default;

		virtual ~CSphereComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CSphereComponent> Create(const std::shared_ptr<FSphereComponentDesc>& _spSphereComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

		virtual HRESULT BindVariable() override;
		
	/********************
		Methods
	********************/
	public:
		virtual HRESULT RemoveRigidActorFromScene() override;

		virtual void ApplySimulation() override;

	public:
		virtual void ApplySimulationDirectly(bool _bSimulation) override;

	/********************
		Getter/Setter
	********************/
	public:
		void SetRadius(float _fRadius);

		float GetRadius() const { return m_fRadius; }

		virtual Vector3 GetLocalPosition() override;

		virtual void SetLocalPosition(const Vector3& _vLocalPosition) override;

		virtual void SetCollisionLayer(ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag) override;

	/********************
		Data Members
	********************/
	private:
		PxShape* m_pSphereShape = nullptr;

		float m_fRadius = 1.0f;

	private:
		// DX Sphere
		BoundingSphere m_tDxLocalSphere{};

		BoundingSphere m_tDxWorldSphere{};
	};
}
