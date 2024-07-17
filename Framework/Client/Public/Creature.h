#pragma once
#include "Character.h"

namespace Engine
{
	class CSoundManager;
	class CMeshComponent;
	class CSphereComponent;
}

namespace Client
{
	class CTreeManager;
	class CNotifyManager;
	class CEffectManager;
	class CUIManager;

	/********************
		Descriptor
	********************/
	struct FCreatureDesc : public FCharacterDesc
	{
		ECreatureType m_eCreatureType = ECreatureType::ENUM_END;

		std::optional<float> m_fCurHp = std::nullopt;

		std::optional<float> m_fMaxHp = std::nullopt;

		std::optional<float> m_fCurExp = std::nullopt;

		std::optional<float> m_fMaxExp = std::nullopt;

		std::optional<float> m_fCurHungerGauge = std::nullopt;

		std::optional<float> m_fAttackPower = std::nullopt;

		std::optional<float> m_fDefensePower = std::nullopt;
	};

	class CCreature : public CCharacter
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CCreature() = default;

		virtual ~CCreature() = default;
		
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release();
				
	/********************
		Getter/Setter
	********************/
	public:
		const float GetCurHp() const { return m_fCurHp; }
		
		const float GetMaxHp() const { return m_fMaxHp; }

		const float GetPreHp() const { return m_fPreHp; }

		const float GetCurExp() const { return m_fCurExp; }

		const float GetMaxExp() const { return m_fMaxExp; }

		const float GetAttackPower() const { return m_fAttackPower; }

		const float GetDefensePower() const { return m_fDefensePower; }

		const float GetCurStaminaGauge() const { return m_fCurStaminaGauge; }

		const float GetMaxStaminaGauge() const { return m_fMaxStaminaGauge; }

		const float GetPreStaminaGauge() const { return m_fPreStaminaGauge; }

		const float GetCurHungerGauge() const { return m_fCurHungerGauge; }

		const float GetMaxHungerGauge() const { return m_fMaxHungerGauge; }

		const int32 GetCurCreatureLevel() const { return m_iCurCreatureLevel; }

		const int32 GetStatusPoint() const { return m_iStatusPoint; }

		float* GetHPLerpTime() { return &m_fHPLerpTime; }

		float GetCreepSpeed() const { return m_fCreepSpeed; }

		float GetRunSpeed() const { return m_fRunSpeed; }

		ECreatureType GetCreatureType() const { return m_eCreatureType; }

	public:
		void SetCurHp(float _fCurHp) { m_fCurHp = _fCurHp; }

		void SetMaxHp(float _fMaxHp) { m_fMaxHp = _fMaxHp; }

		void SetPreHp(float _fPreHp) { m_fPreHp = _fPreHp; }

		void SetCurExp(float _fCurExp) { m_fCurExp = _fCurExp; }

		void SetMaxExp(float _fMaxExp) { m_fMaxExp = _fMaxExp; }

		void SetAttackPower(float _fAttackPower) { m_fAttackPower = _fAttackPower; }

		void SetDefensePower(float _fDefensePower) {  m_fDefensePower = _fDefensePower; }

		void SetCurStaminaGauge(float _fCurStaminaGauge) { m_fCurStaminaGauge = _fCurStaminaGauge; }

		void SetMaxStaminaGauge(float _fMaxStaminaGauge) { m_fMaxStaminaGauge = _fMaxStaminaGauge; }

		void SetPreStaminaGauge(float _fPreStaminaGauge) { m_fPreStaminaGauge = _fPreStaminaGauge; }

		void SetCurHungerGauge(float _fCurHungerGauge) { m_fCurHungerGauge = _fCurHungerGauge; }

		void SetMaxHungerGauge(float _fMaxHungerGauge) { m_fMaxHungerGauge = _fMaxHungerGauge; }

		void SetCurCreatureLevel(int32 _iCurCreatureLevel) { m_iCurCreatureLevel = _iCurCreatureLevel; }

		void SetStatusPoint(int32 _iStatusPoint) { m_iStatusPoint = _iStatusPoint; }

		void SetHPLerpTime(float _fHPLerpTime) { m_fHPLerpTime = _fHPLerpTime; }

	public:
		bool IsDead() const { return m_bDead; };

		void SetIsDead(bool _bDead) { m_bDead = _bDead; }

		virtual bool IsParticipateInCombat() const { return true; }

	/******************
		Methods
	******************/
	public:
		void IncreaseHungerGauge(float _fDeltaSeconds);

		void DecreaseHungerGauge(float _fDeltaSeconds);

	public:
		virtual void TakeDamage(const std::shared_ptr<CGameObject>& _spInstigator, float _fDamage); 
		
		virtual void AnimNotifyCall(const list<FAnimNotifyData>& _lstAnimNotify) override;

	protected:
		virtual void ColliderOn(const FAnimNotifyData& _tNotifyData);

		virtual void PlaySoundWithID(const FAnimNotifyData& _tNotifyData);

		virtual void PlayEffect(const FAnimNotifyData& _tNotifyData);

		virtual void PlayFunction(const FAnimNotifyData& _tNotifyData);

	/********************
		Data Members
	********************/
	protected:
		std::weak_ptr<CNotifyManager> m_wpNotifyManager;

		std::weak_ptr<CEffectManager> m_wpEffectManager;

		std::weak_ptr<CSoundManager> m_wpSoundManager;

		std::weak_ptr<CTreeManager> m_wpTreeManager;

		std::unordered_map<wstring, std::function<void()>> m_umapFunctions;

	protected:
		ECreatureType m_eCreatureType = ECreatureType::ENUM_END;

		// ���� ü��, ���� ü��, �ִ� ü��
		float m_fPreHp = 100.0f;

		float m_fCurHp = 100.0f;

		float m_fMaxHp = 100.0f;

		// ���� ����ġ, �ִ� ����ġ
		float m_fCurExp = 0.0f;

		float m_fMaxExp = 100.0f;

		// ���� ����, �ִ� ����
		int32 m_iCurCreatureLevel = 1;

		// ���ݷ�, �����
		float m_fAttackPower = 1.0f;

		float m_fDefensePower = 1.0f;

		// ���׹̳� ��ġ
		float m_fCurStaminaGauge = 100.0f;

		// �ִ� ���׹̳� ��ġ
		float m_fMaxStaminaGauge = 100.0f;

		// ���� ���׹̳� ��ġ
		float m_fPreStaminaGauge = 100.0f;

		// ���� ��ġ
		float m_fCurHungerGauge = 100.0f;

		// �ִ� ���� ��ġ
		float m_fMaxHungerGauge = 100.0f;

		// �������ͽ� ����Ʈ
		int32 m_iStatusPoint = 0;
		
		// ��� ����
		bool m_bDead = false;

		// ���� �̵� �ӵ�
		float m_fRunSpeed = 10.0f;

		// ���� �̵� �ӵ�
		float m_fCreepSpeed = 1.0f;

	protected:
		// ü�� ���� ���� �ð�
		float m_fHPLerpTime = 0.0f;

		// ���� ���� ���� �ð�
		float m_fHungerLerpTime = 0.0f;

		// �ǵ� ���� ���� �ð�
		float m_fShieldLerpTime = 0.0f;

		// ����ġ ���� ���� �ð�
		float m_fExpLerpTime = 0.0f;

		// ���׹̳� ���� ���� �ð�
		float m_fStaminaLerpTime = 0.0f;

	private:
		std::weak_ptr<CUIManager> m_wpUIManager;
	};
}
