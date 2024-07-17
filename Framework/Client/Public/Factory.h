#pragma once
#include "Architecture.h"

namespace Engine
{
	class CPanel;
	class CProgressBarComponent;
	class CTextComponent;
	class CImageComponent;
}

namespace Client
{
	class CInventorySystem;
	class CUIManager;
	class CEffectManager;

	struct FFactoryDesc : public FArchitectureDesc
	{
		EFactoryType m_eFactoryType = EFactoryType::ENUM_END;
	};

	class CFactory : public CArchitecture
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CFactory() = default;

		virtual ~CFactory() = default;
		
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
		
	/********************
		Getter/Setter
	********************/
	public:
		EFactoryType GetFactoryType() const { return m_eFactoryType; }

		bool GetCanInteract() const { return m_spCurProduceStatus != nullptr; }

		bool IsProduceCompleted() const;

		bool IsProducePartialyCompleted() const;

	/********************
		Methods
	********************/
	public:
		virtual void AddItemToFactory(EItemType _eItemType, int32 _iNumItems) override;

		virtual void RemoveItemFromFactory();

		virtual void TakeItemFromFactory();

		EProduceItemState ProduceItem(float _fDeltaSeconds);
		
		void SetWorkPanel(std::shared_ptr<CPanel> _spWorkPanel);

		std::shared_ptr<CPanel> GetWorkPanel() const { return m_spWorkPanel; }

		void PlayWorkingEffect(EFactoryType _eFactoryType, float _fDeltaSeconds);

	/********************
		Data Members
	********************/
	protected:
		EFactoryType m_eFactoryType = EFactoryType::ENUM_END;

		// ���� ���� ���� ������
		std::shared_ptr<FProduceStatus> m_spCurProduceStatus = nullptr;

	protected:
		std::shared_ptr<CUIManager> m_spUIManager = nullptr;

		// ���� ���� ���� ������ �г��� �����մϴ�.
		std::shared_ptr<CPanel> m_spWorkPanel = nullptr;
		
		// ���� ���α׷����� ������Ʈ�� �����մϴ�.
		std::shared_ptr<CProgressBarComponent> m_spCircleProgressBar = nullptr;
		
		// �ؽ�Ʈ ������Ʈ�� �����մϴ�. // ���� ���۵� ������ ������ ǥ���մϴ�.
		std::shared_ptr<CTextComponent> m_spCurNumText = nullptr;
		
		// ������ ���� �г��� �ڽ� �г��� �����մϴ�.
		std::shared_ptr<CPanel> m_spChildPalPanel = nullptr;
		
		// �ڽ� �г��� �� ������ �̹��� ������Ʈ�� �����մϴ�.
		std::shared_ptr<CImageComponent> m_spPalImage = nullptr;		

	protected:
		std::weak_ptr<CEffectManager> m_wpEffectManager;

		float m_fEffectTimeAcc = 0.f;
	};
}
