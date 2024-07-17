#pragma once
#include "ContainerSystem.h"

namespace Engine
{
	class CPanel;
	class CRectComponent; 
}

namespace Client
{
	class CBuildManager;
	class CMonster;

	/********************
		Descriptor
	********************/
	struct FPalBoxSystemDesc final : public FContainerSystemDesc
	{
	};

	class CPalBoxSystem : public CContainerSystem
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPalBoxSystem() = default;

		virtual ~CPalBoxSystem() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPalBoxSystem> Create(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FContainerSystemDesc>& _spContainerSystemDesc) override;

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
		int32 GetPalboxPalTotalNum(EMonsterType _eMonsterType);

		void AddPalboxPalCount(EMonsterType _eMonsterType);

		void SetPalboxLeftChildPanel(const std::shared_ptr<CPanel>& _spPalboxLeftChildPanel) { m_spPalboxLeftChildPanel = _spPalboxLeftChildPanel; }

		void SetPalboxRightChildPanel(const std::shared_ptr<CPanel>& _spPalboxRightChildPanel) { m_spPalboxRightChildPanel = _spPalboxRightChildPanel; }

		void SetPalboxRightNodataChildPanel(const std::shared_ptr<CPanel>& _spPalboxRightNodataChildPanel) { m_spPalboxRightNodataChildPanel = _spPalboxRightNodataChildPanel; }

		void SetPalboxMidChildPanel(const std::shared_ptr<CPanel>& _spPalboxMidChildPanel) { m_spPalboxMidChildPanel = _spPalboxMidChildPanel; }

		void SetPalboxBottomChildPanel(const std::shared_ptr<CPanel>& _spPalboxBottomChildPanel) { m_spPalboxBottomChildPanel = _spPalboxBottomChildPanel; }

		void SetPartnerPalInfo(int32 _iIndex, FPalBoxPalInfo _tPalboxPalInfo) { (*m_arrLeftPanelPals)[_iIndex].first = _tPalboxPalInfo; }

	public:
		//std::array<FPalBoxPalInfo, 5> GetPalBoxPalInfoFromLeftPanel() const { return m_arrLeftPanelPalInfos; };

		std::array<std::pair<FPalBoxPalInfo, shared_ptr<CMonster>>, 5>* GetPalBoxPalFromLeftPanel() const { return m_arrLeftPanelPals; };

		std::shared_ptr<CPanel> GetPalboxLeftPanel() const { return m_spPalboxLeftChildPanel; }

		FPalBoxPalInfo GeneratePalInfoStruct(const std::shared_ptr<CMonster>& _spMonster, std::pair<int32, int32> _iCurClickedIndex); // level에서의 삭제

	/********************
		Methods
	********************/
	public:
		// 팰 스피어로 잡은 보유팰입니다.
		void AddPalToPalBoxLeftPanel(FPalBoxPalInfo _tPartnerPalInfo);
	
		void AddPalToPalBoxMidPanel(const std::shared_ptr<CMonster>& _spMonster);

		void AddTestPalToPalBoxMidPanel(FPalBoxPalInfo _tPartnerPalInfo);

	public:
		void MovePalSlotUI();

		void ExchangeSlotsUI(std::shared_ptr<CPanel> _spPreClickedPanel, std::shared_ptr<CPanel> _spCurClickedPanel, const std::wstring& _wstrPreSlotExchangeTextureName, const std::wstring& _wstrCurSlotExchangeTextureName);

		void MovePalToEmptySlotFromLeftPanel();

		void MovePalToEmptySlotFromMidPanel();

		void MovePalToEmptySlotFromBottomPanel();

		void MovePalToOccupiedSlotFromLeftPanel();

		void MovePalToOccupiedSlotFromMidPanel();

		void MovePalToOccupiedSlotFromBottomPanel();
		
	public:
		virtual void ActiveSlotHover() override;

		void ActivePalInfoWindow();

		void ShowPartnerPalInfoOnLeftPanel();

		void HiddenPartnerPalInfoOnLeftPanel(int32 _iPreClickedSlotIndex);

		void UpdatePartnerPalInfoOnRightPanel();

		void PartnerPalInfoOnRightPanelPrefab(FPalBoxPalInfo _tPalBoxPalInfo);

	public:
		void InsertPalInfoAndPalIntoPartnerPalArr(FPalBoxPalInfo _tPalBoxPalInfo);

		void AddPalToBaseCamp(FPalBoxPalInfo _tBaseCampPalInfo);

	public:
		void SortPalSlots();

		void SortPalsInPalBox(ESortingType _eSortingType);

		void ExecuteSlavePal(std::shared_ptr<CMonster> _spMonster);

	/********************
		Data Members
	********************/
	private: // 컨테이너
		std::array<std::pair<FPalBoxPalInfo, std::shared_ptr<CMonster>>, 5>* m_arrLeftPanelPals = nullptr;

		std::array<std::array<FPalBoxPalInfo, 6>, 5> m_arrMidPanelPalInfos = {};

		std::array<std::array<shared_ptr<CMonster>, 5>, 2> m_arrBottomPanelPals = {};

		std::vector<std::weak_ptr<CRectComponent>> m_vecPageChangeHovers;// mid panel hover

		std::vector<std::weak_ptr<CRectComponent>> m_vecLeftPanelPalStatus;// left panel hover

		std::vector<std::weak_ptr<CRectComponent>> m_vecRightPanelPalStatus;// right panel hover

		std::unordered_map<EMonsterType, int32> m_umapPartnerPalInfos;

	private: // 스마트 포인터
		std::shared_ptr<CPanel> m_spPalboxLeftChildPanel	    = nullptr;

		std::shared_ptr<CPanel> m_spPalboxRightChildPanel		= nullptr;

		std::shared_ptr<CPanel> m_spPalboxRightNodataChildPanel = nullptr;

		std::shared_ptr<CPanel> m_spPalboxMidChildPanel			= nullptr;

		std::shared_ptr<CPanel> m_spPalboxBottomChildPanel		= nullptr;

		std::weak_ptr<CRectComponent> m_wpLeftHoverImage;	// left panel hover

		std::weak_ptr<CRectComponent> m_wpMidHoverImage;	// mid panel hover

		std::weak_ptr<CRectComponent> m_wpBottomHoverImage; // bottom panel hover

		std::weak_ptr<CRectComponent> m_wpSortHoverImage;	// sort panel hover

		std::weak_ptr<CBuildManager> m_wpBuildManager;
	};
}