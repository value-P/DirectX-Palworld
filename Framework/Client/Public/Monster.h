#pragma once
#include "Creature.h"

namespace Engine
{
	class CBehaviorTreeComponent;
	class CRectComponent;
	class CImageComponent;
	class CTextComponent;
	class CProgressBarComponent;
	class CCompositeNode;
	class CDate;
	class CPanel;
	class CBone;
}

namespace Client
{
	class CCombatSystem;
	class CToDoSystem;
	struct FToDoCommand;

	class CPlayer;
	class CFactory;
	class CArchitecture;
	class CNature;
	class CItem;
	class CPalBed;
	class CChest;
	class CEffectGroup;
	class CWeapon;
	
	class CUIManager;

	/********************
		Descriptor
	********************/
	struct FMonsterDesc : public FCreatureDesc
	{
		EMonsterType m_eMonsterType = EMonsterType::ENUM_END;

		int32 m_iPartnerSkillLevel = 0;

		int32 m_iActiveSkillALevel = 0;

		int32 m_iActiveSkillBLevel = 0;

		int32 m_iActiveSkillCLevel = 0;

		int32 m_iMonsterLevel = 1;

		EMonsterAIType m_eMonsterAIType = EMonsterAIType::ENUM_END;
		
		EMonsterPersonalityType m_eMonsterPersonalityType = EMonsterPersonalityType::ENUM_END;
	};					  

	struct FPalHPInfo
	{
		std::shared_ptr<CPanel> m_spPalHPPanel;
		std::shared_ptr<CImageComponent> m_spElementImage;
		std::shared_ptr<CProgressBarComponent> m_spHPProgress;
		std::shared_ptr<CTextComponent> m_spLevelText;
		std::shared_ptr<CTextComponent> m_spNameText;
		std::shared_ptr<CImageComponent> m_spNameBackImage;

		// �߻� �� �÷� // ����
		Vector4 vRedColor = { 1.0f, 0.0f, 0.0f, 1.0f };
		// ��Ʈ�� �� �÷� // �ʷ�
		Vector4 vGreenColor = { 0.325f, 1.0f, 0.496f, 1.0f };
		// �Ķ�
		Vector4 vBlueColor = { 0.07f, 0.847f, 1.0f, 1.0f };
	};

	class CMonster : public CCreature
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CMonster() = default;

		virtual ~CMonster() = default;
		
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

		virtual void Release() override;

	public:
		void InitBlackboardValues();

		void ResetBlackboardValues(EBlackboardValueType _eBlackboardValueType);

		virtual void UpdateBlackboardValues();

	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	protected:
		virtual void ColliderOn(const FAnimNotifyData& _tNotifyData) override { };

	/********************
		Getter/Setter
	********************/
	public:
		int32 GetCapturePercentage() const { return m_iCapturePercentage; }

		void SetCapturePercentage(int32 _iCurCapturePercentage) { m_iCapturePercentage = _iCurCapturePercentage; }

		int32 GetPartnerSkillUnlockLevel() const { return m_iPartnerSkillUnlockLevel; }

		void SetPartnerSkillUnlockLevel(int32 _iPartnerSkillUnlockLevel) { m_iPartnerSkillUnlockLevel = _iPartnerSkillUnlockLevel; }

		float GetCurPartnerSkillGauge() const { return m_fCurPartnerSkillGauge; }

		void SetCurPartnerSkillGauge(float _fCurPartnerSkillGauge) { m_fCurPartnerSkillGauge = _fCurPartnerSkillGauge; }

		EMonsterToDoState GetMonsterToDoState() const { return m_eMonsterToDoState; }

		void SetMonsterToDoState(EMonsterToDoState _eMonsterToDoState) { m_eMonsterToDoState = _eMonsterToDoState; }

		int32 GetMonsterLevel() const { return m_iMonsterLevel; }
		
		void SetMonsterLevel(int32 _iMonsterLevel) { m_iMonsterLevel = _iMonsterLevel; }

		EMonsterSizeType GetMonsterSizeType() const { return m_eMonsterSizeType; }

