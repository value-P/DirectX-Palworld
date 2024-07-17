#pragma once

namespace Engine
{
	class CSoundManager;
	class CPanel;
	class CRectComponent;
	class CImageComponent;
	class CTextComponent;
}

namespace Client
{
	class CInventorySystem;
	class CUIManager;

	struct FCircleUIInfo final
	{
		//EBuildType eBuildType = EBuildType::ENUM_END;
		EArchitectureType eArchitectureType = EArchitectureType::ENUM_END;
		float fMinAngle = 0.0f;
		float fMaxAngle = 0.0f;
		std::wstring wstrTextureName = L"";
		std::wstring wstrText = L"";
		std::wstring wstrDescriptionText = L"";
		shared_ptr<CImageComponent> spImageComponent = nullptr;
		shared_ptr<CTextComponent> spTextComponent = nullptr;
	};

	struct FInfoBox final
	{
		// 박스 한 칸을 구성하는 요소들입니다.
		shared_ptr<CImageComponent> spBackGroundImageComponent = nullptr;
		shared_ptr<CImageComponent> spLayerImageComponent = nullptr;
		shared_ptr<CImageComponent> spItemImageComponent = nullptr;
		shared_ptr<CTextComponent> spItemTitleTextComponent = nullptr;
		shared_ptr<CTextComponent> spItemCurCountTextComponent = nullptr;
		shared_ptr<CTextComponent> spItemMaxCountTextComponent = nullptr;
		shared_ptr<CTextComponent> spSlashTextComponent = nullptr;

		// 재료 개수를 채우지 못했을 때 바꿔줄 색깔을 저장합니다.
		// 원래 있던 색으로 나오게 합니다.
		Vector4 vBackGroundSuccessColor;
		Vector4 vItemCurCountSuccessColor;
		// 빨간색으로 채워줍니다.
		Vector4 vFailColor;
	};

	/********************
		Descriptor
	********************/
	struct FArchitectureSystemDesc final
	{
	};

	class CArchitectureSystem final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CArchitectureSystem() = default;

		virtual ~CArchitectureSystem() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CArchitectureSystem> Create(const std::shared_ptr<FArchitectureSystemDesc>& _spArchitectureSystemDesc);

		HRESULT Initialize(const std::shared_ptr<FArchitectureSystemDesc>& _spArchitectureSystemDesc);

		HRESULT BeginPlay();

		int32 PreTick(float _fDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);
		
		int32 PostTick(float _fDeltaSeconds);

		HRESULT EndPlay();

		void Release();
		
	/********************
		Getter/Setter
	********************/
		void SetArchitecturePanel(shared_ptr<CPanel> _spArchitecturePanel) { m_spArchitecturePanel = _spArchitecturePanel; }

		const std::shared_ptr<CPanel> GetArchitecturePanel() const { return m_spArchitecturePanel; }

		void SetHoverImageComponent(shared_ptr<CRectComponent> _spHoverImageComponent) { m_spHoverImageComponent = _spHoverImageComponent; }

		const std::shared_ptr<CRectComponent> GetHoverImageComponent() { return m_spHoverImageComponent; }

		void SetCenterImageComponent(std::shared_ptr<CImageComponent> _spImageComponent) { m_spCenterImageComponent = _spImageComponent; }

		void SetCenterTextComponent(std::shared_ptr<CTextComponent> _spTextComponent) { m_spCenterTextComponent = _spTextComponent; }

		std::vector<pair<int32, FCircleUIInfo>> GetVecWoodIndex() { return m_vecWoodBulidIndex; }

	/********************
		Methods
	********************/
		// 벡터에 데이터를 채워줍니다.
		void AddIndex();

		void AddInfraIndex();

		void AddProductionIndex();

		void AddGroceriesIndex();

		void AddChildItemBoxIndex();

		bool IsInAngle(float _fMinAngle, float _fMaxAngle, float _fDegreeAngle);

		void FindIsInIndex();

		void ActiveHoverImage(std::vector<pair<int32, FCircleUIInfo>> _vecCurIndex);

		void ChangeIndex(EArchitectureSystemType _eArchitectureSystemType);

		void IsActiveItemBox(bool _bActive, FInfoBox _tInfoBox);

		void FindItemBox(EArchitectureType _eArchitectureType);

		const pair<std::wstring, std::wstring> GetItemTypeImageName(EMineralType _emineralType);

		//EItemType ChangeMineralType(EMineralType _eMineralType);

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CPanel> m_spArchitecturePanel = nullptr;
		// 원 패널에서 마우스 위치에 따라 선택된 인덱스를 보여줄 컴포넌트입니다.
		std::shared_ptr<CRectComponent> m_spHoverImageComponent = nullptr;
		// 현재 선택된 건축시스템 타입을 알려줄 컴포넌트입니다.
		std::shared_ptr<CRectComponent> m_spTabCurImageComponent = nullptr;
		// 원 패널 중앙에 위치한 이미지 컴포넌트를 저장합니다.
		std::shared_ptr<CImageComponent> m_spCenterImageComponent = nullptr;
		// 원 패널 중앙에 위치한 텍스트 컴포넌트를 저장합니다.
		std::shared_ptr<CTextComponent> m_spCenterTextComponent = nullptr;
		// 원 패널 중앙에 위치한 소재 부족 이미지 컴포넌트를 저장합니다.
		std::shared_ptr<CImageComponent> m_spCenterFailImageComponent = nullptr;

		// 자식 패널의 포인터를 저장합니다.
		std::shared_ptr<CPanel> m_spChildPanel = nullptr;
		// 자식 패널의 타이틀 텍스트 컴포넌트를 저장합니다.
		std::shared_ptr<CTextComponent> m_spChildTitleTextComponent = nullptr;
		// 자식 패널의 설명 텍스트 컴포넌트를 저장합니다.
		std::shared_ptr<CTextComponent> m_spChildDescriptionTextComponent = nullptr;

		// 자식 패널의 아이템 칸들을 담아놓는 벡터입니다.
		std::vector<FInfoBox> m_vecItemBoxIndex;

		// 건축 시스템 탭의 위치를 저장해줄 벡터입니다.
		std::vector<int32> m_vecTabPosition;
		// 건축 시스템 탭의 인덱스를 저장합니다.
		int32 m_iTapIndex = 0;
		// 건축용 벡터입니다
		std::vector<pair<int32, FCircleUIInfo>> m_vecWoodBulidIndex;
		// 인프라용 벡터입니다.
		std::vector<pair<int32, FCircleUIInfo>> m_vecInfraIndex;
		// 생산용 벡터입니다.
		std::vector<pair<int32, FCircleUIInfo>> m_vecProductionIndex;
		// 식료품용 벡터입니다.
		std::vector<pair<int32, FCircleUIInfo>> m_vecGroceriesIndex;
		// 현재 활성화 된 타입을 저장합니다.
		EArchitectureSystemType m_eArchitectureSystemType = EArchitectureSystemType::ARCHITECTURE;
		// 현재 벡터 중 조건을 통과한 인덱스를 저장합니다.
		int32 m_iCurIndex = -1;
		int32 m_iPreIndex = 0;
		// 각 건축물의 총 갯수입니다.
		int32 m_iWoodBuildIndex = 8;
		int32 m_iInfraIndex = 5;
		int32 m_iProductionIndex = 8;
		int32 m_iGroceriesIndex = 6;
	private:
		shared_ptr<CInventorySystem> m_spInventorySystem = nullptr;

	private:
		weak_ptr<CUIManager> m_wpUIManager;

		weak_ptr<CSoundManager> m_wpSoundManager;
	};
}
