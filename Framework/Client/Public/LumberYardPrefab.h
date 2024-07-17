#pragma once
#include "ArchitecturePrefab.h"

namespace Client
{
	struct FLumberYardPrefabDesc : public FArchitecturePrefabDesc
	{

	};

	class CLumberYardPrefab : public CArchitecturePrefab
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CLumberYardPrefab() = default;

		virtual ~CLumberYardPrefab() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CLumberYardPrefab> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	private:
		virtual HRESULT InitializeMeshComponent() override;

		virtual HRESULT InitializeColliderComponent() override;

		HRESULT InitializeTree();

		/********************
			Methods
		********************/
	public:


		/********************
			Getter/Setter
		********************/
	public:


		/********************
			Data Members
		********************/
	protected:

	};
}