#pragma once
#include "Component.h"

namespace Engine
{
	class CLightManager;

    class ENGINE_DLL CLightComponent : public CComponent
    {
	/********************
		Description
	********************/
	public:
		struct ENGINE_DLL FLightComponentDesc : public FComponentDesc
		{
			ELightType m_eLightType = ELightType::ENUM_END;

			Vector3 m_vAmbient = Vector3::One;

			Vector3 m_vDiffuse = Vector3::One;

			Vector3 m_vPosition = Vector3::Zero;

			Vector3 m_vDirection = Vector3::Down;

			float m_fFallOffBegin = 0.0f;

			float m_fFallOffEnd = 10.0f;

			float m_fRange = 10.0f;

			float m_fPower = 1.0f;
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CLightComponent() = default;

		virtual ~CLightComponent() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CLightComponent> Create(const std::shared_ptr<FLightComponentDesc>& _spLightComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render() override;

		virtual void Release() override;

		virtual HRESULT BindVariable() override;

	public:
		bool GetActive() { return m_bActive; }
		void SetActive(bool _bActive) { m_bActive = _bActive; }

	/********************
		Data Members
	********************/
	private:
		// 빛 타입
		ELightType m_eLightType = ELightType::ENUM_END;

		// 앰비언트
		Vector3 m_vAmbient = Vector3::One;

		// 디퓨즈
		Vector3 m_vDiffuse = Vector3::One;

		// 빛의 위치
		Vector3 m_vPosition = Vector3::Zero;

		// 빛의 방향
		Vector3 m_vDirection = Vector3::Down;

		// 감쇠 시작 거리
		float m_fFallOffBegin = 0.0f;

		// 감쇠 끝 거리
		float m_fFallOffEnd = 10.0f;

		// 빛의 범위
		float m_fRange = 10.0f;

		// 빛의 세기
		float m_fPower = 1.0f;

		// 활성화 여부
		bool m_bActive = false;

	private:
		std::weak_ptr<CLightManager> m_wpLightManager;
    };
}
