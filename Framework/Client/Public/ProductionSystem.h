#pragma once

namespace Engine
{
	class CSoundManager;
	class CPanel;
	class CImageComponent;
	class CTextComponent;
	class CButtonComponent;
}

namespace Client
{
	class CUIManager;
	class CPlayer;
	class CArchitecture;
	class CInventorySystem;

	struct FSlotInfo
	{
		std::shared_ptr<CImageComponent> spSlot;
		std::shared_ptr<CImageComponent> spBaseSlot;
		EItemType eItemType;
		vector<pair<EItemType, int32>> vecDataSheet;
	};

	struct FChildItemProductionSystemInfo
	{
		std::shared_ptr<CPanel> spChildItemPanel = nullptr;
	};

	struct FChildProductionSystemInfo
	{
		std::shared_ptr<CPanel> spChildProductionPanel = nullptr;

		std::shared_ptr<CImageComponent> spSelectItemImage = nullptr;
		std::shared_ptr<CTextComponent> spItemNameText = nullptr;

		std::shared_ptr<CButtonComponent> spDownButton = nullptr;
		std::shared_ptr<CButtonComponent> spUpButton = nullptr;
		std::shared_ptr<CButtonComponent> spProductionStartButton = nullptr;
		std::shared_ptr<CTextComponent> spProductionItemNumText = nullptr;
	};

	struct FMineralBoxInfo
	{
		// 박스 한 칸을 구성하는 요소들입니다.
		shared_ptr<CImageComponent> spBackGroundImage = nullptr;
		shared_ptr<CImageComponent> spLayerImage = nullptr;
		shared_ptr<CImageComponent> spItemImage = nullptr;
		shared_ptr<CTextComponent> spItemNameText = nullptr;
		shared_ptr<CTextComponent> spItemCurNumText = nullptr;
		shared_ptr<CTextComponent> spItemMaxNumText = nullptr;

		// 재료 개수를 채우지 못했을 때 바꿔줄 색깔을 저장합니다.
		// 원래 있던 색으로 나오게 합니다.
		Vector4 vBackGroundSuccessColor;
		Vector4 vItemCurCountSuccessColor;
		// 빨간색으로 채워줍니다.
		Vector4 vFailColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	};

	/********************
		Descriptor
	********************/
	struct FProductionSystemDesc final
	{
	};

	class CProductionSystem final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CProductionSystem() = default;

		virtual ~CProductionSystem() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CProductionSystem> Create(const std::shared_ptr<FProductionSystemDesc>& _spProductionSystemDesc);

		HRESULT Initialize(const std::shared_ptr<FProductionSystemDesc>& _spProductionSystemDesc);

		HRESULT BeginPlay();

		int32 PreTick(float _fDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);
		
		int32 PostTick(float _fDeltaSeconds);

		HRESULT EndPlay();

		void Release();

	/********************
		Getter/Setter
	********************/
	public:
		void SetProductionPanel(std::shared_ptr<CPanel> _spProductionPanel) { m_spProductionWorkBenchUI = _spProductionPanel; }

	/********************
		Methods
	********************/
	public:
		// 로드한 패널의 정보를 변경하기 위해 포인터에 넣어주는 작업을 합니다.
		void AddInfo();
		void AddProductionComponent();
		void AddChildProductionInfo();
		void AddChildItemProductionInfo();
		
		void AddSlot();

		void AddMineralBox();

		// 실제로 작업대 아이템칸에 정보를 넣어줍니다.
		void AddWorkBenchSlot();
		// 실제로 화로 아이템칸에 정보를 넣어줍니다.
		void AddStoveSlot();
		// 실제로 요리 아이템칸에 정보를 넣어줍니다.
		void AddFoodSlot();
		// 실제로 장비 아이템칸에 정보를 넣어줍니다.
		void AddEquipmentSlot();

		// 마우스를 호버했을 때 이미지를 나오게 합니다.
		void ActiveHoverImage();

		void SetActiveMineralBox(bool _isActive, FMineralBoxInfo _tMineralBox);