	public:
		EMonsterType GetMonsterType() const { return m_eMonsterType; }

		EMonsterAIType GetMonsterAIType() const { return m_eMonsterAIType; }

		EMonsterPersonalityType GetMonsterPersonalityType() const { return m_eMonsterPersonalityType; }

		std::shared_ptr<FToDoCommand> GetToDoCommand() { return m_wpToDoCommand.expired() ? nullptr : m_wpToDoCommand.lock(); }

		EToDoType GetToDoType() const { return m_eToDoType; };

		int32 GetPartnerSkillLevel() const { return m_iPartnerSkillLevel; }

		int32 GetActiveSkillLevelA() const { return m_iActiveSkillLevelA; }

		int32 GetActiveSkillLevelB() const { return m_iActiveSkillLevelB; }

		int32 GetActiveSkillLevelC() const { return m_iActiveSkillLevelC; }

		const std::wstring& GetActiveSkillNameA() const { return m_wstrActiveSkillNameA; }

		const std::wstring& GetActiveSkillNameB() const { return m_wstrActiveSkillNameB; }

		const std::wstring& GetActiveSkillNameC() const { return m_wstrActiveSkillNameC; }

		EMonsterElementType GetActiveSkillElementA() const { return m_eActiveSkillElementTypeA; }

		EMonsterElementType GetActiveSkillElementB() const { return m_eActiveSkillElementTypeB; }

		EMonsterElementType GetActiveSkillElementC() const { return m_eActiveSkillElementTypeC; }

		const std::wstring& GetPassiveSkillNameA() const { return m_wstrPassiveSkillNameA; }

		const std::wstring& GetPassiveSkillNameB() const { return m_wstrPassiveSkillNameB; }
		
		bool IsCaptured() const { return m_bCaptureResult; }

	public:
		void SetJump(bool _bJump) { m_bJump = _bJump; }

		bool IsJump() const { return m_bJump; }

		void SetCanInteract(bool _bCanInteract) { m_bCanInteract = _bCanInteract; }

		bool CanInteract() const { return m_bCanInteract; }

		void SetParticipateInCombat(bool _bCombat) { m_bCombat = _bCombat; }

		virtual bool IsParticipateInCombat() const { return m_bCombat; }

	public:
		virtual std::shared_ptr<CColliderComponent> GetBodyColliderComponent() { return nullptr; }

		virtual std::shared_ptr<CColliderComponent> GetAttackColliderComponent() { return nullptr; }

		std::shared_ptr<CBehaviorTreeComponent> GetBehaviorTreeComponent() { return m_spBehaviorTreeComponent; }

		virtual void SetLinearVelocity(const PxVec3& _vLinearVelocity) { return; }

		virtual void SetAngularVelocity(const PxVec3& _vLinearVelocity) { return; };

	/******************
		Methods
	******************/
	public:
		// ������ �� �б�
		bool GetBoolValue(EBlackboardBoolValue _eBBValue);

		float GetFloatValue(EBlackboardFloatValue _eBBValue);

		int32 GetIntValue(EBlackboardIntValue _eBBValue);

		Vector3 GetVector3Value(EBlackboardVector3Value _eBBValue);

		shared_ptr<CGameObject> GetGameObjectValue(EBlackboardGameObjectValue _eBBValue);

		const std::vector<EChannelType>& GetChannelValue(int32 _iAnimationID);

	public:
		// ������ �� ����
		HRESULT ModifyBoolValue(EBlackboardBoolValue _eBBValue, bool _bValue);

		HRESULT ModifyFloatValue(EBlackboardFloatValue _eBBValue, float _fValue);

		HRESULT ModifyIntValue(EBlackboardIntValue _eBBValue, int32 _iValue);

		HRESULT ModifyVector3Value(EBlackboardVector3Value _eBBValue, const Vector3& _vValue);

		HRESULT ModifyGameObjectValue(EBlackboardGameObjectValue _eBBValue, const std::shared_ptr<CGameObject>& _spValue);

		HRESULT ModifyChannelValue(int32 _iAnimationID, const std::vector<EChannelType>& _vChannelType);

