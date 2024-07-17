#pragma once
#include "Factory.h"

namespace Engine
{
	class CPanel;
}

namespace Client
{
	struct FWeaponWorkBenchDesc : public FFactoryDesc
	{
	};

	class CWeaponWorkBench final : public CFactory
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CWeaponWorkBench() = default;

		virtual ~CWeaponWorkBench() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CWeaponWorkBench> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	private:
		virtual HRESULT InitializeMeshComponent() override;

		virtual HRESULT InitializeColliderComponent() override;

		virtual void AddItemToFactory(EItemType _eItemType, int32 _iNumItems) override;

		virtual void RemoveItemFromFactory() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CPanel> m_spSelectPanel = nullptr;
	};
}
