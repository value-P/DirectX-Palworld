#pragma once
#include "Architecture.h"

namespace Engine
{
	class CPanel;
}

namespace Client
{
	struct FWheatFarmDesc : public FArchitectureDesc
	{
	};

	class CWheatFarm final : public CArchitecture
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CWheatFarm() = default;

		virtual ~CWheatFarm() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CWheatFarm> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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
	};
}

