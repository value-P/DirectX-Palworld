#pragma once
#include "ColliderComponent.h"

namespace Engine
{
	/********************
		Description
	********************/
	struct ENGINE_DLL FCapsuleComponentDesc : public FColliderComponentDesc
	{
		float m_fRadius = 1.0f;

		float m_fHalfHeight = 1.0f;
	};

	class ENGINE_DLL CCapsuleComponent : public CColliderComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CCapsuleComponent() = default;

		virtual ~CCapsuleComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CCapsuleComponent> Create(const std::shared_ptr<FCapsuleComponentDesc>& _spCapsuleComponentDesc);

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

		void SetHalfHeight(float _fHalfHeight);
		
		float GetRadius() const { return m_fRadius; }

		float GetHalfHeight() const { return m_fHalfHeight; }

		virtual void SetLocalPosition(const Vector3& _vLocalPosition) override;

	/********************
		Data Members
	********************/
	private:
		PxShape* m_pCapsuleShape = nullptr;

		float m_fRadius = 1.0f;

		float m_fHalfHeight = 1.0f;

	private:
		// DX Sphere
		BoundingSphere m_tDxLocalSphere[2]{};

		BoundingSphere m_tDxWorldSphere[2]{};

		// DX OBB ¹Ú½º
		BoundingOrientedBox m_tDxLocalOBB{};

		BoundingOrientedBox m_tDxWorldOBB{};
	};
}
