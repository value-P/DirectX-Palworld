#pragma once
#include "Habitat.h"

namespace Engine
{
	class CPanel;
	class CAnimationImageComponent;
	class CSoundManager;
}

namespace Client
{
	class CUIManager;

	/********************
		Descriptor
	********************/
	struct FNatureDesc : public FHabitatDesc
	{
		ENatureType m_eNatureType = ENatureType::ENUM_END;
	};

	class CNature : public CHabitat
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CNature() = default;

		virtual ~CNature() = default;
		
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
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
		ENatureType GetNatureType() const { return m_eNatureType; }

		void SetCurHp(float _fCurHp) { m_fCurHp = _fCurHp; }

		float GetCurHp() const { return m_fCurHp; }

	/********************
		Methods
	********************/
	public:
		virtual void TakeDamage(const std::shared_ptr<CGameObject>& _spInstigator, float _fDamage);

		virtual void RespawnNature(float _fDeltaSeconds);

		virtual bool DestroyNature(float _fDeltaSeconds);

		void ShakeNature(float _fDeltaSeconds);

		bool IsDropItem();

		virtual float GetAcceptableRadius() { return 0.0f; }

	/********************
		Data Members
	********************/
	protected:
		ENatureType m_eNatureType = ENatureType::ENUM_END;

		// GetItem 패널의 부모를 들고 있게합니다.
		std::weak_ptr<CPanel>	m_wpGetItemParentPanel;

		std::weak_ptr<CAnimationImageComponent>	m_wpGetItemAnimationImageComponentSlot0;

		std::weak_ptr<CAnimationImageComponent>	m_wpGetItemAnimationImageComponentSlot1;

		std::weak_ptr<CUIManager> m_wpUIManager;

	protected:
		bool m_bShaked = false;

		bool m_bStartShaking = false;

		float m_fSumShakedDeltaSecond = 0.0f;

		int32 m_iGetDamageCount = 0;

		std::weak_ptr<CSoundManager> m_wpSoundManager;

	protected:
		// 현재 체력
		float m_fCurHp = 100.0f;

		// 누적된 리스폰 시간
		float m_fSumRespawnDeltaSeconds = 0.0f;

		// 드롭될 아이템 이름
		const std::array<std::wstring, 2> m_arrDroppedItemNames{ L"", L"" };
	};
}
