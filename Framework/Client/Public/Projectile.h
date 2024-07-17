#pragma once
#include "Weapon.h"

namespace Engine
{
	class CSphereComponent;
	class CTrailComponent;
}

namespace Client
{	
	class CEffectManager;
	class CEffectGroup;
	class CPlayer;

	struct FProjectileDesc : FWeaponDesc
	{
		shared_ptr<CPlayer> m_spOwnPlayer = nullptr;
		float m_fColliderRadius = 0.f;
		bool m_bDeferredAdd = true;
		wstring m_wstrEffectName = L"";
	};

    class CProjectile : public CWeapon
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CProjectile() = default;

		virtual ~CProjectile() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CProjectile> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	public:
		void InitColliderComponent(shared_ptr<FProjectileDesc> _spProjectileDesc);
		virtual void InitTrailComponent();
		void TickTrail(float _fTrailSize);

	/********************
		Getter/Setter
	********************/
	public:
		virtual void SetPosition(const Vector3& _vPosition) override;
		const Vector3& GetThrowDirection() { return m_vThrowDirection; }

	protected:
		virtual void ResetDataMember();

	/********************
		Methods
	********************/
	public:
		virtual void Throw(Vector3 _vActivePos, Vector3 _vDirection, float _fPower, float _fDamage);

	/********************
		Data Members
	********************/
	protected:
		std::weak_ptr<CSphereComponent> m_wpCollider;

		std::weak_ptr<CPlayer> m_wpOwnedPlayer;

		bool m_bThrowing = false;
		
		float m_fThrowSpeed = 0.0f;
		float m_fSumThrowSeconds = 0.0f;
		float m_fDisableTime = 5.f;

		float m_fLocalGravity = 9.8f;
		float m_fLocalTerminalSpeed = 65.f;

		Vector3 m_vThrowDirection = Vector3(0.f);

	protected:
		wstring m_wstrEffectName = L"";

		weak_ptr<CEffectGroup> m_wpEffectGroup;

		Vector3 m_vEffectOffset = Vector3(0.f);

	protected:
		std::shared_ptr<CTrailComponent> m_spTrailComponent = nullptr;

		float m_fTrailSize = 0.2f;

	protected:
		weak_ptr<CEffectManager> m_wpEffectManager;
    };
}