	public:
		// �۾� �ý���
		void RequestToDoCommand();

		HRESULT StartToDoCommand(const std::shared_ptr<FToDoCommand>& _spToDoCommand);

		HRESULT PauseToDoCommand();

		HRESULT FinishToDoCommand();

		virtual bool ExecuteToDoCommand();

		void AddSlavePalToToDoSystem();

		void RemoveSlavePalFromToDoSystem();

		// ���� �ý���
		HRESULT RequestCombatCommand();

		HRESULT PariticipateInCombat();

		HRESULT ParticipateOutCombat();

		// ��Ÿ �ý���
		std::shared_ptr<CArchitecture> FindArchitecture(EArchitectureType _eArchitectureType);

		void VacantPalBed();

		void OccupyPalBed(const std::shared_ptr<CPalBed>& _spPalBed);

		void AddItemToChestBox(const std::shared_ptr<CChest>& _spChest);

		// �ӽ��Ǿ�
		void RetrieveToPalSphere();

		// �����̺�� Ʈ��
		virtual HRESULT ReInitializePalBT(EMonsterAIType _eMonsterAIType);

		void InteractWithSlavePal(EMonsterInteractType _eMonsterInteractType);

		float GetAcceptableRadiusFromColliderComponent();

		virtual void GrabTool(EItemType _eItemType, bool _bGrab);

		virtual void ChangeEye(EPalEyeType _ePalEyeType);

		virtual void ChangeMouth(EPalMouthType _ePalMouthType);

		void StopSoundAll();

	public:
		virtual void TakeDamage(const std::shared_ptr<CGameObject>& _spInstigator, float _fDamage);

	public:
		virtual std::tuple<bool, int32> ComputeCaptureProbability();

		void SetPalWorkPanel();

		void SetPalWorkPanelImage(EToDoType _eTodoType);

		void SetActivePalWorkPanel(bool _bActive);

		void ReturnPalWorkPanel();

		std::pair<std::shared_ptr<CPanel>, std::shared_ptr<CImageComponent>>& GetPalWorkPanel() { return m_prWorkPanel; }

		void SetUISize();

		Vector3 GetUISize() { return m_vUISize; }

		void SetPalHPPanel();

		void ReturnPalHPPanel();

		void ChangeHPColor(EMonsterAIType _eMonsterAIType);

		virtual void SetAllyCollisionLayer() { }

	public:
		virtual void SetActive(bool _bActive) override;

	/********************
		Data Members
	********************/
	protected:
		// ������Ʈ
		std::shared_ptr<CBehaviorTreeComponent> m_spBehaviorTreeComponent = nullptr;

		std::shared_ptr<CRectComponent> m_spHpRectComponent = nullptr;

		// ������ ��
		std::unordered_map<EBlackboardBoolValue, bool> m_umapBBBools;

		std::unordered_map<EBlackboardFloatValue, float> m_umapBBFloats;

		std::unordered_map<EBlackboardIntValue, int32> m_umapBBInts;

		std::unordered_map<EBlackboardVector3Value, Vector3> m_umapBBVector3s;

		std::unordered_map<EBlackboardGameObjectValue, std::shared_ptr<CGameObject>> m_umapBBGameObjects;

		std::unordered_map<int32 /* �ִϸ��̼� ��ȣ */, std::vector<EChannelType> /* ���� ä�ε� */ > m_umapBBChannels;

	protected:
		// ���� ������Ʈ
		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

		std::shared_ptr<CDate> m_spDate = nullptr;

	protected:
		// �г�
		std::shared_ptr<CPanel> m_spHpPanel = nullptr;

	protected:
		// ��Ʈ ������Ʈ ���
		std::shared_ptr<CCompositeNode> m_spRootNode = nullptr;

	protected:
		// �ý���
		std::shared_ptr<CCombatSystem> m_spCombatSystem = nullptr;

		std::shared_ptr<CToDoSystem> m_spToDoSystem = nullptr;

