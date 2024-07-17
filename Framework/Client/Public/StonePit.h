#pragma once
#include "Factory.h"

namespace Engine
{
	class CPanel;
}

namespace Client
{
	struct FStonePitDesc : public FFactoryDesc
	{
	};

	// √§ºÆ¿Â
	class CStonePit : public CFactory
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CStonePit() = default;

		virtual ~CStonePit() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CStonePit> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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

	public:
		void AddPermanentCommand();

		virtual bool BuildArchitecture(float _fDeltaSeconds) override;
		
	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CPanel> m_spSelectPanel = nullptr;
	};
}
