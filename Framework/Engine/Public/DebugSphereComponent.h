#pragma once
#include "DebugColliderComponent.h"

namespace Engine
{
	struct ENGINE_DLL FDebugSphereComponentDesc : public FDebugColliderComponentDesc
	{
		float m_fRadius = 1.0f;
	};

	class ENGINE_DLL CDebugSphereComponent : public CDebugColliderComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDebugSphereComponent() = default;

		virtual ~CDebugSphereComponent() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CDebugSphereComponent> Create(const std::shared_ptr<FDebugSphereComponentDesc>& _spDebugSphereComponentDesc);

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
			Getter/Setter
		********************/
	public:
		void SetRadius(float _fRadius) { m_fRadius = _fRadius; }

		float GetRadius() const { return m_fRadius; }

		/********************
			Data Members
		********************/
	private:
		float m_fRadius = 1.0f;

	private:
		// DX Sphere
		BoundingSphere m_tDxLocalSphere{};

		BoundingSphere m_tDxWorldSphere{};
	};
}


