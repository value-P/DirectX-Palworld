#pragma once
#include "Weapon.h"

namespace Engine
{
	class CLightManager;
}

namespace Client
{
	class CEffectManager;
	class CEffectGroup;

	class CTorch : public CWeapon
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTorch() = default;

		virtual ~CTorch() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTorch> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Getter/Setter
	********************/
	public:


	/********************
		Methods
	********************/
	public:
		virtual void SetActive(bool _bActive) override;

	/********************
		Data Members
	********************/
	protected:
		// 이펙트 매니저
		std::weak_ptr<CEffectManager> m_wpEffectManager;

		// 라이트 매니저
		std::weak_ptr<CLightManager> m_wpLightManager;

	protected:
		// 횃불 이펙트
		std::shared_ptr<CEffectGroup> m_spFireEffect = nullptr;

		// 횃불 점 조명
		std::shared_ptr<FPointLight> m_spPointLight = nullptr;
	};
}
