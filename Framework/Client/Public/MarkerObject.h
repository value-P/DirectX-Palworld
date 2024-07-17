#pragma once
#include "GameObject.h"

namespace Engine
{
	class CPanel;
	class CAnimationImageComponent;
	class CBoxComponent;
}

namespace Client
{
	class CUIManager;

	enum class EMapNameType
	{
		WIND_HILL,
		TOWER,
		ENUM_END
	};

	struct FMarkerObjectDesc : public CGameObject::FGameObjectDesc
	{
		EMapNameType m_eMapNameType = EMapNameType::ENUM_END;
	};

	class CMarkerObject : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CMarkerObject() = default;

		virtual ~CMarkerObject() = default;
		
	/********************
		Events
	********************/
	public:
		static shared_ptr<CMarkerObject> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT BeginPlay() override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Getter/Setter
	********************/
	public:
		virtual void SetPosition(const Vector3& _vPosition) override;

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
		EMapNameType m_eMapNameType = EMapNameType::ENUM_END;
		// 맵 이름 패널을 저장합니다.
		std::shared_ptr<CPanel> m_spMapNamePanel = nullptr;
		// 맵 이름 애니메이션 컴포넌트를 저장합니다.
		std::shared_ptr<CAnimationImageComponent> m_spNameAnimation = nullptr;
		std::shared_ptr<CBoxComponent> m_spBoxTrigger = nullptr;

		std::wstring m_wstrWindTextureName = L"T_Wind_NameImage0";
		std::wstring m_wstrTowerTextureName = L"T_RainTower_NameImage0";
	private:
		std::weak_ptr<CUIManager> m_wpUIManager;
	};
}