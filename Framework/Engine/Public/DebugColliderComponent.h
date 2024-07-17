#pragma once
#include "Component.h"

namespace Engine
{
	class CRenderManager;
	class CCameraManager;

	struct ENGINE_DLL FDebugColliderComponentDesc : public CComponent::FComponentDesc
	{
	public:
		// 콜라이더 모양
		EColliderType m_eColliderType = EColliderType::ENUM_END;

		// 콜리전 레이어
		ECollisionLayer m_eCollisionLayer = ECollisionLayer::ENUM_END;

		// 콜리전 플래그
		ECollisionFlag m_eCollisionFlag = ECollisionFlag::ENUM_END;

		Vector3 m_vCenter = Vector3::Zero;
	};

	class ENGINE_DLL CDebugColliderComponent : public CComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDebugColliderComponent() = default;

		virtual ~CDebugColliderComponent() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual HRESULT BindVariable() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

	/********************
		Getter/Setter
	********************/
	public:
		ECollisionLayer GetCollisionLayer() const { return m_eCollisionLayer; }

		void SetCenter(const Vector3& _vCenter) { m_vCenter = _vCenter; }

		const Vector3& GetCenter() const { return m_vCenter; }

	/********************
		Data Members
	********************/
	protected:
		EColliderType m_eColliderType = EColliderType::ENUM_END;

		ECollisionLayer m_eCollisionLayer = ECollisionLayer::ENUM_END;

		ECollisionFlag m_eCollisionFlag = ECollisionFlag::ENUM_END;

		Vector3 m_vCenter = Vector3::Zero;

	protected:
		std::weak_ptr<CRenderManager> m_wpRenderManager;

		std::weak_ptr<CCameraManager> m_wpCameraManager;

	protected:
		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		std::shared_ptr<PrimitiveBatch<VertexPositionColor>> m_spPrimitiveBatch = nullptr;

		std::shared_ptr<BasicEffect> m_spBasicEffect = nullptr;

		ComPtr<ID3D11InputLayout> m_cpInputLayout = nullptr;

	};
}


