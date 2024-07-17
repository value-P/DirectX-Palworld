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
		// ���� �����ϰ� �ִ� �Ƹ� Ÿ�� �Դϴ�.
		EArmorType m_eCurArmorType = EArmorType::ENUM_END;

		// ���� �Դϴ�.
		float m_fDefensePower = 0.0f;

		// �������� �Դϴ�(��ġ�� 0�� �Ǹ�, ���� �ð��� ������ ��ȣ���� �ٽ� ����ϴ�).
		float m_fDefenseGauge = 0.0f;

		// ������ �Դϴ�(���������� ���������� ������ �޴� �����̸�, �������� 0�� �Ǹ�, ��ȣ�� �ƸӰ� �ı��� �˴ϴ�).
		float m_fDurability = 0.0f;

		// ���������� �� �����Ǳ� �������� ��Ÿ�� �ð��Դϴ�.
		float m_fDefenseCoolTime = 0.f;

		// �ƸӸ� ��� ���� ��� �ر� ���� �Դϴ�.
		int32 m_iArmorUnlockLevel = 0;
	};
}


