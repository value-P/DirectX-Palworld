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
		// �ڽ� �� ĭ�� �����ϴ� ��ҵ��Դϴ�.
		shared_ptr<CImageComponent> spBackGroundImageComponent = nullptr;
		shared_ptr<CImageComponent> spLayerImageComponent = nullptr;
		shared_ptr<CImageComponent> spItemImageComponent = nullptr;
		shared_ptr<CTextComponent> spItemTitleTextComponent = nullptr;
		shared_ptr<CTextComponent> spItemCurCountTextComponent = nullptr;
		shared_ptr<CTextComponent> spItemMaxCountTextComponent = nullptr;
		shared_ptr<CTextComponent> spSlashTextComponent = nullptr;

		// ��� ������ ä���� ������ �� �ٲ��� ������ �����մϴ�.
		// ���� �ִ� ������ ������ �մϴ�.
		Vector4 vBackGroundSuccessColor;
		Vector4 vItemCurCountSuccessColor;
		// ���������� ä���ݴϴ�.
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
		// ���Ϳ� �����͸� ä���ݴϴ�.
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
		// �� �гο��� ���콺 ��ġ�� ���� ���õ� �ε����� ������ ������Ʈ�Դϴ�.
		std::shared_ptr<CRectComponent> m_spHoverImageComponent = nullptr;
		// ���� ���õ� ����ý��� Ÿ���� �˷��� ������Ʈ�Դϴ�.
		std::shared_ptr<CRectComponent> m_spTabCurImageComponent = nullptr;
		// �� �г� �߾ӿ� ��ġ�� �̹��� ������Ʈ�� �����մϴ�.
		std::shared_ptr<CImageComponent> m_spCenterImageComponent = nullptr;
		// �� �г� �߾ӿ� ��ġ�� �ؽ�Ʈ ������Ʈ�� �����մϴ�.
		std::shared_ptr<CTextComponent> m_spCenterTextComponent = nullptr;
		// �� �г� �߾ӿ� ��ġ�� ���� ���� �̹��� ������Ʈ�� �����մϴ�.
		std::shared_ptr<CImageComponent> m_spCenterFailImageComponent = nullptr;

		// �ڽ� �г��� �����͸� �����մϴ�.
		std::shared_ptr<CPanel> m_spChildPanel = nullptr;
		// �ڽ� �г��� Ÿ��Ʋ �ؽ�Ʈ ������Ʈ�� �����մϴ�.
		std::shared_ptr<CTextComponent> m_spChildTitleTextComponent = nullptr;
		// �ڽ� �г��� ���� �ؽ�Ʈ ������Ʈ�� �����մϴ�.
		std::shared_ptr<CTextComponent> m_spChildDescriptionTextComponent = nullptr;

		// �ڽ� �г��� ������ ĭ���� ��Ƴ��� �����Դϴ�.
		std::vector<FInfoBox> m_vecItemBoxIndex;

		// ���� �ý��� ���� ��ġ�� �������� �����Դϴ�.
		std::vector<int32> m_vecTabPosition;
		// ���� �ý��� ���� �ε����� �����մϴ�.
		int32 m_iTapIndex = 0;
		// ����� �����Դϴ�
		std::vector<pair<int32, FCircleUIInfo>> m_vecWoodBulidIndex;
		// ������� �����Դϴ�.
		std::vector<pair<int32, FCircleUIInfo>> m_vecInfraIndex;
		// ����� �����Դϴ�.
		std::vector<pair<int32, FCircleUIInfo>> m_vecProductionIndex;
		// �ķ�ǰ�� �����Դϴ�.
		std::vector<pair<int32, FCircleUIInfo>> m_vecGroceriesIndex;
		// ���� Ȱ��ȭ �� Ÿ���� �����մϴ�.
		EArchitectureSystemType m_eArchitectureSystemType = EArchitectureSystemType::ARCHITECTURE;
		// ���� ���� �� ������ ����� �ε����� �����մϴ�.
		int32 m_iCurIndex = -1;
		int32 m_iPreIndex = 0;
		// �� ���๰�� �� �����Դϴ�.
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
