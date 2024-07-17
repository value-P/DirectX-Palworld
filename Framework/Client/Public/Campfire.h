#pragma once
#include "Factory.h"

namespace Engine
{
	class CPanel;
}

namespace Client
{
	struct FCampfireDesc : public FFactoryDesc
	{

	};

	class CCampfire : public CFactory
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CCampfire() = default;

		virtual ~CCampfire() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CCampfire> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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
	private:
		virtual HRESULT InitializeMeshComponent() override;

		virtual HRESULT InitializeColliderComponent() override;

		virtual void AddItemToFactory(EItemType _eItemType, int32 _iNumItems) override;

		virtual void RemoveItemFromFactory() override;

	/********************
		Data Members
	********************/
	private:
		// todo; ÆÐ³Î
	};
}
