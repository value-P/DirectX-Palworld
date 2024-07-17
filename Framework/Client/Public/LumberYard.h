#pragma once
#include "Factory.h"

namespace Engine
{
	class CPanel;
	class CMeshComponent;
}

namespace Client
{
	struct FLumberYardDesc : public FFactoryDesc
	{
	};

	// √§ºÆ¿Â
	class CLumberYard : public CFactory
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CLumberYard() = default;

		virtual ~CLumberYard() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CLumberYard> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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

	private:
		std::shared_ptr<CMeshComponent> m_spTreeMesh1 = nullptr;
		std::shared_ptr<CMeshComponent> m_spTreeMesh2 = nullptr;
		std::shared_ptr<CMeshComponent> m_spTreeMesh3 = nullptr;
	};
}
