#pragma once
#include "Manager.h"

namespace Engine
{
	class CSoundManager;

	class CWorld;
	class CPanel;

	class CRectComponent;
	class CProgressBarComponent;
	class CTextComponent;
	class CImageComponent;
	class CAnimationImageComponent;
}

namespace Client
{
	class CDamageFontObject;

	// HUD를 관리하기 위한 구조체입니다.
	struct FHUDHPInfo
	{
		shared_ptr<CPanel> spChildHPPanel;
		// LINEAR_INTERPOLATION_PROGRESSBAR
		shared_ptr<CProgressBarComponent> spShieldProgressComponent;
		//
		shared_ptr<CImageComponent> spBackShieldImageComponent;
		// LINEAR_INTERPOLATION_PROGRESSBAR
		shared_ptr<CProgressBarComponent> spHPProgressComponent;
		// LINEAR_HORIZONTAL_PROGRESSBAR
		shared_ptr<CProgressBarComponent> spHungerProgressComponent;
		// CIRCULAR_PROGRESSBAR
		// 낮밤 시스템 표시
		shared_ptr<CImageComponent> spTimezoneImageComponent;
		shared_ptr<CProgressBarComponent> spTimeCircleProgressComponent;
		// 플레이어 체력의 현재와 최대 체력 표시
		shared_ptr<CTextComponent> spCurHPTextComponent;
		shared_ptr<CTextComponent> spMaxHPTextComponent;
	};

	struct FHUDTutorialInfo
	{
		shared_ptr<CPanel> spChildTutorialPanel;
		// 퀘스트 텍스트 0, 1, 2
		shared_ptr<CTextComponent> spTutorialTextComponent0;
		shared_ptr<CTextComponent> spTutorialTextComponent1;
		shared_ptr<CTextComponent> spTutorialTextComponent2;
		// 퀘스트 체크박스 0, 1, 2
		shared_ptr<CImageComponent> spTutorialCheckBoxComponent0;
		shared_ptr<CImageComponent> spTutorialCheckBoxComponent1;
		shared_ptr<CImageComponent> spTutorialCheckBoxComponent2;
	};

	struct FHUDWeaponInfo
	{
		shared_ptr<CPanel> spChildWeaponPanel;
		// 팰 스피어 개수
		shared_ptr<CTextComponent> spPalSphereNumComponent;
		// 무기 이름
		shared_ptr<CTextComponent> spWeaponNameComponent;
		// 현재 가지고 있는 개수 (화살이나 총알)
		shared_ptr<CTextComponent> spWeaponCurNumComponent;
		// 한 발당 나가는 개수
		shared_ptr<CTextComponent> spWeaponOnceNumComponent;
		// 무기 이미지
		shared_ptr<CImageComponent> spWeaponImageComponent;
	};

	struct FHUDDirectionInfo
	{
		shared_ptr<CPanel> spChildDirectionPanel;
		// 방향 나타내는 이미지
		shared_ptr<CImageComponent> spDirectionImageComponent;
	};

	struct FHUDPalInfo
	{
		shared_ptr<CPanel> spChildPalPanel;
		// 현재 가지고 있는 팰을 보여줍니다.
		shared_ptr<CAnimationImageComponent> spPalSlotComponent0;
		shared_ptr<CAnimationImageComponent> spPalSlotComponent1;
		shared_ptr<CAnimationImageComponent> spPalSlotComponent2;

		int32 e_LeftSlot = 0;
		int32 e_CenterSlot = 1;
		int32 e_RightSlot = 2;
	};

	struct FBossHPInfo
	{
		shared_ptr<CPanel> spBossPanel;
		shared_ptr<CImageComponent> spNameImage;
		shared_ptr<CProgressBarComponent> spBossHPProgress;
		shared_ptr<CTextComponent> spCurHPText;
		shared_ptr<CTextComponent> SPMaxHPText;
	};
	
