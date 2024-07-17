#pragma once
#include "Item.h"

namespace Client
{
	/********************
		Descriptor
	********************/
	struct FArmorDesc : public FItemDesc
	{
	};

	class CArmor : public CItem
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CArmor() = default;

		virtual ~CArmor() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CArmor> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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
		const float GetDefensePower() const { return m_fDefensePower; }

		const float GetDefenseGauge() const { return m_fDefenseGauge; }

		const float GetDurability() const { return m_fDurability; }

		const float GetDefenseCoolTime() const { return m_fDefenseCoolTime; }

		const int32 GetArmorUnlockLevel() const { return m_iArmorUnlockLevel; }

	public:
		void SetDefensePower(float _fDefensePower) { m_fDefensePower = _fDefensePower; }

		void SetDefenseGauge(float _fDefenseGauge) { m_fDefenseGauge = _fDefenseGauge; }

		void SetDurability(float _fDurability) { m_fDurability = _fDurability; }

		void SetDefenseCoolTime(float _fDefenseCoolTime) { m_fDefenseCoolTime = _fDefenseCoolTime; }

		void SetArmorUnlockLevel(int32 _iArmorUnlockLevel) { m_iArmorUnlockLevel = _iArmorUnlockLevel; }

	public:
		const EArmorType GetCurArmorType() const { return m_eCurArmorType; }
		
		void SetCurArmorType(const EArmorType& _eArmorType) { m_eCurArmorType = _eArmorType; }

	/********************
		Data Members
	********************/
	protected:
		// 현재 장착하고 있는 아머 타입 입니다.
		EArmorType m_eCurArmorType = EArmorType::ENUM_END;

		// 방어력 입니다.
		float m_fDefensePower = 0.0f;

		// 방어게이지 입니다(수치가 0이 되면, 일정 시간이 지나야 보호막이 다시 생깁니다).
		float m_fDefenseGauge = 0.0f;

		// 내구도 입니다(방어게이지는 내구도에서 충전을 받는 로직이며, 내구도가 0이 되면, 보호막 아머가 파괴가 됩니다).
		float m_fDurability = 0.0f;

		// 방어게이지가 재 충전되기 전까지의 쿨타임 시간입니다.
		float m_fDefenseCoolTime = 0.f;

		// 아머를 얻기 위한 기술 해금 레벨 입니다.
		int32 m_iArmorUnlockLevel = 0;
	};
}