		// 버튼 클릭 이벤트를 만듭니다.
		void DownButtonEvent();
		void UpButtonEvent();
		void ProductionStartEvent();
		void CloseEvent();

		void CheckButtonEvent();

		// 슬롯을 끄고 켜줍니다.
		void ActiveSlot(std::vector<FSlotInfo> _tSlotInfo[4], int32 _iNum);

		int32 CheckPlayerLevel(EProductionSystemType _eProductionSystemType);

		void OnWorkPanel(EArchitectureType _eArchitectureType, std::shared_ptr<CArchitecture> _spArchitecture);

	/********************
		Data Members
	********************/
	private:
		// workbench ui용 패널입니다.
		std::shared_ptr<CPanel> m_spProductionWorkBenchUI = nullptr;
		// 재료칸을 담아둡니다.
		std::vector<FMineralBoxInfo> m_vecMineralBoxs;
		// 작업대 슬롯칸의 정보를 담아둡니다.
		std::vector<FSlotInfo> m_vecWorkBenchSlotInfos[4];
		// 화로 슬롯칸의 정보를 담아둡니다.
		std::vector<FSlotInfo> m_vecStoveSlotInfos[4];
		// 요리 슬롯칸의 정보를 담아둡니다.
		std::vector<FSlotInfo> m_vecFoodSlotInfos[4];
		// 장비 슬롯칸의 정보를 담아둡니다.
		std::vector<FSlotInfo> m_vecEquipmentSlotInfos[4];
		// 마우스에 닿았을 떄 나오는 컴포넌트를 담아둡니다. 
		std::shared_ptr<CImageComponent> m_spHover;
		// 클릭한 컴포넌트에 나오는 컴포넌트를 담아둡니다..
		std::shared_ptr<CImageComponent> m_spSelect;
		// 패널 끄기용 버튼 컴포넌트를 담아둡니다.
		std::shared_ptr<CButtonComponent> m_spCloseButton;
		// 부모 패널의 레이어 이미지 컴포넌트들 담아둡니다.
		std::shared_ptr<CImageComponent> m_spLayerImage;
		
		FChildProductionSystemInfo m_tChildProductionSystemInfo;
		FChildItemProductionSystemInfo m_tChildItemProductionSystemInfo;

		std::vector<FSlotInfo> m_vecCurSlotInfos[4];

	private:
		// 생산 개수를 저장합니다.
		int32 m_iProductionNum = 0;
		// 생산할 아이템타입을 저장합니다.
		EItemType m_eItemType = EItemType::ENUM_END;
		// 현재 선택된 아이템 정보를 저장합니다.
		FSlotInfo m_tCurSlotInfo;

		bool m_bActive = false;

		// 현재 생산 인덱스가 무엇인지 저장합니다.
		EProductionSystemType m_eCurProductionSystemType = EProductionSystemType::WORKBENCH;
		EProductionSystemType m_ePreProductionSystemType = EProductionSystemType::ENUM_END;

		std::wstring m_wstrWorkBenchLayerImage = L"T_WorkBench_LayerImage";
		std::wstring m_wstrStoveLayerImage = L"T_Stove_LayerImage";
		std::wstring m_wstrFoodLayerImage = L"T_Food_LayerImage";
		std::wstring m_wstrEquipmentLayerImage = L"T_Equipment_LayerImage";

		std::wstring m_wstritemSuccessImage = L"T_WorkBench_Item_BlankImage";
		std::wstring m_wstritemFailImage = L"T_WorkBench_Item_FailStateImage";
	private:
		shared_ptr<CUIManager> m_spUIManager = nullptr;
		shared_ptr<CInventorySystem> m_spInventorySystem = nullptr;
		shared_ptr<CArchitecture> m_spCurArchitecture = nullptr;
		weak_ptr<CPlayer> m_wpPlayer;
		weak_ptr<CSoundManager> m_wpSoundManager;
	private:
		int32 m_iExnum = 1;
	};
}



