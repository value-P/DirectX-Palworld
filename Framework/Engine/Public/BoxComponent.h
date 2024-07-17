#pragma once
#include "ColliderComponent.h"

namespace Engine
{	
	struct ENGINE_DLL FBoxComponentDesc : public FColliderComponentDesc
	{
		Vector3 m_vExtents = Vector3(1.0f, 1.0f, 1.0f);
	};

	class ENGINE_DLL CBoxComponent : public CColliderComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CBoxComponent() = default;

		virtual ~CBoxComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CBoxComponent> Create(const std::shared_ptr<FBoxComponentDesc>& _spBoxComponentDesc);

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
		void SetExtents(const Vector3& _vExtents);

		const Vector3& GetExtents() const { return m_vExtents; }

		virtual Vector3 GetLocalPosition() override;

		virtual void SetLocalPosition(const Vector3& _vLocalPosition) override;

		virtual void SetCollisionLayer(ECollisionLayer _eCollisionLayer, ECollisionFlag _eCollisionFlag) override;

	/********************
		Data Members
	********************/
	private:
		PxShape* m_pBoxShape = nullptr;

		Vector3 m_vExtents = Vector3::One;

	private:
		// DX OBB ¹Ú½º
		BoundingOrientedBox m_tDxLocalOBB{};

		BoundingOrientedBox m_tDxWorldOBB{};
	};
}
