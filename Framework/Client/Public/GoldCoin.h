#pragma once
#include "EffectGroup.h"

namespace Engine
{
	class CMeshComponent;
	class CSphereComponent;
}

namespace Client
{
	class CInventorySystem;

	/********************
		Descriptor
	********************/
	struct FGoldCoinDesc : public FEffectGroupDesc
	{

	};

	class CGoldCoin : public CEffectGroup
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CGoldCoin() = default;

		virtual ~CGoldCoin() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CGoldCoin> Create(const std::shared_ptr<FGoldCoinDesc>& _spGoldCoinDesc);

		static std::shared_ptr<CEffectGroup> CreateFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT InitializeFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release();

	/********************
		Overrides
	********************/
	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual void OnTimerOver(const shared_ptr<CTimer>& _spTimer) override;

	/********************
		Methods
	********************/
	private: 
		void InitMeshComponent();

		void InitColliderComponent();

	/********************
		Getter/Setter
	********************/
	public:

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CInventorySystem> m_wpInventorySystem;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::shared_ptr<CSphereComponent> m_spSphereComponent = nullptr;
	};
}