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
		// �ڽ� �� ĭ�� �����ϴ� ��ҵ��Դϴ�.
		shared_ptr<CImageComponent> spBackGroundImage = nullptr;
		shared_ptr<CImageComponent> spLayerImage = nullptr;
		shared_ptr<CImageComponent> spItemImage = nullptr;
		shared_ptr<CTextComponent> spItemNameText = nullptr;
		shared_ptr<CTextComponent> spItemCurNumText = nullptr;
		shared_ptr<CTextComponent> spItemMaxNumText = nullptr;

		// ��� ������ ä���� ������ �� �ٲ��� ������ �����մϴ�.
		// ���� �ִ� ������ ������ �մϴ�.
		Vector4 vBackGroundSuccessColor;
		Vector4 vItemCurCountSuccessColor;
		// ���������� ä���ݴϴ�.
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
		// �ε��� �г��� ������ �����ϱ� ���� �����Ϳ� �־��ִ� �۾��� �մϴ�.
		void AddInfo();
		void AddProductionComponent();
		void AddChildProductionInfo();
		void AddChildItemProductionInfo();
		
		void AddSlot();

		void AddMineralBox();

		// ������ �۾��� ������ĭ�� ������ �־��ݴϴ�.
		void AddWorkBenchSlot();
		// ������ ȭ�� ������ĭ�� ������ �־��ݴϴ�.
		void AddStoveSlot();
		// ������ �丮 ������ĭ�� ������ �־��ݴϴ�.
		void AddFoodSlot();
		// ������ ��� ������ĭ�� ������ �־��ݴϴ�.
		void AddEquipmentSlot();

		// ���콺�� ȣ������ �� �̹����� ������ �մϴ�.
		void ActiveHoverImage();

		void SetActiveMineralBox(bool _isActive, FMineralBoxInfo _tMineralBox);

		// ��ư Ŭ�� �̺�Ʈ�� ����ϴ�.
		void DownButtonEvent();
		void UpButtonEvent();
		void ProductionStartEvent();
		void CloseEvent();

		void CheckButtonEvent();

		// ������ ���� ���ݴϴ�.
		void ActiveSlot(std::vector<FSlotInfo> _tSlotInfo[4], int32 _iNum);

		int32 CheckPlayerLevel(EProductionSystemType _eProductionSystemType);

		void OnWorkPanel(EArchitectureType _eArchitectureType, std::shared_ptr<CArchitecture> _spArchitecture);

	/********************
		Data Members
	********************/
	private:
		// workbench ui�� �г��Դϴ�.
		std::shared_ptr<CPanel> m_spProductionWorkBenchUI = nullptr;
		// ���ĭ�� ��ƵӴϴ�.
		std::vector<FMineralBoxInfo> m_vecMineralBoxs;
		// �۾��� ����ĭ�� ������ ��ƵӴϴ�.
		std::vector<FSlotInfo> m_vecWorkBenchSlotInfos[4];
		// ȭ�� ����ĭ�� ������ ��ƵӴϴ�.
		std::vector<FSlotInfo> m_vecStoveSlotInfos[4];
		// �丮 ����ĭ�� ������ ��ƵӴϴ�.
		std::vector<FSlotInfo> m_vecFoodSlotInfos[4];
		// ��� ����ĭ�� ������ ��ƵӴϴ�.
		std::vector<FSlotInfo> m_vecEquipmentSlotInfos[4];
		// ���콺�� ����� �� ������ ������Ʈ�� ��ƵӴϴ�. 
		std::shared_ptr<CImageComponent> m_spHover;
		// Ŭ���� ������Ʈ�� ������ ������Ʈ�� ��ƵӴϴ�..
		std::shared_ptr<CImageComponent> m_spSelect;
		// �г� ����� ��ư ������Ʈ�� ��ƵӴϴ�.
		std::shared_ptr<CButtonComponent> m_spCloseButton;
		// �θ� �г��� ���̾� �̹��� ������Ʈ�� ��ƵӴϴ�.
		std::shared_ptr<CImageComponent> m_spLayerImage;
		
		FChildProductionSystemInfo m_tChildProductionSystemInfo;
		FChildItemProductionSystemInfo m_tChildItemProductionSystemInfo;

		std::vector<FSlotInfo> m_vecCurSlotInfos[4];

	private:
		// ���� ������ �����մϴ�.
		int32 m_iProductionNum = 0;
		// ������ ������Ÿ���� �����մϴ�.
		EItemType m_eItemType = EItemType::ENUM_END;
		// ���� ���õ� ������ ������ �����մϴ�.
		FSlotInfo m_tCurSlotInfo;

		bool m_bActive = false;

		// ���� ���� �ε����� �������� �����մϴ�.
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



