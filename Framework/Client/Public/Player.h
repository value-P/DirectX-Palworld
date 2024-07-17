#pragma once
#include "Creature.h"

namespace Engine
{
	class CBoxComponent;
	class CCameraComponent;
	class CFSMComponent;
	class CMeshComponent;
	class CSphereComponent;
	class CInputManager;
	class CCameraManager;
	class CPhysXManager;
	class CPanel;
	class CTextComponent;
	class CProgressBarComponent;
	class CAnimationImageComponent;
	class CImageComponent;
}

namespace Client
{
	// manager
	class CEffectManager;
	class CUIManager;
	class CBuildManager;
	class CPalInteractionSystem;

	// component
	class CPlayerControllerComponent;

	// gameobject
	class CProjectilePool;
	class CWeapon;
	class CMonster;
	class CArchitecture;
	class CFactory;
	class CNature;
	class CNpc;
	class CRidingObject;
	class CInventoryCustomObject;

	// system
	class CToDoSystem;
	class CInventorySystem;

	struct FPlayerDesc : public FCreatureDesc
	{
		std::shared_ptr<CInventoryCustomObject> m_spInvenCustomObject;
	};

	class CPlayer final : public CCreature
	{
	/********************
		Enum
	********************/
	public:
		enum class EPlayerState { IDLE, MOVE, ATTACK, JUMP, ROLL, EMOTE, INTERACT, CLIMB, RIDING, SWIMMING, ENUM_END };

		enum class EStandType { STAND, CROUCH, ENUM_END };

		enum class EInteractType { WORK, MINING, DEFOREST, WATERING, COOKING, ARCHITECT, PLANTING, PETTING, PALRETURN, SPAWN_PAL, RIDING_RETURN, SPAWN_RIDING, TALKING, RIDING, OPEN, FEEDING, SLAUGHTER, COLLECT, SLEEP, ENUM_END };

	/********************
		Struct
	********************/
	public:
		struct FPlayerStateData
		{
			EPlayerState			m_eCurrentState		= EPlayerState::IDLE;
			EStandType				m_eStandType		= EStandType::STAND;
			
			EWeaponActionType		m_eWeaponActionType		= EWeaponActionType::NONE;
			EWeaponActionType		m_ePrevWeaponActionType = EWeaponActionType::NONE;

			EItemType				m_eWeaponID			= EItemType::NONE;
			EItemType				m_ePrevWeaponID		= EItemType::NONE;

			EItemType				m_eOutfitID			= EItemType::NONE;
			EItemType				m_eHeadEquipID		= EItemType::NONE;

			EInteractType			m_eInteractType		= EInteractType::ENUM_END;
			weak_ptr<CNature>		m_wpInteractNature;
			EInteractType			m_ePrevInteractType = EInteractType::ENUM_END;
			weak_ptr<CFactory>		m_wpInteractFactory;
			weak_ptr<CArchitecture>	m_wpInteractArchitecture;
			weak_ptr<CNpc>			m_wpInteractNpc;
			weak_ptr<CRidingObject>	m_wpRidingObject;
			weak_ptr<CMonster>		m_wpInteractAllyPal;

			EEmoteType				m_eEmoteType		= EEmoteType::ENUM_END;
			EVerticalMovementType	m_eVerticalMoveType = EVerticalMovementType::ON_GROUND;
			EPlayerControllerMode	m_eControlMode		= EPlayerControllerMode::DEFAULT_MODE;
			Vector3					m_vMoveDirection	= Vector3(0.0f, 0.0f, 0.0f);

			bool m_bWeaponChangeCall = false;
			bool m_bJumping = false;
			bool m_bOnAir = false;
			bool m_bAiming = false;
			bool m_bUseSpineRot = false;
			bool m_bStaminaEmptied = false;

			int32 m_iCurrentBullet = 40;
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPlayer() = default;

