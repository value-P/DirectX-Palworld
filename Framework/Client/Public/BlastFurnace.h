#pragma once
#include "Factory.h"

namespace Engine
{
	class CPanel;
}

namespace Client
{
	class CEffectGroup;

	struct FBlastFurnaceDesc : public FFactoryDesc
	{
	};

	class CBlastFurnace final : public CFactory
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CBlastFurnace() = default;

		virtual ~CBlastFurnace() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CBlastFurnace> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	public:
		virtual void ClearEffect() override;

	private:
		virtual HRESULT InitializeMeshComponent() override;

		virtual HRESULT InitializeColliderComponent() override;

		virtual void AddItemToFactory(EItemType _eItemType, int32 _iNumItems) override;

		virtual void RemoveItemFromFactory() override;

	/********************
		Data Members
	********************/
	protected:
		/* == F를 누르지 않았을 때 == */
		// 패널 : 버튼 선택
		// [F] : 자기
		std::shared_ptr<CPanel> m_spSelectPanel = nullptr;

	private:
		std::weak_ptr<CEffectGroup> m_wpFireEffect;

		bool m_bFireEffectOn = false;
	};
}
