#pragma once
#include "Item.h"

namespace Engine
{
	class CBone;
}

namespace Client
{
	struct FWeaponDesc : public FItemDesc
	{
		shared_ptr<CBone> m_spAttachedBone = nullptr;
		bool m_bIsAnimed = false;
		Matrix m_matPivot;
		wstring m_wstrModelFilePath = L"";

		ERenderType m_eRenderType = ERenderType::NONE;
		EWeaponType m_eWeaponType = EWeaponType::NONE;
		EWeaponActionType m_eWeaponActionType = EWeaponActionType::ENUM_END;
		EMeshManageType m_eMeshManageType = EMeshManageType::UNIQUE;

		float m_fAttackPower = 0.0f;
		float m_fDurability = 0.0f;
		int32 m_iWeaponUnlockLevel = 0;
	};

	class CWeapon : public CItem
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CWeapon() = default;

		virtual ~CWeapon() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CWeapon> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	private:
		void InitMeshComponent(const std::shared_ptr<FWeaponDesc>& _spWeaponDesc);

	/********************
		Getter/Setter
	********************/
	public:
		Matrix GetBoneWorldMatrix(const wstring& _wstrBonename);

		const float GetAttackPower() const { return m_fAttackPower; }

		const float GetDurability() const { return m_fDurability; }

		const int32 GetWeaponUnlockLevel() const { return m_iWeaponUnlockLevel; }

		const EWeaponType GetWeaponType() const { return m_eWeaponType; }

		const EWeaponActionType GetWeaponActionType() const { return m_eWeaponActionType; }

		void SetUseAnimation(bool _bUseAnimation, bool _bUseIdentityMatrices = false);

		void SetWeaponType(EWeaponType _eWeaponType) { m_eWeaponType = _eWeaponType; }
		
	/********************
		Methods
	********************/
	public:
		void ChangeAnimation(int32 _iAnimIndex);

		Vector3 GetAttachedWorldPosition();

		Matrix GetAttachedWorldMatrix();

		weak_ptr<CBone> GetAttachedBone() { return m_wpAttachedBone; }

	/********************
		Data Members
	********************/
	protected:
		weak_ptr<CBone> m_wpAttachedBone;

		// 상세 무기 종류 타입입니다.
		EWeaponType m_eWeaponType = EWeaponType::ENUM_END;

		// 현재 무기 타입 입니다.
		EWeaponActionType m_eWeaponActionType = EWeaponActionType::ENUM_END;

		// 무기 공격력
		float m_fAttackPower = 0.0f;
		
		// 무기 내구도
		float m_fDurability = 0.0f;

		// 무기 해금 레벨
		int32 m_iWeaponUnlockLevel = 0;
	};
}


