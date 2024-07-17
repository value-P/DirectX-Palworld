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

		// 현재 생산 중인 아이템
		std::shared_ptr<FProduceStatus> m_spCurProduceStatus = nullptr;

	protected:
		std::shared_ptr<CUIManager> m_spUIManager = nullptr;

		// 현재 생산 중인 아이템 패널을 저장합니다.
		std::shared_ptr<CPanel> m_spWorkPanel = nullptr;
		
		// 원형 프로그래스바 컴포넌트를 저장합니다.
		std::shared_ptr<CProgressBarComponent> m_spCircleProgressBar = nullptr;
		
		// 텍스트 컴포넌트를 저장합니다. // 현재 제작된 아이템 개수를 표시합니다.
		std::shared_ptr<CTextComponent> m_spCurNumText = nullptr;
		
		// 아이템 제작 패널의 자식 패널을 저장합니다.
		std::shared_ptr<CPanel> m_spChildPalPanel = nullptr;
		
		// 자식 패널의 팰 아이콘 이미지 컴포넌트를 저장합니다.
		std::shared_ptr<CImageComponent> m_spPalImage = nullptr;		

	protected:
		std::weak_ptr<CEffectManager> m_wpEffectManager;

		float m_fEffectTimeAcc = 0.f;
	};
}
