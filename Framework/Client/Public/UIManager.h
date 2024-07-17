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

	// HUD�� �����ϱ� ���� ����ü�Դϴ�.
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
		// ���� �ý��� ǥ��
		shared_ptr<CImageComponent> spTimezoneImageComponent;
		shared_ptr<CProgressBarComponent> spTimeCircleProgressComponent;
		// �÷��̾� ü���� ����� �ִ� ü�� ǥ��
		shared_ptr<CTextComponent> spCurHPTextComponent;
		shared_ptr<CTextComponent> spMaxHPTextComponent;
	};

	struct FHUDTutorialInfo
	{
		shared_ptr<CPanel> spChildTutorialPanel;
		// ����Ʈ �ؽ�Ʈ 0, 1, 2
		shared_ptr<CTextComponent> spTutorialTextComponent0;
		shared_ptr<CTextComponent> spTutorialTextComponent1;
		shared_ptr<CTextComponent> spTutorialTextComponent2;
		// ����Ʈ üũ�ڽ� 0, 1, 2
		shared_ptr<CImageComponent> spTutorialCheckBoxComponent0;
		shared_ptr<CImageComponent> spTutorialCheckBoxComponent1;
		shared_ptr<CImageComponent> spTutorialCheckBoxComponent2;
	};

	struct FHUDWeaponInfo
	{
		shared_ptr<CPanel> spChildWeaponPanel;
		// �� ���Ǿ� ����
		shared_ptr<CTextComponent> spPalSphereNumComponent;
		// ���� �̸�
		shared_ptr<CTextComponent> spWeaponNameComponent;
		// ���� ������ �ִ� ���� (ȭ���̳� �Ѿ�)
		shared_ptr<CTextComponent> spWeaponCurNumComponent;
		// �� �ߴ� ������ ����
		shared_ptr<CTextComponent> spWeaponOnceNumComponent;
		// ���� �̹���
		shared_ptr<CImageComponent> spWeaponImageComponent;
	};

	struct FHUDDirectionInfo
	{
		shared_ptr<CPanel> spChildDirectionPanel;
		// ���� ��Ÿ���� �̹���
		shared_ptr<CImageComponent> spDirectionImageComponent;
	};

	struct FHUDPalInfo
	{
		shared_ptr<CPanel> spChildPalPanel;
		// ���� ������ �ִ� ���� �����ݴϴ�.
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
		// ���� UI�� ���콺 �̺�Ʈ�Դϴ�.
		void UIMouseDown();

		void UIMousePress();

		void UIMouseUp();

		void UIMouseHover();

		// ����� �г��� ���� Hover�Դϴ�.
		void CircleUIMouseHover();
		// ����� �г��� ���� Click�Դϴ�.
		void CircleUIMouseClick();

		// ������ ������ �г��� �ҷ��� �ʿ� �����մϴ�.
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
		// �ֻ��� �г��� �����մϴ�.
		void SetTopPanel(shared_ptr<CPanel> _spPanel);
		// �ֻ��� �г��� �����͸� �����ɴϴ�.
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

		// ���� �� ui list�� �����ɴϴ�.
		std::list<std::shared_ptr<CPanel>>& GetUsingListPalWorkUI() { return m_listUsingPalWorkUI; }

		std::shared_ptr<CPanel>& GetSlaughterPanel() { return m_spSlaughterUI; }

		std::shared_ptr<CPanel>& GetHungerPanel() { return m_spHungerUI; }

	/******************
		Data Members
	******************/
	private:
		// �θ� �гε鸸 ��� �ְ� �մϴ�.
		std::map<std::wstring, std::shared_ptr<CPanel>> m_mapParentPanels;

		// ��Ŀ����Ʈ���� �Ѱ����� Ŭ���ǰ��մϴ�.
		std::weak_ptr<CRectComponent> m_wpFocusedControlComponent;

		// �̹����� ȣ������ �� �������� �Ǵ��ϱ� ���� ������Ʈ�Դϴ�.
		std::weak_ptr<CRectComponent> m_wpHoverImageComponent;

		// ���� ȣ�� ������Ʈ�� �����մϴ�.
		std::weak_ptr<CRectComponent> m_wpPreHoverComponent;

		// Ŭ���� �̹��� ������Ʈ�� �����մϴ�.
		std::weak_ptr<CRectComponent> m_wpClickImageComponent;

		// �ֻ��� �г��� �����մϴ�.
		std::shared_ptr<CPanel> m_spTopPanel;

		float m_fCircleDegreeAngle = -1.0f;
		
		/* 24-04-26 �̵��� �߰� */
		std::weak_ptr<CRectComponent> m_wpRightClickFocusedControlComponent;

		// ���� ���� UI�� ��ƵӴϴ�.
		std::list<std::shared_ptr<CPanel>> m_listProductionArchitectureUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingProductionArchitectureUI;

		// ������ ���� UI�� ��ƵӴϴ�.
		std::list<std::shared_ptr<CPanel>> m_listWorkUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingWorkUI;

		// �ӽ��Ǿ� UI�� ��ƵӴϴ�.
		std::list<std::shared_ptr<CPanel>> m_listCaptureUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingCaptureUI;

		// �ӽ��Ǿ� ��ȹ ���� UI�� ��ƵӴϴ�.
		std::list<std::shared_ptr<CPanel>> m_listCaptureSuccessUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingCaptureSuccessUI;

		// �� ü�� UI�� ��ƵӴϴ�.
		std::list<std::shared_ptr<CPanel>> m_listPalHPUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingPalHPUI;

		// ���� �� �� UI�� ��ƵӴϴ�.
		std::list<std::shared_ptr<CPanel>> m_listPalWorkUI;
		std::list<std::shared_ptr<CPanel>> m_listUsingPalWorkUI;

		// ������ ��Ʈ UI�� ��ƵӴϴ�.
		std::list<std::shared_ptr<CDamageFontObject>> m_listDamageUI;
		std::list<std::shared_ptr<CDamageFontObject>> m_listUsingDamageUI;

		// HUD �г��� �θ� ��� �ְ��մϴ�.
		shared_ptr<CPanel>	m_spParentsHUDPanel = nullptr;

		// HUD �г��� �ڽ� �г��Դϴ�.
		FHUDHPInfo			m_tHUDHPInfo;
		FHUDWeaponInfo		m_tHUDWeaponInfo;
		FHUDTutorialInfo	m_tHUDTutorialInfo;
		FHUDPalInfo			m_tHUDPalInfo;
		FHUDDirectionInfo	m_tHUDDirectionInfo;

		// ���׹̳� �г��� �����մϴ�.
		std::shared_ptr<CPanel> m_spStaminaPanel = nullptr;

		// ���� �г��� ������ ��� �ְ��մϴ�.
		shared_ptr<FBossHPInfo> m_spBossHPInfo;

		// FŰ ��ȣ�ۿ� �г��� �����մϴ�.
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
