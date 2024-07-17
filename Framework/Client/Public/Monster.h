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

		// 야생 팰 컬러 // 빨강
		Vector4 vRedColor = { 1.0f, 0.0f, 0.0f, 1.0f };
		// 파트너 팰 컬러 // 초록
		Vector4 vGreenColor = { 0.325f, 1.0f, 0.496f, 1.0f };
		// 파랑
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
		// 블랙보드 값 읽기
		bool GetBoolValue(EBlackboardBoolValue _eBBValue);

		float GetFloatValue(EBlackboardFloatValue _eBBValue);

		int32 GetIntValue(EBlackboardIntValue _eBBValue);

		Vector3 GetVector3Value(EBlackboardVector3Value _eBBValue);

		shared_ptr<CGameObject> GetGameObjectValue(EBlackboardGameObjectValue _eBBValue);

		const std::vector<EChannelType>& GetChannelValue(int32 _iAnimationID);

	public:
		// 블랙보드 값 쓰기
		HRESULT ModifyBoolValue(EBlackboardBoolValue _eBBValue, bool _bValue);

		HRESULT ModifyFloatValue(EBlackboardFloatValue _eBBValue, float _fValue);

		HRESULT ModifyIntValue(EBlackboardIntValue _eBBValue, int32 _iValue);

		HRESULT ModifyVector3Value(EBlackboardVector3Value _eBBValue, const Vector3& _vValue);

		HRESULT ModifyGameObjectValue(EBlackboardGameObjectValue _eBBValue, const std::shared_ptr<CGameObject>& _spValue);

		HRESULT ModifyChannelValue(int32 _iAnimationID, const std::vector<EChannelType>& _vChannelType);

	public:
		// 작업 시스템
		void RequestToDoCommand();

		HRESULT StartToDoCommand(const std::shared_ptr<FToDoCommand>& _spToDoCommand);

		HRESULT PauseToDoCommand();

		HRESULT FinishToDoCommand();

		virtual bool ExecuteToDoCommand();

		void AddSlavePalToToDoSystem();

		void RemoveSlavePalFromToDoSystem();

		// 전투 시스템
		HRESULT RequestCombatCommand();

		HRESULT PariticipateInCombat();

		HRESULT ParticipateOutCombat();

		// 기타 시스템
		std::shared_ptr<CArchitecture> FindArchitecture(EArchitectureType _eArchitectureType);

		void VacantPalBed();

		void OccupyPalBed(const std::shared_ptr<CPalBed>& _spPalBed);

		void AddItemToChestBox(const std::shared_ptr<CChest>& _spChest);

		// 팰스피어
		void RetrieveToPalSphere();

		// 비헤이비어 트리
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
		// 컴포넌트
		std::shared_ptr<CBehaviorTreeComponent> m_spBehaviorTreeComponent = nullptr;

		std::shared_ptr<CRectComponent> m_spHpRectComponent = nullptr;

		// 블랙보드 값
		std::unordered_map<EBlackboardBoolValue, bool> m_umapBBBools;

		std::unordered_map<EBlackboardFloatValue, float> m_umapBBFloats;

		std::unordered_map<EBlackboardIntValue, int32> m_umapBBInts;

		std::unordered_map<EBlackboardVector3Value, Vector3> m_umapBBVector3s;

		std::unordered_map<EBlackboardGameObjectValue, std::shared_ptr<CGameObject>> m_umapBBGameObjects;

		std::unordered_map<int32 /* 애니메이션 번호 */, std::vector<EChannelType> /* 사운드 채널들 */ > m_umapBBChannels;

	protected:
		// 게임 오브젝트
		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

		std::shared_ptr<CDate> m_spDate = nullptr;

	protected:
		// 패널
		std::shared_ptr<CPanel> m_spHpPanel = nullptr;

	protected:
		// 루트 컴포지트 노드
		std::shared_ptr<CCompositeNode> m_spRootNode = nullptr;

	protected:
		// 시스템
		std::shared_ptr<CCombatSystem> m_spCombatSystem = nullptr;

		std::shared_ptr<CToDoSystem> m_spToDoSystem = nullptr;

		// 팰이 작업하고 있는 명령
		std::weak_ptr<FToDoCommand> m_wpToDoCommand;

		// 생산할 공장
		std::weak_ptr<CFactory> m_wpFactory;

		// 건설할 건물
		std::weak_ptr<CArchitecture> m_wpArchitecture;

		// 나무, 바위
		std::weak_ptr<CNature> m_wpNature;

		// 나무 조각, 돌 조각
		std::weak_ptr<CItem> m_wpItem;

		// 아이템이 운반될 상자
		std::weak_ptr<CArchitecture> m_wpChestBox;

		// 운반 본
		std::weak_ptr<CBone> m_wpTransportBone;

		// 팰 침대
		std::weak_ptr<CPalBed> m_wpPalBed;

		float m_fToDoDeltaSeconds = 0.0f;

		// 팰이 갖고 있는 도구들
		std::unordered_map<EItemType, std::shared_ptr<CWeapon>> m_umapTools;

	protected:
		// 몬스터 타입
		EMonsterType m_eMonsterType = EMonsterType::ENUM_END; 

		// 몬스터 AI 타입
		EMonsterAIType m_eMonsterAIType = EMonsterAIType::WILD_PAL;

		// 몬스터 성격
		EMonsterPersonalityType m_eMonsterPersonalityType = EMonsterPersonalityType::ENUM_END;

		// 일하고 있는지 여부
		EMonsterToDoState m_eMonsterToDoState = EMonsterToDoState::NO_WORK;

		// 몬스터 크기 타입
		EMonsterSizeType m_eMonsterSizeType = EMonsterSizeType::ENUM_END;

		// 포획 확률
		int32 m_iCapturePercentage = 20;

		// 현재 파트너 스킬 게이지
		float m_fCurPartnerSkillGauge = 100.0f;

		// 파트너 스킬을 해금할 수 있는 레벨
		int32 m_iPartnerSkillUnlockLevel = 0;

		// 포획되었는지 여부
		bool m_bCaptureResult = false;

		// 전투에 참여 중인지 여부
		bool m_bCombat = false;

		// 뛰고 있는지 여부
		bool m_bJump = false;

		// 팰과 상호작용할 수 있는 거리 안에 있는 경우
		bool m_bCanInteract = false;

		// 엑티브 스킬 고유 속성
		EMonsterElementType m_eActiveSkillElementTypeA = EMonsterElementType::ENUM_END;
		EMonsterElementType m_eActiveSkillElementTypeB = EMonsterElementType::ENUM_END;
		EMonsterElementType m_eActiveSkillElementTypeC = EMonsterElementType::ENUM_END;

		// 엑티브 스킬 이름
		std::wstring m_wstrActiveSkillNameA = L"스킬 없음";
		std::wstring m_wstrActiveSkillNameB = L"스킬 없음";
		std::wstring m_wstrActiveSkillNameC = L"스킬 없음";

		// 패시브 스킬 이름
		std::wstring m_wstrPassiveSkillNameA = L"스킬 없음";
		std::wstring m_wstrPassiveSkillNameB = L"스킬 없음";

		// 몬스터 자체 레벨
		int32 m_iMonsterLevel = 0;

		// 파트너 스킬 레벨
		int32 m_iPartnerSkillLevel = 0;

		// 엑티브 스킬 레벨
		int32 m_iActiveSkillLevelA = 1;
		int32 m_iActiveSkillLevelB = 0;
		int32 m_iActiveSkillLevelC = 0;

		// 해야할 일
		EToDoType m_eToDoType = EToDoType::ENUM_END;

	// UI
	private:
		std::pair<std::shared_ptr<CPanel>, std::shared_ptr<CImageComponent>> m_prWorkPanel;

		std::shared_ptr<FPalHPInfo> m_spPalHPPanelInfo = nullptr;

		std::weak_ptr<CUIManager> m_wpUIManager;
		// 사이즈에 따른 ui 위치 연동
		Vector3 m_vUISize = { 0.f, 1.f, 0.f };
	};
}