		// ���� �۾��ϰ� �ִ� ���
		std::weak_ptr<FToDoCommand> m_wpToDoCommand;

		// ������ ����
		std::weak_ptr<CFactory> m_wpFactory;

		// �Ǽ��� �ǹ�
		std::weak_ptr<CArchitecture> m_wpArchitecture;

		// ����, ����
		std::weak_ptr<CNature> m_wpNature;

		// ���� ����, �� ����
		std::weak_ptr<CItem> m_wpItem;

		// �������� ��ݵ� ����
		std::weak_ptr<CArchitecture> m_wpChestBox;

		// ��� ��
		std::weak_ptr<CBone> m_wpTransportBone;

		// �� ħ��
		std::weak_ptr<CPalBed> m_wpPalBed;

		float m_fToDoDeltaSeconds = 0.0f;

		// ���� ���� �ִ� ������
		std::unordered_map<EItemType, std::shared_ptr<CWeapon>> m_umapTools;

	protected:
		// ���� Ÿ��
		EMonsterType m_eMonsterType = EMonsterType::ENUM_END; 

		// ���� AI Ÿ��
		EMonsterAIType m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		// ���� ����
		EMonsterPersonalityType m_eMonsterPersonalityType = EMonsterPersonalityType::ENUM_END;

		// ���ϰ� �ִ��� ����
		EMonsterToDoState m_eMonsterToDoState = EMonsterToDoState::NO_WORK;

		// ���� ũ�� Ÿ��
		EMonsterSizeType m_eMonsterSizeType = EMonsterSizeType::ENUM_END;

		// ��ȹ Ȯ��
		int32 m_iCapturePercentage = 20;

		// ���� ��Ʈ�� ��ų ������
		float m_fCurPartnerSkillGauge = 100.0f;

		// ��Ʈ�� ��ų�� �ر��� �� �ִ� ����
		int32 m_iPartnerSkillUnlockLevel = 0;

		// ��ȹ�Ǿ����� ����
		bool m_bCaptureResult = false;

		// ������ ���� ������ ����
		bool m_bCombat = false;

		// �ٰ� �ִ��� ����
		bool m_bJump = false;

		// �Ӱ� ��ȣ�ۿ��� �� �ִ� �Ÿ� �ȿ� �ִ� ���
		bool m_bCanInteract = false;

		// ��Ƽ�� ��ų ���� �Ӽ�
		EMonsterElementType m_eActiveSkillElementTypeA = EMonsterElementType::ENUM_END;
		EMonsterElementType m_eActiveSkillElementTypeB = EMonsterElementType::ENUM_END;
		EMonsterElementType m_eActiveSkillElementTypeC = EMonsterElementType::ENUM_END;

		// ��Ƽ�� ��ų �̸�
		std::wstring m_wstrActiveSkillNameA = L"��ų ����";
		std::wstring m_wstrActiveSkillNameB = L"��ų ����";
		std::wstring m_wstrActiveSkillNameC = L"��ų ����";

		// �нú� ��ų �̸�
		std::wstring m_wstrPassiveSkillNameA = L"��ų ����";
		std::wstring m_wstrPassiveSkillNameB = L"��ų ����";

		// ���� ��ü ����
		int32 m_iMonsterLevel = 0;

		// ��Ʈ�� ��ų ����
		int32 m_iPartnerSkillLevel = 0;

		// ��Ƽ�� ��ų ����
		int32 m_iActiveSkillLevelA = 1;
		int32 m_iActiveSkillLevelB = 0;
		int32 m_iActiveSkillLevelC = 0;

		// �ؾ��� ��
		EToDoType m_eToDoType = EToDoType::ENUM_END;

	// UI
	private:
		std::pair<std::shared_ptr<CPanel>, std::shared_ptr<CImageComponent>> m_prWorkPanel;

		std::shared_ptr<FPalHPInfo> m_spPalHPPanelInfo = nullptr;

		std::weak_ptr<CUIManager> m_wpUIManager;
		// ����� ���� ui ��ġ ����
		Vector3 m_vUISize = { 0.f, 1.f, 0.f };
	};
}
