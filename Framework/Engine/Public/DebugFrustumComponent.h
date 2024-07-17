#pragma once
#include "DebugColliderComponent.h"

namespace Engine
{
	/********************
		Description
	********************/
	struct ENGINE_DLL FDebugFrustumComponentDesc : public FDebugColliderComponentDesc
	{
		float	m_fRightSlope	= 1.0f;
		float	m_fLeftSlope	= -1.0f;
		float	m_fTopSlope		= 1.0f;
		float	m_fBottomSlope	= -1.0f;
		float	m_fNearPlane	= 0.1f;
		float	m_fFarPlane		= 1.0f;
		Vector3 m_vCenter		= Vector3(0.0f, 0.0f, 0.0f);
		Vector3 m_vRadians		= Vector3(0.0f, 0.0f, 0.0f);
	};

	class ENGINE_DLL CDebugFrustumComponent : public CDebugColliderComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDebugFrustumComponent() = default;

		virtual ~CDebugFrustumComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDebugFrustumComponent> Create(const std::shared_ptr<FDebugFrustumComponentDesc>& _spDebugFrustumComponentDesc);

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
		void SetRadians(const Vector3& _vRadians) { m_vRadians = _vRadians; }

		const Vector3& GetRadians() const { return m_vRadians; }

	/********************
		Data Members
	********************/
	private:
		float		m_fRightSlope	= 1.0f;
		float		m_fLeftSlope	= -1.0f;
		float		m_fTopSlope		= 1.0f;
		float		m_fBottomSlope	= -1.0f;
		float		m_fNearPlane	= 0.0f;
		float		m_fFarPlane		= 1.0f;
		Vector3		m_vCenter		= Vector3(0.0f, 0.0f, 0.0f);
		Vector3		m_vRadians		= Vector3(0.0f, 0.0f, 0.0f);

	private:
		// DX Frustum
		BoundingFrustum m_tDxLocalFrustum{};

		BoundingFrustum m_tDxWorldFrustum{};
	};
}