	class CUIManager final : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CUIManager)

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		HRESULT BeginPlay();

		int32 Tick(float _fDeltaSeconds);

		void Release();
		
	/********************
		Events
	********************/
	public:
		void AddParentPanel(const std::wstring& _wstrParentName, shared_ptr<CPanel>& _spParentPanel);

	/******************
		Game Object
	******************/
	public:
		std::shared_ptr<CPanel> FindParentsPanel(const std::wstring& _wstrPanelName)
		{
			if (!m_mapParentPanels.contains(_wstrPanelName)) { return nullptr; }

			return m_mapParentPanels[_wstrPanelName];
		}

		std::shared_ptr<CPanel> FindChildPanel(const std::wstring& _wstrParentPanelName, const std::wstring& _wstrChildPanelName);

	/********************
		Methods
	********************/
	public:
		// 직교 UI의 마우스 이벤트입니다.
		void UIMouseDown();

		void UIMousePress();

		void UIMouseUp();

		void UIMouseHover();

		// 원모양 패널을 위한 Hover입니다.
		void CircleUIMouseHover();
		// 원모양 패널을 위한 Click입니다.
		void CircleUIMouseClick();

		// 툴에서 저장한 패널을 불러와 맵에 저장합니다.
		void LoadPanel(const std::wstring& _wstrLoadFileName);

		shared_ptr<CPanel> LoadPanel(const std::wstring& _wstrLoadFileName, const std::wstring& _wstrMapKeyName);

		shared_ptr<CDamageFontObject> LoadDamageFont(const std::wstring& _wstrLoadFileName, const std::wstring& _wstrMapKeyName);

		bool MinusHP(shared_ptr<CProgressBarComponent> _spHPComponent, float _fPreHP, float _fCurHP, float _fMaxHP, float* _fHPLerpTime, float fDeltaSeconds);

		bool PlusHP(shared_ptr<CProgressBarComponent> _spHPComponent, shared_ptr<CTextComponent> _spCurHPTextComponent, float _fPreHP, float _fCurHP, float _fMaxHP, float* _fHPLerpTime, float fDeltaSeconds);

		bool MinusStamina(shared_ptr<CProgressBarComponent> _spStaminaComponent, float _fPreStamina, float _fCurStamina, float _fMaxStamina, float* _fStaminaLerpTime, float _fDeltaSeconds);

		bool PlusStamina(shared_ptr<CProgressBarComponent> _spStaminaComponent, float _fPreStamina, float _fCurStamina, float _fMaxStamina, float* _fStaminaLerpTime, float _fDeltaSeconds);

		bool MinusShield(shared_ptr<CProgressBarComponent> _spShildComponent, int32 _iPreCount, int32 _iCurCount, float* _fShildLerpTime, float _fDeltaSeconds);

		bool MinusBossHP(shared_ptr<CProgressBarComponent> _spHPComponent, shared_ptr<CTextComponent> _spCurHPTextComponent, float _fPreHP, float _fCurHP, float _fMaxHP, float* _fHPLerpTime, float fDeltaSeconds);

		void ResetTopPanel();

		void ContainerUIMouseLeftDown();

		void ContainerUIMouseRightDown();

		void ContainerUIMouseHover();
		
		shared_ptr<CPanel> GetUsingProductionArchitectureUI();

		void ReturnProductionArchitectureUI(const std::shared_ptr<CPanel> _spPanel);

		shared_ptr<CPanel> GetUsingWorkUI();

		void ReturnWorkUI(const std::shared_ptr<CPanel> _spPanel);

		shared_ptr<CPanel> GetUsingCaptureUI();

		void ReturnCaptureUI(const std::shared_ptr<CPanel> _spPanel);

		shared_ptr<CPanel> GetUsingPalHPUI();

		void ReturnPalHPUI(const std::shared_ptr<CPanel> _spPanel);

		shared_ptr<CPanel> GetUsingCaptureSuccessUI();

		void ReturnCaptureSuccessUI(const std::shared_ptr<CPanel> _spPanel);

		shared_ptr<CPanel> GetUsingPalWorkUI();

		void ReturnPalWorkUI(const std::shared_ptr<CPanel> _spPanel);

		shared_ptr<CDamageFontObject> GetUsingDamageFontUI();

		void ReturnDamageFont(const std::shared_ptr<CDamageFontObject> _spDamageFont);

		std::wstring FindPalIconTextureName(EMonsterType _eMonsterType);

		std::wstring FindPalName(EMonsterType _eMonsterType);

		std::wstring FindPalElementName(EMonsterElementType _eMonsterElementType);

		std::wstring FindPalSkillElementName(EMonsterElementType _eMonsterElementType);

		std::wstring FindItemUITextureName(EItemType _eItemType);

		std::wstring FindPlayerWeaponTextureName(EWeaponType _eWeaponType);

		std::vector<pair<EItemType, int32>> FindItemDataSheet(EItemType _eItemType);

		std::wstring FindItemName(EItemType _eItemType);

		std::wstring FindItemExplanation(EItemType _eItemType);

		std::wstring FindHUDWeaponTextureName(EItemType _eItemType);

		std::wstring FindPalWorkTextureName(EToDoType _eTodoType);

		Vector4 ChangeCirCleProgressBarColor(float _fRatio);

		void SetActiveInteractionPanel(bool _bActive);

		void SetInteractionPivotPoint(Vector3 _vPosition);

	public:
		void InitHUDInfo();

		void InitBossInfo();

		void UpdateTimeProgress();

		void SetHUDHPInfoPanel(float _fmaxHp, float _fCurHp);

		void SetActiveShield(bool _bActive);

		void SetBossHP(float _fMaxHP, float _fCurHP, EMonsterType _eMonsterType);

	public:
		void ResetClickImageComponent() { m_wpClickImageComponent.reset(); }

		EItemType ChangeMineralType(EMineralType _eMineralType);
	/********************
		Getter/Setter
	********************/
	public:
		// 최상위 패널을 설정합니다.
		void SetTopPanel(shared_ptr<CPanel> _spPanel);
		// 최상위 패널의 포인터를 가져옵니다.
		const shared_ptr<CPanel>& GetTopPanel() { return m_spTopPanel; }

		const float GetCircleDegreeAngle() const { return m_fCircleDegreeAngle; }

		std::weak_ptr<CRectComponent> GetHoverImageComponent() { return m_wpHoverImageComponent; }

		std::weak_ptr<CRectComponent> GetFocusedControlComponent() { return m_wpFocusedControlComponent; }

		std::weak_ptr<CRectComponent> GetClickImageComponent() { return m_wpClickImageComponent; }

		std::weak_ptr<CRectComponent> GetRightClickFocusedControlComponent() { return m_wpRightClickFocusedControlComponent; }

		void SetHoverComponent(std::shared_ptr<CRectComponent> _spRectComponent) { m_wpHoverImageComponent = _spRectComponent; }

		void SetFocusedControlComponent(std::shared_ptr<CRectComponent> _spRectComponent) { m_wpFocusedControlComponent = _spRectComponent; }
		
		void SetRightClickFocusedControlComponent(std::shared_ptr<CRectComponent> _spRectComponent) { m_wpRightClickFocusedControlComponent = _spRectComponent; }

		void RecycleUIs(const std::wstring& _wstrLoadFileName, const std::wstring& _wstrMapKeyName, int32 _iIndex, std::list<std::shared_ptr<CPanel>>& _listUIs, int32 _ilistCount);

		void RecycleDamageFonts(int32 _iIndex);

		FHUDHPInfo GetHUDHPInfo() { return m_tHUDHPInfo; }

		FHUDWeaponInfo GetHUDWeaponInfo() { return m_tHUDWeaponInfo; }

		FHUDTutorialInfo GetHUDTutorialInfo() { return m_tHUDTutorialInfo; }
		
		FHUDPalInfo GetHUDPalInfo() { return m_tHUDPalInfo; }
		
		std::shared_ptr<FHUDPalInfo> GetHUDPalInfoPointer() { return m_spHUDPalInfo; }

		std::shared_ptr<FHUDTutorialInfo> GetQuestInfo() { return m_spQuestInfo; }

		std::shared_ptr<FHUDWeaponInfo> GetHUDWeaponInfoPointer() { return m_spHUDWeaponInfo; }

		FHUDDirectionInfo GetHUDDirectionInfo() { return m_tHUDDirectionInfo; }

		bool IsOnUIPanel() { return m_spTopPanel != nullptr; }

		const std::shared_ptr<CPanel>& GetHUDParentPanel() { return m_spParentsHUDPanel; }

		std::shared_ptr<CPanel>& GetInteractionPanel() { return m_spInteractionPanel; }

		std::shared_ptr<FBossHPInfo>& GetBossInfo() { return m_spBossHPInfo; }

		std::shared_ptr<CPanel>& GetLockOnPanel() { return m_spLockOnUI; }

		// 거점 팰 ui list를 가져옵니다.
		std::list<std::shared_ptr<CPanel>>& GetUsingListPalWorkUI() { return m_listUsingPalWorkUI; }

		std::shared_ptr<CPanel>& GetSlaughterPanel() { return m_spSlaughterUI; }

		std::shared_ptr<CPanel>& GetHungerPanel() { return m_spHungerUI; }

	/******************
		Data Members
	******************/
	private:
		// 부모 패널들만 들고 있게 합니다.
		std::map<std::wstring, std::shared_ptr<CPanel>> m_mapParentPanels;

		// 포커스컨트롤은 한가지만 클릭되게합니다.
		std::weak_ptr<CRectComponent> m_wpFocusedControlComponent;

		// 이미지를 호버했을 때 무엇인지 판단하기 위한 컴포넌트입니다.
		std::weak_ptr<CRectComponent> m_wpHoverImageComponent;

		// 이전 호버 컴포넌트를 저장합니다.
		std::weak_ptr<CRectComponent> m_wpPreHoverComponent;

		// 클릭한 이미지 컴포넌트를 저장합니다.
		std::weak_ptr<CRectComponent> m_wpClickImageComponent;

		// 최상위 패널을 저장합니다.
		std::shared_ptr<CPanel> m_spTopPanel;

		float m_fCircleDegreeAngle = -1.0f;
		
		/* 24-04-26 이동준 추가 */
		std::weak_ptr<CRectComponent> m_wpRightClickFocusedControlComponent;

		// 건축 제작 UI를 모아둡니다.
		std::list<std::shared_ptr<CPanel>> m_listProductionArchitectureUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingProductionArchitectureUI;

		// 아이템 제작 UI를 모아둡니다.
		std::list<std::shared_ptr<CPanel>> m_listWorkUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingWorkUI;

		// 팰스피어 UI를 모아둡니다.
		std::list<std::shared_ptr<CPanel>> m_listCaptureUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingCaptureUI;

		// 팰스피어 포획 성공 UI를 모아둡니다.
		std::list<std::shared_ptr<CPanel>> m_listCaptureSuccessUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingCaptureSuccessUI;

		// 팰 체력 UI를 모아둡니다.
		std::list<std::shared_ptr<CPanel>> m_listPalHPUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingPalHPUI;

		// 거점 팰 일 UI를 모아둡니다.
		std::list<std::shared_ptr<CPanel>> m_listPalWorkUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingPalWorkUI;

		// 데미지 폰트 UI를 모아둡니다.
		std::list<std::shared_ptr<CDamageFontObject>> m_listDamageUI;
		std::list<std::shared_ptr<CDamageFontObject>> m_listUsingDamageUI;

		// HUD 패널의 부모를 들고 있게합니다.
		shared_ptr<CPanel>	m_spParentsHUDPanel = nullptr;

		// HUD 패널의 자식 패널입니다.
		FHUDHPInfo			m_tHUDHPInfo;
		FHUDWeaponInfo		m_tHUDWeaponInfo;
		FHUDTutorialInfo	m_tHUDTutorialInfo;
		FHUDPalInfo			m_tHUDPalInfo;
		FHUDDirectionInfo	m_tHUDDirectionInfo;

		// 스테미나 패널을 저장합니다.
		std::shared_ptr<CPanel> m_spStaminaPanel = nullptr;

		// 보스 패널의 정보를 들고 있게합니다.
		shared_ptr<FBossHPInfo> m_spBossHPInfo;

		// F키 상호작용 패널을 저장합니다.
		std::shared_ptr<CPanel> m_spInteractionPanel = nullptr;

		std::shared_ptr<FHUDPalInfo> m_spHUDPalInfo = nullptr;
		std::shared_ptr<FHUDTutorialInfo> m_spQuestInfo = nullptr;
		std::shared_ptr<FHUDWeaponInfo> m_spHUDWeaponInfo = nullptr;

		std::shared_ptr<CPanel> m_spLockOnUI = nullptr;

		std::shared_ptr<CPanel> m_spSlaughterUI = nullptr;

		std::shared_ptr<CPanel> m_spHungerUI = nullptr;

	private:
		std::weak_ptr<CWorld> m_wpWorld;

	private:
		int32 m_iWorkArchitectureCount = 0;
		int32 m_iWorkCount = 0;
		int32 m_iCaptureCount = 0;
		int32 m_iCaptureSuccessCount = 0;
		int32 m_iPalHPCount = 0;
		int32 m_iPalWorkCount = 0;
		int32 m_iDamageCount = 0;

	private:
		std::weak_ptr<CSoundManager> m_wpSoundManager;
	};
}