		virtual ~CPlayer() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPlayer> Create(const std::shared_ptr<FPlayerDesc>& _spPlayerDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

		virtual void SetActive(bool _bActive) override;

	/********************
		Getter/Setter
	********************/
	public:
		shared_ptr<CMonster> GetPartnerPal() const { return m_spPartnerPal; }

		FPlayerStateData GetPlayerData();
		FPalBoxPalInfo GetCurrentPartnerPalData() { return m_arrPartnerPals[m_iCurPalInfoIndex].first; }
		
		void SetUseCameraMouseMove(bool _bUseMouseMove);
		void SetPartnerPal(const std::shared_ptr<CMonster>& _spMonster) { m_spPartnerPal = _spMonster; }
		void SetCustomInvenCustomObject(bool _bActivate);
		virtual void SetPositionWithController(const Vector3& _vPos);

		void ResetInteractingArchitectureData();

		bool IsPartnerPalSpawned()			{ return m_spPartnerPal != nullptr; }
		bool IsCurrentPartnerPalEmpty()		{ return m_arrPartnerPals[m_iCurPalInfoIndex].first.m_wstrMonsterName == L""; }
		bool IsRidingObjectSpawned()		{ return !m_wpSpawnedRidingObject.expired(); }
		bool CanSpawnCurrentRiding()		{ return m_arrCanUseRiding[m_iCurRidingObjectIndex]; }
		bool IsHandTorchHeld();
		bool IsArmorEquipped();
		bool IsGliderEquipped();
		bool CanAttack();
		bool CanThrowPalSphere();

		void SetarrPartnerPals(std::array<std::pair<FPalBoxPalInfo, std::shared_ptr<CMonster>>, 5>& _arrPartnerPals) { m_arrPartnerPals = _arrPartnerPals; }

		std::array<std::pair<FPalBoxPalInfo, shared_ptr<CMonster>>, 5>* GetPartnerPalPtr() { return &m_arrPartnerPals; }
		
	/********************
		Methods
	********************/
	private:
		void InitMeshComponent();

		void InitRidingObject();

		void InitCameraComponent();

		void InitFSMComponent();

		void InitPlayerControllerComponent();

		void InitColliderComponent();

		void InitProjectilePool();

		void InitUIPtr();

		void InitNotifyFunction();

		void SetColorHair(const Vector3& _vColor);

		void SetHeadMaterial(const FCustomObjectData& _tCustomData);

		void SetColorHeadEquip(const Vector3& _vColor);

	public:
		void SetOnAir();

	/********************
		Overrides
	********************/
	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void TakeDamage(const std::shared_ptr<CGameObject>& _spInstigator, float _fDamage) override;

	public:
		// ���ִ� ����
		void SetStandType(EStandType _eStandType) { m_tPlayerData.m_eStandType = _eStandType; }

		// ���� ����
		void ChangeWeapon(EItemType _eWeaponID);
		void WeaponChangeEnd() { m_tPlayerData.m_bWeaponChangeCall = false; }

		// ��� ����
		void ChangeOutfit(EItemType _eOutfitType);
		void ChangeHeadEquip(EItemType _eHeadEquipType);

		// �� ��/�ƿ�
		void ZoomIn();
		void ZoomOut() { m_tPlayerData.m_bAiming = false; }

		// ���� �ִϸ��̼� ����
		void ChangeBowAnimation(int32 _iAnimID);
		void ActiveCurrentWeapon(bool _bActivate);
		void ActiveArrow(bool _bActivate);
		void ActiveGlider(bool _bActivate);
		void ActivePalSphere(bool _bActivate);
		void ActiveConstructHammer(bool _bActivate);
		void ActiveFryingPan(bool _bActivate);
		void ActiveStoneAxe(bool _bActivate);
		void ActiveStonePickAxe(bool _bActivate);
		void ActiveMeatCleaver(bool _bActivate);
		void ActiveStaminaUI(bool _bActivate);

		void DisableBowAnimation();
		void ChangeAssaultRifleAnimation(int32 _iAnimID);
		bool IsPalSphereActivate();
		void SpawnPartnerPal(const Vector3& _vSpawnPos);
		void SpawnRidingObject(const Vector3& _vSpawnPos);
		void RetrievePartnerPal();
		void RetrieveRidingObject();

		// �ӽ��Ǿ� ������
		void ThrowPalSphere(float _fSpeed);
		void ThrowPartnerPalSphere(float _fSpeed);
		void ThrowRidingPalSphere(float _fSpeed);

		// �ѽ�� : �Ѿ˼� ���� �� false
		bool FireAssaultRifle();
		void ReloadBullet();

		// Ȱ���
		void FireArrow();

		// �ӵ�
		void SetSpeed(float _fSpeed);

		// �㸮 ���� Ȱ��ȭ ���
		void SetUseSpineRotation(bool _bUseSpineRotation) { m_tPlayerData.m_bUseSpineRot = _bUseSpineRotation; }
	
		// Controller
		void ChangeControlMode(Client::EPlayerControllerMode _eControlMode);
		Client::EPlayerControllerMode GetCurrentControlMode();

		// �������ɻ��� ����
		void SetCanJump(bool _bCanJump);

		void SetTerminalVelocity(float _fTerminalVelocity);
		void SetTerminalVelocityAsDefault();

		// �̵��Է� ����
		void SetFixedMoveDirection(Vector3 _vFixedMoveDir);

		// �÷��̾� ��� ���ڶ� ���޽� ȣ��
		void ActiveLedgeMode();

		void SetInteractType(EInteractType _eInteractType);

		// ����ǥ�� ����
		void SetEmoteType(EEmoteType _eEmoteType) { m_tPlayerData.m_eEmoteType = _eEmoteType; }

		// ��ȹȮ�� UI Ȱ��/��Ȱ��ȭ
		void SetCatchRateUI(const std::shared_ptr<CMonster>& _spTargetMonster, const Vector3& _vRayHitPos);
		void EnActiveCatchRateUI();

		// ���� ���⽽�� ������� �ľ�
		void CoumputeCurrentWeapon();
		void ComputeCurrentHeadEquip();
		void ComputeCurrentCloth();
		void ComputeHp(float _fDeltaSeconds);
		void ComputeHunger(float _fDeltaSeconds);

		// Riding����
		void RideOff();
		void RidingSprint(bool _bSprint) { m_bRidingSprint = _bSprint; }
		void DeclineStamina();

		// ��ȣ�ۿ��ϴ� ������ ����
		void ResetInteractAllyPal();

		// ���̵� �ر�
		void UnlockRiding(ERidingObjectType _eRidingObjectType);

		void ChangeRidingPalUI();

		void AddForce(const Vector3& _vForceDirection, float _fPower);

		void AddExp(float _fExp);

	private:
		// key input
		void KeyInput();
			void WeaponChangeInput();
			void StandChangeInput();
			void AimChangeInput();
			void PartnerPalInput();
			void UIInput();
			void CheatInput();
			void TestInput();

	public:
		// ���׹̳� �ڿ�ȸ��
		void ComputeStamina(float _fDeltaSeconds);
		// ���� �ڵ� ȸ��
		void ComputeShield(float _fDeltaSeconds);

	private:
		void AdjustPlayerRotation();

		void ComputeCamera();

		// ī�޶� ���� �������� ĳ������ rotation����
		void SetUpRotationFreeWithCamera();

		// ī�޶� ���� �̵��ϰ��� �ϴ� �������� rotation����
		void SetUpRotationWithCameraLook();

		// ī�޶���look�� �������� �㸮�� ��縦 ����
		void ComputeSpineRotationWithCameraLook();

		void ComputeMouseScrollLock();

		// ��ȣ�ۿ� Ÿ�� �ľ�
		void CheckInteractType(const std::shared_ptr<CArchitecture> _spArchitecture);

		// UI
	public:
		void UpdatePalIndexUI();
	
	private:
		void UpdateHUDUI(float _fDeltaSeconds);

		void NextPalIndex(uint32 _iDirection);

		void ChangeWeaponUI(EItemType _eItemType);

		void ChangePalSphereUI();

		void LoadMeshComponent(const wstring& _wstrFilePath, EItemType _eItemType, unordered_map<EItemType, shared_ptr<CMeshComponent>>& _umapMeshContainer, const wstring& _wstrAttachBoneName = L"");

	public:
		bool TryToAddCapturedPal(const std::shared_ptr<CMonster>& _spMonster);

	/********************
		Data Members
	********************/
	private:
		// �Ŵ���
		std::weak_ptr<CEffectManager>			m_wpEffectManager;
		std::weak_ptr<CInputManager>			m_wpInputManager;
		std::weak_ptr<CCameraManager>			m_wpCameraManager;
		std::weak_ptr<CPhysXManager>			m_wpPhysXManager;
		std::weak_ptr<CBuildManager>			m_wpBuildManager;
		std::weak_ptr<CUIManager>				m_wpUIManager;		
		std::weak_ptr<CToDoSystem>				m_wpToDoSystem;
		std::weak_ptr<CInventorySystem>			m_wpInventorySystem;
		std::weak_ptr<CPalInteractionSystem>	m_wpPalInteractionSystem;

	private:
		// �޽� ������Ʈ
		std::shared_ptr<CMeshComponent> m_spHeadMeshComponent = nullptr;
		std::shared_ptr<CMeshComponent> m_spHairMeshComponent = nullptr;
		std::unordered_map<EItemType, std::shared_ptr<CMeshComponent>> m_umapOutiftMeshs;
		std::unordered_map<EItemType, std::shared_ptr<CMeshComponent>> m_umapHeadEquipMeshs;
		std::shared_ptr<CMeshComponent> m_spArrowComponent	= nullptr;

		// �κ��丮 Ŀ�� ��
		std::weak_ptr<CInventoryCustomObject> m_wpInvenCustomObject;

		// FSM ������Ʈ
		std::shared_ptr<CFSMComponent> m_spFSMComponent = nullptr;

		// ��Ʈ�ѷ� ������Ʈ
		std::shared_ptr<CPlayerControllerComponent> m_spPlayerControllerComponent = nullptr;

		// �ݶ��̴� ������Ʈ
		std::shared_ptr<CBoxComponent> m_spJumpBoxComponent = nullptr;

		std::shared_ptr<CSphereComponent> m_spInteractionSphereComponent = nullptr;

		// ��Ʈ�� ��
		std::shared_ptr<CMonster> m_spPartnerPal = nullptr;

		// ���̵� (0�� ������/1�� ��Ʈ����)
		std::array<std::shared_ptr<CRidingObject>, (size_t)ERidingObjectType::ENUM_END> m_arrRidingObjects;
		std::array<bool, (size_t)ERidingObjectType::ENUM_END> m_arrCanUseRiding = { false, false };
		std::weak_ptr<CRidingObject> m_wpSpawnedRidingObject;

		// ���� �� ����� ��Ƴ����ϴ�. 
		std::array<std::pair<FPalBoxPalInfo, std::shared_ptr<CMonster>>, 5> m_arrPartnerPals;
				
		// ���� ���� �ε����� �����մϴ�.
		uint32 m_iCurPalInfoIndex = 0;
		int32 m_iCurRidingObjectIndex = 0;

	private:
		// ���� ����
		std::weak_ptr<CWeapon> m_wpWeapon;

		int32 m_iCurrentWeaponSlotIndex = 0;

		// ����
		std::unordered_map<EItemType, shared_ptr<CWeapon>> m_umapWeapon;
		std::shared_ptr<CProjectilePool> m_spArrowPool = nullptr;
		std::shared_ptr<CProjectilePool> m_spPalSpherePool = nullptr;
		std::shared_ptr<CProjectilePool> m_spPartnerPalSpherePool = nullptr;
		std::shared_ptr<CProjectilePool> m_spRidingPalSpherePool = nullptr;

	private:
		// UI
		// ��ȹ
		std::weak_ptr<CPanel> m_wpCatchRateUI;
			std::weak_ptr<CProgressBarComponent> m_wpCatchProgressBar;
			std::weak_ptr<CTextComponent> m_wpCatchRateText;
			std::weak_ptr<CImageComponent> m_wpCatchArrowImage;
			std::weak_ptr<CAnimationImageComponent> m_wpCatchAnimation0;
			std::weak_ptr<CAnimationImageComponent> m_wpCatchAnimation1;

		// ���׹̳�
		std::weak_ptr<CPanel> m_wpStaminaUI;
			std::weak_ptr<CProgressBarComponent> m_wpStaminaProgress;

		// ���̵�
		std::weak_ptr<CPanel> m_wpRidingUI;
			std::weak_ptr<CImageComponent> m_wpRidingObjectImage;
			std::weak_ptr<CImageComponent> m_wpLockImage;

	private:
		// ����ī�޶� radius
		float m_fCurrentRadius = 2.f;
		float m_fDefaultRadius = 2.f;
		float m_fZoomInRadius = 1.3f;
		float m_fRidingFarRadius = 10.f;
		float m_fRidingNearRadius = 5.f;
		float m_fSleepRaius = 3.5f;

		float m_fCurrentFOV = 60.f;
		float m_fDefaultFOV = 60.f;
		float m_fZoomFOV = 40.f;

		Vector3 m_vStandCameraOffset = Vector3(0.f, 1.6f, 0.0f);
		Vector3 m_vZoomInCameraOffset = Vector3(0.f, 1.45f, 0.0f);
		Vector3 m_vCrouchCameraOffset = Vector3(0.f, 1.3f, 0.0f);

		// ��ǥ Look
		Vector3 m_vDestLook = Vector3(0.0f, 0.0f, 1.0f);

		// �㸮 ����� ����
		float m_fCurrentSpineRotX = 0.f;
		float m_fCurrentSpineRotY = 0.f;
		float m_fPrevSpineRotX = 0.f;
		float m_fPrevSpineRotY = 0.f;

		// �÷��̾��� ������ ����
		FPlayerStateData m_tPlayerData;

		// Ż�� �޸��⶧ ī�޶� �־������� �Ѵ�
		bool m_bRidingSprint = false;

		int m_iShieldCount = 5;
		int m_iPreShieldCount = 5;
		float m_fShieldTimeChecker = 0.f;

		// �������
		bool m_bInvincibility = false; 
	};
}