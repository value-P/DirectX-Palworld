#pragma once
#include "ArchitecturePrefab.h"

namespace Client
{
	struct FStonePitPrefabDesc : public FArchitecturePrefabDesc
	{

	};

	class CStonePitPrefab : public CArchitecturePrefab
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CStonePitPrefab() = default;

		virtual ~CStonePitPrefab() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CStonePitPrefab> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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